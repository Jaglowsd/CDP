#include "mainwindow.h"
#include "fileselect.h"
#include "updatewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);\
    MainWindow *window = new MainWindow();

    window->show();

    return app.exec();
}
