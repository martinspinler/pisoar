#ifndef OBJECTITEM_H
#define OBJECTITEM_H

#include "common.h"

namespace DB {

class ObjectItem : public QStandardItem, public EventListener<ObjectImageChange>, public EventBroadcaster<ObjectItemChange> {
    static QStringList s_filenames;

    QString m_filename;

public:
    QList<ObjectView*> views;
    QList<ObjectImage*> images;

    ObjectItem(const QString & name);
    ObjectItem(const QJsonObject & obj);
    QJsonObject toJsonObject();

    virtual ~ObjectItem();

    int addImage(ObjectImage * image);
    QString imagePath(int index);

    bool rename(QString newName);
    bool clean();
    bool isUsed();

    bool _canRename(QString newName);
    void _rename(QString newName);
    void _renameFiles(QString newFilename);
    QString _getNewFilename(QString newName);

    void update();
    void    event(ObjectImageChange & event);
};

}

#endif // OBJECTITEM_H
