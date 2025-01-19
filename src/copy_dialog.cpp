#include "copy_dialog.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickWindow>


struct CopyDialog::Private
{
    QQmlEngine engine;
    QQmlComponent component;

    Private()
        : component(&engine)
    {
    }
};

CopyDialog::CopyDialog(QObject* parent, const QDir& destination) : d(new Private), QObject(parent)
{
    connect(&d->component, &QQmlComponent::statusChanged, [this, destination](const QQmlComponent::Status status)
    {
        switch (status) {
        case QQmlComponent::Error:
            for (const QQmlError &error: d->component.errors()) {
                qDebug() << "QML Error:" << error.toString();
            }
        case QQmlComponent::Ready:
            QObject* obj = d->component.createWithInitialProperties(
                QVariantMap{
                    {"destination", destination.absolutePath()},
                });

            if (obj) {
                if (auto* window = qobject_cast<QQuickWindow*>(obj); window) {
                    window->show();
                }
            }
        }
    });

    d->component.loadUrl(QUrl("qrc:/unixcmd/qml/copy_dialog.qml"));
}

CopyDialog::~CopyDialog() = default;