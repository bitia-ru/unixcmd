#include "create_directory_dialog.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickWindow>


struct CreateDirectoryDialog::Private
{
    QQmlEngine engine;
    QQmlComponent component;
    QQuickWindow* window;

    Private()
        : component(&engine)
    {
    }
};

CreateDirectoryDialog::CreateDirectoryDialog(QObject* parent) : d(new Private), QObject(parent)
{
    connect(&d->component, &QQmlComponent::statusChanged, [this](const QQmlComponent::Status status)
    {
        switch (status) {
        case QQmlComponent::Error:
            for (const QQmlError &error: d->component.errors())
                qDebug() << "QML Error:" << error.toString();
            break;

        case QQmlComponent::Ready:{
            QObject* obj = d->component.create();

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

    d->component.loadUrl(QUrl("qrc:/unixcmd/qml/create_directory_dialog.qml"));
}

CreateDirectoryDialog::~CreateDirectoryDialog() = default;

void CreateDirectoryDialog::close() const
{
    if (d->window)
        d->window->close();
}

void CreateDirectoryDialog::onAccepted(const QString& directoryName)
{
    close();

    emit accepted(directoryName);
    emit closed();
}

void CreateDirectoryDialog::onCanceled()
{
    close();

    emit rejected();
    emit closed();
}