#include "file_processing_dialog.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickWindow>

namespace functions {
namespace common {

struct FileProcessingDialog::Private
{
    QQmlEngine engine;
    QQmlComponent component;
    QQuickWindow* window;

    Private()
        : component(&engine)
    {
    }
};

FileProcessingDialog::FileProcessingDialog(QObject* parent, const QString& title) : d(new Private), QObject(parent)
{
    connect(&d->component, &QQmlComponent::statusChanged, [this, title](const QQmlComponent::Status status)
    {
        switch (status) {
        case QQmlComponent::Error:
            for (const QQmlError &error: d->component.errors()) {
                qDebug() << "QML Error:" << error.toString();
            }
            break;

        case QQmlComponent::Ready:{
            QObject* obj = d->component.createWithInitialProperties(
                QVariantMap{
                    {"title", title},
                });

            if (obj) {
                if (auto* window = qobject_cast<QQuickWindow*>(obj); window) {
                    connect(window, SIGNAL(aborted()), this, SLOT(onAborted()));

                    d->window = window;
                }
            }
        }

        default:;
        }
    });

    d->component.loadUrl(QUrl("qrc:/unixcmd/qml/file_processing_dialog.qml"));
}

FileProcessingDialog::~FileProcessingDialog() = default;

void FileProcessingDialog::abort() const
{
    if (d->window)
        d->window->close();
}

void FileProcessingDialog::show()
{
    if (d->window)
        d->window->show();
}

void FileProcessingDialog::setStatus(const QString& status)
{
    if (d->window)
        d->window->setProperty("status", status);
}

void FileProcessingDialog::onAborted()
{
    abort();

    emit aborted();
}

} // namespace common
} // namespace functions
