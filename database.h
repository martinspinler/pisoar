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

class Database : public QObject
{
    Q_OBJECT

public:
    class ObjectItemModel;
    class ObjectItem;

    class ImageFile : public QStandardItem {
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

        QString path()   {return m_path;}
        float   scale()  {return m_scale;}
        Flags   flags()  {return m_flags;}
        void    setScale(float scale);
        void    setFlags(Flags flags);
    };
    class ObjectImage {
        enum Types {TYPE_POINT, TYPE_RECT};

        ImageFile * m_file;
        QVariant m_object;

    public:
        ObjectImage(ImageFile * file, QVariant obj) : m_file(file), m_object(obj) {}
        ObjectImage(const QJsonObject & obj);
        QJsonObject toJsonObject();

        const QVariant& object(){return m_object;}
        QString path()   {return m_file->path();}
        float scale()    {return m_file->scale();}
        QString text()   {return m_file->text();}
    };
    class ObjectView : public QStandardItem {
        friend class ObjectItem;

        QList<int> m_mapping;
        QList<int> m_rotation;
        int m_type;

    public:
        ObjectItem & item;
    public:
        ObjectView(ObjectItem & item, QString name);
        ObjectView(const QJsonObject & obj, ObjectItem & item);
        QJsonObject toJsonObject();

        int  type()             {return m_type;}
        void setType(int type)  {m_type = type;}
        //void setMapping(QList<int> mapping);
        void setRotation(int index, int angle)  {m_rotation[index] = angle;}
        int mapping(int index) {return m_mapping[index];}
        int rotation(int index) {return m_rotation[index];}
        QString name() {return item.text();}
    };
    class ObjectItem : public QStandardItem {
        QString m_filename;
    public:
        QList<ObjectView*> views;
        QList<ObjectImage*> images;

        ObjectItem(const QString & name);
        ObjectItem(const QJsonObject & obj);
        QJsonObject toJsonObject();

        virtual ~ObjectItem() { while(!images.isEmpty()) delete images.takeFirst(); }

        ObjectImage *createImage(ImageFile* file, QVariant obj);

        bool rename(QString newName);
        QString filename() {return m_filename;}

        bool _canRename(QString newName);
        void _rename(QString newName);
        void _renameFiles(QString newFilename);
        QString _getNewFilename(QString newName);
    };
    class LayoutItem {
        ObjectView* m_objectView;
        QPointF m_pos;
        float m_scale;
        bool m_ruler;
        bool m_border;

    public:
        LayoutItem(ObjectView *view);
        LayoutItem(QJsonObject & obj);
        QJsonObject toJsonObject();

        QString name()              {return m_objectView->name();}
        bool border()               {return m_border;}
        bool ruler()                {return m_ruler;}
        float scale()               {return m_scale;}
        QPointF pos()               {return m_pos;}
        ObjectView* objectView()    {return m_objectView;}
        void setBorder(bool border) {m_border = border;}
        void setRuler(bool ruler)   {m_ruler = ruler;}
        void setScale(float scale)  {m_scale = scale;}
        void setPos(QPointF pos)    {m_pos = pos;}
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

    const QDir & getDirBase()       {return dir_base;}
    const QDir & getDirItems()      {return dir_items;}
    const QDir & getDirLayouts()    {return dir_layouts;}

    ObjectItem *createObject(QString name);
    ObjectItem *findObjectByName(QString name);
    bool        removeObject(ObjectItem *item);
    bool        cleanObject(ObjectItem* item);

    LayoutItem* createItem(LayoutPage *page, ObjectView *objectItem);
    void        removeItem(LayoutPage* page, LayoutItem * item);

    ImageFile * createFile(const QString &name);
    ImageFile * findFileByName(QString name);
    ImageFile * getFileByName(QString name);
    const QList<QPair<Database::ObjectItem*, QVariant> > getObjectsByFile(ImageFile *file);

    LayoutPage* createLayout(QString name);
};

extern Database * db;

#endif // DATABASE_H
