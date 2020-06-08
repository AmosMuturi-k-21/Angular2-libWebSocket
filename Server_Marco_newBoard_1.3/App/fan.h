#pragma once
#include "include.h"
#include "addc.h"

#ifdef __cplusplus
extern "C" {
#endif

	#define AIN1	1	// AIN1 - GPIO18  pin 12 - used as PWM pin
						// unhappy name does NOT indicate an Analog Input,
						// but "A-phase input 1" of chip DRV8833 motor driver

	void initialize_fan();
	void fan_set_perc(float perc);
	float fan_get_perc();


#ifdef __cplusplus
} // closing brace for extern "C"
#endif