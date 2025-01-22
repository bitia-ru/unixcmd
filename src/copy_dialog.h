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

    void close() const;

signals:
    void accepted(const QDir& destination);
    void rejected();
    void closed();

private slots:
    void onAccepted(const QString& destination);
    void onCanceled();

private:
    struct Private;
    QScopedPointer<Private> d;
};