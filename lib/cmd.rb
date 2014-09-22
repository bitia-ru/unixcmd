# encoding: UTF-8

require 'thread'


$accels = Gtk::AccelGroup.new

$actMap = Hash.new

$actMap['<unixcmd>/panel/reload'] = Proc.new { $wnd.curpanel.reload }

$actMap['<unixcmd>/file/copy']   = Proc.new do
  cmd_file_copy $wnd.curpanel.selection, $wnd.curpanel.path, $wnd.otherpanel.path
end

$actMap['<unixcmd>/file/move']   = Proc.new do
  cmd_file_move $wnd.curpanel.selection, $wnd.curpanel.path, $wnd.otherpanel.path
end

$actMap['<unixcmd>/file/remove'] = Proc.new do
  cmd_file_remove $wnd.curpanel.selection, $wnd.curpanel.path
end

$actMap['<unixcmd>/file/mkdir'] = Proc.new do
  cmd_file_mkdir $wnd.curpanel.path
end

$actMap['<unixcmd>/file/view'] = Proc.new { cmd_file_view $wnd.selected_file }
$actMap['<unixcmd>/file/edit'] = Proc.new { cmd_file_edit $wnd.selected_file }

$actMap['<unixcmd>/file/info'] = Proc.new { cmd_file_info $wnd.selected_file }

$actMap['<unixcmd>/app/quit']  = Proc.new { $wnd.destroy }

$actMap['<unixcmd>/test/print_selection'] = Proc.new { p $wnd.curpanel.selection; $wnd.cmd "cd #{$wnd.curpanel.path.to_s}" }

$actMap.each_key do |key|
  $accels.connect(key, &$actMap[key])
end

unixcmd_require 'copydlg'
unixcmd_require 'removedlg'
unixcmd_require 'movedlg'
unixcmd_require 'mkdirdlg'

def cmd_file_copy(files, srcdir, dstdir)
  files_s = files.map do |file|
    file.to_s
  end

  dlg = CopyDlg.new files_s, dstdir.to_s
  res = dlg.run

  flags = ''

  flags << '-r ' if dlg.recursive?
  flags << '-a ' if dlg.archive?
  flags << '-v ' if dlg.verbose?
  flags.strip!

  dstdir = Pathname.new(dlg.dest)

  unless res == 0
    dlg.destroy
    return
  end

  cpthread = Thread.new do
    Dir.chdir(srcdir.expand_path.to_s) do
      $wnd.cmd "cp #{flags} #{files_s.join ' '} #{dstdir.to_s}"
    end
  end

  cpthread.abort_on_exception = true
  cpthread.join
  cpthread.exit

  dlg.destroy
end

def cmd_file_move(files, srcdir, dstdir)
  files_s = files.map do |file|
    file.to_s
  end

  dlg = MoveDlg.new files_s, dstdir
  res = dlg.run

  flags = ''

  flags << '-v ' if dlg.verbose?
  flags.strip!

  unless res == 0
    dlg.destroy
    return
  end

  mvthread = Thread.new do
    Dir.chdir(srcdir.expand_path.to_s) do
      puts $wnd.cmd "mv #{flags} #{files_s.join ' '} #{dstdir.to_s}"
    end
  end

  mvthread.abort_on_exception = true
  mvthread.join
  mvthread.exit

  dlg.destroy
end

def cmd_file_remove(files, dir)
  files_s = files.map do |file|
    file.to_s
  end

  dlg = RemoveDlg.new files_s
  res = dlg.run

  flags = ''

  flags << '-r ' if dlg.recursive?
  flags << '-v ' if dlg.verbose?
  flags.strip!

  unless res == 0
    dlg.destroy
    return
  end

  thread = Thread.new do
    Dir.chdir(dir.expand_path.to_s) do
      $wnd.cmd "rm #{flags} #{files_s.join ' '}"
    end
  end

  thread.abort_on_exception = true
  thread.join
  thread.exit

  dlg.destroy
end

def cmd_file_mkdir(dir)
  dlg = MkDirDlg.new
  res = dlg.run

  flags = ''

  flags << '-v ' if dlg.verbose?
  flags.strip!

  dirname = dlg.dirname

  unless res == 0
    dlg.destroy
    return
  end

  thread = Thread.new do
    Dir.chdir(dir.expand_path.to_s) do
      $wnd.cmd "mkdir #{flags} #{dirname}"
    end
  end

  thread.abort_on_exception = true
  thread.join
  thread.exit

  dlg.destroy
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

