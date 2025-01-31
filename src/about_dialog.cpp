#include "about_dialog.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickWindow>


struct AboutDialog::Private
{
    QQmlEngine engine;
    QQmlComponent component;
    QQuickWindow* window;

    Private()
        : component(&engine)
    {
    }
};

AboutDialog::AboutDialog(QObject* parent) : d(new Private), QObject(parent)
{
    connect(&d->component, &QQmlComponent::statusChanged, [this](const QQmlComponent::Status status)
    {
        switch (status) {
        case QQmlComponent::Error:
            for (const QQmlError &error: d->component.errors()) {
                qDebug() << "QML Error:" << error.toString();
            }
            emit closed();
            break;

        case QQmlComponent::Ready:{
            QObject* obj = d->component.create();

            if (obj) {
                if (auto* window = qobject_cast<QQuickWindow*>(obj); window) {
                    connect(window, &QQuickWindow::visibilityChanged, this, [this](QWindow::Visibility visibility) {
                        if (visibility == QWindow::Hidden)
                            emit closed();
                    });

                    window->show();

                    d->window = window;
                }
            }
        }

        default:
        }
    });

    d->component.loadUrl(QUrl("qrc:/unixcmd/qml/about_dialog.qml"));
}

AboutDialog::~AboutDialog() = default;