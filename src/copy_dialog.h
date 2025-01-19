#pragma once

#include <QDir>
#include <QObject>
#include <QScopedPointer>

class CopyDialog final : public QObject
{
    Q_OBJECT

public:
    CopyDialog(QObject* parent = nullptr, const QDir& destination = QDir());
    virtual ~CopyDialog();

private:
    struct Private;
    QScopedPointer<Private> d;
};