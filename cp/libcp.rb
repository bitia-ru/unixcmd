require 'ostruct'


module CopyView
  class View
    def self.start() puts 'Copying started' end
    def self.end() puts 'Copying finished' end
    def self.file_start(src, dst) puts 'Copying %s to %s' % [src.to_s, dst.to_s] end
    def self.file_end() puts 'Copying file finished' end
    def self.file_skip(path) puts 'Copying file %s skiped' % [path.to_s] end
    def self.file_progress(percent) puts 'Copied %d %%' % [(percent*100).to_i] end
    def self.dir_create(path) puts 'Creating directory %s' % [path.to_s] end
    def self.progress(percent) puts 'All files: copied %d %%' % [(percent*100).to_i] end

    def self.file_speed(speed) puts 'File copying speed: %s/s' % [speed.to_i.to_szstr] end
  end

  def view() return View end
end

module CopyModel
  def cp(src, dst)
    return if src == dst

    p [:cp, [src, dst]]

    (1..10).each do |i|
      sleep 0.1
      view.file_progress i/10.to_f
    end

    nil
  end

  def mkdir(path) p [:mkdir, [path]] end

  def model() return self end
end

class CopierError < StandardError 
  attr_accessor :hash

  def initialize(msg = nil, hash)
    super msg
    @hash = hash
  end
end

class Copier
  @src
  @dst
  @move

  def initialize(src, dst, overwrite, move)
    @src, @dst, @overwrite, @move = src, dst, overwrite, move 

    dst = Pathname.new dst if dst.class == String
    src = Pathname.new src if src.class == String

    if src.class == Array
      src.count.times do |i|
        if src[i].class == String
          src[i] = Pathname.new src[i]
        else
          raise ArgumentError, 'source paths can be String or Pathname'
        end
      end
    end
  end

  def run
    view.start

    copy @src, @dst

    view.end
  end

  def copy(src, dst)
    if src.class == Pathname && !dst.directory?
      # TODO: copy one item
      view.file_start src, dst+src.basename

      model.cp src, dst+src.basename

      view.file_end

      view.progress 1.0
    elsif src.class == Array
      if !dst.directory? and src.count > 1
        # attempting to copy many files to one file
        raise CopierError.new nil, { :type => :manytoone, :src => src, :dst => dst }
      end

      src.each_with_index do |srcfile, i|
        overwrite = false

        filename = srcfile.basename

        if (dst+filename).exist?

          if (dst+filename).directory? != srcfile.directory?
            msg = CopierError.new nil, { :type => :srcdstdifftype, :src => srcfile, :dst => (dst+filename), :stop => false}

            raise msg

            break if msg.hash[:stop]
            next
          end

        elsif srcfile.directory?

          view.dir_create dst+filename

          model.mkdir dst+filename

        end


        if (src+file).exist?

          if (src+file).directory?

              #TODO: cp nil, src+file, dst+file

          else # (src+file).directory? == false

            if (dst+file).exist?
              msg = CopierError.new nil, { :type => :exist, :path => (dst+file), :overwrite => overwrite, :stop => false }

              raise msg

              break if msg.hash[:stop]
              overwrite = msg.hash[:overwrite]
              #puts 'overwrite = %d' % overwrite
              p overwrite
            end

            if (!(dst+file).exist? or (dst+file).exist? && (dst+file).writable_real? && overwrite) and (src+file).readable_real?
              view.file_start src+file, dst+file

              model.cp src+file, dst+file

              view.file_end

              view.progress (i+1)/files.count.to_f
            elsif !(src+file).readable_real? and overwrite
              msg = CopierError.new nil, { :type => :access_denied, :path => (src+file), :stop => false}

              raise msg

              break if msg.hash[:stop]
            elsif !(dst+file).writable_real? and overwrite
              msg = CopierError.new nil, { :type => :access_denied, :path => (dst+file), :stop => false}

              raise msg

              break if msg.hash[:stop]
            else # overwrite == false
              view.file_skip dst+file
            end

          end

        else # (src+file).exist? == false
          msg = CopierError.new nil, { :type => :not_exist, :path => (src+file), :stop => false}

          raise msg

          break if msg.hash[:stop]
        end

      end
    else
      raise ArgumentError, 'argument src shall be String, Pathname or Array'
    end
  end

  def view=(view) self.extend view end
  def model=(model) self.extend model end

  include CopyView
  include CopyModel
end

# vim: sw=2 sts=2 ts=8:
