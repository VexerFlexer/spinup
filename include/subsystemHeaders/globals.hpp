/*
#pragma once

#include "main.h"
#include "pros/misc.h"
#include "pros/motors.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <type_traits>
 
//motors
extern pros::Motor left_back;
extern pros::Motor left_back;
extern pros::Motor left_front;
extern pros::Motor right_back;
extern pros::Motor right_front; // reversed
extern pros::Motor flywheel1;
extern pros::Motor flywheel2;
extern pros::Motor intake;
//pneumatics
extern pros::ADIDigitalOut indexer;
extern pros::ADIDigitalOut indexer2;
extern pros::ADIDigitalOut expansion;
//controller
extern pros::Controller master;


// Ports
#define LEFT_BACK_PORT 11
#define LEFT_FRONT_PORT 1
#define RIGHT_BACK_PORT 20
#define RIGHT_FRONT_PORT 10
#define FLYWHEEL_PORT 18
#define FLYWHEEL_PORT2 17
#define INTAKE_PORT1 2
#define RED 1
#define BLUE 2
#define target_color 1
#define INDEXER_PORT 'H'
#define INDEXER_PORT2 'G'
#define INTAKE_DEFAULT_VELOCITY 375
#define INTAKE_SLOW_VELOCITY 150
#define FLYWHEEL_DEFAULT_SPEED 350
#define FLYWHEEL_STEP_SIZE 25
#define FLYWHEEL_THRESHOLD 0.10
#define INTAKE_STEP_SIZE 2f0
#define SHOOTING_TIME_MS 100
#define EXPANSION_PORT 'F'
#define BUTTON_TRIPPLE_SHOT DIGITAL_X
#define BUTTON_FLYWHEEL_DOWN DIGITAL_LEFT
#define BUTTON_FLYWHEEL_UP DIGITAL_RIGHT
#define BUTTON_FLYWHEEL_SET DIGITAL_L1
#define BUTTON_INDEXER DIGITAL_L2
#define VISION_PORT 15
*/