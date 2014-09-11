require 'launchy'
require 'mime/types'
require '../lib/unixcmd/aux'


class CmdDirWidget < Gtk::Frame
    @path
    @view

    def view() @view end

    def initialize
        super

        model = Gtk::ListStore.new String, String, String, String, String, String, Gdk::Pixbuf, Integer 
        @view = Gtk::TreeView.new model
        
        Gtk::VScrollbar.new @view.vadjustment

        @view.signal_connect('row-activated') { |view, path, column| open(path) }

        scrollwnd = Gtk::ScrolledWindow.new
        scrollwnd.set_policy Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC

        scrollwnd.add @view

        add_child Gtk::Builder.new, scrollwnd

        checker = Gtk::CellRendererToggle.new
        checker.signal_connect('toggled') { |s, path| iter = @view.model.get_iter(path); iter[7] = ~iter[7]; }

        @view.signal_connect('key-press-event') { |s, event| key_pressed(event) }

        cols = [ Gtk::TreeViewColumn.new('', checker, :active => 7 ),
                 Gtk::TreeViewColumn.new('Name'),
                 Gtk::TreeViewColumn.new('Ext',  Gtk::CellRendererText.new, :text => 2),
                 Gtk::TreeViewColumn.new('Size', Gtk::CellRendererText.new, :text => 3),
                 Gtk::TreeViewColumn.new('Date', Gtk::CellRendererText.new, :text => 4),
                 Gtk::TreeViewColumn.new('Attr', Gtk::CellRendererText.new, :text => 5) ]

        renderer = Gtk::CellRendererPixbuf.new
        cols[1].pack_start renderer, false
        cols[1].add_attribute renderer, 'pixbuf', 6

        renderer = Gtk::CellRendererText.new
        cols[1].pack_start renderer, false
        cols[1].add_attribute renderer, 'text', 1

        (1..cols.count).each do |i|
            cols[i-1].sort_column_id = i
        end

        cols.drop(2).each { |col| col.expand = false }
        cols[0].expand = false 
        cols[1].expand = true

        cols.each { |col| @view.append_column col }

        @view.selection.mode = Gtk::SELECTION_MULTIPLE

        model.set_sort_column_id 2

        self.focus_chain = [ @view ]

        grab_focus

        chdir Pathname.new '~'
    end

    def path() @path end
    def expanded_path() @path.expand_path end

    def back() chdir(@path + Pathname.new('..')) end

    def reload
        model = @view.model

        model.clear

        @view.columns[0].set_visible(false)

        files = expanded_path.entries.sort 

        first_row = nil

        files.each do |file|
            full_path = expanded_path + file
            file_icon_path = MIME::Types.get_icon_path full_path

            next if file.to_s == '.'
            next if file.to_s == '..' && expanded_path.root?
            next if file.to_s[0] == '.' && file.to_s != '..' 

            row = model.append

            first_row = row if first_row.nil?

            row[0] = file.to_s

            if full_path.directory? then
                row[1] = file.to_s + '/'
                row[3] = '<DIR>'
            else
                row[1] = file.basename('.*').to_s
                row[3] = full_path.stat.size.to_i.to_szstr
            end

            row[2] = file.extname
            row[4] = Time.at(full_path.stat.mtime).strftime '%x %R'
            row[5] = full_path.stat.mode.to_rwx
            row[6] = Gdk::Pixbuf.new file_icon_path.to_s
            row[7] = 0
        end

        @view.selection.select_iter first_row
        @view.scroll_to_cell first_row.path, @view.columns[1], false, 0.0, 0.0
    end

    def open(path)
        iter = @view.model.get_iter(path)

        file = iter.get_value 0

        if (expanded_path + Pathname.new(file)).directory? then
            chdir @path + Pathname.new(file)
        else
            Launchy.open (expanded_path + Pathname.new(file)).to_s
        end
    end

    def chdir(path)
        old_path = @path == nil ? Pathname.new('') : @path 
        @path = path

        set_label @path.to_s

        reload

        if path == old_path.parent then
            dirname = old_path.basename

            @view.model.each do |model, path, iter|
                if iter.get_value(0) == dirname.to_s then
                    @view.selection.select_path path
                    @view.set_cursor path, nil, false
                    break
                end
            end

        end

        if @view.selection.count_selected_rows == 0 then
            @view.selection.select_iter @view.model.iter_first
            @view.set_cursor @view.model.iter_first.path, nil, false
        end

        grab_focus

        signal_emit 'dir-changed'
    end

    def key_pressed(event)
        if event.keyval == Gdk::Keyval::GDK_Insert
            @view.columns[0].visible = true

            last = nil

            @view.selection.selected_each do |model, path, iter|
                @view.selection.unselect_iter iter
                iter[7] = ~iter[7] if iter[0] != '..'
                last = iter
            end

            @view.selection.select_iter last if last && last.next!

            return true
        elsif event.keyval == Gdk::Keyval::GDK_space
            return true
        end
    end

    type_register

    signal_new('dir_changed', GLib::Signal::RUN_FIRST, nil, nil)

    def signal_do_dir_changed() nil end
end
