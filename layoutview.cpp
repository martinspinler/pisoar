#include <cmath>
#include <QApplication>

#include "factory.h"
#include "layoutview.h"

LayoutView * LayoutView::createView(Database::LayoutItem* item)
{
    LayoutView * view = NULL;
    switch(item->objectView()->type()) {
        case 0: view = new LayoutView(item); break;
        case 1: view = new LayoutTopView(item); break;
        case 2: view = new LayoutTopSideView(item); break;
        case 3: view = new LayoutTopBottomView(item); break;
        case 4: view = new LayoutTopSideFrontView(item); break;
    }
    return view;
}
LayoutRuler::LayoutRuler()
{
    QGraphicsRectItem * r;
    QGraphicsTextItem * t;
    r = new QGraphicsRectItem();                                addToGroup(r);
    r = new QGraphicsRectItem();    r->setBrush(f->blackbrush); addToGroup(r);
    r = new QGraphicsRectItem();    r->setBrush(f->blackbrush); addToGroup(r);
    r = new QGraphicsRectItem();    r->setBrush(f->blackbrush); addToGroup(r);
    t = new QGraphicsTextItem("0");                             addToGroup(t);
    t = new QGraphicsTextItem("5 cm");                          addToGroup(t);
}
void LayoutRuler::updateObject(float scale)
{
    float mmscale = scale *  db->set.ppm;
    ((QGraphicsRectItem *)childItems()[0])->setRect(mmscale*00, 0, mmscale*50, 5*db->set.ppm);
    ((QGraphicsRectItem *)childItems()[1])->setRect(mmscale*00, 0, mmscale*10, 5*db->set.ppm);
    ((QGraphicsRectItem *)childItems()[2])->setRect(mmscale*20, 0, mmscale*10, 5*db->set.ppm);
    ((QGraphicsRectItem *)childItems()[3])->setRect(mmscale*40, 0, mmscale*10, 5*db->set.ppm);
    ((QGraphicsTextItem *)childItems()[4])->setPos(mmscale*-5, 7*db->set.ppm);
    ((QGraphicsTextItem *)childItems()[5])->setPos(mmscale*45, 7*db->set.ppm);
}

LayoutView::LayoutView(Database::LayoutItem *li, int pixmaps) : layoutItem(li)
{
    childPadding = db->set.ppm * 5;
    m_pixmaps = pixmaps;

    addToGroup((rruler      = new LayoutRuler()));
    addToGroup((rselect     = new QGraphicsRectItem()));
    addToGroup((rrect       = new QGraphicsRectItem()));
    addToGroup((rscale      = new QGraphicsTextItem()));
    addToGroup((rindex      = new QGraphicsTextItem()));

    rscale->hide();
    rselect->hide();
    rselect->setBrush(f->selectbrush);

    if(m_pixmaps) {
        pix = new QPixmap[m_pixmaps];
        pixitem = new QGraphicsPixmapItem[m_pixmaps];
        for(int i = 0; i < m_pixmaps; i++) {
            QString path = layoutItem->objectView()->item.filename() + QString("_%1.png").arg(layoutItem->objectView()->mapping(i));
            pix[i].load(db->getDirItems().filePath(path));
            pixitem[i].setPixmap(pix[i]);
            pixitem[i].setZValue(1);
            addToGroup(&pixitem[i]);
        }
    }
}
LayoutView::~LayoutView()
{
    if(m_pixmaps){
        for(int i = 0; i < m_pixmaps; i++) {
            pixitem[i].setParentItem(NULL);
        }
        delete [] pixitem;
        delete [] pix;
    }
}

void LayoutView::updateObject()
{
    float rulerHeight  = layoutItem->ruler() ? rruler->sceneBoundingRect().height() + childPadding : 0;
    float rulerWidth   = db->set.ppm * 50 * layoutItem->scale();

    rselect ->setRect(childPadding, childPadding, childWidth, childHeight);
    rrect   ->setRect(0, 0, childWidth + 2*childPadding, childHeight + 2*childPadding + rulerHeight);
    rruler  ->setPos(childPadding + childWidth/2 - rulerWidth/2, childPadding*1.5 + childHeight);
    rindex  ->setPos(0, childHeight);
    rscale  ->setPlainText("Scale: 1x");
    rrect   ->setVisible(layoutItem->border());
    rruler  ->setVisible(layoutItem->ruler());
    rruler  ->updateObject(layoutItem->scale());

    setPos(layoutItem->pos());
    prepareGeometryChange();
    m_br = childrenBoundingRect();
}

QRectF LayoutView::doUniversalTransform(int i, float scale, int rotation)
{
    QRectF br;
    QTransform tr;
    tr.reset();
    tr.rotate(rotation);
    tr.scale(scale, scale);
    br = tr.mapRect(pixitem[i].boundingRect());

    pixitem[i].setTransform(tr);
    pixitem[i].setPos(-br.left(), -br.top());
    return br;
}

void LayoutView::setObjectScale(float scale)
{
    layoutItem->setScale(scale);
    db->setModified();
    updateObject();
    //rscale->setPlainText(QString("Scale: %1x").arg(QString::number(view->scale()*10)));
}
float LayoutView::objectScale()
{
    return layoutItem->scale();
}

QVariant LayoutView::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged){
        rselect->setVisible(value.toBool());
    }
    else if (change == ItemPositionHasChanged) {
        layoutItem->setPos(pos());
        db->setModified();
    }
    else if (change == ItemScaleHasChanged) {

    }
    else if (change == ItemPositionChange && QApplication::keyboardModifiers() & Qt::ControlModifier) {
        int alignment = 10 * db->set.ppm;
        QPointF newPos = value.toPointF();
        newPos.setX(round((newPos.x())/alignment)*alignment);
        newPos.setY(round((newPos.y())/alignment)*alignment);
        return newPos;
    }
    return QGraphicsItem::itemChange(change, value);
}

LayoutTopSideView::LayoutTopSideView(Database::LayoutItem *i) : LayoutView(i, 2)
{
    addToGroup((rlinetop    = new QGraphicsLineItem()));
    addToGroup((rlinebot    = new QGraphicsLineItem()));

    rlinetop->setZValue(1);
    rlinebot->setZValue(1);

    updateObject();
}

void LayoutTopSideView::updateObject()
{
    const bool swapside = false;
    const int LEFT = !swapside ? 1 : 0;

    float width[m_pixmaps];
    float height[m_pixmaps];

    float space     = 10 * db->set.ppm * layoutItem->scale();
    float scale  = db->set.ppm / (layoutItem->objectView()->item.images.first()->scale() / 10) * layoutItem->scale(); // pixels per mm

    QRectF br;

    for (int i = 0; i < m_pixmaps; i++) {
        switch(i) {
        case 1: scale = height[0] / pix[i].height(); break;
        }

        br = doUniversalTransform(i, scale, layoutItem->objectView()->rotation(i));
        width[i]    = br.width();
        height[i]   = br.height();
    }

    pixitem[0].moveBy(childPadding + (!swapside ? width[LEFT] + space : 0), childPadding);
    pixitem[1].moveBy(childPadding + ( swapside ? width[LEFT] + space : 0), childPadding);

    childHeight = height[0];
    childWidth  = width[0] + space + width[1];

    rlinetop->setLine(childPadding + width[LEFT] + 5, childPadding,               childPadding + width[LEFT] + space - 5, childPadding);
    rlinebot->setLine(childPadding + width[LEFT] + 5, childPadding + childHeight, childPadding + width[LEFT] + space - 5, childPadding + childHeight);
    LayoutView::updateObject();
}

LayoutTopBottomView::LayoutTopBottomView(Database::LayoutItem *i) : LayoutView(i, 2)
{
    addToGroup((rlineleft   = new QGraphicsLineItem()));
    addToGroup((rlineright  = new QGraphicsLineItem()));

    rlineleft->setZValue(1);
    rlineright->setZValue(1);

    updateObject();
}

void LayoutTopBottomView::updateObject()
{
    bool swapside = false;
    const int TOP = swapside ? 1 : 0;

    float width[m_pixmaps];
    float height[m_pixmaps];

    float space  = 10 * db->set.ppm * layoutItem->scale();
    float scale  = db->set.ppm / (layoutItem->objectView()->item.images.first()->scale() / 10) * layoutItem->scale(); // pixels per mm

    QRectF br;

    for (int i = 0; i < m_pixmaps; i++) {
        switch(i) {
        case 1: scale = width[0] / pix[i].width(); break;
        }

        br = doUniversalTransform(i, scale, layoutItem->objectView()->rotation(i));
        width[i]    = br.width();
        height[i]   = br.height();
    }

    pixitem[0].moveBy(childPadding, childPadding + ( swapside ? height[TOP] + space : 0));
    pixitem[1].moveBy(childPadding, childPadding + (!swapside ? height[TOP] + space : 0));

    childWidth  = width[0];
    childHeight = height[0] + space + height[1];

    rlineleft ->setLine(childPadding,              childPadding + height[TOP] + 5, childPadding,              childPadding + height[TOP] + space - 5);
    rlineright->setLine(childPadding + childWidth, childPadding + height[TOP] + 5, childPadding + childWidth, childPadding + height[TOP] + space - 5);

    LayoutView::updateObject();
}
LayoutTopSideFrontView::LayoutTopSideFrontView(Database::LayoutItem *i) : LayoutView(i, 3)
{
    addToGroup((rlineleft   = new QGraphicsLineItem()));
    addToGroup((rlineright  = new QGraphicsLineItem()));
    addToGroup((rlinetop    = new QGraphicsLineItem()));
    addToGroup((rlinebot    = new QGraphicsLineItem()));

    rlineleft->setZValue(1);
    rlineright->setZValue(1);
    rlinetop->setZValue(1);
    rlinebot->setZValue(1);

    updateObject();
}

void LayoutTopSideFrontView::updateObject()
{
    bool swapside   = true;
    const int LEFT = !swapside ? 1 : 0;

    float width[m_pixmaps];
    float height[m_pixmaps];

    float space  = 10 * db->set.ppm * layoutItem->scale();
    float scale  = db->set.ppm / (layoutItem->objectView()->item.images.first()->scale() / 10) * layoutItem->scale(); // pixels per mm

    QRectF br;

    for (int i = 0; i < m_pixmaps; i++) {
        switch(i) {
        case 1: scale = height[0] / pix[i].height(); break;
        case 2: scale = width[0] / pix[i].width(); break;
        }

        br = doUniversalTransform(i, scale, layoutItem->objectView()->rotation(i));
        width[i]    = br.width();
        height[i]   = br.height();
    }

    pixitem[0].moveBy(childPadding + (!swapside ? width[LEFT] + space : 0), childPadding + height[2] + space);
    pixitem[1].moveBy(childPadding + ( swapside ? width[LEFT] + space : 0), childPadding + height[2] + space);
    pixitem[2].moveBy(childPadding + (!swapside ? width[LEFT] + space : 0), childPadding);

    childWidth  = width[0]  + space + width[1];
    childHeight = height[2] + space + height[1];

    rlinetop  ->setLine(childPadding + width[LEFT] + 5, childPadding + height[2] + space,               childPadding + width[LEFT] + space - 5, childPadding + height[2] + space            );
    rlinebot  ->setLine(childPadding + width[LEFT] + 5, childPadding + height[2] + space + height[0],   childPadding + width[LEFT] + space - 5, childPadding + height[2] + space + height[0]);
    rlineleft ->setLine(childPadding + (!swapside ? width[LEFT] + space : 0                          ), childPadding + height[2] + 5, childPadding + (!swapside ? width[LEFT] + space : 0),                           childPadding + height[2] + space - 5);
    rlineright->setLine(childPadding + ( swapside ? width[LEFT]         : width[0] + space + width[1]), childPadding + height[2] + 5, childPadding + ( swapside ? width[LEFT]         : width[0] + space + width[1]), childPadding + height[2] + space - 5);

    LayoutView::updateObject();
}

LayoutTopView::LayoutTopView(Database::LayoutItem *i) : LayoutView(i, 1)
{
    updateObject();
}

void LayoutTopView::updateObject()
{
    int i = 0;
    float filescale = layoutItem->objectView()->item.images.first()->scale() / 10; // pixels per mm
    float scale  = db->set.ppm / filescale * layoutItem->scale();

    QRectF br;
    br = doUniversalTransform(i, scale, layoutItem->objectView()->rotation(i));
    childWidth  = br.width();
    childHeight = br.height();

    LayoutView::updateObject();
}
