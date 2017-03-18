#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyle>
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>

#include "mainwindow.h"
#include "database.h"
#include "settings.h"
#include "factory.h"

#define createNewMenuAction(act, text, help, key, icon, action) (\
    act = new QAction(tr(text), this),\
    act->setShortcuts(key),\
    act->setStatusTip(tr(help)),\
    act->setIcon(icon),\
    connect(act, &QAction::triggered, this, (action)));

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    Q_INIT_RESOURCE(resources);

    /* Tabs */
    pisoar = new Pisoar();
    jaguar = new Jaguar();
    kasuar = new Kasuar();

    tabs = new QTabWidget();

    tabs->addTab(pisoar, "Přiřazení");
    tabs->addTab(jaguar, "Náhled");
    tabs->addTab(kasuar, "Výstup");

    /* Menu */
    menuFile    = menuBar()->addMenu(tr("&Databáze"));
    menuTools   = menuBar()->addMenu(tr("&Nástroje"));

    createNewMenuAction(menuNewAct,     "&Nová",        "Vytvořit novou databázi",      QKeySequence::New,  f->icon_new,    &MainWindow::onMenuFileNew);
    createNewMenuAction(menuOpenAct,    "&Otevřít",     "Otevřít existující databázi",  QKeySequence::Open, f->icon_open,   &MainWindow::onMenuFileOpen);
    createNewMenuAction(menuSaveAct,    "&Uložit",      "Uložit databázi",              QKeySequence::Save, f->icon_save,   &MainWindow::onMenuFileSave);
    createNewMenuAction(menuQuitAct,    "&Konec",       "Ukončit program",              QKeySequence::Quit, f->icon_close,  &QWidget::close);
    createNewMenuAction(menuGenerateAct,"&Vygenerovat výstupy", "Vygenerovat obrázky pro všechny layouty", QKeySequence::UnknownKey, f->icon_save,  &MainWindow::onMenuToolsGenerateLayouts);
    createNewMenuAction(menuBatchAddAct,"&Přidat více objektů", "Přidat více objektů najednou", QKeySequence::UnknownKey, f->icon_new,      &MainWindow::onMenuToolsBatchAdd);
    createNewMenuAction(menuBatchScaleAct,"&Spustit proces měřítka", "Režim automatického vyhledání fotografií potřebujících měřítko", QKeySequence::UnknownKey, f->icon_ruler,      &MainWindow::onMenuToolsBatchScale);
    createNewMenuAction(menuCheckIntegrity,"&Zkontrolovat integritu", "Zkontroluje databázi a souborový systém", QKeySequence::UnknownKey, f->icon_check,      &MainWindow::onMenuToolsCheckIntegrity);
    createNewMenuAction(menuSettingsAct,"&Nastavení",   "Nastavení parametrů programu", QKeySequence::UnknownKey, f->icon_settings,     &MainWindow::onMenuToolsSettings);
    createNewMenuAction(menuAboutAct,   "&O programu",  "Další informace o programu",   QKeySequence::UnknownKey, f->icon_help,         &MainWindow::onMenuToolsAbout);

    for (int i = 0; i < MaxRecentFiles; ++i) {
        menuRecentFileActs[i] = new QAction(this);
        menuRecentFileActs[i]->setVisible(false);
        connect(menuRecentFileActs[i], &QAction::triggered, this, &MainWindow::onMenuFileOpenRecent);
    }

    menuFile->addAction(menuNewAct);
    menuFile->addAction(menuOpenAct);
    menuFile->addAction(menuSaveAct);
    menuSeparatorAct = menuFile->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        menuFile->addAction(menuRecentFileActs[i]);
    menuFile->addSeparator();
    menuFile->addAction(menuQuitAct);
    updateRecentFileActions();

    menuTools->addAction(menuGenerateAct);
    menuTools->addAction(menuSettingsAct);
    menuTools->addAction(menuBatchAddAct);
    menuTools->addAction(menuBatchScaleAct);
    menuTools->addAction(menuCheckIntegrity);
    menuTools->addSeparator();
    menuTools->addAction(menuAboutAct);

    /* Hide tabs when no database */
    tabs->hide();

    connect(pisoar->db_list, &QListView::activated, this, &MainWindow::onPisoarDBListActivated);
    connect(jaguar->db_list, &QListView::activated, this, &MainWindow::onJaguarDBListActivated);

    /* Main window */
    setWindowTitle("Pisoar - PIcture SOrter for ARchaelogy [*]");
    setCentralWidget(tabs);
    showMaximized();
}
MainWindow::~MainWindow()
{
}

void MainWindow::onMenuFileNew()
{
    QFileDialog fd(this);
    fd.setFileMode(fd.DirectoryOnly);
    if(fd.exec() == QDialog::Rejected)
        return;

    if(db->open(fd.directory())) {
        QMessageBox msgBox;
        msgBox.setText("Existujcí databáze");
        msgBox.setInformativeText("Opravdu chcete přepsat databázi a začít novou?");
        msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                break;
            default:
                return;
        }
    }

    if(db->create(fd.directory())) {
        pisoar->setCurrentDir(db->getDirBase());
        tabs->show();
    }
    else
        tabs->hide();
}

void MainWindow::onMenuFileOpen()
{
    QFileDialog fd(this);
    fd.setFileMode(fd.DirectoryOnly);
    if(fd.exec() == QDialog::Accepted)
        openDir(fd.directory());
}

void MainWindow::openDir(const QDir & dir)
{
    if(db->open(dir)) {
        setCurrentFile(dir.path());
        pisoar->setCurrentDir(db->getDirBase());
        tabs->show();
    }
    else
        tabs->hide();
}

void MainWindow::onMenuFileSave()
{
    db->save();
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if(!db->modified()) {
        event->accept();
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Uložit změny");
    msgBox.setText("Chcete uložit změny v databázi?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
        case QMessageBox::Save:
            db->save();
            event->accept();
            break;
        case QMessageBox::Discard:
            event->accept();
            break;
        default:
            event->ignore();
    }
}

void MainWindow::onMenuToolsBatchAdd()
{
    QInputDialog id;
    id.setWindowTitle("Více objektů");
    id.setLabelText("Vložte názvy objektů, každý na samostatný řádek");
    id.setOption(QInputDialog::UsePlainTextEditForTextInput);
    id.exec();
    QStringList lines = id.textValue().split("\n");
    foreach (QString line, lines){
        if(!line.isEmpty())
            db->createObject(line);
    }
}

void MainWindow::onMenuToolsGenerateLayouts()
{
    kasuar->bakeLayouts();
}

void MainWindow::onMenuToolsBatchScale()
{
    pisoar->batchScale();
}

void MainWindow::onMenuToolsAbout()
{
    QMessageBox::about(this, "O programu Pisoar", QString(
        "Program Pisoar verze %1.%2"        "\n"
        "Vytvořil Martin Špinler"           "\n"
        "Chyby a požadavky reportujte na:"  "\n"
        "martin.spinler@gmail.com"
        ).arg(PISOAR_CURRENT_VERSION >> 16).arg(PISOAR_CURRENT_VERSION & 0xFFFF)
    );
}
void MainWindow::onMenuToolsSettings()
{
    db->set.exec();
}

void MainWindow::onMenuFileOpenRecent()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        openDir(QDir(action->data().toString()));
}
void MainWindow::setCurrentFile(const QString &fileName)
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();
    settings.setValue("recentFileList", files);

    setWindowFilePath(fileName);
    updateRecentFileActions();
}
void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(files[i]);
        menuRecentFileActs[i]->setText(text);
        menuRecentFileActs[i]->setData(files[i]);
        menuRecentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        menuRecentFileActs[j]->setVisible(false);

    menuSeparatorAct->setVisible(numRecentFiles > 0);
}

void MainWindow::onMenuToolsCheckIntegrity()
{
    for(int i = 0; i < db->object_model.rowCount(); i++) {
        Database::ObjectItem * item = (Database::ObjectItem *) db->object_model.item(i);
        for(int j = 0; j < item->images.size(); j++) {
            QFile file(db->getDirBase().filePath(item->images[j]->path()));
            if(!file.open(QFile::OpenModeFlag::ReadOnly)) {
                qInfo() << "Missing " << item->images[j]->path() << " for item " << item->text();
            }
        }
    }
}

void MainWindow::onPisoarDBListActivated(const QModelIndex & index)
{
    QModelIndex db_index = pisoar->db_filter_model->mapToSource(index);
    QModelIndex fg_index = jaguar->db_filter_model->mapFromSource(db_index);
    tabs->setCurrentIndex(1);
    jaguar->db_filter_model->setFilterWildcard("");
    jaguar->db_list->selectionModel()->setCurrentIndex(fg_index, QItemSelectionModel::ClearAndSelect);
}

void MainWindow::onJaguarDBListActivated(const QModelIndex & index)
{
    QModelIndex db_index = jaguar->db_filter_model->mapToSource(index);
    QModelIndex fg_index = pisoar->db_filter_model->mapFromSource(db_index);
    tabs->setCurrentIndex(0);
    pisoar->db_filter_model->setFilterWildcard("");
    pisoar->db_list->selectionModel()->setCurrentIndex(fg_index, QItemSelectionModel::ClearAndSelect);
}
