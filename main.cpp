#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets>
#define W 480
#define H 800
int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_QPA_FONTDIR", QByteArray("/usr/share/fonts/dejavu/"));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
