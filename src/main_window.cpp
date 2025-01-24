#include "main_window.h"

#include "copy_dialog.h"
#include "create_directory_dialog.h"
#include "directory_widget.h"
#include "double_panel_splitter.h"
#include "file_processing_dialog.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QKeyEvent>
#include <QMessageBox>
#include <QProcess>
#include <QQuickWindow>
#include <QSplitter>
#include <QTableWidgetItem>
#include <QtConcurrent/QtConcurrentRun>


struct MainWindow::Private
{
    DirectoryWidget* leftPanel = nullptr;
    DirectoryWidget* rightPanel = nullptr;
    bool showHiddenFiles = false;
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
        case Qt::Key_F7:
            createDirectory();
            return true;
        case Qt::Key_F8:
            if (keyEvent->modifiers() & Qt::ShiftModifier)
                removeSelected();
            return true;
        case Qt::Key_Period:
            if (keyEvent->modifiers() & Qt::AltModifier) {
                toggleShowHiddenFiles();
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    emit closed();

    event->accept();
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

DirectoryWidget* MainWindow::destinationPanelWidget() const {
    return m_activePanel == LEFT ? d->rightPanel : d->leftPanel;
}

void MainWindow::toggleActivePanel() {
    setActivePanel(m_activePanel == LEFT ? RIGHT : LEFT);
}

void MainWindow::viewSelection() {
    QList<QFileInfo> selectedFiles = this->selectedFiles();

    if (selectedFiles.isEmpty())
        return;

    const auto viewProcess = new QProcess(this);

    QString command;

    QStringList files;

    for (const auto& item : selectedFiles) {
        files << item.absoluteFilePath();
    }

    if (QSysInfo::productType() == "macos") {
        viewProcess->setProgram("/usr/sbin/qlmanage");
        viewProcess->setArguments(QStringList{"-p",} + files);
    } else if (QSysInfo::productType() == "ubuntu") {
        viewProcess->setProgram("/usr/bin/xdg-open");
        viewProcess->setArguments(files);
    }

    connect(
        viewProcess,
        QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [viewProcess](int, const QProcess::ExitStatus exitStatus) {
            if (exitStatus == QProcess::CrashExit) {
                QMessageBox::critical(nullptr, "Error", "Failed to view file(s)");
            }

            viewProcess->deleteLater();
        }
    );

    connect(
        viewProcess,
        &QProcess::errorOccurred,
        [viewProcess](QProcess::ProcessError error) {
            QMessageBox::critical(nullptr, "Error", QString("Failed to view file(s): %1").arg(viewProcess->errorString()));
            viewProcess->deleteLater();
        }
    );

    viewProcess->start();
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

    auto copyDialog = new CopyDialog(this, destinationPanelWidget()->directory());
    auto fileProcessingDialog = new FileProcessingDialog(this, "Copying files");

    auto aborted = new std::atomic(false);
    auto watcher = new QFutureWatcher<void>(this);

    connect(fileProcessingDialog, &FileProcessingDialog::aborted, [aborted] { *aborted = true; });

    connect(watcher, &QFutureWatcher<void>::finished, [fileProcessingDialog, watcher] {
        fileProcessingDialog->abort();
        fileProcessingDialog->deleteLater();
        watcher->deleteLater();
    });

    connect(copyDialog, &CopyDialog::closed, [this, copyDialog] {
        copyDialog->deleteLater();
    });

    connect(copyDialog, &CopyDialog::rejected, [this, fileProcessingDialog, watcher] {
        fileProcessingDialog->abort();
        fileProcessingDialog->deleteLater();
        watcher->waitForFinished();
    });

    connect(copyDialog, &CopyDialog::accepted, [this, selectedFiles, fileProcessingDialog, aborted, watcher](const QDir& destination) {
        const auto future = QtConcurrent::run([this, destination, selectedFiles, fileProcessingDialog, aborted]() -> void {
            QMetaObject::invokeMethod(fileProcessingDialog, [fileProcessingDialog] { fileProcessingDialog->show(); });

            const std::function<bool(const QFileInfo&, const QDir&)> copyFileRecursive =
                [this, &copyFileRecursive, fileProcessingDialog, aborted](const QFileInfo& file, const QDir& destination) -> bool {
                    if (file.isFile() || file.isSymLink()) {
                        const auto destinationFilePath = destination.filePath(file.fileName());

                        QMetaObject::invokeMethod(fileProcessingDialog, [fileProcessingDialog, file] {
                            fileProcessingDialog->setStatus(QString("Copying file '%1'").arg(file.fileName()));
                        });

                        if (!QFile::copy(file.absoluteFilePath(), destinationFilePath)) {
                            const auto selectedButton = QMessageBox::question(
                                this,
                                "Error copying file",
                                QString("Failed to copy file '%1'").arg(file.fileName()),
                                QMessageBox::Ignore | QMessageBox::Abort,
                                QMessageBox::Abort
                            );

                            if (selectedButton == QMessageBox::Abort)
                                return true;
                        }
                    } else if (file.isDir() || file.isBundle()) {
                        const auto& dirFiles = QDir(file.absoluteFilePath())
                            .entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
                        const auto& subDir = QDir(destination.filePath(file.fileName()));
                        if (!subDir.exists()) {
                            if (!subDir.mkpath(".")) {
                                qDebug() << "Failed to create directory: " << subDir.absolutePath();
                                return true;
                            }
                        }
                        for (const auto& subFile: dirFiles)
                            if (copyFileRecursive(subFile, subDir))
                                return true;
                    } else {
                        qDebug() << "Unsupported file type: " << file.absoluteFilePath() << ", ignoring";
                    }

                    if (*aborted)
                        return true;

                    return false;
                };

                for (const auto& file: selectedFiles)
                    if (copyFileRecursive(file, destination))
                        break;

                if (destination == destinationPanelWidget()->directory())
                    destinationPanelWidget()->reload();

                if (activePanelWidget()->directory() == destinationPanelWidget()->directory())
                    activePanelWidget()->reload();
            }
        );

        watcher->setFuture(future);
    });
}

void MainWindow::createDirectory()
{
    auto createDirectoryDialog = new CreateDirectoryDialog(this);

    connect(createDirectoryDialog, &CreateDirectoryDialog::closed, [this, createDirectoryDialog] {
        createDirectoryDialog->deleteLater();
    });

    connect(createDirectoryDialog, &CreateDirectoryDialog::accepted, [this](const QString& directoryName) {
        if (const QDir dir(activePanelWidget()->directory()); !dir.mkdir(directoryName)) {
            QMessageBox::critical(
                this,
                "Error creating directory",
                QString("Failed to create directory '%1'").arg(directoryName)
            );
        } else {
            activePanelWidget()->reload();
            if (activePanelWidget()->directory() == destinationPanelWidget()->directory())
                destinationPanelWidget()->reload();
        }
    });
}

void MainWindow::removeSelected()
{
    QList<QFileInfo> selectedFiles = this->selectedFiles();

    if (selectedFiles.isEmpty())
        return;

    const auto message = QString("Are you sure you want to delete %1?").arg(
        selectedFiles.size() == 1
            ? QString("file '%1'").arg(selectedFiles.first().fileName())
            : QString("%1 files").arg(selectedFiles.count())
    );

    const auto response = QMessageBox::question(
        this,
        "Deleting files",
        message,
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes);

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
        if (activePanelWidget()->directory() == destinationPanelWidget()->directory())
            destinationPanelWidget()->reload();
    }
}

void MainWindow::open(const QFileInfo& fileInfo) {
    const auto filePath = fileInfo.absoluteFilePath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void MainWindow::toggleShowHiddenFiles() {
    d->showHiddenFiles = !d->showHiddenFiles;

    d->leftPanel->setShowHiddenFiles(d->showHiddenFiles);
    d->rightPanel->setShowHiddenFiles(d->showHiddenFiles);
}

QList<QFileInfo> MainWindow::selectedFiles() const {
    QList<QFileInfo> files;

    for (const auto& index : activePanelWidget()->selectionModel()->selectedRows())
        files.append(index.data(Qt::UserRole).value<QFileInfo>());

    return files;
}