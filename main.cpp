#include <QApplication>

#include "factory.h"
#include "database.h"
#include "mainwindow.h"

Factory     *f;
Database    *db;

int main(int argc, char *argv[])
{
    int ret;
    MainWindow * w;
    QApplication a(argc, argv);
    a.setOrganizationName("spinler.cz");
    a.setApplicationName("Pisoar");

    f   = new Factory();
    db  = new Database();

    w = new MainWindow;
    w->show();
    ret = a.exec();
    delete w;

    delete db;
    delete f;

    return ret;
}
