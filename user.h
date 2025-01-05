#ifndef USER_H
#define USER_H

#include <QtCore/QDebug>
#include "ostream"
#include "direct.h"

enum UserStatus {NEW, WAITING_CAR, IN_CAR, DONE};

//  User Class design
class User {

private:
    UserStatus status;   //status
    int waitFloor;            // waiting floor
    int destinationFloor;     //destination floor

public:

    User(int waitFloor, int destinationFloor); //constructor

    void goToCar(); // user is using car

    void done();   // user finish request

    void carAllocated(); // car is allocated

    Direction getDirection(); // cars's direction

    bool isWaiting();  // user is waiting

    bool isWaiting(int floor); // user is waiting on specified floor

    bool isInCar();  // check if user is in car

    bool hasNotAllocatedCar(); // no allocation for car

    bool isDone(); // ride is done

    int getWaitFloor();  // return waiting floor

    int getDestinationFloor(); // return destination floor

    friend bool operator==(const User & lhs, const User & rhs);
    friend QDebug& operator<< (QDebug& stream, const User& p);
};

#endif // USER_H
