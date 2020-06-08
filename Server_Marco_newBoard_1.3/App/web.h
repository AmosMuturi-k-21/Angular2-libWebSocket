#pragma once
#include "include.h"

#ifndef DISABLE_WEB

#ifdef __cplusplus
extern "C" {
#endif

	enum ui_messages {
		ui_magnetic_down, //put magnetic down

		ui_salt_1, //put salt 1
		ui_salt_2, //put salt 2

		ui_clean_mirror, //clean mirror

		ui_aborted_magnetic, //magnetic is up, op aborted
	};

	enum ui_event {
		ui_event_measure_started,
		ui_event_measure_end,
		ui_event_measure_error,
		ui_event_measure_aborted,

		ui_event_calibration_started,
		ui_event_calibration_error,
		ui_event_calibration_error_salt, //same salt found
		ui_event_calibration_error_close_salt,
		ui_event_calibration_end,

		//ui_event_measure_aborted_magnetic
	};

	void initialize_web();
	static PI_THREAD(th_web);

	int ui_info_msg(enum ui_messages msg); //ok
	int ui_dialog_msg(enum ui_messages msg); //ok,cancel
	int ui_error_msg(enum ui_messages msg);

	int ui_event(enum ui_event evt);

	void command_cicle();

	int ws_clients_connected();

	//extern int WAIT_WS;

	/*extern char ABORT_MEASURE;
	extern char START_MEASURE;
	extern char START_CALIB;*/

#ifdef __cplusplus
} // closing brace for extern "C"
#endif


#else

enum ui_messages {
	ui_magnetic_down, //put magnetic down

	ui_salt_1, //put salt 1
	ui_salt_2, //put salt 2

	ui_clean_mirror, //clean mirror

	ui_aborted_magnetic, //magnetic is up, op aborted
};

enum ui_event {
	ui_event_measure_started,
	ui_event_measure_end,
	ui_event_measure_error,
	ui_event_measure_aborted,

	ui_event_calibration_started,
	ui_event_calibration_error,
	ui_event_calibration_error_salt, //same salt found
	ui_event_calibration_error_close_salt,
	ui_event_calibration_end,

	//ui_event_measure_aborted_magnetic
};

#define initialize_web()

#define ui_info_msg(msg)
#define ui_dialog_msg(msg)
#define ui_error_msg(msg)

#define ui_event(evt)

#define command_cicle()

#define ws_clients_connected() 0

#endif