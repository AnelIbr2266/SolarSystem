#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setApplicationName("Солнечная система");
    application.setApplicationDisplayName("Солнечная система");
    MainWindow window;
    window.show();
    return application.exec();
}
