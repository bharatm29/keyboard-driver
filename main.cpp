#include <gtk-4.0/gtk/gtk.h>
#include <string>
#include <vector>
#include "keyboard.c"

using std::string;
using std::vector;

#define TOTAL_EFFECTS 22

static const vector<string> effects = {"Neon_Stream",
    "Ripples Shining",
    "Rotating Windmill",
    "Sine Wave",
    "rainbow Roulette",
    "Stars Twinkle",
    "Layer upon Layer",
    "Rich and Honored",
    "Marquee Effect",
    "Rotating Storm",
    "serpentine Horse",
    "Retro_Snake",
    "Diagonal Transformation",
    "Customize",
    "Ambilight",
    "Streamer",
    "Steady",
    "Breathing",
    "Neon",
    "Shadow_Disappear",
    "Flash Away",
    "Music"};


GtkWidget *brightessScale, *animationaScale, *sleepScale, *mixCheckButton, *colorButton;
static void handle_click(GtkWidget *widget, gpointer data) {
    const GdkRGBA *color = gtk_color_dialog_button_get_rgba(GTK_COLOR_DIALOG_BUTTON(colorButton));
    cmd(GPOINTER_TO_INT(data),
            (u_char)gtk_range_get_value(GTK_RANGE(brightessScale)),
            (u_char)gtk_range_get_value(GTK_RANGE(animationaScale)),
            (u_char)gtk_range_get_value(GTK_RANGE(sleepScale)),
        (u_char)gtk_check_button_get_active(GTK_CHECK_BUTTON(mixCheckButton)),
        color->red * 255, color->green * 255, color->blue * 255 // (r, g, b)
        );
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "RK 71 LED Config");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 800);

    grid = gtk_grid_new();
    // gtk_widget_set_halign(grid, GTK_ALIGN_START);
    gtk_widget_set_halign(grid, GTK_ALIGN_FILL);
    gtk_widget_set_valign(grid, GTK_ALIGN_FILL);

    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);

    for (int i = 0; i < TOTAL_EFFECTS; i++) {
        button = gtk_button_new_with_label(effects[i].c_str());
        g_signal_connect(button, "clicked", G_CALLBACK(handle_click),
                         GINT_TO_POINTER(i + 1));
        gtk_grid_attach(GTK_GRID(grid), button, 0, i, 1, 1);
    }

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    brightessScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, 5, 1);
    gtk_widget_set_hexpand(brightessScale, TRUE);
    gtk_scale_set_draw_value(GTK_SCALE(brightessScale), TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(brightessScale), GTK_POS_RIGHT);

    GtkWidget* label = gtk_label_new("Brightness");
    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), brightessScale);

    animationaScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, 5, 1);
    gtk_widget_set_hexpand(animationaScale, TRUE);
    gtk_scale_set_draw_value(GTK_SCALE(animationaScale), TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(animationaScale), GTK_POS_RIGHT);

    label = gtk_label_new("Animation");
    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), animationaScale);

    sleepScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, 5, 1);
    gtk_widget_set_hexpand(sleepScale, TRUE);
    gtk_scale_set_draw_value(GTK_SCALE(sleepScale), TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(sleepScale), GTK_POS_RIGHT);

    label = gtk_label_new("Sleep");
    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), sleepScale);

    mixCheckButton = gtk_check_button_new_with_label("Color Mixing");
    gtk_box_append(GTK_BOX(box), mixCheckButton);

    GtkColorDialog *dialog = gtk_color_dialog_new();

    colorButton =
        gtk_color_dialog_button_new(dialog);
    gtk_box_append(GTK_BOX(box), colorButton);

    gtk_grid_attach(GTK_GRID(grid), box, 1, 5, 20, 22);

    gtk_window_set_child(GTK_WINDOW(window), grid);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    setup_hid();
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    close_hid();

    return status;
}
