#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

#include "database.h"
#include "factory.h"

#define ALLOW_DUPLICATES false

bool Database::ObjectItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = false;

    QString newFilename;
    QString newName = value.toString();
    ObjectItem * i = (ObjectItem*) item(index.row());

    if(!ALLOW_DUPLICATES) {
        for(int i = 0; i < rowCount(); i++) {
            if(item(i)->text() == newName && i != index.row()) {
                QMessageBox(QMessageBox::Warning, "Špatné jméno", "Objekt s takovým názvem již existuje.").exec();
                return false;
            }
        }
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

Database::ImageFile::ImageFile(const QString & path) : m_path(path), m_flags(FLAG_NONE), m_scale(0)
{
    QFileInfo fi(path);
    setText(fi.fileName());
    setIcon(f->icon_image);
}
Database::ImageFile::ImageFile(const ImageFile & file) : QStandardItem(file)
{
    m_path  = file.m_path;
    m_scale = file.m_scale;
    m_flags = file.m_flags;
    setText(file.text());
    updateFlags();
}

Database::ImageFile::ImageFile(const QJsonObject &obj)
{
    m_path  = obj["name"].toString();
    m_scale = obj["scale"].toDouble();
    m_flags = (Flags) obj["flags"].toInt();
    QFileInfo fi(m_path);
    setText(fi.fileName());
    updateFlags();
}
QJsonObject Database::ImageFile::toJsonObject()
{
    QJsonObject obj;
    obj["name"] = m_path;
    obj["scale"] = m_scale;
    obj["flags"] = (int) m_flags;
    return obj;
}
void Database::ImageFile::setScale(float scale)
{
    this->m_scale = scale;
    db->setModified();
}
void Database::ImageFile::setFlags(Flags flags)
{
    this->m_flags = flags;
    updateFlags();
    db->setModified();
}
void Database::ImageFile::updateFlags()
{
    switch(m_flags) {
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
    m_file = NULL;

    pos.setX(obj["pointx"].toDouble());
    pos.setY(obj["pointy"].toDouble());
    m_object = pos;

    if(obj.contains("type") && obj["type"].toString() == "rect") {
        rect.setTopLeft(pos);
        rect.setWidth(obj["width"].toDouble());
        rect.setHeight(obj["height"].toDouble());
        m_object = rect;
    }

    for(int i = 0; i < db->list_files.size(); i++) {
        if(db->list_files.at(i)->path() == path) {
            m_file = db->list_files.at(i);
            break;
        }
    }
    Q_ASSERT(m_file != NULL);
}
QJsonObject Database::ObjectImage::toJsonObject()
{
    QJsonObject obj;
    obj["filename"] = m_file->path();
    switch (m_object.type()) {
    case QVariant::Point:
        obj["type"] = "point";
        obj["pointx"] = m_object.toPoint().x();
        obj["pointy"] = m_object.toPoint().y();
        break;
    case QVariant::Rect:
        obj["type"] = "rect";
        obj["pointx"] = m_object.toRect().topLeft().x();
        obj["pointy"] = m_object.toRect().topLeft().y();
        obj["width"]  = m_object.toRect().width();
        obj["height"] = m_object.toRect().height();
        break;
    default:
        break;
    }
    return obj;
}
Database::ObjectItem::ObjectItem(const QString(&name)) : QStandardItem(name)
{
    setIcon(f->icon_0);
    m_filename = _getNewFilename(name);

    // TODO: 1 default view
    ObjectView * view = new ObjectView(*this, name);
    views.append(view);
    db->view_model.appendRow(view);
}
Database::ObjectItem::ObjectItem(const QJsonObject &obj)
{
    setText(obj["name"].toString());

    // Old DB format compatibility
    if(!obj.contains("file"))
        m_filename = text();
    else
        m_filename = obj["file"].toString();

    QJsonArray items = obj["items"].toArray();
    for(QJsonArray::const_iterator it = items.constBegin(); it != items.constEnd(); it++ ) {
        QJsonObject o = (*it).toObject();
        images.append(new ObjectImage(o));
    }

    ObjectView *view;
    // Old DB format compatibility
    if(!obj.contains("views")) {
        view = new ObjectView(*this, text());
        views.append(view);
        db->view_model.appendRow(view);
    } else {
        QJsonArray viewsArray = obj["views"].toArray();
        for(QJsonArray::const_iterator it = viewsArray.constBegin(); it != viewsArray.constEnd(); it++ ) {
            QJsonObject o = (*it).toObject();
            view = new ObjectView(o, *this);
            views.append(view);
            db->view_model.appendRow(view);
        }
    }
}
QJsonObject Database::ObjectItem::toJsonObject()
{
    QJsonObject obj;
    QJsonArray items;
    for(QList<ObjectImage*>::const_iterator i = images.constBegin(); i != images.constEnd(); i++)
        items.append((*i)->toJsonObject());

    QJsonArray viewsArray;
    for(QList<ObjectView*>::const_iterator i = views.constBegin(); i != views.constEnd(); i++)
        viewsArray.append((*i)->toJsonObject());

    obj["name"] = text();
    obj["file"] = m_filename;
    obj["items"] = items;
    obj["views"] = viewsArray;
    return obj;
}
Database::LayoutItem::LayoutItem(ObjectView* view) : m_objectView(view), m_ruler(false), m_border(false)
{
    m_scale = db->set.dpi / 2.54;
    m_pos = QPointF(0, 0);
}
Database::LayoutItem::LayoutItem(QJsonObject &obj)
{
    m_objectView = (ObjectView*) db->view_model.findItems(obj["name"].toString()).first();
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
QJsonObject Database::LayoutItem::toJsonObject()
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
Database::LayoutPage::LayoutPage(QJsonObject & obj)
{
    setText(obj["name"].toString());
    QJsonArray items = obj["items"].toArray();
    for(QJsonArray::const_iterator it = items.constBegin(); it != items.constEnd(); it++) {
        QJsonObject o = (*it).toObject();
        LayoutItem *li = new LayoutItem(o);
        li->objectView()->setIcon(f->icon_done);
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
Database::ObjectView::ObjectView(const QJsonObject & obj, Database::ObjectItem & item) : item(item)
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
        m_type = item.images.size() > 2 ? 2: item.images.size();
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
            m_rotation.append((*it).toInt());
        }
    }

    setIcon(f->icon_image);
}
QJsonObject Database::ObjectView::toJsonObject()
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
Database::ObjectView::ObjectView(Database::ObjectItem & item, QString name) : item(item)
{
    setText(name);
    m_type = -1;

    for(int i = 0; i < item.images.size(); i++) {
        m_mapping.append(i);
        m_rotation.append(0);
    }

    setIcon(f->icon_image);
}

Database::ObjectItem* Database::createObject(QString name)
{
    if(name.isEmpty())
        return NULL;

    if(!ALLOW_DUPLICATES && db->findObjectByName(name)) {
        QMessageBox(QMessageBox::Warning, "Špatné jméno", "Objekt s takovým názvem již existuje.").exec();
        return NULL;
    }

    ObjectItem * item = new ObjectItem(name);
    object_model.appendRow(item);

    setModified();
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
    foreach(Database::ObjectView* view, views) {
        view->m_mapping.append(view->m_mapping.size());
        view->m_rotation.append(view->m_mapping.size());
    }

    db->setModified();
    return image;
}
bool Database::ObjectItem::_canRename(QString newName)
{
    return !(newName.isEmpty());
}
QString Database::ObjectItem::_getNewFilename(QString newName)
{
    int i = -1;
    QFile file;
    QString newFilename;
    QString fnAlnum = newName.replace(QRegExp(QString::fromUtf8("[^a-zA-Z0-9_-]")), "_");

    if(fnAlnum == m_filename)
        return m_filename;

    do {
        newFilename = fnAlnum;
        if(++i != 0)
            newFilename += "_" + QString::number(i);
        file.setFileName(db->dir_items.filePath("." + newFilename));
    } while(file.exists());

    /*
    if(!file.open(QFile::OpenModeFlag::WriteOnly)) {
        throw QString("Nelze vytvořit soubory s takovým názvem.");
    }
    file.close();
    file.remove();
    */

    return newFilename;
}
void Database::ObjectItem::_renameFiles(QString newFilename)
{
    QString oldFilename = m_filename;
    QFile::remove(db->dir_items.filePath("." + m_filename));

    m_filename = newFilename;
    QFile file(db->dir_items.filePath("." + newFilename));
    file.open(QFile::OpenModeFlag::WriteOnly);
    file.close();

    for(int i = 0; i < images.size(); i++) {
        QString ofp = oldFilename + "_" + QString::number(i) + ".png";
        QString nfp = newFilename + "_" + QString::number(i) + ".png";
        db->dir_items.rename(ofp, nfp);
    }
}
void Database::ObjectItem::_rename(QString newName)
{
    // TODO: 1 default view
    views[0]->setText(newName);
}
bool Database::ObjectItem::rename(QString newName)
{
    QString newFilename;
    if(_canRename(newName)) {
        newFilename = _getNewFilename(newName);
        setText(newName);
        _rename(newName);
        _renameFiles(newFilename);
        return true;
    } else
        return false;
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

const QList<QPair<Database::ObjectItem *, QVariant> > Database::getObjectsByFile(ImageFile* file)
{
    QList<QPair<Database::ObjectItem*, QVariant> > list;
    for(int i = 0; i < object_model.rowCount(); i++) {
        ObjectItem * item = (ObjectItem*) object_model.item(i);
        for(int j = 0; j < item->images.size(); j++) {
            if(item->images[j]->path() == file->path()) {
                QPair<Database::ObjectItem*, QVariant> pair;
                pair.first = item;
                pair.second= item->images[j]->object();
                list.append(pair);
            }
        }
    }
    return list;
}

Database::ImageFile * Database::findFileByName(QString name)
{
    for(int i = 0; i < list_files.size(); i++)
        if(list_files[i]->path() == name)
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
    list_files.append(file);
    return file;
}
Database::LayoutItem* Database::createItem(LayoutPage *page, ObjectView* objectView)
{
    LayoutItem* item = new LayoutItem(objectView);

    page->list_items.append(item);
    objectView->setIcon(f->icon_done);

    setModified();
    return item;
}
void Database::removeItem(LayoutPage *page, LayoutItem * item)
{
    page->list_items.removeAll(item);

    for(int i = 0; i < layout_model.rowCount(); i++) {
        LayoutPage * layout = (LayoutPage*) layout_model.item(i);
        if(layout->list_items.contains(item))
            return;
    }
    item->objectView()->setIcon(f->icon_image);
    setModified();
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

    setModified();
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
            if(&layout->list_items[j]->objectView()->item == item) {
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
        dir_items.remove(item->filename() + "_" + QString::number(i) + ".png");
        delete item->images[i];
        setModified();
    }
    item->images.clear();
    item->setIcon(f->icon_0);
    return true;
}
