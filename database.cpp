#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

#include "database.h"
#include "factory.h"

bool Database::ObjectItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = false;

    QString newFilename;
    QString newName = value.toString();
    ObjectItem * i = (ObjectItem*) item(index.row());

    if(newName != i->text() && findItems(newName).size() != 0) {
        QMessageBox(QMessageBox::Warning, "Špatné jméno", "Objekt s takovým názvem již existuje.").exec();
        return ret;
    }

    if(i->_canRename(newName)) {
        newFilename = i->_getNewFilename(newName);
        ret = QStandardItemModel::setData(index, value, role);
        i->_rename(newName);
        if(ret)
            i->_renameFiles(newFilename);
    }
    if(!ret)
        QMessageBox(QMessageBox::Warning, "Špatné jméno", "Nelze použít takové jméno.").exec();
    return ret;
}

Database::Database()
{
    m_bModified = false;
}
void Database::clear()
{
    while(!list_files.isEmpty())
        delete list_files.takeFirst();

    object_model.clear();
    view_model.clear();
    layout_model.clear();
    m_bModified = false;
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
        object_model.appendRow(objectItem);
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

    m_bModified = false;
    return true;
}
void Database::save()
{
    /* Store files */
    QJsonArray files;
    for(QList<ImageFile*>::const_iterator i = list_files.constBegin(); i != list_files.constEnd(); i++) {
        QJsonObject obj = (*i)->toJsonObject();
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
    json["base"]    = dir_base.absolutePath();
    json["version"] = PISOAR_CURRENT_VERSION;
    json["files"]   = files;
    json["objects"] = objects;
    json["layouts"] = layouts;
    json["settings"] = set.toJsonObject();

    QJsonDocument doc(json);
    QFile file("pisoardb.json");
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());

    m_bModified = false;
}

ObjectItem* Database::createObject(QString name)
{
    if(name.isEmpty()) {
        throw QString("Jméno je prázdné");
    }
    if(!db->object_model.findItems(name).isEmpty()) {
        throw QString("Objekt s takovým názvem již existuje");
    }

    ObjectItem * item = new ObjectItem(name);
    object_model.appendRow(item);

    setModified();
    return item;
}

const QList<ObjectImage*> Database::objectImagesByFile(ImageFile* file)
{
    QList<ObjectImage* > list;
    for(int i = 0; i < object_model.rowCount(); i++) {
        ObjectItem * item = (ObjectItem*) object_model.item(i);
        foreach(ObjectImage* image, item->images) {
            if(image->path() == file->path()) {
                list.append(image);
            }
        }
    }
    return list;
}

ImageFile* Database::imageFileByPath(QString path)
{
    ImageFile * imageFile;
    foreach(ImageFile * file, list_files) {
        if(file->path() == path) {
            return file;
        }
    }

    imageFile = new ImageFile(path);
    list_files.append(imageFile);
    return imageFile;
}
LayoutPage* Database::createLayout(QString name)
{
    LayoutPage* layout = new LayoutPage(name);
    layout_model.appendRow(layout);

    setModified();
    return layout;
}
bool Database::removeObject(ObjectItem* item)
{
    if(!item->clean())
        return false;

    object_model.removeRow(item->row());
    return true;
}
