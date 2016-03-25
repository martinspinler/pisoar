#include <QMessageBox>

#include "pisoar.h"

Pisoar::Pisoar(Database * database, QWidget *parent)
    : QWidget(parent)
{
    db          = database;

    fl_none     = new QPushButton   (db->icon_image,    "&Bez označení");
    fl_wip      = new QPushButton   (db->icon_wip,      "&Rozparcováno");
    fl_done     = new QPushButton   (db->icon_done,     "&Hotovo");
    fl_show     = new QCheckBox     ("Zobrazit hotové");
    fl_info     = new QLabel        ("Bez měřítka");

    fl_list     = new QListWidget();
    fl_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    db_new      = new QPushButton   ("&Nový objekt");
    db_save     = new QPushButton   ("Uložit objekt");
    db_assign   = new QPushButton   ("&Přiřadit masku");
    db_rename   = new QPushButton   ("Přejmenovat vybraný");
    db_calib    = new QPushButton   ("Nastavit &měřítko (5cm)");
    db_info     = new QLabel        ("--INFO--");

    db_list     = new QListView();
    db_list->setModel(&db->object_model);
    db_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    db_list->setEditTriggers(QAbstractItemView::EditKeyPressed);
//    db_list->setEditTriggers(QAbstractItemView::);

    db_preview  = new QLabel();
    db_preview->setAlignment(Qt::AlignCenter);
    db_preview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    db_name     = new QLineEdit();

    image       = new Image();

    box_filelist= new QVBoxLayout();
    box_database= new QVBoxLayout();
    box_main    = new QHBoxLayout();

    box_filelist->addWidget(fl_list);
    box_filelist->addWidget(fl_info);
    box_filelist->addWidget(fl_none);
    box_filelist->addWidget(fl_wip);
    box_filelist->addWidget(fl_done);
    box_filelist->addWidget(fl_show);

    box_database->addWidget(db_preview);
    box_database->addWidget(db_list);
    box_database->addWidget(db_info);
    box_database->addWidget(db_name);
    box_database->addWidget(db_new);
    box_database->addWidget(db_rename);
    box_database->addWidget(db_calib);
    box_database->addWidget(db_assign);
    box_database->addWidget(db_save);

    box_main->addLayout(box_filelist);
    box_main->addWidget(image, 1);
    box_main->addLayout(box_database);

    connect(fl_list,    &QListWidget::currentTextChanged, this, &Pisoar::fl_list_itemSelectionChanged);
    connect(fl_list,    &QListWidget::itemActivated,      this, &Pisoar::fl_list_itemActivated);
    connect(fl_none,    &QPushButton::clicked, this, &Pisoar::fl_setFileFlag_clicked);
    connect(fl_wip,     &QPushButton::clicked, this, &Pisoar::fl_setFileFlag_clicked);
    connect(fl_done,    &QPushButton::clicked, this, &Pisoar::fl_setFileFlag_clicked);
    connect(fl_show,    &QCheckBox::clicked,   this, &Pisoar::fl_show_stateChanged);

    connect(db_assign,  &QPushButton::clicked, this, &Pisoar::db_assign_clicked);
    connect(db_save,    &QPushButton::clicked, this, &Pisoar::db_save_clicked);
    connect(db_new,     &QPushButton::clicked, this, &Pisoar::db_new_clicked);
    connect(db_calib,   &QPushButton::clicked, this, &Pisoar::db_calib_clicked);
    connect(db_rename,  &QPushButton::clicked, this, &Pisoar::db_rename_clicked);
    connect(db_name,    &QLineEdit::returnPressed, this, &Pisoar::db_new_clicked);
    connect(db_list->selectionModel(),  &QItemSelectionModel::selectionChanged, this, &Pisoar::db_list_itemSelectionChanged);
    connect(db_list,    &QListView::activated, this, &Pisoar::db_list_activated);//selectionModel(),  &QItemSelectionModel::selectionChanged, this, &Pisoar::db_list_itemSelectionChanged);


    connect(image,    &Image::calibrateDone, this, &Pisoar::onCalibrateDone);
    connect(image,    &Image::objectSelected,this, &Pisoar::onObjectSelected);

    fl_show->setChecked(true);

    setLayout(box_main);
}
Pisoar::~Pisoar()
{
}

void Pisoar::fl_list_itemActivated()
{
    QList <QListWidgetItem*> items = fl_list->selectedItems();
    if(items.isEmpty())
        return;
    if(dir_list.cd(items.first()->text())) {
        fl_list_fill();
    }
}
void Pisoar::fl_list_itemSelectionChanged(const QString & text)
{
    if(text.isEmpty())
        return;
    if(QFileInfo(dir_list.filePath(text)).isFile()) {
        fl_filename = text;
        image->loadImage(dir_list.filePath(fl_filename));

        Database::ImageFile * file = db->findFileByName(dir_list.filePath(fl_filename));
        if(file == NULL || file->scale == 0)
            fl_info->setText(QString("Bez měřítka"));
        else
            fl_info->setText(QString("Měřítko: ") + QString::number(file->scale, 'f', 2));
    }
    else
        fl_filename.clear();
}
void Pisoar::fl_setFileFlag_clicked()
{
    int flag = sender() == fl_wip ? 1: (sender() == fl_done ? 2 : 0);

    QList <QListWidgetItem*> items = fl_list->selectedItems();
    if(items.isEmpty())
        return;
    QString name = items.first()->text();
    QString path = dir_list.filePath(name);
    db->getFileByName(path)->flags = flag;

    items.first()->setIcon(flag == 0 ? db->icon_image : (flag == 1 ? db->icon_wip : db->icon_done));
    if(flag == 2 && !fl_show->isChecked())
        delete fl_list->takeItem(fl_list->currentRow());
    else
        fl_list->setCurrentRow(fl_list->currentRow()+1);
}
void Pisoar::fl_show_stateChanged(int state)
{
    fl_list_fill();
}
void Pisoar::setCurrentDir(QDir dir)
{
    dir_list = dir;
    fl_list_fill();
}
void Pisoar::fl_list_fill()
{
    QStringList filter;
    filter << "*.JPG" << "*.jpg" << "*.PNG" << "*.png";

    fl_list->clear();

    if(dir_list != QDir::current()) {
        fl_list->addItem(new QListWidgetItem(db->icon_up, ".."));
    }

    QStringList dirs = dir_list.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(QStringList::const_iterator i = dirs.constBegin(); i != dirs.constEnd(); i++) {
        fl_list->addItem(new QListWidgetItem(db->icon_folder, *i));
    }

    QDir dir = dir_list;
    dir.setNameFilters(filter);
    QStringList files = dir.entryList(QDir::Files);
    for(QStringList::const_iterator i = files.constBegin(); i != files.constEnd(); i++) {
        int flags = 0;
        Database::ImageFile * file = db->findFileByName(dir_list.filePath(*i));
        if(file)
            flags = file->flags;

        if(flags == 2 && !fl_show->isChecked())
            continue;
        fl_list->addItem(new QListWidgetItem(flags == 0 ? db->icon_image: (flags == 1 ? db->icon_wip : db->icon_done), *i));
    }
}
void Pisoar::db_save_clicked()
{
    //db->save();
}

void Pisoar::db_new_clicked()
{
    QString text = db_name->text();
    if(!text.isEmpty() && db->findObjectByName(text) == NULL) {
        Database::ObjectItem * i = db->createObject(text);
        db_list->selectionModel()->select(db->object_model.indexFromItem(i->it), QItemSelectionModel::ClearAndSelect);
    }
    else {
        QMessageBox mb(QMessageBox::Warning, "Špatné jméno", "Objekt s tímto jménem nelze vytvořit.");
        mb.exec();
    }
}
void Pisoar::db_rename_clicked(){
    QString text = db_name->text();
    if(!text.isEmpty() && db->findObjectByName(text) == NULL) {
        QModelIndexList i = db_list->selectionModel()->selectedIndexes();
        if(i.isEmpty())
            return;

        QString name = db->object_model.item(i.first().row())->text();
        Database::ObjectItem * item = db->findObjectByName(name);

        if(item->views.size()) {
            QMessageBox mb(QMessageBox::Warning, "Špatné jméno", "Objekt už má přiřazeny pohledy.");
            mb.exec();
        }else
        {
            db->object_model.item(i.first().row())->setText(text);
            item->name = text;
        }
    }
    else {
        QMessageBox mb(QMessageBox::Warning, "Špatné jméno", "Objekt nemůže mít takové jméno.");
        mb.exec();
    }
}

void Pisoar::db_assign_clicked()
{
    assign_mask();
    db_list->selectionModel()->select(db->object_model.index(db_list->selectionModel()->selectedRows().first().row() + 1, 0), QItemSelectionModel::ClearAndSelect);
}
void Pisoar::assign_mask()
{
    QModelIndexList i = db_list->selectionModel()->selectedIndexes();
    if(i.isEmpty())
        return;

    QString name = db->object_model.item(i.first().row())->text();
    QPoint pt = image->getSelectedPoint();

    Database::ObjectItem * item = db->findObjectByName(name);
    QString path = db->getDirItems().filePath(name + "_" + QString::number(item->views.size()) +".png");
    db->createView(item, dir_list.filePath(fl_filename), pt);
    obj_selected.save(path, "PNG");
    db_info_update();
}
void Pisoar::db_calib_clicked()
{
   image->calibrate();
}
void Pisoar::onObjectSelected()
{
    int w = db_preview->width();
    obj_selected = image->getSelectedObject();
    if(obj_selected.height() > height()/2)
        db_preview->setPixmap(obj_selected.scaled(w, height()/2, Qt::KeepAspectRatio));
    else
        db_preview->setPixmap(obj_selected.scaledToWidth(w));
}
void Pisoar::onCalibrateDone(QVariant scale)
{
    float s = scale.toFloat() / 5;
    QString path = dir_list.filePath(fl_filename);
    db->getFileByName(path)->scale = s;
    fl_info->setText(QString("Měřítko: ") + QString::number(s, 'f', 2));
}
void Pisoar::db_list_activated(const QModelIndex &index)
{
    assign_mask();
}

void Pisoar::db_list_itemSelectionChanged(const QItemSelection &selection)
{
    db_info_update();
}
void Pisoar::db_info_update()
{
    if(!db_list->selectionModel()->selectedIndexes().size()) {
        db_info->setText("--INFO--");
        return;
    }
    QStandardItem * i = db->object_model.itemFromIndex(db_list->selectionModel()->selectedIndexes().first());
    Database::ObjectItem * item = db->findObjectByName(i->text());
    QString text = QString("Pohledy: ") + QString::number(item->views.size());
    for(int j = 0; j < item->views.size(); j++)
        text += QString(", ") + item->views[j]->filename;

    db_info->setText(text);
}

