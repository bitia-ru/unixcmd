#!/usr/bin/ruby19

require 'pathname'
require 'optparse'
require 'optparse/time'
require 'ostruct'


PROG_NAME = 'configure'
VERSION = '0.1'
USAGE = "Usage: #{PROG_NAME} [options]"
ARGS_NUM = (0..(1.0/0.0))


PACKAGE_NAME = 'unixcmd'


def parse_options(args)
    options = OpenStruct.new

    def options.eval_options
      list = marshal_dump

      list.each do |pair|
        opt = pair[0]
        val = pair[1]

        if(val.class == String)
          method("#{opt}=".to_sym).call eval('"' + val + '"')
        end
      end
    end

    opt_parser = OptionParser.new do |opts|
      opts.banner = USAGE
      options.verbose = false

      options.prefix         = '/usr/local'
      options.exec_prefix    = '#{prefix}'
      options.bindir         = '#{exec_prefix}/bin'
      options.sbindir        = '#{exec_prefix}/sbin'
      options.libexecdir     = '#{exec_prefix}/libexec'
      options.sysconfdir     = '#{prefix}/etc'
      options.sharedstatedir = '#{prefix}/com'
      options.localstatedir  = '#{prefix}/var'
      options.runstatedir    = '#{prefix}/run'
      options.libdir         = '#{exec_prefix}/lib'
      options.includedir     = '#{prefix}/include'
      options.oldincludedir  = '/usr/include'
      options.datarootdir    = '#{prefix}/share'
      options.datadir        = '#{datarootdir}'
      options.infodir        = '#{datarootdir}/info'
      options.localedir      = '#{datarootdir}/locale'
      options.mandir         = '#{datarootdir}/man'
      options.docdir         = '#{datarootdir}/doc/#{PACKAGE_NAME}'
      options.htmldir        = '#{docdir}'
      options.dvidir         = '#{docdir}'
      options.pdfdir         = '#{docdir}'
      options.psdir          = '#{docdir}'

      options.program_prefix = ''
      options.program_suffix = ''

      options.features = { 'foo' => false, 'bar' => true }
      options.libs = { 'foobar' => true, 'barbar' => false }

      diropts = {
        'prefix' => 'install architecture-independent files in DIR',
        'exec-prefix' => 'install architecture-dependent files in DIR',
        'bindir' => 'user executables',
        'sbindir' => 'system admin executables',
        'libexecdir' => 'program executables',
        'sysconfdir' => 'read-only single-machine data',
        'sharedstatedir' => 'modifiable architecture-independent data',
        'localstatedir' => 'modifiable single-machine data',
        'runstatedir' => nil,
        'libdir' => 'object code libraries',
        'includedir' => 'C header files',
        'oldincludedir' => 'C header files for non-gcc',
        'datarootdir' => 'read-only arch.-independent data root',
        'datadir' => 'read-only architecture-independent data',
        'infodir' => 'info documentation',
        'localedir' => 'locale-dependent data',
        'mandir' => 'man documentation',
        'docdir' => 'documentation root',
        'htmldir' => 'HTML documentation',
        'dvidir' => 'dvi documentation',
        'pdfdir' => 'PDF documentation',
        'psdir' => 'PS documentation'
      }

      features = {
        'foo' => 'foo feature',
        'bar' => 'foo feature'
      }

      libs = {
        'foobar' => 'foobar library',
        'barbar' => 'barbar library'
      }

      opts.separator ""
      opts.separator "Fine tuning of the installation directories:"

      diropts.each do |pair|
        opt = pair[0]
        opt_r = opt.sub '-', '_'
        desc = pair[1]
        opts.on("--#{opt}=DIR", String, "#{desc} [#{options.method(opt_r.to_sym).call}]") do |path|
          options.method("#{opt_r}=".to_sym).call(path)
        end
      end

      opts.separator ""
      opts.separator "Program names:"

      opts.on("--program-prefix=PREFIX", String, "prepend PREFIX to installed program names") do |prefix|
        options.program_prefix = prefix
      end

      opts.on("--program-suffix=SUFFIX", String, "append SUFFIX to installed program names") do |suffix|
        options.program_suffix = suffix
      end

      opts.separator ""
      opts.separator "Optional Features:"

      features.each do |pair|
        feature = pair[0]
        desc = pair[1]

        newval = !(options.features[feature] == true)
        actstr = newval ? 'enable' : 'disable'

        opts.on("--#{actstr}-#{feature}", "#{actstr} #{features[feature]}") do
          options.features[feature] = newval
        end
      end

      libs.each do |pair|
        lib = pair[0]
        desc = pair[1]

        newval = !(options.libs[lib] == true)
        actstr = newval ? 'with' : 'without'

        opts.on("--#{actstr}-#{lib}", "#{actstr} #{libs[lib]}") do
          options.libs[lib]= newval
        end
      end

      opts.separator ""
      opts.separator "Common options:"

      opts.on("--verbose", "Print maximum information") do
        options.verbose = true
      end

      opts.on_tail("-h", "--help", "Show this message") do
        puts opts
        exit
      end

      opts.on_tail("--version", "Show version") do
        puts VERSION
        exit
      end
    end

    opt_parser.parse!(args)
    [options, opt_parser]
  end

res = parse_options(ARGV)
options = res[0]
opt_parser = res[1]

options.eval_options

unless ARGS_NUM.include? ARGV.count
  p opt_parser 
  exit 1
end

ARGV.delete_if do |arg|
  m = arg.match /\s*([\w\d_]+)\s*\=\s*(.*)\s*/
  unless m.nil?
    ENV[m[1]] = m[2]
    next true
  end
end

diropts = [
  'prefix', 'exec_prefix', 'bindir', 'sbindir', 'libexecdir', 'sysconfdir', 'sharedstatedir',
  'localstatedir', 'runstatedir', 'libdir', 'includedir', 'oldincludedir', 'datarootdir',
  'datadir', 'infodir', 'localedir', 'mandir', 'docdir', 'htmldir', 'dvidir', 'pdfdir', 'psdir'
]

diropts.each do |opt|
  path = options.method(opt.to_sym).call
  if path.class == String
    puts "option #{opt} = #{Pathname.new(path).cleanpath}"
  end
end

options.features.each do |pair|
  puts "feature #{pair[0]} = #{pair[1]}"
end

options.libs.each do |pair|
  puts "using lib #{pair[0]} = #{pair[1]}"
end

p ARGV
p ENV 

# vim: sw=2 sts=2 ts=8:

