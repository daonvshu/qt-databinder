#pragma once

#include "global.h"

#include <qobject.h>
#include <qvariant.h>

DATA_BINDER_BEGIN_NAMESPACE

/**
 * @brief 自定义类型转为QVariant工具类
 */
class VariantCastUtil {
public:
    template<typename T>
    static QVariant getVariantValue(const typename std::enable_if<std::is_constructible<QVariant, T>::value, T>::type& value) {
        return QVariant(value);
    }

    template<typename T>
    static QVariant getVariantValue(const typename std::enable_if<!std::is_constructible<QVariant, T>::value, T>::type& value) {
        return QVariant::fromValue(value);
    }
};

DATA_BINDER_END_NAMESPACE