#include <QBitmap>
#include <QApplication>

#include <stack>

#include "objectimage.h"
#include "../database.h"

namespace  DB {

#define skipcolor 0xFFFFFFFF

ObjectImage::ObjectImage(ImageFile * file, QVariant obj, float rotation) : m_item(NULL), m_file(file), m_object(obj), m_rotation(rotation)
{
    //m_file->link(this);
}
ObjectImage::ObjectImage(const QJsonObject &obj) : m_item(NULL)
{
    QString path = obj["filename"].toString();
    QPoint pos;
    QRect rect;

    pos.setX(obj["pointx"].toDouble());
    pos.setY(obj["pointy"].toDouble());
    m_object = pos;

    if(obj.contains("type") && obj["type"].toString() == "rect") {
        rect.setTopLeft(pos);
        rect.setWidth(obj["width"].toDouble());
        rect.setHeight(obj["height"].toDouble());
        m_object = rect;
    }
    if(obj.contains("rotation")) {
        m_rotation = obj["rotation"].toDouble();
    } else {
        m_rotation = 0;
    }

    m_file = db->imageFileByPath(path);
    //m_file->link(this);
}
QJsonObject ObjectImage::toJsonObject()
{
    QJsonObject obj;
    obj["filename"] = m_file->path();
    obj["rotation"] = (double) m_rotation;
    switch (m_object.type()) {
    case QVariant::Point:
        obj["type"] = "point";
        obj["pointx"] = m_object.toPoint().x();
        obj["pointy"] = m_object.toPoint().y();
        break;
    case QVariant::Rect:
        obj["type"] = "rect";
        obj["pointx"] = m_object.toRect().topLeft().x();
        obj["pointy"] = m_object.toRect().topLeft().y();
        obj["width"]  = m_object.toRect().width();
        obj["height"] = m_object.toRect().height();
        break;
    default:
        break;
    }
    return obj;
}

QPixmap ObjectImage::pixmapFromMask(QImage &mask, const QPixmap &pixmap)
{
    if(m_rotation == 0) {
        QBitmap bitmap = QBitmap::fromImage(mask);
        QRect r = QRegion(bitmap).boundingRect();
        QPixmap p = pixmap;
        p.setMask(bitmap);
        return p.copy(r.left(), r.top(),r.width(), r.height());
    } else {
        QTransform tr;
        tr.rotate(m_rotation);

        QBitmap bitmap = QBitmap::fromImage(mask);
        QPixmap p = pixmap;
        p.setMask(bitmap);

        QPixmap p2 =  p.transformed(tr);
        QBitmap bitmap2 = p2.mask();
        QRect r2 = QRegion(bitmap2).boundingRect();
        return p2.copy(r2.left(), r2.top(), r2.width(), r2.height());
    }
}

QImage ObjectImage::imageFileMask(const QPixmap & pixmap)
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

    std::stack<s_check> check;
    s_check ch;

    switch(m_object.type()) {
    case QVariant::Point:
        ch.x = m_object.toPoint().x(); ch.y = m_object.toPoint().y();
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
        for(int y = m_object.toRect().top(); y < m_object.toRect().bottom(); y++) {
            if(y < height && y >= 0) {
                for(int x = m_object.toRect().left(); x < m_object.toRect().right(); x++) {
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
QString ObjectImage::objectName()
{
    if(m_item)
        return m_item->text();
    else
        return "Nový";
}

}

void ObjectImage::event(ScaleChange & event)
{
    Q_UNUSED(event);
    ObjectImageChange e;
    emitEvent(e);
}
