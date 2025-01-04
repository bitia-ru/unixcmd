#include "directory_widget.h"

#include <QDir>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMessageBox>


namespace {

QString fileNameByEntry(const QFileInfo& entry)
{
    if (entry.fileName() == "..")
        return "[..]";

    if (entry.isDir())
        return entry.fileName();

    return entry.baseName();
}

QTableWidgetItem* fileSizeItemByEntry(const QFileInfo& entry)
{
    auto item = new QTableWidgetItem;

    if (entry.isDir()) {
        item->setText("<DIR>");
        item->setTextAlignment(Qt::AlignCenter);
    } else {
        const auto size = entry.size();
        constexpr auto G = 1024*1024*1024;

        const auto text = size > G
            ? QString("%1G").arg(static_cast<double>(size) / G, 0, 'f', 2)
            : QLocale::system().toString(size);

        item->setText(text);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

    return item;
}

QTableWidgetItem* attrItemByEntry(const QFileInfo& entry)
{
    QString attr;

    if (entry.isDir())
        attr += "d";
    else
        attr += "-";

    if (entry.isReadable())
        attr += "r";
    else
        attr += "-";

    if (entry.isWritable())
        attr += "w";
    else
        attr += "-";

    if (entry.isExecutable())
        attr += "x";
    else
        attr += "-";

    return new QTableWidgetItem(attr);
}

}

DirectoryWidget::DirectoryWidget(QWidget* parent) : QTableWidget(parent)
{
    setEditTriggers(NoEditTriggers);
    setSelectionBehavior(SelectRows);

    setColumnCount(5);
    setHorizontalHeaderLabels({"Name", "Ext", "Size", "Date", "Attr"});
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->resizeSection(1, 60);
    horizontalHeader()->resizeSection(2, 90);
    horizontalHeader()->resizeSection(3, 110);
    horizontalHeader()->resizeSection(4, 50);

    verticalHeader()->setVisible(false);
    verticalHeader()->setDefaultSectionSize(14);

    setDirectory(QDir::currentPath());

    const auto onCellEntered = [this](const QTableWidgetItem* item) {
        const auto fileNameItem = this->item(item->row(), 0);

        const QString fileName = fileNameItem->text() == "[..]" ? ".." : fileNameItem->text();

        if (const auto path = QDir(m_directory).filePath(fileName); QFileInfo(path).isDir()) {
            if (!QDir(path).isReadable()) {
                QMessageBox::critical(this, "Error", "Can't open directory");
                return;
            }

            setDirectory(path);
        }
    };

    connect(this, &QTableWidget::itemActivated, onCellEntered);
    connect(this, &QTableWidget::itemDoubleClicked, onCellEntered);
}

void DirectoryWidget::setDirectory(const QString& directory)
{
    setRowCount(0);

    const QDir dir(QDir::cleanPath(directory));

    if (!dir.isReadable())
        return;

    m_directory = dir.absolutePath();

    if (!dir.isRoot()) {
        insertRow(0);

        setItem(0, 0, new QTableWidgetItem("[..]"));
    }

    for (const auto& entry : dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot))
    {
        const auto iRow = rowCount();

        insertRow(iRow);

        auto fileEntry = new QTableWidgetItem(fileNameByEntry(entry));

        QFileIconProvider iconProvider;
        QIcon icon = iconProvider.icon(entry);

        if (!icon.isNull())
            fileEntry->setIcon(icon);

        setItem(iRow, 0, fileEntry);

        auto extItem = new QTableWidgetItem(entry.suffix());
        extItem->setTextAlignment(Qt::AlignCenter);
        setItem(iRow, 1, extItem);

        setItem(iRow, 2, fileSizeItemByEntry(entry));

        auto dateItem = new QTableWidgetItem(entry.lastModified().toString("dd/MM/yy hh:mm"));
        dateItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        setItem(iRow, 3, dateItem);

        setItem(iRow, 4, attrItemByEntry(entry));
    }

    if (rowCount() > 0)
        selectRow(0);
}

void DirectoryWidget::reload()
{
    setDirectory(m_directory);
}

void DirectoryWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit itemActivated(currentItem());
        return;
    }

    QTableWidget::keyPressEvent(event);
}

void DirectoryWidget::focusInEvent(QFocusEvent* event)
{
    QTableWidget::focusInEvent(event);

    emit focusIn();
}