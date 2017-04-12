#ifndef OBJECTVIEW_H
#define OBJECTVIEW_H

#include "common.h"
#include "objectitem.h"

namespace DB {

class ObjectView : public QStandardItem {
    friend class ObjectItem;

    int m_type;
    QList<int> m_mapping;
    QList<int> m_rotation;
    int m_pagesCount;
    //QList<LayoutPage*> m_layoutPages;

public:
    ObjectItem & item;
public:
    ObjectView(ObjectItem & item, QString name);
    ObjectView(const QJsonObject & obj, ObjectItem & item);
    QJsonObject toJsonObject();

    void updateIcon();
    int  type()                                 {return m_type;}
    void setType(int type)                      {m_type = type;}
    void setMapping(int index, int moveto);
    void setRotation(int index, int angle)      {m_rotation[index] = angle;}
    int mapping(int index)                      {return m_mapping[index];}
    int rotation(int index)                     {return m_rotation[index];}
    QString name()                              {return item.text();}
    int pagesCount()                            {return m_pagesCount;}
};

}

#endif // OBJECTVIEW_H
