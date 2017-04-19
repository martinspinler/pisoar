#include <QMessageBox>
#include <QActionGroup>
#include <QDesktopServices>

#include "factory.h"
#include "database.h"
#include "pisoar.h"

Pisoar::Pisoar(QWidget *parent)
    : QWidget(parent)
{
    fl_file     = NULL;
    m_selectedImage = NULL;
    doBatchScale = false;

    db_filter_model = new QSortFilterProxyModel(this);
    db_filter_model->setSourceModel(&db->object_model);
    db_filter_model->setDynamicSortFilter(true);

    fl_none     = new QPushButton   (f->icon_image,    "&Bez označení");
    fl_wip      = new QPushButton   (f->icon_wip,      "&Rozparcováno");
    fl_done     = new QPushButton   (f->icon_done,     "&Hotovo");

    fl_info     = new QLabel        ("--INFO--");
    fl_info->setTextFormat(Qt::RichText);
    connect(fl_info,  &QLabel::linkActivated, this, &Pisoar::fl_info_linkActivated);

    fl_list_model = new QStandardItemModel();
    fl_list     = new QListView();
    fl_list->setModel(fl_list_model);
    fl_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

    db_new      = new QPushButton   (f->icon_new,       "&Nový objekt");
    db_delete   = new QPushButton   (f->icon_delete,    "Odstranit objekt");
    db_assign   = new QPushButton   (f->icon_forward,   "&Přiřadit pohled");
    db_clean    = new QPushButton   (f->icon_clean,     "Odstranit pohledy");
    db_generate = new QPushButton   (f->icon_refresh,   "Přegenerovat pohledy");
    db_info     = new QLabel        ("--INFO--");

    db_info->setTextFormat(Qt::RichText);
    connect(db_info,  &QLabel::linkActivated, this, &Pisoar::db_info_linkActivated);

    db_list     = new QListView();
    db_list->setModel(db_filter_model);
    db_list->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);

    /*db_preview  = new QLabel("<center>Náhled pohledu</center>");
    db_preview->setAlignment(Qt::AlignCenter);*/
    db_preview = new QGraphicsView();
    db_preview->setScene(new QGraphicsScene());
    db_preview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    db_preview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    db_rotate   = new QSlider(Qt::Horizontal);
    db_rotate->setRange(0, 360);

    db_name     = new QLineEdit();
    db_filter   = new QCheckBox("&Filtrovat");
    db_sort     = new QCheckBox("&Seřadit podle jména");

    m_image       = new Image();
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

    box_database->addWidget(db_preview, 1);
    box_database->addWidget(db_rotate);
    box_database->addWidget(db_list, 1);
    box_database->addWidget(db_info);
    box_database->addWidget(db_name);
    box_database->addWidget(db_filter);
    box_database->addWidget(db_sort);

    box_database->addWidget(db_new);
    box_database->addWidget(db_delete);

    box_database->addWidget(db_assign);
    box_database->addWidget(db_clean);
    box_database->addWidget(db_generate);

    box_image->addWidget(image_toolbar);
    box_image->addWidget(m_image, 1);

    box_main->addLayout(box_filelist);
    box_main->addLayout(box_image, 1);
    box_main->addLayout(box_database);

    connect(fl_list->selectionModel(), &QItemSelectionModel::selectionChanged, this, &Pisoar::fl_list_selectionChanged);
    connect(fl_list,    &QListView::activated, this, &Pisoar::fl_list_activated);
    connect(fl_none,    &QPushButton::clicked, this, &Pisoar::fl_setFileFlag_clicked);
    connect(fl_wip,     &QPushButton::clicked, this, &Pisoar::fl_setFileFlag_clicked);
    connect(fl_done,    &QPushButton::clicked, this, &Pisoar::fl_setFileFlag_clicked);

    connect(db_assign,  &QPushButton::clicked, this, &Pisoar::db_assign_clicked);
    connect(db_new,     &QPushButton::clicked, this, &Pisoar::db_new_clicked);
    connect(db_delete,  &QPushButton::clicked, this, &Pisoar::db_remove_clicked);
    connect(db_clean,   &QPushButton::clicked, this, &Pisoar::db_clean_clicked);
    connect(db_generate,&QPushButton::clicked, this, &Pisoar::db_generate_clicked);
    connect(db_name,    &QLineEdit::returnPressed, this, &Pisoar::db_new_clicked);

    connect(db_name,    &QLineEdit::textChanged, this, &Pisoar::db_filter_edit);
    connect(db_filter,  &QCheckBox::toggled, this, &Pisoar::db_filter_toggled);
    connect(db_sort,    &QCheckBox::toggled, this, &Pisoar::db_sort_toggled);

    connect(db_list->selectionModel(), &QItemSelectionModel::selectionChanged, this, &Pisoar::db_list_selectionChanged);

    connect(m_image,      &Image::calibrateDone, this, &Pisoar::onCalibrateDone);
    connect(m_image,      &Image::objectSelected,this, &Pisoar::onObjectSelected);

    connect(db->set.fl_show, &QCheckBox::stateChanged, this, &Pisoar::fl_list_show);
    connect(db_rotate,    &QSlider::valueChanged, this, &Pisoar::db_rotated);

    setLayout(box_main);

    fl_none     ->setEnabled(false);
    fl_wip      ->setEnabled(false);
    fl_done     ->setEnabled(false);

    db_assign   ->setEnabled(false);
    db_delete   ->setEnabled(false);
    db_clean    ->setEnabled(false);
    db_generate ->setEnabled(false);
}
Pisoar::~Pisoar()
{
    if(m_selectedImage && m_selectedImage->objectItem() == NULL)
        delete m_selectedImage;

    QGraphicsScene * prev;
    prev =  db_preview->scene();
    db_preview->setScene(0);
    delete prev;
}
void Pisoar::setCurrentDir(QDir dir)
{
    dir_list = dir;
    fl_list_fill();
}
void Pisoar::batchScale()
{
    ObjectItem* item;

    doBatchScale = !doBatchScale;

    if(doBatchScale) do {
        db_list->setCurrentIndex(db_filter_model->index(db_list->currentIndex().row() + 1, 0));
        item = (ObjectItem*) db->object_model.itemFromIndex(db_filter_model->mapToSource(db_list->currentIndex()));
        if(item && item->images.size() > 0 && item->images.first()->scale() < 1) {
            db_info_linkActivated(item->images[0]->path());
            return;
        }
    }
    while (item != NULL);

    doBatchScale = false;
}

void Pisoar::fl_list_activated(const QModelIndex &index)
{
    QDir dir = dir_list;
    if(dir.cd(fl_list_model->itemFromIndex(index)->text())) {
        setCurrentDir(dir);
    }
    else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(dir.filePath(fl_list_model->itemFromIndex(index)->text())));
    }
}
void Pisoar::fl_list_selectionChanged(const QItemSelection &selection)
{
    Q_UNUSED(selection);
    QDir dir = dir_list;

    if(!selection.indexes().isEmpty() && !dir.cd(fl_list_model->itemFromIndex(selection.indexes().first())->text())) {
        fl_file = db->imageFileByPath(dir.filePath(fl_list_model->itemFromIndex(selection.indexes().first())->text()));
        fl_none->setEnabled(true);
        fl_wip ->setEnabled(true);
        fl_done->setEnabled(true);

        m_image->loadFile(fl_file);
        fl_info_update();
    } else {
        fl_file = 0;
        fl_none->setEnabled(false);
        fl_wip ->setEnabled(false);
        fl_done->setEnabled(false);
        fl_info->setText(QString("--INFO--"));

        m_image->clear();
    }
}
void Pisoar::fl_setFileFlag_clicked()
{
    ImageFile::Flags newFlag;

    if(!fl_file)
        return;

    if      (sender() == fl_wip)
        newFlag =  ImageFile::FLAG_WIP;
    else if(sender() == fl_done)
        newFlag =  ImageFile::FLAG_DONE;
    else
        newFlag = ImageFile::FLAG_NONE;

    ImageFile* file = (ImageFile*) fl_list_model->itemFromIndex(fl_list->currentIndex());
    fl_file->setFlags(newFlag);
    file->setFlags(newFlag);
    fl_list->setCurrentIndex(fl_list_model->index(file->row() + 1, 0));

    if(!db->set.showDoneFiles && newFlag == ImageFile::FLAG_DONE)
        fl_list_model->removeRow(file->row());
}
void Pisoar::fl_list_fill()
{
    QStringList filter;
    QDir dir = dir_list;
    filter << "*.jpg" << "*.png" << "*.tif" << "*.tiff";

    fl_list_model->clear();
    if(dir_list != QDir::current())
        fl_list_model->appendRow(new QStandardItem(f->icon_up, ".."));

    QStringList dirs = dir_list.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(QStringList::const_iterator i = dirs.constBegin(); i != dirs.constEnd(); i++)
        fl_list_model->appendRow(new QStandardItem(f->icon_dir, *i));

    dir.setNameFilters(filter);
    QStringList files = dir.entryList(QDir::Files);
    for(QStringList::const_iterator i = files.constBegin(); i != files.constEnd(); i++) {
        ImageFile * file = db->imageFileByPath(dir.filePath(*i));

        if(file->flags() == ImageFile::FLAG_DONE && !db->set.showDoneFiles)
            continue;
        fl_list_model->appendRow(new ImageFile(*file));
    }
}
void Pisoar::fl_list_show(int state)
{
    Q_UNUSED(state);
    fl_list_fill();
}
void Pisoar::db_new_clicked()
{
    if(db_name->text().isEmpty())
        return;

    try {
        ObjectItem * item = db->createObject(db_name->text());
        db_list->setCurrentIndex(db_filter_model->mapFromSource(item->index()));
    } catch (QString & e) {
        Q_UNUSED(e);
        QMessageBox(QMessageBox::Warning, "Chyba", "Nepodařilo se vytvořit nový objekt.").exec();
    }
}
void Pisoar::db_assign_clicked()
{
    bool added = false;
    QImage mask;
    ObjectItem * item;
    int index = 0;

    if(m_selectedImage->objectItem()) {
        item = m_selectedImage->objectItem();
        foreach(ObjectImage * image, item->images) {
            if(image == m_selectedImage) {
                break;
            }
            index++;
        }
    } else {
        added = true;
        item = (ObjectItem*) db->object_model.itemFromIndex(db_filter_model->mapToSource(db_list->currentIndex()));
        index = item->addImage(m_selectedImage);
    }

    m_selectedImage->setRotation(db_rotate->value() / 1.0f);
    mask = m_selectedImage->imageFileMask(m_originalPixmap);
    m_selectedPixmap = m_selectedImage->pixmapFromMask(mask, m_originalPixmap);
    m_selectedPixmap.save(item->imagePath(index), "PNG");

    if (added) {
        m_image->updateObjects();
        db_info_update();

        if(index >= 2 || db->set.nextItemOnAssignView[index]) {
            db_list->setCurrentIndex(db_list->model()->index(db_list->currentIndex().row() + 1, 0));
        }
    }
    item->update();
}
void Pisoar::db_remove_clicked()
{
    ObjectItem * item = (ObjectItem*)db->object_model.itemFromIndex(db_filter_model->mapToSource(db_list->currentIndex()));
    m_image->updateObjects();
    if(item->isUsed()) {
        QMessageBox msgBox;
        msgBox.setText("Objekt používán");
        msgBox.setInformativeText("Opravdu chcete smazat objekt, který je již použitý na některé ploše?");
        msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Yes:
                break;
            default:
                return;
        }
    }
    item->clean();
    db->removeObject(item);
}
void Pisoar::db_clean_clicked()
{
    ObjectItem* item = ((ObjectItem*)db->object_model.itemFromIndex(db_filter_model->mapToSource(db_list->currentIndex())));
    if(item->isUsed()) {
        QMessageBox msgBox;
        msgBox.setText("Objekt se používá");
        msgBox.setInformativeText("Nejprve odstraňte všechny instance objektu z layoutů.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    } else {
        item->clean();
        m_image->updateObjects();
    }
}
void Pisoar::db_generate_clicked()
{
    ObjectItem * item = (ObjectItem*) db->object_model.itemFromIndex(db_filter_model->mapToSource(db_list->currentIndex()));
    QPixmap pixmap;

    for(int i = 0; i < item->images.size(); i++) {
        pixmap.load(item->images[i]->file()->path());
        QImage mask = item->images[i]->imageFileMask(pixmap);
        item->images[i]->pixmapFromMask(mask, pixmap).save(item->imagePath(i), "PNG");
    }
}
void Pisoar::onObjectSelected(ObjectImage * image)
{
    QImage mask;

    if(m_selectedImage && m_selectedImage->objectItem() == NULL)
        delete m_selectedImage;

    m_selectedImage = image;
    db_rotate->setValue(m_selectedImage->rotation());
    m_originalPixmap = m_image->pixmap();
    mask = m_selectedImage->imageFileMask(m_originalPixmap);
    m_selectedPixmap = m_selectedImage->pixmapFromMask(mask, m_originalPixmap);

    db_preview->scene()->clear();
    db_preview->resetTransform();
    m_previewItem = db_preview->scene()->addPixmap(m_selectedPixmap);
    m_previewItem->setTransformOriginPoint(m_previewItem->boundingRect().width()/2, m_previewItem->boundingRect().height()/2);

    db_preview->fitInView(m_previewItem, Qt::KeepAspectRatio);
    db_preview->centerOn(m_previewItem);

    m_image->updateObjects();
    m_image->addObjectImage(m_selectedImage);
}
void Pisoar::onCalibrateDone(QVariant scale)
{
    QModelIndex index;
    ObjectItem * item;

    if(fl_file)
        fl_file->setScale(scale.toFloat() / db->set.calibLength);
    fl_info_update();
    db_info_update();

    index = db_list->currentIndex();
    if(doBatchScale && index.isValid()) {
        do {
            index = db_list->model()->index(index.row() + 1, 0);
            item = (ObjectItem*) db->object_model.itemFromIndex(db_filter_model->mapToSource(index));
            if(item && item->images.size() > 0 && item->images.first()->scale() < 1) {
                db_list->setCurrentIndex(index);
                db_info_linkActivated(item->images[0]->path());
                break;
            }
        }
        while (item != NULL);
    }
}
void Pisoar::fl_info_update()
{
    QString text;
    if(fl_file != NULL) {
        if(fl_file->scale() < 1)
            text = QString("Bez měřítka");
        else
            text = (QString("Měřítko: ") + QString::number(fl_file->scale(), 'f', 2) + QString(" px/cm"));

        QList<ObjectImage*> images = db->objectImagesByFile(fl_file);
        foreach(ObjectImage* image, images) {
            text += QString("<br><a href=\"%1\">%1</a>").arg(image->objectName());
        }
    }

    fl_info->setText(text);
}

void Pisoar::db_list_selectionChanged(const QItemSelection &selection)
{
    Q_UNUSED(selection);
    bool enabled = db_list->currentIndex().isValid();
    db_assign   ->setEnabled(enabled);
    db_clean    ->setEnabled(enabled);
    db_generate ->setEnabled(enabled);
    db_delete   ->setEnabled(enabled);
    db_info_update();
}

void Pisoar::db_info_update()
{
    ObjectItem * item;
    QModelIndex index = db_list->currentIndex();
    if(!index.isValid()) {
        db_info->setText("--INFO--");
        return;
    }
    item = (ObjectItem*) db->object_model.itemFromIndex(db_filter_model->mapToSource(index));

    QString text = "";
    int viewCount = item->images.size();
    if(viewCount && item->images.first()->scale() < 1)
        text += "BEZ MĚŘÍTKA!<br>";

    text += QString("Pohledy: ") + QString::number(viewCount);
    for(int i = 0; i < viewCount; i++)
        text += QString("<br><a href=\"") + item->images[i]->path() + QString("\">") + item->images[i]->text() + QString("</a>");

    db_info->setText(text);
}
void Pisoar::fl_info_linkActivated(const QString & link)
{
    db_list->setCurrentIndex(db_filter_model->mapFromSource(db->object_model.findItems(link).first()->index()));
}
void Pisoar::db_info_linkActivated(const QString & link)
{
    QFileInfo fi(link);
    if(dir_list != fi.path()) {
        setCurrentDir(fi.path());
    }

    QList <QStandardItem*> list = fl_list_model->findItems(fi.fileName());
    if(!list.isEmpty())
        fl_list->setCurrentIndex(list.first()->index());
}
void Pisoar::db_filter_edit(const QString & str)
{
    if(db_filter->isChecked())
        db_filter_model->setFilterWildcard(str);
}
void Pisoar::db_filter_toggled(bool checked)
{
    if(checked) {
        db_filter_model->setFilterWildcard(db_name->text());
    } else {
        db_filter_model->setFilterWildcard("");
    }
}
void Pisoar::db_sort_toggled(bool checked)
{
    db_filter_model->sort(checked ? 0 : 1);
}
void Pisoar::db_rotated(int value)
{
    if(db_preview->scene()->items().isEmpty())
        return;

    m_previewItem->resetTransform();
    m_previewItem->setRotation(value);
    QRectF br   = m_previewItem->boundingRect();
    db_preview->setSceneRect(br);
    db_preview->resetTransform();
    db_preview->fitInView(m_previewItem, Qt::KeepAspectRatio);
    db_preview->centerOn(m_previewItem);
}
