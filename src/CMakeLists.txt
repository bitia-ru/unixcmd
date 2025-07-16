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
      functions/copy_move/dialog.cpp
      functions/copy_move/dialog.h
      functions/create_directory/dialog.cpp
      functions/create_directory/dialog.h
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
      functions/common/file_processing_dialog.cpp
      functions/common/file_processing_dialog.h
      main.cpp
      main_window.cpp
      main_window.h
    )
endif()

set_target_properties(
  ${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

qt_add_qml_module(
  ${PROJECT_NAME}
    URI unixcmd
    VERSION 1.0
    QML_FILES
      qml/about_dialog.qml
      qml/move_copy_dialog.qml
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

if(APPLE)
  include(cmake/macos.cmake)
endif()

include(cmake/distribution.cmake)

install(
  TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Platform-specific deployment
if(APPLE)
  qt_generate_deploy_qml_app_script(
    TARGET ${PROJECT_NAME}
    MACOS_BUNDLE_POST_BUILD
    FILENAME_VARIABLE deploy_script
  )
  install(SCRIPT ${deploy_script})
elseif(UNIX AND NOT APPLE)
  include(cmake/linux.cmake)
  install(SCRIPT ${deploy_script})
endif()

install_license()