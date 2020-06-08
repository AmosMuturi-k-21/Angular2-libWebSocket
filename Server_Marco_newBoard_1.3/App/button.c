#include "button.h"

//#define BUZZ_ON_PRESS
//#define DEBUG_ON_PRESS

/*typedef struct button_ctx {
	unsigned long time;
	int pressed;
	int gpio;
	int sim_press;
};*/

static int menu = 1;
static int ok = 1;
static int mag = 1;

static int sim_press_menu = 0;
static int sim_press_ok = 0;
static int sim_press_mag = 0;

static int sim_press_menu_val = 0;
static int sim_press_ok_val = 0;
static int sim_press_mag_val = 0;

static unsigned long tmenu = 0;
static unsigned long tok = 0;
static unsigned long tmag = 0;

static void int_menu() {
	if (abs(tms - tmenu) > 300) {
		menu++;
		tmenu = tms;

#ifdef BUZZ_ON_PRESS
		buzzer(100);
#endif
#ifdef DEBUG_ON_PRESS
		log_info("MENU button");
#endif
	}
}
static void int_mag() {
	if (abs(tms - tmag) > 300) {
		mag++;
		tmag = tms;

#ifdef BUZZ_ON_PRESS
		buzzer(100);
#endif
#ifdef DEBUG_ON_PRESS
		log_info("MAGNETIC button");
#endif
	}
}
static void int_ok() {
	if (abs(tms - tok) > 300) {
		ok++;
		tok = tms;

#ifdef BUZZ_ON_PRESS
		buzzer(100);
#endif
#ifdef DEBUG_ON_PRESS
		log_info("OK/CALIB button");
#endif
	}
}

void initialize_buttons() {
	pinMode(GPIO_MENU, INPUT);
	pullUpDnControl(GPIO_MENU,PUD_UP);
	wiringPiISR(GPIO_MENU, INT_EDGE_FALLING, int_menu);

	pinMode(GPIO_OK, INPUT);
	pullUpDnControl(GPIO_OK, PUD_UP);
	wiringPiISR(GPIO_OK, INT_EDGE_FALLING, int_ok);

	pinMode(GPIO_MAG, INPUT);
	pullUpDnControl(GPIO_MAG, PUD_UP);
	wiringPiISR(GPIO_MAG, INT_EDGE_FALLING, int_mag);

	delay(100);
}

int button(int but) {
	switch (but)
	{
	case GPIO_MENU: return menu>1 ? --menu : 0;
	case GPIO_OK: return ok>1 ? --ok : 0;
	case GPIO_MAG: return mag>1 ? --mag : 0;
	default:
		return 0;
	}
}

void button_clear(int but) {
	switch (but)
	{
	case GPIO_MENU: menu = 1;
	case GPIO_OK: ok = 1;
	case GPIO_MAG: mag = 1;
	default:
		return 0;
	}
}

void button_clear_all() {
	menu = 1;
	ok = 1;
	mag = 1;
}

int button_read(int but) {
	switch (but)
	{
	case GPIO_MENU: return menu>1 ? menu : 0;
	case GPIO_OK: return ok>1 ? ok : 0;
	case GPIO_MAG: return mag>1 ? mag : 0;
	default:
		return 0;
	}
}

int button_is_press(int but) {
	switch (but)
	{
	case GPIO_MENU: return sim_press_menu ? sim_press_menu_val : digitalRead(but); break;
	case GPIO_OK: return sim_press_ok ? sim_press_ok_val : digitalRead(but); break;
	case GPIO_MAG: return sim_press_mag ? sim_press_mag_val : digitalRead(but); break;
	default:
		return 0;
	}
}

void simulate_press(int but, int status,int time) {
	switch (but)
	{
	case GPIO_MENU: sim_press_menu = 1; sim_press_menu_val = status; delay(time); sim_press_menu = 0; break;
	case GPIO_OK: sim_press_ok = 1; sim_press_ok_val = status; delay(time); sim_press_ok = 0; break;
	case GPIO_MAG: sim_press_mag = 1; sim_press_mag_val = status; delay(time); sim_press_mag = 0; break;
	default:
		return 0;
	}
}

void simulate_status(int but, int status) {
	switch (but)
	{
	case GPIO_MENU: sim_press_menu = 1; sim_press_menu_val = status;  break;
	case GPIO_OK: sim_press_ok = 1; sim_press_ok_val = status; break;
	case GPIO_MAG: sim_press_mag = 1; sim_press_mag_val = status; break;
	default:
		return 0;
	}
}

void simulate_status_off(int but) {
	switch (but)
	{
	case GPIO_MENU: sim_press_menu = 0;  break;
	case GPIO_OK: sim_press_ok = 0;  break;
	case GPIO_MAG: sim_press_mag = 0;  break;
	default:
		return 0;
	}
}