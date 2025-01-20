#include "processing_dialog.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickWindow>


struct ProcessingDialog::Private
{
    QQmlEngine engine;
    QQmlComponent component;
    QQuickWindow* window;

    Private()
        : component(&engine)
    {
    }
};

ProcessingDialog::ProcessingDialog(QObject* parent, const QString& title) : d(new Private), QObject(parent)
{
    connect(&d->component, &QQmlComponent::statusChanged, [this, title](const QQmlComponent::Status status)
    {
        switch (status) {
        case QQmlComponent::Error:
            for (const QQmlError &error: d->component.errors()) {
                qDebug() << "QML Error:" << error.toString();
            }
        case QQmlComponent::Ready:
            QObject* obj = d->component.createWithInitialProperties(
                QVariantMap{
                    {"title", title},
                });

            if (obj) {
                if (auto* window = qobject_cast<QQuickWindow*>(obj); window) {
                    connect(window, SIGNAL(aborted()), this, SLOT(onAborted()));

                    window->show();

                    d->window = window;
                }
            }
        }
    });

    d->component.loadUrl(QUrl("qrc:/unixcmd/qml/processing_dialog.qml"));
}

ProcessingDialog::~ProcessingDialog() = default;

void ProcessingDialog::abort() const
{
    if (d->window)
        d->window->close();
}

void ProcessingDialog::onAborted()
{
    abort();

    emit aborted();
}