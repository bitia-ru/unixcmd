#pragma once

#include <QDir>
#include <QObject>
#include <QScopedPointer>

namespace functions {
namespace CopyMove {

enum class OperationType
{
    Copy,
    Move
};

class Dialog final : public QObject
{
    Q_OBJECT

public:
    Dialog(QObject* parent = nullptr, OperationType operationType = OperationType::Copy, const QString& destination = QString(), int fileCount = 1);
    virtual ~Dialog();

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

} // namespace CopyMove
} // namespace functions