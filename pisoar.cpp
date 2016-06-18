#include <QMessageBox>

#include "pisoar.h"

Pisoar::Pisoar(Database * database, QWidget *parent)
    : QWidget(parent)
{
    db          = database;
    settings    = new Settings();

    fl_calib    = new QPushButton   ("Nastavit &měřítko (5cm)");
    fl_none     = new QPushButton   (db->icon_image,    "&Bez označení");
    fl_wip      = new QPushButton   (db->icon_wip,      "&Rozparcováno");
    fl_done     = new QPushButton   (db->icon_done,     "&Hotovo");

    fl_info     = new QLabel        ("--INFO--");

    fl_list     = new QListWidget();
    fl_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    db_new      = new QPushButton   ("&Nový objekt");
    db_assign   = new QPushButton   ("&Přiřadit masku");
    db_remove   = new QPushButton   ("Odstranit objekt");
    db_clean    = new QPushButton   ("Odstranit pohledy");
    db_generate = new QPushButton   ("Přegenerovat pohledy");
    db_info     = new QLabel        ("--INFO--");

    db_list     = new QListView();
    db_list->setModel(&db->object_model);
    db_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    db_list->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);

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
    box_filelist->addWidget(fl_calib);
    box_filelist->addWidget(fl_none);
    box_filelist->addWidget(fl_wip);
    box_filelist->addWidget(fl_done);

    box_database->addWidget(db_preview);
    box_database->addWidget(db_list);
    box_database->addWidget(db_info);
    box_database->addWidget(db_name);
    box_database->addWidget(db_new);
    box_database->addWidget(db_assign);
    box_database->addWidget(db_remove);
    box_database->addWidget(db_clean);
    box_database->addWidget(db_generate);

    box_main->addLayout(box_filelist);
    box_main->addWidget(image, 1);
    box_main->addLayout(box_database);

    connect(fl_list->selectionModel(),    &QItemSelectionModel::selectionChanged, this, &Pisoar::fl_list_itemSelectionChanged);
    connect(fl_list,    &QListWidget::itemActivated,      this, &Pisoar::fl_list_itemActivated);
    connect(fl_calib,   &QPushButton::clicked, this, &Pisoar::fl_calib_clicked);
    connect(fl_none,    &QPushButton::clicked, this, &Pisoar::fl_setFileFlag_clicked);
    connect(fl_wip,     &QPushButton::clicked, this, &Pisoar::fl_setFileFlag_clicked);
    connect(fl_done,    &QPushButton::clicked, this, &Pisoar::fl_setFileFlag_clicked);

    connect(db_assign,  &QPushButton::clicked, this, &Pisoar::db_assign_clicked);
    connect(db_new,     &QPushButton::clicked, this, &Pisoar::db_new_clicked);
    connect(db_remove,  &QPushButton::clicked, this, &Pisoar::db_remove_clicked);
    connect(db_clean,   &QPushButton::clicked, this, &Pisoar::db_clean_clicked);
    connect(db_generate,&QPushButton::clicked, this, &Pisoar::db_generate_clicked);
    connect(db_name,    &QLineEdit::returnPressed, this, &Pisoar::db_new_clicked);

    connect(db_list->selectionModel(),    &QItemSelectionModel::selectionChanged, this, &Pisoar::db_list_selectionChanged);
    connect(db_list,    &QListView::activated, this, &Pisoar::db_list_activated);

    connect(image,      &Image::calibrateDone, this, &Pisoar::onCalibrateDone);
    connect(image,      &Image::objectSelected,this, &Pisoar::onObjectSelected);

    setLayout(box_main);

    fl_calib->setEnabled(false);
    fl_none->setEnabled(false);
    fl_wip ->setEnabled(false);
    fl_done->setEnabled(false);
}
Pisoar::~Pisoar()
{
    delete settings;
}

void Pisoar::fl_list_itemActivated()
{
    QList <QListWidgetItem*> items = fl_list->selectedItems();
    if(!items.isEmpty() && dir_list.cd(items.first()->text())) {
        fl_list_fill();
    }
}
void Pisoar::fl_list_itemSelectionChanged(const QItemSelection &selection)
{
    Q_UNUSED(selection);
    QString name;

    if(!fl_list->selectedItems().isEmpty() && (
            name = fl_list->selectedItems().first()->text(),
            QFileInfo(dir_list.filePath(name)).isFile())) {

        fl_filename = name;
        fl_calib->setEnabled(true);
        fl_none->setEnabled(true);
        fl_wip ->setEnabled(true);
        fl_done->setEnabled(true);

        image->loadImage(dir_list.filePath(fl_filename));

        Database::ImageFile * file = db->findFileByName(dir_list.filePath(fl_filename));
        if(file == NULL || file->scale == 0)
            fl_info->setText(QString("Bez měřítka"));
        else
            fl_info->setText(QString("Měřítko: ") + QString::number(file->scale, 'f', 2) + QString(" px/cm"));

        QList<QPair<QString, QPoint> > points = db->getPointsByFile(dir_list.filePath(fl_filename));
        for(int i = 0; i < points.size(); i++)
            image->addPoint(points[i].second, points[i].first);
    } else {
        fl_filename.clear();
        fl_calib->setEnabled(false);
        fl_none->setEnabled(false);
        fl_wip ->setEnabled(false);
        fl_done->setEnabled(false);

        image->clear();
        fl_info->setText(QString("--INFO--"));
    }
}
void Pisoar::fl_setFileFlag_clicked()
{
    int flag = sender() == fl_wip ? 1: (sender() == fl_done ? 2 : 0);

    QList<QListWidgetItem*> list = fl_list->selectedItems();
    if(list.isEmpty())
        return;

    QString path = dir_list.filePath(list.first()->text());
    db->getFileByName(path)->flags = flag;

    list.first()->setIcon(flag == 0 ? db->icon_image : (flag == 1 ? db->icon_wip : db->icon_done));

    int currentRow = fl_list->currentRow();
    if(flag == 2 && !settings->fl_show->isChecked()) {
        delete fl_list->takeItem(currentRow);
        fl_list->setCurrentRow(currentRow);
    }
    else
        fl_list->setCurrentRow(currentRow+1);
}
void Pisoar::fl_show_stateChanged(int state)
{
    Q_UNUSED(state);
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
        Database::ImageFile * file = db->findFileByName(dir.filePath(*i));
        int flags = file ? file->flags : 0;
        if(flags == 2 && !settings->fl_show->isChecked())
            continue;
        fl_list->addItem(new QListWidgetItem(flags == 0 ? db->icon_image: (flags == 1 ? db->icon_wip : db->icon_done), *i));
    }
}
void Pisoar::db_new_clicked()
{
    if(db_name->text().isEmpty())
        return;

    Database::ObjectItem * i = db->createObject(db_name->text());
    if(i)
        db_list->setCurrentIndex(db->object_model.indexFromItem(db->object_model.findItems(i->name).first()));
    else
        QMessageBox::warning(this, "Špatné jméno", "Objekt s tímto jménem nelze vytvořit.");
}
void Pisoar::db_assign_clicked()
{
    assign_mask();
    db_list->setCurrentIndex(db->object_model.index(db_list->currentIndex().row() + 1, 0));
}
void Pisoar::db_remove_clicked()
{
    db->removeObject(db->object_model.itemFromIndex(db_list->currentIndex())->text());
}
void Pisoar::db_clean_clicked()
{
    db->cleanObject(db->object_model.itemFromIndex(db_list->currentIndex())->text());
}
void Pisoar::db_generate_clicked()
{
    QModelIndex index = db_list->currentIndex();
    if(!index.isValid())
        return;
    QString name = db->object_model.itemFromIndex(index)->text();

    Database::ObjectItem * item = db->findObjectByName(name);
    for(int i = 0; i < item->views.size(); i++) {
        image->loadImage(item->views[i]->filename);
        image->assignMask(item->views[i]->pos);

        QString path = db->getDirItems().filePath(name + "_" + QString::number(i) + ".png");
        image->getSelectedObject().save(path, "PNG");
    }
}
void Pisoar::assign_mask()
{
    QModelIndex index = db_list->currentIndex();
    if(!index.isValid())
        return;

    QString name = db->object_model.itemFromIndex(index)->text();
    QPoint pt = image->getSelectedPoint();

    Database::ObjectItem * item = db->findObjectByName(name);
    QString path = db->getDirItems().filePath(name + "_" + QString::number(item->views.size()) + ".png");
    db->createView(item, dir_list.filePath(fl_filename), pt);
    obj_selected.save(path, "PNG");
    image->addPoint(pt, name);
    db_info_update();
}
void Pisoar::fl_calib_clicked()
{
   setCursor(Qt::CrossCursor);
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
    setCursor(Qt::ArrowCursor);
    float s = scale.toFloat() / 5;
    QString path = dir_list.filePath(fl_filename);
    db->getFileByName(path)->scale = s;
    fl_info->setText(QString("Měřítko: ") + QString::number(s, 'f', 2));
}
void Pisoar::db_list_activated(const QModelIndex &index)
{
    Q_UNUSED(index);
    assign_mask();
}

void Pisoar::db_list_selectionChanged(const QItemSelection &selection)
{
    Q_UNUSED(selection);
    db_info_update();
}

void Pisoar::db_info_update()
{
    QModelIndex index = db_list->currentIndex();
    if(!index.isValid()) {
        db_info->setText("--INFO--");
        return;
    }
    QString name = db->object_model.itemFromIndex(index)->text();
    Database::ObjectItem * item = db->findObjectByName(name);
    QString text = "";
    int viewCount = item->views.size();
    if(viewCount) {
        Database::ImageFile *file = db->findFileByName(item->views.first()->filename);
        if(!file || file->scale < 1)
            text += "BEZ MĚŘÍTKA! ";
    }

    text += QString("Pohledy: ") + QString::number(viewCount);
    for(int i = 0; i < viewCount; i++)
        text += QString(i ? ", " : "") + item->views[i]->filename;

    db_info->setText(text);
}
