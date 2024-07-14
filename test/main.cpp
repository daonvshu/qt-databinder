#include <QApplication>

#include "pagea.h"
#include "pageb.h"

#include "singleinstance.h"

#include <qdebug.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto pageA = new PageA;
    pageA->setAttribute(Qt::WA_DeleteOnClose);
    auto pageB = new PageB;
    pageB->setAttribute(Qt::WA_DeleteOnClose);

    pageA->show();
    pageB->show();

    SingleInstance::instance().cachedText.onViewChanged(&a, [] (const QString& text) {
        qDebug() << "cache text changed by view:" << text;
    });

    return a.exec();
}

