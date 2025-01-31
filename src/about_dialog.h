#pragma once

#include <QDir>
#include <QObject>
#include <QScopedPointer>

class AboutDialog final : public QObject
{
    Q_OBJECT

public:
    AboutDialog(QObject* parent = nullptr);
    virtual ~AboutDialog();

signals:
    void closed();

private:
    struct Private;
    QScopedPointer<Private> d;
};