#include "objectview.h"
#include "../database.h"
#include "../factory.h"

namespace DB
{

ObjectView::ObjectView(const QJsonObject & obj, ObjectItem & item) : item(item)
{
    QJsonArray mapping;
    QJsonArray rotation;

    // Old DB format compatibility
    if(!obj.contains("name"))
        setText(item.text());
    else
        setText(obj["name"].toString());

    // Old DB format compatibility
    if(!obj.contains("type"))
        m_type = item.images.size() > 2 ? 2 : item.images.size();
    else
        m_type = obj["type"].toInt();

    if(!obj.contains("scalefrom"))
        m_scaleFrom = 0;
    else
        m_scaleFrom = obj["scalefrom"].toInt();

    // Old DB format compatibility
    if(!obj.contains("mapping")) {
        for(int i = 0; i < item.images.size(); i++) {
            m_mapping.append(i);
            m_rotation.append(0);
        }
    } else {
        mapping = obj["mapping"].toArray();
        for(QJsonArray::const_iterator it = mapping.constBegin(); it != mapping.constEnd(); it++) {
            m_mapping.append((*it).toInt());
        }
        rotation = obj["rotation"].toArray();
        for(QJsonArray::const_iterator it = rotation.constBegin(); it != rotation.constEnd(); it++) {
            m_rotation.append((*it).toInt() % 360);
        }
    }

    setIcon(f->icon_image);
}
QJsonObject ObjectView::toJsonObject()
{
    QJsonObject obj;
    QJsonArray mapping;
    QJsonArray rotation;

    foreach(int imgMapping, m_mapping)
        mapping.append(imgMapping);
    foreach(int imgRotation, m_rotation)
        rotation.append(imgRotation);

    obj["name"] = text();
    obj["type"] = m_type;
    obj["mapping"] = mapping;
    obj["rotation"] = rotation;
    obj["scalefrom"] = m_scaleFrom;
    return obj;
}
ObjectView::ObjectView(ObjectItem & item, QString name) : item(item)
{
    setText(name);
    m_type = -1;

    for(int i = 0; i < item.images.size(); i++) {
        m_mapping.append(i);
        m_rotation.append(0);
    }
    m_scaleFrom = 0;

    setIcon(f->icon_image);
}
ObjectView::~ObjectView()
{
    m_layoutItems.clear();
}

void ObjectView::setMapping(int index, int moveto)
{
    if(moveto < 0 || moveto >= m_mapping.size())
        return;

    m_mapping[m_mapping.indexOf(moveto)] = m_mapping[index];
    m_mapping[index] = moveto;
}
int ObjectView::revMapping(int revIndex)
{
    for(int i = 0; i < item.images.size(); i++) {
        if(m_mapping[i] == revIndex)
            return i;
    }
    return -1;
}
void ObjectView::link(LayoutItem * view)
{
    Q_ASSERT(!m_layoutItems.contains(view));
    m_layoutItems.append(view);
    setIcon(f->icon_done);
}
void ObjectView::unlink(LayoutItem * view)
{
    m_layoutItems.removeAll(view);
    if(m_layoutItems.isEmpty()) {
        setIcon(f->icon_image);
    }
}
void ObjectView::clean()
{
    foreach(LayoutItem* item, m_layoutItems) {
        item->page()->removeItem(item);
    }
    setIcon(f->icon_image);
}

}
