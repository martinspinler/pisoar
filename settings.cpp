#include <QLabel>
#include <QIntValidator>

#include "settings.h"
#include "database.h"

Settings::Settings(QWidget *parent) : QDialog(parent)
{
    vbox = new QVBoxLayout();
    layout = new QGridLayout();

    fl_show         = new QCheckBox("Zobrazit hotové soubory");
    db_align        = new QLineEdit();
    db_resolution   = new QLineEdit();
    db_edgeTop      = new QLineEdit();
    db_edgeBottom   = new QLineEdit();
    db_edgeSides    = new QLineEdit();
    db_pageWidth    = new QLineEdit();
    db_pageHeight   = new QLineEdit();
    db_calibSize    = new QLineEdit();

    /*layout->addWidget(new QLabel("Zarovnání"),0,0);
    layout->addWidget(new QLabel("Rozlišení"),1,0);
    layout->addWidget(new QLabel("Okraj nahoře"),2,0);
    layout->addWidget(new QLabel("Okraj dole"),3,0);
    layout->addWidget(new QLabel("Okraj po stranách"),4,0);
    layout->addWidget(new QLabel("Šířka stránky"),5,0);
    layout->addWidget(new QLabel("Výška stránky"),6,0);*/
    layout->addWidget(new QLabel("Kalibrační velikost [cm]"),8,0);

    /*layout->addWidget(db_align,0,1);
    layout->addWidget(db_resolution,1,1);
    layout->addWidget(db_edgeTop,2,1);
    layout->addWidget(db_edgeBottom,3,1);
    layout->addWidget(db_edgeSides,4,1);
    layout->addWidget(db_pageWidth,5,1);
    layout->addWidget(db_pageHeight,6,1);*/
    layout->addWidget(db_calibSize,8,1);

    vbox->addLayout(layout);
    for(int i = 0; i < 3; i++) {
        db_next[i]      = new QCheckBox(QString("Skočit na další objekt po přiřazení %1. objektu").arg(i+1));
        vbox->addWidget(db_next[i]);
    }
    vbox->addWidget(fl_show);

    //db_calibSize->setValidator(new QIntValidator(1, 10, this));

    setLayout(vbox);
    setWindowTitle("Nastavení");

    /* Default values */
    calibLength             = 5;
    showDoneFiles           = true;
    nextItemOnAssignView[0] = true;
    nextItemOnAssignView[1] = true;
    nextItemOnAssignView[2] = true;

    for(int i = 0; i < 3; i++)
        db_next[i]->setChecked(nextItemOnAssignView[i]);
    QString text = QString::number(calibLength);
    db_calibSize->setText(text);
    fl_show->setChecked(showDoneFiles);
}

void Settings::closeEvent(QCloseEvent * event){
    calibLength = db_calibSize->text().toInt();
    showDoneFiles = fl_show->isChecked();
    for(int  i = 0; i < 3; i++)
        nextItemOnAssignView[i] = db_next[i]->isChecked();

    return QDialog::closeEvent(event);
}

void Settings::fromJsonObject(const QJsonObject & obj)
{
    calibLength             = obj["calibLength"].toInt();
    showDoneFiles           = obj["showDoneFiles"].toBool();
    nextItemOnAssignView[0] = obj["nextItemOnAssignView1"].toBool();
    nextItemOnAssignView[1] = obj["nextItemOnAssignView2"].toBool();
    nextItemOnAssignView[2] = obj["nextItemOnAssignView3"].toBool();

    for(int i = 0; i < 3; i++)
        db_next[i]->setChecked(nextItemOnAssignView[i]);
    QString text = QString::number(calibLength);
    db_calibSize->setText(text);
    fl_show->setChecked(showDoneFiles);
}

QJsonObject Settings::toJsonObject()
{
    QJsonObject obj;
    obj["calibLength"]          = calibLength;
    obj["showDoneFiles"]        = showDoneFiles;
    obj["nextItemOnAssignView1"]= nextItemOnAssignView[0];
    obj["nextItemOnAssignView2"]= nextItemOnAssignView[1];
    obj["nextItemOnAssignView3"]= nextItemOnAssignView[2];
    return obj;
}

