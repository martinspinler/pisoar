#ifndef JAGUARVIEW_H
#define JAGUARVIEW_H

#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QWheelEvent>

class JaguarView : public QGraphicsView
{
    QPoint ptMouseDown;
    QGraphicsLineItem * m_scaleLineItem;

public:
    JaguarView();
    virtual ~JaguarView();

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent*event);
    void mouseReleaseEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);
};

#endif // JAGUARVIEW_H
