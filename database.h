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

#define PISOAR_CURRENT_VERSION 0x00000002

class Database : public QObject
{
    Q_OBJECT

public:
    class ObjectItemModel;

    class ImageFile : public QStandardItem {
    public:
        enum Flags {FLAG_NONE, FLAG_WIP, FLAG_DONE};
    private:
        friend class Database;

        QString path;
        Flags flags;
        int scale;

        void updateFlags();

    public:
        ImageFile(const QJsonObject & obj);
        ImageFile(const QString & path);

        QJsonObject toJsonObject();

        QString getPath() {return path;}
        int getScale() {return scale;}
        Flags getFlags() {return flags;}
        void setScale(float scale);
        void setFlags(Flags flags);
    };
    class ObjectImage {
    public:
        enum Types {TYPE_POINT, TYPE_RECT};

        ImageFile * file;
        QVariant object;

        ObjectImage(const QJsonObject & obj);
        ObjectImage(ImageFile * file, QVariant obj) : file(file), object(obj) {}

        QJsonObject toJsonObject();
    };
    class ObjectView {
    public:
        QList<ObjectImage*> images;
        int type;

        ObjectView(const QJsonObject & obj, const QList<ImageFile *> &list_files);
        ObjectView(int type, const QList<ImageFile *> &list_files);

        QJsonObject toJsonObject();
    };
    class ObjectItem : public QStandardItem {

    public:
        QList<ObjectView*> views;
        QList<ObjectImage*> images;

        ObjectItem(const QString & name);
        ObjectItem(const QJsonObject & obj);

        virtual ~ObjectItem() { while(!images.isEmpty()) delete images.takeFirst(); }

        QJsonObject toJsonObject();

        ObjectImage *createImage(ImageFile* file, QVariant obj);
        ObjectView *createView(int type, QList<ObjectImage *> images);

        //LayoutView loadLayoutView();
    };
    class LayoutItem {
    public:
        ObjectItem * objectItem;
        QPointF pos;
        float scale;
        bool ruler;
        bool border;
        int type;

        LayoutItem(ObjectItem * item);
        LayoutItem(QJsonObject & obj);
        QJsonObject toJsonObject();
    };
    class LayoutPage : public QStandardItem {
    public:
        QList<LayoutItem*> list_items;

        LayoutPage() {}
        LayoutPage(QString &name) : QStandardItem(name) {}
        LayoutPage(QJsonObject & obj);

        virtual ~LayoutPage() { while(!list_items.isEmpty()) delete list_items.takeFirst(); }

        QJsonObject toJsonObject();
    };

    class ObjectItemModel : public QStandardItemModel {
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    };

private:
    int  object_maxid;

    QDir dir_base;
    QDir dir_items;
    QDir dir_layouts;

    QList <ImageFile*> list_files;

    bool bIsModified;

public:

    Settings set;
    ObjectItemModel object_model;
    QStandardItemModel layout_model;

    Database();
    ~Database() {clear();}

    bool  isModified() {return bIsModified;}
    void  setModified() {bIsModified = true;}
    bool  create(QDir dir);
    bool  open(QDir dir);
    void  save();
    void  clear();

    const QDir & getDirBase() {return dir_base;}
    const QDir & getDirItems() {return dir_items;}
    const QDir & getDirLayouts() {return dir_layouts;}

    ObjectItem *createObject(QString name);
    ObjectItem *findObjectByName(QString name);
    bool        removeObject(ObjectItem *item);
    bool        cleanObject(ObjectItem* item);

    LayoutItem* createItem(LayoutPage *page, ObjectItem *objectItem);
    void        removeItem(LayoutPage* page, LayoutItem * item);

    ImageFile * createFile(const QString &name);
    ImageFile * findFileByName(QString name);
    ImageFile * getFileByName(QString name);
    const QList<QPair<Database::ObjectItem*, QVariant> > getObjectsByFile(ImageFile *file);

    LayoutPage* createLayout(QString name);
};

extern Database * db;

#endif // DATABASE_H
