#include <cmath>

#include <QApplication>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QGraphicsItem>
#include <QTextDocument>
#include <QFont>
#include <QPaintDevice>
#include <QMessageBox>

#include "layout.h"
#include "factory.h"
#include "layoutviewfactory.h"

Layout::Layout()
{
    scene = new QGraphicsScene(this);
    setScene(scene);

    db->set.ppm         = logicalDpiX() / 25.4;
    db->set.dpi         = 300.0f / logicalDpiX();
    edgel               = 25    * db->set.ppm;
    edger               = 15    * db->set.ppm;
    edget               = 15    * db->set.ppm;
    edgeb               = 15    * db->set.ppm;
    paperw              = 210   * db->set.ppm - edgel - edger;
    paperh              = 297   * db->set.ppm - edget - edgeb;
    alignment           = 5     * db->set.ppm;

    setDragMode(QGraphicsView::RubberBandDrag);
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);

    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    scene->addItem((edgeRect = new QGraphicsRectItem(-edgel, -edget, paperw+edgel+edger, paperh+edget+edgeb)));
    scene->addItem((mainRect = new QGraphicsRectItem(0, 0, paperw, paperh)));
    edgeRect->setBrush(f->selectbrush);
    mainRect->setBrush(f->whitebrush);

    scene->addItem((text_objectList = new QGraphicsTextItem()));
    //QFont font = text_objectList->font();
    //font.setPointSize(3);
    //text_objectList->setFont(font);
    text_objectList->setTextWidth(paperw);
    text_objectList->setPos(0, paperh-12);

    m_ruler = new LayoutRuler();
    scene->addItem(m_ruler);
    m_ruler->hide();
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
    const bool edgeTransparent = true;
    const bool edgeHidden = true;
    const bool edgePresent = true;
    const bool mainRectPresent = true;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    scene->clearSelection();
    if(!edgeTransparent)
        edgeRect->setBrush(f->whitebrush);
    if(edgeHidden)
        edgeRect->hide();

    QPen oldPen = mainRect->pen();
    if(!mainRectPresent)
        mainRect->setPen(QPen(Qt::NoPen));

    if(edgePresent)
        scene->setSceneRect(-edgel, -edget, paperw+edgel+edger, paperh+edget+edgeb);
    else
        scene->setSceneRect(0, 0, paperw, paperh);

    QSizeF size = scene->sceneRect().size();
    QImage image(size.width()*db->set.dpi, size.height()*db->set.dpi, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    scene->render(&painter);
    image.save(filename);

    if(edgeHidden)
        edgeRect->show();
    if(edgeTransparent)
        edgeRect->setBrush(f->selectbrush);
    if(!mainRectPresent)
        mainRect->setPen(oldPen);

    QApplication::restoreOverrideCursor();
}
void Layout::loadPage(LayoutPage *page)
{
    float scale = 0;
    bool sameScale = true;
    clearLayout();

    currentLayout = page;
    if(!currentLayout)
        return;

    foreach(LayoutItem * item, currentLayout->list_items) {
        createObject(item);

        if(scale == 0)
            scale = item->scale();
        else if(scale != item->scale())
            sameScale = false;
    }

    if(scale != 0 && sameScale && db->set.showScaleOnLayoutPages) {
        QRectF r;
        m_ruler->show();
        m_ruler->updateObject(scale);
        r = m_ruler->childrenBoundingRect();
        m_ruler->setPos(paperw/2 /*- r.width()/2 * scale*/, paperh - r.height() - text_objectList->boundingRect().height());
    } else {
        m_ruler->hide();
    }
}

LayoutView* Layout::createObject(LayoutItem *item)
{
    LayoutViewFactory f;
    LayoutView * view;
    view = f.newLayoutView(item);
    view->setFlag(LayoutView::ItemIsSelectable);
    view->setFlag(LayoutView::ItemIsMovable);
    view->setFlag(LayoutView::ItemSendsScenePositionChanges);

    scene->addItem(view);

    int index = 0;
    foreach(LayoutView * object, objects) {
        if(object->pos().y() > view->pos().y())
            break;
        index++;
    }

    objects.append(view);
    //objects.insert(index, view);

    updateText();
    return view;
}

void Layout::updateText()
{
    if(!currentLayout) {
        text_objectList->setPlainText("");
        return;
    }

    int index = 1;
    QString text = db->set.layoutText.arg(currentLayout->text());
    foreach(LayoutView* object, objects) {
        object->rindex->setPlainText(QString::number(index));
        text += QString(index == 1 ? ": <b>" : ", <b>") + QString::number(index) + "</b> - " + object->layoutItem->name();
        index++;
    }
    text_objectList->setPlainText(text);
    text_objectList->setHtml(text);
    text_objectList->setPos(0, paperh - text_objectList->boundingRect().height());
}

void Layout::addNewObject(LayoutItem *item)
{
    float left = 0, right = paperw / 2;
    float lefty = 0;
    float leftytop = 0;
    float righty = 0;
    float x = 0, y = 0;

    foreach(LayoutView * object, objects) {
        if(x == left) {
            x = right;
            lefty = object->rrect->sceneBoundingRect().bottom();
            leftytop = object->rrect->sceneBoundingRect().top();
        }
        else {
            x = left;
            righty = object->rrect->sceneBoundingRect().bottom();
        }
    }
    y = lefty > righty ? lefty : righty;

    if(x == right)
        y = righty > leftytop ? righty : leftytop;

    if(item->objectView()->item.images.first()->scale() < 0.01) {
        QMessageBox::warning(this, "Bez měřítka", "Tento objekt nemá měřítko!");
        return;
    }

    x = ceil(x/alignment)*alignment;
    y = ceil(y/alignment)*alignment;

    LayoutView* object = createObject(item);
    object->setPos(QPointF(x,y));

    scene->clearSelection();
    object->setSelected(true);
}
void Layout::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = event->delta() > 0 ? 1.25 : 1 / 1.25;

    if(event->modifiers() & Qt::ControlModifier) {
        foreach (QGraphicsItem *item, scene->selectedItems()) {
            LayoutView * view = (LayoutView*) item;
            /* TODO: scale selected and move to center of selection */

            view->setObjectScale(view->objectScale() * scaleFactor);
        }
    }
    else {
        scale(scaleFactor, scaleFactor);
    }
}

void Layout::toggleBorder()
{
    QList<QGraphicsItem*> selected = scene->selectedItems();
    if(selected.isEmpty())
        return;
    bool border = !((LayoutView*)selected[0])->border();
    foreach (QGraphicsItem *item, selected) {
        LayoutView * view = (LayoutView*) item;
        view->setBorder(border);
        db->setModified();
    }
}
void Layout::toggleRuler()
{
    QList<QGraphicsItem*> selected = scene->selectedItems();
    if(selected.isEmpty())
        return;
    bool ruler = !((LayoutView*)selected[0])->ruler();
    foreach (QGraphicsItem *item, selected) {
        LayoutView * view = (LayoutView*) item;
        view->setRuler(ruler);
        db->setModified();
    }
}

void Layout::keyPressEvent(QKeyEvent * event)
{
    float dx = 0, dy = 0;
    switch(event->key()){
    case Qt::Key_Delete:
        foreach (QGraphicsItem *item, scene->selectedItems()) {
            LayoutView * view = (LayoutView*) item;
            LayoutItem * layoutItem = view->layoutItem;
            scene->removeItem(view);
            objects.removeAll(view);
            currentLayout->removeItem(layoutItem);
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
