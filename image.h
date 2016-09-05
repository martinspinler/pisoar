#ifndef IMAGE_H
#define IMAGE_H

#include <QtCore>
#include <QGraphicsView>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QBitmap>
#include <QVariant>

class Image : public QGraphicsView
{
    Q_OBJECT

public:
    enum Mode {MODE_HAND, MODE_MASK, MODE_RECT, MODE_SCALE, MODE_COLOR};
private:
    struct s_check{int x; int y;};

    QGraphicsScene scene;
    QPixmap pixmap;

    uint skipcolor;

    QGraphicsLineItem * scale_line;
    QPoint ptMouseDown;

    Mode mode;
    QPixmap pixmap_object;

public:

    Image();

    void setMode(Mode m);
    void clear();
    void loadImage(QString filename);
    void addObject(QVariant obj, QString name);

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    QImage objectMask(QVariant obj);
    QPixmap pixmapFromMask(QImage &mask);

signals:
    void calibrateDone(float length);
    void objectSelected(QVariant obj);
};

#endif // IMAGE_H
