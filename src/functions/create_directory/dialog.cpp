#include "dialog.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickWindow>

namespace functions {
namespace CreateDirectory {

struct Dialog::Private
{
    QQmlEngine engine;
    QQmlComponent component;
    QQuickWindow* window;

    Private()
        : component(&engine)
    {
    }
};

Dialog::Dialog(QObject* parent) : d(new Private), QObject(parent)
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

Dialog::~Dialog() = default;

void Dialog::close() const
{
    if (d->window)
        d->window->close();
}

void Dialog::onAccepted(const QString& directoryName)
{
    close();

    emit accepted(directoryName);
    emit closed();
}

void Dialog::onCanceled()
{
    close();

    emit rejected();
    emit closed();
}

} // namespace CreateDirectory
} // namespace functions

#include "dialog.moc"