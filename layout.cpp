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

#include <cmath>

Layout::LayoutView::LayoutView(Database::LayoutItem *i, Layout *layout)
{
    item = i;
    l = layout;
    childPadding = 30;

    addToGroup((rruler      = new Ruler(l)));
    addToGroup((rselect     = new QGraphicsRectItem()));
    addToGroup((rrect       = new QGraphicsRectItem()));
    addToGroup((rscale      = new QGraphicsTextItem()));
    addToGroup((rindex      = new QGraphicsTextItem()));

    rscale->hide();
    rselect->hide();
    rselect->setBrush(l->selectbrush);
}

void Layout::LayoutView::updateObject()
{
    float rulerHeight  = item->ruler ? rruler->sceneBoundingRect().height() + childPadding : 0;
    float rulerWidth   = l->ppm * 50 * item->scale;

    rselect ->setRect(childPadding, childPadding, childWidth, childHeight);
    rrect   ->setRect(0, 0, childWidth + 2*childPadding, childHeight + 2*childPadding + rulerHeight);
    rruler  ->setPos(childPadding + childWidth/2 - rulerWidth/2, childPadding*1.5 + childHeight);
    rindex  ->setPos(0, childHeight + childPadding);
    rscale  ->setPlainText("Scale: 1x");
    rrect   ->setVisible(item->border);
    rruler  ->setVisible(item->ruler);
    rruler  ->updateObject(item->scale);

    setPos(item->pos);
}

Layout::TopSideView::TopSideView(Database::LayoutItem *i, Layout *layout) : Layout::LayoutView(i, layout)
{
    QPixmap ptop (l->db->getDirItems().filePath(item->name + "_0.png"));
    QPixmap pside(l->db->getDirItems().filePath(item->name + "_1.png"));

    addToGroup((rlinetop    = new QGraphicsLineItem()));
    addToGroup((rlinebot    = new QGraphicsLineItem()));
    addToGroup((rtop        = new QGraphicsPixmapItem(ptop)));
    addToGroup((rside       = new QGraphicsPixmapItem(pside)));

    updateObject();
}

void Layout::TopSideView::updateObject()
{
    const QPixmap pixtop = rtop->pixmap();
    const QPixmap pixside = rside->pixmap();

    bool swapside   = false;

    float scale0    = 1;
    float scale1    = ((float)pixtop.height()) / pixside.height();
    float filescale = l->db->findFileByName(l->db->findObjectByName(item->name)->views.first()->filename)->scale / 10; // pixels per mm
    float imgscale  = l->ppm / filescale;

    rtop    ->setScale(item->scale * imgscale * scale0);
    rside   ->setScale(item->scale * imgscale * scale1);

    float width_space   = 10 * l->ppm * item->scale;
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

Layout::TopView::TopView(Database::LayoutItem *i, Layout *layout) : Layout::LayoutView(i, layout)
{
    LayoutView(i,layout);
    QPixmap ptop (l->db->getDirItems().filePath(item->name + "_0.png"));
    addToGroup((rtop        = new QGraphicsPixmapItem(ptop)));
    updateObject();
}

void Layout::TopView::updateObject()
{
    float filescale = l->db->findFileByName(l->db->findObjectByName(item->name)->views.first()->filename)->scale / 10; // pixels per mm
    float imgscale  = l->ppm / filescale;

    rtop    ->setScale(item->scale * imgscale);
    rtop    ->setPos(childPadding , childPadding);
    childWidth    = rtop->sceneBoundingRect().width();
    childHeight   = rtop->sceneBoundingRect().height();

    LayoutView::updateObject();
}

Layout::Ruler::Ruler(Layout * layout)
{
    l = layout;
    QGraphicsRectItem * r;
    QGraphicsTextItem * t;
    r = new QGraphicsRectItem();                                addToGroup(r);
    r = new QGraphicsRectItem();    r->setBrush(l->blackbrush); addToGroup(r);
    r = new QGraphicsRectItem();    r->setBrush(l->blackbrush); addToGroup(r);
    r = new QGraphicsRectItem();    r->setBrush(l->blackbrush); addToGroup(r);
    t = new QGraphicsTextItem("0");                             addToGroup(t);
    t = new QGraphicsTextItem("5 cm");                          addToGroup(t);
}
void Layout::Ruler::updateObject(float scale)
{
    float mmscale = scale *  l->ppm;
    ((QGraphicsRectItem *)childItems()[0])->setRect(mmscale*00, 0, mmscale*50, 5*l->ppm);
    ((QGraphicsRectItem *)childItems()[1])->setRect(mmscale*00, 0, mmscale*10, 5*l->ppm);
    ((QGraphicsRectItem *)childItems()[2])->setRect(mmscale*20, 0, mmscale*10, 5*l->ppm);
    ((QGraphicsRectItem *)childItems()[3])->setRect(mmscale*40, 0, mmscale*10, 5*l->ppm);
    ((QGraphicsTextItem *)childItems()[4])->setPos(mmscale*-5, 7*l->ppm);
    ((QGraphicsTextItem *)childItems()[5])->setPos(mmscale*45, 7*l->ppm);
}

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
    alignment           = 5 * ppm;

    isMoving = false;

    QGraphicsRectItem * r;

    scene->addItem((r = new QGraphicsRectItem(-edgew, -edgeh, paperw+edgew*2, paperh+edgeh*2)));
    r->setBrush(selectbrush);
    scene->addItem((r = new QGraphicsRectItem(0, 0, paperw, paperh)));
    r->setBrush(whitebrush);

    scene->addItem((text_objectList = new QGraphicsTextItem()));
    //QFont font = text_objectList->font();
    //font.setPointSize(3);
    //text_objectList->setFont(font);
    text_objectList->setTextWidth(paperw);
    text_objectList->setPos(0, paperh-12);
}

void Layout::clearLayout()
{
    for(int i = 0; i < objects.size(); i++)
        scene->removeItem(objects[i]);

    selected.clear();
    objects.clear();

    currentLayout = NULL;
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
    clearLayout();

    currentLayout = page;
    if(!currentLayout)
        return;

    for(int i = 0; i < currentLayout->list_items.size(); i++) {
        createObject(currentLayout->list_items[i]);
    }
}

Layout::LayoutView* Layout::createObject(Database::LayoutItem *item)
{
    LayoutView * view;
    Database::ObjectItem * i = db->findObjectByName(item->name);
    switch(i->views.size()) {
        case 0: return NULL;
        case 1: view = new TopView(item, this); break;
        case 2: view = new TopSideView(item, this); break;
        default: view = new TopSideView(item, this); break;
    }
    scene->addItem(view);
    objects.append(view);
    updateText();
    return view;
}

void Layout::updateText()
{
    QString text = QString("Tabulka č. " + currentLayout->name + "\n");
    int index = 1;
    for(QList<LayoutView*>::const_iterator i = objects.constBegin(); i != objects.constEnd(); i++, index++) {
        (*i)->rindex->setPlainText(QString::number(index));
        text += QString(index == 1 ? "" : ", ") + QString::number(index) + " - " + (*i)->item->name;
    }
    text_objectList->setPlainText(text);
    text_objectList->setPos(0, paperh - text_objectList->boundingRect().height());
}

void Layout::updateObject(Database::LayoutItem *item)
{
    for(QList<LayoutView*>::const_iterator i = objects.constBegin(); i != objects.constEnd(); i++) {
        if((*i)->item == item)
            (*i)->updateObject();
    }
}
void Layout::addNewObject(Database::LayoutItem *item)
{
    float left = 0, right = paperw / 2;
    float lefty = 0;
    float leftytop = 0;
    float righty = 0;
    float x = 0, y = 0;

    for(QList<LayoutView*>::const_iterator i = objects.constBegin(); i != objects.constEnd(); i++) {
        if(x == left) {
            x = right;
            lefty = (*i)->rrect->sceneBoundingRect().bottom();
            leftytop = (*i)->rrect->sceneBoundingRect().top();
        }
        else {
            x = left;
            righty = (*i)->rrect->sceneBoundingRect().bottom();
        }
    }
    y = lefty > righty ? lefty : righty;

    if(x == right)
        y = righty > leftytop ? righty : leftytop;

    if(db->findFileByName(db->findObjectByName(item->name)->views.first()->filename)->scale < 0.01) {
        QMessageBox::warning(this, "Bez měřítka", "Tento objekt nemá měřítko!");
        return;
    }

    x = ceil(x/alignment)*alignment;
    y = ceil(y/alignment)*alignment;

    LayoutView* object = createObject(item);
    if(!object)
        return;

    item->pos = QPointF(x,y);
    object->setPos(item->pos);

    selectNone();
    selected.append(object);
    object->rselect->show();
}
void Layout::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y();

    if(event->modifiers() & Qt::ControlModifier) {
        for(QList<LayoutView*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
            /* TODO: scale selected and move to center of selection */

            /*float scale = (*i)->scale();
            (*i)->setScale(delta > 0 ? scale * 1.25 : scale * 0.8);*/

            (*i)->item->scale *= delta > 0 ? 1.25 : 0.8;
            (*i)->updateObject();
            QString text = QString("Scale: %1x").arg(QString::number((*i)->scale()*10));
            (*i)->rscale->setPlainText(text);
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
    LayoutView * item = NULL;

    mouse_move_pos = mouse_down_pos = mapToScene(event->pos());

    items = scene->items(mouse_down_pos);
    for(int i = 0; i < items.size(); i++) {
        if(items[i]->parentItem() && objects.indexOf((LayoutView*)items[i]->parentItem()) != -1) {
            item = (LayoutView*)items[i]->parentItem();
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

    QPointF pos = mapToScene(event->pos()) - mouse_down_pos;
    for(QList<LayoutView*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        QPointF & ip = (*i)->item->pos;
        if(event->modifiers() & Qt::ControlModifier) {
            ip.setX(round((ip.x() + pos.x())/alignment)*alignment);
            ip.setY(round((ip.y() + pos.y())/alignment)*alignment);
        } else
            ip += pos;
        (*i)->setPos(ip.rx(), ip.ry());
    }
}

void Layout::mouseMoveEvent(QMouseEvent *event)
{
    if(!isMoving)
        return;
    QPointF pos = mapToScene(event->pos()) - mouse_down_pos;
    for(QList<LayoutView*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        QPointF ip = (*i)->item->pos;
        if(event->modifiers() & Qt::ControlModifier) {
            ip.setX(round((ip.x() + pos.x())/alignment)*alignment);
            ip.setY(round((ip.y() + pos.y())/alignment)*alignment);
        } else
            ip += pos;
        (*i)->setPos(ip.rx(), ip.ry());
    }
}
void Layout::setSelectedBorder(bool border)
{
    for(QList<LayoutView*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        (*i)->item->border = border;
        (*i)->updateObject();
    }
}
void Layout::setSelectedRuler(bool ruler)
{
    for(QList<LayoutView*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        (*i)->item->ruler = ruler;
        (*i)->updateObject();
    }
}

void Layout::selectNone()
{
    for(QList<LayoutView*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++)
        (*i)->rselect->hide();
    selected.clear();
    emit selectionChanged();
}
void Layout::selectObject(LayoutView* item, bool sel)
{
    if(sel && selected.indexOf(item) == -1) {
        item->rselect->show();
        selected.append(item);
    }
    else if(!sel) {
        item->rselect->hide();
        selected.removeAll(item);
    }
    emit selectionChanged();
}
void Layout::selectToggle(LayoutView* item)
{
    if(selected.indexOf(item) != -1) {
        item->rselect->hide();
        selected.removeAll(item);
    }
    else {
        item->rselect->show();
        selected.append(item);
    }
    emit selectionChanged();
}

Layout::LayoutView * Layout::findViewByItem(Database::LayoutItem * item)
{
    for(QList<LayoutView*>::const_iterator i = objects.constBegin(); i != objects.constEnd(); i++) {
        if((*i)->item == item)
            return (*i);
    }
    return NULL;
}

const QList<Database::LayoutItem*> Layout::getSelection()
{
    QList<Database::LayoutItem*> list;
    for(QList<LayoutView*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        list.append((*i)->item);
    }
    return list;
}

void Layout::keyPressEvent(QKeyEvent * event)
{
    float dx = 0, dy = 0;
    switch(event->key()){
    case Qt::Key_Delete:
        for(QList<LayoutView*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
            Database::LayoutItem * item = (*i)->item;
            scene->removeItem(*i);
            db->removeItem(currentLayout, item);
        }
        updateText();
        break;
    case Qt::Key_Up:        dy = -alignment;    break;
    case Qt::Key_Down:      dy = +alignment;    break;
    case Qt::Key_Left:      dx = -alignment;    break;
    case Qt::Key_Right:     dx = +alignment;    break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }

    for(QList<LayoutView*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        (*i)->item->pos += QPoint(dx,dy);
        (*i)->moveBy(dx, dy);
    }
}
