#include "main_window.h"

#include "copy_dialog.h"
#include "directory_widget.h"
#include "double_panel_splitter.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMessageBox>
#include <QProcess>
#include <QSplitter>
#include <QTableWidgetItem>


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

    connect(d->leftPanel, &DirectoryWidget::focusIn, [this] { setActivePanel(LEFT); });
    connect(d->rightPanel, &DirectoryWidget::focusIn, [this] { setActivePanel(RIGHT); });
    connect(d->leftPanel, &DirectoryWidget::fileTriggered, this, &MainWindow::open);
    connect(d->rightPanel, &DirectoryWidget::fileTriggered, this, &MainWindow::open);

    resize(1024, 600);

    setActivePanel(m_activePanel);
}

MainWindow::~MainWindow() = default;

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        const auto keyEvent = dynamic_cast<QKeyEvent*>(event);

        switch (keyEvent->key()) {
        case Qt::Key_Tab:
            toggleActivePanel();
            return true;
        case Qt::Key_F2:
            activePanelWidget()->reload();
            return true;
        case Qt::Key_F3:
            viewSelection();
            return true;
        case Qt::Key_F4:
            editSelection();
            return true;
        case Qt::Key_F5:
            copySelection();
            return true;
        case Qt::Key_F8:
            if (keyEvent->modifiers() & Qt::ShiftModifier)
                removeSelected();
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

DirectoryWidget* MainWindow::activePanelWidget() const {
    return m_activePanel == LEFT ? d->leftPanel : d->rightPanel;
}

void MainWindow::toggleActivePanel() {
    setActivePanel(m_activePanel == LEFT ? RIGHT : LEFT);
}

void MainWindow::viewSelection() {
    QList<QFileInfo> selectedFiles = this->selectedFiles();

    if (selectedFiles.isEmpty())
        return;

    auto viewProcess = new QProcess(this);

    QStringList files;

    for (const auto& item : selectedFiles) {
        files << item.absoluteFilePath();
    }

    const auto command = "less " + files.join(' ');

    viewProcess->start(
        "/usr/bin/qlmanage",
        QStringList{"-p"} << files
    );
}

void MainWindow::editSelection() {
    QList<QFileInfo> selectedFiles = this->selectedFiles();

    if (selectedFiles.isEmpty())
        return;

    auto viewProcess = new QProcess(this);

    QStringList files;

    for (const auto& item : selectedFiles) {
        files << item.absoluteFilePath();
    }

    const auto command = "vim " + files.join(' ');

    viewProcess->start(
        "/usr/bin/osascript",
        QStringList{
            "-e", "tell application \"iTerm\" to activate",
            "-e", "tell application \"System Events\" to keystroke \"t\" using command down",
            "-e", QString("tell application \"iTerm\" to tell current session of current window to write text \"%1\"").arg(command),
        }
    );
}

void MainWindow::copySelection() {
    QList<QFileInfo> selectedFiles = this->selectedFiles();

    if (selectedFiles.isEmpty())
        return;

    auto copyDialog = new CopyDialog(this);
}

void MainWindow::removeSelected() {
    QList<QFileInfo> selectedFiles = this->selectedFiles();

    if (selectedFiles.isEmpty())
        return;

    const auto message = QString("Are you sure you want to delete %1?").arg(
        selectedFiles.size() == 1
            ? QString("file '%1'").arg(selectedFiles.first().fileName())
            : QString("%1 files").arg(selectedFiles.count())
    );

    const auto response = QMessageBox::question(this, "Deleting files", message, QMessageBox::Yes | QMessageBox::No);

    const auto removeFile = [this](const QFileInfo& fileInfo) -> bool {
        QFile file(fileInfo.absoluteFilePath());
        if (!file.remove()) {
            const auto errorMsgBoxResponse = QMessageBox::critical(
                this,
                "Error deleting file",
                QString("Failed to delete file '%1'").arg(fileInfo.fileName()),
                QMessageBox::Ignore | QMessageBox::Abort
            );

            if (errorMsgBoxResponse == QMessageBox::Abort)
                return false;
        }

        return true;
    };

    if (response == QMessageBox::Yes) {
        for (const auto& fileInfo : selectedFiles) {
            if (fileInfo.isFile() || fileInfo.isSymLink()) {
                if (!removeFile(fileInfo))
                    break;
            } else if (fileInfo.isDir() || fileInfo.isBundle()) {
                QDir dir(fileInfo.absoluteFilePath());
                if (!dir.removeRecursively()) {
                    const auto errorMsgBoxResponse = QMessageBox::critical(
                        this,
                        "Error deleting directory",
                        QString("Failed to delete directory '%1'").arg(fileInfo.fileName()),
                        QMessageBox::Ignore | QMessageBox::Abort
                    );

                    if (errorMsgBoxResponse == QMessageBox::Abort)
                        break;
                }
            }
        }

        activePanelWidget()->reload();
    }
}

void MainWindow::open(const QFileInfo& fileInfo) {
    const auto filePath = fileInfo.absoluteFilePath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

QList<QFileInfo> MainWindow::selectedFiles() const {
    QList<QFileInfo> files;

    for (const auto& index : activePanelWidget()->selectionModel()->selectedRows())
        files.append(index.data(Qt::UserRole).value<QFileInfo>());

    return files;
}