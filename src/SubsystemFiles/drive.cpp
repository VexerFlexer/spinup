/*
#include "main.h"
#include "pros/misc.h"
#include "subsystemHeaders/globals.hpp"

//helper funtions

void setDrive(int left, int right){
   left_back.move(left);
    left_front.move(left);
    right_back.move(right);
    right_front.move(right); 
}

void Drive4mArcade() {
    int left_joy = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    int right_joy = master.get_analog( pros::E_CONTROLLER_ANALOG_RIGHT_Y);

    if (abs(left_joy) < 10){
        left_joy = 0;
    }
    if (abs(right_joy) < 10){
        right_joy = 0;
    }

    setDrive(left_joy,right_joy);



}
*/