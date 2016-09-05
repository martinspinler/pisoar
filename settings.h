#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QGridLayout>

class Settings : public QDialog
{
    Q_OBJECT

    QLineEdit   *db_align;
    QLineEdit   *db_pagewidth;
    QLineEdit   *db_pageheight;

    QLineEdit   *db_pageWidth;
    QLineEdit   *db_pageHeight;
    QLineEdit   *db_edgeTop;
    QLineEdit   *db_edgeBottom;
    QLineEdit   *db_edgeSides;

    QLineEdit   *db_resolution;
    QLineEdit   *db_calibSize;

    QLineEdit   *db_layoutText;

    QVBoxLayout *vbox;
    QGridLayout *layout;

    QCheckBox   *fl_show;
    QCheckBox   *db_next[3];

   void closeEvent(QCloseEvent * event);

public:
    explicit Settings(QWidget *parent = 0);
    void fromJsonObject(const QJsonObject & obj);
    QJsonObject toJsonObject();

    int calibLength;
    bool showDoneFiles;
    bool nextItemOnAssignView[3];
    QString layoutText;

    float dpi;
    float ppm;
};

#endif // SETTINGS_H
