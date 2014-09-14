class CopyDlg
  @builder
  @dlg

  def initialize
    @builder = Gtk::Builder.new
    @builder.add_from_file 'data/forms/copydlg.glade'
    @dlg = @builder.get_object 'CopyDlg'
  end

  def run
    @dlg.run
  end
end

# vim: sw=2 sts=2 ts=8:

