#include "objectitem.h"

#include "objectview.h"

#include "../database.h"

namespace DB {

QStringList ObjectItem::s_filenames;

ObjectItem::ObjectItem(const QString(&name)) : QStandardItem(name)
{
    setIcon(f->icon_0);
    m_filename = _getNewFilename(name);
    s_filenames.append(m_filename);

    // TODO: 1 default view
    ObjectView * view = new ObjectView(*this, name);
    views.append(view);
    db->view_model.appendRow(view);
}
ObjectItem::ObjectItem(const QJsonObject &obj)
{
    setText(obj["name"].toString());

    // Old DB format compatibility
    if(!obj.contains("file"))
        m_filename = text();
    else
        m_filename = obj["file"].toString();

    s_filenames.append(m_filename);

    QJsonArray items = obj["items"].toArray();
    for(QJsonArray::const_iterator it = items.constBegin(); it != items.constEnd(); it++ ) {
        QJsonObject o = (*it).toObject();
        ObjectImage * image = new ObjectImage(o);
        image->setObjectItem(this);
        images.append(image);
        image->link(this);
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

    QIcon icon = f->icon_image;
    switch(images.size()) {
        case 0: icon = f->icon_0; break;
        case 1: icon = f->icon_1; break;
        case 2: icon = f->icon_2; break;
    }
    setIcon(icon);
}
QJsonObject ObjectItem::toJsonObject()
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

ObjectItem::~ObjectItem()
{
    ObjectImage * image;
    while(!images.isEmpty()) {
        image = images.takeFirst();
        image->unlink(this);
        delete image;
    }
    s_filenames.removeAll(m_filename);
}

bool ObjectItem::isUsed() {
    foreach(ObjectView* view, views) {
        if(view->isUsed()) {
            return true;
        }
    }
    return false;
}
bool ObjectItem::clean(bool force)
{
    if(!force && isUsed()) {
        return false;
    }

    foreach(ObjectView* view, views) {
        view->clean();
    }

    QDir dirItems = db->dirItems();

    for(int i = 0; i < images.size(); i++) {
        dirItems.remove(m_filename + "_" + QString::number(i) + ".png");
        images[i]->unlink(this);
        delete images[i];
    }
    for(int i = 0; i < views.size(); i++) {
        views[i]->m_mapping.clear();
        views[i]->m_rotation.clear();
    }
    images.clear();
    db->setModified();
    setIcon(f->icon_0);
    return true;
}

int ObjectItem::addImage(ObjectImage * image)
{
    images.append(image);
    image->setObjectItem(this);

    switch(images.size()) {
    case 0: setIcon(f->icon_0); break;
    case 1: setIcon(f->icon_1); break;
    case 2: setIcon(f->icon_2); break;
    default:setIcon(f->icon_image); break;
    }
    foreach(ObjectView* view, views) {
        view->m_mapping.append(view->m_mapping.size());
        view->m_rotation.append(0);
    }

    db->setModified();
    return images.size() - 1;
}
bool ObjectItem::_canRename(QString newName)
{
    return !(newName.isEmpty());
}
QString ObjectItem::_getNewFilename(QString newName)
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
    } while(s_filenames.contains(newFilename));
    return newFilename;
}
void ObjectItem::_renameFiles(QString newFilename)
{
    QDir dirItems = db->dirItems();
    for(int i = 0; i < images.size(); i++) {
        dirItems.rename( m_filename + "_" + QString::number(i) + ".png",
                         newFilename + "_" + QString::number(i) + ".png");
    }
    m_filename = newFilename;
}
void ObjectItem::_rename(QString newName)
{
    // TODO: 1 default view
    views[0]->setText(newName);
}
bool ObjectItem::rename(QString newName)
{
    QString newFilename;
    if(_canRename(newName)) {
        newFilename = _getNewFilename(newName);
        setText(newName);
        _rename(newName);
        _renameFiles(newFilename);
        return true;
    } else {
        return false;
    }
}

QString ObjectItem::imagePath(int index)
{
    return db->dirItems().filePath(m_filename + "_" + QString::number(index) + ".png");
}
void ObjectItem::event(ObjectImageChange &event)
{
    Q_UNUSED(event);
    ObjectItemChange e = {this};
    emitEvent(e);
}

void ObjectItem::update() {
    ObjectItemChange e = {this};
    emitEvent(e);
}

}
