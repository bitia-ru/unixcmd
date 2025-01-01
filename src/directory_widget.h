#pragma once

#include <QTableWidget>


class DirectoryWidget final : public QTableWidget {
public:
    DirectoryWidget(QWidget* parent = nullptr);

    void setDirectory(const QString& directory);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QString m_directory;
};