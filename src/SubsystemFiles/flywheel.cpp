/*
#include "main.h"
#include "subsystemHeaders/globals.hpp"
#define RETRACTING -1
#define READY 0
#define EXPANDING 1
//variable defintions
std::chrono::system_clock::time_point indexer_time =  std::chrono::system_clock::now();
bool flywheel_status = false;
int flywheel_speed = FLYWHEEL_DEFAULT_SPEED;
int flywheel_spin = 0;
int disks_to_shoot = 0;
int indexer_state = 0;

long indexer_threshold_time = 1000;
void setIndexer(bool expand) {
  indexer.set_value(expand);
  indexer2.set_value(!expand);
}
  
void flywheel2m(){
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
}


void indexer2solaniond(){
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
// -1 = retracting, 0 = ready, 1 = expanding
void triple_shot(){
    int timediff = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch() - indexer_time.time_since_epoch()).count();

    if (indexer_state == RETRACTING){
        if (disks_to_shoot <= 0){
            indexer_state = READY;
        }
        if (disks_to_shoot > 0 && timediff<=indexer_threshold_time){
            setIndexer(true);
            indexer_state = READY;
            indexer_time = std::chrono::system_clock::now();
        }
    }

    else if (indexer_state == EXPANDING){

        if (timediff>=indexer_threshold_time){
            setIndexer(false);
            indexer_state = RETRACTING;
            indexer_time = std::chrono::system_clock::now();
            disks_to_shoot--;
        }

    }

    else if (indexer_state == READY){
        if(master.get_digital(BUTTON_TRIPPLE_SHOT)) {
            disks_to_shoot = 3;
            indexer_state = EXPANDING;
            indexer_time = std::chrono::system_clock::now();
        }
    }
}

//expand
*/