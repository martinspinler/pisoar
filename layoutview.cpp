#include <cmath>
#include <QApplication>

#include "factory.h"
#include "layoutview.h"

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

LayoutView::LayoutView(Database::LayoutItem *li) : layoutItem(li)
{
    childPadding = db->set.ppm * 5;

    addToGroup((rruler      = new LayoutRuler()));
    addToGroup((rselect     = new QGraphicsRectItem()));
    addToGroup((rrect       = new QGraphicsRectItem()));
    addToGroup((rscale      = new QGraphicsTextItem()));
    addToGroup((rindex      = new QGraphicsTextItem()));

    rscale->hide();
    rselect->hide();
    rselect->setBrush(f->selectbrush);
}

void LayoutView::updateObject()
{
    float rulerHeight  = layoutItem->ruler ? rruler->sceneBoundingRect().height() + childPadding : 0;
    float rulerWidth   = db->set.ppm * 50 * layoutItem->scale;

    rselect ->setRect(childPadding, childPadding, childWidth, childHeight);
    rrect   ->setRect(0, 0, childWidth + 2*childPadding, childHeight + 2*childPadding + rulerHeight);
    rruler  ->setPos(childPadding + childWidth/2 - rulerWidth/2, childPadding*1.5 + childHeight);
    rindex  ->setPos(0, childHeight);
    rscale  ->setPlainText("Scale: 1x");
    rrect   ->setVisible(layoutItem->border);
    rruler  ->setVisible(layoutItem->ruler);
    rruler  ->updateObject(layoutItem->scale);

    setPos(layoutItem->pos);
    prepareGeometryChange();
    br = childrenBoundingRect();
}

void LayoutView::setObjectScale(float scale)
{
    db->setModified();
    layoutItem->scale = scale;
    updateObject();
    /*QString text = QString("Scale: %1x").arg(QString::number(view->scale()*10));
    rscale->setPlainText(text);*/
}
float LayoutView::objectScale()
{
    return layoutItem->scale;
}

QVariant LayoutView::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged){
        rselect->setVisible(value.toBool());
    }
    else if (change == ItemPositionHasChanged) {
        layoutItem->pos = pos();
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

LayoutTopSideView::LayoutTopSideView(Database::LayoutItem *i) : LayoutView(i)
{
    QPixmap ptop (db->getDirItems().filePath(layoutItem->objectItem->text() + "_0.png"));
    QPixmap pside(db->getDirItems().filePath(layoutItem->objectItem->text() + "_1.png"));

    addToGroup((rlinetop    = new QGraphicsLineItem()));
    addToGroup((rlinebot    = new QGraphicsLineItem()));
    addToGroup((rtop        = new QGraphicsPixmapItem(ptop)));
    addToGroup((rside       = new QGraphicsPixmapItem(pside)));

    rlinetop->setZValue(1);
    rlinebot->setZValue(1);
    rtop->setZValue(1);
    rside->setZValue(1);

    updateObject();
}

void LayoutTopSideView::updateObject()
{
    const QPixmap pixtop = rtop->pixmap();
    const QPixmap pixside = rside->pixmap();

    bool swapside   = false;

    float scale0    = 1;
    float scale1    = ((float)pixtop.height()) / pixside.height();
    float filescale = layoutItem->objectItem->images.first()->file->getScale() / 10; // pixels per mm
    float imgscale  = db->set.ppm / filescale;

    rtop    ->setScale(layoutItem->scale * imgscale * scale0);
    rside   ->setScale(layoutItem->scale * imgscale * scale1);

    float width_space   = 10 * db->set.ppm * layoutItem->scale;
    float width_pix0    = rtop->sceneBoundingRect().width();
    float width_pix1    = rside->sceneBoundingRect().width();
    childHeight         = rtop->sceneBoundingRect().height();
    childWidth          = width_pix0 + width_space + width_pix1;

    rtop    ->setPos(childPadding + (swapside ? 0 : width_pix1 + width_space), childPadding);
    rside   ->setPos(childPadding + (swapside ? width_pix0 + width_space : 0), childPadding);

    rlinetop->setLine(childPadding + (swapside ? width_pix0 : width_pix1) + 5, childPadding,               childPadding + (swapside ? width_pix0 : width_pix1) + width_space - 5, childPadding);
    rlinebot->setLine(childPadding + (swapside ? width_pix0 : width_pix1) + 5, childPadding + childHeight, childPadding + (swapside ? width_pix0 : width_pix1) + width_space - 5, childPadding + childHeight);

    LayoutView::updateObject();
}

LayoutTopBottomView::LayoutTopBottomView(Database::LayoutItem *i) : LayoutView(i)
{
    QPixmap ptop (db->getDirItems().filePath(layoutItem->objectItem->text() + "_0.png"));
    QPixmap pside(db->getDirItems().filePath(layoutItem->objectItem->text() + "_1.png"));

    addToGroup((rlineleft   = new QGraphicsLineItem()));
    addToGroup((rlineright  = new QGraphicsLineItem()));
    addToGroup((rtop        = new QGraphicsPixmapItem(ptop)));
    addToGroup((rside       = new QGraphicsPixmapItem(pside)));

    rlineleft->setZValue(1);
    rlineright->setZValue(1);
    rtop->setZValue(1);
    rside->setZValue(1);

    updateObject();
}

void LayoutTopBottomView::updateObject()
{
    const QPixmap pixtop = rtop->pixmap();
    const QPixmap pixside = rside->pixmap();

    bool swapside   = false;

    float scale0    = 1;
    float scale1    = ((float)pixtop.width()) / pixside.width();
    float filescale = layoutItem->objectItem->images.first()->file->getScale() / 10; // pixels per mm
    float imgscale  = db->set.ppm / filescale;

    rtop    ->setScale(layoutItem->scale * imgscale * scale0);
    rside   ->setScale(layoutItem->scale * imgscale * scale1);

    float height_space  = 10 * db->set.ppm * layoutItem->scale;
    float height_pix0   = rtop->sceneBoundingRect().height();
    float height_pix1   = rside->sceneBoundingRect().height();
    childHeight         = height_pix0 + height_space + height_pix1;
    childWidth          = rtop->sceneBoundingRect().width();

    rtop    ->setPos(childPadding, childPadding + (swapside ? height_pix1 + height_space :0));
    rside   ->setPos(childPadding, childPadding + (swapside ? 0 : height_pix0 + height_space));
    rlineleft ->setLine(childPadding,              childPadding + (swapside ? height_pix1 : height_pix0) + 5, childPadding,              childPadding + (swapside ? height_pix1 : height_pix0) + height_space - 5);
    rlineright->setLine(childPadding + childWidth, childPadding + (swapside ? height_pix1 : height_pix0) + 5, childPadding + childWidth, childPadding + (swapside ? height_pix1 : height_pix0) + height_space - 5);

    LayoutView::updateObject();
}

LayoutTopView::LayoutTopView(Database::LayoutItem *i) : LayoutView(i)
{
    QPixmap ptop (db->getDirItems().filePath(layoutItem->objectItem->text() + "_0.png"));
    addToGroup((rtop        = new QGraphicsPixmapItem(ptop)));
    rtop->setZValue(1);

    updateObject();
}

void LayoutTopView::updateObject()
{
    float filescale = layoutItem->objectItem->images.first()->file->getScale() / 10; // pixels per mm
    float imgscale  = db->set.ppm / filescale;

    rtop    ->setScale(layoutItem->scale * imgscale);
    rtop    ->setPos(childPadding , childPadding);
    childWidth    = rtop->sceneBoundingRect().width();
    childHeight   = rtop->sceneBoundingRect().height();

    LayoutView::updateObject();
}
