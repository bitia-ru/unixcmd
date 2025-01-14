#pragma once

#include <QFileInfo>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTableWidget>


class DirectoryWidgetModel final : public QStandardItemModel {
    Q_OBJECT

public:
    DirectoryWidgetModel(const int rows, const int columns, QObject *parent = nullptr)
        : QStandardItemModel(rows, columns, parent) {}

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    bool setDirectory(const QDir& dir);
};

class DirectoryWidget final : public QTableView {
    Q_OBJECT

public:
    explicit DirectoryWidget(QWidget* parent = nullptr);

    void setDirectory(const QString& directory);

public slots:
    void reload();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

    DirectoryWidgetModel* model() const;

signals:
    void focusIn();
    void fileTriggered(const QFileInfo& fileInfo);

private:
    QString m_directory;
};