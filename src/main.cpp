#include "main.h"
#include "pros/misc.h"
#include "pros/motors.h"
#include <algorithm>
#include <cstdint>
#include <type_traits>
 
#define LEFT_BACK_PORT 11
#define LEFT_FRONT_PORT 1
#define RIGHT_BACK_PORT 20
#define RIGHT_FRONT_PORT 10
#define FLYWHEEL_PORT 18
#define FLYWHEEL_PORT2 17
#define INTAKE_PORT1 2
// #define INTAKE_PORT2 20
#define INDEXER_PORT 'H'
#define INTAKE_DEFAULT_VELOCITY 350
#define FLYWHEEL_DEFAULT_SPEED 400
#define FLYWHEEL_STEP_SIZE 25
#define FLYWHEEL_THRESHOLD 0.05
#define INTAKE_STEP_SIZE 20

#define BUTTON_TRIPPLE_SHOT DIGITAL_X
#define BUTTON_FLYWHEEL_DOWN DIGITAL_LEFT
#define BUTTON_FLYWHEEL_UP DIGITAL_RIGHT
#define BUTTON_FLYWHEEL_SET DIGITAL_L1
#define BUTTON_FLYHWEEL_STOP DIGITAL_L2
#define BUTTON_INDEXER DIGITAL_A

using namespace pros;
using namespace std;

typedef enum { 
  spinup,
  ready,
  shooting,
  shot
} shooter_state_t;

bool flywheel_status = false;
int flywheel_speed = FLYWHEEL_DEFAULT_SPEED;
bool flywheel_up_pressed = false;
bool flywheel_down_pressed = false;
bool intake_up_pressed = false;
bool intake_down_pressed = false;
int intake_status = 0;
int intake_speed = INTAKE_DEFAULT_VELOCITY;
bool indexer_status = false;
int flywheel_spin = 0;
int disk_counter = 0;
shooter_state_t shooter_state = spinup; 

auto time_start = std::chrono::system_clock::now();
pros::Motor left_back (LEFT_BACK_PORT,true);
pros::Motor left_front (LEFT_FRONT_PORT);
pros::Motor right_back (RIGHT_BACK_PORT);
pros::Motor right_front (RIGHT_FRONT_PORT, true); // reversed

pros::Motor flywheel1 (FLYWHEEL_PORT, MOTOR_GEARSET_06,true);
pros::Motor flywheel2 (FLYWHEEL_PORT2, MOTOR_GEARSET_06,false);
pros::Motor intake (INTAKE_PORT1, MOTOR_GEARSET_06);
//pros::ADIDigitalOut expansion (EXPANSION_PORT);
//pros::ADIDigitalOut expansion2 (EXPANSION_PORT2);
pros::ADIDigitalOut indexer (INDEXER_PORT);
//pros::ADIDigitalOut blocker (BLOCKER_PORT);
pros::Controller master (CONTROLLER_MASTER);

bool motor_threshold(pros::Motor & motor) {
  return abs(1.0 - motor.get_actual_velocity() / (double)motor.get_target_velocity()) < FLYWHEEL_THRESHOLD;
}

void opcontrol() {
 
 //expansion.set_value(true);
 master.clear();
 
 while (true) {
   std::int8_t left_joystick = master.get_analog(ANALOG_LEFT_Y);
   std::int8_t right_joystick = master.get_analog(ANALOG_RIGHT_Y);
   left_back.move(left_joystick);
   left_front.move(left_joystick);
   right_front.move(right_joystick);
   right_back.move(right_joystick);
 
   //finished with tank drive
   if (master.get_digital(BUTTON_FLYWHEEL_DOWN)) {
     if (!flywheel_down_pressed)
       flywheel_speed = std::max(flywheel_speed - FLYWHEEL_STEP_SIZE, 50);
     flywheel_down_pressed = true;
   } else {
     flywheel_down_pressed = false;
   }
 
   if (master.get_digital(BUTTON_FLYWHEEL_UP)) {
     if (!flywheel_up_pressed)
       flywheel_speed = std::min(flywheel_speed + FLYWHEEL_STEP_SIZE, 600);
     flywheel_up_pressed = true;
   } else {
     flywheel_up_pressed = false;
   }
    //flywheel_set(flywheel_spin,flywheel1,flywheel2);
  
   flywheel1.move(flywheel_spin * 127 / 600);
   flywheel2.move(flywheel_spin * 127 / 600);
 
   if (master.get_digital(BUTTON_FLYWHEEL_SET)) {
     flywheel_spin = flywheel_speed;
   }
 
   if (master.get_digital(BUTTON_FLYHWEEL_STOP)) {
     flywheel_spin = 0;
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

   // -1 reverse, 0 stop, 1 fwd
   intake.move_velocity(intake_status * 100);

   // indexer
   // TODO: remove delay
   if(master.get_digital(BUTTON_INDEXER)) {
     pros::delay(20);
     indexer.set_value(false);
   }
   if(!master.get_digital(BUTTON_INDEXER)) {
     pros::delay(20);
     indexer.set_value(true);
   }

   switch(shooter_state) {
    case spinup: 
      if (motor_threshold(flywheel1) && motor_threshold(flywheel2) && !master.get_digital(BUTTON_TRIPPLE_SHOT))
        shooter_state = ready;
      break;
    case ready:
      // if motors slowed down go back to spinup
      if (!(motor_threshold(flywheel1) && motor_threshold(flywheel2))) {
        shooter_state = spinup;
      } else if (master.get_digital(BUTTON_TRIPPLE_SHOT)) { // shoot
        shooter_state = shooting;
        disk_counter = 3;
        // TODO: activate pneumatics
      }
      break;
    case shooting:
      disk_counter--;
      // TODO: retract pneumatics
      break;
    case shot:
      if (disk_counter == 0) {
        shooter_state = spinup;
      } else if (motor_threshold(flywheel1) && motor_threshold(flywheel2))
        shooter_state = shooting;
      }
      break;
   }
 
   master.print(0, 0, "%3d %3.0f %3.0f", flywheel_speed, 
     flywheel1.get_actual_velocity(), flywheel2.get_actual_velocity());
   master.print(1, 0, "delta: %3.0f %3.0f", flywheel1.get_target_velocity() - flywheel1.get_actual_velocity(),
    flywheel2.get_target_velocity() - flywheel2.get_actual_velocity());
 
   pros::delay(2);
 
 }
}
 
 

