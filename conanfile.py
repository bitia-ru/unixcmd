from conans import ConanFile, CMake, tools, __version__ as conan_version
from conans.client.api.conan_api import Conan

assert conan_version >= tools.Version('1.66'), 'Conan version is too old.'


class QtUnixCmdConan(ConanFile):
    name = 'unixcmd'
    version = '0.0.0'
    generators = 'cmake'

    settings = "os", "arch", "compiler", "build_type"

    exports_sources = (
        'CMakeLists.txt',
        'main.cpp',
        'qml/move_copy_dialog.qml',
        'qml.qrc'
    )

    requires = 'qt/6.4.0@nap/devel'

    options = {
        'ci_build': [False, True],
        'shared': [False, True],
    }

    default_options = {
        'ci_build': False,
        'shared': True,
    }

    def build(self):
        self.cmake.build()

    def package(self):
        self.cmake.install()

    def imports(self):
        self.copy('*.dll', dst='bin', src='bin')
        self.copy('*.dylib', dst='bin', src='lib')
        self.copy('*.so', dst='bin', src='lib')

    @property
    def cmake(self):
        generator = 'Xcode' if self.settings.os == 'iOS' else 'Ninja'
        cmake = CMake(self, generator=generator)
        # cmake.definitions['QT_HOST_PATH'] = 'TODO'
        cmake.definitions['CONAN_CACHE_FOLDER'] = Conan().cache_folder
        cmake.definitions['CONAN_DISABLE_CHECK_COMPILER'] = True

        cmake.definitions['CMAKE_SYSTEM_NAME'] = self.cmake_system_name

        if self.options.ci_build:
            cmake.definitions['IS_CI_BUILD'] = True

        cmake.configure()

        return cmake

    @property
    def cmake_system_name(self):
        if self.settings.os == 'Macos':
            return 'Darwin'
        return self.settings.os
