#pragma once

#include <QTableWidget>
#include <QFileInfo>


class DirectoryWidget final : public QTableWidget {
    Q_OBJECT

public:
    explicit DirectoryWidget(QWidget* parent = nullptr);

    void setDirectory(const QString& directory);

public slots:
    void reload();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

signals:
    void focusIn();
    void fileTriggered(const QFileInfo& fileInfo);

private:
    QString m_directory;
};