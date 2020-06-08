#pragma once

#include <time.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define DISABLE_WEB

	void log_error(char*);
	void log_warning(char*);
	void log_info(char*);

	//buzzer
	extern void buzzer_error();
	extern void buzzer_alert();

	#define g_tms(x) {struct timespec _ttime;clock_gettime(CLOCK_REALTIME, &_ttime);x=(_ttime.tv_nsec / 1000000);};

	extern unsigned long tms;

	unsigned long time_ms();

#define map(value,min,max,mapstart,mapstop) ((float)(value - min) * (float)(((float)(mapstop - mapstart)) / ((float)(max - min))) + mapstart)

#define MAX_32_BIT 4294967295
#define MAX_24_BIT 16777215
#define MAX_16_BIT 65535
#define MAX_8_BIT 255


#ifdef __cplusplus
} // closing brace for extern "C"
#endif