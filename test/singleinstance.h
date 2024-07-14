#pragma once

#include <qobject.h>

#include "mutableproperty.h"
#include "viewmodelbase.h"

DATA_BINDER_USING_NAMESPACE

class SingleInstance : public ViewModelBase {
public:
    static SingleInstance& instance();

    MutableProperty<QString> cachedText{QString(), "cachedText"};
    MutableProperty<QStringList> selectableTexts{{}, "selectableTexts"};
    MutableProperty<int> selectedIndex{-1, "selectedIndex"};

    QList<PropertyRefreshAble *> getPropertyBinders() override;
};
