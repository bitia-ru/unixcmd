#pragma once

#include <QDir>
#include <QObject>
#include <QScopedPointer>

namespace functions {
namespace CreateDirectory {

class Dialog final : public QObject
{
    Q_OBJECT

public:
    Dialog(QObject* parent = nullptr);
    virtual ~Dialog();

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

} // namespace CreateDirectory
} // namespace functions