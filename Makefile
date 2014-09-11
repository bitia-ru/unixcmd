require 'pathname'
require 'digest'
require 'git'

require_relative '../common/build'


$root = '../../../'
$tools = $root + 'soft/tools/'
$gnu = 'arm-none-eabi-'
$target = 'kernel'

def ld target, objects
  options = [
    '-static',
    '-nostdlib',
    '--gc-sections',
    '-Map memory.map',
    '-T ldscript_ram_gnu.ld'
  ]

  `#{$gnu}ld #{options.join ' '} #{objects.join ' '} -o #{target}`

  $?
end

def cc target, source
  cfg_file = File.open '../config/global.cfg', 'r'
  cfg = cfg_file.read

  cfg.extend CFLAGSConfigData

  options = [
    '-std=gnu99',
    '-mcpu=cortex-m3',
    '-nostartfiles',
    '-nodefaultlibs',
    '-mthumb',
    '-Wno-all',
    '-Werror-implicit-function-declaration',
    '-O2',
    '-mapcs-frame',
    '-fno-builtin',
    '-D__thumb2__=1',
    cfg.to_s,
    '-msoft-float',
    '-mno-sched-prolog',
    '-fno-hosted',
    '-march=armv7-m',
    '-mfix-cortex-m3-ldrd',
    '-ffunction-sections',
    '-fdata-sections',
    '-I./inc',
    '-I../common/inc'
  ]

  `#{$gnu}gcc -c #{options.join ' '} #{source} -o #{target}`

  $?
end

def as target, source
  options = [
    '-mcpu=cortex-m3'
  ]

  `#{$gnu}as #{options.join ' '} #{source} -o #{target}`

  $?
end

def mkimg target, source
  `#{$tools}mkimg #{source} -o #{target}`

  $?
end

def objcopy target, source
  `#{$gnu}objcopy -O binary #{source} #{target}`

  $?
end

def update_version
  hash = Digest::MD5.digest(IO.binread "#{$target}.bin").unpack("H*")[0]

  version_c = IO.read('src/version.c')

  oldhash = version_c[/KERNEL_BIN_HASH\[\]\s*=\s*\"(.*)\";/, 1]

  fail if oldhash.nil?

  if oldhash != hash
    git = Git.open($root)

    version_c[/KERNEL_BIN_HASH\[\]\s*=\s*\"(.*)\";/, 1] = hash.to_s
    version_c[/KERNEL_BUILD_DATE\[\]\s*=\s*\"(.*)\";/, 1] = Time.now.to_s
    version_c[/KERNEL_COMMIT_HASH\[\]\s*=\s*\"(.*)\";/, 1] = git.log.first.to_s
    version_c[/KERNEL_COMMIT_DATE\[\]\s*=\s*\"(.*)\";/, 1] = git.log.first.date.to_s
    version_c[/KERNEL_BRANCH_NAME\[\]\s*=\s*\"(.*)\";/, 1] = git.current_branch.to_s

    IO.write 'src/version.c', version_c

    return true
  end

  return false
end

sources = FileList['src/*.c', 'src/*.s', "main.c"]

objects = sources.collect { |f| Pathname.new(f).chext('o').to_s }

sources << '../common/src/leds.c'
sources << '../common/src/svi.c'
sources << '../common/src/stubs.c'

objects << 'src/leds.o'
objects << 'src/svi.o'
objects << 'src/stubs.o'


desc "Make #{$target}.bin"
task :default => "#{$target}.bin" do
  puts 'Build successful'
  puts 'Checking version info...'

  if update_version
    puts 'Rebuild with new version info...'

    Rake::Task[:default].reenable
    Rake::Task[:default].invoke
  else
    puts 'Done'
    puts `#{$gnu}size #{$target}.elf`
  end
end

desc 'Clear objects'
task :clean do
  `rm -f #{objects}`
  puts "RM #{objects}"
end

desc 'Clear all unversioned files (not using git)'
task :distclean => :clean do
  `rm -f #{$target}.rom #{$target}.bin #{$target}.elf memory.map tags`
  puts "RM #{$target}.rom #{$target}.bin #{$target}.elf memory.map tags"
end


file "#{$target}.rom" => "#{$target}.bin" do
    puts 'MKIMAGE %s <- %s' % ["#{$target}.rom", "#{$target}.bin"]

    mkimg("#{$target}.rom", "#{$target}.bin").success? or fail
end

file "#{$target}.bin" => "#{$target}.elf" do
  puts 'OBJCOPY %s <- %s' % ["#{$target}.bin", "#{$target}.elf"]

  objcopy("#{$target}.bin", "#{$target}.elf").success? or fail
end

file "#{$target}.elf" => objects do
  puts 'LD %s <- %s' % ["#{$target}.elf", objects]

  ld("#{$target}.elf", objects).success? or fail
end

(sources.select {|f| f =~ /.*.c$/ }).transform('o') do |src, dst|
  case dst
  when /\.\.\/common\/src\/.*/
    rdst = dst.sub '../common/', ''
    file rdst => src do
      puts 'CC %s <- %s' % [rdst, src]

      cc(rdst, src).success? or fail
    end
  else
    file dst => src do
      puts 'CC %s <- %s' % [dst, src]

      cc(dst, src).success? or fail
    end
  end
end

(sources.select {|f| f =~ /.*.s$/ }).transform('o') do |src, dst|
  file dst => src do
    puts 'AS %s <- %s' % [dst, src]

    as(dst, src).success? or fail
  end
end

# vim: sw=2 sts=2 ts=8:

