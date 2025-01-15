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

CopyDialog::CopyDialog(QObject* parent) : d(new Private), QObject(parent)
{
    connect(&d->component, &QQmlComponent::statusChanged, [this](const QQmlComponent::Status status)
    {
        switch (status) {
        case QQmlComponent::Error:
            for (const QQmlError &error: d->component.errors()) {
                qDebug() << "QML Error:" << error.toString();
            }
        case QQmlComponent::Ready:
            qDebug() << "CopyDialog::Ready";
            QObject* obj = d->component.create();

            if (!obj) {
                qDebug() << "FOOOOOO";
            } else {
                QQuickWindow* window = qobject_cast<QQuickWindow*>(obj);

                if (!window) {
                    qDebug() << "FOOOOOO";
                } else {
                    window->show();
                }
            }
        }
    });

    d->component.loadUrl(QUrl("qrc:/unixcmd/qml/copy_dialog.qml"));
}

CopyDialog::~CopyDialog() = default;