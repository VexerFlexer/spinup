#include "main.h"
#include "pros/misc.h"
#include "pros/motors.h"
#include <algorithm>
#include <cstdint>
#include <type_traits>

#define LEFT_BACK_PORT 3
#define LEFT_FRONT_PORT 4
#define RIGHT_BACK_PORT 1
#define RIGHT_FRONT_PORT 12
#define FLYWHEEL_PORT 5
#define FLYWHEEL_PORT2 6
#define INTAKE_PORT 7
#define EXPANSION_PORT 'A'
#define INDEXER_PORT 'B'
#define BLOCKER_PORT 'C'
#define STEP_SIZE 20
using namespace pros;
using namespace std;

typedef enum {
  wait,
  expanding,
  retracting
} pneumatic;

void opcontrol() {
  bool flywheel_status = false;
  int intake_status = 0;
  int flywheel_speed = 600;
  pneumatic state = wait;
  bool flywheel_up_pressed = false;
  bool flywheel_down_pressed = false;
  bool intake_up_pressed = false;
  bool intake_down_pressed = false; 
  bool indexer_status = false;
  int flywheel_spin = 0;
  auto time_start = std::chrono::system_clock::now();
  pros::Motor left_back (LEFT_BACK_PORT,true);
  pros::Motor left_front (LEFT_FRONT_PORT); 
  pros::Motor right_back (RIGHT_BACK_PORT);
  pros::Motor right_front (RIGHT_FRONT_PORT, true); // reversed
  pros::Motor flywheel1 (FLYWHEEL_PORT, MOTOR_GEARSET_06,true);
  pros::Motor flywheel2 (FLYWHEEL_PORT2, MOTOR_GEARSET_06,true);
  pros::Motor intake (INTAKE_PORT, MOTOR_GEARSET_06);
  pros::ADIDigitalOut expansion (EXPANSION_PORT);
  pros::ADIDigitalOut indexer (INDEXER_PORT);
  pros::ADIDigitalOut blocker (BLOCKER_PORT);
  pros::Controller master (CONTROLLER_MASTER);

  expansion.set_value(true);

  while (true) {
	  std::int8_t left_joystick = master.get_analog(ANALOG_LEFT_Y);
	  std::int8_t right_joystick = master.get_analog(ANALOG_RIGHT_Y);
    left_back.move(left_joystick);
	  left_front.move(left_joystick);
    right_front.move(right_joystick);
	  right_back.move(right_joystick);
    
    //finished with tank drive
    if (master.get_digital(DIGITAL_LEFT)) {
      if (!flywheel_down_pressed)
        flywheel_speed = std::max(flywheel_speed - STEP_SIZE, 20);
      flywheel_down_pressed = true;
    } else {
      flywheel_down_pressed = false;
    }

    if (master.get_digital(DIGITAL_RIGHT)) {
      if (!flywheel_up_pressed)
        flywheel_speed = std::min(flywheel_speed + STEP_SIZE, 600);
      flywheel_up_pressed = true;
    } else {
      flywheel_up_pressed = false;
    }
    flywheel1.move_velocity(flywheel_spin);
    flywheel2.move_velocity(flywheel_spin);

    if (master.get_digital(DIGITAL_UP)) {
      flywheel_spin=flywheel_speed;
    }

    if (master.get_digital(DIGITAL_DOWN)) {
      flywheel_spin=0; 
    }

    //intake
    if (master.get_digital(DIGITAL_R2)) {
      if (!intake_down_pressed){
        if (intake_status == -1){
          intake_status = 0;
        }
        else if (intake_status == 0){
          intake_status = -1;
        }
         intake_down_pressed = true;
      }
     
    } else {
      intake_down_pressed = false;
    }
    if (master.get_digital(DIGITAL_R1)) {
      if (!intake_up_pressed){
        if (intake_status == 1){
          intake_status = 0;
        }
        else if (intake_status == 0){
          intake_status = 1;
        }
        intake_up_pressed = true;
      }
    } else {
      intake_up_pressed = false;
    }


    if (intake_status==1) {
      intake.move_velocity(10000);
    }
    if (intake_status==0) {
      intake.move_velocity(0);
    }
    if (intake_status==-1) {
      intake.move_velocity(-100000);
    }
    
    //indexer
    if(master.get_digital(DIGITAL_A)) {
      blocker.set_value(false);
      pros::delay(20);
      indexer.set_value(true);
    }
    if(!master.get_digital(DIGITAL_A)) {
      blocker.set_value(true);
      pros::delay(20);
      indexer.set_value(false);
    }
    //expansion

    if(master.get_digital(DIGITAL_B)) {
      expansion.set_value(false);
    }
    
    //print
    master.print(0,0,"%5d %d",flywheel_speed, state);

  
    pros::delay(2);

  }
}