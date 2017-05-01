#include "layoutitem.h"
#include "../database.h"

namespace DB {

LayoutItem::LayoutItem(ObjectView* view) : m_objectView(view), m_ruler(false), m_border(false)
{
    m_page = NULL;
    m_scale = db->set.dpi / 2.54;
    m_pos = QPointF(0, 0);
}
LayoutItem::LayoutItem(QJsonObject &obj)
{
    QString name = obj["name"].toString();
    QList<QStandardItem*> list = db->view_model.findItems(name);
    Q_ASSERT(!list.empty());

    m_page = NULL;
    m_objectView = (ObjectView*) list.first();
    m_scale = obj["scale"].toDouble();
    m_ruler = obj["ruler"].toBool();
    m_border= obj["border"].toBool();
    m_pos.setX(obj["pointx"].toInt());
    m_pos.setY(obj["pointy"].toInt());

    // Old DB format compatibility.
    if(obj.contains("type")) {
        int type = 0;
        switch(m_objectView->item.images.size()) {
        case 0: type = -1; break;
        case 1: type = 1; break;
        case 2: type = 2 + (obj["type"].toInt() == 1 ? 1 : 0); break;
        case 3: type = 4; break;
        }
        m_objectView->setType(type);
    }
}
QJsonObject LayoutItem::toJsonObject()
{
    QJsonObject obj;
    obj["name"] = m_objectView->text();
    obj["scale"] = m_scale;
    obj["ruler"] = m_ruler;
    obj["border"] = m_border;
    obj["pointx"] = (int) m_pos.x();
    obj["pointy"] = (int) m_pos.y();
    return obj;
}
LayoutItem::~LayoutItem()
{
    if(m_page)
        unlink(m_page);
}

void LayoutItem::link(LayoutPage * page)
{
    Q_ASSERT(m_page == NULL);
    m_page = page;
    m_objectView->link(this);
}
void LayoutItem::unlink(LayoutPage * page)
{
    Q_ASSERT(m_page != NULL && m_page == page);

    if(m_page) {
        m_page = NULL;
        m_objectView->unlink(this);
    }
}

}
