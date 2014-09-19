# encoding: UTF-8

class MkDirDlg
  @builder
  @dlg
  @mkdirentry
  @flags

  def initialize
    @builder = Gtk::Builder.new
    @builder.add_from_file 'data/forms/mkdirdlg.glade'
    @dlg = @builder.get_object 'MkDirDlg'
    @mkdirentry = @builder.get_object 'MkDirEntry'

    @flags = {
      :verbose => @builder.get_object('VerboseFlag'),
    }
  end

  def run
    @dlg.run
  end

  def destroy
    @dlg.destroy
  end

  def verbose?
    @flags[:verbose].active?
  end

  def dirname
    @mkdirentry.text
  end
end

# vim: sw=2 sts=2 ts=8:

