#include <gio/gio.h>

int main(int argc, char* argv[])
{
    gchar *type;
    GIcon *icon;

    g_type_init ();

    type = g_content_type_from_mime_type (argv[1]);
    icon = g_content_type_get_icon (type);

    char* str = g_icon_to_string(icon);

    printf("%s\n", str);
    return 0;
}
