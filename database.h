#ifndef DATABASE_H
#define DATABASE_H

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


class Database
{

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
        //int count;
        QList<ObjectView*> views;
        QStandardItem *it;
    };
    struct LayoutItem {
        QString name;
        bool ruler;
        bool border;
        float scale;
        QPointF pos;
    };
    struct LayoutPage {
        QString name;
        QList<LayoutItem*> list_items;
    };

private:
    //QJsonObject  json;
    QDir dir_base;
    QDir dir_items;
    QDir dir_layouts;

    QList <ImageFile*>  list_files;
    QList <ObjectItem*> list_objects;
    QList <LayoutPage*> list_layouts;

public:

    QIcon icon_up;
    QIcon icon_folder;
    QIcon icon_wip;
    QIcon icon_done;
    QIcon icon_image;
    QIcon icon_0;
    QIcon icon_1;
    QIcon icon_2;

    QStandardItemModel object_model;
    QStandardItemModel layout_model;

    Database();
    ~Database() { clear(); }

    bool  create(QDir dir);
    bool  open(QDir dir);
    void  save();
    void  clear();

    const QDir & getDirBase() {return dir_base;}
    const QDir & getDirItems() {return dir_items;}
    const QDir & getDirLayouts() {return dir_layouts;}

    ObjectItem *createObject(QString name);
    ObjectItem *findObjectByName(QString name);

    ObjectView *createView(ObjectItem* item, QString filename, QPoint pt);

    LayoutItem *createItem(LayoutPage* page, QString name);

    ImageFile * createFile(QString name);
    ImageFile * findFileByName(QString name);
    ImageFile * getFileByName(QString name);

    LayoutPage* getLayoutByName(QString name);
};

#endif // DATABASE_H
