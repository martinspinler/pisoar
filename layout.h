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
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPointF>
#include <QWidget>

#include "database.h"
#include "layoutview.h"

class Layout : public QGraphicsView
{  
    Q_OBJECT

private:
    QList<LayoutView*> objects;

    QGraphicsScene *scene;
    QGraphicsTextItem *text_objectList;

    Database::LayoutPage * currentLayout;

    LayoutView * findViewByItem(Database::LayoutItem * layoutItem);

public:
    Layout();

    void clearLayout();
    void loadPage(Database::LayoutPage * page);
    void exportToImage(QString filename);

    LayoutView* createObject(Database::LayoutItem *item);
    void addNewObject(Database::LayoutItem *item);
    void updateObject(Database::LayoutItem* group);

    void updateText();
    void wheelEvent(QWheelEvent *event);
    void setSelectedBorder(bool border);
    void setSelectedRuler(bool ruler);

    void keyPressEvent(QKeyEvent * event);
    void bakeLayout(Database::LayoutPage* page);

    const QList<Database::LayoutItem*> getSelection();

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
