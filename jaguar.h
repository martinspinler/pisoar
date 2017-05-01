#ifndef JAGUAR_H
#define JAGUAR_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QGraphicsRectItem>
#include <QLabel>
#include <QItemSelection>
#include <QPushButton>
#include <QToolBar>

#include "layoutviewfactory.h"
#include "layoutview.h"
#include "jaguarview.h"

using namespace LayoutViews;

class Jaguar : public QWidget, public EventListener<ObjectItemChange>
{
    Q_OBJECT

    friend class MainWindow;

    QSortFilterProxyModel * db_filter_model;
    QStandardItemModel * model_images;
    QStandardItemModel * model_types;
    QStandardItemModel * model_views;
    JaguarView  *view;
    LayoutViewFactory viewFactory;
    QGraphicsScene*scene;
    QToolBar    *image_toolbar;

    QListView   *db_list;
    QListView   *db_viewlist;
    QLineEdit   *db_filter;
    QLabel      *db_imagePreview;
    QCheckBox   *db_sort;

    QListView   *list_types;
    QListView   *list_images;
    QPushButton *images_moveup;
    QPushButton *images_movedown;
    QPushButton *images_rotateleft;
    QPushButton *images_rotateright;
    QPushButton *images_setScaleFrom;

    QHBoxLayout *box_main;
    QVBoxLayout *box_database;
    QVBoxLayout *box_image;
    QVBoxLayout *box_images;
    QHBoxLayout *box_imagesButtons;

    LayoutView  *m_layoutView;
    LayoutItem  *m_layoutItem;
    ObjectView  *m_objectView;

public:
    explicit Jaguar(QWidget *parent = 0);
    ~Jaguar();

    void clear();
    void setView(ObjectView * objectView);
    void updateView();

private:
    void db_filter_edit(const QString &str);
    void db_sort_toggled(bool checked);
    void db_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void view_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void types_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void images_move_clicked();
    void images_rotate_clicked();
    void images_setScaleFrom_clicked();

    void event(ObjectItemChange & event);

    void onImageSetToolHand()  {view->setMode(JaguarView::MODE_HAND);}
    void onImageSetToolRuler() {view->setMode(JaguarView::MODE_RULER);}

    void onSceneSelectionChanged();

private:
    void onRulerDone(float scale);
};

#endif // JAGUAR_H
