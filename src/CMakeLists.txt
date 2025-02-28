if(ANDROID)
  qt_add_library(${PROJECT_NAME} SHARED)
else()
  if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE "unixcmd.icns")

    set(project_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/resources/unixcmd.icns")
    set_source_files_properties(${project_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set(PLATFORM_DEPENDENT_ARGUMENTS MACOSX_BUNDLE ${project_icon_macos})
  endif()

  qt_add_executable(
    ${PROJECT_NAME}
      ${PLATFORM_DEPENDENT_ARGUMENTS}
      about_dialog.cpp
      about_dialog.h
      copy_dialog.cpp
      copy_dialog.h
      create_directory_dialog.cpp
      create_directory_dialog.h
      directory_model.cpp
      directory_model.h
      directory_status_bar.cpp
      directory_status_bar.h
      directory_view.cpp
      directory_view.h
      directory_view_style.cpp
      directory_view_style.h
      directory_widget.cpp
      directory_widget.h
      double_panel_splitter.cpp
      double_panel_splitter.h
      file_processing_dialog.cpp
      file_processing_dialog.h
      main.cpp
      main_window.cpp
      main_window.h
    )
endif()

qt_add_qml_module(
  ${PROJECT_NAME}
    URI unixcmd
    VERSION 1.0
    QML_FILES
  qml/about_dialog.qml
  qml/copy_dialog.qml
  qml/create_directory_dialog.qml
  qml/file_processing_dialog.qml
)

target_link_libraries(
  ${PROJECT_NAME}
    PUBLIC
      Qt6::Core
      Qt6::Gui
      Qt6::Qml
      Qt6::Quick
      Qt6::QuickControls2
      Qt6::Widgets
)

include(cmake/ios.cmake)
if(APPLE)
  include(cmake/macos.cmake)
endif()

install(CODE "include(${CMAKE_SOURCE_DIR}/src/cmake/install.cmake)")
install(CODE "include(${CMAKE_SOURCE_DIR}/src/cmake/utils.cmake)")
install(CODE "set(CONAN_CACHE_FOLDER \"${CONAN_CACHE_FOLDER}\")")

if(LINUX)
  install(CODE [[
    set(TARGETS $<TARGET_FILE:unixcmd> $<TARGET_FILE:Qt6::QXcbIntegrationPlugin>)
    set(BUNDLE_LIB_RELATIVE_DIR "lib")
    set(BUNDLE_PLATFORMS_RELATIVE_DIR "plugins/platforms")
  ]])
elseif(MACOS)
  set(BUNDLE_OPTIONS BUNDLE DESTINATION .)
  install(CODE [[
    set(MACOS TRUE)
    set(TARGETS $<TARGET_FILE:unixcmd> $<TARGET_FILE:Qt6::QCocoaIntegrationPlugin>)
    set(BUNDLE_CONTENTS_RELATIVE_DIR "unixcmd.app/Contents")
    set(BUNDLE_LIB_RELATIVE_DIR "${BUNDLE_CONTENTS_RELATIVE_DIR}/Frameworks")
    set(BUNDLE_PLATFORMS_RELATIVE_DIR "${BUNDLE_CONTENTS_RELATIVE_DIR}/PlugIns/platforms")
  ]])
elseif(WIN32)
  set(BUNDLE_OPTIONS DESTINATION .)
  install(CODE [[
    set(TARGETS $<TARGET_FILE:unixcmd> $<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>)
    set(BUNDLE_LIB_RELATIVE_DIR "")
    set(BUNDLE_PLATFORMS_RELATIVE_DIR "platforms")
  ]])
else()
  message(WARNING "Unsupported platform")
endif()

install(TARGETS ${PROJECT_NAME} ${BUNDLE_OPTIONS})

install(CODE [[
  file(GET_RUNTIME_DEPENDENCIES
    RESOLVED_DEPENDENCIES_VAR APP_LIBRARIES
    UNRESOLVED_DEPENDENCIES_VAR APP_LIBRARIES_UNRES
    CONFLICTING_DEPENDENCIES_PREFIX APP_LIBRARIES_CONFLICTING_DEPENDENCIES
    EXECUTABLES ${TARGETS}
  )

  foreach(LIBRARY ${APP_LIBRARIES})
    check_starts_with(LIBRARY "${CONAN_CACHE_FOLDER}" IS_CONAN_LIBRARY)
    if(NOT IS_CONAN_LIBRARY)
      check_starts_with(LIBRARY "${CMAKE_BINARY_DIR}" IS_LOCAL_LIBRARY)

      if(NOT IS_LOCAL_LIBRARY)
        continue()
      endif()
    endif()

    install_library(${LIBRARY} ${CMAKE_INSTALL_PREFIX}/${BUNDLE_LIB_RELATIVE_DIR})
  endforeach()
]])

if(LINUX)
  install(CODE [[
    install_platform_plugin_file($<TARGET_FILE:Qt6::QXcbIntegrationPlugin>
      ${CMAKE_INSTALL_PREFIX}/${BUNDLE_PLATFORMS_RELATIVE_DIR})
  ]])
elseif(MACOS)
  install(CODE [[
    install_platform_plugin_file($<TARGET_FILE:Qt6::QCocoaIntegrationPlugin>
      ${CMAKE_INSTALL_PREFIX}/${BUNDLE_PLATFORMS_RELATIVE_DIR})
  ]])
elseif(WIN32)
  install(CODE [[
    install_platform_plugin_file($<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>
      ${CMAKE_INSTALL_PREFIX}/${BUNDLE_PLATFORMS_RELATIVE_DIR})
  ]])
endif()
