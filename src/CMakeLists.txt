if(ANDROID)
  qt_add_library(${PROJECT_NAME} SHARED)
else()
  qt_add_executable(
    ${PROJECT_NAME}
      copy_dialog.cpp
      copy_dialog.h
      directory_widget.cpp
      directory_widget.h
      double_panel_splitter.cpp
      double_panel_splitter.h
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
  qml/copy_dialog.qml
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