#ifndef LAYOUT_H
#define LAYOUT_H

#include <QList>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPointF>
#include <QWidget>

#include "database.h"
#include "layoutview.h"

using namespace LayoutViews;

class Layout : public QGraphicsView
{  
    Q_OBJECT

private:
    QList<LayoutView*> objects;

    QGraphicsScene *scene;
    QGraphicsTextItem *text_objectList;
    QGraphicsRectItem * edgeRect;
    QGraphicsRectItem * mainRect;

    LayoutPage * currentLayout;

public:
    Layout();

    void clearLayout();
    void loadPage(LayoutPage * page);
    void exportToImage(QString filename);

    LayoutView* createObject(LayoutItem *item);
    void addNewObject(LayoutItem *item);

    void updateText();
    void wheelEvent(QWheelEvent *event);
    void toggleBorder();
    void toggleRuler();

    void keyPressEvent(QKeyEvent * event);
    void bakeLayout(LayoutPage* page);

    const QList<LayoutItem*> getSelection();

    int edgew;
    int edgeh;
    int paperw;
    int paperh;

    float space_multiplier;
    float default_scale;
    int offsetx;
    int offsety;
    int alignment;
};

#endif // LAYOUT_H
