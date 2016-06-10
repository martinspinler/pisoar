#include "settings.h"

Settings::Settings(QWidget *parent) : QWidget(parent)
{
    fl_show     = new QCheckBox     ("Zobrazit hotové");
    //addWidget(fl_show);
    //connect(fl_show,    &QCheckBox::clicked,   this, &Pisoar::fl_show_stateChanged);
    fl_show->setChecked(true);
}
