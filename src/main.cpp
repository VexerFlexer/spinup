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
#define STEP_SIZE 20

typedef enum {
  wait,
  expanding,
  retracting
} pneumatic;

void opcontrol() {
  bool flywheel_status = false;
  bool intake_status = false;
  int flywheel_speed = 100;
  pneumatic state = wait;
  bool flywheel_up_pressed = false;
  bool flywheel_down_pressed = false;
  bool indexer_status = false;
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
  pros::Controller master (CONTROLLER_MASTER);


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

    if (master.get_digital(DIGITAL_UP)) {
      flywheel1.move_velocity(flywheel_speed);
      flywheel2.move_velocity(flywheel_speed);
    }
    if (master.get_digital(DIGITAL_DOWN)) {
      flywheel1.move_velocity(0);
      flywheel2.move_velocity(0);
    }
    if (master.get_digital(DIGITAL_X)) {
      intake.move_velocity(10000);
    }
    if (master.get_digital(DIGITAL_B)) {
      intake.move_velocity(0);
    }
    
    //statemachine
    auto current = std::chrono::system_clock::now();

    if(state == wait && master.get_digital(DIGITAL_A)) {
      state = expanding;
      time_start = current;
      indexer.set_value(true);
    }
    
    std::chrono::duration<double> elapsed_seconds = current - time_start;

    if(state == expanding && elapsed_seconds.count() > 0.5){
      state = retracting;
      time_start = current;
      indexer.set_value(true);
    }

    if(state == retracting && elapsed_seconds.count() > 0.5){
      state = wait;
    }
    master.print(0,0,"%5d %d",flywheel_speed, state);

  
    pros::delay(2);

  }
}