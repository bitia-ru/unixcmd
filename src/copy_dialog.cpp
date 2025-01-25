#include "copy_dialog.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickWindow>


struct CopyDialog::Private
{
    QQmlEngine engine;
    QQmlComponent component;
    QQuickWindow* window;

    Private()
        : component(&engine)
    {
    }
};

CopyDialog::CopyDialog(QObject* parent, const QString& destination) : d(new Private), QObject(parent)
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
                    {"destination", destination},
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
    });

    d->component.loadUrl(QUrl("qrc:/unixcmd/qml/copy_dialog.qml"));
}

CopyDialog::~CopyDialog() = default;

void CopyDialog::close() const
{
    if (d->window)
        d->window->close();
}

void CopyDialog::onAccepted(const QString& destination)
{
    close();

    emit accepted(destination);
    emit closed();
}

void CopyDialog::onCanceled()
{
    close();

    emit rejected();
    emit closed();
}