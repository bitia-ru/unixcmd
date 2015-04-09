require 'sys/filesystem'
require 'unixcmd/dirview'

class CmdPanelCaptionWidget < Gtk::HBox
    @mounts
    @mounts_handler
    @text

    type_register

    signal_new('go_root', GLib::Signal::RUN_FIRST, nil, nil)
    signal_new('go_home', GLib::Signal::RUN_FIRST, nil, nil)
    signal_new('go_back', GLib::Signal::RUN_FIRST, nil, nil)
    signal_new('go_mountpoint', GLib::Signal::RUN_FIRST, nil, nil, String)

    def signal_do_go_root() nil end 
    def signal_do_go_home() nil end 
    def signal_do_go_back() nil end 
    def signal_do_go_mountpoint(point) nil end

    def initialize
        super

        @text = Gtk::Label.new 'Caption'
        @mounts = Gtk::ComboBox.new

        btn_root = Gtk::Button.new '/'
        btn_home = Gtk::Button.new '~'
        btn_back = Gtk::Button.new '..'

        btn_root.relief = Gtk::RELIEF_NONE
        btn_home.relief = Gtk::RELIEF_NONE
        btn_back.relief = Gtk::RELIEF_NONE

        btn_root.signal_connect('clicked') { signal_emit 'go-root' }
        btn_home.signal_connect('clicked') { signal_emit 'go-home' }
        btn_back.signal_connect('clicked') { signal_emit 'go-back' }

        @mounts_handler = @mounts.signal_connect('changed') { signal_emit 'go-mountpoint', @mounts.active_text }

        @mounts.focus_on_click = false
        
        self.focus_chain = []

        reload_mounts

        pack_start @mounts, false
        pack_start @text, true
        pack_start Gtk::VSeparator.new, false
        pack_start btn_root, false
        pack_start Gtk::VSeparator.new, false
        pack_start btn_home, false
        pack_start Gtk::VSeparator.new, false
        pack_start btn_back, false
    end

    def reload_mounts
        cur_mounts = Array.new
        new_mounts = Array.new 

        Sys::Filesystem.mounts do |mount|
            # TODO: make correct mounts filter
            next if mount.mount_type != 'vfat' && mount.mount_type != 'ntfs' && mount.mount_type != 'ext3' && mount.mount_type != 'ext2' && mount.mount_type != 'ext4'
            new_mounts << Pathname.new(mount.mount_point).to_s
        end

        @mounts.model.each do |model, path, iter|
            cur_mounts << model.get_value(iter, 0)
        end

        if new_mounts != cur_mounts
            @mounts.signal_handler_block @mounts_handler
            @mounts.model.clear
            @mounts.active = -1
            new_mounts.each { |point| @mounts.append_text point }
            @mounts.signal_handler_unblock @mounts_handler
        end
    end

    def update_mount_point(point)
        new_active = nil

        @mounts.model.each do |model, path, iter|
            new_active = model.get_iter(path) if point == model.get_iter(path).get_value(0)
        end

        @mounts.signal_handler_block @mounts_handler
        @mounts.active_iter = new_active if @mounts.active_iter != new_active
        @mounts.signal_handler_unblock @mounts_handler
    end

    def set_text(text)
        @text.text = text
    end
end

class CmdPanelWidget < Gtk::VBox
    @dir
    @caption

    def initialize
        super

        @caption = CmdPanelCaptionWidget.new

        @dir = CmdDirWidget.new

        pack_start @caption, false
        pack_end @dir

        @dir.signal_connect('dir-changed') { update_caption; signal_emit 'dir-changed' }
        @caption.signal_connect('go-root') { @dir.chdir Pathname.new '/' }
        @caption.signal_connect('go-home') { @dir.chdir Pathname.new '~' }
        @caption.signal_connect('go-back') { @dir.back }
        @caption.signal_connect('go-mountpoint') { |widget, point| @dir.chdir Pathname.new point if point != nil }

        self.focus_chain = [ @dir ]

        @dir.chdir Pathname.new '~'
    end

    def update_caption
        path = @dir.path.expand_path.to_s
        mount_point = Sys::Filesystem.mount_point path

        stat = Sys::Filesystem.stat mount_point

        capacity = stat.blocks*stat.block_size
        free = stat.blocks_free*stat.block_size

        @caption.set_text(sprintf("%s of %s free", free.to_i.to_szstr, capacity.to_i.to_szstr))
        @caption.reload_mounts
        @caption.update_mount_point mount_point
    end

    def path
        @dir.path
    end

    def selection
        res = Array.new

        @dir.view().model.each { |model, path, iter|
            res << Pathname.new(iter[0]) if(iter[7] != 0)
        }

        if res.empty? then
            @dir.view.selection.selected_each do |model, path, iter|
                res << Pathname.new(model.get_value(iter, 0))
            end
        end

        res.count > 0 ? res : nil
    end

    def reload
        @dir.reload
    end

    type_register

    signal_new('dir_changed', GLib::Signal::RUN_FIRST, nil, nil)

    def signal_do_dir_changed() nil end
end
