#pragma once

#include <QDir>
#include <QObject>
#include <QScopedPointer>

class ProcessingDialog final : public QObject
{
    Q_OBJECT

public:
    ProcessingDialog(QObject* parent = nullptr, const QString& title = QString());
    virtual ~ProcessingDialog();

    void abort() const;

signals:
    void aborted();

private slots:
    void onAborted();

private:
    struct Private;
    QScopedPointer<Private> d;
};