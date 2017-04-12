#include "layoutviewfactory.h"

namespace LayoutViews {

LayoutViewFactory::LayoutViewFactory()
{

}


QStringList LayoutViewFactory::getAllViews()
{
    QStringList list;
    list.append("Empty");
    list.append("Top");
    list.append("Top-Side");
    list.append("Top-Bottom");
    list.append("Top-Side-Front");
    return list;
}

QList<bool> LayoutViewFactory::getViewsAvailability(ObjectItem * item)
{
    QList<bool> list;
    list.append(item->images.size() >= 0);
    list.append(item->images.size() >= 1);
    list.append(item->images.size() >= 2);
    list.append(item->images.size() >= 2);
    list.append(item->images.size() >= 3);
    return list;
}
LayoutView * LayoutViewFactory::newLayoutView(LayoutItem * item)
{
    ObjectView * objectView = item->objectView();

    if(objectView->type() == -1)  {
        switch(objectView->item.images.size()) {
            case 0: objectView->setType(0); break;
            case 1: objectView->setType(1); break;
            case 2: objectView->setType(2); break;
            case 3: objectView->setType(4); break;
            default: objectView->setType(0); break;
        }
    }

    LayoutView * view = NULL;
    switch(objectView->type()) {
        case 0: view = new LayoutView(item); break;
        case 1: view = new LayoutTopView(item); break;
        case 2: view = new LayoutTopSideView(item); break;
        case 3: view = new LayoutTopBottomView(item); break;
        case 4: view = new LayoutTopSideFrontView(item); break;
    }
    return view;
}

}
