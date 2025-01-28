#include "directory_widget.h"

#include "directory_view.h"

#include <QDir>
#include <QLabel>
#include <QVBoxLayout>


struct DirectoryWidget::Private
{
    QLabel* title = nullptr;
    DirectoryView* view = nullptr;
};

DirectoryWidget::DirectoryWidget(QWidget* parent)
    : QWidget(parent)
    , d(new Private{
        .title = new QLabel("", this),
        .view = new DirectoryView(this),
    })
{
    d->title->setContentsMargins(4, 0, 0, 0);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(d->title);
    layout->addWidget(d->view);
    layout->setContentsMargins(0, 5, 0, 0);
    setLayout(layout);

    connect(d->view, &DirectoryView::directoryChanged, [this](const QDir& directory) {
        d->title->setText(directory.absolutePath());
    });
    d->title->setText(d->view->directory().absolutePath());
}

DirectoryWidget::~DirectoryWidget() = default;

DirectoryView* DirectoryWidget::view()
{
    return d->view;
}