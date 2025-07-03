# Linux Deployment with linuxdeploy

This document describes the implementation of Linux deployment using linuxdeploy to replace the broken Qt deployment tools.

## Problem

The original issue was that "distribution building for Linux is broken" and needed to be fixed with "linuxqtdeploy". The problem was that Qt's native deployment tools (`qt_generate_deploy_qml_app_script`) don't work properly on Linux.

## Solution

The solution replaces Qt's native deployment tools with linuxdeploy, which is a third-party tool specifically designed for Linux Qt application deployment.

### Key Components

1. **linuxdeploy**: Main deployment tool that creates AppImage packages
2. **linuxdeploy-plugin-qt**: Qt-specific plugin that handles Qt library bundling
3. **Platform-specific CMake configuration**: Separates macOS and Linux deployment logic

### Implementation Details

#### CMake Changes

1. **src/CMakeLists.txt**: Modified to use platform-specific deployment
   - macOS: Uses Qt's native `qt_generate_deploy_qml_app_script`
   - Linux: Uses custom linuxdeploy-based deployment

2. **src/cmake/linux.cmake**: Linux-specific deployment configuration
   - Sets up linuxdeploy tool paths
   - Configures deployment script template
   - Creates custom deployment target

3. **src/cmake/linux_deploy.cmake.in**: Deployment script template
   - Handles executable discovery
   - Creates proper AppDir structure
   - Generates desktop file with metadata
   - Runs linuxdeploy with Qt plugin
   - Creates AppImage package

#### CI/CD Changes

1. **GitHub Actions Workflow**: Updated to support linuxdeploy
   - Downloads linuxdeploy tools during build
   - Runs AppImage creation after package generation
   - Integrates with existing packaging workflow

### Usage

The deployment system is automatically triggered during the build process:

1. During build: CMake detects Linux platform and sets up deployment
2. During package: linuxdeploy creates AppImage with all Qt dependencies
3. During CI: AppImage is included in distribution package

### Features

- **Automatic Qt dependency detection**: linuxdeploy-plugin-qt handles Qt libraries
- **AppImage creation**: Creates portable Linux executables
- **Desktop integration**: Generates proper .desktop files
- **Icon handling**: Supports application icons
- **Error handling**: Robust error detection and reporting

### Testing

The implementation has been tested with:
- Tool availability and version checking
- Qt plugin detection
- Basic AppImage creation workflow
- CI integration compatibility

### Benefits

1. **Fixes broken Linux deployment**: Replaces non-functional Qt tools
2. **Creates portable packages**: AppImage format works across Linux distributions
3. **Maintains CI compatibility**: Integrates with existing build pipeline
4. **Reduces maintenance**: Uses well-maintained external tools
5. **Improves user experience**: Creates proper desktop-integrated applications

## Files Modified

- `src/CMakeLists.txt` - Platform-specific deployment logic
- `src/cmake/linux.cmake` - Linux deployment configuration
- `src/cmake/linux_deploy.cmake.in` - Deployment script template
- `.github/workflows/build.yml` - CI workflow updates
- `.gitignore` - Exclude build artifacts

## Dependencies

- linuxdeploy (downloaded during build)
- linuxdeploy-plugin-qt (downloaded during build)
- Qt6 development libraries (existing requirement)

The solution successfully addresses the original issue by providing a robust, maintainable Linux deployment system using industry-standard tools.