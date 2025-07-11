#pragma once

#include <QDir>
#include <QObject>
#include <QScopedPointer>

enum class OperationType
{
    Copy,
    Move
};

class MoveCopyDialog final : public QObject
{
    Q_OBJECT

public:
    MoveCopyDialog(QObject* parent = nullptr, OperationType operationType = OperationType::Copy, const QString& destination = QString(), int fileCount = 1);
    virtual ~MoveCopyDialog();

    void close() const;

signals:
    void accepted(const QString& destination);
    void rejected();
    void closed();

private slots:
    void onAccepted(const QString& destination);
    void onCanceled();

private:
    struct Private;
    QScopedPointer<Private> d;
};