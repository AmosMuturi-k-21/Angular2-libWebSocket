#include "board.h"

unsigned long tms = 0;

PI_THREAD(th_time)
{
	piHiPri(50);
	tms = 0;
	long time;
	long oldtime;
	g_tms(oldtime);
	while (1) {
		delayMicroseconds(500);
		g_tms(time);
		long result = time - oldtime;
		if (result > 0) {
			tms += result;
			g_tms(oldtime);
			//printf("result = %ld\n", result);
		}else
		if (result < 0)
		{
			result = 1000 + result;
			tms += result;
			g_tms(oldtime);
			//printf("result = %ld\n", result);
		}
	}
}

void initialize_board() {
	if (wiringPiSetup()) {
		log_error("init board");
		return;
	}
	piHiPri(90);

	//other setup
	piThreadCreate(th_time);

	//hardware setup
	/*initialize_opt();
	initialize_buzzer();
	initialize_buttons();
	initialize_peltier();
	initialize_sensors();
	initialize_lcd();
	initialize_addc();
	initialize_fan();
	initialize_pt();
	initialize_ad595();
	initialize_web();

*/

	//initialize_opt();
	//initialize_web();
	//initialize_buzzer();
	//initialize_buttons();
	//initialize_peltier();
	//initialize_sensors();
	//initialize_lcd();
	//initialize_addc();
	//initialize_fan();
	//initialize_pt();
	//initialize_ad595();

	//changed by ShanSong: initialize the addc before initialize peltier.
	initialize_opt();
	initialize_web();
	initialize_buzzer();
	initialize_buttons();
	initialize_sensors();
	initialize_lcd();
	initialize_addc();
	initialize_peltier();
	printf("\n*********** initialize peltier");
	initialize_fan();
	initialize_pt();
	initialize_ad595();

	/*MeCom*/
	int i = 0;
	uint8_t str[256];
	while (1) {
		if (i > 20) break;
		if (ComPort_Open(i, 57600)==0 && MeCom_GetIdentString(0,str)) {
			printf("\nMeCom open at tty=[%d] %s\n",i,str);
			MeCom_ResetDevice(0);
			printf("\nMeCom_ResetDevice issued\n");
			sleep(1);
			ComPort_Close();
			printf("\nComPort closed\n");
			ComPort_Open(i, 57600); //reopen port
			MeCom_SetTemp(0, opt.SAMPLE_TEMP); //0 address broadcast
			printf("\nMeCom_SetTemp issued\n");
			break;
		}
		i++;
	};

	printf("\nMeCom initial SetTemp exited\n");	// this is to check if the MeCom errors on screen derive from the code above...

	if (i >= 20) {
		printf("error opening MeCom\n");
	}
		

	delay(300);
}