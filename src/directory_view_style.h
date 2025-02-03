#pragma once

#include <QProxyStyle>
#include <QPainter>

class CustomStyle : public QProxyStyle
{
    Q_OBJECT

public:
    explicit CustomStyle(QStyle *baseStyle = nullptr) : QProxyStyle(baseStyle) {}

    void drawPrimitive(
        PrimitiveElement element,
        const QStyleOption *option,
        QPainter *painter,
        const QWidget* widget = nullptr
    ) const override;
};
