#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "proto.h"
#include "protocole_fast.h"
#include "ad7714.h"
#include "bb7614.h"
#include "HD44780.h"
#include "i2c.h"
#include "options.h"
#include <time.h>


//#define OLD_MODE_LCD
//#define PRINT_DEBUG

#define NB_CHOIX_REGU 16

	extern data char etape_communication;
	extern data char numero_param;
	extern code char option_menu_min[NB_MENU + 2];

#ifdef __cplusplus
} // closing brace for extern "C"
#endif


#define		TSETPOINT		25.0	// degrees Celsius
#define		TSET_COUNTS		10		// iterations occurred before next SetTemp to peltier board

float g_deltaTemp;	
float g_prev_deltaTemp;
float g_readV;

double valdet_offset;
double valmir_offset;
double valsurf_offset;

FILE *fl;
#define	DOLOG			1	// un-comment this line to generate file logdata.txt

#define	NO_AVERAGING	1	// if un-commented, no averaging will take place on tmir, tsurf, valdet