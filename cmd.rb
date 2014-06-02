$accels = Gtk::AccelGroup.new

$actMap = Hash.new

require 'thread'

require_relative './copier.rb'
require_relative './copyctrl.rb'
require_relative './fileinfodlg.rb'

$actMap['<unixcmd>/panel/reload'] = Proc.new { $wnd.curpanel.reload }

$actMap['<unixcmd>/file/copy']   = Proc.new { cmd_file_copy $wnd.curpanel.selection, $wnd.curpanel.path, $wnd.otherpanel.path }
$actMap['<unixcmd>/file/remove'] = Proc.new { cmd_file_remove $wnd.curpanel.selection }

$actMap['<unixcmd>/file/view'] = Proc.new { cmd_file_view $wnd.selected_file }
$actMap['<unixcmd>/file/edit'] = Proc.new { cmd_file_edit $wnd.selected_file }

$actMap['<unixcmd>/file/info'] = Proc.new { cmd_file_info $wnd.selected_file }

$actMap['<unixcmd>/app/quit']  = Proc.new { $wnd.destroy }

$actMap['<unixcmd>/test/print_selection'] = Proc.new { p $wnd.curpanel.selection }

$actMap.each_key do |key|
  $accels.connect(key, &$actMap[key])
end

module MyCopyModel
  include CopyModel

  def cp(src, dst)
    return if src == dst

    p [:cp, [src, dst]]

    srcfile    = File.new src.expand_path, 'rb'
    dstfile    = File.new dst.expand_path, 'wb'
    copied_sum = 0
    size       = srcfile.stat.size
    block_size = $cfg['copy']['block_size'] 
    time_start = Time::now.to_f
    elapsed    = 0.0

    begin 
      copied = IO::copy_stream srcfile, dstfile, block_size
      copied_sum += copied

      view.file_progress copied_sum.to_f/size.to_f

      elapsed = Time::now.to_f - time_start
      view.file_speed (copied_sum/elapsed).to_i if elapsed > 0
    end while copied > 0

    nil
  end

  def mkdir(path)
    p [:mkdir, [path]]
    path.mkdir
  end
end

module MyCopyView
  include CopyView

  class << View
    @queue

    def queue=(queue) @queue = queue end

    def start() end
    def end() @queue.push [:end] end

    def file_start(src, dst) @queue.push [:file_start, [src, dst]] end

    def file_end() end

    def file_progress(percent) @queue.push [:file_progress, [percent]] end
    def progress(percent) @queue.push [:progress, [percent]] end
    def dir_create(path) @queue.push [:dir_create, [path]] end

    def file_speed(speed) @queue.push [:file_speed, [speed]] end
  end
end

def cmd_file_copy(files, srcdir, dstdir)
  (dlg = CopyOptionsDlg.new files, srcdir, dstdir).run do |res|
  case res
    when Gtk::Dialog::RESPONSE_CANCEL, Gtk::Dialog::RESPONSE_DELETE_EVENT
      dlg.destroy
      return
    when Gtk::Dialog::RESPONSE_OK then dlg.destroy
    else
      raise ArgumentError, res.to_s
  end
  end

  queue = Queue.new

  copier = Copier.new dlg.files, dlg.src, dlg.dst, false, false
  copier.view = MyCopyView
  copier.model = MyCopyModel
  copier.view.queue = queue

  dlg = CopyDlg.new

  cpthread = Thread.new do
    begin
      copier.run
    rescue => err
      print 'error: '
      p err.hash[:type]

      if err.hash[:type] == :not_exist
          p err.hash[:path]
          #err.hash[:stop] = true
      end

      if err.hash[:type] == :srcdstdifftype
          p err.hash[:src]
          p err.hash[:dst]
          #err.hash[:stop] = true
      end

      if err.hash[:type] == :access_denied
          p err.hash[:path]
          #err.hash[:stop] = true
      end

      if err.hash[:type] == :exist
          p err.hash[:path]
          err.hash[:overwrite] = true if err.hash[:path].basename.to_s == '6'
          #err.hash[:stop] = true
      end

      err.continue
    end
  end

  cpthread.abort_on_exception = true

  idle = Gtk.idle_add do
    if !queue.empty?
      call = queue.pop
      args = call[1]

      dlg.dispatch_model_call call[0], args
    end

    true
  end

  dlg.run do |res|
    case res
      when Gtk::Dialog::RESPONSE_CANCEL then copy.stop
    end
  end

  Gtk.idle_remove idle
  cpthread.join
  cpthread.exit
end

def cmd_file_remove(files)
  files.each do |file|
  end
end

def cmd_file_view(file)
  fullpath = file.expand_path

  return if fullpath.directory?

  if file.expand_path.directory?
    filetype = 'directory'
  else
    filetype = MIME::Types.of fullpath.to_s
    filetype = 'binary (undefined)' if filetype == nil || filetype.count == 0
    filetype = filetype[0].to_s if filetype.class == Array
  end

  viewer = 'xdg-open'

  viewer = $cfg['viewers'][filetype] if $cfg['viewers'].has_key? filetype

  Open3.popen3 viewer, fullpath.to_s
end

def cmd_file_edit(file)
  fullpath = file.expand_path

  return if fullpath.directory?

  if file.expand_path.directory?
    filetype = 'directory'
  else
    filetype = MIME::Types.of fullpath.to_s
    filetype = 'binary (undefined)' if filetype == nil || filetype.count == 0
    filetype = filetype[0].to_s if filetype.class == Array
  end

  editor = 'xdg-open'

  editor = $cfg['editors'][filetype] if $cfg['editors'].has_key? filetype

  Open3.popen3 editor, fullpath.to_s
end

def cmd_file_info(file)
  dlg = FileInfoDlg.new file
  dlg.run
  dlg.destroy
end

# vim: sw=2 sts=2 ts=8:

