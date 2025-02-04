#include "directory_view.h"
#include "directory_view_style.h"

#include <QDir>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableView>


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

DirectorySortFilterProxyModel::DirectorySortFilterProxyModel(QObject* parent/* = nullptr*/)
    : QSortFilterProxyModel(parent)
{
}

bool DirectorySortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const auto leftFirstColumnIndex = sourceModel()->index(left.row(), 0);
    const auto rightFirstColumnIndex = sourceModel()->index(right.row(), 0);

    const auto leftData = sourceModel()->data(leftFirstColumnIndex, Qt::UserRole).value<QFileInfo>();
    const auto rightData = sourceModel()->data(rightFirstColumnIndex, Qt::UserRole).value<QFileInfo>();

    const auto leftDotDotDirIndicator = sourceModel()->data(leftFirstColumnIndex, Qt::UserRole + 1);
    const auto rightDotDotDirIndicator = sourceModel()->data(rightFirstColumnIndex, Qt::UserRole + 1);

    if (leftDotDotDirIndicator.toBool())
        return sortOrder() == Qt::AscendingOrder;
    if (rightDotDotDirIndicator.toBool())
        return sortOrder() == Qt::DescendingOrder;

    if (const int column = left.column(); column == 0)
        return leftData.fileName().toLower() < rightData.fileName().toLower();
    else if (column == 1) {
        if (leftData.isDir())
            return false;
        if (rightData.isDir())
            return true;

        return leftData.suffix() < rightData.suffix();
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

QVariant DirectoryItemModel::data(const QModelIndex &index, const int role/* = Qt::DisplayRole*/) const
{
    if (index.column() == 0 && role == Qt::EditRole) {
        const auto fileInfo = QStandardItemModel::data(index, Qt::UserRole).value<QFileInfo>();

        return fileInfo.fileName();
    }

    return QStandardItemModel::data(index, role);
}

bool DirectoryItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.column() == 0 && role == Qt::EditRole) {
        const auto newFileName = value.toString();
        const auto originalFileInfo = QStandardItemModel::data(index, Qt::UserRole).value<QFileInfo>();
        const QFileInfo newFileInfo(originalFileInfo.absoluteDir().filePath(newFileName));

        if (originalFileInfo.fileName() != newFileName && !originalFileInfo.dir().rename(originalFileInfo.fileName(), newFileName)) {
            QMessageBox::critical(nullptr, "Error", "Can't rename file");

            return false;
        }

        QStandardItemModel::setData(index, QVariant::fromValue(newFileInfo), Qt::UserRole);

        if (newFileInfo.isDir())
            QStandardItemModel::setData(index, newFileInfo.fileName(), Qt::DisplayRole);
        else {
            QStandardItemModel::setData(index, newFileInfo.baseName(), Qt::DisplayRole);
            QStandardItemModel::setData(
                DirectoryItemModel::index(index.row(), 1),
                newFileInfo.suffix(),
                Qt::DisplayRole
            );
        }

        return true;
    }

    return QStandardItemModel::setData(index, value, role);
}

struct DirectoryView::Private
{
    QDir directory;
    bool showHiddenFiles = false;

    QString quickSearch;
    QLabel* quickSearchLabel = nullptr;
    int quickSearchIndex = -1;
    DirectorySortFilterProxyModel* model = nullptr;

    SortType sortType = Unsorted;
    bool sortAscending = true;
};

DirectoryView::DirectoryView(QWidget* parent) : QTableView(parent), d(new Private)
{
    d->model = new DirectorySortFilterProxyModel(this);
    d->model->setSourceModel(new DirectoryItemModel(0, 5, this));
    QTableView::setModel(d->model);

    setEditTriggers(SelectedClicked);
    setSelectionBehavior(SelectRows);
    setSelectionMode(NoSelection);

    setStyle(new CustomStyle(style()));

    const auto headers = QStringList{"Name", "Ext", "Size", "Date", "Attr"};
    for (int i = 0; i < headers.size(); ++i)
        d->model->setHeaderData(i, Qt::Horizontal, headers[i]);

    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    horizontalHeader()->resizeSection(1, 60);
    horizontalHeader()->resizeSection(2, 90);
    horizontalHeader()->resizeSection(3, 110);
    horizontalHeader()->resizeSection(4, 50);

    verticalHeader()->setVisible(false);
    verticalHeader()->setDefaultSectionSize(14);

    setDirectory(QDir::currentPath());

    const auto onCellEntered = [this](const QModelIndex& index) {
        const auto fileNameIndex = d->model->index(index.row(), 0);

        if (const auto fileInfo = d->model->data(fileNameIndex, Qt::UserRole).value<QFileInfo>(); fileInfo.isDir()) {
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

    connect(horizontalHeader(), &QHeaderView::sectionClicked, [this](int column) {
        const auto newSortType = static_cast<SortType>(column);
        const bool ascending = d->sortType == newSortType ? !d->sortAscending : true;

        setSorting(newSortType, ascending);
    });

    updateSorting();

    connect(d->model, &QAbstractItemModel::dataChanged, [this] { updateSorting(); });
}

DirectoryView::~DirectoryView() = default;

bool DirectoryView::setDirectoryInternal(const QDir& dir, bool showHiddenFiles)
{
    d->model->removeRows(0, d->model->rowCount());

    if (!dir.exists())
        return false;

    if (!dir.isRoot()) {
        const auto item = new QStandardItem("[..]");
        const auto parentDirPath = QDir::cleanPath(QFileInfo(dir.absolutePath()).dir().absolutePath());
        item->setData(QVariant::fromValue(QFileInfo(parentDirPath)), Qt::UserRole);
        item->setData(QVariant::fromValue(true), Qt::UserRole + 1);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setIcon(QFileIconProvider().icon(QAbstractFileIconProvider::Folder));

        const auto extItem = new QStandardItem("");
        extItem->setTextAlignment(Qt::AlignCenter);
        extItem->setEditable(false);

        sourceModel()->appendRow({
            item,
            extItem,
            fileSizeItemByEntry(QFileInfo(dir.absolutePath())),
        });

        for (int i = 0; i < d->model->columnCount(); i++)
            sourceModel()->itemFromIndex(sourceModel()->index(0, i))->setSelectable(false);
    }

    QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System;

    if (showHiddenFiles)
        filters |= QDir::Hidden;

    for (const auto& entry : dir.entryInfoList(filters))
    {
        QList<QStandardItem*> items;

        const auto fileEntry = new QStandardItem(fileNameByEntry(entry));
        fileEntry->setData(QVariant::fromValue(entry), Qt::UserRole);
        fileEntry->setEditable(true);

        if (QIcon icon = QFileIconProvider().icon(entry); !icon.isNull())
            fileEntry->setIcon(icon);

        items.append(fileEntry);

        const auto extItem = new QStandardItem(entry.isDir() ? "" : entry.suffix());
        extItem->setTextAlignment(Qt::AlignCenter);
        extItem->setEditable(false);
        items.append(extItem);

        items.append(fileSizeItemByEntry(entry));

        auto dateItem = new QStandardItem(entry.lastModified().toString("dd/MM/yy hh:mm"));
        dateItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        dateItem->setEditable(false);
        items.append(dateItem);

        items.append(attrItemByEntry(entry));

        dynamic_cast<QStandardItemModel*>(d->model->sourceModel())->appendRow(items);
    }

    updateSorting();

    return true;
}

void DirectoryView::setDirectory(const QDir& directory)
{
    const QDir dir(QDir::cleanPath(directory.absolutePath()));

    if (!dir.isReadable())
        return;

    if (!setDirectoryInternal(dir, d->showHiddenFiles))
        return;

    d->directory = dir;

    if (d->model->rowCount() > 0)
        setCurrentRow(0);

    emit directoryChanged(d->directory);
}

QDir DirectoryView::directory() const
{
    return d->directory;
}

void DirectoryView::setShowHiddenFiles(bool showHiddenFiles)
{
    if (d->showHiddenFiles != showHiddenFiles) {
        d->showHiddenFiles = showHiddenFiles;

        reload();
    }
}

bool DirectoryView::hiddenFilesVisible() const
{
    return d->showHiddenFiles;
}

void DirectoryView::setQuickSearch(const QString& text)
{
    d->quickSearch = text;

    if (d->quickSearchLabel == nullptr) {
        d->quickSearchLabel = new QLabel(this);
        d->quickSearchLabel->setFrameStyle(Panel | Raised);
        d->quickSearchLabel->setMargin(4);
        d->quickSearchLabel->setAutoFillBackground(true);
    }

    if (d->quickSearch.isEmpty()) {
        if (d->quickSearchLabel)
            d->quickSearchLabel->hide();
        d->quickSearchIndex = -1;

        return;
    }

    d->quickSearchLabel->setText(d->quickSearch);
    d->quickSearchLabel->resize(
        d->quickSearchLabel->fontMetrics().boundingRect(d->quickSearch).size() + QSize(12, 10)
    );
    d->quickSearchLabel->move(30, parentWidget()->height() - 30 - d->quickSearchLabel->height());
    d->quickSearchLabel->show();

    for (int i = 0; i < d->model->rowCount(); ++i) {
        if (const auto item = dynamic_cast<QStandardItemModel*>(d->model->sourceModel())->item(i, 0); item->text().startsWith(d->quickSearch, Qt::CaseInsensitive)) {
            d->quickSearchIndex = i;
            selectRow(i);
            return;
        }
    }

    d->quickSearchIndex = -1;
}

void DirectoryView::setCurrentRow(int row)
{
    const auto newIndex = model()->index(row, 0);
    if (!newIndex.isValid())
        return;

    selectionModel()->setCurrentIndex(newIndex, QItemSelectionModel::Rows);
}

void DirectoryView::setSorting(SortType sortType, bool ascending)
{
    setSortingEnabled(true);

    switch (sortType) {
    case SortByName:
        sortByColumn(0, d->sortAscending ? Qt::AscendingOrder : Qt::DescendingOrder);
        break;
    case SortByExtension:
        sortByColumn(1, d->sortAscending ? Qt::AscendingOrder : Qt::DescendingOrder);
        break;
    case SortBySize:
        sortByColumn(2, d->sortAscending ? Qt::AscendingOrder : Qt::DescendingOrder);
        break;
    default:
        setSortingEnabled(false);
    }

    d->sortType = sortType;
    d->sortAscending = ascending;
}

void DirectoryView::updateSorting()
{
    setSorting(d->sortType, d->sortAscending);
}

void DirectoryView::reload()
{
    setDirectory(d->directory);
}

void DirectoryView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit activated(currentIndex());

        event->accept();
        return;
    }

    if (
        const auto key = event->key();
        key > 0 && key < 0xffff && QChar::isPrint(key)
            && (!d->quickSearch.isEmpty() || key != Qt::Key_Space)
    ) {
        setQuickSearch(d->quickSearch + event->text());

        event->accept();
        return;
    }

    if (!d->quickSearch.isEmpty() && event->key() == Qt::Key_Backspace) {
        auto newValue = d->quickSearch;
        newValue.chop(1);

        setQuickSearch(newValue);

        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Escape && !d->quickSearch.isEmpty()) {
        setQuickSearch("");

        event->accept();
        return;
    }

    if (d->model->rowCount() > 0) {
        if (event->key() == Qt::Key_Up) {
            if (const auto index = currentIndex(); !index.isValid())
                setCurrentRow(0);
            else if (index.row() > 0) {
                if (event->modifiers() & Qt::ShiftModifier) {
                    selectionModel()->select(
                        index,
                        QItemSelectionModel::Rows | QItemSelectionModel::Toggle
                    );
                }

                setCurrentRow(index.row() - 1);
            }

            event->accept();
            return;
        }

        if (event->key() == Qt::Key_Down) {
            if (const auto index = currentIndex(); !index.isValid())
                setCurrentRow(d->model->rowCount() - 1);
            else if (index.row() < d->model->rowCount() - 1) {
                if (event->modifiers() & Qt::ShiftModifier) {
                    selectionModel()->select(
                        index,
                        QItemSelectionModel::Rows | QItemSelectionModel::Toggle
                    );
                }

                setCurrentRow(index.row() + 1);
            }

            event->accept();
            return;
        }

        if (event->key() == Qt::Key_Space) {
            selectionModel()->select(
                currentIndex(),
                QItemSelectionModel::Rows | QItemSelectionModel::Toggle
            );

            event->accept();
            return;
        }
    }

    QTableView::keyPressEvent(event);
}

void DirectoryView::focusInEvent(QFocusEvent* event)
{
    QTableView::focusInEvent(event);

    emit focusIn();
}

void DirectoryView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        if (const QModelIndex index = indexAt(event->pos()); index.isValid()) {
            selectionModel()->select(
                index,
                QItemSelectionModel::Rows | QItemSelectionModel::Toggle
            );
            setCurrentRow(index.row());
        }
    }

    QTableView::mousePressEvent(event);
}

void DirectoryView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    QTableView::selectionChanged(selected, deselected);

    for (const auto& index : deselected.indexes()) {
        if (index.row() == d->quickSearchIndex) {
            setQuickSearch("");
            break;
        }
    }
}

QStandardItemModel* DirectoryView::sourceModel()
{
    return dynamic_cast<QStandardItemModel*>(d->model->sourceModel());
}
