#include <QFileInfo>

#include "imagefile.h"
#include "../database.h"

namespace DB {

ImageFile::ImageFile(const QString & path) : m_path(path), m_flags(FLAG_NONE), m_scale(0)
{
    QFileInfo fi(path);
    setText(fi.fileName());
    setIcon(f->icon_image);
}
ImageFile::ImageFile(const ImageFile & file) : QStandardItem(file)
{
    m_path  = file.m_path;
    m_scale = file.m_scale;
    m_flags = file.m_flags;
    setText(file.text());
    updateFlags();
}
ImageFile::ImageFile(const QJsonObject &obj)
{
    m_path  = obj["name"].toString();
    m_scale = obj["scale"].toDouble();
    m_flags = (Flags) obj["flags"].toInt();

    QFileInfo fi(m_path);
    setText(fi.fileName());
    updateFlags();
}
QJsonObject ImageFile::toJsonObject()
{
    QJsonObject obj;
    obj["name"] = m_path;
    obj["scale"] = m_scale;
    obj["flags"] = (int) m_flags;
    return obj;
}
void ImageFile::setScale(float scale)
{
    m_scale = scale;

    ScaleChange s;
    s.image = this;
    emitEvent(s);

    db->setModified();
}
void ImageFile::setFlags(Flags flags)
{
    m_flags = flags;
    updateFlags();
    db->setModified();
}
void ImageFile::updateFlags()
{
    switch(m_flags) {
    case FLAG_NONE: setIcon(f->icon_image); break;
    case FLAG_WIP:  setIcon(f->icon_wip);   break;
    case FLAG_DONE: setIcon(f->icon_done);  break;
    }
}

}
