#ifndef LAYOUTVIEW_H
#define LAYOUTVIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QVariant>

class Database;

#include "database.h"

class LayoutRuler : public QGraphicsItemGroup {
public:
    LayoutRuler();
    void updateObject(float scale);
};

class LayoutView: public QGraphicsItem {
protected:
    float childWidth;
    float childHeight;
    float childPadding;

    QRectF br;
public:
    Database::LayoutItem * layoutItem;

    LayoutView(Database::LayoutItem *li);
    virtual void updateObject();

    virtual void setObjectScale(float scale);
    virtual float objectScale();
    virtual QRectF boundingRect() const {return br;}
    virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {}
    virtual void addToGroup(QGraphicsItem*i) {i->setParentItem(this);}
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    QGraphicsRectItem * rrect;
    LayoutRuler       * rruler;
    QGraphicsRectItem * rselect;
    QGraphicsTextItem * rscale;
    QGraphicsTextItem * rindex;
};

class LayoutTopSideView: public LayoutView {
    QGraphicsPixmapItem * rtop;
    QGraphicsPixmapItem * rside;
    QGraphicsLineItem * rlinetop;
    QGraphicsLineItem * rlinebot;
public:
    LayoutTopSideView(Database::LayoutItem*item);
    void updateObject();
};

class LayoutTopBottomView: public LayoutView {
    QGraphicsPixmapItem * rtop;
    QGraphicsPixmapItem * rside;
    QGraphicsLineItem * rlineleft;
    QGraphicsLineItem * rlineright;
public:
    LayoutTopBottomView(Database::LayoutItem*item);
    void updateObject();
};

class LayoutTopView: public LayoutView {
    QGraphicsPixmapItem * rtop;
public:
    LayoutTopView(Database::LayoutItem*item);
    void updateObject();
};

#endif // LAYOUTVIEW_H
