#pragma once

#include "asyncdatasetter.h"
#include "holdproperty.h"

#include "dataobserver.h"
#include "variantcastutil.h"

DATA_BINDER_BEGIN_NAMESPACE

/**
 * @brief 可监听对象
 * @tparam T
 */
template<typename T>
class ObserverProperty : public HoldProperty<T>, public DataNotifier<T>, public AsyncDataSetter {
public:
    using HoldProperty<T>::HoldProperty;

    /**
     * @brief 赋值并检查值是否改变，将触发onDataChanged
     * @param value
     * @return
     */
    ObserverProperty& operator=(const T& value) override {
        DataNotifier<T>::dataSetNotify(value, true);
        return *this;
    }

    /**
     * @brief 工作线程中设置值，如果设置值时的操作在主线程，与operator=表现一致
     * @param value
     * @param waitForApply 是否同步等待值设置到主线程
     */
    void postValue(const T& value, bool waitForApply = false) {
        AsyncDataSetter::setValue(VariantCastUtil::getVariantValue<T>(value), waitForApply);
    }

    /**
     * @brief 赋值并检查值是否改变，将触发onViewChanged
     * @param value
     */
    void setByView(const T& value) {
        DataNotifier<T>::dataSetNotify(value, false);
    }

    /**
     * @brief 监听数值改变
     * @tparam L 监听者对象类型
     * @param context 监听者对象
     * @param changeNotify 数值改变时触发的回调函数
     * @param notifyOnChanged true表示数值改变时才触发回调函数
     */
    template <typename L>
    void onDataChanged(L* context,
                       const std::function<void(const T& value)>& changeNotify,
                       bool notifyOnChanged = true)
    {
        auto observer = new DataObserver<L, T>(context, notifyOnChanged);
        observer->caller = changeNotify;
        DataNotifier<T>::registerNotifier(observer, true);
    }

    /**
     * @brief 监听界面数值改变，通过setByView设置值时触发该回调
     * @tparam L 监听者对象类型
     * @param context 监听者对象
     * @param changeNotify 数值改变时触发的回调函数
     * @param notifyOnChanged true表示数值改变时才触发回调函数
     */
    template <typename L>
    void onViewChanged(L* context,
                       const std::function<void(const T& value)>& changeNotify,
                       bool notifyOnChanged = true)
    {
        auto observer = new DataObserver<L, T>(context, notifyOnChanged);
        observer->caller = changeNotify;
        DataNotifier<T>::registerNotifier(observer, false);
    }

protected:
    /**
     * @brief 判断值是否改变，该函数要求类型具有operator==的重载实现
     * @param other
     * @return
     */
    bool checkValueChanged(const T& other) override {
        return !(this->holdValue == other);
    }

    /**
     * @brief 赋值新的值，并保存上一次值，当checkValueChanged返回true时才进行赋值
     * @param newValue
     */
    void doValueUpdate(const T& newValue) override {
        this->lastValue = this->holdValue;
        this->holdValue = newValue;
    }

    /**
     * @brief 通过postValue工作线程中设置值时回调该函数，让赋值操作在主线程进行
     * @param value
     */
    void setDataInMainThread(const QVariant &value) override {
        DataNotifier<T>::dataSetNotify(value.value<T>(), true);
    }
};

DATA_BINDER_END_NAMESPACE