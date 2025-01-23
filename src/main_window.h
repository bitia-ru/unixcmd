#pragma once

#include <QMainWindow>
#include <QScopedPointer>
#include <QFileInfo>

class DirectoryWidget;


class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    enum ActivePanel
    {
        LEFT,
        RIGHT,
    };

public:
    MainWindow();
    ~MainWindow() override;

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

signals:
    void closed();

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

private:
    struct Private;
    QScopedPointer<Private> d;

    ActivePanel m_activePanel = LEFT;
};
