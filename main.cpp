#include "include/mainwindow.h"

#include <QApplication>
#include <QWidget>
#include <QPainter>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("BookShop");
    w.show();

    return a.exec();
}
