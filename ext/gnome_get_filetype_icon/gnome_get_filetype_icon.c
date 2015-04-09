#include <gio/gio.h>
#include "ruby.h"

void Init_gnome_get_filetype_icon();

VALUE method_get(VALUE self, VALUE path);

void Init_gnome_get_filetype_icon() {
	rb_define_global_function("gnome_get_filetype_icon", method_get, 1);
}

VALUE method_get(VALUE self, VALUE path) {
    gchar *type;
    GIcon *icon;

    g_type_init ();

    type = g_content_type_from_mime_type (StringValuePtr(path));
    icon = g_content_type_get_icon (type);

    char* str = g_icon_to_string(icon);

	return rb_str_new2(str);
}

