#include <QPainter>
#include <QListView>
#include <QProgressDialog>


#include "kasuar.h"
#include "factory.h"

Kasuar::Kasuar(QWidget *parent)
    : QWidget(parent)
{
/*    filter = new QSortFilterProxyModel();
    filter->setSourceModel(&db->object_model);*/

    db_list = new QListView();
    db_list->setModel(&db->object_model);
    db_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    db_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

    db_add          = new QPushButton(f->icon_forward, "Přid&at na plochu");

    db_sort         = new QCheckBox("Řadit podle jména");

    layout          = new Layout();
    layout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout_toolbar  = new QToolBar();
    layout_toolbar->addAction(f->icon_rect, "Ohraničení", this, &Kasuar::onLayoutRect);
    layout_toolbar->addAction(f->icon_ruler, "Pravítko", this, &Kasuar::onLayoutRuler);
    layout_add      = new QPushButton(f->icon_new, "&Nová stránka");
    layout_save     = new QPushButton(f->icon_save, "Uložit výsledek");

    layout_list     = new QListView();
    layout_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    layout_list->setModel(&db->layout_model);

    box_database = new QVBoxLayout();
    box_database->addWidget(db_list);
    box_database->addWidget(db_add);
    box_database->addWidget(db_sort);

    box_middle = new QVBoxLayout();
    box_middle->addWidget(layout_toolbar);
    box_middle->addWidget(layout);

    box_layout = new QVBoxLayout();
    box_layout->addWidget(layout_list);
    box_layout->addWidget(layout_add);
    box_layout->addWidget(layout_save);

    box_main = new QHBoxLayout();
    box_main->addLayout(box_database);
    //box_main->addWidget(layout, 1);
    box_main->addLayout(box_middle, 1);
    box_main->addLayout(box_layout);

    connect(db_add,         &QPushButton::clicked,  this, &Kasuar::db_add_clicked);
    connect(db_sort,        &QCheckBox::toggled,    this, &Kasuar::db_sort_toggled);
    connect(db_list,        &QListView::activated,  this, &Kasuar::db_activated);

    connect(layout_save,    &QPushButton::clicked,  this, &Kasuar::db_save_clicked);
    connect(layout_add,     &QPushButton::clicked,  this, &Kasuar::layout_add_clicked);
    connect(layout_list->selectionModel(),  &QItemSelectionModel::selectionChanged, this, &Kasuar::layout_itemSelectionChanged);

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
    layout->addNewObject(db->createItem(currentLayout, (Database::ObjectItem*)db->object_model.itemFromIndex(index)));
    db_list->setCurrentIndex(db->object_model.index(index.row() + 1, 0));
}
void Kasuar::db_save_clicked()
{    
    layout->exportToImage(db->getDirLayouts().filePath(currentLayout->text() + QString(".png")));
}
void Kasuar::bakeLayouts()
{
    QProgressDialog progress("Generování výstupů", "Přerušit", 0, db->layout_model.rowCount(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    progress.update();
    for(int i = 0; i < db->layout_model.rowCount(); i++) {
        progress.setValue(i);
        layout_list->setCurrentIndex(db->layout_model.index(i,0));
        layout->exportToImage(db->getDirLayouts().filePath(currentLayout->text() + QString(".png")));

        if (progress.wasCanceled())
            break;
    }
    progress.setValue(db->layout_model.rowCount());
    progress.close();
}
void Kasuar::layout_itemSelectionChanged(const QItemSelection &selection)
{
    Q_UNUSED(selection);
    QModelIndex index = layout_list->currentIndex();
    if(!index.isValid())
        return;

    currentLayout = (Database::LayoutPage*) db->layout_model.itemFromIndex(index);
    layout->loadPage(currentLayout);
}
void Kasuar::layout_add_clicked()
{
    int count = db->layout_model.rowCount();
    currentLayout = db->createLayout(QString::number(count+1));
    layout_list->setCurrentIndex(db->layout_model.index(count, 0));
}
void Kasuar::onLayoutRect()
{
    QList<Database::LayoutItem*> selected = layout->getSelection();
    if(!selected.isEmpty())
        layout->setSelectedBorder(!selected[0]->border);
}
void Kasuar::onLayoutRuler()
{
    QList<Database::LayoutItem*> selected = layout->getSelection();
    if(!selected.isEmpty())
        layout->setSelectedRuler(!selected[0]->ruler);
}
void Kasuar::db_sort_toggled(bool checked)
{
    db->object_model.sort(checked ? 0 : 1);
}
