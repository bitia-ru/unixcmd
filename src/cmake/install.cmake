function(install_library LIBRARY_PATH DESTINATION_DIR)
  if(LINUX)
    file(INSTALL ${LIBRARY_PATH} DESTINATION ${DESTINATION_DIR} FOLLOW_SYMLINK_CHAIN)

    return()
  endif()

  if(WIN32)
    file(INSTALL ${LIBRARY_PATH} DESTINATION ${DESTINATION_DIR})

    return()
  endif()

  extract_framework_path(LIBRARY_PATH FRAMEWORK_NAME LIBRARY_RELATIVE_PATH)

  get_filename_component(LIBRARY_RELATIVE_DIR ${LIBRARY_RELATIVE_PATH} DIRECTORY)

  file(INSTALL ${LIBRARY_PATH} DESTINATION ${DESTINATION_DIR}/${FRAMEWORK_NAME}/${LIBRARY_RELATIVE_DIR})
endfunction()

function(extract_framework_path LIBRARY_PATH_VAR FRAMEWORK_NAME LIBRARY_RELATIVE_PATH)
  set(_input_path "${${LIBRARY_PATH_VAR}}")
  cmake_path(NORMAL_PATH _input_path)

  set(_regex "^(.*)/([^/]+\\.framework)/(.*)$")

  string(REGEX REPLACE "${_regex}" "\\2" _result_frameworkname "${_input_path}")
  string(REGEX REPLACE "${_regex}" "\\3" _result_lib_relative_path "${_input_path}")

  set(${FRAMEWORK_NAME} "${_result_frameworkname}" PARENT_SCOPE)
  set(${LIBRARY_RELATIVE_PATH} "${_result_lib_relative_path}" PARENT_SCOPE)
endfunction()

function(install_platform_plugin_file PLUGIN_FILE_PATH BUNDLE_PLATFORMS_DIR)
    file(INSTALL ${PLUGIN_FILE_PATH} DESTINATION ${BUNDLE_PLATFORMS_DIR})
endfunction()
