#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile aa("style.css");
    aa.open(QIODevice::ReadOnly);

    QString style = aa.readAll();
    a.setStyleSheet(style);

    MainWindow w;
    w.show();

    return a.exec();
}
