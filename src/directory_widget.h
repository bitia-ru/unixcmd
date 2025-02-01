#pragma once

#include <QScopedPointer>
#include <QWidget>


class DirectoryView;

class DirectoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DirectoryWidget(QWidget* parent = nullptr);
    ~DirectoryWidget() override;

    DirectoryView* view() const;

private:
    struct Private;
    QScopedPointer<Private> d;
};