#pragma once

#include <QTableWidget>


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

private:
    QString m_directory;
};