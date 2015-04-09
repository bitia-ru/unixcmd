# TODO: find more elegance solution
class Fixnum
    def to_rwx
        res = '----------'

        if self&(1<<0) > 0 then res[res.size-1] = 'x' end
        if self&(1<<1) > 0 then res[res.size-2] = 'w' end
        if self&(1<<2) > 0 then res[res.size-3] = 'r' end

        if self&(1<<3) > 0 then res[res.size-4] = 'x' end
        if self&(1<<4) > 0 then res[res.size-5] = 'w' end
        if self&(1<<5) > 0 then res[res.size-6] = 'r' end

        if self&(1<<6) > 0 then res[res.size-7] = 'x' end
        if self&(1<<7) > 0 then res[res.size-8] = 'w' end
        if self&(1<<8) > 0 then res[res.size-9] = 'r' end

        if (self>>9) == 040 then res[res.size-10] = 'd' end

        res
    end
end

# TODO: Bug in Pathname
class Pathname
    alias_method :old_plus, :+
    alias_method :old_exist, :exist?
    alias_method :old_directory, :directory?
    alias_method :old_entries, :entries
    alias_method :old_readable_real, :readable_real?
    alias_method :old_writable_real, :writable_real?
    alias_method :old_mkdir, :mkdir

    def + (pathname)
        return old_plus pathname if pathname.class != Pathname || to_s != '~' || pathname.to_s != '..'

        expand_path.old_plus pathname
    end

    def exist?() expand_path.old_exist end
    def directory?() expand_path.old_directory end
    def entries() expand_path.old_entries end
    def readable_real?() expand_path.old_readable_real end
    def writable_real?() expand_path.old_writable_real end
    def mkdir() expand_path.old_mkdir end
end

# TODO: find another way
UNITS = %W(kiB MiB GiB TiB).freeze

class Integer 
    def to_szstr 
        number = self
        if number < 1024
            exponent = 0
        else
            max_exp  = UNITS.size - 1

            exponent = ( Math.log(number) / Math.log(1024) ).to_i # convert to base
            exponent = max_exp if exponent > max_exp # we need this to avoid overflow for the highest unit

            number /= 1024 ** exponent
        end

        "#{number}" + (exponent > 0 ? " #{UNITS[exponent-1]}" : '')
    end
end

# TODO: bug in Time::strftime
class Time
    alias_method :old_strftime, :strftime

    def strftime(format)
        if ENV['LANG'] == 'ru_RU.UTF-8'
            format = format.sub '%x', '%d.%m.%Y'
        end

        return old_strftime(format)
    end
end

require 'gnome_get_filetype_icon/gnome_get_filetype_icon'

class MIME::Types
    def self.get_icon_names(filepath)
        types = MIME::Types.of filepath

        return nil if types == nil || types.count == 0

        script_path = Pathname.new(__FILE__).expand_path.dirname.to_s

        begin
            res = `#{script_path}/../gnome-get-filetype-icon #{types[0]}`
        rescue
            return nil
        end

        return (res.strip.split ' ').drop(2) if $? == 0

        nil
    end

    def self.get_icon_path(filepath)
        icons_path = Pathname.new '/usr/share/icons/gnome/16x16'

        return icons_path + Pathname.new('places/folder.png') if filepath.directory?

        icon_names = MIME::Types.get_icon_names(filepath.to_s)

        return icons_path + Pathname.new('mimetypes/text-x-generic.png') if icon_names == nil

        icon_names.each do |icon|
            icon_path = icons_path + Pathname.new('mimetypes') + Pathname.new(icon.to_s + '.png')

            return icon_path.to_s if icon_path.exist?
        end

        icons_path + Pathname.new('mimetypes/text-x-generic.png')
    end
end

class Gtk::Widget
    def connect_bypath(path)
        signal_connect('clicked') { $actMap[path].call }
        self
    end

    def inside? widget
        w = self

        while w do
            return true if w == widget

            w = w.parent
        end

        false
    end
end

# continue from exception
require 'continuation'

class Exception
  class NoContinuation < StandardError
  end
  
  attr_accessor :continuation
  
  def continue
    raise NoContinuation unless continuation.respond_to?(:call)
    continuation.call
  end
end

module NeverGonnaLetYouDown
  def raise(exception = RuntimeError, string = nil, array = caller)
    # With a single String argument, raises a
    # RuntimeError with the string as a message. 
    if exception.is_a?(String)
      string = exception
      exception = RuntimeError
    end
    
    callcc do |cc|
      obj = exception.exception(string)
      obj.set_backtrace(array)
      obj.continuation = cc
      super obj
    end
  end
  
  def fail(exception = RuntimeError, string = nil, array = caller)
    raise(exception, string, array)
  end
end

class Object
  include NeverGonnaLetYouDown
end
