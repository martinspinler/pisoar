#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include "objectview.h"

namespace DB {

class LayoutPage;

class LayoutItem : public EventListener<ObjectItemChange>, public EventBroadcaster<LayoutItemChange> {
    int m_pagesCount;
    ObjectView* m_objectView;
    QPointF m_pos;
    float m_scale;
    bool m_ruler;
    bool m_border;

    QList<LayoutPage*> m_layoutPages;

public:
    LayoutItem(ObjectView *view);
    LayoutItem(QJsonObject & obj);
    QJsonObject toJsonObject();

    QString name()              {return m_objectView->name();}
    bool border()               {return m_border;}
    bool ruler()                {return m_ruler;}
    float scale()               {return m_scale;}
    QPointF pos()               {return m_pos;}
    ObjectView* objectView()    {return m_objectView;}
    void setBorder(bool border) {m_border = border;}
    void setRuler(bool ruler)   {m_ruler = ruler;}
    void setScale(float scale)  {m_scale = scale;}
    void setPos(QPointF pos)    {m_pos = pos;}

    void link  (LayoutPage * page){Q_UNUSED(page);}//EventBroadcaster<LayoutItemChange>::link(page);   m_pagesCount++;}
    void unlink(LayoutPage * page){Q_UNUSED(page);}//{EventBroadcaster<LayoutItemChange>::unlink(page); m_pagesCount--;}
    void event(ObjectItemChange &event) {Q_UNUSED(event);LayoutItemChange e; emitEvent(e);}
};

}

#endif // LAYOUTITEM_H
