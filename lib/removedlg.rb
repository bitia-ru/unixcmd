# encoding: UTF-8

class RemoveDlg
  FILES_FEW = 3
  FILES_MANY = 5

  @builder
  @dlg
  @filesline
  @fileslist
  @flags
  @fewfiles

  def initialize files
    @builder = Gtk::Builder.new
    @builder.add_from_file 'data/forms/removedlg.glade'
    @dlg = @builder.get_object 'RemoveDlg'
    @filesline = @builder.get_object 'FilesLine'
    @fileslist = @builder.get_object 'FilesList'

    @fewfiles = files.count <= FILES_FEW

    unless @fewfiles == true
      @fileslist.show
      @filesline.hide

      files.each_with_index do |file, i|
        row = @fileslist.model.append
        row[0] = file

        # *TODO!* remove 110 magic number
        @fileslist.set_height_request 110 if i+1 == FILES_MANY
      end
    else
      @filesline.text = "#{files.join ' '}"
    end

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

