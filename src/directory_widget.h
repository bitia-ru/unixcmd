#pragma once

#include <QListWidget>


class DirectoryWidget : public QListWidget {
public:
    DirectoryWidget(QWidget* parent = nullptr);

    void setDirectory(const QString& directory);

private:
    QString m_directory;
};