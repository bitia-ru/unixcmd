set_target_properties(
  ${PROJECT_NAME} PROPERTIES
  MACOSX_BUNDLE_BUNDLE_NAME "unixcmd"
  MAIN_NIBFILE "unixcmd"
  MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist.in
)