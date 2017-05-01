#ifndef KASUAR_H
#define KASUAR_H

#include <QtCore>
#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QCheckBox>
#include <QSortFilterProxyModel>
#include <QToolBar>

#include "layout.h"
#include "database.h"

class Kasuar : public QWidget
{
    Q_OBJECT

    QHBoxLayout *box_main;
    QVBoxLayout *box_database;
    QVBoxLayout *box_layout;
    QVBoxLayout *box_middle;

    QListView   *db_list;
    QPushButton *db_add;

    QCheckBox   *db_sort;

    /* Layout - main page */
    Layout      *layout;
    QToolBar    *layout_toolbar;

    QListView   *layout_list;
    QPushButton *layout_add;
    QPushButton *layout_save;

    LayoutPage * currentLayout;
    QSortFilterProxyModel * filter;

public:
    Kasuar(QWidget *parent = 0);

    void db_add_clicked();
    void db_save_clicked();
    void db_sort_toggled(bool checked);
    void db_activated(const QModelIndex &index);

    void layout_add_clicked();
    void layout_itemSelectionChanged(const QItemSelection &selection);
    void bakeLayouts();

    void onLayoutRect();
    void onLayoutRuler();
    void onSceneSelectionChanged();
};

#endif // KASUAR_H
