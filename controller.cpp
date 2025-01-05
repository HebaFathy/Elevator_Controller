#include <QtCore/QThread>
#include <QtCore/QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets/QLCDNumber>
#include <QString>
#include "controller.h"
#include "ui_controller.h"
#include "car.h"
#include "button.h"
#include "mainwindow.h"
#include <qmessagebox.h>

//Constructor
Controller::Controller(unsigned long waitDuration, QWidget *parent) : QMainWindow(parent), ui(new Ui::Controller) {
    ui->setupUi(this);
    this->setFixedSize(1250, 732);
    this->statusBar()->setSizeGripEnabled(false);
    this->waitDuration = waitDuration;

    connect(ui->settingsButton, &QAction::triggered, this, [=] { openSettings(); });

    connect(ui->help, SIGNAL (clicked()), this, SLOT(help()));
    connect(ui->fire, SIGNAL (clicked()), this, SLOT(senseFire()));
    connect(ui->powerOut, SIGNAL (clicked()), this, SLOT(recievePowerOutSignal()));
    connect(ui->overload, SIGNAL (clicked()), this, SLOT(overweight()));
    connect(ui->doorBlock, SIGNAL (clicked()), this, SLOT(doorBlock()));
    //connect(ui->btn, SIGNAL(clicked(bool)), this,SLOT(on_btn_clicked()));


    addCar(0, 5); //Add car for  test
    addCar(0, 5); //another car for test
}

Controller::~Controller() {
    delete ui;
}

//return created car
Car *Controller::addCar(int minFloor, int maxFloor) {
    this->maxFloorOverall = this->maxFloorOverall < maxFloor ? maxFloor : this->maxFloorOverall;

    auto lay = new QVBoxLayout();
    auto lcd = new QLCDNumber();
    lcd->setFixedHeight(40);
    lcd->setFixedWidth(25);
    lcd->setDigitCount(2);
    lcd->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
    lay->addWidget(lcd);
    auto *slider = new QProgressBar();
    lay->addWidget(slider);
    ui->CarsView->addLayout(lay);

    auto car = new Car(QString::number(Cars.size()), minFloor, maxFloor, slider, waitDuration);
    connect(car, &Car::updateView, this, [=] { slider->setValue(car->getCurrentFloor()); });
    connect(car, &Car::updateView, this, [=] { lcd->display(car->getCurrentFloor()); });
    connect(car, &Car::finishedTransfer, this, [=] { handleUsers(); });
    connect(car, &Car::pickedUpCar, this, [=] { handleUsers(); });

    this->Cars.push_front(car);

    rerenderCars();
    rerenderCallButtonsAndLabels();
    return car;
}

int Controller::control(int floor) {
    int count = 0;
    for (auto user : users) {
        if (user->isWaiting(floor)) {
            count++;
        }
    }
    return count;
}



void Controller::callCar(int from, int to) {
    auto *user = new User(from, to);
    users.push_front(user);
    qInfo() << "[BTN]" << "Called Car:" << from << "->" << to << "[UsersCount:" << users.size() << "]";
    handleUsers();
}


// call car with given floor
void Controller::rerenderCars() {
    for (auto &car : this->Cars) {
        car->rerender(this->maxFloorOverall);
    }
}

void Controller::rerenderCallButtonsAndLabels() {
    for (auto &i : buttonsEntries) {
        delete i;
    }

    buttonsEntries.clear();

    for (auto &l : labels) {
        delete l;
    }

    labels.clear();

    for (int fromFloor = maxFloorOverall; fromFloor >= GROUND_FLOOR_NUMBER; fromFloor--) {
        auto lay = new QHBoxLayout();
        lay->setObjectName("Layout" + QString::number(fromFloor));
        for (int toFloor = GROUND_FLOOR_NUMBER; toFloor <= maxFloorOverall; toFloor++) {
            auto *button = new QPushButton();
            button->setText(QString::number(fromFloor) + " -> " + QString::number(toFloor));
            button->setObjectName("Button" + QString::number(fromFloor) + "/" + QString::number(toFloor));
            button->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
            button->setDisabled(fromFloor == toFloor);
            lay->addWidget(button);
            connect(button, &QPushButton::clicked, this, [=] { callCar(fromFloor, toFloor); });
            buttonsEntries.push_front(button);
            button->setStyleSheet("QPushButton:pressed { background-color: red }");
        }
        ui->buttonsView->addLayout(lay);

        auto *label = new QLabel();
        label->setText(QString("0"));
        ui->waitingList->addWidget(label);
        labels.push_front(label);
        connect(this, &Controller::userRequestedCar, label, [=] {
            label->setText(QString::number(control(fromFloor)));
        });
    }

}


// waiting users
void Controller::handleUsers() {
    qInfo() << "[CON]" << "Invoked handleUsers()";
    userRequestedCar();
    for (auto user : users) {
        std::list<Car *> availableCars;
        std::copy_if(Cars.begin(), Cars.end(), std::back_inserter(availableCars),
                     [=](Car *e) { return e->canHandle(user); });

        if (user->hasNotAllocatedCar()) {
            if (!availableCars.empty()) {
                Car *closestCar = availableCars.front();
                for (auto &car : availableCars) {
                    int oldDistance = abs(closestCar->getCurrentFloor() - user->getWaitFloor());
                    int distance = abs(car->getCurrentFloor() - user->getWaitFloor());
                    if (distance < oldDistance) {
                        closestCar = car;
                    }
                }
                closestCar->addUser(user);
                user->carAllocated();
                qInfo() << "[CON]" << "Handled user" << *user;
            } else {
                qInfo() << "[CON]" << "No car left" << *user;
            }
        }
    }
}

void Controller::openSettings() {
    auto *dialog = new MainWindow(this);
    //dialog->exec();
}

// Saftey Features
//Alert for Emergency
void Controller:: alert(QString str)
{
    if (str=="F")   //fire
    {
         qInfo() << "[CON]" << "Fire Alarm moving to a safe place";
         QMessageBox::critical(nullptr, QObject::tr("Critical"),  QObject::tr("<h3>Fire<h>"));
    }
    else if(str=="O") //overload
    {
        qInfo() << "[CON]" << "Overload";
        QMessageBox::critical(nullptr, QObject::tr("Critical"),  QObject::tr("<h3>Overload<h3>"));
    }
    else if(str=="P") //power
    {
        qInfo() << "[CON]" << "Power Outage Moving to a safe floor";
        QMessageBox::critical(nullptr, QObject::tr("Critical"),  QObject::tr("<h3>Powerage<h3>"));
    }
    else if(str=="D") //door block
    {
        qInfo() << "[CON]" << "Door Blocked";
        QMessageBox::critical(nullptr, QObject::tr("Critical"),  QObject::tr("<h3>Door Block<h3>"));
    }
    else if(str=="H") //help
    {
        qInfo() << "[CON]" << "Calling BSS";
        QMessageBox::critical(nullptr, QObject::tr("Critical"),  QObject::tr("<h3>Calling Building Safety Services<h3>"));
    }

    qInfo()<< '\a';

}

//Timout
void Controller:: timeout()
{
    unsigned long waitDuration=1000;
    qInfo()<<"[CON]"<<"Waiting time";
     QThread::msleep(2 * waitDuration);
}

//Move car to safe floor
void Controller::emergencyMove()
{
    int floor_front= Cars.front()->getCurrentFloor();
    int floor_back= Cars.back()->getCurrentFloor();
    if(floor_front >0)
        callCar(floor_front,this->GROUND_FLOOR_NUMBER);
    if(floor_back >0)
        callCar(floor_back,this->GROUND_FLOOR_NUMBER);
}

//Recive powerOut signal
void Controller:: recievePowerOutSignal()
{
    ui->powerOut->setStyleSheet("QPushButton:pressed { background-color: red }");
    alert("P");
    emergencyMove();
}

//Fire case
void Controller:: senseFire()
{
    ui->fire->setStyleSheet("QPushButton:pressed { background-color: red }");
    alert("F");
    emergencyMove();
}

//User push help button
void Controller:: help()
{
    ui->help->setStyleSheet("QPushButton:pressed { background-color: red }");
    alert("H");
    timeout();
}

//door is blocked by any obstacle
void Controller:: doorBlock()
{
    ui->doorBlock->setStyleSheet("QPushButton:pressed { background-color: red }");
    alert("D");
    timeout();
    Cars.front()->getDoor().closeDoor(1000);
}

//Car overweight
void Controller::overweight()
{
    Cars.front()->overload();
}
