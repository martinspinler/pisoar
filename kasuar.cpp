#include <QPainter>
#include <QListView>
#include "kasuar.h"

Kasuar::Kasuar(Database *database, QWidget *parent)
    : QWidget(parent)
{
    db = database;

    db_list = new QListView();
    db_list->setModel(&db->object_model);
    db_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    db_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

    db_add          = new QPushButton("Přidat na plochu");
    db_save         = new QPushButton("Uložit výsledek");

    layout          = new Layout(db);

    layout_border   = new QCheckBox("Zobrazit okraj");
    layout_ruler    = new QCheckBox("Zobrazit pravítko");

    layout_prev     = new QPushButton("<<");
    layout_next     = new QPushButton(">>");

    layout_list     = new QListView();
    layout_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    layout_list->setModel(&db->layout_model);


    box_database = new QVBoxLayout();
    box_database->addWidget(db_list);
    box_database->addWidget(db_add);
    box_database->addWidget(db_save);

    box_layout_prevnext = new QHBoxLayout();
    box_layout_prevnext->addWidget(layout_prev);
    box_layout_prevnext->addWidget(layout_next);

    box_layout = new QVBoxLayout();
    box_layout->addWidget(layout_list);
    box_layout->addWidget(layout_border);
    box_layout->addWidget(layout_ruler);
    box_layout->addLayout(box_layout_prevnext);

    box_main = new QHBoxLayout();
    box_main->addLayout(box_database);
    box_main->addWidget(layout, 1);
    box_main->addLayout(box_layout);

    connect(db_add,         &QPushButton::clicked,  this, &Kasuar::db_add_clicked);
    connect(db_save,        &QPushButton::clicked,  this, &Kasuar::db_save_clicked);
    connect(db_list,        &QListView::activated,  this, &Kasuar::db_activated);

    connect(layout_border,  &QCheckBox::toggled,    this, &Kasuar::layout_border_toggled);
    connect(layout_ruler,   &QCheckBox::toggled,    this, &Kasuar::layout_ruler_toggled);

    connect(layout_next,    &QPushButton::clicked,  this, &Kasuar::layout_nav_clicked);
    connect(layout_prev,    &QPushButton::clicked,  this, &Kasuar::layout_nav_clicked);
    connect(layout_list->selectionModel(),  &QItemSelectionModel::selectionChanged, this, &Kasuar::layout_itemSelectionChanged);
    connect(layout,         &Layout::selectionChanged, this, &Kasuar::layout_selectionChanged);

    setLayout(box_main);

    currentLayout = NULL;
}
void Kasuar::db_activated(const QModelIndex &index)
{
    db_add_clicked();
    /*QString name = db->object_model.itemFromIndex(index)->text();
    layout->addNewObject(db->createItem(currentLayout, name));*/
}
void Kasuar::db_add_clicked()
{
    QItemSelectionModel *m = db_list->selectionModel();
    if(!currentLayout || m->selectedIndexes().isEmpty())
        return;
    QString name = db->object_model.itemFromIndex(m->selectedIndexes().first())->text();
    layout->addNewObject(db->createItem(currentLayout, name));
    m->select(db->object_model.index(m->selectedIndexes().first().row() + 1, 0), QItemSelectionModel::ClearAndSelect);
}

void Kasuar::db_save_clicked()
{    
    layout->exportToImage(db->getDirLayouts().filePath(currentLayout->name + QString(".png")));
}
void Kasuar::layout_itemSelectionChanged(const QItemSelection &selection)
{
    if(!selection.indexes().size())
        return;
    currentLayout = db->getLayoutByName(db->layout_model.itemFromIndex(selection.indexes().first())->text());
    layout->loadPage(currentLayout);
}
void Kasuar::layout_nav_clicked()
{
    QModelIndexList ml = layout_list->selectionModel()->selectedIndexes();
    int next = sender() == layout_next ? 1 : -1;

    /* If list is empty, create one layout page */
    if(db->layout_model.rowCount() == 0)
        currentLayout = db->getLayoutByName(QString::number(1));

    /* It's buggy */
    if(ml.isEmpty()) {
        layout_list->selectionModel()->select(db->layout_model.index(0,0), QItemSelectionModel::ClearAndSelect);
        next = 0;
    }
    else {
        next += ml.first().row();
        if(next < 0)
            return;
        layout_list->selectionModel()->select(db->layout_model.index(next, 0), QItemSelectionModel::ClearAndSelect);
    }
    currentLayout = db->getLayoutByName(QString::number(next+1));
    layout->loadPage(currentLayout);
}
void Kasuar::layout_load(Database::LayoutPage *l)
{

}

void Kasuar::layout_selectionChanged()
{
    int ruler = -1;
    int border = -1;
    QList<Database::LayoutItem*> selected = layout->getSelection();
    for(QList<Database::LayoutItem*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        if(ruler == -1 || ruler == (*i)->ruler)
            ruler = (*i)->ruler;
        else
            ruler = 2;
        if(border == -1 || border == (*i)->border)
            border = (*i)->border;
        else
            border = 2;
    }
    switch(ruler) {
        case 0: layout_ruler->setCheckState(Qt::Unchecked); break;
        case 1: layout_ruler->setCheckState(Qt::Checked); break;
        case 2: layout_ruler->setCheckState(Qt::PartiallyChecked); break;
    }
    switch(border) {
        case 0: layout_border->setCheckState(Qt::Unchecked); break;
        case 1: layout_border->setCheckState(Qt::Checked); break;
        case 2: layout_border->setCheckState(Qt::PartiallyChecked); break;
    }
}

void Kasuar::layout_border_toggled(bool checked)
{
    QList<Database::LayoutItem*> selected = layout->getSelection();
    for(QList<Database::LayoutItem*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        (*i)->border = checked;
        layout->recomputeObject(*i);
    }
    layout_border->setTristate(false);
}
void Kasuar::layout_ruler_toggled(bool checked)
{
    QList<Database::LayoutItem*> selected = layout->getSelection();
    for(QList<Database::LayoutItem*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        (*i)->ruler = checked;
        layout->recomputeObject(*i);
    }
    layout_ruler->setTristate(false);
}


