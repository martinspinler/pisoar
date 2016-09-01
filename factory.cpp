#include <QApplication>

#include "factory.h"

Factory::Factory() :
    blackbrush(Qt::black),
    selectbrush(Qt::lightGray),
    whitebrush(Qt::white)
{
    s           = QApplication::style();

    icon_new    = s->standardIcon(QStyle::SP_FileIcon);
    icon_open   = s->standardIcon(QStyle::SP_DialogOpenButton);
    icon_save   = s->standardIcon(QStyle::SP_DialogSaveButton);
    icon_close  = s->standardIcon(QStyle::SP_DialogCloseButton);
    icon_settings=s->standardIcon(QStyle::SP_ComputerIcon);
    icon_help   = s->standardIcon(QStyle::SP_DialogHelpButton);
    icon_delete = s->standardIcon(QStyle::SP_TrashIcon);
    icon_forward= s->standardIcon(QStyle::SP_ArrowForward);
    icon_back   = s->standardIcon(QStyle::SP_ArrowBack);
    icon_clean  = s->standardIcon(QStyle::SP_DialogDiscardButton);
    icon_refresh= s->standardIcon(QStyle::SP_BrowserReload);
    icon_up     = s->standardIcon(QStyle::SP_FileDialogToParent);
    icon_dir    = s->standardIcon(QStyle::SP_DirIcon);

    icon_ruler  = QIcon(":/icons/res/ruler.png");
    icon_wip    = QIcon(":/icons/res/wip.png");
    icon_done   = QIcon(":/icons/res/done.png");
    icon_image  = QIcon(":/icons/res/image.png");
    icon_0      = QIcon(":/icons/res/0.png");
    icon_1      = QIcon(":/icons/res/1.png");
    icon_2      = QIcon(":/icons/res/2.png");
    icon_wand   = QIcon(":/icons/res/wand.png");
    icon_hand   = QIcon(":/icons/res/hand.png");
    icon_rect   = QIcon(":/icons/res/rect.png");
    icon_color  = QIcon(":/icons/res/color.png");
}
