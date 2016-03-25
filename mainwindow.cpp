#include <QMenuBar>
#include <QFileDialog>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    Q_INIT_RESOURCE(resources);

    /* Base objects */
    database = new Database();
    kasuar = new Kasuar(database);
    pisoar = new Pisoar(database);

    /* Tabs */
    tabs = new QTabWidget();
    tabs->addTab(pisoar, "Pisoar");
    tabs->addTab(kasuar, "Kasuar");

    /* Menu */
    menuFile = menuBar()->addMenu(tr("&Databáze"));

    newAct = new QAction(tr("&Nová"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Vytvořit novou databázi"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction(tr("&Otevřít"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Otevřít existující databázi"));
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

    saveAct = new QAction(tr("&Uložit"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Uložit databázi"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    quitAct = new QAction(tr("&Konec"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Ukončit program"));
    connect(quitAct, &QAction::triggered, this, &MainWindow::quitKasuar);

    menuFile->addAction(newAct);
    menuFile->addAction(openAct);
    menuFile->addAction(saveAct);
    menuFile->addAction(quitAct);

    /* Main window */
    setWindowTitle("Pisoar - Photo Image SOrter for ARchaelogy");
    setCentralWidget(tabs);
    showMaximized();

    /* Try to open database in predefined paths */
    const char * base_dirs[] = { "D:\\anez\\", "/maestro/work/pisoar/photos", "/maestro/dismas/photos" };
    for(unsigned int i = 0; i < sizeof(base_dirs) / sizeof(base_dirs[0]); i++) {
        if(database->open(QDir(base_dirs[i]))) {
            pisoar->setCurrentDir(database->getDirBase());
            break;
        }
    }
}
MainWindow::~MainWindow()
{
    delete database;
}

void MainWindow::newFile()
{
    QFileDialog fd(this);
    fd.setFileMode(fd.DirectoryOnly);
    fd.exec();
    database->create(fd.directory());
    pisoar->setCurrentDir(database->getDirBase());
}

void MainWindow::openFile()
{
    QFileDialog fd(this);
    fd.setFileMode(fd.DirectoryOnly);
    fd.exec();
    if(database->open(fd.directory())) {
        pisoar->setCurrentDir(database->getDirBase());
    }
}

void MainWindow::saveFile()
{
    database->save();
}

void MainWindow::quitKasuar()
{
    close();
}

