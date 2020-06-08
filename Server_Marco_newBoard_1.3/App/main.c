#include "board.h"
#include "vardef.h"
#include "proto.h"

#define cmp(x)(strcmp(str,x)==0)


/*
commands:
buz -> start buzzer
peltier -> 50% heating and then 50% cooling
peltierauto -> start pelteir with % (+ or -)
fan -> set fan speed %

*/

void test() {
	char str[100] = { 0 };
	printf("test mode\n");
	while (1) {
		delay(1);
		printf("\n> ");
		fflush(stdout);
		scanf("%s", str);

		if (cmp("q") || cmp("quit") || cmp("exit")) {
			return;
		}
		else
			if (cmp("startup")) {
				int val = 0;
				printf("set value\n[0] disable\n[1] enable automatic startup of the program\n[2] enable automatic startup of the program + browser startup inside the rpi\n->");
				fflush(stdout);
				scanf("%d", &val);
				boot_at_startup(val);
			}
		else
			if (cmp("slopesurf")) {
				float val = 0;
				printf("set value->");
				fflush(stdout);
				scanf("%f", &val);
				opt.SLOPE_TEMP_SURF = val;
				sync_opt();
			}
			else
				if (cmp("slopemir")) {
					float val = 0;
					printf("set value->");
					fflush(stdout);
					scanf("%f", &val);
					opt.SLOPE_TEMP_MIRROR = val;
					sync_opt();
				}
		else
			if (cmp("deltasurf")) {
				float val = 0;
				printf("set value->");
				fflush(stdout);
				scanf("%f", &val);
				opt.DELTA_TEMP_SURF = val;
				sync_opt();
			}
			else
				if (cmp("deltamir")) {
					float val = 0;
					printf("set value->");
					fflush(stdout);
					scanf("%f", &val);
					opt.DELTA_TEMP_MIRROR = val;
					sync_opt();
				}
				else
					if (cmp("deltareflex")) {
						float val = 0;
						printf("set value->");
						fflush(stdout);
						scanf("%f", &val);
						opt.DELTA_VALUE_IR = val;
						sync_opt();
					}
		else
			if (cmp("buz") || cmp("buzzer")) {
				buzzer(500);
			}else
				if (cmp("song")) {
					int list[] = {1000,45};
					buzzer_song(printf);
				}
				else
				if (cmp("setbuz") || cmp("setbuzzer")) {
					int perc = 0;
					printf("set [0..5000] ->");
					fflush(stdout);
					scanf("%d", &perc);
					buzzer_set_freq(perc);
				}
				else
			if (cmp("line")) {
				printf("\33[%dA", (int)8);
			}
			else
				if (cmp("button") || cmp("b")) {
					printf("menu: %d\n", button(GPIO_MENU));
					printf("ok: %d\n", button(GPIO_OK));
					printf("mag: %d\n", button(GPIO_MAG));
				}
				else
					if (cmp("peltierauto")) {
						initialize_peltier();
						int perc = 0;
						printf("set %% ->");
						fflush(stdout);
						scanf("%d", &perc);
						//printf("%d%%\n", perc);
						peltier_set_perc(perc);
					}
				else
					if (cmp("peltiertest")) {
						//pwmWrite(PELTIER_PWM, 0);
						//pinMode(PELTIER_PWM, OUTPUT);
						//digitalWrite(PELTIER_PWM,HIGH);
						//changed by ShanSong, remove the above old code.
						initialize_peltier();
					}else
					if (cmp("peltier")) {
						initialize_peltier();
						int perc = 0;
						printf("set %% ->");
						fflush(stdout);
						scanf("%d", &perc);

						printf("\n\n\n");

						peltier_set_perc(perc);
						for (int i = 0; i < 20; i++) {
							printf("\33[%dA", (int)3);
							printf("heating %d%% : %ds\n", perc,20-i);
							printf("mirror: %fC\n", temp_mirror());
							printf("surface: %fC\n", temp_surface());
							delay(1000);
						}

						peltier_set_perc(-perc);
						for (int i = 0; i < 20; i++) {
							printf("\33[%dA", (int)3);
							printf("cooling %d%% : %ds\n", perc, 20 - i);
							printf("mirror T: %fC\n", temp_mirror());
							printf("surface T: %fC\n", temp_surface());
							delay(1000);
						}
						peltier_set_perc(0);
						printf("\n");
					}
					else
						if (cmp("fan")) {
							int perc = 0;
							printf("set %% ->");
							fflush(stdout);
							scanf("%d", &perc);
							fan_set_perc(perc);
						}
					else
						if (cmp("addc")) {
							addc_test();
						}
					else
					if (cmp("tempmir")) {
						printf("calib: %f\n", temp_mirror());
						printf("nocalib: %f\n", temp_mirror_nocalib());

						acqui_miroir();
						printf("acqui_miroir(): %f\n", tmir);
					}else
					if (cmp("ptinfo")) {
						pt_info();
					}
					else
					if (cmp("tempsurf")) {
						printf("calib: %f\n", temp_surface());
						printf("nocalib: %f\n", temp_surface_nocalib());

						acqui_surface();
						printf("acqui_surface(): %f\n", tsurf);
					}
					else
						if (cmp("reflex")) {
							acqui_reflexion();
							printf("acqui_reflexion(): %f\n", reflexion);
						}
					else
						if (cmp("lcd")) {
							lcdprintf("Hello World %d\n",10);
						}
						else {
							printf("ERR cmd\n");
						}

							str[0] = 0;
	}
}


static PI_THREAD(fifomanager) {
	int fd;
	char * myfifo = "/tmp/app";
	/* create the FIFO (named pipe) */
	mkfifo(myfifo, 7777);

	char str[128];
	fd = open(myfifo, O_RDONLY);
	if (fd < 0) log_error("opening fifo");
	while (fd && 1) {
		memset(str, 0, sizeof(str));
		int rd = read(fd, str, 1024);
		if (rd > 0) {
			str[strlen(str) - 1] = 0; //remove \n

			//int err = 0;
			printf("CMD: %s -> ", str);

			if (cmp("press ok")) {
				printf("OK\n");
				simulate_press(GPIO_OK, 0, 150);
			}
			else 
			if (cmp("get reflex")) {
				printf("OK\n");
				acqui_reflexion();
				printf("reflexion = %f\n", reflexion);
			}
				else
				if (cmp("press both")) {
				printf("OK\n");
				simulate_status(GPIO_OK,0);
				simulate_status(GPIO_MENU, 0);
				delay(150);
				simulate_status_off(GPIO_OK);
				simulate_status_off(GPIO_MENU);
			}else
				if (cmp("press menu")) {
					printf("OK\n");
					simulate_press(GPIO_MENU, 0, 150);
				}
				else
					if (cmp("set mag 0")) {
						printf("OK\n");
						simulate_status(GPIO_MAG, 0);
					}
					else
						if (cmp("set mag 1")) {
							printf("OK\n");
							simulate_status(GPIO_MAG, 1);
						}
						else {
							printf("ERR\n");
							//err = 1;
						}
		}
		delay(500);
	}
	close(fd);
}


int main(int argc, char *argv[])
{
	time_t t;
	/* Intializes random number generator */
	srand((unsigned)time(&t));

	printf("Server_NewBoard_1.1\n");

	// read hardware configs
	read_hw_ini_file();

	if (argc >= 2 && strcmp(argv[1], "test") == 0) {
		initialize_opt();
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

		test();
		return 0;
	}

	//printf("map %f\n", map(6000, -6000, 6000, -2048, 2048));
	initialize_board();

	piThreadCreate(fifomanager);

	delay(1000);

	

	addc_test();
	printf("tempsurf: %f\n", temp_surface_nocalib());
	printf("tempmirr: %f\n", temp_mirror_nocalib());
	printf("button menu: %d\n", (unsigned int)button_is_press(GPIO_MENU));
	printf("button ok: %d\n", (unsigned int)button_is_press(GPIO_OK));
	printf("button mag: %d\n", (unsigned int)button_is_press(GPIO_MAG));

	//test();

	g_deltaTemp = -99.9;	// set to a unreal value for test, COMMENT after debug
	Start();

	return 0;
}