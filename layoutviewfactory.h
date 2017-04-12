#ifndef LAYOUTVIEWFACTORY_H
#define LAYOUTVIEWFACTORY_H

#include <QStringList>

#include "layoutview.h"

namespace LayoutViews {

class LayoutViewFactory
{
public:
    LayoutViewFactory();

    QStringList getAllViews();
    QList<bool> getViewsAvailability(ObjectItem *item);

    LayoutView * newLayoutView(LayoutItem *objectView);

};

}

#endif // LAYOUTVIEWFACTORY_H
