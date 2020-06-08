#include "web.h"

#include "peltier.h"
#include "button.h"
#include "buzzer.h"
#include "fan.h"
#include "addc.h"

#include "proto.h"
#include "vardef.h"
#include "opt.h"

#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ssj.h"

#ifndef DISABLE_WEB

#define DEBUG_WS

#define WS_TIMEOUT 3000
#define DIALOG_TIMEOUT 60000

static int FREQUENCY = 30;
static int DELAY_MESSAGE = 10;

const HTTP_PATH = "/home/pi/http";

static int CLIENTS = 0;

static char ABORT_MEASURE = 0;
static char START_MEASURE = 0;
static char MEASURE_MODE = 0; //0 normal, 1 kinetics, 2 calibration
static char START_CALIB = 0;

extern float DELTA_TEMP_SURF;
extern float DELTA_TEMP_MIRROR;

static struct json response;
static struct json_value cmd, value, id;
static int cmd_ok = 1;

static int message_to_ui = -1;
static int message_type_to_ui = 0; //0 info, 1 dialog
static int message_to_ui_response = 0; //0 ok, 1 cancel

//static int event_list[10] = { 0 };
//static int event_list_size = 0;
static int event_ui = -1;

static int WAIT_WS = 0;

static int callback_http(
	struct libwebsocket_context * this,
	struct libwebsocket *wsi,
	enum libwebsocket_callback_reasons reason,
	void *user,
	void *in,
	size_t len
);
static int dumb(
	struct libwebsocket_context * this,
	struct libwebsocket *wsi,
	enum libwebsocket_callback_reasons reason,
	void *user,
	void *in,
	size_t len
);

static PI_THREAD(th_web);
static PI_THREAD(th_web_worker);

static uint8_t SEND_BUFFER[LWS_SEND_BUFFER_PRE_PADDING + 2048
+ LWS_SEND_BUFFER_POST_PADDING];

static struct json_token parsed[100];

static void manage_option();
static int is_head_ok();

void initialize_web() {
	json_init_buffer(&response, &SEND_BUFFER[LWS_SEND_BUFFER_PRE_PADDING], 2048,0);

	piThreadCreate(th_web);
	piThreadCreate(th_web_worker);
}

enum demo_protocols {
	/* always first */
	PROTOCOL_HTTP = 0,

	PROTOCOL_DUMB_INCREMENT,
	PROTOCOL_LWS_MIRROR,
	PROTOCOL_LWS_STATUS,

	PROTOCOL_LWS_META,

	/* always last */
	DEMO_PROTOCOL_COUNT
};

static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */

	{
		"http-only",		/* name */
		callback_http,		/* callback */
		1024*1024*5,	/* per_session_data_size */
		1024*512,			/* max frame size / rx buffer */
		// WARNING
		NULL,
		0

	},
	{
		"app",
		dumb,
		2048,
		2048, /* rx buf size must be >= permessage-deflate rx size
			* dumb-increment only sends very small packets, so we set
			* this accordingly.  If your protocol will send bigger
			* things, adjust this to match */
	NULL,
	0
	},
	{ NULL, NULL, 0, 0, NULL, 0 } /* terminator */
};

static int callback_http(
	struct libwebsocket_context * this,
	struct libwebsocket *wsi,
	enum libwebsocket_callback_reasons reason,
	void *user,
	void *in,
	size_t len
) {
	switch (reason) {
	case LWS_CALLBACK_CLIENT_WRITEABLE:
		printf("http connection established\n");

	case LWS_CALLBACK_HTTP: {
		char *requested_uri = (char *)in;
		printf("requested URI: %s\n", requested_uri);

		if (strcmp(requested_uri, "/") == 0) {
			char cwd[4096];
			cwd[0] = 0;
			//getcwd(cwd, sizeof(cwd));
			strcat(cwd,HTTP_PATH);
			strcat(cwd, "/");
			strcat(cwd,"index.html");
			printf("resource path: %s\n", cwd);
			libwebsockets_serve_http_file(this, wsi, cwd, "text/html"); //return index
			break;
		}
		else {
			char cwd[4096];
			char *mime;
			cwd[0] = 0;
			//getcwd(cwd, sizeof(cwd));
			strcat(cwd, HTTP_PATH);
			strcat(cwd, requested_uri);
			printf("resource path: %s\n", cwd);

			char *extension = strrchr(cwd, '.');

			if (extension == NULL) {
				mime = "text/plain";
			}
			else if (strcmp(extension, ".png") == 0) {
				mime = "image/png";
			}
			else if (strcmp(extension, ".jpg") == 0) {
				mime = "image/jpg";
			}
			else if (strcmp(extension, ".gif") == 0) {
				mime = "image/gif";
			}
			else if (strcmp(extension, ".svg") == 0) {
				mime = "image/svg+xml";
			}
			else if (strcmp(extension, ".html") == 0) {
				mime = "text/html";
			}
			else if (strcmp(extension, ".css") == 0) {
				mime = "text/css";
			}
			else if (strcmp(extension, ".js") == 0) {
				mime = "application/javascript";
			}
			//fonts
			else if (strcmp(extension, ".eot") == 0) {
				mime = "application/vnd.ms-fontobject";
			}
			else if (strcmp(extension, ".otf") == 0) {
				mime = "application/font-sfnt";
			}
			else if (strcmp(extension, ".ttf") == 0) {
				mime = "application/font-sfnt";
			}
			else if (strcmp(extension, ".woff") == 0) {
				mime = "application/font-woff";
			}
			else if (strcmp(extension, ".woff2") == 0) {
				mime = "font/woff2";
			}
			//any other
			else {
				mime = "text/plain";
			}

			if (access(cwd, F_OK) != -1) {
				libwebsockets_serve_http_file(this, wsi, cwd, mime); //return file
			}
			else {
				char cwd[4096];
				cwd[0] = 0;
				//getcwd(cwd, sizeof(cwd));
				strcat(cwd, HTTP_PATH);
				strcat(cwd, "/");
				strcat(cwd, "index.html");
				printf("resource path: %s\n", cwd);
				libwebsockets_serve_http_file(this, wsi, cwd, "text/html"); //return index
			}
				//return index
		}
		/*else {
			// try to get current working directory
			char cwd[4096];
			char *resource_path;

			if (getcwd(cwd, sizeof(cwd)) != NULL) {
				// allocate enough memory for the resource path
				resource_path = malloc(strlen(cwd)
					+ strlen(requested_uri));

				// join current working directory to the resource path
				sprintf(resource_path, "%s%s", cwd, requested_uri);
				printf("resource path: %s\n", resource_path);

				char *extension = strrchr(resource_path, '.');
				char *mime;

				// choose mime type based on the file extension
				if (extension == NULL) {
					mime = "text/plain";
				}
				else if (strcmp(extension, ".png") == 0) {
					mime = "image/png";
				}
				else if (strcmp(extension, ".jpg") == 0) {
					mime = "image/jpg";
				}
				else if (strcmp(extension, ".gif") == 0) {
					mime = "image/gif";
				}
				else if (strcmp(extension, ".html") == 0) {
					mime = "text/html";
				}
				else if (strcmp(extension, ".css") == 0) {
					mime = "text/css";
				}
				else {
					mime = "text/plain";
				}

				// by default non existing resources return code 400
				// for more information how this function handles 
				// headers see it's source code
				// http://git.warmcat.com/cgi-bin/cgit/libwebsockets/tree/lib/parsers.c#n1896
				libwebsockets_serve_http_file(this,wsi, resource_path, mime);

			}
		}*/

		// close connection
		/*libwebsocket_close_and_free_session(
			this, wsi, LWS_CLOSE_STATUS_NORMAL);*/
		break;
	}
	default:
		//printf("unhandled callback\n");
		break;
	}
	return 0;
}

#define RESPONSE(x) {\
int siz = strlen(x);\
memcpy(&SEND_BUFFER[LWS_SEND_BUFFER_PRE_PADDING], x, siz);\
libwebsocket_write(wsi, &SEND_BUFFER[LWS_SEND_BUFFER_PRE_PADDING],siz, LWS_WRITE_TEXT);\
}

#define SEND(){\
int siz = strlen(&SEND_BUFFER[LWS_SEND_BUFFER_PRE_PADDING]); \
libwebsocket_write(wsi, &SEND_BUFFER[LWS_SEND_BUFFER_PRE_PADDING], siz, LWS_WRITE_TEXT); \
}

#define SEND_SIZ(x){\
libwebsocket_write(wsi, &SEND_BUFFER[LWS_SEND_BUFFER_PRE_PADDING], x, LWS_WRITE_TEXT); \
}

#define ADDSTR(x,y) json_add(&response, x, "%s", y);
#define ADDJSON(x,y) json_add(&response, x, "%j", y);
#define ADDNUM(x,y) {\
float temp = (y);\
if (isnan(temp)) temp = 0;\
json_add(&response, x, "%f", &temp);\
}

#define CHECKSTR(_value,_x) (_value.json_data_type == data_string && strcmp(_value.value.ptr,_x)==0)
#define CHECKCMP(_value,_x) (_value.json_data_type == data_string && memcmp(_value.value.ptr,_x,sizeof(_x)-1)==0)
#define CHECKNUM(_value,_x) ((_value.json_data_type == data_number || _value.json_data_type == data_boolean) && _value.value.number == _x)
#define GETNUM(_value) _value.value.number
#define GETSTR(_value) _value.value.ptr

void reset_ui_message_handler() {
	message_to_ui = -1;
	message_type_to_ui = -1;
}

void reset_ui_event_handler() {
	event_ui = -1;
}

static int dumb(
	struct libwebsocket_context * this,
	struct libwebsocket *wsi,
	enum libwebsocket_callback_reasons reason,
	void *user,
	void *in,
	size_t len
) {

	//printf("status = %d\n", reason);
	float temp;
	int list_size;
	switch (reason) {
	case LWS_CALLBACK_CLOSED:
		printf("LWS_CALLBACK_CLOSED");
		
		if (CLIENTS > 0)
			CLIENTS--;
		break;
		// just log message that someone is connecting
	case LWS_CALLBACK_ESTABLISHED:
		printf("connection established\n");
		libwebsocket_callback_on_writable(this,wsi);
		CLIENTS++;
		break;
	case LWS_CALLBACK_SERVER_WRITEABLE:
		json_clear(&response);
		WAIT_WS = 1;

		if (message_to_ui != -1) {

#ifdef DEBUG_WS
			printf("mesage_ui [%d]:[%d]\n", message_type_to_ui, message_to_ui);
#endif

			ADDSTR("type", "dialog");
			if (message_type_to_ui == 0)
				ADDSTR("dialog", "info");
			if (message_type_to_ui == 1)
				ADDSTR("dialog", "error");
			if (message_type_to_ui == 2)
				ADDSTR("dialog", "input");
			ADDNUM("message", message_to_ui);

			//!!!!!
			//message_to_ui = -1;
			//message_type_to_ui = -1;

			WAIT_WS = 0;
		}else
			if (event_ui != -1) {

#ifdef DEBUG_WS
				printf("event [%d]\n", event_ui);
#endif

				ADDSTR("type", "event");
				if (event_ui == ui_event_measure_started)
					ADDSTR("event", "aw")
				else
					if (event_ui == ui_event_measure_end)
						ADDSTR("event", "measure_end")
					else
						if (event_ui == ui_event_measure_error)
							ADDSTR("event", "measure_error")
						else
							if (event_ui == ui_event_measure_aborted)
								ADDSTR("event", "measure_aborted")
							else
								if (event_ui == ui_event_calibration_started)
									ADDSTR("event", "calibration")
								else
								if (event_ui == ui_event_calibration_error)
									ADDSTR("event", "calibration_error")
								else
									if (event_ui == ui_event_calibration_error_salt)
										ADDSTR("event", "calibration_error_salt")
									else
										if (event_ui == ui_event_calibration_error_close_salt)
											ADDSTR("event", "calibration_error_close")
										else
											if (event_ui == ui_event_calibration_end)
												ADDSTR("event", "calibration_end");

				//event_ui = -1;
				WAIT_WS = 0;
			}
		else {

			ADDSTR("type", "auto");
			ADDNUM("reflexion", reflexion);
			ADDNUM("detection", detection);
			ADDNUM("t_surface", tsurf);
			ADDNUM("t_mirror", tmir);

			ADDNUM("fan", fan_get_perc());
			ADDNUM("peltier", perltier_get_perc());

			ADDNUM("clean_mirror", CLEAN_MIRROR);
			ADDNUM("measure_started", MESURE_EN_COURS);

			ADDNUM("current_task", etape);

			ADDNUM("button_magnetic", DEM_AUTO);
			ADDNUM("button_ok", BP_D);
			ADDNUM("button_menu", BP_G);

			//extra
			ADDNUM("calib", CALIB);
			ADDNUM("delta_tmir", delta_tmir);
			ADDNUM("pente_tmir", pente_tmir);
			ADDNUM("measure_mode", MEASURE_MODE); //0,1,2 normal, kinetics, calibration
			//extra end

			ADDNUM("time", time_etape);
			ADDNUM("time_measure", time_mesure);

			ADDNUM("xi", xi);
			ADDNUM("aw_inst", aw_inst);
			ADDNUM("aw_medium", aw_moy);
			ADDNUM("aw_flash", aw_flash);
			ADDNUM("aw_finale", aw_finale);
			ADDNUM("aw_int", aw_int);				// REMOVE comment after debug
			//ADDNUM("aw_int", g_deltaTemp);			// COMMENT this line after debug
 
			//ADC
			ADDNUM("adc0", addc_read_v(0));
			ADDNUM("adc1", addc_read_v(1));
			ADDNUM("adc2", addc_read_v(2));
			ADDNUM("adc3", addc_read_v(3));
			ADDNUM("adc4", addc_read_v(4));
			ADDNUM("adc5", addc_read_v(5));
			ADDNUM("adc6", addc_read_v(6));
			ADDNUM("adc7", addc_read_v(7));
		}

		SEND_SIZ(response.siz);

		libwebsocket_callback_on_writable(this, wsi);
		break;
	case LWS_CALLBACK_RECEIVE: {
#ifdef DEBUG_WS
		printf("parse: [%s]\n",in);
#endif
		list_size = json_parse(&parsed, 100, in, len);
		if (list_size < 0) break; //parse error
		//printf("parse ok list_size = %d\n", list_size);

		cmd_ok = 1;

		id = json_get_value(&parsed, list_size, "id");
		cmd = json_get_value(&parsed, list_size, "cmd");
		value = json_get_value(&parsed, list_size, "value");

		json_clear(&response);
		ADDNUM("id", GETNUM(id));

		ADDSTR("type", "cmd");
		ADDSTR("cmd", GETSTR(cmd));

		//printf("parsed first\n");
		
		/*printf("id = %f\n", GETNUM(id));
		printf("cmd = [%d]%s\n",cmd.json_data_type, cmd.value.ptr);
		printf("value =  [%d]%s\n", value.json_data_type, value.value.ptr);*/
		if (CHECKSTR(cmd, "dialog")) {
			message_to_ui_response = GETNUM(value);
		}
		else
		if (CHECKSTR(cmd, "history")) {
			static uint8_t BUFFER[2000];
			static uint8_t js[256];
			static struct json array;
			static struct json jsdata;
			json_init_buffer(&array, BUFFER, 2000, 1);
			
			for (int i = 0; i < NB_MESURE; i++) {
				json_init_buffer(&jsdata, js, 256, 0);
				JADDNUM(jsdata,"aw", tableau_result[i].mesure.aw);
				JADDNUM(jsdata, "date", tableau_result[i].mesure.date);
				JADDNUM(jsdata, "t_surface", tableau_result[i].mesure.tsurf);
				JADDJSON(array,"",jsdata.buffer);
			}

			JADDJSON(response, "response", array.buffer);
		}
		else
		if (CHECKSTR(cmd, "reset_config")) {
			default_opt();
			sync_opt();
		}
		else
		if (CHECKSTR(cmd, "peltier")) {
			peltier_set_perc(GETNUM(value));
		}
		else
		if (CHECKSTR(cmd, "fan")) {
			fan_set_perc(GETNUM(value));
		}
		else
		if (CHECKSTR(cmd, "buzzer")) {
			if (JISSTR(value)) {
				if (CHECKSTR(value, "error")) {
					buzzer_error();
				}else
				if (CHECKSTR(value, "alert")) {
					buzzer_alert();
				}
				else cmd_ok = 0;
			}else
				buzzer(GETNUM(value));
		}else
		if (CHECKSTR(cmd, "measure")) {
			if (CHECKSTR(value, "calib")) {
				if (MESURE_EN_COURS)
					ADDSTR("response", "started")
				else {
					MEASURE_MODE = 2;
					START_CALIB = 1;
				}
			}
			else
			if (CHECKSTR(value, "aw")) {
				if (is_head_ok()) {
					if (MESURE_EN_COURS)
						ADDSTR("response", "started")
					else {
						START_MEASURE = 1;
						MEASURE_MODE = 0;
					}
				}
				else {
					ADDSTR("response", "magnetic");
				}
			}
			else
				if (CHECKSTR(value, "testaw")) {
					if (is_head_ok()) {
						if (MESURE_EN_COURS)
							ADDSTR("response", "started")
						else {
							START_MEASURE = 1;
							MEASURE_MODE = 255;
						}
					}
					else {
						ADDSTR("response", "magnetic");
					}
				}
			else
				if (CHECKSTR(value, "kinetics")) {
					if (is_head_ok()) {
						if (MESURE_EN_COURS)
							ADDSTR("response", "started")
						else {
							START_MEASURE = 1;
							MEASURE_MODE = 1;
						}
					}
					else {
						ADDSTR("response", "magnetic");
					}
				}
			else
			if (CHECKSTR(value, "stop")) {
				if (MESURE_EN_COURS)
					//arret_utilisateur();
					ABORT_MEASURE = 1;
				else {
					ADDSTR("response", "not_started");
				}
			}
		}else
		if (CHECKCMP(cmd, "config")) {
			manage_option();
		}
		else {
			cmd_ok = 0;
		}

		if (cmd_ok) {
			ADDSTR("result", "ok");
		}
		else {
#ifdef DEBUG_WS
			printf("unknow cmd\n");
#endif
			ADDSTR("result", "error");
		}

		//printf("request end\n");

		SEND_SIZ(response.siz);

		break;
	}
	default:
		break;
	}

	return 0;
}

static void manage_option() {
#ifdef DEBUG_WS
	printf("manage option\n");
#endif

	//lock mutex
	if (value.json_data_type != data_null)
	LOCK_OPT();

	if (CHECKSTR(cmd, "config.DELTA_AW")) {
		if (value.json_data_type == data_null) {
			ADDNUM("response", opt.DELTA_AW);
		}
		else {
			opt.DELTA_AW = GETNUM(value);
			sync_opt();

			/* this is TEST code to be normally commented out
			hacked by Marco to check hardware:

			float daw;
			if (opt.DELTA_AW < 0.0)
				daw = 0;
			else
				daw = opt.DELTA_AW / 100.0;
			if (daw > 3.0)
				daw = 3.0;
			addc_dac_set(daw);
			//moteur(daw * 100 / 3.0);
			// end of hacked by Marco to check DAC AD5640
			*/
		}
	}
	else
		if (CHECKSTR(cmd, "config.SAMPLE_TEMP")) {
			if (value.json_data_type == data_null) {
				ADDNUM("response", opt.SAMPLE_TEMP);
			}
			else {
				opt.SAMPLE_TEMP = GETNUM(value);
				sync_opt();

				/* update temp */
				MeCom_SetTemp(0, opt.SAMPLE_TEMP); //0 address broadcast
				printf("\n *** Set by user interface TEC-1091 temp to: %.1f °C", opt.SAMPLE_TEMP);
			}
		}
		else
		if (CHECKSTR(cmd, "config.SLOPE_AW")) {
			if (value.json_data_type == data_null) {
				ADDNUM("response", opt.SLOPE_AW);
			}
			else {
				opt.SLOPE_AW = GETNUM(value);
				sync_opt();
			}
		}
		else
	if (CHECKSTR(cmd, "config.LCD_ID")) {
		if (value.json_data_type == data_null) {
			ADDNUM("response", opt.LCD_ID);
		}
		else {
			opt.LCD_ID = GETNUM(value);
			sync_opt();

			//restart lcd
			initialize_lcd();
			DISPLAY = lcd_working();
		}
	}
	else
	if (CHECKSTR(cmd, "config.BUZZ_FREQ")) {
		if (value.json_data_type == data_null) {
			ADDNUM("response", opt.BUZZ_FREQ);
		}
		else {
			opt.BUZZ_FREQ = GETNUM(value);
			sync_opt();
		}
	}
	else
		if (CHECKSTR(cmd, "config.LCD_REFRESH")) {
			if (value.json_data_type == data_null) {
				ADDNUM("response", opt.LCD_REFRESH);
			}
			else {
				opt.LCD_REFRESH = GETNUM(value);
				sync_opt();
			}
		}
		else
	if (CHECKSTR(cmd, "config.SERVICE_PASSWORD")) {
		if (value.json_data_type == data_null) {
			ADDSTR("response", opt.SERVICE_PASSWORD);
		}
		else {
			//opt.UI_STORAGE = GETNUM(value);
			char* str = GETSTR(value);
			int len = strlen(str);
			if (len >= sizeof(opt.SERVICE_PASSWORD) - 1)
				len = opt.SERVICE_PASSWORD - 1;

			memcpy(opt.SERVICE_PASSWORD, str, len);
			opt.SERVICE_PASSWORD[len] = 0; //set string eng

			sync_opt();
		}
	}
	else
	if (CHECKSTR(cmd, "config.UI_STORAGE")) {
		if (value.json_data_type == data_null) {
			ADDSTR("response", opt.UI_STORAGE);
		}
		else {
			char* str = GETSTR(value);
			int len = strlen(str);
			if (len >= sizeof(opt.UI_STORAGE) -1)
				len = opt.UI_STORAGE - 1;

			memcpy(opt.UI_STORAGE, str,len);
			opt.UI_STORAGE[len] = 0; //set string eng

			sync_opt();
		}
	}
	else
		if (CHECKSTR(cmd, "config.MACHINE_TYPE")) {
			if (value.json_data_type == data_null) {
				ADDNUM("response", opt.MACHINE_TYPE);
			}
			else {
				opt.MACHINE_TYPE = GETNUM(value);
				sync_opt();
			}
		}
	else
	if (CHECKSTR(cmd, "config.MEASURE_MODE")) {
		if (value.json_data_type == data_null) {
			ADDNUM("response", opt.MEASURE_MODE);
		}
		else {
			opt.MEASURE_MODE = GETNUM(value);
			sync_opt();
		}
	}
	else
	if (CHECKSTR(cmd, "config.KINETIC_MODE")) {
		if (value.json_data_type == data_null) {
			ADDNUM("response", opt.KINETIC_MODE);
		}
		else {
			opt.KINETIC_MODE = GETNUM(value);
			sync_opt();
		}
	}
	else
	if (CHECKSTR(cmd, "config.CALIBRATION_MODE")) {
		if (value.json_data_type == data_null) {
			ADDNUM("response", opt.CALIBRATION_MODE);
		}
		else {
			opt.CALIBRATION_MODE = GETNUM(value);
			sync_opt();
		}
	}
	else
	if (CHECKSTR(cmd, "config.AUTOSTART")) {
		if (value.json_data_type == data_null) {
			ADDNUM("response", opt.AUTOSTART);
		}
		else {
			opt.AUTOSTART = GETNUM(value);
			sync_opt();
		}
	}
	else
	if (CHECKSTR(cmd, "config.BIP_ON_BUTTON")) {
		if (value.json_data_type == data_null) {
			ADDNUM("response", opt.BIP_ON_BUTTON);
		}
		else {
			opt.BIP_ON_BUTTON = GETNUM(value);
			sync_opt();
		}
	}
	else
	if (CHECKSTR(cmd, "config.LANGUAGE")) {
		if (value.json_data_type == data_null) {
			ADDNUM("response", opt.LANGUAGE);
		}
		else {
			opt.LANGUAGE = GETNUM(value);
			sync_opt();
		}
	}
	else
	if (CHECKSTR(cmd, "config.FAN_SPEED")) {
		if (value.json_data_type == data_null) {
			ADDNUM("response", opt.FAN_SPEED);
		}
		else {
			opt.FAN_SPEED = GETNUM(value);
			sync_opt();
		}
	}
	else
		if (CHECKSTR(cmd, "config.DELTA_VALUE_IR")) {
			if (value.json_data_type == data_null) {
				ADDNUM("response", opt.DELTA_VALUE_IR);
			}
			else {
				opt.DELTA_VALUE_IR = GETNUM(value);
				sync_opt();
			}
		}
		else
		if (CHECKSTR(cmd, "config.DELTA_TEMP_SURF")) {
			if (value.json_data_type == data_null) {
				ADDNUM("response", opt.DELTA_TEMP_SURF);
			}
			else {
				opt.DELTA_TEMP_SURF = GETNUM(value);
				sync_opt();
			}
		}
		else
			if (CHECKSTR(cmd, "config.SLOPE_TEMP_SURF")) {
				if (value.json_data_type == data_null) {
					ADDNUM("response", opt.SLOPE_TEMP_SURF);
				}
				else {
					opt.SLOPE_TEMP_SURF = GETNUM(value);
					sync_opt();
				}
			}
			else
				if (CHECKSTR(cmd, "config.DELTA_TEMP_MIRROR")) {
					if (value.json_data_type == data_null) {
						ADDNUM("response", opt.DELTA_TEMP_MIRROR);
					}
					else {
						opt.DELTA_TEMP_MIRROR = GETNUM(value);
						sync_opt();
					}
				}
				else
					if (CHECKSTR(cmd, "config.SLOPE_TEMP_MIRROR")) {
						if (value.json_data_type == data_null) {
							ADDNUM("response", opt.SLOPE_TEMP_MIRROR);
						}
						else {
							opt.SLOPE_TEMP_MIRROR = GETNUM(value);
							sync_opt();
						}
					}
					else {
						cmd_ok = 0; //error
#ifdef DEBUG_WS
						printf("unknow option [%s]\n", GETSTR(cmd));
#endif
					}
					//unlock mutex
					if (value.json_data_type != data_null)
					RELEASE_OPT();
}

static PI_THREAD(th_web) {
	struct libwebsocket_context *context;

	struct lws_context_creation_info opt;
	opt.port = 3000;
	opt.iface = NULL;
	opt.protocols = &protocols;
	opt.extensions = libwebsocket_get_internal_extensions();
	opt.ssl_ca_filepath = NULL;
	opt.ssl_cert_filepath = NULL;
	opt.ssl_private_key_filepath = NULL;
	opt.user = 0;
	opt.ka_time = 5000; //keep alive timeout
	opt.ka_probes = 2;
	opt.ka_interval = 1000;
	opt.options = 0;
	

	// create libwebsocket context representing this server
	context = libwebsocket_create_context(&opt);

	if (context == NULL) {
		fprintf(stderr, "libwebsocket init failed\n");
		printf("error init libwebsocket\n");
		printf("probably another istance of App.out running or libwebsocket3 not installed\n");
		exit(EXIT_FAILURE);
		return -1;
	}

	int wait = 0;

	// infinite loop, to end this server send SIGTERM. (CTRL+C)
	while (1) {
		int was_ui_message = message_to_ui;
		int was_ui_event = ui_event;

		int clients = 0;
		do {
			libwebsocket_service(context, 0);
			clients++;
		} while (clients < CLIENTS);

		if (was_ui_message >= 0)
			reset_ui_message_handler();
		if (was_ui_event >= 0)
			reset_ui_event_handler();

		delay(FREQUENCY);
	}

	libwebsocket_context_destroy(context);

	return 0;
}

static PI_THREAD(th_web_worker) {
	while (1) {
		command_cicle();
		delay(50);
	}
}

int ui_info_msg(enum ui_messages msg) {//ok
	if (CLIENTS == 0)return;
	while (message_to_ui != -1 || message_to_ui_response == -1) {
		if (CLIENTS == 0)return;
		delay(DELAY_MESSAGE);
	}

	  message_to_ui = msg;
	  message_type_to_ui = 0; //info
	  message_to_ui_response = -1;

	  WAIT_WS = 0;//force send

	  int timeout = 0;
	  while (message_to_ui_response == -1) {
		  if (CLIENTS == 0)return;
		  delay(DELAY_MESSAGE);
		  timeout++;
		  if (timeout > WS_TIMEOUT) { //30 seconds
			  message_to_ui_response = 1;//cancell
			  message_to_ui = -1;
		  }
	  }

	  return message_to_ui_response;
}

int ui_error_msg(enum ui_messages msg) {//ok
	if (CLIENTS == 0)return;
	while (message_to_ui != -1 || message_to_ui_response == -1) {
		if (CLIENTS == 0)return;
		delay(DELAY_MESSAGE);
	}

	message_to_ui = msg;
	message_type_to_ui = 1; //error
	message_to_ui_response = -1;

	WAIT_WS = 0;//force send

	int timeout = 0;
	while (message_to_ui_response == -1) {
		if (CLIENTS == 0)return;
		delay(DELAY_MESSAGE);
		timeout++;
		if (timeout > WS_TIMEOUT) { //30 seconds
			message_to_ui_response = 1;//cancell
			message_to_ui = -1;
		}
	}

	return message_to_ui_response;
}

int ui_dialog_msg(enum ui_messages msg) { //ok,cancell
	if (CLIENTS == 0)return;
	while (message_to_ui != -1 || message_to_ui_response == -1) {
		if (CLIENTS == 0)return;
		delay(DELAY_MESSAGE);
	}

	message_to_ui = msg;
	message_type_to_ui = 2; //dialog
	message_to_ui_response = -1;

#ifdef DEBUG_WS
	printf("msg [dialog] [%d]:[%d]\n", message_type_to_ui, message_to_ui);
#endif

	WAIT_WS = 0;//force send

	int timeout = 0;
	while (message_to_ui_response == -1) {
		if (CLIENTS == 0)return;
		delay(DELAY_MESSAGE);
		timeout++;
		if (timeout > DIALOG_TIMEOUT || ABORT_MEASURE || START_MEASURE || START_CALIB) { //30 seconds
#ifdef DEBUG_WS
			printf("msg [dialog] [%d]:[%d] => timeout\n", message_type_to_ui, message_to_ui);
#endif
			message_to_ui_response = 1;//cancell
			message_to_ui = -1;
		}
	}

#ifdef DEBUG_WS
	printf("msg [dialog] [%d]:[%d] => [%d]\n", message_type_to_ui, message_to_ui, message_to_ui_response);
#endif

	return message_to_ui_response;
}

int ui_event(enum ui_event evt) {

	switch (evt)
	{
	case ui_event_measure_started:
		log_info("Measure started");
		break;
	case ui_event_measure_end:
		log_info("Measure end");
#ifdef DOLOG
		if (fl != NULL) fclose(fl);
#endif
		break;
	case ui_event_measure_error:
		log_error("Measure error");
#ifdef DOLOG
		if (fl != NULL) fclose(fl);
#endif
		break;
	case ui_event_measure_aborted:
		log_error("Measure aborted");
#ifdef DOLOG
		if (fl != NULL) fclose(fl);
#endif
		break;
	case ui_event_calibration_started:
		log_info("Calibration started");
		break;
	case ui_event_calibration_error:
		log_error("Calibration error");
		break;
	case ui_event_calibration_error_salt:
		log_error("Calibration error same salt for 2p calib");
		break;
	case ui_event_calibration_error_close_salt:
		log_error("Calibration error close salt");
		break;
	case ui_event_calibration_end:
		log_info("Calibration end");
		break;
	default:
		break;
	}
	if (CLIENTS == 0)return;

	while (event_ui != -1) {
		if (CLIENTS == 0)return;
		delay(DELAY_MESSAGE);
	}

	event_ui = evt;
	WAIT_WS = 0; //force send now
}

int ws_clients_connected() {
	return CLIENTS;
}

int is_head_ok() {
	if (opt.AUTOSTART == off) {
		return 1;
	}
	return DEM_AUTO == 0;
}

int command_queue_size() {
	return (ABORT_MEASURE || START_MEASURE || START_CALIB);
}


extern int _PAUSE_TH_MEASURE;

int get_fake_aw() {
	return (rand() % 1000);
}

int get_fake_aw_xi(int value,float xi) {
	return ((int)(value+((((rand() % 2000)) / (1000 - xi)) - (((rand() % 2000)) / (1000 - xi)))))%1000;
}

void command_cicle() {
	if (ABORT_MEASURE) {
		ABORT_MEASURE = 0;
		arret_utilisateur();
	}

	if (START_MEASURE) {
		START_MEASURE = 0;
		if (MEASURE_MODE == 0)
			mesure_std();
		else 
			if (MEASURE_MODE == 1)
			cinetique_std();

		if (MEASURE_MODE == 255) { //test measure mode
			_PAUSE_TH_MEASURE = 1;
			delay(100);

			log_info("measure simulator launched");
			bip(10, 10, 10);
			ui_event(ui_event_measure_started);

			MESURE_EN_COURS = 1;

			xi = 7.5;
			tsurf = (rand() % 20) + 10;
			tmir = (rand() % 20) + 10;

			ETAPE_H = 0;
			ETAPE_L = 1; //task 1
			log_info("measure simulator [equilibrium]");

			aw_finale = 0;
			aw_inst = 1;

			//simulate equilibrium
			float delta;
			while (delta > 2 || delta < -2) {
				delta = tsurf - tmir;
				if (xi > 0) tsurf -= 0.1;
				if (xi < 0) tsurf += 0.1;
				if (command_queue_size() != 0) return; //abort
				delay(200);
			}

			log_info("measure simulator [flash]");

			//flash
			ETAPE_H = 1;
			ETAPE_L = 0;
			tsurf -= 15;
			delay(5000);


			log_info("measure simulator [measure]");

			//measure
			ETAPE_H = 1;
			ETAPE_L = 1;

			//some fake values
			for (int i = 0; i < 10; i++) {
				aw_inst = get_fake_aw();
				delay(100);
			}

			int aw_value_fake = aw_inst;
			float errorsim = 1000;
			while (xi > 0.01) {
				tsurf += ((rand()%10)/20.0); //randomize temperature
				tsurf -= ((rand()%10)/20.0); //randomize temperature
				tmir += ((rand() % 10) / 20.0); //randomize temperature
				tmir -= ((rand() % 10) / 20.0); //randomize temperature

				aw_inst = get_fake_aw_xi(aw_value_fake, errorsim);
				errorsim = 1000 * (xi/7.5);
				xi -= 0.01;
				delay(100);
				if (command_queue_size() != 0) return; //abort
			}

			delay(2000);

			aw_finale = aw_inst;

			//measure
			ETAPE_H = 0;
			ETAPE_L = 0;
			MESURE_EN_COURS = 0;
			bip(10, 10, 10);
			ui_event(ui_event_measure_end);

			//fake save values
			formate_mesure();   // mise en forme des donnees avant sauvegarde
			write_tableau();    // ecriture dans le tableau de resultat

			_PAUSE_TH_MEASURE = 0;
		}
	}

	if (START_CALIB) {
		START_CALIB = 0;
#ifdef DEBUG_WS
		printf("calib started\n");
#endif
		
		if (ui_dialog_msg(ui_salt_1)) {
			return; //abort
		}

		while (!is_head_ok()) {
			if (ui_dialog_msg(ui_magnetic_down)) {
				return;//abort
			}
		}

		calib_auto_std();

		//wait
		while (MESURE_EN_COURS) {
			delay(DELAY_MESSAGE);
			if (command_queue_size() != 0) return; //abort
			if (!is_head_ok()) {
				ABORT_MEASURE = 1;
				return;
			}
		}

		if (opt.CALIBRATION_MODE < 128) {
			if (ERREUR == 0) {
				ui_event(ui_event_calibration_end);
			}
			else {
				log_info("calibration ended with error");
			}
			return;
		}; //calibration end

		if (ui_dialog_msg(ui_salt_2)) {
			return; //abort
		}

		while ((!is_head_ok())) {
			if (ui_dialog_msg(ui_magnetic_down)) {
				return;//abort
			}
		}

		calib_auto_std();

		//wait
		while (MESURE_EN_COURS) {
			delay(DELAY_MESSAGE);
			if (command_queue_size() != 0) return; //abort
			if (!is_head_ok()) {
				ABORT_MEASURE = 1;
				return;
			}
		}

		if (ERREUR == 0) {
			ui_event(ui_event_calibration_end);
		}
		else {
			log_info("calibration ended with error");
		}
	}
}

#endif