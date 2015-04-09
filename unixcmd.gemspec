require './lib/unixcmd/version.rb'

Gem::Specification.new do |s|
  s.name        = APP_NAME
  s.version     = APP_VERSION
  s.date        = Time.now.strftime '%Y-%m-%d'
  s.summary     = "UNIX Commander"
  s.description = "Orthodox file manager"
  s.authors     = ["Levenkov Artem"]
  s.email       = 'artem@levenkov.org'
  s.files       = Dir['lib/unixcmd/**/*.rb'] + Dir['data/**/*'] + ['ext/gnome_get_filetype_icon/gnome_get_filetype_icon.c']
  s.executables = [ 'unixcmd' ]
  s.homepage    = 'http://www.unixcmd.org/'
  s.license     = 'MIT'
  s.extensions  = ['ext/gnome_get_filetype_icon/extconf.rb']

  s.add_runtime_dependency 'launchy', '~> 0'
end
