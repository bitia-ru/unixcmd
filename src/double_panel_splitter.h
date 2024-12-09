#pragma once

#include <QSplitter>


class DoublePanelSplitter : public QSplitter
{
    Q_OBJECT

public:
    DoublePanelSplitter(QWidget* parent = nullptr);

    void resetState();

protected:
    QSplitterHandle* createHandle() override;
};
