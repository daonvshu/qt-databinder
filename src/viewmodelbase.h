#pragma once

#include "mutableproperty.h"

DATA_BINDER_BEGIN_NAMESPACE

/**
 * @brief ViewModel接口
 */
class ViewModelBase : public QObject {
public:
    using QObject::QObject;

    virtual QList<PropertyRefreshAble*> getPropertyBinders() = 0;
};

DATA_BINDER_END_NAMESPACE