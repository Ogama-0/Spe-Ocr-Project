#include <gtk/gtk.h>

GtkWidget *image_widget = NULL;
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
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (loaded_pixbuf)
            g_object_unref(loaded_pixbuf);

        loaded_pixbuf = gdk_pixbuf_new_from_file(filename, NULL);

        if (loaded_pixbuf) {
            gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), loaded_pixbuf);
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void on_solve(GtkButton *button, gpointer user_data) {
    g_print("Solve button clicked!\n");
    // Appelle moi la fonction pour solve ici le tigre
}

static void on_train(GtkButton *button, gpointer user_data) {
    g_print("Train button clicked!\n");
    // Ici je veut l'appelle pour train l'ocr la bete
}

static void on_save(GtkButton *button, gpointer user_data) {
    g_print("Save button clicked!\n");
    // fonction pour save ici grossee folle
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox_buttons;

    GtkWidget *load_button;
    GtkWidget *solve_button;
    GtkWidget *train_button;
    GtkWidget *save_button;

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

    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}
