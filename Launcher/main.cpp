#include "mainwindow.h"

#include <QApplication>

#include "configreader.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ConfigReader::read();
    MainWindow w;
    w.show();
    return a.exec();
}
