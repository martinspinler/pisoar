#include <cmath>

#include <QApplication>
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

#include "layout.h"
#include "factory.h"

Layout::Layout()
{
    scene = new QGraphicsScene(this);
    setScene(scene);

    db->set.ppm         = logicalDpiX() / 25.4;
    db->set.dpi         = 300.0f / logicalDpiX();
    edgew               = 10 * db->set.ppm;
    edgeh               = 10 * db->set.ppm;
    paperw              = 210 * db->set.ppm - edgew*2;
    paperh              = 297 * db->set.ppm - edgeh*2;
    alignment           = 5 * db->set.ppm;

    setDragMode(QGraphicsView::RubberBandDrag);
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);

    QGraphicsRectItem * r;

    scene->addItem((r = new QGraphicsRectItem(-edgew, -edgeh, paperw+edgew*2, paperh+edgeh*2)));
    r->setBrush(f->selectbrush);
    scene->addItem((r = new QGraphicsRectItem(0, 0, paperw, paperh)));
    r->setBrush(f->whitebrush);

    scene->addItem((text_objectList = new QGraphicsTextItem()));
    //QFont font = text_objectList->font();
    //font.setPointSize(3);
    //text_objectList->setFont(font);
    text_objectList->setTextWidth(paperw);
    text_objectList->setPos(0, paperh-12);
}

void Layout::clearLayout()
{
    for(int i = 0; i < objects.size(); i++) {
        scene->removeItem(objects[i]);
        delete objects[i];
    }
    setSceneRect(QRect());

    objects.clear();
    currentLayout = NULL;
    updateText();
}

void Layout::exportToImage(QString filename)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    scene->clearSelection();
    scene->setSceneRect(0, 0, paperw+1, paperh+1);

    QSizeF size = scene->sceneRect().size();
    QImage image(size.width()*db->set.dpi, size.height()*db->set.dpi, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    scene->render(&painter);
    image.save(filename);
    QApplication::restoreOverrideCursor();
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

LayoutView* Layout::createObject(Database::LayoutItem *item)
{
    LayoutView * view;
    switch(item->objectItem->images.size()) {
        case 0: return NULL;
        case 1: view = new LayoutTopView(item); break;
        case 2: (item->type == 1 ? view = new LayoutTopBottomView(item) : view = new LayoutTopSideView(item)); break;
        default: view = new LayoutTopSideView(item); break;
    }
    view->setFlag(LayoutView::ItemIsSelectable);
    view->setFlag(LayoutView::ItemIsMovable);
    view->setFlag(LayoutView::ItemSendsScenePositionChanges);

    scene->addItem(view);
    objects.append(view);
    updateText();
    return view;
}

void Layout::updateText()
{
    if(!currentLayout) {
        text_objectList->setPlainText("");
        return;
    }
    QString text = db->set.layoutText.arg(currentLayout->text()) + "\n";
    int index = 1;
    for(QList<LayoutView*>::const_iterator i = objects.constBegin(); i != objects.constEnd(); i++, index++) {
        (*i)->rindex->setPlainText(QString::number(index));
        text += QString(index == 1 ? "" : ", ") + QString::number(index) + " - " + (*i)->layoutItem->objectItem->text();
    }
    text_objectList->setPlainText(text);
    text_objectList->setPos(0, paperh - text_objectList->boundingRect().height());
}

void Layout::updateObject(Database::LayoutItem *layoutItem)
{
    for(QList<LayoutView*>::const_iterator i = objects.constBegin(); i != objects.constEnd(); i++) {
        if((*i)->layoutItem == layoutItem)
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

    if(item->objectItem->images.first()->file->getScale() < 0.01) {
        QMessageBox::warning(this, "Bez měřítka", "Tento objekt nemá měřítko!");
        return;
    }

    x = ceil(x/alignment)*alignment;
    y = ceil(y/alignment)*alignment;

    LayoutView* object = createObject(item);
    if(!object)
        return;

    object->setPos(QPointF(x,y));

    scene->clearSelection();
    object->setSelected(true);
}
void Layout::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y();

    if(event->modifiers() & Qt::ControlModifier) {
        foreach (QGraphicsItem *item, scene->selectedItems()) {
            LayoutView * view = (LayoutView*) item;
            /* TODO: scale selected and move to center of selection */

            view->layoutItem->scale *= delta > 0 ? 1.25 : 0.8;
            view->updateObject();
            QString text = QString("Scale: %1x").arg(QString::number(view->scale()*10));
            view->rscale->setPlainText(text);
        }
    }
    else {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        double scaleFactor = 1.25;
        if(event->delta() > 0)
            scale(scaleFactor, scaleFactor);
        else
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void Layout::setSelectedBorder(bool border)
{
    foreach (QGraphicsItem *item, scene->selectedItems()) {
        LayoutView * view = (LayoutView*) item;
        view->layoutItem->border = border;
        view->updateObject();
    }
}
void Layout::setSelectedRuler(bool ruler)
{
    foreach (QGraphicsItem *item, scene->selectedItems()) {
        LayoutView * view = (LayoutView*) item;
        view->layoutItem->ruler = ruler;
        view->updateObject();
    }
}

LayoutView * Layout::findViewByItem(Database::LayoutItem * layoutItem)
{
    for(QList<LayoutView*>::const_iterator i = objects.constBegin(); i != objects.constEnd(); i++) {
        if((*i)->layoutItem == layoutItem)
            return (*i);
    }
    return NULL;
}

const QList<Database::LayoutItem*> Layout::getSelection()
{
    QList<Database::LayoutItem*> list;
    foreach (QGraphicsItem *item, scene->selectedItems()) {
        LayoutView * view = (LayoutView*) item;
        list.append(view->layoutItem);
    }
    return list;
}

void Layout::keyPressEvent(QKeyEvent * event)
{
    float dx = 0, dy = 0;
    switch(event->key()){
    case Qt::Key_Delete:
        foreach (QGraphicsItem *item, scene->selectedItems()) {
            LayoutView * view = (LayoutView*) item;
            Database::LayoutItem * layoutItem = view->layoutItem;
            scene->removeItem(view);
            objects.removeAll(view);
            db->removeItem(currentLayout, layoutItem);
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

    foreach (QGraphicsItem *item, scene->selectedItems()) {
        item->moveBy(dx, dy);
    }
}
