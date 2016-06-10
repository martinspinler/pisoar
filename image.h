#ifndef IMAGE_H
#define IMAGE_H

#include <QtCore>
#include <QGraphicsView>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QBitmap>

class Image : public QGraphicsView
{
    Q_OBJECT

private:
    struct s_check{int x; int y;};

    QGraphicsScene scene;
    QPixmap pixmap;

    uint skipcolor;

    QPoint pos;

    int state;
    QPixmap pixmap_object;


public:

    Image();
    void clear();
    void loadImage(QString filename);
    void addPoint(QPoint pos, QString name);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QImage findEdgeMask(int x, int y);
    QPoint getSelectedPoint();
    QPixmap&getSelectedObject();

    void calibrate();
    void setBorderColor();

signals:
    void calibrateDone(float length);
    void objectSelected();
};

#endif // IMAGE_H
