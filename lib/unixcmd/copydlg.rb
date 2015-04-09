# encoding: UTF-8

class CopyDlg
  FILES_FEW = 3

  @builder
  @dlg
  @srcline
  @srclist
  @dstline
  @dstentry
  @flags
  @singlesrc
  @fewsrc

  def initialize files, dest
    @builder = Gtk::Builder.new
    @builder.add_from_file 'data/forms/copydlg.glade'
    @dlg = @builder.get_object 'CopyDlg'
    @srcline = @builder.get_object 'SrcLine'
    @srclist = @builder.get_object 'SrcList'
    @dstline = @builder.get_object 'DstLine'
    @dstentry = @builder.get_object 'DstEntry'

    @singlesrc = files.count == 1
    @fewsrc = files.count <= FILES_FEW

    unless @singlesrc == true
      @dstline.text = dest
    else
      @dstentry.text = dest
    end

    unless @fewsrc == true
      @srcline.hide
      @srclist.show

      files.each do |file|
        row = @srclist.model.append
        row[0] = file
      end
    else
      @srcline.text = files.join ' '
    end

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

  def dest
    @singlesrc == true ? @dstentry.text : @dstline.text
  end
end

# vim: sw=2 sts=2 ts=8:

