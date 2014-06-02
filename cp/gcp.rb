#!/usr/bin/ruby

require 'gtk2'


class CopyWnd < Gtk::Window
  @files

  def initialize files
    super()

    @files = files

    set_title 'Copy window'
    set_border_width 5

    vbox = Gtk::VBox.new

    label1 = Gtk::Label.new 'Copy %s file(s) to:' % @files.count
    dest_combo = Gtk::ComboBoxEntry.new
    label2 = Gtk::Label.new 'Only files of this type:'
    types_combo = Gtk::ComboBoxEntry.new
    types_combo.append_text 'text/text'
    types_combo.append_text 'text/html'
    types_combo.append_text 'image'
    types_combo.append_text 'image/jpeg'
    types_combo.append_text 'image/png'
    types_combo.append_text 'image/gif'
    arch_check = Gtk::CheckButton.new 'Archive mode'
    list_model = Gtk::ListStore.new String
    list = Gtk::TreeView.new list_model

    list_filename_col = Gtk::TreeViewColumn.new('Filename',  Gtk::CellRendererText.new, :text => 0)

    list.append_column list_filename_col

    @files.each { |file| list_model.append[0] = file }

    list_expander = Gtk::Expander.new 'List of files'
    list_expander << list

    hbox = Gtk::HBox.new
    hbox.spacing = 3

    ok_btn = Gtk::Button.new 'OK'
    queue_btn = Gtk::Button.new 'Queue'
    list_btn = Gtk::Button.new 'Files list'
    cancel_btn = Gtk::Button.new 'Cancel'

    hbox << ok_btn
    hbox << queue_btn
    hbox << list_btn
    hbox << cancel_btn

    vbox << label1
    vbox << dest_combo
    vbox << label2
    vbox << types_combo
    vbox << arch_check
    vbox << hbox
    vbox << list_expander

    add vbox

    a = nil

    signal_connect('destroy') { destroy }
    
    list_btn.signal_connect('clicked') {
        self.show
    } 

    #self.allow_shrink = true
    self.allow_grow = true
    set_size_request -1, -1
    show_all
  end

  def destroy
    Gtk.main_quit
  end
end

$wnd = CopyWnd.new ['/usr/bin/ruby', '/etc/fstab', '/home/alev', '~/tutu']

Gtk.main

#require './libcp.rb'
#require './libgcp.rb'


#module GCopyModel
#  include CopyModel
#
#  def cp(src, dst)
#    return if src == dst
#
#    p [:cp, [src, dst]]
#
#    srcfile    = File.new src.expand_path, 'rb'
#    dstfile    = File.new dst.expand_path, 'wb'
#    copied_sum = 0
#    size       = srcfile.stat.size
#    block_size = $cfg['copy']['block_size'] 
#    time_start = Time::now.to_f
#    elapsed    = 0.0
#
#    begin 
#      copied = IO::copy_stream srcfile, dstfile, block_size
#      copied_sum += copied
#
#      view.file_progress copied_sum.to_f/size.to_f
#
#      elapsed = Time::now.to_f - time_start
#      view.file_speed (copied_sum/elapsed).to_i if elapsed > 0
#    end while copied > 0
#
#    nil
#  end
#
#  def mkdir(path)
#    p [:mkdir, [path]]
#    path.mkdir
#  end
#end
#
#module GCopyView
#  include CopyView
#
#  class << View
#    @queue
#
#    def queue=(queue) @queue = queue end
#
#    def start() end
#    def end() @queue.push [:end] end
#
#    def file_start(src, dst) @queue.push [:file_start, [src, dst]] end
#
#    def file_end() end
#
#    def file_progress(percent) @queue.push [:file_progress, [percent]] end
#    def progress(percent) @queue.push [:progress, [percent]] end
#    def dir_create(path) @queue.push [:dir_create, [path]] end
#
#    def file_speed(speed) @queue.push [:file_speed, [speed]] end
#  end
#end
#
#src = ARGV[0..ARGV.count-2]
#dst = ARGV[ARGV.count-1]
#
#queue = Queue.new
#
#cp = Copy.new src, dst, false, false
#cp.view = GCopyView
#cp.model = GCopyModel
#cp.view.queue = queue
#
#dlg = CopyDlg.new
#
#cpthread = Thread.new do
#  begin
#    cp.run
#  rescue => err
#    print 'error: '
#    p err.hash[:type]
#
#    if err.hash[:type] == :not_exist
#        p err.hash[:path]
#        #err.hash[:stop] = true
#    end
#
#    if err.hash[:type] == :srcdstdifftype
#        p err.hash[:src]
#        p err.hash[:dst]
#        #err.hash[:stop] = true
#    end
#
#    if err.hash[:type] == :access_denied
#        p err.hash[:path]
#        #err.hash[:stop] = true
#    end
#
#    if err.hash[:type] == :exist
#        p err.hash[:path]
#        err.hash[:overwrite] = true if err.hash[:path].basename.to_s == '6'
#        #err.hash[:stop] = true
#    end
#
#    err.continue
#  end
#end
#
#cpthread.abort_on_exception = true
#
#idle = Gtk.idle_add do
#  if !queue.empty?
#    call = queue.pop
#    args = call[1]
#
#    dlg.dispatch_model_call call[0], args
#  end
#
#  true
#end
#
#dlg.run do |res|
#  case res
#    when Gtk::Dialog::RESPONSE_CANCEL then cp.stop
#  end
#end
#
#Gtk.idle_remove idle
#cpthread.join
#cpthread.exit

# vim: sw=2 sts=2 ts=8:

