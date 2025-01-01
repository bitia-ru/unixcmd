#include "double_panel_splitter.h"
#include <QMouseEvent>


class DoublePanelSplitterHandle : public QSplitterHandle
{
public:
    DoublePanelSplitterHandle(Qt::Orientation orientation, QSplitter* parent)
        : QSplitterHandle(orientation, parent)
    {
    }

    void mouseDoubleClickEvent(QMouseEvent *event) override
    {
        qobject_cast<DoublePanelSplitter*>(splitter())->resetState();
    }
};


DoublePanelSplitter::DoublePanelSplitter(QWidget* parent)
    : QSplitter(parent)
{
    installEventFilter(this);
}

void DoublePanelSplitter::resetState()
{
    Q_ASSERT(count() == 2);

    const auto width = this->width();
    setSizes({width/2, width/2});
}

QSplitterHandle* DoublePanelSplitter::createHandle()
{
    return new DoublePanelSplitterHandle(orientation(), this);
}
