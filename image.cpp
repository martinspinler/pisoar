#include <stack>
#include <stdint.h>
#include <cmath>

#include <QApplication>
#include <QSizePolicy>
#include <QPointF>
#include <QBitmap>
#include <QTransform>
#include <QGraphicsTextItem>

#include "image.h"
#include "factory.h"

Image::Image()
{
    setScene(&m_scene);

    m_mode = MODE_HAND;
    setDragMode(QGraphicsView::ScrollHandDrag);

    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    m_scaleLineItem = new QGraphicsLineItem();
    QPen p = m_scaleLineItem->pen();
    p.setWidth(4);
    m_scaleLineItem->setPen(p);
}
Image::~Image()
{
    delete m_scaleLineItem;
}

void Image::clear()
{
    m_file = NULL;
    m_pixmapItem = NULL;
    m_scene.clear();
    resetTransform();
}
void Image::loadFile(ImageFile* file)
{
    clear();

    if(m_pixmap.load(file->path())) {
        m_file = file;
        m_pixmapItem = m_scene.addPixmap(m_pixmap);
        QRectF br   = m_pixmapItem->boundingRect();
        setSceneRect(br);

        resetTransform();
        fitInView(m_pixmapItem, Qt::KeepAspectRatio);
        centerOn(m_pixmapItem);

        updateObjects();
    }
}
void Image::updateObjects()
{
    QList<QGraphicsItem*> items = m_scene.items();
    foreach(QGraphicsItem *item, items) {
        if(item != m_pixmapItem) {
            m_scene.removeItem(item);
        }
    }

    QList<ObjectImage*> objects = db->objectImagesByFile(m_file);
    foreach(ObjectImage* image, objects) {
        addObjectImage(image);
    }
}
void Image::addObjectImage(ObjectImage* image)
{
    QVariant obj = image->object();
    QPoint pos = obj.type() == QVariant::Point? obj.toPoint() : obj.toRect().center();
    QPen pen(Qt::red);
    pen.setWidth(5);
    QBrush brush(Qt::black);
    QPoint offset(40, 40);
    QGraphicsEllipseItem * e = m_scene.addEllipse(QRect(pos - offset, pos + offset), pen, brush);

    QGraphicsTextItem* t = m_scene.addText(image->objectName());
    QFont f = t->font();
    f.setPixelSize(72);
    t->setFont(f);
    t->setPos(pos + QPoint(-t->boundingRect().width()/2, 50));

    e->setData(1, qVariantFromValue((void *) image));
}

void Image::setMode(Mode mode)
{
    switch(mode){
    case MODE_HAND:
        setDragMode(QGraphicsView::ScrollHandDrag);
        break;
    case MODE_SCALE:
        setDragMode(QGraphicsView::NoDrag);
        viewport()->setCursor(Qt::CrossCursor);
        break;
    case MODE_COLOR:
        setDragMode(QGraphicsView::NoDrag);
        viewport()->setCursor(Qt::ArrowCursor);
        break;
    case MODE_RECT:
        setDragMode(QGraphicsView::RubberBandDrag);
        viewport()->setCursor(Qt::CrossCursor);
        break;
    case MODE_MASK:
        setDragMode(QGraphicsView::NoDrag);
        viewport()->setCursor(Qt::ArrowCursor);
        break;
    default:
        break;
    }
    m_mode = mode;
}

void Image::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() & Qt::ControlModifier) {
        rotate(event->delta() > 0 ? 90 : -90);
    }
    else {
        double scaleFactor = 1.25;
        if(event->delta() > 0)
            scale(scaleFactor, scaleFactor);
        else
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void Image::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = ((QPointF) mapToScene(event->pos())).toPoint();

    switch(m_mode){
    case MODE_RECT:
        m_ptMouseDown = pos;
        break;
    case MODE_COLOR:
        m_skipcolor = m_pixmap.toImage().pixel(pos.x(), pos.y());
        break;
    case MODE_SCALE:
        m_ptMouseDown = pos;
        m_scaleLineItem->setLine(m_ptMouseDown.x(), m_ptMouseDown.y(), pos.x(), pos.y());
        m_scene.addItem(m_scaleLineItem);
        break;
    default:
        break;
    }
    return QGraphicsView::mousePressEvent(event);
}
void Image::mouseMoveEvent(QMouseEvent *event) {
    QPoint pos = ((QPointF) mapToScene(event->pos())).toPoint();

    switch(m_mode){
    case MODE_SCALE:
        m_scaleLineItem->setLine(m_ptMouseDown.x(), m_ptMouseDown.y(), pos.x(), pos.y());
        break;
    default:
        break;
    }

    return QGraphicsView::mouseMoveEvent(event);
}
void Image::mouseReleaseEvent(QMouseEvent *event)
{
    float calibrated_length;
    QPoint pos = ((QPointF) mapToScene(event->pos())).toPoint();
    QRect rect;
    QVector2D v;
    ObjectImage * image = NULL;

    switch (m_mode){
    case MODE_MASK:
        if (m_file) {
            QList<QGraphicsItem*> list = scene()->items(pos);
            foreach (QGraphicsItem *item, list) {
                if (item->data(1).isValid()) {
                    image = (ObjectImage *) item->data(1).value<void *>();
                }
            }
            if (image == NULL) {
                image = new ObjectImage(m_file, QVariant::fromValue(pos), 0);
            }
            emit objectSelected(image);
        }
        break;
    case MODE_RECT:
        if (m_file) {
            rect.setRect(   std::min(m_ptMouseDown.x(), pos.x()),
                            std::min(m_ptMouseDown.y(), pos.y()),
                            std::abs((m_ptMouseDown - pos).x()),
                            std::abs((m_ptMouseDown - pos).y()) );
            image = new ObjectImage(m_file, QVariant::fromValue(rect), 0);
            emit objectSelected(image);
        }
        break;
    case MODE_SCALE:
        m_scene.removeItem(m_scaleLineItem);
        calibrated_length = sqrt(pow((pos - m_ptMouseDown).x(), 2) + pow((pos - m_ptMouseDown).y(), 2));
        if(m_file)
            emit calibrateDone(calibrated_length);
        break;
    default:
        break;
    }
    return QGraphicsView::mouseReleaseEvent(event);
}
