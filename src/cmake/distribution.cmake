function(install_license)
  if(APPLE)
    install(
      FILES ${CMAKE_SOURCE_DIR}/LICENSE
      DESTINATION "${CMAKE_INSTALL_BINDIR}/../${PROJECT_NAME}.app/Contents/Resources"
    )
  else()
    install(
      FILES ${CMAKE_SOURCE_DIR}/LICENSE
      DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
  endif()
endfunction()