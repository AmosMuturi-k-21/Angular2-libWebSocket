#pragma once
#include "include.h"

#ifdef __cplusplus
extern "C" {
#endif

	void initialize_buttons();

	int button(int but);
	int button_read(int but);
	void button_clear(int but);
	void button_clear_all();
	int button_is_press(int but);
	void simulate_press(int but, int status, int time);
	void simulate_status(int but, int status);
	void simulate_status_off(int but );

#ifdef __cplusplus
} // closing brace for extern "C"
#endif