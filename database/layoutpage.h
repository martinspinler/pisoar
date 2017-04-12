#ifndef LAYOUTPAGE_H
#define LAYOUTPAGE_H

#include "layoutitem.h"
#include "events.h"

namespace DB {

class LayoutPage : public QStandardItem, EventListener<LayoutItemChange> {
public:
    QList<LayoutItem*> list_items;

    LayoutPage() {}
    LayoutPage(QString &name) : QStandardItem(name) {}
    LayoutPage(QJsonObject & obj);

    virtual ~LayoutPage();

    QJsonObject toJsonObject();
    LayoutItem* createItem(ObjectView *objectItem);
    void        removeItem(LayoutItem * item);

    void event(LayoutItemChange &event) {Q_UNUSED(event);}
};

}
#endif // LAYOUTPAGE_H
