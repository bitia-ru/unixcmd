# Linux-specific deployment configuration using linuxdeploy
# This replaces the broken Qt deployment tools for Linux

set(LINUXDEPLOY_TOOL "${CMAKE_SOURCE_DIR}/tools/linuxdeploy-x86_64.AppImage")
set(LINUXDEPLOY_QT_PLUGIN "${CMAKE_SOURCE_DIR}/tools/linuxdeploy-plugin-qt-x86_64.AppImage")

# Create a deployment script for Linux
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/linux_deploy.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/linux_deploy.cmake"
    @ONLY
)

# Set the deployment script variable for Linux
set(deploy_script "${CMAKE_CURRENT_BINARY_DIR}/linux_deploy.cmake")

# Add a custom target to run the deployment
add_custom_target(deploy_linux
    COMMAND ${CMAKE_COMMAND} -P "${deploy_script}"
    DEPENDS ${PROJECT_NAME}
    COMMENT "Creating Linux deployment package"
)