#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

#include "database.h"
#include "factory.h"

bool Database::ObjectItemModel::setData(const QModelIndex &index, const QVariant &value, int role)  {
    bool ret = false;
    QString oldName = item(index.row())->text();
    QString newName = value.toString();
    if(newName.isEmpty())
        return ret;
    if(newName == oldName)
        return QStandardItemModel::setData(index, value, role);

    for(int i = 0; i < rowCount(); i++) {
        if(item(i)->text() == newName && i != index.row()) {
            QMessageBox(QMessageBox::Warning, "Špatné jméno", "Objekt s takovým názvem již existuje.").exec();
            return ret;
        }
    }

    QFile file(db->dir_items.filePath(newName));
    if(!file.open(QFile::OpenModeFlag::WriteOnly)) {
        QMessageBox(QMessageBox::Warning, "Špatné jméno", "Nelze vytvořit soubory s takovým názvem.").exec();
        return ret;
    }
    file.close();
    file.remove();

    ret = QStandardItemModel::setData(index, value, role);

    if(ret)
        for(int i = 0; i < ((Database::ObjectItem*)item(index.row()))->images.size(); i++)
            db->dir_items.rename(oldName + "_" + QString::number(i) + ".png", newName + "_" + QString::number(i) + ".png");

    return ret;
}


Database::ImageFile::ImageFile(const QString & path) : path(path), flags(FLAG_NONE), scale(0)
{
    QFileInfo fi(path);
    setText(fi.fileName());
    setIcon(f->icon_image);
}
Database::ImageFile::ImageFile(const ImageFile & file) : QStandardItem(file)
{
    path  = file.path;
    scale = file.scale;
    flags = file.flags;
    setText(file.text());
    updateFlags();
}

Database::ImageFile::ImageFile(const QJsonObject &obj)
{
    path  = obj["name"].toString();
    scale = obj["scale"].toDouble();
    flags = (Flags) obj["flags"].toInt();
    QFileInfo fi(path);
    setText(fi.fileName());
    updateFlags();
}
QJsonObject Database::ImageFile::toJsonObject()
{
    QJsonObject obj;
    if(scale == 0 && flags == FLAG_NONE)
        return obj;

    obj["name"] = path;
    obj["scale"] = scale;
    obj["flags"] = (int) flags;
    return obj;
}
void Database::ImageFile::setScale(float scale)
{
    this->scale = scale;
    db->bIsModified = true;
}
void Database::ImageFile::setFlags(Flags flags)
{
    this->flags = flags;
    updateFlags();
    db->bIsModified = true;
}
void Database::ImageFile::updateFlags()
{
    switch(flags) {
    case FLAG_NONE: setIcon(f->icon_image); break;
    case FLAG_WIP:  setIcon(f->icon_wip);   break;
    case FLAG_DONE: setIcon(f->icon_done);  break;
    }
}
Database::ObjectImage::ObjectImage(const QJsonObject &obj)
{
    QString path = obj["filename"].toString();
    QPoint pos;
    QRect rect;

    pos.setX(obj["pointx"].toDouble());
    pos.setY(obj["pointy"].toDouble());
    object = pos;

    if(obj.contains("type") && obj["type"].toString() == "rect") {
        rect.setTopLeft(pos);
        rect.setWidth(obj["width"].toDouble());
        rect.setHeight(obj["height"].toDouble());
        object = rect;
    }

    for(int i = 0; i < db->list_files.size(); i++) {
        if(db->list_files.at(i)->path == path) {
            file = db->list_files.at(i);
            break;
        }
    }
}
QJsonObject Database::ObjectImage::toJsonObject()
{
    QJsonObject obj;
    obj["filename"] = file->path;
    switch (object.type()) {
    case QVariant::Point:
        obj["type"] = "point";
        obj["pointx"] = object.toPoint().x();
        obj["pointy"] = object.toPoint().y();
        break;
    case QVariant::Rect:
        obj["type"] = "rect";
        obj["pointx"] = object.toRect().topLeft().x();
        obj["pointy"] = object.toRect().topLeft().y();
        obj["width"] = object.toRect().width();
        obj["height"] = object.toRect().height();
        break;
    default:
        break;
    }
    return obj;
}
Database::ObjectItem::ObjectItem(const QString(&name)) : QStandardItem(name) {setIcon(f->icon_0);}
Database::ObjectItem::ObjectItem(const QJsonObject &obj)
{
    setText(obj["name"].toString());
    QJsonArray items = obj["items"].toArray();
    for(QJsonArray::const_iterator it = items.constBegin(); it != items.constEnd(); it++ ) {
        QJsonObject o = (*it).toObject();
        images.append(new ObjectImage(o));
    }
}
QJsonObject Database::ObjectItem::toJsonObject()
{
    QJsonObject obj;
    QJsonArray items;
    for(QList<ObjectImage*>::const_iterator i = images.constBegin(); i != images.constEnd(); i++)
        items.append((*i)->toJsonObject());

    obj["name"] = text();
    obj["items"] = items;
    return obj;
}
Database::LayoutItem::LayoutItem(ObjectItem * item) : objectItem(item), ruler(0), border(0), type(0)
{
    scale = db->set.dpi / 2.54;
}
Database::LayoutItem::LayoutItem(QJsonObject &obj)
{
    objectItem = (ObjectItem*) db->object_model.findItems(obj["name"].toString()).first();
    type    = obj["type"].toInt();
    scale   = obj["scale"].toDouble();
    ruler   = obj["ruler"].toBool();
    border  = obj["border"].toBool();
    pos.setX(obj["pointx"].toInt());
    pos.setY(obj["pointy"].toInt());
}
QJsonObject Database::LayoutItem::toJsonObject()
{
    QJsonObject obj;
    obj["name"] = objectItem->text();
    obj["type"] = type;
    obj["scale"] = scale;
    obj["ruler"] = ruler;
    obj["border"] = border;
    obj["pointx"] = (int) pos.x();
    obj["pointy"] = (int) pos.y();
    return obj;
}
Database::LayoutPage::LayoutPage(QJsonObject & obj)
{
    setText(obj["name"].toString());
    QJsonArray items = obj["items"].toArray();
    for(QJsonArray::const_iterator it = items.constBegin(); it != items.constEnd(); it++ ) {
        QJsonObject o = (*it).toObject();
        LayoutItem *li = new LayoutItem(o);
        li->objectItem->setIcon(f->icon_done);
        list_items.append(li);
    }
}
QJsonObject Database::LayoutPage::toJsonObject()
{
    QJsonObject obj;
    QJsonArray items;

    for(QList<LayoutItem*>::const_iterator i = list_items.constBegin(); i != list_items.constEnd(); i++)
        items.append((*i)->toJsonObject());

    obj["name"] = text();
    obj["items"] = items;
    return obj;
}
/*Database::ObjectView::ObjectView(int type, const QList<ImageFile *> &list_files)
{
}
Database::ObjectView* Database::ObjectItem::createView(int type, QList<ObjectImage*> images)
{
    //ObjectView * view = new ObjectView();
}*/

Database::ObjectItem* Database::createObject(QString name)
{
    if(name.isEmpty())
        return NULL;

    if(db->findObjectByName(name)) {
        QMessageBox(QMessageBox::Warning, "Špatné jméno", "Objekt s takovým názvem již existuje.").exec();
        return NULL;
    }

    QFile file(dir_items.filePath(name));
    if(!file.open(QFile::OpenModeFlag::WriteOnly)) {
        QMessageBox(QMessageBox::Warning, "Špatné jméno", "Nelze vytvořit soubory s takovým názvem.").exec();
        return NULL;
    }
    file.close();
    file.remove();

    ObjectItem * item = new ObjectItem(name);
    QList <QStandardItem*> list;
    list.append(item);
    list.append(new QStandardItem(QString::number(object_maxid++).rightJustified(6, '0')));
    object_model.appendRow(list);

    bIsModified = true;
    return item;
}
Database::ObjectImage* Database::ObjectItem::createImage(ImageFile * file, QVariant obj)
{
    ObjectImage * image = new ObjectImage(file, obj);
    images.append(image);

    switch(images.size()) {
    case 0: setIcon(f->icon_0); break;
    case 1: setIcon(f->icon_1); break;
    case 2: setIcon(f->icon_2); break;
    default:setIcon(f->icon_image); break;
    }

    db->bIsModified = true;
    return image;
}

Database::Database()
{
	bIsModified = false;
}
void Database::clear()
{
    while(!list_files.isEmpty())
        delete list_files.takeFirst();

    object_model.clear();
    layout_model.clear();
    bIsModified = false;
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
    QJsonArray files = json["files"].toArray();
    for(QJsonArray::const_iterator i = files.constBegin(); i != files.constEnd(); i++ ) {
        QJsonObject obj = (*i).toObject();
        ImageFile * file = new ImageFile(obj);
        list_files.append(file);
    }

    /* Parse objects */
    object_maxid = 0;
    QJsonArray objs = json["objects"].toArray();
    for(QJsonArray::const_iterator i = objs.constBegin(); i != objs.constEnd(); i++ ) {
        QJsonObject obj = (*i).toObject();
        ObjectItem * objectItem = new ObjectItem(obj);

        QIcon icon = f->icon_image;
        switch(objectItem->images.size()) {
            case 0: icon = f->icon_0; break;
            case 1: icon = f->icon_1; break;
            case 2: icon = f->icon_2; break;
        }
        objectItem->setIcon(icon);

        QList <QStandardItem*> list;
        list.append(objectItem);
        list.append(new QStandardItem(QString::number(object_maxid++).rightJustified(6, '0')));
        object_model.appendRow(list);
    }

    /* Parse layouts */
    QJsonArray layouts = json["layouts"].toArray();
    for(QJsonArray::const_iterator i = layouts.constBegin(); i != layouts.constEnd(); i++ ) {
        QJsonObject obj = (*i).toObject();
        LayoutPage *layoutPage = new LayoutPage(obj);
        layout_model.appendRow(layoutPage);
    }

    if(json.contains("settings"))
        set.fromJsonObject(json["settings"].toObject());

    bIsModified = false;
    return true;
}
void Database::save()
{
    /* Store files */
    QJsonArray files;
    for(QList<ImageFile*>::const_iterator i = list_files.constBegin(); i != list_files.constEnd(); i++) {
        QJsonObject obj = (*i)->toJsonObject();
        if(!obj.isEmpty())
            files.append(obj);
    }

    /* Store objects */
    QJsonArray objects;
    for(int i = 0; i < object_model.rowCount(); i++) {
        ObjectItem * item = (ObjectItem*) object_model.item(i);
        objects.append(item->toJsonObject());
    }

    /* Store layouts */
    QJsonArray layouts;
    for(int i = 0; i < layout_model.rowCount(); i++) {
        LayoutPage * layoutPage = (LayoutPage*) layout_model.item(i);
        layouts.append(layoutPage->toJsonObject());
    }

    QJsonObject json;
    json["base"] = dir_base.absolutePath();
    json["version"] = PISOAR_CURRENT_VERSION;
    json["files"] = files;
    json["objects"] = objects;
    json["layouts"] = layouts;
    json["settings"] = set.toJsonObject();

    QJsonDocument doc(json);
    QFile file("pisoardb.json");
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());

    bIsModified = false;
}

const QList<QPair<Database::ObjectItem *, QVariant> > Database::getObjectsByFile(ImageFile* file)
{
    QList<QPair<Database::ObjectItem*, QVariant> > list;
    for(int i = 0; i < object_model.rowCount(); i++) {
        ObjectItem * item = (ObjectItem*) object_model.item(i);
        for(int j = 0; j < item->images.size(); j++) {
            if(item->images[j]->file == file) {
                QPair<Database::ObjectItem*, QVariant> pair;
                pair.first = item;
                pair.second= item->images[j]->object;
                list.append(pair);
            }
        }
    }
    return list;
}

Database::ImageFile * Database::findFileByName(QString name)
{
    for(int i = 0; i < list_files.size(); i++)
        if(list_files[i]->path == name)
            return list_files[i];
    return NULL;
}
Database::ObjectItem * Database::findObjectByName(QString name)
{
    for(int i = 0; i < object_model.rowCount(); i++)
        if(object_model.item(i)->text() == name)
            return (ObjectItem*)object_model.item(i);
    return NULL;
}
Database::ImageFile* Database::createFile(const QString & name)
{
    ImageFile* file = new ImageFile(name);
    file->flags = ImageFile::FLAG_NONE;
    file->scale = 0;
    file->setIcon(f->icon_image);
    list_files.append(file);
    return file;
}
Database::LayoutItem* Database::createItem(LayoutPage *page, ObjectItem* objectItem)
{
    LayoutItem* item = new LayoutItem(objectItem);
    item->scale = 1.0;
    item->pos = QPointF(0,0);
    item->border = false;
    item->ruler = false;
    page->list_items.append(item);
    item->objectItem->setIcon(f->icon_done);

    bIsModified = true;
    return item;
}
void Database::removeItem(LayoutPage *page, LayoutItem * item)
{
    page->list_items.removeAll(item);

    QIcon icon = f->icon_image;
    switch(item->objectItem->images.size()) {
        case 0: icon = f->icon_0; break;
        case 1: icon = f->icon_1; break;
        case 2: icon = f->icon_2; break;
    }
    for(int i = 0; i < layout_model.rowCount(); i++) {
        LayoutPage * layout = (LayoutPage*) layout_model.item(i);
        if(layout->list_items.contains(item))
            return;
    }
    item->objectItem->setIcon(icon);
    bIsModified = true;
}
Database::ImageFile* Database::getFileByName(QString name)
{
    ImageFile* i = findFileByName(name);
    if(i == NULL)
        i = createFile(name);
    return i;
}
Database::LayoutPage* Database::createLayout(QString name)
{
    LayoutPage* layout = new LayoutPage(name);
    layout_model.appendRow(layout);

    bIsModified = true;
    return layout;
}
bool Database::removeObject(ObjectItem* item)
{
    if(!cleanObject(item))
        return false;

    object_model.removeRow(item->row());
    return true;
}

bool Database::cleanObject(ObjectItem* item)
{
     for(int i = 0; i < layout_model.rowCount(); i++) {
        LayoutPage * layout = (LayoutPage*) layout_model.item(i);

        for(int j = 0; j < layout->list_items.size(); j++) {
            if(layout->list_items[j]->objectItem == item) {
                QMessageBox msgBox;
                msgBox.setText("Objekt se používá");
                msgBox.setInformativeText("Nejprve odstraňte všechny instance objektu z layoutů.");
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.exec();
                return false;
            }
        }
    }

    for(int i = 0; i < item->images.size(); i++) {
        dir_items.remove(item->text() + "_" + QString::number(i) + ".png");
        delete item->images[i];
        bIsModified = true;
    }
    item->images.clear();
    item->setIcon(f->icon_0);
    return true;
}
