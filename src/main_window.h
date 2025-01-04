#pragma once

#include <QMainWindow>
#include <QScopedPointer>

class DirectoryWidget;


class MainWindow : public QMainWindow
{
public:
    enum ActivePanel
    {
        LEFT,
        RIGHT,
    };

public:
    MainWindow();
    virtual ~MainWindow();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setActivePanel(ActivePanel panel);
    [[nodiscard]] DirectoryWidget* activePanelWidget() const;
    void toggleActivePanel();

    struct Private;
    QScopedPointer<Private> d;

    ActivePanel m_activePanel = LEFT;
};
