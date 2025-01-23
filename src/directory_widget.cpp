#include "directory_widget.h"

#include <QDir>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QMetaProperty>
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

struct DirectoryWidget::Private
{
    QDir directory;

    QString quickSearch;
    QLabel* quickSearchLabel = nullptr;
    int quickSearchIndex = -1;
};

DirectoryWidget::DirectoryWidget(QWidget* parent) : QTableView(parent), d(new Private)
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

DirectoryWidget::~DirectoryWidget() = default;

void DirectoryWidget::setDirectory(const QDir& directory)
{
    const QDir dir(QDir::cleanPath(directory.absolutePath()));

    if (!dir.isReadable())
        return;

    if (!model()->setDirectory(dir))
        return;

    d->directory = dir;

    if (model()->rowCount() > 0)
        selectRow(0);
}

QDir DirectoryWidget::directory() const
{
    return d->directory;
}

void DirectoryWidget::setQuickSearch(const QString& text)
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

    for (int i = 0; i < model()->rowCount(); ++i) {
        if (const auto item = model()->item(i, 0); item->text().startsWith(d->quickSearch, Qt::CaseInsensitive)) {
            d->quickSearchIndex = i;
            selectRow(i);
            return;
        }
    }

    d->quickSearchIndex = -1;
}

void DirectoryWidget::reload()
{
    setDirectory(d->directory);
}

void DirectoryWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit activated(currentIndex());

        event->accept();
        return;
    }

    ;
    if (const auto key = event->key(); key > 0 && key < 0xffff && QChar::isPrint(key)) {
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

    QTableView::keyPressEvent(event);
}

void DirectoryWidget::focusInEvent(QFocusEvent* event)
{
    QTableView::focusInEvent(event);

    emit focusIn();
}

void DirectoryWidget::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    QTableView::selectionChanged(selected, deselected);

    for (const auto& index : deselected.indexes()) {
        if (index.row() == d->quickSearchIndex) {
            setQuickSearch("");
            break;
        }
    }
}

DirectoryWidgetModel* DirectoryWidget::model() const
{
    return qobject_cast<DirectoryWidgetModel*>(QTableView::model());
}