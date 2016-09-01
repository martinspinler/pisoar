#ifndef FACTORY_H
#define FACTORY_H

#include <QStyle>
#include <QIcon>
#include <QBrush>

class Factory : public QObject
{
    Q_OBJECT

    QStyle * s;

public:
    QIcon icon_new;
    QIcon icon_open;
    QIcon icon_save;
    QIcon icon_close;
    QIcon icon_settings;
    QIcon icon_help;
    QIcon icon_delete;
    QIcon icon_forward;
    QIcon icon_back;
    QIcon icon_clean;
    QIcon icon_refresh;
    QIcon icon_up;
    QIcon icon_dir;

    QIcon icon_ruler;
    QIcon icon_wip;
    QIcon icon_done;
    QIcon icon_image;
    QIcon icon_0;
    QIcon icon_1;
    QIcon icon_2;
    QIcon icon_wand;
    QIcon icon_hand;
    QIcon icon_rect;
    QIcon icon_color;

    QBrush blackbrush;
    QBrush selectbrush;
    QBrush whitebrush;

    Factory();
};

extern Factory * f;

#endif // FACTORY_H
