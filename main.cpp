#include "mainwindow.h"
#include "singleton.h"
#include <QApplication>

#define MAIN_CPP
Singleton *IxTrak;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
