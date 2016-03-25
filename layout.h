#ifndef LAYOUT_H
#define LAYOUT_H

#include <QList>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPointF>
#include <QWidget>
#include <QGraphicsTextItem>
#include "database.h"


class Layout : public QGraphicsView
{  
    Q_OBJECT

    Database * db;
    QBrush blackbrush;
    QBrush selectbrush;
    QBrush whitebrush;

    QPointF mouse_down_pos;
    QPointF mouse_move_pos;
    //QGraphicsItem* selected;
    bool isMoving;

    QList<QGraphicsItem*> objects;
    QList<QGraphicsItem*> selected;

    QGraphicsScene *scene;
    QGraphicsTextItem *text_objectList;

    Database::LayoutPage * currentLayout;

    QGraphicsItemGroup * findGroupByItem(Database::LayoutItem * item);

public:
    Layout(Database * database);

    void loadPage(Database::LayoutPage * page);
    void exportToImage(QString filename);

    QGraphicsItemGroup* createObject(Database::LayoutItem *item);
    void addNewObject(Database::LayoutItem *item);
    void recomputeObject(Database::LayoutItem* group);
    void recomputeObject(QGraphicsItemGroup * group);

    QGraphicsItemGroup* addRuler();
    void updateRuler(QGraphicsItemGroup* ruler, float scale);

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *e);

    void selectNone();
    void selectObject(QString name, bool sel = true);
    void selectObject(QPointF pos, bool sel = true);
    void selectObject(QGraphicsItem *item, bool sel = true);
    void selectToggle(QGraphicsItem *item);


    void keyPressEvent(QKeyEvent * event);

    const QList<Database::LayoutItem*> getSelection();

    float ppm;
    int edgew;
    int edgeh;
    int paperw;
    int paperh;
    float dpi;

    float space_multiplier;
    float default_scale;
    int offsetx;
    int offsety;

signals:
    void selectionChanged();
};

#endif // LAYOUT_H
