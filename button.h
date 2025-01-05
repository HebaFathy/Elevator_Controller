#ifndef BUTTON_H
#define BUTTON_H
class Button{
    bool light= false;

public:
    //toggle button light
    void switchLight();
};


class Car_Button: public Button{
    int floor_number;

public:
    // call elevator car from any floor
    void callCar(int , int);
};



class Floor_Button: public Button{
    int floor_number;

public:
    // call elevator car from any floor
    void pressFloorButton(int floor_num);
    // press help button
    void help();
};



#endif // BUTTON_H
