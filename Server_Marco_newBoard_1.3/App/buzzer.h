#pragma once
#include "include.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define FREQ 3000

void initialize_buzzer();
void buzzer_start();
void buzzer_stop();
void buzzer(int);
void buzzer_error();
void buzzer_alert();
void buzzer_set_freq(int f);
void buzzer_song(int list[]);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif