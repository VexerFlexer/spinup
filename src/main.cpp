#include "main.h"
#include "pros/misc.h"
#include "pros/motors.h"
#include <algorithm>
#include <chrono>
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
#define INDEXER_PORT2 'G'
#define INTAKE_DEFAULT_VELOCITY 375
#define FLYWHEEL_DEFAULT_SPEED 350
#define FLYWHEEL_STEP_SIZE 25
#define FLYWHEEL_THRESHOLD 0.10
#define INTAKE_STEP_SIZE 20
#define SHOOTING_TIME_MS 100
#define EXPANSION_PORT 'F'
#define BUTTON_TRIPPLE_SHOT DIGITAL_X
#define BUTTON_FLYWHEEL_DOWN DIGITAL_LEFT
#define BUTTON_FLYWHEEL_UP DIGITAL_RIGHT
#define BUTTON_FLYWHEEL_SET DIGITAL_L1
#define BUTTON_INDEXER DIGITAL_L2

using namespace pros;
using namespace std;
using namespace std::chrono;

typedef enum { 
  spinup,
  ready,
  shooting,
  shot
} shooter_state_t;

bool flywheel_status = false;
int flywheel_speed = FLYWHEEL_DEFAULT_SPEED;
int intake_status = 0;
int intake_speed = INTAKE_DEFAULT_VELOCITY;
bool indexer_status = false;
int flywheel_spin = 0;
int disk_counter = 0;
int line = 0;
long timediff = 0;

system_clock::time_point indexer_time =  system_clock::now();
shooter_state_t shooter_state = spinup; 

auto time_start = std::chrono::system_clock::now();
pros::Motor left_back (LEFT_BACK_PORT,true);
pros::Motor left_front (LEFT_FRONT_PORT);
pros::Motor right_back (RIGHT_BACK_PORT);
pros::Motor right_front (RIGHT_FRONT_PORT, true); // reversed

pros::Motor flywheel1 (FLYWHEEL_PORT, MOTOR_GEARSET_06,true);
pros::Motor flywheel2 (FLYWHEEL_PORT2, MOTOR_GEARSET_06,false);
pros::Motor intake (INTAKE_PORT1, MOTOR_GEARSET_06, true);

//pros::ADIDigitalOut expansion2 (EXPANSION_PORT2);
pros::ADIDigitalOut indexer (INDEXER_PORT);
pros::ADIDigitalOut indexer2 (INDEXER_PORT2);
pros::ADIDigitalOut expansion (EXPANSION_PORT);
//pros::ADIDigitalOut blocker (BLOCKER_PORT);
pros::Controller master (CONTROLLER_MASTER);

bool motor_threshold(pros::Motor & motor) {
  return abs(1.0 - motor.get_actual_velocity() / (double)motor.get_target_velocity()) < FLYWHEEL_THRESHOLD;
}



void setIndexer(bool expand) {
  indexer.set_value(expand);
  indexer2.set_value(!expand);
  indexer_time = system_clock::now();
}



void update_display() {
  switch(line) {
    case 0: if (master.print(line, 0, "%3d %3.0f %3.0f", flywheel_speed, 
        flywheel1.get_actual_velocity(), flywheel2.get_actual_velocity()) == 1)
          line ++;
        break;
    case 1: if (master.print(line, 0, "delta: %3.0f %3.0wf", flywheel1.get_target_velocity() - flywheel1.get_actual_velocity(),
      flywheel2.get_target_velocity() - flywheel2.get_actual_velocity()) == 1)
        line++;
      break;
    case 2: if (master.print(line, 0, "STATE: %d, %d, %d, %d",
      shooter_state, motor_threshold(flywheel1), motor_threshold(flywheel2), timediff) == 1)
      line++;
    break;
  }
  line = line % 3;
}
void pid_tuning() {
  double target_velocity = 75;
  double kP = 10;
  double kI = 0;
  double kD = 0;
  double derivative = 0;
  double integral = 0;
  double prev_error = 400;
  while(true) {
    double error = target_velocity*2 - (flywheel1.get_actual_velocity() + flywheel2.get_actual_velocity());
    integral = integral + error;
    derivative = error - prev_error;
    prev_error = error;
    auto new_velocity = (error*kP + integral*kI + derivative*kD);
    flywheel1.move(new_velocity);
    flywheel2.move(new_velocity);
    update_display();
    pros::delay(15);
  }
}

void opcontrol() {
  expansion.set_value(false);
  if (master.get_digital_new_press(DIGITAL_Y)){
    pid_tuning();
  }

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
    if (master.get_digital_new_press(BUTTON_FLYWHEEL_DOWN)) {
      flywheel_speed = std::max(flywheel_speed - FLYWHEEL_STEP_SIZE, 50);
    }

    if (master.get_digital_new_press(BUTTON_FLYWHEEL_UP)) {
      flywheel_speed = std::min(flywheel_speed + FLYWHEEL_STEP_SIZE, 600);
    }

    flywheel1.move(flywheel_spin * 127 / 600);
    flywheel2.move(flywheel_spin * 127 / 600);

    if (master.get_digital_new_press(BUTTON_FLYWHEEL_SET)) {
      if (flywheel_spin==0){
        flywheel_spin = flywheel_speed;
      }else{
        flywheel_spin = 0;
      }
    }

    //intake
    if (master.get_digital_new_press(DIGITAL_R2)) {
      switch(intake_status) {
        case 0: intake_status = -1; break;
        case -1: intake_status = 0; break;
      }
    }

    if (master.get_digital_new_press(DIGITAL_R1)) {
      switch(intake_status) {
        case 1: intake_status = 0; break;
        case 0: intake_status = 1; break;
      }
    }

    // -1 reverse, 0 stop, 1 fwd
    intake.move_velocity(intake_status * INTAKE_DEFAULT_VELOCITY);

    // indexer, only set if not in auto-shooting state
    if (shooter_state == spinup || shooter_state == ready) {
      if(master.get_digital(BUTTON_INDEXER)) {
        pros::delay(20);
        indexer.set_value(true);
        indexer2.set_value(false);
      }
      if(!master.get_digital(BUTTON_INDEXER)) {
        pros::delay(20);
        indexer.set_value(false);
        indexer2.set_value(true);
      }
    }
    //expansion
    if (master.get_digital_new_press(DIGITAL_B)){
      expansion.set_value(true);
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
        setIndexer(true);
      }
      break;
    case shooting: {
        timediff = chrono::duration_cast<chrono::milliseconds>(
          system_clock::now().time_since_epoch() - indexer_time.time_since_epoch()).count();
        if (timediff > SHOOTING_TIME_MS) {
          disk_counter--;
          setIndexer(false);
          shooter_state = shot;
        }
      } break;
    case shot:
      setIndexer(false);
      if (disk_counter == 0) {
        shooter_state = spinup;
      } else if (motor_threshold(flywheel1) && motor_threshold(flywheel2)) {
        shooter_state = shooting;
      }
      break;
    }
  
    update_display();

    pros::delay(2);
  }
}
 
 

