#include <QMessageBox>
#include <QActionGroup>

#include "factory.h"
#include "database.h"
#include "pisoar.h"

Pisoar::Pisoar(QWidget *parent)
    : QWidget(parent)
{
    fl_file     = NULL;

    fl_none     = new QPushButton   (f->icon_image,    "&Bez označení");
    fl_wip      = new QPushButton   (f->icon_wip,      "&Rozparcováno");
    fl_done     = new QPushButton   (f->icon_done,     "&Hotovo");

    fl_info     = new QLabel        ("--INFO--");
    fl_info->setOpenExternalLinks(false);
    fl_info->setTextFormat(Qt::RichText);
    connect(fl_info,  &QLabel::linkActivated, this, &Pisoar::fl_info_linkActivated);

    fl_list_model = new QStandardItemModel();
    fl_list     = new QListView();
    fl_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    fl_list->setModel(fl_list_model);
    fl_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

    db_new      = new QPushButton   ("&Nový objekt");
    db_remove   = new QPushButton   ("Odstranit objekt");
    db_assign   = new QPushButton   ("&Přiřadit pohled");
    db_clean    = new QPushButton   ("Odstranit pohledy");
    db_generate = new QPushButton   ("Přegenerovat pohledy");
    db_info     = new QLabel        ("--INFO--");

    db_new      ->setIcon(f->icon_new);
    db_remove   ->setIcon(f->icon_delete);
    db_assign   ->setIcon(f->icon_forward);
    db_clean    ->setIcon(f->icon_clean);
    db_generate ->setIcon(f->icon_refresh);

    db_info->setOpenExternalLinks(false);
    db_info->setTextFormat(Qt::RichText);
    connect(db_info,  &QLabel::linkActivated, this, &Pisoar::db_info_linkActivated);

    db_list     = new QListView();
    db_list->setModel(&db->object_model);
    db_list->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    db_list->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);

    db_preview  = new QLabel("<center>Náhled pohledu</center>");
    db_preview->setAlignment(Qt::AlignCenter);
    db_preview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    db_name     = new QLineEdit();

    image       = new Image();
    image->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    image->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    image->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    image_toolbar = new QToolBar();

    QAction * a;
    QActionGroup *actionGroup = new QActionGroup(image_toolbar);
    a = image_toolbar->addAction(f->icon_hand, "Ručička",  this, &Pisoar::onImageSetToolHand);  a->setCheckable(true); a->setActionGroup(actionGroup); a->setChecked(true);
    a = image_toolbar->addAction(f->icon_wand, "Maska",    this, &Pisoar::onImageSetToolMask);  a->setCheckable(true); a->setActionGroup(actionGroup);
    a = image_toolbar->addAction(f->icon_rect, "Čtverec",  this, &Pisoar::onImageSetToolRect);  a->setCheckable(true); a->setActionGroup(actionGroup);
    a = image_toolbar->addAction(f->icon_ruler, "Měřítko", this, &Pisoar::onImageSetToolScale); a->setCheckable(true); a->setActionGroup(actionGroup);
    //a = image_toolbar->addAction(f->icon_color, "Barva",   this, &Pisoar::onImageSetToolColor); a->setCheckable(true); a->setActionGroup(actionGroup);

    box_image   = new QVBoxLayout();
    box_filelist= new QVBoxLayout();
    box_database= new QVBoxLayout();
    box_main    = new QHBoxLayout();

    box_filelist->addWidget(fl_list);
    box_filelist->addWidget(fl_info);
    box_filelist->addWidget(fl_none);
    box_filelist->addWidget(fl_wip);
    box_filelist->addWidget(fl_done);

    box_database->addWidget(db_preview);
    box_database->addWidget(db_list);
    box_database->addWidget(db_info);
    box_database->addWidget(db_name);

    box_database->addWidget(db_new);
    box_database->addWidget(db_remove);

    box_database->addWidget(db_assign);
    box_database->addWidget(db_clean);
    box_database->addWidget(db_generate);

    box_image->addWidget(image_toolbar);
    box_image->addWidget(image);

    box_main->addLayout(box_filelist);
    box_main->addLayout(box_image, 1);
    box_main->addLayout(box_database);

    connect(fl_list->selectionModel(),    &QItemSelectionModel::selectionChanged, this, &Pisoar::fl_list_selectionChanged);
    connect(fl_list,    &QListView::activated, this, &Pisoar::fl_list_activated);
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

    connect(image,      &Image::calibrateDone, this, &Pisoar::onCalibrateDone);
    connect(image,      &Image::objectSelected,this, &Pisoar::onObjectSelected);

    setLayout(box_main);

    fl_none     ->setEnabled(false);
    fl_wip      ->setEnabled(false);
    fl_done     ->setEnabled(false);

    db_assign   ->setEnabled(false);
    db_remove   ->setEnabled(false);
    db_clean    ->setEnabled(false);
    db_generate ->setEnabled(false);
}
Pisoar::~Pisoar()
{
}
void Pisoar::setCurrentDir(QDir dir)
{
    fl_list_model->clear();
    dir_list = dir;
    fl_list_fill();
}
void Pisoar::fl_list_activated(const QModelIndex &index)
{
    QDir dir = dir_list;
    if(dir.cd(fl_list_model->itemFromIndex(index)->text()))
        setCurrentDir(dir);
}
void Pisoar::fl_list_selectionChanged(const QItemSelection &selection)
{
    Q_UNUSED(selection);
    QDir dir = dir_list;

    if(!selection.indexes().isEmpty() && !dir.cd(fl_list_model->itemFromIndex(selection.indexes().first())->text())) {
        fl_file = db->getFileByName(dir.filePath(fl_list_model->itemFromIndex(selection.indexes().first())->text()));
        fl_none->setEnabled(true);
        fl_wip ->setEnabled(true);
        fl_done->setEnabled(true);

        image->loadImage(fl_file->getPath());
        fl_info_update();

        QList<QPair<Database::ObjectItem*, QVariant> > objects = db->getObjectsByFile(fl_file);
        for(int i = 0; i < objects.size(); i++)
            image->addObject(objects[i].second, objects[i].first->text());
    } else {
        fl_file = 0;
        fl_none->setEnabled(false);
        fl_wip ->setEnabled(false);
        fl_done->setEnabled(false);
        fl_info->setText(QString("--INFO--"));

        image->clear();
    }
}
void Pisoar::fl_setFileFlag_clicked()
{
    if(!fl_file)
        return;

    Database::ImageFile::Flags newFlag = Database::ImageFile::FLAG_NONE;
    if      (sender() == fl_wip)
        newFlag =  Database::ImageFile::FLAG_WIP;
    else if(sender() == fl_done)
        newFlag =  Database::ImageFile::FLAG_DONE;

    Database::ImageFile* file = (Database::ImageFile*) fl_list_model->itemFromIndex(fl_list->currentIndex());
    fl_file->setFlags(newFlag);
    file->setFlags(newFlag);
    fl_list->setCurrentIndex(fl_list_model->index(file->row() + 1, 0));
}
void Pisoar::fl_show_stateChanged(int state)
{
    Q_UNUSED(state);
    fl_list_fill();
}
void Pisoar::fl_list_fill()
{
    QStringList filter;
    QDir dir = dir_list;
    filter << "*.jpg" << "*.png" << "*.tif" << "*.tiff";

    if(dir_list != QDir::current())
        fl_list_model->appendRow(new QStandardItem(f->icon_up, ".."));

    QStringList dirs = dir_list.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(QStringList::const_iterator i = dirs.constBegin(); i != dirs.constEnd(); i++)
        fl_list_model->appendRow(new QStandardItem(f->icon_dir, *i));

    dir.setNameFilters(filter);
    QStringList files = dir.entryList(QDir::Files);
    for(QStringList::const_iterator i = files.constBegin(); i != files.constEnd(); i++) {
        Database::ImageFile * file = db->getFileByName(dir.filePath(*i));

        if(file->getFlags() == Database::ImageFile::FLAG_DONE && !db->set.showDoneFiles)
            continue;
        fl_list_model->appendRow(new Database::ImageFile(*file));
    }
}

void Pisoar::db_new_clicked()
{
    if(db_name->text().isEmpty())
        return;

    Database::ObjectItem * item = db->createObject(db_name->text());
    if(item)
        db_list->setCurrentIndex(db->object_model.indexFromItem(item));
}
void Pisoar::db_assign_clicked()
{
    Database::ObjectItem * item = (Database::ObjectItem*) db->object_model.itemFromIndex(db_list->currentIndex());
    QString name = item->text();
    QString path = db->getDirItems().filePath(name + "_" + QString::number(item->images.size()) + ".png");
    obj_selected.save(path, "PNG");
    item->createImage(mask_file, selectedView);
    image->addObject(selectedView, name);
    db_info_update();

    if(item->images.size() >= 3 || db->set.nextItemOnAssignView[item->images.size()-1])
        db_list->setCurrentIndex(db->object_model.index(db_list->currentIndex().row() + 1, 0));
}
void Pisoar::db_remove_clicked()
{
    db->removeObject((Database::ObjectItem*)db->object_model.itemFromIndex(db_list->currentIndex()));
}
void Pisoar::db_clean_clicked()
{
    db->cleanObject((Database::ObjectItem*)db->object_model.itemFromIndex(db_list->currentIndex()));
}
void Pisoar::db_generate_clicked()
{
    Database::ObjectItem * item = (Database::ObjectItem*) db->object_model.itemFromIndex(db_list->currentIndex());
    QString name = item->text();

    for(int i = 0; i < item->images.size(); i++) {
        QString path = db->getDirItems().filePath(name + "_" + QString::number(i) + ".png");
        image->loadImage(item->images[i]->file->getPath());
        QImage mask = image->objectMask(item->images[i]->object);
        image->pixmapFromMask(mask).save(path, "PNG");
    }
}
void Pisoar::onObjectSelected(QVariant obj)
{
    QImage mask;
    selectedView = obj;
    mask = image->objectMask(selectedView);
    obj_selected = image->pixmapFromMask(mask);
    mask_file = fl_file;

    if(obj_selected.height() > db_list->height())
        db_preview->setPixmap(obj_selected.scaled(db_list->width(), db_list->height(), Qt::KeepAspectRatio));
    else
        db_preview->setPixmap(obj_selected.scaledToWidth(db_list->width()));
}
void Pisoar::onCalibrateDone(QVariant scale)
{
    fl_file->setScale(scale.toFloat() / db->set.calibLength);
    fl_info_update();
    db_info_update();
}
void Pisoar::fl_info_update()
{
    QString text;
    if(fl_file->getScale() < 1)
        text = QString("Bez měřítka");
    else
        text = (QString("Měřítko: ") + QString::number(fl_file->getScale(), 'f', 2) + QString(" px/cm"));

    QList<QPair<Database::ObjectItem*, QVariant> > objects = db->getObjectsByFile(fl_file);
    for(int i = 0; i < objects.size(); i++)
        text += QString("<br><a href=\"") + objects[i].first->text() + QString("\">") + objects[i].first->text() + QString("</a>");

    fl_info->setText(text);
}

void Pisoar::db_list_selectionChanged(const QItemSelection &selection)
{
    Q_UNUSED(selection);
    bool enabled = db_list->currentIndex().isValid();
    db_assign   ->setEnabled(enabled);
    db_clean    ->setEnabled(enabled);
    db_generate ->setEnabled(enabled);
    db_remove   ->setEnabled(enabled);
    db_info_update();
}

void Pisoar::db_info_update()
{
    Database::ObjectItem * item;
    QModelIndex index = db_list->currentIndex();
    if(!index.isValid()) {
        db_info->setText("--INFO--");
        return;
    }
    item = (Database::ObjectItem*) db->object_model.itemFromIndex(index);

    QString text = "";
    int viewCount = item->images.size();
    if(viewCount && item->images.first()->file->getScale() < 1)
        text += "BEZ MĚŘÍTKA!<br>";

    text += QString("Pohledy: ") + QString::number(viewCount);
    for(int i = 0; i < viewCount; i++)
        text += QString("<br><a href=\"") + item->images[i]->file->getPath() + QString("\">") + item->images[i]->file->text() + QString("</a>");

    db_info->setText(text);
}
void Pisoar::fl_info_linkActivated(const QString & link)
{
    db_list->setCurrentIndex(db->object_model.findItems(link).first()->index());
}
void Pisoar::db_info_linkActivated(const QString & link)
{
    QFileInfo fi(link);
    if(dir_list != fi.path())
        setCurrentDir(fi.path());
    QList <QStandardItem*> list = fl_list_model->findItems(fi.fileName());
    if(!list.isEmpty())
        fl_list->setCurrentIndex(list.first()->index());
}

