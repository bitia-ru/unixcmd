require 'mkmf'

pkg_config('gio-2.0')

create_makefile 'gnome_get_filetype_icon/gnome_get_filetype_icon'
