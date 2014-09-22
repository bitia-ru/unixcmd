Gem::Specification.new do |s|
  s.name        = 'unixcmd'
  s.version     = '0.2.0'
  s.date        = '2014-09-22'
  s.summary     = "UNIX Commander"
  s.description = "Orthodox file manager"
  s.authors     = ["Levenkov Artem"]
  s.email       = 'artem@levenkov.org'
  s.files       = [
                    "lib/unixcmd/panel.rb",
                    "lib/unixcmd/dirview.rb",
                    "lib/unixcmd/config.rb",
                    "lib/unixcmd/cmd.rb",
                    "lib/unixcmd/version.rb",
                    "lib/unixcmd/aux.rb"
                  ]
  s.executables = ["unixcmd"]
  s.homepage    =
    'http://www.unixcmd.org/'
  s.license       = 'MIT'
end
