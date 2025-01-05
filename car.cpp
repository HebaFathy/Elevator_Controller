#include <utility>
#include <QtWidgets/QGridLayout>
#include <QtCore/QThread>
#include <QtCore/QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QInputDialog>
#include "car.h"
#include "controller.h"
#include "user.h"


// Car constructor

Car::Car(){}
Car::Car(QString name, int minFloor, int maxFloor, QProgressBar *slider, unsigned long waitDuration) {
    this->name = std::move(name);
    this->minFloor = minFloor;
    this->maxFloor = maxFloor;
    this->slider = slider;
    this->slider->setMinimum(Controller::GROUND_FLOOR_NUMBER);
    this->slider->setOrientation(Qt::Orientation::Vertical);
    this->waitDuration = waitDuration;
    direction = STOP;
    currentFloor = 0;
    isReturning = false;
    capacity =10;
}

// start ride
void Car::move() {
    if (currentFloor == destinationFloor) {
        qInfo() << "[CAR]" << "Car on destination floor";
        checkIfAnyUserIsInDestination();
        checkIfAnyUserWantsGetIn();
    } else if (currentFloor < destinationFloor) {
        direction = UP;
        qInfo() << "[CAR]" << "Car going up";
        while (currentFloor < destinationFloor) {
            QThread::msleep(waitDuration);
            currentFloor++;
            updateView();
            qInfo() << "[CAR]" << "Car floor" << currentFloor;
            checkIfAnyUserIsInDestination();
            checkIfAnyUserWantsGetIn();
        }

    } else {
        direction = DOWN;
        qInfo() << "[CAR]" << "Car going down";
        while (currentFloor > destinationFloor) {
            QThread::msleep(waitDuration);
            currentFloor--;
            updateView();
            qInfo() << "[CAR]" << "Car floor" << currentFloor;
            checkIfAnyUserIsInDestination();
            checkIfAnyUserWantsGetIn();
        }
    }

    if (isReturning) {
        qInfo() << "[CAR]" << "Car reached destination floor" << destinationFloor;
        direction = STOP;
        users.clear();
        isReturning = false;
        finishedTransfer();
    } else {
        isReturning = true;
        updateDestinationFloor();
        move();
    }
}


Direction Car::getDirection() {
    return direction;
}

QString Car::getName() {
    return this->name;
}

int Car::getMinFloor() {
    return this->minFloor;
}

int Car::getMaxFloor() {
    return this->maxFloor;
}

int Car::getCurrentFloor() {
    return this->currentFloor;
}


// lowest destination floor.
int Car::getLowestDestinationFloor() {
    User *min = *std::min_element(users.begin(), users.end(), [=](User *a, User *b) {
        return (a->getDestinationFloor() < b->getDestinationFloor());
    });
    return min->getDestinationFloor();
}

// higest destination floor.
int Car::getHighestDestinationFloor() {
    User *max = *std::max_element(users.begin(), users.end(), [=](User *a, User *b) {
        return (a->getDestinationFloor() < b->getDestinationFloor());
    });
    return max->getDestinationFloor();
}


// lowest waiting floor
int Car::getLowestWaitingFloor() {
    User *min = *std::min_element(users.begin(), users.end(), [=](User *a, User *b) {
        return (a->getWaitFloor() < b->getWaitFloor());
    });
    return min->getWaitFloor();
}

// highest waiting floor.
int Car::getHighestWaitingFloor() {
    User *max = *std::max_element(users.begin(), users.end(), [=](User *a, User *b) {
        return (a->getWaitFloor() < b->getWaitFloor());
    });
    return max->getWaitFloor();
}


// User reaches it's destination floor
void Car::checkIfAnyUserIsInDestination() {
    bool anyOneInDestination = std::any_of(users.begin(), users.end(), [=](User *user) {
        return user->isInCar() && currentFloor == user->getDestinationFloor();
    });

    if (anyOneInDestination) {
        door.openDoor(waitDuration);
        for (auto &user : users) {
            if (user->isInCar() && currentFloor == user->getDestinationFloor()) {
                user->done();
            }
        }
        door.closeDoor(waitDuration);
        pickedUpCar();
    }
}

//user wants to ride car
void Car::checkIfAnyUserWantsGetIn() {
    bool anyOneWantsGetIn = std::any_of(users.begin(), users.end(), [=](User *user) {
        return user->isWaiting(currentFloor) && (user->getDirection() == direction || user->getWaitFloor() == destinationFloor);
    });

    bool anyOneInDestination = std::any_of(users.begin(), users.end(), [=](User *user) {
        return currentFloor == user->getDestinationFloor();
    });

    if (anyOneWantsGetIn) {
        if (!anyOneInDestination) {
            door.openDoor(waitDuration);
        }

        for (auto &user : users) {
            if (user->isWaiting(currentFloor)) {
                user->goToCar();
            }
        }
        updateDestinationFloor();

        if (!anyOneInDestination) {
            door.closeDoor(waitDuration);
            pickedUpCar();
        }
    }
}

// Add user to ride
void Car::addUser(User *user) {
    qInfo() << "[CAR]" << "User is waiting for car at floor" << user->getWaitFloor();

    if (!canHandle(user)) {
        throw std::invalid_argument("Don't add user when it's not handled");
    }

    users.push_front(user);

    if (direction == STOP) {
        destinationFloor = user->getWaitFloor();
        updateDestinationFloor();
        future = QtConcurrent::run([=] { move(); });
    }
}

//Update destination floor
void Car::updateDestinationFloor() {
    int lowestWaitingFloor = getLowestWaitingFloor();
    int highestWaitingFloor = getHighestWaitingFloor();

    int lowestDestinationFloor = getLowestDestinationFloor();
    int highestDestinationFloor = getHighestDestinationFloor();

    if (isReturning) {
        if (currentFloor == highestDestinationFloor && destinationFloor == lowestDestinationFloor) {
            destinationFloor = currentFloor;
        } else {
            destinationFloor =
                    currentFloor < highestDestinationFloor ? highestDestinationFloor : lowestDestinationFloor;
        }
    } else {
        if (currentFloor == highestWaitingFloor && destinationFloor == lowestWaitingFloor) {
            destinationFloor = currentFloor;
        } else {
            destinationFloor = currentFloor < highestWaitingFloor ? highestWaitingFloor : lowestWaitingFloor;
        }
    }
    qInfo() << "[CAR]" << "Calculated destination floor as" << destinationFloor;
}

// Checks if given user can be handled in car

bool Car::canHandle(User *user) {
    if (user->getWaitFloor() != 0 &&
        (user->getWaitFloor() < minFloor || user->getWaitFloor() > maxFloor)) {
        return false;
    }

    if (user->getDestinationFloor() != 0 &&
        (user->getDestinationFloor() < minFloor || user->getDestinationFloor() > maxFloor)) {
        return false;
    }

    if (direction == STOP) {
        return true;
    }

    if (direction == UP) {
        if (isReturning) {
            return user->getDirection() == UP && user->getWaitFloor() > currentFloor;
        } else {
            return user->getDirection() == DOWN && user->getWaitFloor() <= destinationFloor;
        }
    }

    if (direction == DOWN) {
        if (isReturning) {
            return user->getDirection() == DOWN && user->getWaitFloor() < currentFloor;
        } else {
            return user->getDirection() == UP && user->getWaitFloor() >= destinationFloor;
        }
    }
}


void Car::rerender(int maxFloor) {
    slider->setMaximum(maxFloor);
}

void Car::stop() {
    qInfo() << "[CAR]" << "Forcing stop";
    future.cancel();
    future.waitForFinished();
}


Door Car::getDoor()
{
    return this->door;
}

void Car:: overload(){
     if(users.size() >capacity)
        {
         Controller c;
         c.alert("O");
        }
     else {
         qInfo()<<"[CAR]"<<"Number of users ["<<users.size()<<"] less than Capacity ["<<capacity<<"]";
     }

}
