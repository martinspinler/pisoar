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

    QSortFilterProxyModel * filter;
    JaguarView  *view;
    QGraphicsScene*scene;
    QListView   *db_list;
    QLineEdit   *db_filter;
    QLabel      *db_imagePreview;

    LayoutView  *layoutView;

    QHBoxLayout *box_main;
    QVBoxLayout *box_database;
    QVBoxLayout *box_image;

    //QGraphicsRectItem * sceneRect;

public:
    explicit Jaguar(QWidget *parent = 0);
    ~Jaguar();

    void filter_edit(const QString &str);
    void db_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

signals:

public slots:
};

#endif // JAGUAR_H
