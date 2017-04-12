#ifndef IMAGE_H
#define IMAGE_H

#include <QtCore>
#include <QGraphicsView>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QBitmap>
#include <QVariant>

#include "database.h"
#include "layoutview.h"

class Image : public QGraphicsView
{
    Q_OBJECT

public:
    enum Mode {MODE_HAND, MODE_MASK, MODE_RECT, MODE_SCALE, MODE_COLOR};
private:

    uint m_skipcolor;
    Mode m_mode;

    QGraphicsScene m_scene;

    QPoint m_ptMouseDown;

    ImageFile *m_file;
    QPixmap m_pixmap;
    QGraphicsLineItem * m_scaleLineItem;
    QGraphicsPixmapItem * m_pixmapItem;

public:
    Image();
    virtual ~Image();

    void clear();
    void loadFile(ImageFile* image);
    void setMode(Mode mode);
    void updateObjects();

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void addObjectImage(ObjectImage *image);

    const QPixmap & pixmap() {return m_pixmap;}

signals:
    void calibrateDone(float length);
    void objectSelected(ObjectImage * image);
};

#endif // IMAGE_H
