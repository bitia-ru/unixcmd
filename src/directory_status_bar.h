#pragma once

#include <QScopedPointer>
#include <QStatusBar>


class DirectoryStatusBar : public QStatusBar
{
    Q_OBJECT

public:
    explicit DirectoryStatusBar(QObject* parent = nullptr);
    ~DirectoryStatusBar() override;

public slots:
    void setHiddenFilesVisible(bool visible);

private:
    struct Private;
    const QScopedPointer<Private> d;
};