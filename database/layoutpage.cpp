#include "layoutpage.h"
#include "../database.h"

namespace DB {

LayoutPage::LayoutPage(QJsonObject & obj)
{
    setText(obj["name"].toString());
    QJsonArray items = obj["items"].toArray();
    for(QJsonArray::const_iterator it = items.constBegin(); it != items.constEnd(); it++) {
        QJsonObject o = (*it).toObject();
        LayoutItem *li = new LayoutItem(o);
        list_items.append(li);
        li->link(this);
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
    while(!list_items.isEmpty()) {
        LayoutItem * item = list_items.takeFirst();
        item->unlink(this);
        delete item;
    }
}

LayoutItem* LayoutPage::createItem(ObjectView* objectView)
{
    LayoutItem* item = new LayoutItem(objectView);
    list_items.append(item);
    item->link(this);
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
