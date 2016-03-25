#include "layout.h"

#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QTextDocument>
#include <QFont>
#include <QPaintDevice>
#include <QMessageBox>


#define min(x,y) (x<y?x:y)



#define INDEX_SELECT 0
#define INDEX_RECT   1
#define INDEX_RULER  2
#define INDEX_SCALE  3
#define INDEX_COUNT  4
#define INDEX_PIX0   5
#define INDEX_PIX1   6
#define INDEX_LINEUP 7
#define INDEX_LINEDOWN 8

Layout::Layout(Database * database)
    :db(database), blackbrush(Qt::black), selectbrush(Qt::lightGray), whitebrush(Qt::white)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scene = new QGraphicsScene(this);
    setScene(scene);

    ppm                 = logicalDpiX() / 25.4;
    edgew               = 10 * ppm;
    edgeh               = 10 * ppm;
    paperw              = 210 * ppm - edgew*2;
    paperh              = 297 * ppm - edgeh*2;
    dpi                 = 300.0f / logicalDpiX();
    space_multiplier    = 1.2;

    //default_scale       = 25.4f / logicalDpiX();
    offsetx             = 30;
    offsety             = 30;

    QGraphicsRectItem * r;
    r = new QGraphicsRectItem(-edgew, -edgeh, paperw+edgew*2, paperh+edgeh*2);
    r->setBrush(selectbrush);
    scene->addItem(r);
    r = new QGraphicsRectItem(0,0, paperw, paperh);
    r->setBrush(whitebrush);
    scene->addItem(r);

    /*float x = float(size().width())  / (paperw + 2*edgew);
    float y = float(size().height()) / (paperh + 2*edgeh);
    scale(min(x,y), min(x,y));*/
    //scale(3, 3);

    isMoving = false;
}

void Layout::exportToImage(QString filename)
{
    scene->clearSelection();
    scene->setSceneRect(0, 0, paperw+1, paperh+1);

    QSizeF size = scene->sceneRect().size();
    QImage image(size.width()*dpi, size.height()*dpi, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    scene->render(&painter);
    image.save(filename);
}
void Layout::loadPage(Database::LayoutPage *page)
{
    currentLayout = page;
    selected.clear();
    objects.clear();
    scene->clear();

    QGraphicsRectItem * r;
    r = new QGraphicsRectItem(-edgew, -edgeh, paperw+edgew*2, paperh+edgeh*2);
    r->setBrush(selectbrush);
    scene->addItem(r);
    r = new QGraphicsRectItem(0,0, paperw, paperh);
    r->setBrush(whitebrush);
    scene->addItem(r);

    text_objectList = new QGraphicsTextItem();
    scene->addItem(text_objectList);
    QFont font = text_objectList->font();
    //font.setPointSize(3);
    text_objectList->setFont(font);
    text_objectList->setTextWidth(paperw);
    text_objectList->setPos(0, paperh-12);

    if(!currentLayout)
        return;

    for(int i = 0; i < currentLayout->list_items.size(); i++) {
        createObject(currentLayout->list_items[i]);
    }
}
QGraphicsItemGroup * Layout::addRuler()
{
    QGraphicsItemGroup * ruler = new QGraphicsItemGroup();
    QGraphicsRectItem * r;
    r = new QGraphicsRectItem();
    ruler->addToGroup(r);

    r = new QGraphicsRectItem();
    r->setBrush(blackbrush);
    ruler->addToGroup(r);
    r = new QGraphicsRectItem();
    r->setBrush(blackbrush);
    ruler->addToGroup(r);
    r = new QGraphicsRectItem();
    r->setBrush(blackbrush);
    ruler->addToGroup(r);
    QGraphicsTextItem * t;
    t = new QGraphicsTextItem("0");
    ruler->addToGroup(t);
    t = new QGraphicsTextItem("5 cm");
    ruler->addToGroup(t);

    return ruler;
}
void Layout::updateRuler(QGraphicsItemGroup* ruler, float scale)
{
    float mmscale = scale *  ppm;

    ((QGraphicsRectItem *)ruler->childItems()[0])
            ->setRect(mmscale*00, 0, mmscale*50, 5*ppm);

    ((QGraphicsRectItem *)ruler->childItems()[1])
            ->setRect(mmscale*00, 0, mmscale*10, 5*ppm);
    ((QGraphicsRectItem *)ruler->childItems()[2])
            ->setRect(mmscale*20, 0, mmscale*10, 5*ppm);
    ((QGraphicsRectItem *)ruler->childItems()[3])
            ->setRect(mmscale*40, 0, mmscale*10, 5*ppm);

    ((QGraphicsRectItem *)ruler->childItems()[4])
            ->setPos(mmscale*-5, 7*ppm);

    ((QGraphicsRectItem *)ruler->childItems()[5])
            ->setPos(mmscale*45, 7*ppm);
}

QGraphicsItemGroup* Layout::createObject(Database::LayoutItem *item)
{
    QPixmap pix0(db->getDirItems().filePath(item->name + "_0.png"));
    QPixmap pix1(db->getDirItems().filePath(item->name + "_1.png"));

    QGraphicsRectItem * r;
    QGraphicsItem * i;
    QGraphicsTextItem * t;

    QGraphicsItemGroup * group = new QGraphicsItemGroup();
    QVariant v = qVariantFromValue((void*) item);
    group->setData(0, v);
    objects.push_back(group);
    scene->addItem(group);

    float lscale = ((float) pix0.height()) / pix1.height();

    // INDEX_SELECT
    r = new QGraphicsRectItem();
    r->setBrush(selectbrush);
    r->hide();
    group->addToGroup(r);

    // INDEX_RECT
    r = new QGraphicsRectItem();
    group->addToGroup(r);

    // INDEX_RULER
    QGraphicsItemGroup * g = addRuler();
    group->addToGroup(g);

    // INDEX_SCALE
    t = new QGraphicsTextItem();
    t->hide();
    group->addToGroup(t);

    // INDEX_COUNT
    t = new QGraphicsTextItem(QString::number(objects.count()));
    group->addToGroup(t);

    // INDEX_PIX0
    i = scene->addPixmap(pix0);
    group->addToGroup(i);

    // INDEX_PIX1
    i = scene->addPixmap(pix1);
    i->setScale(lscale);
    group->addToGroup(i);

    // INDEX_LINEUP && INDEX_LINEDOWN
    group->addToGroup(new QGraphicsLineItem());
    group->addToGroup(new QGraphicsLineItem());

    QString text = text_objectList->document()->toPlainText() + QString(objects.count() == 1 ? QString("Tabulka č. " + currentLayout->name+"\n") :",  ") + QString::number(objects.count()) + " - " + item->name;
    text_objectList->setPlainText(text);
    text_objectList->setPos(0, paperh - text_objectList->boundingRect().height());

    recomputeObject(group);
    return group;
}

void Layout::recomputeObject(Database::LayoutItem *item)
{
    for(QList<QGraphicsItem*>::const_iterator i = objects.constBegin(); i != objects.constEnd(); i++) {
        Database::LayoutItem * it = (Database::LayoutItem *) (*i)->data(0).value<void *>();
        if(it == item)
            recomputeObject((QGraphicsItemGroup*)*i);
    }
}

void Layout::recomputeObject(QGraphicsItemGroup *group)
{
    Database::LayoutItem * item = (Database::LayoutItem *) group->data(0).value<void *>();

    QGraphicsPixmapItem* ipix0 = ((QGraphicsPixmapItem*)group->childItems()[INDEX_PIX0]);
    QGraphicsPixmapItem* ipix1 = ((QGraphicsPixmapItem*)group->childItems()[INDEX_PIX1]);

    QPixmap pix0 = ipix0->pixmap();
    QPixmap pix1 = ipix1->pixmap();

    bool swapside   = false;

    float scale0    = 1;
    float scale1    = ((float)pix0.height()) / pix1.height();
    float filescale = db->findFileByName(db->findObjectByName(item->name)->views.first()->filename)->scale / 10; // pixels per mm
    if(filescale < 0.1)
    {
        QMessageBox::warning(this, "Bez měřítka", "Tento objekt nemá měřítko!");
        return;
    }
    float imgscale  = ppm / filescale;

    ipix0->setScale(item->scale * imgscale * scale0);
    ipix1->setScale(item->scale * imgscale * scale1);

    updateRuler(((QGraphicsItemGroup*)group->childItems()[INDEX_RULER]), item->scale);

    float width_space   = 10 * ppm * item->scale;
    float width_pix0    = ipix0->sceneBoundingRect().width();
    float width_pix1    = ipix1->sceneBoundingRect().width();
    float height        = ipix0->sceneBoundingRect().height();

    float width_ruler   = ppm * 50 * item->scale;
    //float width_ruler   = ((QGraphicsItemGroup*)group->childItems()[INDEX_RULER])->sceneBoundingRect().width()/2;
    float height_ruler  = item->ruler ? ((QGraphicsItemGroup*)group->childItems()[INDEX_RULER])->sceneBoundingRect().height() + offsety : 0;

    // INDEX_SELECT
    ((QGraphicsRectItem*)group->childItems()[INDEX_SELECT])
            ->setRect(offsetx, offsety, width_pix0 + width_space + width_pix1, height);

    // INDEX_RECT
    ((QGraphicsRectItem*)group->childItems()[INDEX_RECT])
            ->setRect(0, 0, width_pix0 + width_space + width_pix1 + 2*offsetx, height + 2*offsety + height_ruler);

    // INDEX_RULER
    ((QGraphicsItemGroup*)group->childItems()[INDEX_RULER])
            ->setPos(offsetx + (width_pix0 + width_space +width_pix1)/2 - width_ruler/2, offsety*1.5 + height);


    // INDEX_SCALE
    ((QGraphicsTextItem*)group->childItems()[INDEX_SCALE])
            ->setPlainText("Scale: 1x");

    // INDEX_COUNT
    ((QGraphicsTextItem*)group->childItems()[INDEX_COUNT])
            ->setPos(0, height + offsety);

    // INDEX_PIX1
    ((QGraphicsPixmapItem*)group->childItems()[INDEX_PIX0])
            ->setPos(offsetx + (swapside ? 0 : width_pix1 + width_space), offsety);

    // INDEX_PIX2
    ((QGraphicsPixmapItem*)group->childItems()[INDEX_PIX1])
            ->setPos(offsetx + (swapside ? width_pix0 + width_space : 0), offsety);


    // INDEX_LINEUP && INDEX_LINEDOWN
    ((QGraphicsLineItem*)group->childItems()[INDEX_LINEUP])
            ->setLine(offsetx + (swapside ? width_pix0 : width_pix1) + 5, offsety,          offsetx + (swapside ? width_pix0 : width_pix1) + width_space - 5, offsety);
    ((QGraphicsLineItem*)group->childItems()[INDEX_LINEDOWN])
            ->setLine(offsetx + (swapside ? width_pix0 : width_pix1) + 5, offsety + height, offsetx + (swapside ? width_pix0 : width_pix1) + width_space - 5, offsety + height);

    group->setPos(item->pos);

    ((QGraphicsRectItem*)group->childItems()[INDEX_RECT])->setVisible(item->border);
    ((QGraphicsRectItem*)group->childItems()[INDEX_RULER])->setVisible(item->ruler);
}
void Layout::addNewObject(Database::LayoutItem *item)
{
    float left = 0, right = paperw / 2;
    float lefty = 0;
    float righty = 0;
    float x = 0, y = 0;

    for(QList<QGraphicsItem*>::const_iterator i = objects.constBegin(); i != objects.constEnd(); i++) {
        if(x == left) {
            x = right;
            lefty = (*i)->childItems()[INDEX_RECT]->sceneBoundingRect().bottom();
        }
        else {
            x = left;
            righty = (*i)->childItems()[INDEX_RECT]->sceneBoundingRect().bottom();
        }
    }
    y = lefty > righty ? lefty : righty;

    if(x == right)
        y = righty;

    QGraphicsItemGroup* object = createObject(item);
    item->pos = QPointF(x,y);
    object->moveBy(x, y);

    selectNone();
    selected.append(object);
    object->childItems()[INDEX_SELECT]->show();
}
void Layout::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y();

    if(event->modifiers() & Qt::ControlModifier) {
        for(QList<QGraphicsItem*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
            /* TODO: scale selected and move to center of selection */

            /*float scale = (*i)->scale();
            (*i)->setScale(delta > 0 ? scale * 1.25 : scale * 0.8);*/

            Database::LayoutItem * item = (Database::LayoutItem *) (*i)->data(0).value<void *>();
            item->scale *= delta > 0 ? 1.25 : 0.8;
            recomputeObject((QGraphicsItemGroup*)*i);

            QString text = QString("Scale: %1x").arg(QString::number((*i)->scale()*10));
            ((QGraphicsTextItem*)(*i)->childItems()[INDEX_SCALE])->setPlainText(text);
        }
    }
    else {
        if(delta == 0)
            return;
        else if(delta > 0)
            delta = 1.25;
        else if(delta < 0)
            delta = 0.8;

        setTransformationAnchor(QGraphicsView::NoAnchor);
        setResizeAnchor(QGraphicsView::NoAnchor);
        QPointF oldPos = mapToScene(event->pos());
        scale(delta, delta);
        QPointF newPos = mapToScene(event->pos());
        QPointF d = newPos - oldPos;
        translate(d.x(), d.y());
    }
}

void Layout::mousePressEvent(QMouseEvent *event)
{
    QList<QGraphicsItem*> items;
    QGraphicsItem * item = NULL;

    mouse_move_pos = mouse_down_pos = mapToScene(event->pos());

    items = scene->items(mouse_down_pos);
    for(int i = 0; i < items.size(); i++) {
        if(items[i]->parentItem() && objects.indexOf(items[i]->parentItem()) != -1) {
            item = items[i]->parentItem();
            break;
        }
    }
    if(!item) {
        selectNone();
        return;
    }

    if(event->modifiers() & Qt::ControlModifier) {
        if(item)
            selectToggle(item);
    }
    else {
        /* TODO: There is a bug when moving a more objects */
        selectNone();
        if(item)
            selectObject(item);
    }

    isMoving = true;
}
void Layout::mouseReleaseEvent(QMouseEvent *event)
{
    if(!isMoving)
        return;
    isMoving = false;

    QPointF posm = mapToScene(event->pos());
    posm -= mouse_move_pos;
    QPointF pos = mapToScene(event->pos());
    pos -= mouse_down_pos;
    for(QList<QGraphicsItem*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        (*i)->moveBy(posm.x(), posm.y());
        QVariant v = (*i)->data(0);
        Database::LayoutItem * li = (Database::LayoutItem *) v.value<void *>();
        li->pos += pos;
    }
}

void Layout::mouseMoveEvent(QMouseEvent *event)
{
    if(!isMoving)
        return;
    if(!isMoving)
        return;
    QPointF pos = mapToScene(event->pos());
    pos -= mouse_move_pos;
    for(QList<QGraphicsItem*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++)
        (*i)->moveBy(pos.x(), pos.y());
    mouse_move_pos += pos;

}
void Layout::selectNone()
{
    for(QList<QGraphicsItem*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++)
        (*i)->childItems()[INDEX_SELECT]->hide();
    selected.clear();
    emit selectionChanged();
}
void Layout::selectObject(QString name, bool sel)
{

}
void Layout::selectObject(QPointF pos, bool sel)
{

}
void Layout::selectObject(QGraphicsItem* item, bool sel)
{
    if(sel && selected.indexOf(item) == -1) {
        item->childItems()[INDEX_SELECT]->show();
        selected.append(item);
    }
    else if(!sel) {
        item->childItems()[INDEX_SELECT]->hide();
        selected.removeAll(item);
    }
    emit selectionChanged();
}
void Layout::selectToggle(QGraphicsItem* item)
{
    if(selected.indexOf(item) != -1) {
        item->childItems()[INDEX_SELECT]->hide();
        selected.removeAll(item);
    }
    else {
        item->childItems()[INDEX_SELECT]->show();
        selected.append(item);
    }
    emit selectionChanged();
}


QGraphicsItemGroup * Layout::findGroupByItem(Database::LayoutItem * item)
{
    for(QList<QGraphicsItem*>::const_iterator i = objects.constBegin(); i != objects.constEnd(); i++) {
        QVariant v = (*i)->data(0);
        Database::LayoutItem * li = (Database::LayoutItem *) v.value<void *>();
        if(li == item)
            return (QGraphicsItemGroup*) (*i);
    }
    return NULL;
}

const QList<Database::LayoutItem*> Layout::getSelection()
{
    QList<Database::LayoutItem*> list;
    for(QList<QGraphicsItem*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        Database::LayoutItem * item = (Database::LayoutItem *) (*i)->data(0).value<void *>();
        list.append(item);
    }

    return list;
}

void Layout::keyPressEvent(QKeyEvent * event)
{
    if(event->key() == Qt::Key_Delete) {
        for(QList<QGraphicsItem*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
            Database::LayoutItem * item = (Database::LayoutItem *) (*i)->data(0).value<void *>();
            currentLayout->list_items.removeAll(item);
        }
        loadPage(currentLayout);
    }
}
