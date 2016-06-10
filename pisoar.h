#ifndef PISOAR_H
#define PISOAR_H

#include <QtCore>
#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QLabel>
#include <QPixmap>

#include <QList>
#include <QString>
#include <QListWidgetItem>
#include <QModelIndexList>
#include <QPainter>

#include "image.h"
#include "database.h"
#include "kasuar.h"
#include "settings.h"

class Pisoar : public QWidget
{
    Q_OBJECT

    Settings *settings;
    Database * db;
    QDir dir_list;

    QString fl_filename;
    QPixmap obj_selected;

    /* Layout - filelist */
    QListWidget *fl_list;
    QLabel      *fl_info;

    QPushButton *fl_calib;
    QPushButton *fl_none;
    QPushButton *fl_wip;
    QPushButton *fl_done;

    /* Layout - image */
    Image       *image;

    /* Layout - database */
    QLineEdit   *db_name;
    QListView   *db_list;
    QPushButton *db_new;
    QPushButton *db_assign;
    QPushButton *db_remove;
    QPushButton *db_clean;
    QString     *db_id;
    QLabel      *db_preview;
    QLabel      *db_info;

    /* Layout - containers */
    QHBoxLayout *box_main;
    QVBoxLayout *box_filelist;
    QVBoxLayout *box_database;

public:
    Pisoar(Database * db, QWidget *parent = 0);
    ~Pisoar();

    void fl_list_itemSelectionChanged(const QItemSelection &selection);
    void fl_list_itemActivated();
    void fl_list_fill();
    void fl_calib_clicked();
    void fl_show_stateChanged(int state);
    void fl_setFileFlag_clicked();

    void db_list_activated(const QModelIndex &index);
    void db_list_itemChanged(QStandardItem* item);
    void db_list_selectionChanged(const QItemSelection &selection);
    void db_list_fill();

    void db_save_clicked();
    void db_kasuar_clicked();
    void db_new_clicked();
    void db_assign_clicked();
    void db_remove_clicked();
    void db_clean_clicked();
    void db_info_update();

    void assign_mask();

    void setCurrentDir(QDir dir);

//public slots:
    void onObjectSelected();
    void onCalibrateDone(QVariant scale);
};

#endif // PISOAR_H
