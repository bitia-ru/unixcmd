#include "main_window.h"
#include "directory_widget.h"


MainWindow::MainWindow() {
    setWindowTitle("Hello, World!");

    auto widget = new DirectoryWidget(this);

    setCentralWidget(widget);
}