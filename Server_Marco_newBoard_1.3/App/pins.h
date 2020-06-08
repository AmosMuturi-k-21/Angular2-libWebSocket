#pragma once

#define GPIO_BUZZER 30

#define GPIO_MENU 29
#define GPIO_OK 28
#define GPIO_MAG 6

//#define PELTIER_DIR2 22
//PWM is 23/33 and DIR 22/31
//#define PELTIER_DIR 21
//#define PELTIER_PWM 26
//#define PELTIER_DIR 22	//changed by ShanSong, remove this line since we don't use PWM to control peltier any more.
//#define PELTIER_PWM 23	//changed by ShanSong, remove this line since we don't use PWM to control peltier any more.

//#define LCD_ID 0x3f
//#define LCD_ID2 0x27
#define LCD_ROW 4
#define LCD_COL 20

//other config
#define DAC8552_V 5.0F
#define FAN_PORT 0
#define PELTIER_PORT 1 //changed by ShanSong, add the peltier DAC port

#define PELTIER_MAX_A 2.4F
#define DVR_MAX_OUTPUT_A 2.4F

//adc
#define VAL_DET 0
#define VAL_REF 1
#define VAL_MIR 2
#define VAL_SURF 6
#define VAL_INT 4

#define VAL_DET_DIFF  0	// differential channel 0, i.e. AD0-AD1 inputs
#define VAL_MIR_DIFF  1	// differential channel 1, i.e. AD2-AD3 inputs
#define VAL_SURF_DIFF 2	// differential channel 2, i.e. AD4-AD5 inputs


#define	THE_VREF	2.5		// Volt, reference voltage AD580

#define	N_AVER_TSURF 20	// number of averages on tsurf
#define	N_AVER_DET   20	// number of averages on optical detector
#define	N_AVER_MIR   20	// number of averages on optical detector

double tsArr[N_AVER_TSURF];
double dtArr[N_AVER_DET];
double mrArr[N_AVER_MIR];

