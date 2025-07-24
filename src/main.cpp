#include "main_window.h"
#include "application.h"

int main(int argc, char* argv[]) {
    Application app(argc, argv);

    MainWindow window;

    QObject::connect(&window, &MainWindow::closed, &app, &Application::quit);

    window.show();

    return app.exec();
}
