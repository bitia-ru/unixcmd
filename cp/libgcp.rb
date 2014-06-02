class CopyOptionsDlg < Gtk::Dialog
    @dest_path
    @filter_box

    @files
    @src
    @dst

    def files() @files end
    def src()   @src   end
    def dst()   @dst   end

    def initialize(files, src, dst)
        super()

        self.title = 'Copying files...'
        self.modal = true

        @dest_path = Gtk::Entry.new
        @filter_box = Gtk::ComboBoxEntry.new

        @files = files
        @src = src
        @dst = dst

        @dest_path.text = dst.to_s

        self.vbox.add Gtk::Label.new 'Copy ' + files.count.to_s + ' file(s) to:'
        self.vbox.add @dest_path
        self.vbox.add Gtk::Label.new 'Only files of this type:'
        self.vbox.add @filter_box
        self.add_button Gtk::Stock::CANCEL, Gtk::Dialog::RESPONSE_CANCEL
        btn_ok = self.add_button Gtk::Stock::OK, Gtk::Dialog::RESPONSE_OK

        btn_ok.has_focus = true
        self.focus = btn_ok

        show_all
    end
end

class CopyDlg < Gtk::Dialog
    @top_info
    @src_info
    @dest_info

    def initialize
        super()

        self.title = 'Copying files...'

        @top_info = Gtk::Label.new 'Copying...'
        @src_info = Gtk::Label.new 'From: path'
        @dest_info = Gtk::Label.new 'To: path'

        @progress_cur = Gtk::ProgressBar.new
        @progress_all = Gtk::ProgressBar.new

        self.vbox.add(@top_info)
        self.vbox.add(@src_info)
        self.vbox.add(@dest_info)
        self.vbox.add(@progress_cur)
        self.vbox.add(@progress_all)
        self.add_button(Gtk::Stock::CANCEL, Gtk::Dialog::RESPONSE_CANCEL)

        @src_info.text = 'From:'
        @dest_info.text = 'To:'

        show_all
    end

    def dispatch_model_call(call, args)
      case call
        when :end
          destroy
        when :file_start
          @top_info.text = 'Copying file...'
          @src_info.text = args[0].to_s
          @dest_info.text = args[1].to_s
        when :file_progress
          @progress_cur.fraction = args[0]
        when :progress
          @progress_all.fraction = args[0]
        when :dir_create
          @top_info.text = 'Creating directory...'
          @src_info.text = args[0].to_s
          @dest_info.text = '' 
        when :file_speed
          @progress_cur.text = '%s/s' % [args[0].to_i.to_szstr]
        else
          p call
          raise ArgumentError
      end
    end
    
    #def info_errask(text)
      #dlg = Gtk::MessageDialog.new(self, 
      #  0,
      #  Gtk::MessageDialog::QUESTION,
      #  Gtk::MessageDialog::BUTTONS_YES_NO,
      #  text )
      #res = dlg.run
      #dlg.destroy

      #case res
      #  when Gtk::Dialog::RESPONSE_YES then return true
      #  when Gtk::Dialog::RESPONSE_NO then return false
      #  else
      #    raise ArgumentError, res
      #end

    #  return false
    #end
end

# vim: sw=2 sts=2 ts=8:

