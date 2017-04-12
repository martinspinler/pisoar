#include "objectview.h"
#include "../database.h"
#include "../factory.h"

namespace DB
{

ObjectView::ObjectView(const QJsonObject & obj, ObjectItem & item) : item(item)
{
    m_pagesCount = 0;
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
    m_pagesCount = 0;

    setIcon(f->icon_image);
}
void ObjectView::updateIcon()
{
    for(int i = 0; i < db->layout_model.rowCount(); i++) {
        LayoutPage * layout = (LayoutPage*) db->layout_model.item(i);
        for(int j = 0; j < layout->list_items.size(); j++) {
            if(layout->list_items[j]->objectView() == this) {
                setIcon(f->icon_done);
                return;
            }
        }
    }
    setIcon(f->icon_image);
}
void ObjectView::setMapping(int index, int moveto)
{
    if(moveto < 0 || moveto >= m_mapping.size())
        return;

    m_mapping[m_mapping.indexOf(moveto)] = m_mapping[index];
    m_mapping[index] = moveto;
}

}
