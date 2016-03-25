#include "image.h"
#include <QSizePolicy>
#include <QPointF>
#include <QBitmap>
#include <QTransform>
#include <stack>

#include <stdint.h>
#include <cmath>

#define min(x,y) (x<y?x:y)

Image::Image()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setScene(&scene);
    state = 0;
    skipcolor = 0xFFFFFFFF;
}
QPoint Image::getSelectedPoint()
{
    return pos;
}
QPixmap& Image::getSelectedObject()
{
    return pixmap_object;
}

void Image::calibrate()
{
    state = 1;
}
void Image::setBorderColor()
{
    state = 2;
}

void Image::wheelEvent(QWheelEvent *event)
{
    if(scene.items().count() == 0)
        return;

    float delta = event->angleDelta().y();

    if(event->modifiers() & Qt::ControlModifier) {
        rotate(delta > 0 ? 90 : -90);
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

void Image::mousePressEvent(QMouseEvent *event)
{
    if(scene.items().count() == 0)
        return;

    QPointF pos = mapToScene(event->pos());
    if(state == 1){
        this->pos = pos.toPoint();
    }
    else if(state == 2) {
        state = 0;
        skipcolor = pixmap.toImage().pixel(pos.x(), pos.y());
    }
    else {
        QBitmap b;
        QImage mask = findEdgeMask(pos.x(), pos.y());
        b.convertFromImage(mask);
        QRegion reg(b);
        QRect r = reg.boundingRect();
        QPixmap pixmap = this->pixmap;
        pixmap.setMask(b);
        pixmap_object = pixmap.copy(r.left(), r.top(),r.width(), r.height());
        emit objectSelected();
    }
}
void Image::mouseReleaseEvent(QMouseEvent *event)
{
    if(scene.items().count() == 0)
        return;

    QPointF posf = mapToScene(event->pos());
    QPoint oldpos = pos;
    pos = posf.toPoint();

    if(state == 1) {
        state = 0;
        posf = posf - oldpos;
        float calibrated_length = sqrt(pow(posf.x(), 2) + pow(posf.y(), 2));
        emit calibrateDone(calibrated_length);
    }

}
void Image::loadImage(QString filename)
{
    float x, y;
    resetTransform();
    scene.clear();
    if(filename.length()) {
        pixmap.load(filename);
        scene.addPixmap(pixmap);
        x = float(size().width())  / pixmap.size().width();
        y = float(size().height()) / pixmap.size().height();
        scale(min(x,y), min(x,y));
        //translate(x/2, y/2);
    }
}
QImage Image::findEdgeMask(int x, int y)
{
    int width = pixmap.size().width();
    int height = pixmap.size().height();
    std::stack<Image::s_check> check;

    QImage image = pixmap.toImage();
    QImage mask = QImage(width, height, QImage::Format_ARGB32);
    mask.fill(0xffffffff);

    uint *src = (uint*)image.bits();
    uint *dst = (uint*)mask.bits();
    uint bpl = image.bytesPerLine()/4;

    s_check ch; ch.x = x; ch.y = y;
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
        /*ch.x--;       ;if(src[bpl*ch.y+ch.x] != skipcolor) check.push(ch);
        ch.x+=2;      ;if(src[bpl*ch.y+ch.x] != skipcolor) check.push(ch);
               ch.y-=2;if(src[bpl*ch.y+ch.x] != skipcolor) check.push(ch);
               ch.x-=2;if(src[bpl*ch.y+ch.x] != skipcolor) check.push(ch);*/
    }
    return mask;
}
