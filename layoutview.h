#ifndef LAYOUTVIEW_H
#define LAYOUTVIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QVariant>

#include "database.h"

namespace LayoutViews {

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

    int m_mainScaleIndex;
    int m_pixmaps;
    float childWidth;
    float childHeight;
    float childPadding;

    QRectF doUniversalTransform(int i, float scale, int rotation);
    QRectF doRotationTransform(int i, int rotation);
    QRectF doScaleTransform(int i, float scale);
    QRectF doClearTransform(int i);

public:
    QGraphicsTextItem * rindex;
    QGraphicsRectItem * rrect;

    LayoutItem * layoutItem;
    virtual void    updateObject();

public:
    LayoutView(LayoutItem *li, int pixmaps = 0);
    virtual ~LayoutView();

    virtual void    setObjectScale(float scale);
    virtual float   objectScale();
    virtual QRectF  boundingRect() const {return m_br;}
    virtual void    paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {}
    virtual void    addToGroup(QGraphicsItem*i) {i->setParentItem(this);}
    QVariant        itemChange(GraphicsItemChange change, const QVariant &value);

    void setBorder(bool border) {layoutItem->setBorder(border);updateObject();}
    void setRuler(bool ruler)   {layoutItem->setRuler(ruler);updateObject();}
    bool border()               {return layoutItem->border();}
    bool ruler()                {return layoutItem->ruler();}

    /*void link(LayoutPage*page)  {layoutItem->link(page);}
    void unlink(LayoutPage*page){layoutItem->unlink(page);}*/
};

class LayoutTopSideView: public LayoutView {
    QGraphicsLineItem * rlinetop;
    QGraphicsLineItem * rlinebot;
public:
    LayoutTopSideView(LayoutItem*item);
    void updateObject();
};

class LayoutTopBottomView: public LayoutView {
    QGraphicsLineItem * rlineleft;
    QGraphicsLineItem * rlineright;
public:
    LayoutTopBottomView(LayoutItem*item);
    void updateObject();
};

class LayoutTopSideFrontView: public LayoutView {
    QGraphicsLineItem * rlineleft;
    QGraphicsLineItem * rlineright;
    QGraphicsLineItem * rlinetop;
    QGraphicsLineItem * rlinebot;
public:
    LayoutTopSideFrontView(LayoutItem*item);
    void updateObject();
};

class LayoutRTopSideFrontView: public LayoutView {
    QGraphicsLineItem * rlineleft;
    QGraphicsLineItem * rlineright;
    QGraphicsLineItem * rlinetop;
    QGraphicsLineItem * rlinebot;
public:
    LayoutRTopSideFrontView(LayoutItem*item);
    void updateObject();
};

class LayoutTopView: public LayoutView {
public:
    LayoutTopView(LayoutItem*item);
    void updateObject();
};

}
#endif // LAYOUTVIEW_H
