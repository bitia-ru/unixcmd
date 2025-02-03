#include "directory_view_style.h"

#include <QTableView>
#include <QItemSelectionModel>


void CustomStyle::drawPrimitive(
    PrimitiveElement element,
    const QStyleOption* option,
    QPainter* painter,
    const QWidget* widget
) const
{
    if (element == PE_FrameFocusRect) {
        Q_ASSERT(widget);

        const auto frameOption = qstyleoption_cast<const QStyleOptionFocusRect*>(option);
        Q_ASSERT(frameOption);

        const QPalette::ColorGroup cg = widget->isEnabled() ? QPalette::Normal : QPalette::Disabled;

        const auto pen = painter->pen();
        auto color = frameOption->palette.brush(cg, QPalette::Text).color();
        color.setAlphaF(0.5);
        painter->setClipping(false);
        painter->setPen(QPen(color, 2, Qt::DotLine));
        painter->drawRect(frameOption->rect);
        painter->setPen(pen);

        return;
    }

    QProxyStyle::drawPrimitive(element, option, painter, widget);
}
