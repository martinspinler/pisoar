#include <QGuiApplication>

#include "jaguar.h"
#include "layout.h"
#include "factory.h"
#include "database.h"

Jaguar::Jaguar(QWidget *parent) : QWidget(parent)
{
    layoutView  = NULL;
    objectView  = NULL;

    scene       = new QGraphicsScene();
    view        = new JaguarView();
    view->setScene(scene);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    db_filter   = new QLineEdit();
    db_sort     = new QCheckBox("Řadit podle jména");

    filter      = new QSortFilterProxyModel(this);
    filter->setSourceModel(&db->object_model);
    filter->setDynamicSortFilter(true);

    model_images = new QStandardItemModel(this);
    model_types = new QStandardItemModel(this);
    model_views = new QStandardItemModel(this);

    db_list     = new QListView();
    db_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    db_list->setModel(filter);

    db_viewlist     = new QListView();
    db_viewlist->setEditTriggers(QAbstractItemView::NoEditTriggers);
    db_viewlist->setModel(&db->view_model);

    list_types = new QListView();
    list_types->setEditTriggers(QAbstractItemView::NoEditTriggers);
    list_types->setModel(model_types);

    list_images = new QListView();
    list_images->setEditTriggers(QAbstractItemView::NoEditTriggers);
    list_images->setModel(model_images);

    box_database = new QVBoxLayout();
    box_database->addWidget(db_list, 1);
    box_database->addWidget(new QLabel("Filtrace:"));
    box_database->addWidget(db_filter);
    box_database->addWidget(db_sort);
    box_database->addWidget(db_viewlist);

    images_moveup       = new QPushButton("^");
    images_movedown     = new QPushButton("v");
    images_rotateleft   = new QPushButton(f->icon_rleft, "");
    images_rotateright  = new QPushButton(f->icon_rright, "");

    box_imagesButtons   = new QHBoxLayout();
    box_imagesButtons->addWidget(images_moveup);
    box_imagesButtons->addWidget(images_movedown);
    box_imagesButtons->addWidget(images_rotateleft);
    box_imagesButtons->addWidget(images_rotateright);

    box_images = new QVBoxLayout();
    box_images->addWidget(list_types, 1);
    box_images->addWidget(list_images, 2);
    box_images->addLayout(box_imagesButtons);

    box_main = new QHBoxLayout();
    box_main->addLayout(box_database);
    box_main->addWidget(view, 1);
    box_main->addLayout(box_images);

    images_moveup->setVisible(false);
    images_movedown->setVisible(false);
    db_viewlist->setVisible(false);

    connect(db_filter,      &QLineEdit::textChanged, this, &Jaguar::filter_edit);
    connect(db_sort,        &QCheckBox::toggled,    this, &Jaguar::db_sort_toggled);
    connect(db_list->selectionModel(),  &QItemSelectionModel::selectionChanged,  this, &Jaguar::db_selectionChanged);
    connect(db_viewlist->selectionModel(),  &QItemSelectionModel::selectionChanged,  this, &Jaguar::view_selectionChanged);
    connect(list_types->selectionModel(),  &QItemSelectionModel::selectionChanged,  this, &Jaguar::types_selectionChanged);

    connect(images_moveup,      &QPushButton::clicked, this, &Jaguar::images_move_clicked);
    connect(images_movedown,    &QPushButton::clicked, this, &Jaguar::images_move_clicked);
    connect(images_rotateleft,  &QPushButton::clicked, this, &Jaguar::images_rotate_clicked);
    connect(images_rotateright, &QPushButton::clicked, this, &Jaguar::images_rotate_clicked);

    setLayout(box_main);

    model_types->appendRow(new QStandardItem("Empty"));
    model_types->appendRow(new QStandardItem("Top"));
    model_types->appendRow(new QStandardItem("Top-Side"));
    model_types->appendRow(new QStandardItem("Top-Bottom"));
    model_types->appendRow(new QStandardItem("Top-Side-Front"));

    clear();
}
Jaguar::~Jaguar()
{
    clear();
}

void Jaguar::clear()
{
    if(layoutView) {
        scene->removeItem(layoutView);
        delete layoutView->layoutItem;
        delete layoutView;
        layoutView = NULL;
    }
    scene->clear();
}
void Jaguar::setView(Database::ObjectView * objectView)
{
    Database::LayoutItem * item = new Database::LayoutItem(objectView);

    item->setRuler(true);

    layoutView = LayoutView::createView(item);
    if(!layoutView)
        return;

    QGraphicsRectItem * rect = new QGraphicsRectItem();
    scene->addItem(rect);
    scene->addItem(layoutView);

    QRectF br   = layoutView->boundingRect();
    rect->setRect(br);
    view->setSceneRect(br);
    view->resetTransform();
    view->fitInView(layoutView, Qt::KeepAspectRatio);
    view->centerOn(layoutView);
}

void Jaguar::filter_edit(const QString & str)
{
    filter->setFilterWildcard(str);
}

void Jaguar::db_sort_toggled(bool checked)
{
    db->view_model.sort(checked ? 0 : 1);
}

void Jaguar::images_move_clicked()
{
    if(layoutView && layoutView->layoutItem->objectView()) {
        if(sender() == images_movedown) {
        }
    }
}

void Jaguar::images_rotate_clicked()
{
    int index;
    int angle = 45;
    if(layoutView && layoutView->layoutItem->objectView() && list_images->selectionModel()->selectedIndexes().size()) {

        if(QGuiApplication::keyboardModifiers() & Qt::ShiftModifier)
            angle = 5;
        if(QGuiApplication::keyboardModifiers() & Qt::ControlModifier)
            angle = 1;

        if(sender() == images_rotateleft)
            angle *= -1;
        index = list_images->selectionModel()->selectedIndexes().at(0).row();
        angle += layoutView->layoutItem->objectView()->rotation(index);
        layoutView->layoutItem->objectView()->setRotation(index, angle);
        layoutView->updateObject();
    }
}

void Jaguar::db_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    QModelIndex objectIndex = filter->mapToSource(db_list->currentIndex());
    QModelIndex viewIndex;
    if(objectIndex.isValid()) {
        QString name = db->object_model.itemFromIndex(objectIndex)->text();
        viewIndex = db->view_model.findItems(name).first()->index();
        db_viewlist->selectionModel()->setCurrentIndex(viewIndex, QItemSelectionModel::ClearAndSelect);
    }
}

void Jaguar::view_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    clear();

    model_images->clear();
    for(int i = 0; i < model_types->rowCount(); i++)
        model_types->item(i)->setEnabled(false);

    objectView = NULL;
    QModelIndex index = db_viewlist->currentIndex();
    if(!index.isValid())
        return;

    objectView = (Database::ObjectView*) db->view_model.itemFromIndex(index);

    switch(objectView->item.images.size()) {
    default:
    case 3:
        model_types->item(4)->setEnabled(true);
    case 2:
        model_types->item(3)->setEnabled(true);
        model_types->item(2)->setEnabled(true);
    case 1:
        model_types->item(1)->setEnabled(true);
    case 0:
        model_types->item(0)->setEnabled(true);
    }

    int itemIndex = 0;
    foreach(Database::ObjectImage* image, objectView->item.images) {
        Q_UNUSED(image);
        QList <QStandardItem*> list;
        QString path = db->getDirItems().filePath(objectView->name() + "_" + QString::number(itemIndex) + ".png");
        QStandardItem * image_item = new QStandardItem(QString::number(itemIndex++));
        QPixmap pic = QPixmap(path).scaledToWidth(list_images->width() - 50);
        image_item->setData(pic, Qt::DecorationRole);
        list.append(image_item);
        model_images->appendRow(list);
    }

    if(objectView->type() == -1)  {
        switch(objectView->item.images.size()) {
            case 0: objectView->setType(0); break;
            case 1: objectView->setType(1); break;
            case 2: objectView->setType(2); break;
            case 3: objectView->setType(4); break;
            default: objectView->setType(0); break;
        }
    }

    list_types->selectionModel()->setCurrentIndex(model_types->index(objectView->type(), 0), QItemSelectionModel::ClearAndSelect);
    setView(objectView);
}

void Jaguar::types_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    QModelIndex index = list_types->currentIndex();
    if(!objectView || !index.isValid())
        return;

    clear();
    objectView->setType(index.row());
    setView(objectView);
}

void JaguarView::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = event->delta() > 0 ? 1.25 : 1/1.25;
    scale(scaleFactor, scaleFactor);
}
