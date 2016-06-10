#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QAction>
#include <QMenu>

#include "pisoar.h"
#include "kasuar.h"
#include "database.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    /* Menu */
    QMenu   *menuFile;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *quitAct;

    /* Main layout */
    QTabWidget *tabs;
    Pisoar *pisoar;
    Kasuar *kasuar;

    /* Misc */
    Database *database;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void closeEvent(QCloseEvent * event);
};

#endif // MAINWINDOW_H
