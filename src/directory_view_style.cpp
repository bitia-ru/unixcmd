#include "directory_view_style.h"

#include <QTableView>
#include <QItemSelectionModel>


void CustomStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option,
                                QPainter *painter, const QWidget *widget) const {
    if (element == QStyle::PE_PanelItemViewRow) {
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem*>(option)) {
            bool isEnabled = (widget ? widget->isEnabled() : (vopt->state & QStyle::State_Enabled));
            QPalette::ColorGroup cg = isEnabled ? QPalette::Normal : QPalette::Disabled;

            if (cg == QPalette::Normal && !(vopt->state & QStyle::State_Active))
                cg = QPalette::Inactive;

            if (vopt->state & QStyle::State_HasFocus)
                painter->fillRect(vopt->rect, QBrush(QColor(255, 255, 0, 128)));
            else if ((vopt->state & QStyle::State_Selected) && proxy()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected, option, widget))
                painter->fillRect(vopt->rect, vopt->palette.brush(cg, QPalette::Highlight));
            else if (vopt->features & QStyleOptionViewItem::Alternate)
                painter->fillRect(vopt->rect, vopt->palette.brush(cg, QPalette::AlternateBase));
        }

        return;
    }

    // В остальных случаях рисуем стандартный стиль
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}
