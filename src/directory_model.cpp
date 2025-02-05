#include "directory_model.h"

#include <QDir>
#include <QMessageBox>


Q_DECLARE_METATYPE(DirectoryModelIndex::SizeCalculationInfo)

DirectoryModelIndex::DirectoryModelIndex(const QModelIndex& index)
    : QModelIndex(index)
{
}

std::optional<QFileInfo> DirectoryModelIndex::fileInfo() const
{
    if (!hasFileInfo())
        return {};

    return data(FileInfoRole).value<QFileInfo>();
}

bool DirectoryModelIndex::isDotDotDir() const
{
    return hasDotDotDirFlag() && data(IsDotDotRole).toBool();
}

std::optional<DirectoryModelIndex::SizeCalculationInfo>
    DirectoryModelIndex::sizeCalculationInfo() const
{
    if (!hasSizeCalculationInfo())
        return {};

    return data(SizeCalculationInfoRole).value<SizeCalculationInfo>();
}

bool DirectoryModelIndex::hasFileInfo() const
{
    return data(FileInfoRole).userType() == qMetaTypeId<QFileInfo>();
}

bool DirectoryModelIndex::hasDotDotDirFlag() const
{
    return data(IsDotDotRole).userType() == QMetaType::Bool;
}

bool DirectoryModelIndex::hasSizeCalculationInfo() const
{
    return data(SizeCalculationInfoRole).userType() == qMetaTypeId<SizeCalculationInfo>();
}

DirectorySortFilterProxyModel::DirectorySortFilterProxyModel(QObject* parent/* = nullptr*/)
    : QSortFilterProxyModel(parent)
{
}

DirectoryItemModel* DirectorySortFilterProxyModel::directoryModel() const
{
    return dynamic_cast<DirectoryItemModel*>(sourceModel());
}

bool DirectorySortFilterProxyModel::lessThan(
    const QModelIndex& left, const QModelIndex& right
) const
{
    const auto leftFirstColumnIndex = DirectoryModelIndex(sourceModel()->index(left.row(), 0));
    const auto rightFirstColumnIndex = DirectoryModelIndex(sourceModel()->index(right.row(), 0));

    const auto leftFileInfo = *leftFirstColumnIndex.fileInfo();
    const auto rightFileInfo = *rightFirstColumnIndex.fileInfo();

    if (leftFirstColumnIndex.isDotDotDir())
        return sortOrder() == Qt::AscendingOrder;
    if (rightFirstColumnIndex.isDotDotDir())
        return sortOrder() == Qt::DescendingOrder;

    if (const int column = left.column(); column == 0)
        return leftFileInfo.fileName().toLower() < rightFileInfo.fileName().toLower();
    else if (column == 1) {
        if (leftFileInfo.isDir())
            return false;
        if (rightFileInfo.isDir())
            return true;

        return leftFileInfo.suffix() < rightFileInfo.suffix();
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

QVariant DirectoryItemModel::data(const QModelIndex &index, const int role/* = Qt::DisplayRole*/) const
{
    if (index.column() == 0 && role == Qt::EditRole)
        return DirectoryModelIndex(index).fileInfo()->fileName();

    if (index.column() == 2 && role == Qt::DisplayRole) {
        if (const auto sizeInfo = DirectoryModelIndex(index.siblingAtColumn(0)).sizeCalculationInfo(); sizeInfo) {

            if (sizeInfo->inProgress)
                return "Calculating...";

            return QString::number(sizeInfo->currentSizeInBytes);
        }
    }

    return QStandardItemModel::data(index, role);
}

bool DirectoryItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.column() == 0 && role == Qt::EditRole) {
        const auto newFileName = value.toString();
        const auto originalFileInfo = *DirectoryModelIndex(index).fileInfo();
        const QFileInfo newFileInfo(originalFileInfo.absoluteDir().filePath(newFileName));

        if (originalFileInfo.fileName() != newFileName
            && !originalFileInfo.dir().rename(originalFileInfo.fileName(), newFileName))
        {
            QMessageBox::critical(nullptr, "Error", "Can't rename file");

            return false;
        }

        QStandardItemModel::setData(index, QVariant::fromValue(newFileInfo),
            DirectoryModelIndex::FileInfoRole);

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

    const auto result = QStandardItemModel::setData(index, value, role);

    return result;
}
