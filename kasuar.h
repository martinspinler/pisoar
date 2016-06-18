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
#include "layout.h"
#include "database.h"

class Kasuar : public QWidget
{
    Q_OBJECT

    Database  * db;

    QHBoxLayout *box_main;
    QVBoxLayout *box_database;
    QVBoxLayout *box_layout;

    QListView   *db_list;
    QPushButton *db_add;
    QPushButton *db_save;
    QCheckBox   *db_sort;

    Layout      *layout;

    QListView   *layout_list;
    QPushButton *layout_add;
    QCheckBox   *layout_border;
    QCheckBox   *layout_ruler;

    Database::LayoutPage * currentLayout;

public:
    Kasuar(Database *database, QWidget *parent = 0);

    void db_add_clicked();
    void db_save_clicked();
    void db_sort_toggled(bool checked);
    void db_activated(const QModelIndex &index);

    void layout_add_clicked();
    void layout_itemSelectionChanged(const QItemSelection &selection);
    void layout_selectionChanged();
    void bakeLayout(Database::LayoutPage* page);

    void layout_border_toggled(bool checked);
    void layout_ruler_toggled(bool checked);
};

#endif // KASUAR_H
