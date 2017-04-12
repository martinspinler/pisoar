#ifndef IMAGEFILE_H
#define IMAGEFILE_H

#include "common.h"

namespace DB {

class ImageFile : public QStandardItem, public EventBroadcaster<ScaleChange> {
public:
    enum Flags {FLAG_NONE, FLAG_WIP, FLAG_DONE};

private:
    QString m_path;
    Flags   m_flags;
    float   m_scale;
    void    updateFlags();

public:
    ImageFile(const ImageFile & file);
    ImageFile(const QString & path);
    ImageFile(const QJsonObject & obj);
    QJsonObject toJsonObject();

    QString path()                              {return m_path;}
    float   scale()                             {return m_scale;}
    Flags   flags()                             {return m_flags;}
    void    setScale(float scale);
    void    setFlags(Flags flags);
};

}

#endif // IMAGEFILE_H
