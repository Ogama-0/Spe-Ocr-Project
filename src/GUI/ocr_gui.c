#include "../ocr/predict.h"
#include "../ocr/train_ocr.h"
#include "../pretreatment/binarize.h"
#include "../pretreatment/blocks_treatment.h"
#include "../pretreatment/denoise.h"
#include "../pretreatment/manual_rotate.h"
#include "../solver/solver.h"
#include <glib.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* =========================
   Variables globales
   ========================= */
static GtkWidget *image_widget = NULL;
static GtkWidget *status_label = NULL;
static GtkWidget *angle_entry = NULL;
static GtkWidget *rotate_button = NULL;
static char *selected_image_path = NULL;
static GdkPixbuf *current_pixbuf = NULL;

/* =========================
   Utils GTK thread-safe
   ========================= */
static gboolean update_status_idle(gpointer data)
{
    const char *text = (const char *)data;
    gtk_label_set_text(GTK_LABEL(status_label), text);
    return G_SOURCE_REMOVE;
}

/* =========================
   Thread OCR training
   ========================= */
static gpointer train_ocr_thread(gpointer user_data)
{
    (void)user_data;

    g_idle_add(update_status_idle, "OCR training...");

    int ret = train_ocr();

    if (ret == 0)
        g_idle_add(update_status_idle, "OCR training finished");
    else
        g_idle_add(update_status_idle, "OCR training failed");

    return NULL;
}

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <stdlib.h>

#include <stdlib.h> // abs()

// Dessine un pixel si dans les bornes
static void set_pixel(GdkPixbuf *pixbuf, int x, int y, guchar r, guchar g,
                      guchar b, guchar a)
{
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    guchar *p = pixels + y * rowstride + x * n_channels;
    p[0] = r;
    p[1] = g;
    p[2] = b;
    if (n_channels == 4)
        p[3] = a;
}

void draw_line_thick(GdkPixbuf *pixbuf, int x0, int y0, int x1, int y1,
                     guchar r, guchar g, guchar b, guchar a)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (1)
    {
        // Épaisseur : carré 3x3 autour du point
        for (int ox = -1; ox <= 1; ox++)
        {
            for (int oy = -1; oy <= 1; oy++)
            {
                set_pixel(pixbuf, x0 + ox, y0 + oy, r, g, b, a);
            }
        }

        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
/* =========================
   Callbacks
   ========================= */
static void on_load_image(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Select Image", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel",
        GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        if (selected_image_path)
            g_free(selected_image_path);
        if (current_pixbuf)
            g_object_unref(current_pixbuf);

        selected_image_path =
            gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        current_pixbuf = gdk_pixbuf_new_from_file(selected_image_path, NULL);

        if (current_pixbuf)
        {
            GdkPixbuf *display_pixbuf = gdk_pixbuf_scale_simple(
                current_pixbuf, 400, 400, GDK_INTERP_BILINEAR);
            gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), display_pixbuf);
            g_object_unref(display_pixbuf);
            g_idle_add(update_status_idle, "Image loaded.");
        }
        else
        {
            g_idle_add(update_status_idle, "Failed to load image.");
        }
    }

    gtk_widget_destroy(dialog);
}

static void on_train_ocr(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;

    /* Lancer le training dans un thread */
    g_thread_new("ocr_train_thread", train_ocr_thread, NULL);
}

static void on_solve(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;

    if (selected_image_path == NULL)
    {
        g_idle_add(update_status_idle, "Error: Please load an image first.");
        return;
    }

    g_idle_add(update_status_idle, "Preprocessing image...");

    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(selected_image_path, &error);
    if (pixbuf == NULL)
    {
        gchar *msg = g_strdup_printf("Error loading image: %s", error->message);
        g_idle_add(update_status_idle, msg);
        g_error_free(error);
        g_free(msg);
        return;
    }

    // 1. Grayscale
    grayscale(pixbuf);

    // 2. Denoise
    GdkPixbuf *denoised = median_filter(pixbuf);
    g_object_unref(pixbuf);

    // 3. Binarize
    binarize(denoised);

    // 4. Rotate
    GdkPixbuf *rotated = automatic_rotate(denoised);
    g_object_unref(denoised);

    // 5. Binarize again
    binarize(rotated);

    g_idle_add(update_status_idle,
               "Preprocessing finished. Detecting blocks...");

    // 6. Block detection
    char old_cwd[1024];
    if (getcwd(old_cwd, sizeof(old_cwd)) == NULL)
    {
        g_idle_add(update_status_idle,
                   "Error: Could not get current directory.");
        g_object_unref(rotated);
        return;
    }

    if (chdir("../pretreatment") != 0)
    {
        g_idle_add(update_status_idle,
                   "Error: Could not switch to pretreatment directory.");
        g_object_unref(rotated);
        return;
    }

    struct stat st = {0};
    if (stat("blocks", &st) == -1)
        mkdir("blocks", 0700);
    if (stat("blocks/grid", &st) == -1)
        mkdir("blocks/grid", 0700);
    if (stat("blocks/words", &st) == -1)
        mkdir("blocks/words", 0700);

    int height = gdk_pixbuf_get_height(rotated);
    int width = gdk_pixbuf_get_width(rotated);

    uint8_t *res = malloc(height * width * sizeof(uint8_t));
    if (!res)
    {
        g_idle_add(update_status_idle,
                   "Error: Block detection memory allocation failed.");
        g_object_unref(rotated);
        chdir(old_cwd); // Go back before returning
        return;
    }

    formatting_binarized(rotated, res);

    size_t size_blocks = 0;
    size_t size_letters = 0;
    int m_height = 0;

    struct Rect *blocks = get_blocks(rotated, res, &size_blocks);
    struct Rect *letters = get_letter_blocks(blocks, size_blocks, &m_height,
                                             res, width, &size_letters);
    struct Rect *grid_blocks =
        save_blocks(letters, m_height, size_letters, rotated);

    if (chdir(old_cwd) != 0)
    {
        g_idle_add(update_status_idle,
                   "Warning: Could not switch back to original directory.");
    }

    predict(); // aura ++ ce qui savent savent

    int **res_pos = NULL;
    int words_count = -1;

    if (grid_blocks)

        res_pos = line_pos(grid_blocks, &words_count);

    for (int i = 0; i < words_count; i++)
    {
        printf("mot no%i :\n", i);
        printf("start = (%i,%i)\n", res_pos[i][0], res_pos[i][1]);
        printf("end = (%i,%i)\n", res_pos[i][2], res_pos[i][3]);

        draw_line_thick(rotated, res_pos[i][0], res_pos[i][1], res_pos[i][2],
                        res_pos[i][3], 255, 0, 0, 255);
    }

    // draw line

    for (int i = 0; i < words_count; i++)
    {
        free(res_pos[i]);
    }
    free(res_pos);

    free(res);
    free(blocks);
    free(letters);

    g_idle_add(update_status_idle, "Block detection finished. Blocks saved.");

    // --- Display the result ---
    GdkPixbuf *display_pixbuf =
        gdk_pixbuf_scale_simple(rotated, 400, 400, GDK_INTERP_BILINEAR);
    if (display_pixbuf)
    {
        gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), display_pixbuf);
        g_object_unref(display_pixbuf);
    }

    if (current_pixbuf)
        g_object_unref(current_pixbuf);
    current_pixbuf = rotated;
}

static void on_rotate(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)user_data;

    if (current_pixbuf == NULL)
    {
        g_idle_add(update_status_idle, "Error: Please load an image first.");
        return;
    }

    const char *angle_text = gtk_entry_get_text(GTK_ENTRY(angle_entry));
    double angle = atof(angle_text);

    g_idle_add(update_status_idle, "Rotating image...");

    GdkPixbuf *rotated_pixbuf = manual_rotate(current_pixbuf, angle);

    if (rotated_pixbuf)
    {
        g_object_unref(current_pixbuf);
        current_pixbuf = rotated_pixbuf;

        GdkPixbuf *display_pixbuf = gdk_pixbuf_scale_simple(
            current_pixbuf, 400, 400, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), display_pixbuf);
        g_object_unref(display_pixbuf);
        g_idle_add(update_status_idle, "Rotation complete.");
    }
    else
    {
        g_idle_add(update_status_idle, "Rotation failed.");
    }
}

static void on_save(GtkButton *button, gpointer user_data)
{

    (void)button;
    (void)user_data;

    if (!current_pixbuf)
    {
        g_print("No image to save\n");
        return;
    }

    struct stat st;
    if (stat("output", &st) == -1)
        mkdir("output", 0700);

    GError *error = NULL;
    if (!gdk_pixbuf_save(current_pixbuf, "output/saved_image.png", "png",
                         &error, NULL))
    {
        g_print("Error saving image: %s\n", error->message);
        g_error_free(error);
    }
    else
    {
        g_print("Image saved successfully\n");
    }
}

/* =========================
   Main
   ========================= */
int main(int argc, char *argv[])
{
    g_print("--- Starting GUI main() ---\n");
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "OCR GUI");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* Image */
    image_widget = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox), image_widget, TRUE, TRUE, 0);

    /* Status label */
    status_label = gtk_label_new("Ready");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 5);

    /* Buttons */
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *btn_load = gtk_button_new_with_label("Load Image");
    g_signal_connect(btn_load, "clicked", G_CALLBACK(on_load_image), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), btn_load, TRUE, TRUE, 0);

    GtkWidget *btn_train = gtk_button_new_with_label("Train OCR");
    g_signal_connect(btn_train, "clicked", G_CALLBACK(on_train_ocr), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), btn_train, TRUE, TRUE, 0);

    GtkWidget *btn_solve = gtk_button_new_with_label("Solve");
    g_signal_connect(btn_solve, "clicked", G_CALLBACK(on_solve), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), btn_solve, TRUE, TRUE, 0);

    angle_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(angle_entry), "Angle");
    gtk_box_pack_start(GTK_BOX(hbox), angle_entry, TRUE, TRUE, 0);

    rotate_button = gtk_button_new_with_label("Rotate");
    g_signal_connect(rotate_button, "clicked", G_CALLBACK(on_rotate), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), rotate_button, TRUE, TRUE, 0);

    GtkWidget *btn_save = gtk_button_new_with_label("Save");
    g_signal_connect(btn_save, "clicked", G_CALLBACK(on_save), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), btn_save, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    if (selected_image_path)
        g_free(selected_image_path);
    if (current_pixbuf)
        g_object_unref(current_pixbuf);

    return 0;
}
