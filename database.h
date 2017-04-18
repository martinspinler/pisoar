#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QDir>
#include <QString>
#include <QPoint>
#include <QMap>
#include <QMultiMap>
#include <QPair>
#include <QList>
#include <QJsonObject>
#include <QStringList>
#include <QStandardItemModel>

#include "settings.h"

#define PISOAR_CURRENT_VERSION_MAJOR 0
#define PISOAR_CURRENT_VERSION_MINOR 21
#define PISOAR_CURRENT_VERSION ((PISOAR_CURRENT_VERSION_MAJOR << 16) + PISOAR_CURRENT_VERSION_MINOR)

#include <functional>
#include <vector>

#include "database/imagefile.h"
#include "database/objectimage.h"
#include "database/objectitem.h"
#include "database/layoutitem.h"
#include "database/layoutpage.h"

namespace DB {

class Database : public QObject
{
    Q_OBJECT

    friend class ImageFile;

public:
    class ObjectItemModel : public QStandardItemModel {
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    };

private:
    int  object_maxid;

    QDir dir_base;
    QDir dir_items;
    QDir dir_layouts;

    QList <ImageFile*> list_files;
    QStringList list_filenames;

    bool m_bModified;
public:

    Settings set;
    ObjectItemModel object_model;
    QStandardItemModel view_model;
    QStandardItemModel layout_model;

    Database();
    ~Database() {clear();}

    bool  modified()                {return m_bModified;}
    void  setModified()             {m_bModified = true;}
    bool  create(QDir dir);
    bool  open(QDir dir);
    void  save();
    void  clear();

    const QDir & dirBase()       {return dir_base;}
    const QDir & dirItems()      {return dir_items;}
    const QDir & dirLayouts()    {return dir_layouts;}

    ObjectItem *createObject(QString name);
    bool        removeObject(ObjectItem *item, bool force = false);

    ImageFile * imageFileByPath(QString path);
    const QList<ObjectImage*> objectImagesByFile(ImageFile* file);

    LayoutPage* createLayout(QString name);
};

}

using namespace DB;

extern Database * db;


#endif // DATABASE_H
