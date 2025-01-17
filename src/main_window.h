#pragma once

#include <QMainWindow>
#include <QScopedPointer>
#include <QFileInfo>

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
    [[nodiscard]] DirectoryWidget* destinationPanelWidget() const;
    void toggleActivePanel();
    void viewSelection();
    void editSelection();
    void copySelection();
    void removeSelected();
    void open(const QFileInfo& fileInfo);

    [[nodiscard]] QList<QFileInfo> selectedFiles() const;

    struct Private;
    QScopedPointer<Private> d;

    ActivePanel m_activePanel = LEFT;
};
