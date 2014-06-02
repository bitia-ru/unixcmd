require 'launchy'

class FileInfoDlg < Gtk::Dialog
    def initialize(file)
        super()

        fileapp = 'TODO' 

        if file.expand_path.directory?
            filetype = 'directory'
        else
            filetype = MIME::Types.of file.expand_path.to_s
            filetype = 'binary (undefined)' if filetype == nil || filetype.count == 0
            filetype = filetype[0] if filetype.class == Array
        end

        self.title = 'Properties of file %s' % file.basename.to_s

        self.vbox.add Gtk::Label.new 'Name: %s' % file.basename.to_s
        self.vbox.add Gtk::HSeparator.new
        self.vbox.add Gtk::Label.new 'Filetype: %s' % filetype
        self.vbox.add Gtk::Label.new 'Application: %s' % fileapp
        self.vbox.add Gtk::HSeparator.new
        self.vbox.add Gtk::Label.new 'Location: %s' % file.dirname.to_s
        self.vbox.add Gtk::Label.new 'Size: %s' % file.expand_path.stat.size.to_szstr
        self.vbox.add Gtk::HSeparator.new
        self.vbox.add Gtk::Label.new 'Created: %s' % file.expand_path.stat.ctime
        self.vbox.add Gtk::Label.new 'Changed: %s' % file.expand_path.stat.mtime
        self.vbox.add Gtk::Label.new 'Opened: %s' % file.expand_path.stat.atime

        btn_ok = self.add_button Gtk::Stock::OK, Gtk::Dialog::RESPONSE_OK

        btn_ok.has_focus = true
        self.focus = btn_ok

        show_all
    end
end
