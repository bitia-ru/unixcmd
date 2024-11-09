#include "directory_widget.h"

#include <QDir>


DirectoryWidget::DirectoryWidget(QWidget* parent) : QListWidget(parent)
{
    setIconSize(QSize(100, 100));
    setResizeMode(QListView::Adjust);
    setSpacing(1);

    setDirectory(QDir::currentPath());

    connect(this, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        auto path = QDir(m_directory).filePath(item->text());

        if (QFileInfo(path).isDir()) {
            setDirectory(path);
        }
    });
}

void DirectoryWidget::setDirectory(const QString& directory)
{
    clear();

    if (!QDir(directory).isReadable()) {
        return;
    }

    m_directory = directory;

    QDir dir(directory);

    for (const auto& entry : dir.entryInfoList(QDir::Filter::AllEntries))
    {
        if (entry.fileName() == ".")
            continue;

        new QListWidgetItem(entry.fileName(), this);
    }
}