#include <gtk/gtk.h>
#include "../pretreatment/manual_rotate.h"
#include "../pretreatment/preprocess.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../ocr/train_ocr.h"

GtkWidget *image_widget = NULL;
GtkWidget *angle_entry = NULL;

GdkPixbuf *loaded_pixbuf = NULL;

static void on_load_image(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    GtkWindow *parent_window = GTK_WINDOW(user_data);

    dialog = gtk_file_chooser_dialog_new("Open Image",
                                         parent_window,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (loaded_pixbuf)
            g_object_unref(loaded_pixbuf);

        loaded_pixbuf = gdk_pixbuf_new_from_file(filename, NULL);

        if (loaded_pixbuf)
            gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), loaded_pixbuf);

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void on_rotate(GtkButton *button, gpointer user_data) {
    if (!loaded_pixbuf)
        return;

    const char *text = gtk_entry_get_text(GTK_ENTRY(angle_entry));
    double angle = g_ascii_strtod(text, NULL);

    GdkPixbuf *rotated = rotate_pixbuf(loaded_pixbuf, angle);

    if (!rotated) {
        g_print("Rotation échouée\n");
        return;
    }

    g_object_unref(loaded_pixbuf);
    loaded_pixbuf = rotated;

    gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), loaded_pixbuf);
}

static void on_solve(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;

    if (!loaded_pixbuf) {
        g_print("Aucune image chargée.\n");
        return;
    }

    // Récupérer l'angle de rotation depuis l'entrée (GtkEntry)
    const char *angle_text = gtk_entry_get_text(GTK_ENTRY(angle_entry));
    double angle = g_ascii_strtod(angle_text, NULL);

    // Chemins temporaires pour le prétraitement
    const char *input_path  = "images/temp_input.png";
    const char *output_path = "images/output.bmp";

    // Créer le dossier "images" si nécessaire
    struct stat st;
    if (stat("images", &st) == -1)
        mkdir("images", 0700);
    // Sauvegarder l'image chargée temporairement
    GError *error = NULL;
    if (!gdk_pixbuf_save(loaded_pixbuf, input_path, "png", &error, NULL)) {
        g_print("Erreur lors de la sauvegarde de l'image temporaire: %s\n",
                error->message);
        g_error_free(error);
        return;
    }

    // === Prétraitement automatique ===
    int ret = preprocess_image(input_path, output_path, angle);
    if (ret != 0) {
        g_print("Erreur lors du prétraitement de l'image\n");
        return;
    }

    // Recharger l'image prétraitée dans GtkImage
    if (loaded_pixbuf)
        g_object_unref(loaded_pixbuf);

    loaded_pixbuf = gdk_pixbuf_new_from_file(output_path, &error);
    if (!loaded_pixbuf) {
        g_print("Erreur lors du chargement de l'image prétraitée: %s\n",
                error ? error->message : "Erreur inconnue");
        if (error) g_error_free(error);
        return;
    }

    gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), loaded_pixbuf);

    // === Ici, tu peux lancer la fonction OCR pour résoudre le puzzle ===
    g_print("Prétraitement terminé. On peut maintenant lancer l'OCR.\n");
    // ex: solve_puzzle(loaded_pixbuf, ...);
}

static void on_train(GtkButton *button, gpointer user_data) {
    (void)button;
    (void)user_data;

    g_print("Training OCR...\n");

    if (train_ocr() != 0)
        g_printerr("OCR training failed\n");
    else
        g_print("OCR training completed successfully\n");

}

static void on_save(GtkButton *button, gpointer user_data) {
    g_print("Save button clicked!\n");
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox_buttons;

    GtkWidget *load_button;
    GtkWidget *solve_button;
    GtkWidget *train_button;
    GtkWidget *save_button;
    GtkWidget *rotate_button;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "OCR Word Search Solver");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    image_widget = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(vbox), image_widget, TRUE, TRUE, 5);

    hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_buttons, FALSE, FALSE, 5);

    load_button = gtk_button_new_with_label("Load Image");
    g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_image), window);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), load_button, TRUE, TRUE, 5);

    solve_button = gtk_button_new_with_label("Solve");
    g_signal_connect(solve_button, "clicked", G_CALLBACK(on_solve), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), solve_button, TRUE, TRUE, 5);

    train_button = gtk_button_new_with_label("Train OCR");
    g_signal_connect(train_button, "clicked", G_CALLBACK(on_train), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), train_button, TRUE, TRUE, 5);

    save_button = gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), save_button, TRUE, TRUE, 5);

    angle_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(angle_entry), "Angle (°)");
    gtk_box_pack_start(GTK_BOX(hbox_buttons), angle_entry, TRUE, TRUE, 5);

    rotate_button = gtk_button_new_with_label("Rotate");
    g_signal_connect(rotate_button, "clicked", G_CALLBACK(on_rotate), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), rotate_button, TRUE, TRUE, 5);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
