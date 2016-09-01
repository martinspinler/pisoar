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
#include <QToolBar>

#include <QList>
#include <QString>
#include <QListWidgetItem>
#include <QModelIndexList>
#include <QPainter>
#include <QVariant>

#include "image.h"
#include "database.h"
#include "kasuar.h"
#include "settings.h"

class Pisoar : public QWidget
{
    Q_OBJECT

    QDir dir_list;

    QStandardItemModel * fl_list_model;

    Database::ImageFile* fl_file;
    Database::ImageFile* mask_file;
    QPixmap obj_selected;
    QVariant selectedView;

    /* Layout - filelist */
    QListView   *fl_list;
    QLabel      *fl_info;
    QPushButton *fl_none;
    QPushButton *fl_wip;
    QPushButton *fl_done;

    /* Layout - image */
    Image       *image;
    QToolBar    *image_toolbar;

    /* Layout - database */
    QLineEdit   *db_name;
    QListView   *db_list;
    QPushButton *db_new;
    QPushButton *db_assign;
    QPushButton *db_remove;
    QPushButton *db_generate;
    QPushButton *db_clean;
    QString     *db_id;
    QLabel      *db_preview;
    QLabel      *db_info;

    /* Layout - containers */
    QHBoxLayout *box_main;
    QVBoxLayout *box_image;
    QVBoxLayout *box_filelist;
    QVBoxLayout *box_database;

public:
    Pisoar(QWidget *parent = 0);
    ~Pisoar();

    void setCurrentDir(QDir dir);

    void fl_list_selectionChanged(const QItemSelection &selection);
    void fl_list_activated(const QModelIndex &index);
    void fl_list_fill();
    void fl_show_stateChanged(int state);
    void fl_setFileFlag_clicked();
    void fl_info_linkActivated(const QString & link);
    void fl_info_update();

    void db_list_itemChanged(QStandardItem* item);
    void db_list_selectionChanged(const QItemSelection &selection);
    void db_list_fill();

    void db_save_clicked();
    void db_kasuar_clicked();
    void db_new_clicked();
    void db_assign_clicked();
    void db_remove_clicked();
    void db_clean_clicked();
    void db_generate_clicked();
    void db_info_update();
    void db_info_linkActivated(const QString & link);

    void onImageSetToolHand() {image->setMode(Image::MODE_HAND);}
    void onImageSetToolMask() {image->setMode(Image::MODE_MASK);}
    void onImageSetToolRect() {image->setMode(Image::MODE_RECT);}
    void onImageSetToolScale(){image->setMode(Image::MODE_SCALE);}
    void onImageSetToolColor(){image->setMode(Image::MODE_COLOR);}
    void onObjectSelected(QVariant obj);
    void onCalibrateDone(QVariant scale);
};

#endif // PISOAR_H
