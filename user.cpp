#include "user.h"
#include "qdebug.h"
#include "direct.h"

//Constructor of user
User::User(int waitFloor, int destinationFloor) {
    if (waitFloor == destinationFloor) {
        throw std::invalid_argument("Wait floor is the same as the destination");
    }

    this->waitFloor = waitFloor;
    this->destinationFloor = destinationFloor;
    status = NEW;
}

// Returns direction of ride
Direction User::getDirection() {
    return waitFloor < destinationFloor ? UP : DOWN;
}

// car has been allocated.
void User::carAllocated() {
    status = WAITING_CAR;
    qInfo() << "[USR]" <<"User waits for allocated Car";
}

//Returns if user has NOT allocated car
bool User::hasNotAllocatedCar() {
    return status == NEW;
}


//user in car
void User::goToCar() {
    status = IN_CAR;
    qInfo() << "[USR]" <<"User in car:" << waitFloor << "->" << destinationFloor;
}

//Returns if user is in car
bool User::isInCar() {
    return status == IN_CAR;
}

// user is done
void User::done() {
    status = DONE;
    qInfo() << "[USR]" <<"User has arrived:" << waitFloor << "->" << destinationFloor;
}

bool User::isDone() {
    return status == DONE;
}


// user is waiting
bool User::isWaiting() {
    return status == WAITING_CAR || status == NEW;
}

// user is waiting in specific floor
bool User::isWaiting(int floor) {
    return isWaiting() && waitFloor == floor;
}

//return waiting floor.
int User::getWaitFloor() {
    return waitFloor;
}
//return destination floor
int User::getDestinationFloor() {
    return destinationFloor;
}

bool operator==(const User &lhs, const User &rhs) {
    return lhs.destinationFloor == rhs.destinationFloor && lhs.waitFloor == rhs.waitFloor;
}

QDebug& operator<< (QDebug& stream, const User& p) {
    stream << "User[waitFloor:" << p.waitFloor << ",destFloor:" << p.destinationFloor << ",status:" << p.status << "]";
    return stream;
}
