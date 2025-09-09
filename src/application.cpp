#include "application.h"

#ifdef Q_OS_WIN
    #include <windows.h>
    #include <io.h>
#else
    #include <unistd.h>
#endif

Application::Application(int& argc, char** argv) : QApplication(argc, argv) {
}

bool Application::shouldStartFromUserHome() {
    return !isRunningFromTerminal();
}

bool Application::isRunningFromTerminal() {
#ifdef Q_OS_WIN
    // Windows: Check if we have a console attached
    return _isatty(_fileno(stdout)) && GetConsoleWindow() != nullptr;
#else
    // Unix/Linux/macOS: Check if stdout is a terminal
    return isatty(STDOUT_FILENO);
#endif
}
