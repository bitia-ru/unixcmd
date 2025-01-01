#include "main_window.h"
#include "directory_widget.h"
#include "double_panel_splitter.h"

#include <QSplitter>
#include <QMessageBox>
#include <QKeyEvent>
#include <QApplication>


struct MainWindow::Private
{
    DirectoryWidget* leftPanel = nullptr;
    DirectoryWidget* rightPanel = nullptr;
};

MainWindow::MainWindow()
    : d(new Private)
{
    setWindowTitle("UnixCMD");

    qApp->installEventFilter(this);

    auto splitter = new DoublePanelSplitter(this);

    setCentralWidget(splitter);

    splitter->addWidget(d->leftPanel = new DirectoryWidget(splitter));
    splitter->addWidget(d->rightPanel = new DirectoryWidget(splitter));

    resize(1024, 600);

    setActivePanel(m_activePanel);
}

MainWindow::~MainWindow() = default;

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        if (const auto keyEvent = dynamic_cast<QKeyEvent*>(event); keyEvent->key() == Qt::Key_Tab) {
            toggleActivePanel();
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::setActivePanel(ActivePanel panel) {
    m_activePanel = panel;

    if (panel == LEFT) {
        d->leftPanel->setFocus();
    } else {
        d->rightPanel->setFocus();
    }
}

void MainWindow::toggleActivePanel() {
    setActivePanel(m_activePanel == LEFT ? RIGHT : LEFT);
}