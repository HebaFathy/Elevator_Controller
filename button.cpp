#include "car.h"
#include "button.h"
#include "user.h"
#include "controller.h"

void Button ::switchLight(){
    //button.setStyleSheet("QPushButton:pressed { background-color: red }");
}

void Car_Button::callCar(int from, int to) {
    auto *user = new User(from, to);
   // users.push_front(user);
    qInfo() << "[BUT]" << "Called Car:" << from << "->" << to << "[UsersCount:"  << "]";

}
