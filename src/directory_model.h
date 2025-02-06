#pragma once

#include <QFileInfo>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>


class DirectoryModelIndex final : public QModelIndex
{
public:
    struct SizeCalculationInfo
    {
        bool inProgress = true;
        qint64 currentSizeInBytes = -1;
    };

    enum DirectoryItemRole
    {
        FileInfoRole = Qt::UserRole,
        IsDotDotRole,
        SizeCalculationInfoRole,
    };

public:
    DirectoryModelIndex(const QModelIndex& index);

public:
    [[nodiscard]] std::optional<QFileInfo> fileInfo() const;
    [[nodiscard]] bool isDotDotDir() const;
    [[nodiscard]] std::optional<SizeCalculationInfo> sizeCalculationInfo() const;

    [[nodiscard]] bool hasFileInfo() const;
    [[nodiscard]] bool hasDotDotDirFlag() const;
    [[nodiscard]] bool hasSizeCalculationInfo() const;
};

class DirectoryItemModel;

class DirectorySortFilterProxyModel final : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit DirectorySortFilterProxyModel(QObject *parent = nullptr);

    [[nodiscard]] DirectoryItemModel* directoryModel() const;

protected:
    [[nodiscard]] bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

class DirectoryItemModel final : public QStandardItemModel
{
    Q_OBJECT

public:
    DirectoryItemModel(const int rows, const int columns, QObject *parent = nullptr)
        : QStandardItemModel(rows, columns, parent) {}

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};

