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
    setScene(&scene);
    skipcolor = 0xFFFFFFFF;

    mode = MODE_HAND;
    setDragMode(QGraphicsView::ScrollHandDrag);

    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    scale_line = new QGraphicsLineItem();
    QPen p = scale_line->pen();
    p.setWidth(4);
    scale_line->setPen(p);

}

void Image::setMode(Mode m)
{
    switch(m){
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
    mode = m;
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

    switch(mode){
    case MODE_RECT:
        ptMouseDown = pos;
        break;
    case MODE_MASK:
        if(scene.items().count())
            emit objectSelected(QVariant::fromValue(pos));
        break;
    case MODE_COLOR:
        skipcolor = pixmap.toImage().pixel(pos.x(), pos.y());
        break;
    case MODE_SCALE:
        ptMouseDown = pos;
        scale_line->setLine(ptMouseDown.x(), ptMouseDown.y(), pos.x(), pos.y());
        scene.addItem(scale_line);
        break;
    default:
        break;
    }
    return QGraphicsView::mousePressEvent(event);
}
void Image::mouseMoveEvent(QMouseEvent *event) {
    QPoint pos = ((QPointF) mapToScene(event->pos())).toPoint();

    switch(mode){
    case MODE_SCALE:
        scale_line->setLine(ptMouseDown.x(), ptMouseDown.y(), pos.x(), pos.y());
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

    switch(mode){
    case MODE_RECT:
        rect.setRect(   std::min(ptMouseDown.x(), pos.x()),
                        std::min(ptMouseDown.y(), pos.y()),
                        std::abs((ptMouseDown- pos).x()),
                        std::abs((ptMouseDown- pos).y()) );
        emit objectSelected(QVariant::fromValue(rect));
        break;
    case MODE_SCALE:
        scene.removeItem(scale_line);
        calibrated_length = sqrt(pow((pos-ptMouseDown).x(), 2) + pow((pos-ptMouseDown).y(), 2));
        if(scene.items().count())
            emit calibrateDone(calibrated_length);
        break;
    default:
        break;
    }
    return QGraphicsView::mouseReleaseEvent(event);
}

QPixmap Image::pixmapFromMask(QImage &mask)
{
    QBitmap b;
    b.convertFromImage(mask);
    QRegion reg(b);
    QRect r = reg.boundingRect();
    QPixmap pixmap = this->pixmap;
    pixmap.setMask(b);
    return pixmap.copy(r.left(), r.top(),r.width(), r.height());
}

void Image::clear()
{
    scene.clear();
    resetTransform();
}
void Image::loadImage(QString filename)
{
    clear();
    if(pixmap.load(filename)) {
        QGraphicsPixmapItem * pixmapItem = scene.addPixmap(pixmap);
        QRectF br   = pixmapItem->boundingRect();
        setSceneRect(br);

        resetTransform();
        fitInView(pixmapItem, Qt::KeepAspectRatio);
        centerOn(pixmapItem);
    }
}
void Image::addObject(QVariant obj, QString name)
{
    QPoint pos = obj.type() == QVariant::Point? obj.toPoint() : obj.toRect().center();
    QPen pen(Qt::red);
    pen.setWidth(5);
    QBrush brush(Qt::black);
    QPoint offset(40,40);
    scene.addEllipse(QRect(pos - offset, pos + offset), pen, brush);

    QGraphicsTextItem* t = scene.addText(name);
    QFont f = t->font();
    f.setPixelSize(72);
    t->setFont(f);
    t->setPos(pos + QPoint(-t->boundingRect().width()/2, 50));
}

QImage Image::objectMask(QVariant obj)
{
    int width   = pixmap.size().width();
    int height  = pixmap.size().height();

    QImage image = pixmap.toImage();
    QImage mask = QImage(width, height, QImage::Format_ARGB32);
    mask.fill(0xffffffff);

    uint32_t *src = (uint32_t*)image.bits();
    uint32_t *dst = (uint32_t*)mask.bits();
    uint bpl = image.bytesPerLine()/4;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    std::stack<Image::s_check> check;
    s_check ch;

    switch(obj.type()) {
    case QVariant::Point:
        ch.x = obj.toPoint().x(); ch.y = obj.toPoint().y();
        check.push(ch);

        while(!check.empty()) {
            ch = check.top();
            check.pop();

            src[bpl*ch.y+ch.x] = skipcolor;
            dst[bpl*ch.y+ch.x] = 0xff000000;

            if(ch.x == 1 || ch.y == 1 || ch.x == width-2 || ch.y == height-2)
                continue;
            ch.x--;       ;if(src[bpl*ch.y+ch.x] != skipcolor) check.push(ch);
            ch.x+=2;      ;if(src[bpl*ch.y+ch.x] != skipcolor) check.push(ch);
            ch.x--;ch.y-- ;if(src[bpl*ch.y+ch.x] != skipcolor) check.push(ch);
                   ch.y+=2;if(src[bpl*ch.y+ch.x] != skipcolor) check.push(ch);
        }
        break;
    case QVariant::Rect:
        for(int y = obj.toRect().top(); y < obj.toRect().bottom(); y++) {
            if(y < height && y >= 0) {
                for(int x = obj.toRect().left(); x < obj.toRect().right(); x++) {
                    if(x < width && x >= 0) {
                        dst[bpl*y+x] = 0xff000000;
                    }
                }
            }
        }
        break;
    default:
        break;
    }
    QApplication::restoreOverrideCursor();
    return mask;
}
