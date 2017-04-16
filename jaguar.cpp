#include <QGuiApplication>
#include <QMessageBox>
#include <QActionGroup>

#include "jaguar.h"
#include "layout.h"
#include "factory.h"
#include "database.h"

Jaguar::Jaguar(QWidget *parent) : QWidget(parent)
{
    m_layoutItem = NULL;
    m_layoutView = NULL;
    m_objectView  = NULL;

    scene       = new QGraphicsScene();
    view        = new JaguarView();
    view->setScene(scene);
    view->setMode(JaguarView::MODE_HAND);

    image_toolbar = new QToolBar();

    db_filter   = new QLineEdit();
    db_sort     = new QCheckBox("&Seřadit podle jména");

    db_filter_model = new QSortFilterProxyModel(this);
    db_filter_model->setSourceModel(&db->object_model);
    db_filter_model->setDynamicSortFilter(true);

    model_images = new QStandardItemModel(this);
    model_types = new QStandardItemModel(this);
    model_views = new QStandardItemModel(this);

    db_list     = new QListView();
    db_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    db_list->setModel(db_filter_model);

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
    images_setScaleFrom = new QPushButton("M");

    box_imagesButtons   = new QHBoxLayout();
    box_imagesButtons->addWidget(images_moveup);
    box_imagesButtons->addWidget(images_movedown);
    box_imagesButtons->addWidget(images_rotateleft);
    box_imagesButtons->addWidget(images_rotateright);
    box_imagesButtons->addWidget(images_setScaleFrom);

    box_images = new QVBoxLayout();
    box_images->addWidget(list_types, 1);
    box_images->addWidget(list_images, 2);
    box_images->addLayout(box_imagesButtons);

    box_image = new QVBoxLayout();
    box_image->addWidget(image_toolbar);
    box_image->addWidget(view, 1);

    box_main = new QHBoxLayout();
    box_main->addLayout(box_database);
    box_main->addLayout(box_image, 1);
    box_main->addLayout(box_images);

    db_viewlist->setVisible(false);

    QAction * a;
    QActionGroup *actionGroup = new QActionGroup(image_toolbar);
    a = image_toolbar->addAction(f->icon_hand,  "Ručička",  this, &Jaguar::onImageSetToolHand);  a->setCheckable(true); a->setActionGroup(actionGroup); a->setChecked(true);
    a = image_toolbar->addAction(f->icon_ruler, "Pravítko", this, &Jaguar::onImageSetToolRuler); a->setCheckable(true); a->setActionGroup(actionGroup);

    connect(db_filter,      &QLineEdit::textChanged, this, &Jaguar::db_filter_edit);
    connect(db_sort,        &QCheckBox::toggled,    this, &Jaguar::db_sort_toggled);
    connect(db_list->selectionModel(),  &QItemSelectionModel::selectionChanged,  this, &Jaguar::db_selectionChanged);
    connect(db_viewlist->selectionModel(),  &QItemSelectionModel::selectionChanged,  this, &Jaguar::view_selectionChanged);
    connect(list_types->selectionModel(),  &QItemSelectionModel::selectionChanged,  this, &Jaguar::types_selectionChanged);

    connect(images_moveup,      &QPushButton::clicked, this, &Jaguar::images_move_clicked);
    connect(images_movedown,    &QPushButton::clicked, this, &Jaguar::images_move_clicked);
    connect(images_rotateleft,  &QPushButton::clicked, this, &Jaguar::images_rotate_clicked);
    connect(images_rotateright, &QPushButton::clicked, this, &Jaguar::images_rotate_clicked);
    connect(images_setScaleFrom,&QPushButton::clicked, this, &Jaguar::images_setScaleFrom_clicked);

    connect(view,      &JaguarView::rulerDone, this, &Jaguar::onRulerDone);

    setLayout(box_main);

    foreach(QString str, viewFactory.getAllViews()) {
        model_types->appendRow(new QStandardItem(str));
    }
}
Jaguar::~Jaguar()
{
    clear();
}

void Jaguar::clear()
{
    scene->clear();
    if(m_layoutItem) {
        m_layoutItem->objectView()->item.unlink(this);
        delete m_layoutItem;
        m_layoutItem = NULL;
    }
}
void Jaguar::setView(ObjectView * objectView)
{
    m_layoutItem = new LayoutItem(objectView);
    m_layoutItem->setRuler(true);
    m_layoutItem->setBorder(true);
    m_layoutItem->objectView()->item.link(this);

    scene->clear();

    m_layoutView = viewFactory.newLayoutView(m_layoutItem);
    if(!m_layoutView) {
        delete m_layoutItem;
        m_layoutItem = NULL;
        return;
    }

    list_types->selectionModel()->select(model_types->index(m_objectView->type(), 0), QItemSelectionModel::ClearAndSelect);

    scene->addItem(m_layoutView);
    updateView();
}
void Jaguar::updateView()
{
    view->resetTransform();
    view->setSceneRect(m_layoutView->boundingRect());
    view->fitInView(m_layoutView, Qt::KeepAspectRatio);
    view->centerOn(m_layoutView);
}

void Jaguar::db_filter_edit(const QString & str)
{
    db_filter_model->setFilterWildcard(str);
}

void Jaguar::db_sort_toggled(bool checked)
{
    db_filter_model->sort(checked ? 0 : 1);
}

void Jaguar::db_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    QModelIndex objectIndex = db_filter_model->mapToSource(db_list->currentIndex());
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
    int i = 0;

    model_images->clear();
    for(i = 0; i < model_types->rowCount(); i++)
        model_types->item(i)->setEnabled(false);

    m_objectView = NULL;
    QModelIndex index = db_viewlist->currentIndex();
    if(!index.isValid())
        return;

    m_objectView = (ObjectView*) db->view_model.itemFromIndex(index);

    i = 0;
    foreach(bool b, viewFactory.getViewsAvailability(&m_objectView->item)) {
        model_types->item(i++)->setEnabled(b);
    }

    i = 0;
    int itemIndex = 0;
    foreach(ObjectImage* image, m_objectView->item.images) {
        Q_UNUSED(image);
        QList <QStandardItem*> list;
        itemIndex = m_objectView->mapping(i);
        QString path = m_objectView->item.imagePath(itemIndex);
        QStandardItem * image_item = new QStandardItem(QString::number(itemIndex) + QString((m_objectView->scaleFrom() == i) ? "M" : ""));
        QPixmap pic = QPixmap(path).scaledToWidth(list_images->width() - 50);
        image_item->setData(pic, Qt::DecorationRole);
        list.append(image_item);
        model_images->appendRow(list);
        i++;
    }

    setView(m_objectView);
}

void Jaguar::types_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    QModelIndex index = list_types->currentIndex();
    if(!m_objectView || !index.isValid())
        return;

    clear();
    m_objectView->setType(index.row());
    setView(m_objectView);
}

void Jaguar::event(ObjectItemChange & event)
{
    Q_UNUSED(event);
    setView(m_layoutItem->objectView());
}

void Jaguar::images_move_clicked()
{
    int index;
    int mapping;
    int move = sender() == images_movedown ? 1 : -1;
    QModelIndexList indexList = list_images->selectionModel()->selectedIndexes();

    if(m_layoutItem && m_layoutItem->objectView() && !indexList.isEmpty()) {
        index = indexList.at(0).row();
        mapping = m_layoutItem->objectView()->mapping(index);
        mapping += move;
        m_layoutItem->objectView()->setMapping(index, mapping);
        m_layoutView->updateObject();
        db_viewlist->selectionModel()->setCurrentIndex(db_viewlist->currentIndex(), QItemSelectionModel::ClearAndSelect);
        db->setModified();
    }
}

void Jaguar::images_rotate_clicked()
{
    int index;
    int angle = sender() == images_rotateleft ? -90 : 90;
    QModelIndexList indexList = list_images->selectionModel()->selectedIndexes();

    if(m_layoutItem && m_layoutItem->objectView() && !indexList.isEmpty()) {
        index = indexList.at(0).row();
        angle += m_layoutItem->objectView()->rotation(index);
        m_layoutItem->objectView()->setRotation(index, angle);
        m_layoutView->updateObject();
        updateView();
        db->setModified();
    }
}
void Jaguar::images_setScaleFrom_clicked()
{
    int index;
    QModelIndexList indexList = list_images->selectionModel()->selectedIndexes();

    if(m_layoutItem && m_layoutItem->objectView() && !indexList.isEmpty()) {
        index = indexList.at(0).row();
        m_layoutItem->objectView()->setScaleFrom(index);
        setView(m_objectView);
    }
}
void Jaguar::onRulerDone(float scale)
{
    QMessageBox mb;
    float ppm = db->set.ppm;
    float sc = m_layoutItem->scale();
    mb.setText(QString("Naměřená velikost: %1cm").arg(scale / ppm / sc / 10));
    mb.exec();
}
