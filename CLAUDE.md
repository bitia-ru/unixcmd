# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

UnixCMD is a cross-platform double-panel file manager inspired by Total Commander, built with
Qt6 and QML. It runs on Windows, Linux, and macOS.

## Build System

The project uses CMake with Conan for dependency management and Qt6 for the UI framework.

### Common Commands

```bash
# Build the project (requires Conan cache to be set up)
mkdir cmake-build-debug && cd cmake-build-debug
conan install ..
cmake --build .

# Run the application
./src/unixcmd.app/Contents/MacOS/unixcmd  # macOS
./src/unixcmd  # Linux/Windows

# Create deployment package (platform-specific)
cmake --build . --target install
cmake --build . --target deploy_linux  # Linux only
```

## Architecture

### Core Components

1. **Main Window** (`main_window.h/cpp`) - Application entry point and window management
2. **Directory Panels** - Dual-panel file browser interface:
   - `directory_widget.h/cpp` - Panel container
   - `directory_view.h/cpp` - File list view
   - `directory_model.h/cpp` - File system data model
   - `double_panel_splitter.h/cpp` - Panel layout management
3. **Dialogs** - User interaction components:
   - `move_copy_dialog.h/cpp` - File operations
   - `create_directory_dialog.h/cpp` - Directory creation
   - `file_processing_dialog.h/cpp` - Progress dialogs
   - `about_dialog.h/cpp` - Application info
4. **QML UI** - Modern Qt Quick interface in `src/qml/`:
   - Dialog implementations using QML for better UI flexibility
   - Registered as Qt QML module

### Key Design Patterns

- **Model-View Architecture**: Separates file system logic (`directory_model`) from 
  presentation (`directory_view`)
- **QML Integration**: Dialogs use QML for modern, flexible UI while core components remain 
  in C++
- **Platform Abstraction**: CMake handles platform-specific deployment and packaging

### Platform-Specific Implementation

- **macOS**: Uses native `.app` bundle structure with icon resources
- **Linux**: Deploys as AppImage using linuxdeploy tools
- **Windows**: Standard executable with Qt deployment

### Dependencies

- Qt 6.4.0 (via Conan)
- C++23 standard
- Platform-specific deployment tools (linuxdeploy for Linux)

## Coding Standards

- Maximum line length: 100 characters
- C++23 standard
- Qt coding conventions for naming and structure

## Development Notes

- The project is currently in development and not recommended for production use
- No test framework is currently implemented
- Linux deployment uses custom linuxdeploy integration due to broken Qt deployment tools