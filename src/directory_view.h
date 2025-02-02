#pragma once

#include <QFileInfo>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableWidget>


class DirectorySortFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit DirectorySortFilterProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

class DirectoryItemModel final : public QStandardItemModel {
    Q_OBJECT

public:
    DirectoryItemModel(const int rows, const int columns, QObject *parent = nullptr)
        : QStandardItemModel(rows, columns, parent) {}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};

class DirectoryView final : public QTableView {
    Q_OBJECT

public:
    enum SortType
    {
        SortByName,
        SortByExtension,
        SortBySize,
        Unsorted,
    };

public:
    explicit DirectoryView(QWidget* parent = nullptr);
    ~DirectoryView() override;

    void setDirectory(const QDir& directory);
    [[nodiscard]] QDir directory() const;

    void setQuickSearch(const QString& text);
    void setShowHiddenFiles(bool showHiddenFiles);
    void setSorting(SortType sortType);

public slots:
    void reload();

private:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

signals:
    void directoryChanged(const QDir& directory);
    void focusIn();
    void fileTriggered(const QFileInfo& fileInfo);

private:
    bool setDirectoryInternal(const QDir& dir, bool showHiddenFiles);

    struct Private;
    QScopedPointer<Private> d;
};