#ifndef CONFIGS_H
#define CONFIGS_H

#include "defines.h"

#define ARM_LED_PIN 53
#define WORK_LED_PIN 52

#define FULL_ITERATION_PERIOD 10
//static quint8 FULL_ITERATION_PERIOD = 10;

#define MAX_Y_DECLINATION 45.0
#define MAX_P_DECLINATION 45.0
#define MAX_R_DECLINATION 45.0

#define DEFAULT_PID_MAX_I 0
#define DEFAULT_PID_GLOBAL_MULTIPLIER 1000 // 0.001

#define DEFAULT_RC_MIN 2100
#define DEFAULT_RC_MAX 900
#define MEM_VERSION 1
#define FLAG_FIRST_TIME 987
#define DEFAULT_FS_VALUE 950

#define DEFAULT_P_VALUE 10000 // 0.001
#define DEFAULT_I_VALUE 1000 // 0.001
#define DEFAULT_D_VALUE 1000 // 0.001

#define MAX_PID_FORCE 1000.0

#define MAX_YAW_SPEED 360.0

#define RC_SAFE_YPR_MIDDLE 40

#define INT_TO_FLOAT(A) (float(A)/1000.0)

#define MOTOR_MAX 1900
#define MOTOR_MIN 1100

#endif