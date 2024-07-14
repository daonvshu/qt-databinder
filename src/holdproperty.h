#pragma once

#include "global.h"

#include <qobject.h>
#include <qstring.h>
#include <qvariant.h>

DATA_BINDER_BEGIN_NAMESPACE

/**
 * @brief 保存设置的上一个和值对应的名称
 * @tparam T
 */
template<typename T>
class HoldProperty {
public:
    explicit HoldProperty(T defaultValue = T(), QString propertyKey = QString())
        : propertyKey(std::move(propertyKey))
        , defaultValue(defaultValue)
        , holdValue(defaultValue)
        , lastValue(T())
    {}

    /**
     * @brief 引用方式取值
     * @return
     */
    T& operator()() {
        return holdValue;
    }

    /**
     * @brief 常量引用方式取值
     * @return
     */
    const T& operator()() const {
        return holdValue;
    }

    /**
     * @brief 设置值
     * @param value
     * @return
     */
    virtual HoldProperty& operator=(const T& value) {
        lastValue = holdValue;
        holdValue = value;
        return *this;
    }

    /**
     * @brief 上一次设置的值
     * @return
     */
    const T& last() const {
        return lastValue;
    }

protected:
    QString propertyKey;
    T defaultValue;
    T holdValue;
    T lastValue;
};

DATA_BINDER_END_NAMESPACE