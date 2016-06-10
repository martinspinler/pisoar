#ifndef LAYOUT_H
#define LAYOUT_H

#include <QList>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPointF>
#include <QWidget>

#include "database.h"


class Layout : public QGraphicsView
{  
    Q_OBJECT

    class Ruler : public QGraphicsItemGroup {
        Layout * l;
    public:
        Ruler(Layout * layout);
        void updateObject(float scale);
    };

    class LayoutView: public QGraphicsItemGroup {    
    protected:
        Layout * l;
        float childWidth;
        float childHeight;
        float childPadding;
    public:
        LayoutView(Database::LayoutItem*item, Layout *l);
        virtual void updateObject();

        Database::LayoutItem * item;
        QGraphicsRectItem * rrect;
        Ruler             * rruler;
        QGraphicsRectItem * rselect;
        QGraphicsTextItem * rscale;
        QGraphicsTextItem * rindex;
    };

    class TopSideView: public LayoutView{
    public:
        QGraphicsPixmapItem * rtop;
        QGraphicsPixmapItem * rside;
        QGraphicsLineItem * rlinetop;
        QGraphicsLineItem * rlinebot;
    public:
        TopSideView(Database::LayoutItem*item, Layout *l);
        void updateObject();
    };
    class TopView: public LayoutView{
    public:
        QGraphicsPixmapItem * rtop;
    public:
        TopView(Database::LayoutItem*item, Layout *l);
        void updateObject();
    };

    Database * db;
    QBrush blackbrush;
    QBrush selectbrush;
    QBrush whitebrush;

    QPointF mouse_down_pos;
    QPointF mouse_move_pos;
    bool isMoving;

    QList<LayoutView*> objects;
    QList<LayoutView*> selected;

    QGraphicsScene *scene;
    QGraphicsTextItem *text_objectList;

    Database::LayoutPage * currentLayout;

    LayoutView * findViewByItem(Database::LayoutItem * item);

public:
    Layout(Database * database);

    void clearLayout();
    void loadPage(Database::LayoutPage * page);
    void exportToImage(QString filename);

    LayoutView* createObject(Database::LayoutItem *item);
    void addNewObject(Database::LayoutItem *item);
    void updateObject(Database::LayoutItem* group);

    void updateText();
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *e);

    void selectNone();
    void selectObject(QString name, bool sel = true);
    void selectObject(LayoutView *item, bool sel = true);
    void selectToggle(LayoutView *item);

    void setSelectedBorder(bool border);
    void setSelectedRuler(bool ruler);

    void keyPressEvent(QKeyEvent * event);

    const QList<Database::LayoutItem*> getSelection();

    float ppm;
    int edgew;
    int edgeh;
    int paperw;
    int paperh;
    float dpi;

    float space_multiplier;
    float default_scale;
    int offsetx;
    int offsety;
    int alignment;

signals:
    void selectionChanged();
};

#endif // LAYOUT_H
