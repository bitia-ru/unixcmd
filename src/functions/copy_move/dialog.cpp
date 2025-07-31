#include "dialog.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickWindow>

namespace functions {
namespace CopyMove {

struct Dialog::Private
{
    QQmlEngine engine;
    QQmlComponent component;
    QQuickWindow* window;
    OperationType operationType;

    Private(OperationType type)
        : component(&engine), operationType(type)
    {
    }
};

Dialog::Dialog(QObject* parent, OperationType operationType, const QString& destination, int fileCount) : d(new Private(operationType)), QObject(parent)
{
    connect(&d->component, &QQmlComponent::statusChanged, [this, destination, fileCount](const QQmlComponent::Status status)
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
                    {"destination", destination},
                    {"operationType", static_cast<int>(d->operationType)},
                    {"fileCount", fileCount},
                });

            if (obj) {
                if (auto* window = qobject_cast<QQuickWindow*>(obj); window) {
                    connect(window, SIGNAL(accepted(QString)), this, SLOT(onAccepted(QString)));
                    connect(window, SIGNAL(canceled()), this, SLOT(onCanceled()));

                    window->show();

                    d->window = window;
                }
            }
        }

        default:;
        }
    });

    d->component.loadUrl(QUrl("qrc:/unixcmd/qml/move_copy_dialog.qml"));
}

Dialog::~Dialog() = default;

void Dialog::close() const
{
    if (d->window)
        d->window->close();
}

void Dialog::onAccepted(const QString& destination)
{
    close();

    emit accepted(destination);
    emit closed();
}

void Dialog::onCanceled()
{
    close();

    emit rejected();
    emit closed();
}

} // namespace CopyMove
} // namespace functions
