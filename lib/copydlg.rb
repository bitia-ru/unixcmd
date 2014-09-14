# encoding: UTF-8

class CopyDlg
  @builder
  @dlg
  @cmdline
  @flags

  def initialize files, dest
    @builder = Gtk::Builder.new
    @builder.add_from_file 'data/forms/copydlg.glade'
    @dlg = @builder.get_object 'CopyDlg'
    @cmdline = @builder.get_object 'CmdLine'

    @cmdline.text = "#{files.join ' '} → #{dest}"

    @flags = {
      :recursive => @builder.get_object('RecursiveFlag'),
      :archive => @builder.get_object('ArchiveFlag'),
      :verbose => @builder.get_object('VerboseFlag'),
    }
  end

  def run
    @dlg.run
  end

  def destroy
    @dlg.destroy
  end

  def recursive?
    @flags[:recursive].active?
  end

  def archive?
    @flags[:archive].active?
  end

  def verbose?
    @flags[:verbose].active?
  end
end

# vim: sw=2 sts=2 ts=8:

