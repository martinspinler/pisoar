#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QAction>
#include <QMenu>

#include "pisoar.h"
#include "kasuar.h"
#include "jaguar.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    /* Menu */
    enum { MaxRecentFiles = 4 };
    QMenu   *menuFile;
    QMenu   *menuTools;
    QAction *menuNewAct;
    QAction *menuOpenAct;
    QAction *menuSaveAct;
    QAction *menuQuitAct;
    QAction *menuAboutAct;
    QAction *menuSettingsAct;
    QAction *menuGenerateAct;
    QAction *menuBatchAddAct;
    QAction *menuBatchScaleAct;
    QAction *menuCheckIntegrity;
    QAction *menuSeparatorAct;
    QAction *menuRecentFileActs[MaxRecentFiles];

    /* Main layout */
    QTabWidget *tabs;
    Pisoar *pisoar;
    Jaguar *jaguar;
    Kasuar *kasuar;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void updateRecentFileActions();
    void setCurrentFile(const QString &fileName);

private:
    void onMenuFileNew();
    void onMenuFileOpen();
    void onMenuFileSave();
    void onMenuFileOpenRecent();
    void onMenuToolsGenerateLayouts();
    void onMenuToolsBatchAdd();
    void onMenuToolsBatchScale();
    void onMenuToolsCheckIntegrity();
    void onMenuToolsSettings();
    void onMenuToolsAbout();

    void openDir(const QDir &name);
    void closeEvent(QCloseEvent * event); 

public slots:
    void onPisoarDBListActivated(const QModelIndex & index);
    void onJaguarDBListActivated(const QModelIndex & index);
};

#endif // MAINWINDOW_H
