#ifndef OBJECTIMAGE_H
#define OBJECTIMAGE_H

#include "common.h"
#include "imagefile.h"

#include <QPixmap>
#include <QImage>

namespace DB {

class ObjectItem;

class ObjectImage : public EventListener<ScaleChange>, public EventBroadcaster<ObjectImageChange> {
    enum Types {TYPE_POINT, TYPE_RECT};
    struct s_check{int x; int y;};

    ObjectItem * m_item;
    ImageFile * m_file;
    QVariant m_object;
    float m_rotation;

public:
    ObjectImage(ImageFile * file, QVariant obj, float rotation);
    ObjectImage(const QJsonObject & obj);
    QJsonObject toJsonObject();

    virtual ~ObjectImage()                      {/*m_file->unlink((EventListener<ScaleChange>*) this);*/}

    void setObjectItem(ObjectItem* item)        {m_item = item;}
    const QVariant& object()                    {return m_object;}
    ImageFile* file()                           {return m_file;}
    QString path()                              {return m_file->path();}
    float   scale()                             {return m_file->scale();}
    float   rotation()                          {return m_rotation;}
    void    setRotation(float rotation)         {m_rotation = rotation;}
    QString text()                              {return m_file->text();}
    QString objectName();
    ObjectItem* objectItem()                    {return m_item;}

    QImage imageFileMask(const QPixmap & pixmap);
    QPixmap pixmapFromMask(QImage &mask, const QPixmap &pixmap);

    void    event(ScaleChange & event);
};

}

#endif // OBJECTIMAGE_H
