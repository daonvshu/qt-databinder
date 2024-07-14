#pragma once

#include "datanotifier.h"

#include <qobject.h>
#include <functional>
#include <qpointer.h>

DATA_BINDER_BEGIN_NAMESPACE

/**
 * @brief 用于观察者上下文有效性(是否释放)检查
 * @tparam T Observer
 * @tparam K Data Type
 */
template <typename T, typename K>
class DataObserver : public DataNotifyInterface<K> {

    QPointer<T> contextPointer;

public:
    explicit DataObserver(T* p, bool notifyOnChanged): contextPointer(p), notifyOnChanged(notifyOnChanged) {}

protected:
    bool canBeReference() override {
        return !contextPointer.isNull();
    }

    bool isNotifyOnChanged() override  {
        return notifyOnChanged;
    }

private:
    bool notifyOnChanged;
};

DATA_BINDER_END_NAMESPACE