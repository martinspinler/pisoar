#ifndef OBJECTVIEW_H
#define OBJECTVIEW_H

#include "common.h"
#include "objectitem.h"

namespace DB {

class LayoutItem;

class ObjectView : public QStandardItem {
    friend class ObjectItem;

    int m_type;
    QList<int> m_mapping;
    QList<int> m_rotation;
    int m_scaleFrom;
    QList<LayoutItem*> m_layoutItems;

public:
    ObjectItem & item;
public:
    ObjectView(ObjectItem & item, QString name);
    ObjectView(const QJsonObject & obj, ObjectItem & item);
    QJsonObject toJsonObject();
    virtual ~ObjectView();

    int  type()                                 {return m_type;}
    void setType(int type)                      {m_type = type;}
    void setMapping(int index, int moveto);
    void setRotation(int index, int angle)      {m_rotation[index] = angle;}
    void setScaleFrom(int index)                {m_scaleFrom = index;}
    int mapping(int index)                      {return m_mapping[index];}
    int revMapping(int revIndex);
    int rotation(int index)                     {return m_rotation[index];}
    int scaleFrom()                             {return m_scaleFrom;}
    QString name()                              {return item.text();}
    bool isUsed()                               {return !m_layoutItems.isEmpty();}
    void clean();

    void link(LayoutItem * view);
    void unlink(LayoutItem * view);

};

}

#endif // OBJECTVIEW_H
