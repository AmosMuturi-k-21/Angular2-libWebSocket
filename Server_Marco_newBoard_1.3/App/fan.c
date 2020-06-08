#include "fan.h"

static float old_fan_perc = 0;


// *********************************************************************************************************
// *	name: initialize_fan
// *	function: set the RaspberryPi pin driving DRV8833 and init the waveform generation
// *	parameter: none
// *	return value: none
// *********************************************************************************************************
void initialize_fan(void) 
{
	pinMode (AIN1, PWM_OUTPUT) ;	// set pin AIN1 as PWM
	fan_set_perc(0);
}



// *********************************************************************************************************
// *	name: fan_set_perc
// *	function: set PWM signal driving DRV8833 to a given param value
// *	parameter: percent of pulse width modulation
// *	return value: none
// *********************************************************************************************************
void fan_set_perc(float perc) 
{
	int duty;
	
	if (perc < 0) perc = 0;
	if (perc >= 100) perc = 100;
	if (perc == old_fan_perc) return;

	duty = (int)(perc * 1023.0 / 100.0);
	
	printf("fan percent = %f, duty = %d\n", perc, duty);
	old_fan_perc = perc;

	pwmWrite(AIN1, duty);
}



// *********************************************************************************************************
// *	name: fan_get_perc
// *	function: set the RaspberryPi pin driving DRV8833 and init the waveform generation
// *	parameter: none
// *	return value: the PWM duty cycle percent
// *********************************************************************************************************
float fan_get_perc(void) 
{
	return old_fan_perc;
}