#include "jaguar.h"
#include "layout.h"
#include "factory.h"
#include "database.h"

Jaguar::Jaguar(QWidget *parent) : QWidget(parent)
{
    layoutView  = NULL;

    scene       = new QGraphicsScene();
    view        = new JaguarView();
    view->setScene(scene);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    db_filter   = new QLineEdit();

    filter      = new QSortFilterProxyModel(this);
    filter->setSourceModel(&db->object_model);
    filter->setDynamicSortFilter(true);

    db_list     = new QListView();
    db_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    db_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    db_list->setModel(filter);

    box_database = new QVBoxLayout();
    box_database->addWidget(db_list);
    box_database->addWidget(new QLabel("Filtrace:"));
    box_database->addWidget(db_filter);

    box_main = new QHBoxLayout();
    box_main->addLayout(box_database);
    box_main->addWidget(view, 1);

    connect(db_filter,      &QLineEdit::textChanged, this, &Jaguar::filter_edit);
    connect(db_list->selectionModel(),  &QItemSelectionModel::selectionChanged,  this, &Jaguar::db_selectionChanged);
    setLayout(box_main);
}
Jaguar::~Jaguar()
{
    if(layoutView) {
        scene->removeItem(layoutView);
        delete layoutView->layoutItem;
        delete layoutView;
    }
}

void Jaguar::filter_edit(const QString & str)
{
    QRegExp re;
    re.setPattern(str);
    filter->setFilterRegExp(re);
}

void Jaguar::db_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    if(layoutView) {
        scene->removeItem(layoutView);
        delete layoutView->layoutItem;
        delete layoutView;
    }
    scene->clear();

    QModelIndex index = db_list->currentIndex();
    if(!index.isValid())
        return;

    Database::ObjectItem * objectItem = (Database::ObjectItem*) db->object_model.itemFromIndex(filter->mapToSource(index));
    Database::LayoutItem * item = new Database::LayoutItem(objectItem);
    item->ruler = true;
    switch(objectItem->images.size()) {
        case 0: return;
        case 1: layoutView = new LayoutTopView(item); break;
        case 2: (item->type == 1 ? layoutView = new LayoutTopBottomView(item) : layoutView = new LayoutTopSideView(item)); break;
        default: layoutView = new LayoutTopSideView(item); break;
    }
    QGraphicsRectItem * r = new QGraphicsRectItem();
    scene->addItem(r);
    scene->addItem(layoutView);
    /*QRectF cr   = layoutView->childrenBoundingRect();
    QRectF br   = layoutView->boundingRect();*/
    QRectF sr   = layoutView->sceneBoundingRect();
    r->setRect(sr);

    //view->resetTransform();
    //view->fitInView(layoutView);
    view->centerOn(layoutView);
}

void JaguarView::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = 1.25;
    if(event->delta() > 0)
        scale(scaleFactor, scaleFactor);
    else
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
}
