#pragma once

#include <QDir>
#include <QObject>
#include <QScopedPointer>

class CreateDirectoryDialog final : public QObject
{
    Q_OBJECT

public:
    CreateDirectoryDialog(QObject* parent = nullptr);
    virtual ~CreateDirectoryDialog();

    void close() const;

signals:
    void accepted(const QString& directoryName);
    void rejected();
    void closed();

private slots:
    void onAccepted(const QString& directoryName);
    void onCanceled();

private:
    struct Private;
    QScopedPointer<Private> d;
};