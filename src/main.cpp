#include "main_window.h"

#include <QApplication>


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;

    window.show();

    return QApplication::exec();
}
