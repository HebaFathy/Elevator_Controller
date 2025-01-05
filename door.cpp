#include <QtCore/QThread>
#include <QtCore/QFuture>
#include <QtConcurrent/QtConcurrent>
#include "door.h"
#include "car.h"

//open door
bool Door::openDoor(unsigned long waitDuration) {
    qInfo() << "[DOR]" << "Door opening";
    QThread::msleep(waitDuration);
    return true;
}

//close door
bool Door::closeDoor(unsigned long waitDuration) {
    qInfo() << "[DOR]" << "Door closing";
    QThread::msleep(2 * waitDuration);
    return true;
}
