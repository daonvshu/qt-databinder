#include "pagea.h"

#include "singleinstance.h"

#include "propertybinder.h"

#include <qevent.h>
#include <qdebug.h>
#include <qthread.h>

PageA::PageA(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    PropertyBinder::bindViewModel(this, &SingleInstance::instance());

    SingleInstance::instance().selectedIndex.onViewChanged(this, [] (int index) {
        qDebug() << "selected index changed in view:" << index;
    }, false);
}

void PageA::on_update_cb_clicked() {
    SingleInstance::instance().selectableTexts = {
            "text1", "text2", "text3", "text4", "text5"
    };

    SingleInstance::instance().selectedIndex = 1;

}

void PageA::on_update_thr_clicked() {
    QThread::create([&] {
        QStringList newItems = {
                "newText1", "newText2", "newText3", "newText4"
        };

        SingleInstance::instance().selectableTexts.postValue(newItems, true);
        //SingleInstance::instance().selectedIndex.postValue(2);
    })->start();
}

PageA::~PageA() {
    qDebug() << "page a released!";
}
