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

install(TARGETS ${PROJECT_NAME})

install(CODE [[
  file(GET_RUNTIME_DEPENDENCIES
    RESOLVED_DEPENDENCIES_VAR APP_LIBRARIES
    UNRESOLVED_DEPENDENCIES_VAR APP_LIBRARIES_UNRES
    CONFLICTING_DEPENDENCIES_PREFIX APP_LIBRARIES_CONFLICTING_DEPENDENCIES
    EXECUTABLES $<TARGET_FILE:unixcmd>
  )

  foreach(LIBRARY ${APP_LIBRARIES})
    if(NOT IS_DIRECTORY ${LIBRARY} AND NOT LIBRARY MATCHES "^/lib.*")
      file(INSTALL ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib FOLLOW_SYMLINK_CHAIN)
    endif()
  endforeach()

  message(WARNING "Resolved dependencies: ${RES}")
]])
