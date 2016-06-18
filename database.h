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

class Database : public QObject
{
    Q_OBJECT

public:
    struct ImageFile {
        QString name;
        float scale;
        int flags;
    };
    struct ObjectView {
        QString filename;
        QPoint pos;
    };
    struct ObjectItem {
        QString name;
        QList<ObjectView*> views;
    };
    struct LayoutItem {
        QString name;
        QPointF pos;
        float scale;
        bool ruler;
        bool border;
        int type;
    };
    struct LayoutPage {
        QString name;
        QList<LayoutItem*> list_items;
    };

    class ObjectItemModel : public QStandardItemModel {
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    };

private:
    int  object_maxid;

    QDir dir_base;
    QDir dir_items;
    QDir dir_layouts;

    QList <ImageFile*>  list_files;
    QList <ObjectItem*> list_objects;
    QList <LayoutPage*> list_layouts;

    bool bIsModified;

public:
    QIcon icon_up;
    QIcon icon_folder;
    QIcon icon_wip;
    QIcon icon_done;
    QIcon icon_image;
    QIcon icon_0;
    QIcon icon_1;
    QIcon icon_2;

    ObjectItemModel object_model;
    QStandardItemModel layout_model;

    Database();
    ~Database() {clear();}

    bool  isModified() {return bIsModified;}
    bool  create(QDir dir);
    bool  open(QDir dir);
    void  save();
    void  clear();

    const QDir & getDirBase() {return dir_base;}
    const QDir & getDirItems() {return dir_items;}
    const QDir & getDirLayouts() {return dir_layouts;}

    ObjectItem *createObject(QString name);
    ObjectItem *findObjectByName(QString name);
    bool        removeObject(QString name);
    bool        cleanObject(QString name);

    ObjectView *createView(ObjectItem* item, QString filename, QPoint pt);

    LayoutItem* createItem(LayoutPage *page, QString name);
    void        removeItem(LayoutPage* page, LayoutItem * item);

    ImageFile * createFile(QString name);
    ImageFile * findFileByName(QString name);
    ImageFile * getFileByName(QString name);
    const QList<QPair<QString, QPoint> > getPointsByFile(QString file);

    LayoutPage* createLayout(QString name);
    LayoutPage* getLayoutByName(QString name);

    void        object_itemChanged(QStandardItem * item);
};

#endif // DATABASE_H
