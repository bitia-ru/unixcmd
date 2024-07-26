find_package(PkgConfig REQUIRED)
pkg_check_modules(gtk REQUIRED IMPORTED_TARGET gtk4)

add_executable(
  ${PROJECT_NAME}
    main.c
    main_window.c
    main_window.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::gtk)
