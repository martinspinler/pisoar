#ifndef PISOAR_H
#define PISOAR_H

#include <QtCore>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QLabel>
#include <QPixmap>
#include <QToolBar>
#include <QSlider>

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

    friend class MainWindow;

    QDir dir_list;

    QStandardItemModel * fl_list_model;
    QSortFilterProxyModel * db_filter_model;

    ImageFile* fl_file;
    QPixmap m_originalPixmap;
    QPixmap m_selectedPixmap;
    ObjectImage *m_selectedImage;
    QGraphicsPixmapItem * m_previewItem;

    /* Layout - filelist */
    QListView   *fl_list;
    QLabel      *fl_info;
    QPushButton *fl_none;
    QPushButton *fl_wip;
    QPushButton *fl_done;

    /* Layout - image */
    Image       *m_image;
    QToolBar    *image_toolbar;

    /* Layout - database */
    QLineEdit   *db_name;
    QListView   *db_list;
    QPushButton *db_new;
    QPushButton *db_assign;
    QPushButton *db_delete;
    QPushButton *db_generate;
    QPushButton *db_clean;
    QString     *db_id;
    QSlider     *db_rotate;
    QGraphicsView *db_preview;
    QLabel      *db_info;
    QCheckBox   *db_sort;
    QCheckBox   *db_filter;


    /* Layout - containers */
    QHBoxLayout *box_main;
    QVBoxLayout *box_image;
    QVBoxLayout *box_filelist;
    QVBoxLayout *box_database;

    bool        doBatchScale;

public:
    Pisoar(QWidget *parent = 0);
    ~Pisoar();

    void setCurrentDir(QDir dir);
    void batchScale();
    void scaleBatchNext();

private:
    void fl_list_selectionChanged(const QItemSelection &selection);
    void fl_list_activated(const QModelIndex &index);
    void fl_list_fill();
    void fl_setFileFlag_clicked();
    void fl_info_linkActivated(const QString & link);
    void fl_info_update();
    void fl_list_show(int state);

    void db_list_itemChanged(QStandardItem* item);
    void db_list_selectionChanged(const QItemSelection &selection);
    void db_list_fill();
    void db_sort_toggled(bool checked);
    void db_filter_toggled(bool checked);
    void db_filter_edit(const QString &str);
    void db_rotated(int value);

    void db_new_clicked();
    void db_assign_clicked();
    void db_remove_clicked();
    void db_clean_clicked();
    void db_generate_clicked();
    void db_info_update();
    void db_info_linkActivated(const QString & link);

    void onImageSetToolHand() {m_image->setMode(Image::MODE_HAND);}
    void onImageSetToolMask() {m_image->setMode(Image::MODE_MASK);}
    void onImageSetToolRect() {m_image->setMode(Image::MODE_RECT);}
    void onImageSetToolScale(){m_image->setMode(Image::MODE_SCALE);}
    void onImageSetToolColor(){m_image->setMode(Image::MODE_COLOR);}
    void onObjectSelected(ObjectImage *image);
    void onCalibrateDone(QVariant scale);
};

#endif // PISOAR_H
