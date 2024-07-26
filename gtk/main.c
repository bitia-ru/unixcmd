#include <gtk/gtk.h>

#include "main_window.h"

static void on_activate(GApplication* app, gpointer user_data) {
  GtkWidget* window = gtk_application_window_new(GTK_APPLICATION(app));
  gtk_window_set_title(GTK_WINDOW(window), "UnixCMD");
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);

  GtkWidget* main_window = main_window_new();
  gtk_window_set_child(GTK_WINDOW(window), main_window);

  main_window_foo(UC_MAIN_WINDOW(main_window));

  gtk_widget_show(window);
}

int main(int argc, char **argv) {
  GtkApplication *app = gtk_application_new("app.unixcmd", G_APPLICATION_FLAGS_NONE);

  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

  int status = g_application_run(G_APPLICATION(app), argc, argv);

  g_object_unref(app);

  return status;
}
