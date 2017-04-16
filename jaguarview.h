#ifndef JAGUARVIEW_H
#define JAGUARVIEW_H

#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QWheelEvent>

class JaguarView : public QGraphicsView
{
    Q_OBJECT
public:
    enum Mode {MODE_HAND, MODE_RULER};

private:
    Mode m_mode;
    QPoint ptMouseDown;
    QGraphicsLineItem * m_scaleLineItem;

public:
    JaguarView();
    virtual ~JaguarView();

    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent*event);
    void mouseReleaseEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);

    void setMode(Mode mode);

signals:
    void rulerDone(float length);
};

#endif // JAGUARVIEW_H
