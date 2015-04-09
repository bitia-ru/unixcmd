# encoding: UTF-8

class MoveDlg
  @builder
  @dlg
  @cmdline
  @flags

  def initialize files, dest
    @builder = Gtk::Builder.new
    @builder.add_from_file 'data/forms/movedlg.glade'
    @dlg = @builder.get_object 'MoveDlg'
    @cmdline = @builder.get_object 'CmdLine'

    @cmdline.text = "#{files.join ' '} → #{dest}"

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
end

# vim: sw=2 sts=2 ts=8:

