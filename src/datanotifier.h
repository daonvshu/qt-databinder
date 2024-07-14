#pragma once

#include <qobject.h>
#include <functional>

#include "global.h"

DATA_BINDER_BEGIN_NAMESPACE

/**
 * 数据改变通知接口
 * @tparam T
 */
template <typename T>
class DataNotifyInterface {
public:
    std::function<void(const T&)> caller;

    virtual bool canBeReference() = 0;

    virtual bool isNotifyOnChanged() = 0;

    virtual ~DataNotifyInterface() = default;
};

/**
 * @brief 数据改变发布类
 * @tparam T
 */
template <typename T>
class DataNotifier {
public:
    /**
     * @brief 添加数据改变观察者回调
     * @param notifier 通知回调
     * @param isDataObserve 是否是数据源改变，而非界面导致的改变
     */
    void registerNotifier(DataNotifyInterface<T>* notifier, bool isDataObserve) {
        if (isDataObserve) {
            dataNotifier.append(notifier);
        } else {
            viewNotifier.append(notifier);
        }
    }

protected:
    /**
     * @brief 子类需要检查数据是否变更
     * @param other 新数据
     * @return
     */
    virtual bool checkValueChanged(const T& other) = 0;

    /**
     * @brief 子类应用数据变更
     * @param newValue 已改变的数据
     */
    virtual void doValueUpdate(const T& newValue) = 0;

    /**
     * @brief 检查数据是否变更，并发布变更通知
     * @param value 数据
     * @param setByData 是否是数据源改变，数据源改变将只会发布给数据源观察者，界面导致的数据改变则都会发布
     */
    void dataSetNotify(const T& value, bool setByData) {
        bool valueChanged = checkValueChanged(value);
        if (valueChanged) {
            doValueUpdate(value);
        }
        notify(value, valueChanged, dataNotifier);
        if (!setByData) {
            notify(value, valueChanged, viewNotifier);
        }
    }

private:
    /**
     * @brief 发布给指定变更观察者
     * @param value 数据
     * @param valueChanged 数据是否改变
     * @param notifiers 数据改变观察者
     */
    static void notify(const T& value, bool valueChanged, QList<DataNotifyInterface<T>*>& notifiers) {
        for (int i = 0; i < notifiers.size(); ) {
            if (notifiers[i]->canBeReference()) {
                if ((notifiers[i]->isNotifyOnChanged() && valueChanged) || !notifiers[i]->isNotifyOnChanged()) {
                    notifiers[i]->caller(value);
                }
                i++;
                continue;
            }
            delete notifiers.takeAt(i);
        }
    }

private:
    QList<DataNotifyInterface<T>*> dataNotifier; // 数据源观察者
    QList<DataNotifyInterface<T>*> viewNotifier; // 界面数据改变观察者
};

DATA_BINDER_END_NAMESPACE