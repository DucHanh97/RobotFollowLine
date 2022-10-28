#ifndef FOLLOW_LINE_H
#define FOLLOW_LINE_H

#include <stdint.h>

/*----- Define gains of PID controler -----*/
#define Kp					75
#define Ki					0
#define Kd					0
#define BASE_SPEED	25
/*-----------------------------------------*/

/*------------ Prototype Functions ------------*/
int8_t read_sensor_error(void);
int16_t PID_value_calculate(int8_t error);
void car_following_line_handle(void);
/*---------------------------------------------*/

#endif
