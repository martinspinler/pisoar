#ifndef JAGUAR_H
#define JAGUAR_H

#include <QWidget>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QWheelEvent>
#include <QItemSelection>
#include <QPushButton>

#include "layoutview.h"

class JaguarView : public QGraphicsView
{
public:
    JaguarView() {}
    void wheelEvent(QWheelEvent *event);
};

class Jaguar : public QWidget
{
    Q_OBJECT

    friend class MainWindow;

    QSortFilterProxyModel * db_filter_model;
    QStandardItemModel * model_images;
    QStandardItemModel * model_types;
    QStandardItemModel * model_views;
    JaguarView  *view;
    QGraphicsScene*scene;

    QListView   *db_list;
    QListView   *db_viewlist;
    QLineEdit   *db_filter;
    QLabel      *db_imagePreview;
    QCheckBox   *db_sort;

    QListView   *list_types;
    QListView   *list_images;
    QPushButton *images_moveup;
    QPushButton *images_movedown;
    QPushButton *images_rotateleft;
    QPushButton *images_rotateright;

    QHBoxLayout *box_main;
    QVBoxLayout *box_database;
    QVBoxLayout *box_image;
    QVBoxLayout *box_images;
    QHBoxLayout *box_imagesButtons;

    LayoutView  *layoutView;
    Database::ObjectView  *objectView;

public:
    explicit Jaguar(QWidget *parent = 0);
    ~Jaguar();

    void clear();
    void setView(Database::ObjectView * objectView);
    void filter_edit(const QString &str);
    void db_sort_toggled(bool checked);
    void db_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void view_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void types_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

signals:

public slots:
    void images_move_clicked();
    void images_rotate_clicked();
};

#endif // JAGUAR_H
