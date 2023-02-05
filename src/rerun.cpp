
#include "main.h" 
#include "pros/adi.hpp"
#include <string>



#define LEFT_BACK_PORT 11
#define LEFT_FRONT_PORT 1
#define RIGHT_BACK_PORT 20
#define RIGHT_FRONT_PORT 10
#define FLYWHEEL_PORT 18
#define FLYWHEEL_PORT2 17
#define INTAKE_PORT1 2

int timeOld;
int timeNew;
int deltaTime;

long time_counts = 0;

bool p1;
bool p2;
bool p3;
bool p4;
bool p5;

// int p6;
// int p7;
// int p8;
// int p9;
// int p10;
// int p11;
// int p12;
// int p13;
// int p14;
// int p15;
// int p16;
// int p17;
// int p18;
// int p19;
// int p20;
// int p21;

int m1;
int m2;
int m3;
int m4;
int m5;
int m6;
int m7;
int m8;







 void record(pros::Motor Motors[7],std::string Motor_Names[7], pros::ADIDigitalOut Pneumatics[5],std::string Pnuematic_Names[8]) {

	 FILE* usd_file_write = fopen("/usd/rerun.txt", "w");



	 



	 while (time_counts< 1500) {

		m1 = Motors[0].get_voltage();
		m2 = Motors[1].get_voltage();
		m3 = Motors[2].get_voltage();
		m4 = Motors[3].get_voltage();
		m5 = Motors[4].get_voltage();
		m6 = Motors[5].get_voltage();
		m7 = Motors[6].get_voltage();
		//m8 = Motors[7].get_voltage();
        
        p1 = Pneumatics[0].set_value(true);
        p2 = Pneumatics[1].set_value(true);
        p3 = Pneumatics[2].set_value(true);
        p4 = Pneumatics[3].set_value(true);
        p5 = Pneumatics[4].set_value(true);


        fputs((Motor_Names[0]+".move("+std::to_string(m1)+");"+"\n").c_str(), usd_file_write);
        fputs((Motor_Names[1]+".move("+std::to_string(m2)+");"+"\n").c_str(), usd_file_write);
        fputs((Motor_Names[2]+".move("+std::to_string(m3)+");"+"\n").c_str(), usd_file_write);
        fputs((Motor_Names[3]+".move("+std::to_string(m4)+");"+"\n").c_str(), usd_file_write);
        fputs((Motor_Names[4]+".move("+std::to_string(m5)+");"+"\n").c_str(), usd_file_write);
        fputs((Motor_Names[5]+".move("+std::to_string(m6)+");"+"\n").c_str(), usd_file_write);
        fputs((Motor_Names[6]+".move("+std::to_string(m7)+");"+"\n").c_str(), usd_file_write);
        //fputs((Motor_Names[7]+".move("+std::to_string(m8)+");"+"\n").c_str(), usd_file_write);


        fputs((Pnuematic_Names[0]+".set_value("+std::to_string(p1)+");"+"\n").c_str(), usd_file_write);
        fputs((Pnuematic_Names[1]+".set_value("+std::to_string(p2)+");"+"\n").c_str(), usd_file_write);
        fputs((Pnuematic_Names[2]+".set_value("+std::to_string(p3)+");"+"\n").c_str(), usd_file_write);
        fputs((Pnuematic_Names[3]+".set_value("+std::to_string(p4)+");"+"\n").c_str(), usd_file_write);
        fputs(( Pnuematic_Names[4]+".set_value("+std::to_string(p5)+");"+"\n").c_str(), usd_file_write);

        fputs("delay(10)", usd_file_write);
	    pros::delay(10);
        time_counts+=1;
        
	}
    fclose(usd_file_write);
}
