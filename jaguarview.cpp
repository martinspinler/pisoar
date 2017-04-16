#include "jaguarview.h"

JaguarView::JaguarView()
{
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

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
    QPoint pos;
    switch(m_mode) {
    case MODE_RULER:
        pos = ((QPointF) mapToScene(event->pos())).toPoint();
        ptMouseDown = pos;
        scene()->addItem(m_scaleLineItem);
        m_scaleLineItem->setLine(ptMouseDown.x(), ptMouseDown.y(), pos.x(), pos.y());
        break;
    default:
        break;
    }
    return QGraphicsView::mousePressEvent(event);
}

void JaguarView::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint pos;
    QVector2D v;
    switch(m_mode) {
    case MODE_RULER:
        pos = ((QPointF) mapToScene(event->pos())).toPoint();
        m_scaleLineItem->setLine(ptMouseDown.x(), ptMouseDown.y(), pos.x(), pos.y());
        v = QVector2D(pos - ptMouseDown);
        scene()->removeItem(m_scaleLineItem);
        emit rulerDone(v.length());
        break;
    default:
        break;
    }
    return QGraphicsView::mouseReleaseEvent(event);
}

void JaguarView::mouseMoveEvent(QMouseEvent *event)
{
    switch(m_mode) {
    case MODE_RULER:
        if(m_scaleLineItem->isVisible()) {
            QPoint pos = ((QPointF) mapToScene(event->pos())).toPoint();
           m_scaleLineItem->setLine(ptMouseDown.x(), ptMouseDown.y(), pos.x(), pos.y());
        }
        break;
    default:
        break;
    }
    return QGraphicsView::mouseMoveEvent(event);
}
void JaguarView::setMode(Mode mode)
{
    switch(mode){
    case MODE_HAND:
        setDragMode(QGraphicsView::ScrollHandDrag);
        break;
    case MODE_RULER:
        setDragMode(QGraphicsView::NoDrag);
        viewport()->setCursor(Qt::CrossCursor);
        break;
    default:
        break;
    }
    m_mode = mode;
}

