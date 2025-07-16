#pragma once

#include <QDir>
#include <QObject>
#include <QScopedPointer>

namespace functions {
namespace common {

class FileProcessingDialog final : public QObject
{
    Q_OBJECT

public:
    FileProcessingDialog(QObject* parent = nullptr, const QString& title = QString());
    virtual ~FileProcessingDialog();

    void abort() const;

    void show();

    void setStatus(const QString& status);

signals:
    void aborted();

private slots:
    void onAborted();

private:
    struct Private;
    QScopedPointer<Private> d;
};

} // namespace common
} // namespace functions