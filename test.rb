#!/usr/bin/ruby

require 'pathname'
require_relative './test2.rb'
require_relative './copier'

class Pathname
    alias_method :old_plus, :+
    alias_method :old_exist, :exist?
    alias_method :old_directory, :directory?
    alias_method :old_entries, :entries
    alias_method :old_readable_real, :readable_real?
    alias_method :old_writable_real, :writable_real?

    def + (pathname)
        return old_plus pathname if pathname.class != Pathname || to_s != '~' || pathname.to_s != '..'

        expand_path.old_plus pathname
    end

    def exist?() expand_path.old_exist end
    def directory?() expand_path.old_directory end
    def entries() expand_path.old_entries end
    def readable_real?() expand_path.old_readable_real end
    def writable_real?() expand_path.old_writable_real end
end

module MyCopyView
  include CopyView

  class << View
      def start() puts 'Copying started' end
      def end() puts 'Copying finished' end
      def file_start(src, dst) puts 'Copying %s to %s' % [src.to_s, dst.to_s]; print '[' end
      def file_end() puts "]\nCopying file finished" end
      def file_progress(percent) print '#' end
      def progress(percent) puts 'All files: copied %d %%' % [(percent*100).to_i] end
  end
end

files = [ Pathname.new('1'),
          Pathname.new('2'),
          Pathname.new('3'),
          Pathname.new('4'),
          Pathname.new('5'),
          Pathname.new('6')
        ]

src = Pathname.new '~/1'

dst = Pathname.new '~/2'

cp = Copier.new files, src, dst, false, false

cp.view = MyCopyView

begin
    cp.run
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

