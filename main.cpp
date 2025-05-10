#include "widget.h"

#include <QApplication>
#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QMessageBox>
#include <cstring> // 用于 memset

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    const QString uniqueKey = "sumikko";
    const int maxInstances = 3;

    // 创建信号量保护共享内存访问
    QSystemSemaphore semaphore(uniqueKey + "_semaphore", 1, QSystemSemaphore::Create);
    QSharedMemory sharedMemory(uniqueKey);

    semaphore.acquire(); // 加锁

    // 连接共享内存或创建
    if (!sharedMemory.attach()) {
        if (!sharedMemory.create(maxInstances * sizeof(bool))) {
            semaphore.release();
            QMessageBox::critical(nullptr, "错误", "无法创建共享内存。");
            return -1;
        }
        // 初始化共享内存为 0（所有 ID 均未占用）
        sharedMemory.lock();
        std::memset(sharedMemory.data(), 0, maxInstances * sizeof(bool));
        sharedMemory.unlock();
    }

    // 查找可用 ID
    sharedMemory.lock();
    bool *data = static_cast<bool *>(sharedMemory.data());
    int instanceId = -1;
    for (int i = 0; i < maxInstances; ++i) {
        if (!data[i]) {
            instanceId = i;
            data[i] = true;
            break;
        }
    }
    sharedMemory.unlock();
    semaphore.release();

    if (instanceId == -1) {
        QMessageBox::information(nullptr, "提示", "空啦，最多只有3只哦。");
        return 0;
    }


    Widget s(instanceId);
    s.show();

    int result = app.exec();

    // 释放 ID
    semaphore.acquire();
    sharedMemory.lock();
    data = static_cast<bool *>(sharedMemory.data());
    data[instanceId] = false; // 标记为未占用
    sharedMemory.unlock();
    semaphore.release();

    return result;
}
