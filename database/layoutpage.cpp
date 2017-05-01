#include "layoutpage.h"
#include "../database.h"

namespace DB {

LayoutPage::LayoutPage(QJsonObject & obj)
{
    setText(obj["name"].toString());
    QJsonArray items = obj["items"].toArray();
    for(QJsonArray::const_iterator it = items.constBegin(); it != items.constEnd(); it++) {
        QJsonObject o = (*it).toObject();
        LayoutItem *item = new LayoutItem(o);
        item->link(this);
        list_items.append(item);
    }
}
QJsonObject LayoutPage::toJsonObject()
{
    QJsonObject obj;
    QJsonArray items;

    for(QList<LayoutItem*>::const_iterator i = list_items.constBegin(); i != list_items.constEnd(); i++)
        items.append((*i)->toJsonObject());

    obj["name"] = text();
    obj["items"] = items;
    return obj;
}

LayoutPage::~LayoutPage()
{
    foreach(LayoutItem * item, list_items) {
        delete item;
    }
}

LayoutItem* LayoutPage::createItem(ObjectView* objectView)
{
    LayoutItem* item = new LayoutItem(objectView);
    item->setScale(db->set.layoutItemDefaultScale);
    item->link(this);
    list_items.append(item);
    db->setModified();
    return item;
}
void LayoutPage::removeItem(LayoutItem * item)
{
    list_items.removeAll(item);
    item->unlink(this);
    db->setModified();
}

}
