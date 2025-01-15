#pragma once

#include <QObject>
#include <QScopedPointer>

class CopyDialog final : public QObject
{
    Q_OBJECT

public:
    CopyDialog(QObject* parent = nullptr);
    virtual ~CopyDialog();

private:
    struct Private;
    QScopedPointer<Private> d;
};