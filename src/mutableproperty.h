#pragma once

#include "observerproperty.h"
#include "variantcastutil.h"

DATA_BINDER_BEGIN_NAMESPACE

/**
 * @brief 可设置的属性类型，MutableProperty使用，将其模板类型转为通用类型
 */
class PropertyRefreshAble {
public:
    /**
     * @brief 获取MutableProperty保存的key名称
     * @return
     */
    virtual QString getPropertyKey() = 0;

    /**
     * @brief 设置赋值上下文对象
     * @param context
     */
    virtual void setBindContext(QObject* context) {
        currentContext = context;
    }

    /**
     * @brief 控件值改变时，调用此函数
     * @param data
     */
    virtual void applyDataFromView(const QVariant& data) = 0;

    /**
     * @brief 添加当前上下文对象和回调函数的绑定
     * @param callback
     */
    void addPropertySetter(const std::function<void(const QVariant&)>& callback) {
        Q_ASSERT(currentContext != nullptr);
        propertySetters[currentContext] << callback;
    }

protected:
    /**
     * @brief 回调函数调用工具类，将任意类型转为QVariant调用，自定义类型需要注册到元对象系统中
     * @tparam T
     * @param value
     * @param setters
     */
    template<typename T>
    static void callPropertySet(const T& value, const QList<std::function<void(const QVariant&)>>& setters) {
        for (const auto& setter : setters) {
            setter(VariantCastUtil::getVariantValue<T>(value));
        }
    }

protected:
    QObject* currentContext; //当前设置上下文
    QMap<QObject*, QList<std::function<void(const QVariant&)>>> propertySetters; //上下文对象和回调函数的绑定
};

/**
 * @brief 可观察数据类
 * @tparam T 设置类型
 * @tparam K 存储类型
 */
template<typename T, typename K = T>
class MutableProperty : public ObserverProperty<K>, public PropertyRefreshAble {
public:
    typedef K(*DataTransfer)(const T&);
    explicit MutableProperty(T defaultValue = T(), QString propertyKey = QString(), DataTransfer dataTransfer = nullptr)
        : ObserverProperty<T>(defaultValue, propertyKey)
        , dataTransfer(dataTransfer)
    {}

    MutableProperty<T, K> &operator=(const T &value) override {
        if (dataTransfer == nullptr) {
            ObserverProperty<T>::operator=(value);
        } else {
            ObserverProperty<T>::operator=(dataTransfer(value));
        }
        return *this;
    }

protected:
    QString getPropertyKey() override {
        return this->propertyKey;
    }

    void setBindContext(QObject *context) override {
        PropertyRefreshAble::setBindContext(context);
        this->onDataChanged(context, [&, context] (const K& data) {
            this->template callPropertySet<K>(data, propertySetters[context]);
        }, false);
    }

    void applyDataFromView(const QVariant &data) override {
        this->setByView(data.value<K>());
    }

private:
    DataTransfer dataTransfer;
};

class MutableVariantProperty : public MutableProperty<QVariant> {
public:
    using MutableProperty<QVariant>::MutableProperty;

    template<typename T>
    MutableVariantProperty& operator=(const T& value) {
        MutableProperty<QVariant>::operator=(VariantCastUtil::getVariantValue<T>(value));
        return *this;
    }
};

DATA_BINDER_END_NAMESPACE