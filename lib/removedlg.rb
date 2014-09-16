# encoding: UTF-8

class RemoveDlg
  @builder
  @dlg
  @cmdline
  @flags

  def initialize files
    @builder = Gtk::Builder.new
    @builder.add_from_file 'data/forms/removedlg.glade'
    @dlg = @builder.get_object 'RemoveDlg'
    @cmdline = @builder.get_object 'CmdLine'

    @cmdline.text = "#{files.join ' '}"

    @flags = {
      :recursive => @builder.get_object('RecursiveFlag'),
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

  def verbose?
    @flags[:verbose].active?
  end
end

# vim: sw=2 sts=2 ts=8:

