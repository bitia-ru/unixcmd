#include "main_window.h"

typedef struct {
  int i;
} MainWindowPrivate;

G_DEFINE_TYPE_WITH_CODE(MainWindow, main_window, GTK_TYPE_WIDGET, G_ADD_PRIVATE(MainWindow))

static void main_window_dispose(GObject* object)
{
  GtkWidget *child;
  while ((child = gtk_widget_get_first_child(GTK_WIDGET(object))))
    gtk_widget_unparent(child);
  G_OBJECT_CLASS(main_window_parent_class)->dispose(object);
}

static void main_window_class_init(MainWindowClass* class)
{
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);
  object_class->dispose = main_window_dispose;
}

static void main_window_init(MainWindow* mainWindow)
{
  puts("main_window_init\n");

  ((MainWindowPrivate*)main_window_get_instance_private(mainWindow))->i = 5;
}

void main_window_foo(MainWindow* self)
{
  MainWindowPrivate* priv = main_window_get_instance_private(self);

  g_print("i: %d\n", priv->i);
}

GtkWidget* main_window_new()
{
  return g_object_new(UC_TYPE_MAIN_WINDOW, NULL);
}
