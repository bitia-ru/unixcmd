#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define UC_TYPE_MAIN_WINDOW (main_window_get_type())
#define UC_MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), UC_TYPE_MAIN_WINDOW, MainWindow))
#define UC_MAIN_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), UC_TYPE_MAIN_WINDOW, MainWindowClass))
#define UC_IS_MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), UC_TYPE_MAIN_WINDOW))
#define UC_IS_MAIN_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), UC_TYPE_MAIN_WINDOW))
#define UC_MAIN_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), UC_TYPE_MAIN_WINDOW, MainWindowClass))


typedef struct _MainWindow MainWindow;
typedef struct _MainWindowClass MainWindowClass;

struct _MainWindow {
  GtkWidget parent_instance;
};

struct _MainWindowClass
{
  GtkWidgetClass parent_class;
};

GType main_window_get_type(void) G_GNUC_CONST;
GtkWidget* main_window_new(void);
void main_window_foo(MainWindow* self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(MainWindow, g_object_unref)

G_END_DECLS
