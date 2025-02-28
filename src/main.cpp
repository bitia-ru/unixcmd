#include "main_window.h"

#include <QApplication>


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;

    QObject::connect(&window, &MainWindow::closed, &app, &QApplication::quit);

    window.show();

    return QApplication::exec();

    return 0;
}
