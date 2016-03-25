#include "database.h"

#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Database::Database()
{
    icon_up     = QIcon(":/icons/res/up.png");
    icon_folder = QIcon(":/icons/res/folder.png");
    icon_wip    = QIcon(":/icons/res/wip.png");
    icon_done   = QIcon(":/icons/res/done.png");
    icon_image  = QIcon(":/icons/res/image.png");
    icon_0      = QIcon(":/icons/res/0.png");
    icon_1      = QIcon(":/icons/res/1.png");
    icon_2      = QIcon(":/icons/res/2.png");
}
void Database::clear()
{
    object_model.clear();
    layout_model.clear();

    while(!list_files.isEmpty())
        delete list_files.takeFirst();

    while(!list_objects.isEmpty()) {
        ObjectItem * i = list_objects.takeFirst();
        while(!i->views.isEmpty())
            delete i->views.takeFirst();
        delete i;
    }

    while(!list_layouts.isEmpty()) {
        LayoutPage *l = list_layouts.takeFirst();
        while(!l->list_items.isEmpty())
            delete l->list_items.takeFirst();
        delete l;
    }
}
bool Database::create(QDir dir)
{
    QFile file(dir.filePath("pisoardb.json"));
    if(!file.open(QIODevice::WriteOnly))
		return false;

	dir.mkdir("items");
	dir.mkdir("layouts");
	return open(dir);
}
bool Database::open(QDir dir)
{
    clear();

    dir.setCurrent(dir.path());

    dir_base = QDir();
    dir_items = dir_base;
    dir_items.cd("items");

    dir_layouts = dir;
    dir_layouts.cd("layouts");

    /* Load json file */
    QFile file("pisoardb.json");
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray saveData = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(saveData));
    QJsonObject json = doc.object();

    /* Parse files */
    QJsonObject files = json["files"].toObject();
    for(QJsonObject::const_iterator i = files.constBegin(); i != files.constEnd(); i++ ) {
        ImageFile * f = new ImageFile;
        f->name = i.key();
        f->scale = i.value().toObject()["scale"].toDouble();
        f->flags = i.value().toObject()["flags"].toInt();
        list_files.append(f);
    }

    /* Parse objects */
    QJsonArray objs = json["objects"].toArray();
    for(QJsonArray::const_iterator i = objs.constBegin(); i != objs.constEnd(); i++ ) {

        /* Parse object */
        ObjectItem * o = new ObjectItem;
        o->name = (*i).toObject()["name"].toString();

        /* Parse items in object */
        QJsonArray items = (*i).toObject()["items"].toArray();
        for(QJsonArray::const_iterator it = items.constBegin(); it != items.constEnd(); it++ ) {
            ObjectView * v = new ObjectView;
            v->filename = (*it).toObject()["filename"].toString();
            v->pos.setX((*it).toObject()["pointx"].toDouble());
            v->pos.setY((*it).toObject()["pointy"].toDouble());
            o->views.append(v);
        }
        list_objects.append(o);
        QIcon ic = icon_image;
        switch(o->views.size()) {
            case 0: ic = icon_0; break;
            case 1: ic = icon_1; break;
            case 2: ic = icon_2; break;
        }

        object_model.appendRow(new QStandardItem(ic, o->name));
    }

    /* Parse layouts */
    QJsonObject layouts = json["layouts"].toObject();
    for(QJsonObject::const_iterator i = layouts.constBegin(); i != layouts.constEnd(); i++ ) {
        LayoutPage *l = new LayoutPage;
        l->name = i.key();

        QJsonArray items = i.value().toArray();
        for(QJsonArray::const_iterator it = items.constBegin(); it != items.constEnd(); it++ ) {
            QJsonObject o = (*it).toObject();
            LayoutItem *li = new LayoutItem;
            li->name = o["name"].toString();
            li->scale = o["scale"].toDouble();
            li->ruler = o["ruler"].toBool();
            li->border = o["border"].toBool();
            li->pos.setX(o["pointx"].toDouble());
            li->pos.setY(o["pointy"].toDouble());
            l->list_items.append(li);
            object_model.findItems(li->name).first()->setIcon(icon_done);
        }
        list_layouts.append(l);
        layout_model.appendRow(new QStandardItem(l->name));
    }
    return true;
}
void Database::save()
{
    /* Store files */
    QJsonObject files;
    for(QList<ImageFile*>::const_iterator i = list_files.constBegin(); i != list_files.constEnd(); i++) {
        QJsonObject f;
        f["flags"] = (*i)->flags;
        f["scale"] = (*i)->scale;
        files[(*i)->name] = f;
    }

    /* Parse objects */
    QJsonArray objects;
    for(QList<ObjectItem*>::const_iterator i = list_objects.constBegin(); i != list_objects.constEnd(); i++) {
        QJsonObject f;
        QJsonArray items;
        for(QList<ObjectView*>::const_iterator it = (*i)->views.constBegin(); it != (*i)->views.constEnd(); it++) {
            QJsonObject item;
            item["filename"] = (*it)->filename;
            item["pointx"] = (*it)->pos.x();
            item["pointy"] = (*it)->pos.y();
            items.append(item);
        }
        f["name"] = (*i)->name;
        f["items"] = items;
        objects.append(f);
    }

    /* Parse layouts */
    QJsonObject layouts;
    for(QList<LayoutPage*>::const_iterator i = list_layouts.constBegin(); i != list_layouts.constEnd(); i++) {
        QJsonObject f;
        QJsonArray items;
        for(QList<LayoutItem*>::const_iterator it = (*i)->list_items.constBegin(); it != (*i)->list_items.constEnd(); it++) {
            QJsonObject item;
            item["name"] = (*it)->name;
            //item["type"] = (*it)->type;
            item["scale"] = (*it)->scale;
            item["ruler"] = (*it)->ruler;
            item["border"] = (*it)->border;
            item["pointx"] = (*it)->pos.x();
            item["pointy"] = (*it)->pos.y();
            items.append(item);
        }
        layouts[(*i)->name] = items;
    }

    QJsonObject json;
    json["files"] = files;
    json["objects"] = objects;
    json["layouts"] = layouts;

    QJsonDocument doc(json);
    QFile file("pisoardb.json");
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
}
Database::ObjectItem* Database::createObject(QString name)
{
    QStandardItem * it = new QStandardItem(icon_0, name);
    ObjectItem * i = new ObjectItem;
    i->name = name;
    i->it   = it;
    object_model.appendRow(it);
    list_objects.append(i);
    return i;
}

Database::ObjectView* Database::createView(Database::ObjectItem* item, QString filename, QPoint pt)
{
    ObjectView * v = new ObjectView;
    v->filename = filename;
    v->pos = pt;
    item->views.append(v);

    QIcon i = icon_image;
    switch(item->views.size()) {
        case 0: i = icon_0; break;
        case 1: i = icon_1; break;
        case 2: i = icon_2; break;
    }
    object_model.findItems(item->name).first()->setIcon(i);
    return v;
}
Database::ImageFile * Database::findFileByName(QString name)
{
    for(int i = 0; i < list_files.size(); i++)
        if(list_files[i]->name == name)
            return list_files[i];
    return NULL;
}
Database::ObjectItem * Database::findObjectByName(QString name)
{
    for(int i = 0; i < list_objects.size(); i++)
        if(list_objects[i]->name == name)
            return list_objects[i];
    return NULL;
}
Database::ImageFile* Database::createFile(QString name)
{
    ImageFile* i = new ImageFile;
    i->name = name;
    i->flags = 0;
    i->scale = 0;
    list_files.append(i);
    return i;
}
Database::LayoutItem* Database::createItem(LayoutPage *page, QString name)
{
    LayoutItem* i = new LayoutItem;
    i->name = name;
    i->scale = 0.80000001192092896;
    i->pos = QPointF(0,0);
    i->border = false;
    i->ruler = false;
    page->list_items.append(i);
    object_model.findItems(i->name).first()->setIcon(icon_done);
    return i;
}
Database::ImageFile* Database::getFileByName(QString name)
{
    ImageFile* i = findFileByName(name);
    return i ? i : createFile(name);
}
Database::LayoutPage* Database::getLayoutByName(QString name)
{
    for(int i = 0; i < list_layouts.size(); i++)
        if(list_layouts[i]->name == name)
            return list_layouts[i];

    QStandardItem * it = new QStandardItem(name);
    LayoutPage* i = new LayoutPage;
    i->name = name;
    list_layouts.append(i);
    layout_model.appendRow(it);
    return i;
}
