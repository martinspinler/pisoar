#include "pisoar.h"

#include <QApplication>
#include <QIcon>
#include <QDir>

#include <QStringList>
#include <iostream>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
