/*
include "main.h"
//variable defintition

int intake_status = 0;
bool intake_slow = false;

//helpers

int get_intake_speed(int intake_status){
  if (intake_slow){
    return INTAKE_SLOW_VELOCITY*intake_status;
  }else
    return INTAKE_DEFAULT_VELOCITY * intake_status;
}

void setIntake(int power){
    intake.move(power);
}



//main


void intake1m(){

    if (master.get_digital_new_press(DIGITAL_R2)) {
        //switch(intake_status) {
        //case 0: intake_status = -1; break;
        //case -1: intake_status = 0; break;
        //}

        //cole is more comfortable with if else statements but the switch case works fine
        
        if (intake_status == -1){
            intake_status = 0;
        }else if (intake_status == -1) {
            intake_status = 0;
        }
    }

    if (master.get_digital_new_press(DIGITAL_R1)) {
        //
        switch(intake_status) {
        case 1: intake_status = 0; break;
        case 0: intake_status = 1; break;
        }
        //

        //cole is more comfortable with if else statements but the switch case works fine

        if (intake_status == 1){
            intake_status = 0;
        }else if (intake_status == 0) {
            intake_status = 1;
        }
    }

    // -1 reverse, 0 stop, 1 fwd

    intake.move_velocity(get_intake_speed(intake_status));


    //rotate intake slowly
    if (master.get_digital_new_press(DIGITAL_A)) {
        intake_slow = !intake_slow;
    }
}
*/