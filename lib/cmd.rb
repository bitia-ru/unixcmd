# encoding: UTF-8

$accels = Gtk::AccelGroup.new

$actMap = Hash.new

require 'thread'

$actMap['<unixcmd>/panel/reload'] = Proc.new { $wnd.curpanel.reload }

$actMap['<unixcmd>/file/copy']   = Proc.new { cmd_file_copy $wnd.curpanel.selection, $wnd.curpanel.path, $wnd.otherpanel.path }
$actMap['<unixcmd>/file/move']   = Proc.new { cmd_file_move $wnd.curpanel.selection, $wnd.curpanel.path, $wnd.otherpanel.path }
$actMap['<unixcmd>/file/remove'] = Proc.new { cmd_file_remove $wnd.curpanel.selection }

$actMap['<unixcmd>/file/view'] = Proc.new { cmd_file_view $wnd.selected_file }
$actMap['<unixcmd>/file/edit'] = Proc.new { cmd_file_edit $wnd.selected_file }

$actMap['<unixcmd>/file/info'] = Proc.new { cmd_file_info $wnd.selected_file }

$actMap['<unixcmd>/app/quit']  = Proc.new { $wnd.destroy }

$actMap['<unixcmd>/test/print_selection'] = Proc.new { p $wnd.curpanel.selection }

$actMap.each_key do |key|
  $accels.connect(key, &$actMap[key])
end

def cmd_file_copy(files, srcdir, dstdir)
  builder = Gtk::Builder.new
  builder.add_from_file 'data/forms/copydlg.glade'
  dlg = builder.get_object 'dialog1'
  dlg.run

  cpthread = Thread.new do
    files_with_path = files.map do |file|
      (srcdir+file).to_s
    end

    puts "cp #{files_with_path.join ' '} → #{dstdir}"
  end

  cpthread.abort_on_exception = true
  cpthread.join
  cpthread.exit
end

def cmd_file_move(files, srcdir, dstdir)
  cpthread = Thread.new do
    files_with_path = files.map do |file|
      (srcdir+file).to_s
    end

    puts "mv #{files_with_path.join ' '} → #{dstdir}"
  end

  cpthread.abort_on_exception = true
  cpthread.join
  cpthread.exit
end

def cmd_file_remove(files)
  files_s = files.map do |file|
    file.to_s
  end

  puts "rm #{files_s.join ' '}"
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

