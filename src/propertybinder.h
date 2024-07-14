#pragma once

#include "viewmodelbase.h"

#include <qobject.h>
#include <qhash.h>

#include <qcombobox.h>
#include <QMetaProperty>
#include <qdebug.h>
#include <qregexp.h>

DATA_BINDER_BEGIN_NAMESPACE

/**
 * @brief 用于绑定回调函数
 */
class PropertyUpdater : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

    /**
     * @brief 检查target对象的propertyName是否具有信号函数，如果存在则绑定到slot函数
     * @param target
     * @param propertyName
     * @param slot
     */
    void connectSignal(QObject* target, const QByteArray& propertyName, const std::function<void(const QVariant&)>& slot) {
        mTarget = target;
        mPropertyName = propertyName;
        mSlot = slot;
        auto meta = target->metaObject();
        int index = meta->indexOfProperty(propertyName.constData());
        if (index != -1) {
            auto property = meta->property(index);
            if (property.hasNotifySignal()) {
                auto notifySignal = property.notifySignal();
                connect(target, notifySignal, this, metaObject()->method(metaObject()->indexOfSlot("callbackFunc()")));
            }
        }
    }

private slots:
    void callbackFunc() {
        mSlot(mTarget->property(mPropertyName));
    };

private:
    QObject* mTarget = nullptr;
    QByteArray mPropertyName;
    std::function<void(const QVariant&)> mSlot;
};

/**
 * @brief View数据绑定工具类，扫描界面控件中的propertybinds动态属性，如果设置则绑定到ViewModel中的对象
 */
class PropertyBinder {
public:
    /**
     * @brief 扫描context页面中的所有子对象，如果具有propertybinds属性则进行绑定
     * @param context
     * @param vm
     */
    static void bindViewModel(QObject* context, ViewModelBase* vm) {
        QHash<QString, PropertyRefreshAble*> propertyMap;
        for (const auto& binder : vm->getPropertyBinders()) {
            auto key = binder->getPropertyKey();
            if (!key.isEmpty()) {
                propertyMap[key] = binder;
                binder->setBindContext(context);
            }
        }

        auto children = context->findChildren<QWidget*>();
        for (auto* child : children) {
            auto propertyBindList = child->property("propertybinds").toStringList();
            for (const auto& bindStr : propertyBindList) {
                bindPropertyChanged(child, bindStr, propertyMap);
            }
        }
    }

private:
    /**
     * @brief 根据其设置的key，绑定指定对象到ViewModel中的对象
     * @param target
     * @param propertyBindName 属性绑定模板字符串，vm <-> property
     * @param propertyMap
     */
    static void bindPropertyChanged(QObject* target, const QString& propertyBindName,
                                    const QHash<QString, PropertyRefreshAble*>& propertyMap) {
        QRegExp rx("(<?->?)");
        int pos = 0;
        if ((pos = rx.indexIn(propertyBindName, pos)) != -1) {
            auto bindType = rx.cap(1);
            auto mutablePropName = propertyBindName.mid(0, pos);
            pos += rx.matchedLength();
            auto propertyName = propertyBindName.mid(pos);
            //qDebug() << "target:" << target->objectName() << "mp:" << mutablePropName << "prop:" << propertyName << "bind:" << bindType;
            auto refreshAble = propertyMap[mutablePropName];
            if (refreshAble != nullptr) {
                if (bindType.startsWith(QChar('<'))) {
                    bindTargetPropertyChanged(target, propertyName.toLatin1(), propertyMap[mutablePropName]);
                }
                if (bindType.endsWith(QChar('>'))) {
                    bindMutablePropertyChanged(target, propertyName.toLatin1(), propertyMap[mutablePropName]);
                }
            }
        }
    }

    /**
     * @brief 绑定ViewModel中的对象变化时更新该target界面UI中的数据
     * @param target
     * @param propertyName
     * @param refreshAble
     */
    static void bindMutablePropertyChanged(QObject* target, const QByteArray& propertyName, PropertyRefreshAble* refreshAble) {
        refreshAble->addPropertySetter([=] (const QVariant& data) {
            target->blockSignals(true);
            setObjectValue(target, propertyName, data);
            target->blockSignals(false);
        });
    }

    /**
     * @brief 绑定界面属性值改变时修改ViewModel中的对象
     * @param target
     * @param propertyName
     * @param refreshAble
     */
    static void bindTargetPropertyChanged(QObject* target, const QByteArray& propertyName, PropertyRefreshAble* refreshAble) {
        (new PropertyUpdater(target))->connectSignal(target, propertyName, [=] (const QVariant& data) {
            refreshAble->applyDataFromView(data);
        });
    }

    /**
     * @brief 检查propertyName是否为特殊属性
     * @param target
     * @param propertyName
     * @param data
     */
    static void setObjectValue(QObject* target, const QByteArray& propertyName, const QVariant& data) {
        if (auto cb = dynamic_cast<QComboBox*>(target)) {
            comboBoxDataApply(cb, propertyName, data);
            return;
        }
        defaultDataApply(target, propertyName, data);
    }

    /**
     * @brief 默认的属性设置方法
     * @param target
     * @param propertyName
     * @param data
     */
    static void defaultDataApply(QObject* target, const QByteArray& propertyName, const QVariant& data) {
        target->setProperty(propertyName, data);
    }

    /**
     * @brief ComboBox如果设置了items属性，特殊处理设置函数
     * @param target
     * @param propertyName
     * @param data
     */
    static void comboBoxDataApply(QComboBox* target, const QByteArray& propertyName, const QVariant& data) {
        if (propertyName == "items") {
            auto dataList = data.toStringList();
            auto oldIndex = target->currentIndex();
            target->clear();
            target->addItems(dataList);
            if (oldIndex < dataList.size()) {
                if (oldIndex != -1) {
                    target->setCurrentIndex(oldIndex);
                }
            } else {
                auto newIndex = dataList.isEmpty() ? -1 : 0;
                target->setCurrentIndex(newIndex);
                target->blockSignals(false);
                emit target->currentIndexChanged(newIndex);
            }
            return;
        }

        defaultDataApply(target, propertyName, data);
    }
};

DATA_BINDER_END_NAMESPACE