/*
#include "main.h"
#include "pros/misc.h"
#include "pros/motors.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <type_traits>
 
using namespace pros;
using namespace std;
using namespace std::chrono;

typedef enum { 
  spinup,
  ready,
  shooting,
  shot
} shooter_state_t;


//Motors
pros::Motor left_back (LEFT_BACK_PORT,true);
pros::Motor left_front (LEFT_FRONT_PORT,pros::motor_brake_mode_e());
pros::Motor right_back (RIGHT_BACK_PORT,pros::motor_brake_mode_e());
pros::Motor right_front (RIGHT_FRONT_PORT, true); // reversed
pros::Motor flywheel (FLYWHEEL_PORT, MOTOR_GEARSET_06,true);
pros::Motor intake (INTAKE_PORT1, MOTOR_GEARSET_06, true);
//pneumatics
pros::ADIDigitalOut indexer (INDEXER_PORT);
pros::ADIDigitalOut indexer2 (INDEXER_PORT2);
pros::ADIDigitalOut expansion (EXPANSION_PORT);

//controller
pros::Controller master (pros::E_CONTROLLER_MASTER);
*/