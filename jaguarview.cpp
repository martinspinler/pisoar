#include "jaguarview.h"

JaguarView::JaguarView()
{
    m_scaleLineItem = new QGraphicsLineItem();
}

JaguarView::~JaguarView()
{
    delete m_scaleLineItem;
}

void JaguarView::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = event->delta() > 0 ? 1.25 : 1/1.25;
    scale(scaleFactor, scaleFactor);
}

void JaguarView::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = ((QPointF) mapToScene(event->pos())).toPoint();
    ptMouseDown = pos;
    scene()->addItem(m_scaleLineItem);
    m_scaleLineItem->setLine(ptMouseDown.x(), ptMouseDown.y(), pos.x(), pos.y());
    return QGraphicsView::mousePressEvent(event);
}

void JaguarView::mouseReleaseEvent(QMouseEvent *event)
{
    scene()->removeItem(m_scaleLineItem);
    return QGraphicsView::mouseReleaseEvent(event);
}

void JaguarView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_scaleLineItem->isVisible()) {
        QPoint pos = ((QPointF) mapToScene(event->pos())).toPoint();
        m_scaleLineItem->setLine(ptMouseDown.x(), ptMouseDown.y(), pos.x(), pos.y());
    }
    return QGraphicsView::mouseMoveEvent(event);
}
