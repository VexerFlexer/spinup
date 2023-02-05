#include "main.h"
/*
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/optical.h"
#include "pros/optical.hpp"
#include "pros/rtos.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <type_traits> 
 */
#define REDBLUERATIO 1.5
#define LEFT_BACK_PORT 11
#define LEFT_FRONT_PORT 1
#define RIGHT_BACK_PORT 20
#define RIGHT_FRONT_PORT 10
#define FLYWHEEL_PORT 18
#define FLYWHEEL_PORT2 17
#define INTAKE_PORT1 2
#define RED 1
#define BLUE 2
#define OPTICAL_PORT 9
#define gyro_port 6
#define target_color 1
// #define INTAKE_PORT2 20

#define INDEXER_PORT 'H'
#define INDEXER_PORT2 'G'
#define INTAKE_DEFAULT_VELOCITY 375
#define INTAKE_SLOW_VELOCITY 150
#define FLYWHEEL_DEFAULT_SPEED 350
#define FLYWHEEL_STEP_SIZE 25
#define FLYWHEEL_THRESHOLD 0.10
#define INTAKE_STEP_SIZE 2f0
#define SHOOTING_TIME_MS 100
#define EXPANSION_PORT 'G'
#define EXPANSION_PORT2 'H'
#define EXPANSION2_PORT1 'F'
#define BUTTON_TRIPPLE_SHOT DIGITAL_X
#define BUTTON_FLYWHEEL_DOWN DIGITAL_LEFT
#define BUTTON_FLYWHEEL_UP DIGITAL_RIGHT
#define BUTTON_FLYWHEEL_SET DIGITAL_L1
#define BUTTON_INDEXER DIGITAL_L2
#define VISION_PORT 15
using namespace pros;
using namespace std;
using namespace std::chrono;
using namespace pros;
typedef enum { 
  spinup,
  ready,
  shooting,
  shot
} shooter_state_t;

bool flywheel_status = false;
bool intake_slow = false;
int flywheel_speed = FLYWHEEL_DEFAULT_SPEED;
int intake_status = 0;
int intake_speed = INTAKE_DEFAULT_VELOCITY;
bool indexer_status = false;
int flywheel_spin = 0;
int disk_counter = 0;
int line = 0;
long timediff = 0;
bool team_red = true;
int roller_vision_status = 0;


pros::c::optical_rgb_s_t rgb_vals;

system_clock::time_point indexer_time =  system_clock::now();
system_clock::time_point roller_time =  system_clock::now();
shooter_state_t shooter_state = spinup; 

auto time_start = std::chrono::system_clock::now();
pros::Motor left_back (LEFT_BACK_PORT,true);
pros::Motor left_front (LEFT_FRONT_PORT,pros::motor_brake_mode_e());
pros::Motor right_back (RIGHT_BACK_PORT,pros::motor_brake_mode_e());
pros::Motor right_front (RIGHT_FRONT_PORT, true); // reversed

pros::Motor flywheel1 (FLYWHEEL_PORT, MOTOR_GEARSET_06,true);
pros::Motor flywheel2 (FLYWHEEL_PORT2, MOTOR_GEARSET_06,false);
pros::Motor intake (INTAKE_PORT1, MOTOR_GEARSET_06, true);
//pros::Motor indexer (INDEXER, MOTOR_GEARSET_06);
pros::Vision vision_sensor (VISION_PORT);
pros::vision_signature_s_t ROL_RED =
    pros::Vision::signature_from_utility(1, 3375, 6431, 4902, -655, 511, -72, 3.000, 0);
pros::vision_signature_s_t ROL_BLUE =
    pros::Vision::signature_from_utility(2, -2815, -893, -1854, 4045, 6911, 5478, 3.000, 0);
pros::ADIDigitalOut expansion (EXPANSION_PORT);
pros::ADIDigitalOut expansion2 (EXPANSION_PORT2);
pros::ADIDigitalOut expansion3 (EXPANSION2_PORT1);


pros::ADIDigitalOut indexer (INDEXER_PORT);
pros::ADIDigitalOut indexer2 (INDEXER_PORT2);


pros::Imu gyro(gyro_port);
//pros::ADIDigitalOut blocker (BLOCKER_PORT);
pros::Controller master (CONTROLLER_MASTER);

bool motor_threshold(pros::Motor & motor) {
  return abs(1.0 - motor.get_actual_velocity() / (double)motor.get_target_velocity()) < FLYWHEEL_THRESHOLD;
}
int get_intake_speed(int intake_status){
  if (intake_slow){
    return INTAKE_SLOW_VELOCITY*intake_status;
  }else
    return INTAKE_DEFAULT_VELOCITY * intake_status;
}
void setDrive(int left, int right){
   left_back.move(left);
    left_front.move(left);
    right_back.move(right);
    right_front.move(right); 
}

void resetDriveEncoders(){
  left_back.tare_position();
  left_front.tare_position();
  right_back.tare_position();
  right_front.tare_position();

}

double avgDriveEncoderVal(){
  return fabs(left_front.get_position()) +
  fabs(left_back.get_position()) +
  fabs(right_front.get_position()) +
  fabs(right_back.get_position()) / 4;
}



void translate(int units, int voltage){
  //define a direction based on input
  int direction = abs(units)/units;//-1 or 1
  //reset motor incoders
  gyro.reset();
  resetDriveEncoders();
  //drive forward until levels reached
  while(avgDriveEncoderVal()<abs(units)){
    setDrive(voltage*direction+gyro.get_yaw(), voltage*direction-gyro.get_rotation());
    pros::delay(10); 
  }
  //brake
  /*
  setDrive(-10, -10);
  pros::delay(50);
  //set drive to neutral
  setDrive(0, 0);
  */
  //i dont think i need that
}

void rotate(int degrees, int voltage){
  //define direction
  int direction = abs(degrees)/degrees;
  //reset gyro
  gyro.reset();
  //turn until units reached
  while(fabs(gyro.get_rotation())<abs(degrees)){
    setDrive(-voltage*direction, voltage*direction);
    pros::delay(10);
  }
  /*
  pros::delay(100);
  if (fabs(gyro.get_value()) > abs(degrees*10)){
    setDrive(0.5 * voltage * direction, 0.5 * -voltage * direction);
    while(fabs(gyro.get_value()) > abs(degrees*10)){
      pros::delay(10);
    }
  }
  else if (fabs(gyro.get_value()) < abs(degrees * 10)){
    setDrive(0.5*-voltage*direction, 0.5 * -voltage * direction);
    while(fabs(gyro.get_value()) > abs(degrees * 10)){
      pros::delay(10);
    }
  }
  */

  
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
    
    // case 2: if (master.print(line, 0, "STATE: %d, %d, %d, %d",
    //   shooter_state, motor_threshold(flywheel1), motor_threshold(flywheel2), timediff) == 1)
    //   line++;
    case 2: if (master.print(line, 0,  "%.1d,%d, %.1f, %.1f, %.1f",
      roller_vision_status,team_red, rgb_vals.red,rgb_vals.green,rgb_vals.blue ) == 1)
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
//far auton
void autonomous1(){
  gyro.reset();
  left_back.move(40);
  left_front.move(40);
  right_front.move(40);
  right_back.move(40);

  //increase this delay to move more
  pros::delay(1900);


  left_back.move(0);
  left_front.move(0);
  right_front.move(0);
  right_back.move(0);

  pros::delay(100);
  setDrive(100, -100);

  //increase this delay to turn more
  pros::delay(600);


  setDrive(50,50);
  pros::delay(1500);
  intake.move(-150);
  pros::delay(100);
  intake.move_velocity(0);
  setDrive(0,0);
  //back up
  //translate(10, 200);
  //turn to goal
  //rotate(45, 200);
  //go forward a bit
  //translate(100, 200);

  /*
  //shoot two shots
  flywheel1.move(600);
  flywheel2.move(600);
  pros::delay(6000);
  indexer.set_value(true);
  indexer2.set_value(false);
  pros::delay(200);
  indexer.set_value(false); 
  indexer2.set_value(true);
  pros::delay(5000);
  indexer.set_value(true);
  indexer2.set_value(false);
  pros::delay(200);
  indexer.set_value(false);
  indexer2.set_value(true);
  */
}

//close auton
void autonomous(){
  left_back.move(40);
  left_front.move(40);
  right_front.move(40);
  right_back.move(40);
  pros::delay(1000);

  intake.move(-150);
  pros::delay(100);
  intake.move_velocity(0);
  left_back.move(0);
  left_front.move(0);
  right_front.move(0);
  right_back.move(0);

/*
  pros::delay(100);
  setDrive(100, -100);
  pros::delay(100);
  setDrive(0,0);


  flywheel1.move(600);
  flywheel2.move(600);
  pros::delay(6000);
  indexer.set_value(true);
  indexer2.set_value(false);
  pros::delay(200);
  indexer.set_value(false); 
  indexer2.set_value(true);
  pros::delay(5000);
  indexer.set_value(true);
  indexer2.set_value(false);
  pros::delay(200);
  indexer.set_value(false);
  indexer2.set_value(true);
 */
}
//prog skills
void autonomous2(){
   left_back.move(40);
  left_front.move(40);
  right_front.move(40);
  right_back.move(40);
  pros::delay(1000);

  intake.move(-150);
  pros::delay(100);
  intake.move_velocity(0);
  left_back.move(0);
  left_front.move(0);
  right_front.move(0);
  right_back.move(0);

  pros::delay(100);
  setDrive(100, -100);
  pros::delay(100);
  setDrive(0,0);


  flywheel1.move(600);
  flywheel2.move(600);
  pros::delay(6000);
  indexer.set_value(true);
  indexer2.set_value(false);
  pros::delay(200);
  indexer.set_value(false); 
  indexer2.set_value(true);
  pros::delay(5000);
  indexer.set_value(true);
  indexer2.set_value(false);
  pros::delay(200);
  indexer.set_value(false);
  indexer2.set_value(true);
 
  pros::delay(400);
  setDrive(-100, 100);
  pros::delay(600);
  
  expansion.set_value(true);
  expansion2.set_value(true);
  expansion3.set_value(true);
setDrive(0, 0);


}

void opcontrol() {
  expansion.set_value(false);
  expansion2.set_value(false);
  expansion3.set_value(false);

  //auton try

  master.clear();

  left_back.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  left_front.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  right_back.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
  right_front.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
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
    //roller camera

    if (master.get_digital_new_press(DIGITAL_X)) {
      team_red= ! team_red;
    }

    rgb_vals = pros::c::optical_get_rgb(OPTICAL_PORT);
    if (master.get_digital_new_press(DIGITAL_Y)) {
      roller_vision_status=1;
    }
    if (!team_red){
      if (rgb_vals.red/rgb_vals.blue < REDBLUERATIO &&roller_vision_status ==1){
        intake.move(50);
      }
      if (rgb_vals.red/rgb_vals.blue> REDBLUERATIO &&roller_vision_status ==1){
        roller_vision_status = 0;
        intake.move(0);
      }
    }

    if (team_red){
      if (rgb_vals.red/rgb_vals.blue > REDBLUERATIO&&roller_vision_status ==1){
        intake.move(50);
      }
      if (rgb_vals.red/rgb_vals.blue< REDBLUERATIO &&roller_vision_status ==1){
        roller_vision_status = 0;
        intake.move(0);
      }
    }



    
    
    // -1 reverse, 0 stop, 1 fwd
    if (roller_vision_status!= 1){
      intake.move_velocity(get_intake_speed(intake_status));
    }
  
    //rotate intake slowly
    if (master.get_digital_new_press(DIGITAL_A)) {
      intake_slow = !intake_slow;
    }
    


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
      expansion2.set_value(true);
      expansion3.set_value(true);
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
 
 








































































/*

void opcontrol1(){
  while(true){
    //code to control the drive
    Drive4mArcade();
    //code to control intake
    intake1m();
    //code to control flywheel
    flywheel2m();
    indexer2solaniond();
    //code to control expansion
    //expand();
    pros::delay(10);
  }
}



















void redleft(){
  
}
void redright(){

}
void blueleft(){

}
void blueright(){

}

void autonomous1(){
  setIndexer(false);

  left_back.move(40);
  left_front.move(40);
  right_front.move(40);
  right_back.move(40);
  pros::delay(1000);

  intake.move(-150);
  pros::delay(100);
  intake.move_velocity(0);
  left_back.move(0);
  left_front.move(0);
  right_front.move(0);
  right_back.move(0);
  //turn to goal


}
*/