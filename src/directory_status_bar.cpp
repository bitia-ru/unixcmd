#include "directory_status_bar.h"

#include <QLabel>


namespace
{

constexpr auto kHiddenFilesVisibleLabel = "HID";

}

struct DirectoryStatusBar::Private
{
    QLabel* hiddenFilesLabel = nullptr;
};

DirectoryStatusBar::DirectoryStatusBar(QObject* parent)
    : QStatusBar(qobject_cast<QWidget*>(parent))
    , d(new Private{
        .hiddenFilesLabel = new QLabel(this),
    })
{
    setContentsMargins(0, 2, 0, 2);

    const QFontMetrics metrics(d->hiddenFilesLabel->font());
    const auto rect = metrics.boundingRect(
        QRect(0, 0, d->hiddenFilesLabel->width(), d->hiddenFilesLabel->height()),
        Qt::TextSingleLine,
        kHiddenFilesVisibleLabel
    );

    d->hiddenFilesLabel->setMargin(2);
    d->hiddenFilesLabel->setDisabled(true);
    d->hiddenFilesLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->hiddenFilesLabel->setMinimumWidth(rect.width() + 6);
    d->hiddenFilesLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    addPermanentWidget(d->hiddenFilesLabel);
}

DirectoryStatusBar::~DirectoryStatusBar() = default;

void DirectoryStatusBar::setHiddenFilesVisible(bool visible)
{
    if (visible)
        d->hiddenFilesLabel->setText(kHiddenFilesVisibleLabel);
    else
        d->hiddenFilesLabel->clear();
}