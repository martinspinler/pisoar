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
    QRectF m_br;

    LayoutRuler       * rruler;
    QGraphicsRectItem * rselect;
    QGraphicsTextItem * rscale;

protected:
    QPixmap             *pix;
    QGraphicsPixmapItem *pixitem;

    int m_pixmaps;
    float childWidth;
    float childHeight;
    float childPadding;

    QRectF doUniversalTransform(int i, float scale, int rotation);

public:
    QGraphicsTextItem * rindex;
    QGraphicsRectItem * rrect;

    Database::LayoutItem * layoutItem;
    virtual void    updateObject();

public:
    LayoutView(Database::LayoutItem *li, int pixmaps = 0);
    virtual ~LayoutView();

    virtual void    setObjectScale(float scale);
    virtual float   objectScale();
    virtual QRectF  boundingRect() const {return m_br;}
    virtual void    paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {}
    virtual void    addToGroup(QGraphicsItem*i) {i->setParentItem(this);}
    QVariant        itemChange(GraphicsItemChange change, const QVariant &value);

    static LayoutView * createView(Database::LayoutItem* item);
};

class LayoutTopSideView: public LayoutView {
    QGraphicsLineItem * rlinetop;
    QGraphicsLineItem * rlinebot;
public:
    LayoutTopSideView(Database::LayoutItem*item);
    void updateObject();
};

class LayoutTopBottomView: public LayoutView {
    QGraphicsLineItem * rlineleft;
    QGraphicsLineItem * rlineright;
public:
    LayoutTopBottomView(Database::LayoutItem*item);
    void updateObject();
};

class LayoutTopSideFrontView: public LayoutView {
    QGraphicsLineItem * rlineleft;
    QGraphicsLineItem * rlineright;
    QGraphicsLineItem * rlinetop;
    QGraphicsLineItem * rlinebot;
public:
    LayoutTopSideFrontView(Database::LayoutItem*item);
    void updateObject();
};

class LayoutTopView: public LayoutView {
public:
    LayoutTopView(Database::LayoutItem*item);
    void updateObject();
};

#endif // LAYOUTVIEW_H
