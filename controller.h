#ifndef Controller_H
#define Controller_H

#include "car.h"

#include <QMainWindow>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <list>

namespace Ui {
    class Controller;
}

// manages Cars and it's users.

class Controller : public QMainWindow {
Q_OBJECT

private:
    Ui::Controller *ui;
    std::list<Car *> Cars;                           // List of Cars available
    std::list<QPushButton *> buttonsEntries;       //list of buttons
    std::list<QLabel *> labels;                   // list of labels  to display number of waiting users
    std::list<User *> users;                    // list of users that was handled within controller.*/
    unsigned long waitDuration;                 //Time that Car waits between floor and when opening/closing doors.

    int maxFloorOverall = GROUND_FLOOR_NUMBER;     // Maximum floor

    void chooseDirection();

    void handleUsers(); //waiting users

    void rerenderCars();

    void rerenderCallButtonsAndLabels();

    void openSettings();

    int control(int floor);

    void timeout();

public:

    explicit Controller(unsigned long waitDuration = 600, QWidget *parent = nullptr);


    ~Controller() override; //destructor

    static const int GROUND_FLOOR_NUMBER = 0; //Ground floor


    Car* addCar(int minFloor, int maxFloor); // return created Car


    void callCar(int from, int to); //calls car

    void alert(QString);  //emergency alert

    void emergencyMove(); //for emergency move car to safe floor


signals:

    void userRequestedCar(); // user requested Car.


public slots:

    void help();

    void senseFire();

    void recievePowerOutSignal();

     void doorBlock();

     void overweight();
};

#endif // Controller_H
