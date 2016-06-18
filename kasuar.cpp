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
    db_sort         = new QCheckBox("Řadit podle jména");

    layout          = new Layout(db);

    layout_border   = new QCheckBox("Zobrazit okraj");

    layout_ruler    = new QCheckBox("Zobrazit pravítko");
    layout_add      = new QPushButton("+");

    layout_list     = new QListView();
    layout_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    layout_list->setModel(&db->layout_model);

    box_database = new QVBoxLayout();
    box_database->addWidget(db_list);
    box_database->addWidget(db_add);
    box_database->addWidget(db_save);
    box_database->addWidget(db_sort);

    box_layout = new QVBoxLayout();
    box_layout->addWidget(layout_list);
    box_layout->addWidget(layout_border);
    box_layout->addWidget(layout_ruler);
    box_layout->addWidget(layout_add);

    box_main = new QHBoxLayout();
    box_main->addLayout(box_database);
    box_main->addWidget(layout, 1);
    box_main->addLayout(box_layout);

    connect(db_add,         &QPushButton::clicked,  this, &Kasuar::db_add_clicked);
    connect(db_save,        &QPushButton::clicked,  this, &Kasuar::db_save_clicked);
    connect(db_sort,        &QCheckBox::toggled,    this, &Kasuar::db_sort_toggled);
    connect(db_list,        &QListView::activated,  this, &Kasuar::db_activated);

    connect(layout_border,  &QCheckBox::toggled,    this, &Kasuar::layout_border_toggled);
    connect(layout_ruler,   &QCheckBox::toggled,    this, &Kasuar::layout_ruler_toggled);

    connect(layout_add,     &QPushButton::clicked,  this, &Kasuar::layout_add_clicked);
    connect(layout_list->selectionModel(),  &QItemSelectionModel::selectionChanged, this, &Kasuar::layout_itemSelectionChanged);
    connect(layout,         &Layout::selectionChanged, this, &Kasuar::layout_selectionChanged);

    setLayout(box_main);

    currentLayout = NULL;
}
void Kasuar::db_activated(const QModelIndex &index)
{
    Q_UNUSED(index);
    db_add_clicked();
}
void Kasuar::db_add_clicked()
{
    QModelIndex index = db_list->currentIndex();
    if(!currentLayout || !index.isValid())
        return;
    QString name = db->object_model.itemFromIndex(index)->text();
    layout->addNewObject(db->createItem(currentLayout, name));
    db_list->setCurrentIndex(db->object_model.index(index.row() + 1, 0));
}
void Kasuar::db_save_clicked()
{    
    layout->exportToImage(db->getDirLayouts().filePath(currentLayout->name + QString(".png")));
}
void Kasuar::bakeLayouts()
{
    for(int i = 0; i < db->layout_model.rowCount(); i++) {
        layout_list->setCurrentIndex(db->layout_model.index(i,0));
        layout->exportToImage(db->getDirLayouts().filePath(currentLayout->name + QString(".png")));
    }
}
void Kasuar::layout_itemSelectionChanged(const QItemSelection &selection)
{
    Q_UNUSED(selection);
    QModelIndex index = layout_list->currentIndex();
    if(!index.isValid())
        return;

    currentLayout = db->getLayoutByName(db->layout_model.itemFromIndex(index)->text());
    layout->loadPage(currentLayout);
}
void Kasuar::layout_add_clicked()
{
    int count = db->layout_model.rowCount();
    currentLayout = db->createLayout(QString::number(count+1));
    layout_list->setCurrentIndex(db->layout_model.index(count, 0));
}
void Kasuar::layout_selectionChanged()
{
    int ruler = -1;
    int border = -1;
    QList<Database::LayoutItem*> selected = layout->getSelection();
    for(QList<Database::LayoutItem*>::const_iterator i = selected.constBegin(); i != selected.constEnd(); i++) {
        if(ruler == -1)
            ruler = (*i)->ruler;
        if(ruler != (*i)->ruler)
            ruler = 2;

        if(border == -1)
            border = (*i)->border;
        if(border != (*i)->border)
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
    layout->setSelectedBorder(checked);
}
void Kasuar::layout_ruler_toggled(bool checked)
{
    layout->setSelectedRuler(checked);
}
void Kasuar::db_sort_toggled(bool checked)
{
    db->object_model.sort(checked ? 0 : 1);
}
