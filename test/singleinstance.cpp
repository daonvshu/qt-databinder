#include "singleinstance.h"

SingleInstance &SingleInstance::instance() {
    static SingleInstance singleInstance;
    return singleInstance;
}

QList<PropertyRefreshAble *> SingleInstance::getPropertyBinders() {
    return { &cachedText, &selectableTexts, &selectedIndex };
}
