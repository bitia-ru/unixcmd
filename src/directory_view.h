#pragma once

#include <QFileInfo>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTableWidget>


class DirectoryView final : public QTableView
{
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

    void setShowHiddenFiles(bool showHiddenFiles);
    [[nodiscard]] bool hiddenFilesVisible() const;

    void setQuickSearch(const QString& text);
    void setCurrentRow(int row) const;

    void setSorting(SortType sortType, bool ascending);
    void updateSorting();

public slots:
    void reload();

private /* events: */:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

private:
    bool setDirectoryInternal(const QDir& dir, bool showHiddenFiles);

    [[nodiscard]] QStandardItemModel* sourceModel() const;

signals:
    void directoryChanged(const QDir& directory);
    void focusIn();
    void fileTriggered(const QFileInfo& fileInfo);

private:
    struct Private;
    QScopedPointer<Private> d;
};