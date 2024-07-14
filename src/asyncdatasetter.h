#pragma once

#include "global.h"

#include <qobject.h>
#include <qvariant.h>
#include <qmutex.h>
#include <qthread.h>
#include <qeventloop.h>

DATA_BINDER_BEGIN_NAMESPACE

/**
 * @brief 异步数据设置，让工作线程中数据的修改操作变更到在主线程中修改
 */
class AsyncDataSetter : public QObject {
    Q_OBJECT

public:
    explicit AsyncDataSetter(QObject* parent = nullptr)
        : QObject(parent)
    {
        mainThread = QThread::currentThreadId();

        connect(this, &AsyncDataSetter::setDataAsync,
                this, &AsyncDataSetter::pendingToSetValue, Qt::QueuedConnection);
    }

signals:
    void setDataAsync(QPrivateSignal);

private:
    QMutex mutex;
    Qt::HANDLE mainThread;
    QEventLoop* waitForApplyLoop = nullptr;

    /**
     * @brief 提取线程中的数据，并在主线程中设置
     */
    void pendingToSetValue() {
        mutex.lock();
        lastSetData = pendingData;
        pendingData = QVariant();
        mutex.unlock();
        setDataInMainThread(lastSetData);
        if (waitForApplyLoop != nullptr) {
            waitForApplyLoop->quit();
        }
    }

protected:
    QVariant pendingData; // 等待主线程接收的数据
    QVariant lastSetData; // 线程最后设置的一个数据

protected:
    /**
     * @brief 检查是否在线程中设置数据，如果是主线程，则直接设置数据，如果是工作线程，则发送信号到主线程设置，
     * 如果工作线程设置的速率太快，主线程只取最后一个设置的数据
     * @param value
     * @param waitForApply 是否等待主线程取值并设置
     */
    void setValue(const QVariant& value, bool waitForApply = false) {
        if (QThread::currentThreadId() == mainThread) {
            lastSetData = value;
            setDataInMainThread(lastSetData);
        } else {
            mutex.lock();
            bool needRequestTask = pendingData.isNull();
            pendingData = value;
            mutex.unlock();
            if (needRequestTask) {
                emit setDataAsync(QPrivateSignal());
            }
            if (waitForApply) {
                waitForApplyLoop = new QEventLoop;
                waitForApplyLoop->exec();
                waitForApplyLoop->deleteLater();
            }
        }
    }

    /**
     * @brief 通知子类修改数据
     * @param value
     */
    virtual void setDataInMainThread(const QVariant& value) = 0;
};

DATA_BINDER_END_NAMESPACE