#ifndef CAR_H
#define CAR_H

#include <QSlider>
#include <QString>
#include <QFuture>
//#include <QtWidgets/QProgressBar>
#include <QProgressBar>
#include<list>
#include "user.h"
#include "direct.h"
#include "door.h"

// Car class

class Car : public QObject {
Q_OBJECT


private:
    QString name;
    int minFloor;
    int maxFloor;
    int currentFloor;
    int destinationFloor;               // destination floor
    Direction direction;                 // direction
    Door door;                           // door
    std::size_t capacity;                //capacity
    std::list<User *> users;              // List of user
    bool isReturning;                    // Flag for car is on return way
    unsigned long waitDuration;          // Time that Car waits between floor and when opening/closing doors

    QProgressBar *slider;                // Visual Car.
    QFuture<void> future;                // Future ride


    void checkIfAnyUserIsInDestination(); //  Handles opening and closing door

    void checkIfAnyUserWantsGetIn();

    int getLowestDestinationFloor();  //lowest destination floor

    int getHighestDestinationFloor(); //return highest destination floor

    int getLowestWaitingFloor(); //return lowest waiting floor

    int getHighestWaitingFloor(); //return highest waiting floor

    void updateDestinationFloor();


public:
     // Car constructor
    Car();

    Car(QString name, int minFloor, int maxFloor, QProgressBar *slider, unsigned long waitDuration = 1000);

    void move(); //start ride

    QString getName();

    int getMinFloor();

    int getMaxFloor();

    int getCurrentFloor(); //return current floor

    Door getDoor();

    void rerender(int maxFloor); // visulation

    Direction getDirection();

    void addUser(User *user); // Adds users to Car

    bool canHandle(User *user); // check if car handle user

    void stop(); // future ride

    void overload();

signals:


    void updateView();  // view of Car.

    void finishedTransfer();  //Car finishes transfer

    void pickedUpCar();
};
#endif // CAR_H
