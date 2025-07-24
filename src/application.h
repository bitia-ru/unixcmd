#pragma once

#include <QApplication>

class Application : public QApplication {
    Q_OBJECT

public:
    Application(int& argc, char** argv);

    static bool shouldStartFromUserHome();

private:
    static bool isRunningFromTerminal();
};