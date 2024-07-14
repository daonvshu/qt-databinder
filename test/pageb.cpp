#include "pageb.h"

#include "singleinstance.h"

#include "propertybinder.h"

#include <qdebug.h>

PageB::PageB(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    PropertyBinder::bindViewModel(this, &SingleInstance::instance());
}

PageB::~PageB() {
    qDebug() << "page b released!";
}
