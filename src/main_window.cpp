#include "main_window.h"
#include "directory_widget.h"
#include "double_panel_splitter.h"

#include <QSplitter>


MainWindow::MainWindow() {
    setWindowTitle("UnixCMD");

    auto splitter = new DoublePanelSplitter(this);

    setCentralWidget(splitter);

    splitter->addWidget(new DirectoryWidget(splitter));
    splitter->addWidget(new DirectoryWidget(splitter));

    resize(800, 600);
}