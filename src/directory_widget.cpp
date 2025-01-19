#include "directory_widget.h"

#include <QDir>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMetaProperty>


namespace {

QString fileNameByEntry(const QFileInfo& entry)
{
    if (entry.fileName() == "..")
        return "[..]";

    if (entry.isDir())
        return entry.fileName();

    return entry.baseName();
}

QStandardItem* fileSizeItemByEntry(const QFileInfo& entry)
{
    auto item = new QStandardItem;
    item->setEditable(false);

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

QStandardItem* attrItemByEntry(const QFileInfo& entry)
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

    auto item = new QStandardItem(attr);
    item->setEditable(false);

    return item;
}

}

QVariant DirectoryWidgetModel::data(const QModelIndex &index, const int role/* = Qt::DisplayRole*/) const
{
    if (index.column() == 0 && role == Qt::EditRole) {
        const auto fileInfo = QStandardItemModel::data(index, Qt::UserRole).value<QFileInfo>();

        return fileInfo.fileName();
    }

    return QStandardItemModel::data(index, role);
}

bool DirectoryWidgetModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.column() == 0 && role == Qt::EditRole) {
        const auto newFileName = value.toString();
        const auto originalFileInfo = QStandardItemModel::data(index, Qt::UserRole).value<QFileInfo>();
        const QFileInfo newFileInfo(originalFileInfo.absoluteDir().filePath(newFileName));

        if (originalFileInfo.dir().rename(originalFileInfo.fileName(), newFileName)) {
            QStandardItemModel::setData(index, QVariant::fromValue(newFileInfo), Qt::UserRole);

            if (newFileInfo.isDir())
                QStandardItemModel::setData(index, newFileInfo.fileName(), Qt::DisplayRole);
            else {
                QStandardItemModel::setData(index, newFileInfo.baseName(), Qt::DisplayRole);
                QStandardItemModel::setData(
                    DirectoryWidgetModel::index(index.row(), 1),
                    newFileInfo.suffix(),
                    Qt::DisplayRole
                );
            }

            return true;
        }

        QMessageBox::critical(nullptr, "Error", "Can't rename file");

        return false;
    }

    return QStandardItemModel::setData(index, value, role);
}

bool DirectoryWidgetModel::setDirectory(const QDir& dir)
{
    removeRows(0, rowCount());

    if (!dir.exists())
        return false;

    if (!dir.isRoot()) {
        const auto item = new QStandardItem("[..]");
        const auto parentDirPath = QDir::cleanPath(QFileInfo(dir.absolutePath()).dir().absolutePath());
        item->setData(QVariant::fromValue(QFileInfo(parentDirPath)), Qt::UserRole);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setIcon(QFileIconProvider().icon(QAbstractFileIconProvider::Folder));

        appendRow({item});
    }

    // setRowCount(dir.isRoot() ? entries.size() : entries.size() + 1);

    for (const auto& entry : dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot))
    {
        QList<QStandardItem*> items;

        const auto fileEntry = new QStandardItem(fileNameByEntry(entry));
        fileEntry->setData(QVariant::fromValue(entry), Qt::UserRole);
        fileEntry->setEditable(true);

        if (QIcon icon = QFileIconProvider().icon(entry); !icon.isNull())
            fileEntry->setIcon(icon);

        items.append(fileEntry);

        const auto extItem = new QStandardItem(entry.suffix());
        extItem->setTextAlignment(Qt::AlignCenter);
        extItem->setEditable(false);
        items.append(extItem);

        items.append(fileSizeItemByEntry(entry));

        auto dateItem = new QStandardItem(entry.lastModified().toString("dd/MM/yy hh:mm"));
        dateItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        dateItem->setEditable(false);
        items.append(dateItem);

        items.append(attrItemByEntry(entry));

        appendRow(items);
    }

    return true;
}

DirectoryWidget::DirectoryWidget(QWidget* parent) : QTableView(parent)
{
    QTableView::setModel(new DirectoryWidgetModel(0, 5, this));

    setEditTriggers(SelectedClicked);
    setSelectionBehavior(SelectRows);

    const auto headers = QStringList{"Name", "Ext", "Size", "Date", "Attr"};
    for (int i = 0; i < headers.size(); ++i)
        model()->setHeaderData(i, Qt::Horizontal, headers[i]);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->resizeSection(1, 60);
    horizontalHeader()->resizeSection(2, 90);
    horizontalHeader()->resizeSection(3, 110);
    horizontalHeader()->resizeSection(4, 50);

    verticalHeader()->setVisible(false);
    verticalHeader()->setDefaultSectionSize(14);

    setDirectory(QDir::currentPath());

    const auto onCellEntered = [this](const QModelIndex& index) {
        const auto fileNameItem = model()->itemFromIndex(
            model()->index(index.row(), 0)
        );

        if (const auto fileInfo = fileNameItem->data(Qt::UserRole).value<QFileInfo>(); fileInfo.isDir()) {
            if (!fileInfo.absoluteDir().isReadable()) {
                QMessageBox::critical(this, "Error", "Can't open directory: permission denied");
                return;
            }

            setDirectory(fileInfo.absoluteFilePath());
        } else {
            emit fileTriggered(fileInfo);
        }
    };

    connect(this, &QTableView::activated, onCellEntered);
}

void DirectoryWidget::setDirectory(const QString& directory)
{
    const QDir dir(QDir::cleanPath(directory));

    if (!dir.isReadable())
        return;

    if (!model()->setDirectory(dir))
        return;

    m_directory = dir.absolutePath();

    if (model()->rowCount() > 0)
        selectRow(0);
}

QDir DirectoryWidget::directory() const
{
    return m_directory;
}

void DirectoryWidget::reload()
{
    setDirectory(m_directory);
}

void DirectoryWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit activated(currentIndex());
        return;
    }

    QTableView::keyPressEvent(event);
}

void DirectoryWidget::focusInEvent(QFocusEvent* event)
{
    QTableView::focusInEvent(event);

    emit focusIn();
}

DirectoryWidgetModel* DirectoryWidget::model() const
{
    return qobject_cast<DirectoryWidgetModel*>(QTableView::model());
}