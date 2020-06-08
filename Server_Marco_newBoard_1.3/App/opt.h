#pragma once
#include "include.h"
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

	void initialize_opt();
	void sync_opt();
	void default_opt();

	extern char OPT_SAVE_MUTEX;

#define LOCK_OPT() {\
MUTEX_LOCKED:\
while (OPT_SAVE_MUTEX != 0) delay(1); \
OPT_SAVE_MUTEX = 0xCC; \
if (OPT_SAVE_MUTEX == 0xAA) goto MUTEX_LOCKED;\
OPT_SAVE_MUTEX = 0xAA;\
if (OPT_SAVE_MUTEX == 0xCC) goto MUTEX_LOCKED;\
if (OPT_SAVE_MUTEX != 0xCC && OPT_SAVE_MUTEX != 0xAA){\
	OPT_SAVE_MUTEX = 0;\
	goto MUTEX_LOCKED;\
}\
if (OPT_SAVE_MUTEX == 0xCC) goto MUTEX_LOCKED;}

#define RELEASE_OPT()\
	OPT_SAVE_MUTEX = 0;

#pragma pack(push, 1)

#define MEASURE_MAX 500
#define LABEL_MAX_SIZE 10

	struct Measure {
		time_t date;
		uint32_t id;
		uint16_t aw;
		int16_t tsurf;
		uint32_t duration;
		uint8_t mode;
		uint8_t mode_type;
		uint8_t label[LABEL_MAX_SIZE];
	};

	struct Options
	{
		//misc
		char LANGUAGE;
		char BIP_ON_BUTTON;
		float BUZZ_FREQ;
		float LCD_REFRESH;
		char LCD_ID;

		//password
		char SERVICE_PASSWORD[9];

		//type
		char MACHINE_TYPE;

		//hardware
		float FAN_SPEED; //0 - 100

		//DELTA
		float DELTA_TEMP_SURF;
		float DELTA_TEMP_MIRROR;
		float DELTA_VALUE_IR;

		//CALIBRATION
		//float DELTA_CALIBRATION;
		//float SLOPE_CALIBRATION;

		//slide
		float SLOPE_TEMP_SURF;
		float SLOPE_TEMP_MIRROR;

		//measure mode
		char MEASURE_MODE; //enum MEASURE_MODE
		char KINETIC_MODE; //enum KINETIC_MODE
		char CALIBRATION_MODE; //enum CALIBRATION_MODE
		char AUTOSTART; //enum AUTOSTART

		/*aw calibration*/
		float DELTA_AW;
		float SLOPE_AW;

		/* temeprature sample controll */
		float SAMPLE_TEMP;

		//last 
		char UI_STORAGE[1024];
	};
#pragma pack(pop)

	extern struct Options opt;

	enum MEASURE_MODE {
		_fix_10_m,
		_2_5_flash,
		_3_10_flash,
		_6_30_flash,
		_10_flash,
		_2_5m,
		_3_10m,
		_6_30m
	};

	enum KINETIC_MODE {
		_15m_2_30,
		_30m_2_30,
		_1h_15,
		_2h_10,
		_4h_15,
		_8h_30,
		_cont_60,
		_cont_2_30,
	};

	enum CALIBRATION_MODE {
		_1p_1l,
		_1p_2l,
		_1p_3l,
		_1p_4l,
		_1P_5l,
		_1l_6l,

		_2p_1l = 128,
		_2p_2l,
		_2p_3l,
		_2p_4l,
		_2P_5l,
		_2p_6l,
	};

	enum AUTOSTART {
		off,
		measure_1,
		measure_2,
		kinetic_1,
		kinetic_2,
	};

#ifdef __cplusplus
} // closing brace for extern "C"
#endif