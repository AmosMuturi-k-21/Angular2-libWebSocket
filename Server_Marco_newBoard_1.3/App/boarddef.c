#include "vardef.h"

//#define PRINT_DEBUG

//bip not fan!
/*void sort_moteur_bip(void) {
	printf("fan speed: %d", moteur_bip);
	fan_set_perc(moteur_bip);
}*/

int _PAUSE_TH_MEASURE = 0;

void bip(char volume , char boh , char duration) { //sync or async?
		buzzer(200);
		//delay(1000);
}

//min -6000 max 6000
void sort_peltier(void) {
	//peltier_set_power(map(peltier,-6000,6000,-2048, 2048));
	peltier_set_power(map(peltier, -32000, 32000, -4096, 4096));
}

//get date
time_t  recup_date() {
	return time(NULL);
}

void set_date(time_t date) {
	stime(&date);
}

//@TODO
void sort_regu() {
	//log_error("sort_regu");
	printf("ERR: sort_regu not implemented\n");
}

#define FREQ_CALL 60.0
#define FREQ_STEP 1000.0/FREQ_CALL
extern double MirrorTempTimer;
static PI_THREAD(th_gestion_noyau)
{
	double tm_start2 = time_ms();
	double diff2 = 0;
	double temp = time_ms();

	MirrorTempTimer = time_ms();

	//call every 0.5 second
	while (1) {
		if (!_PAUSE_TH_MEASURE) {
			temp = time_ms();
			diff2 = temp - tm_start2;
			if (diff2 >= FREQ_STEP) {
				noyau_temps_reel();
				tm_start2 = temp - (diff2 - FREQ_STEP); //set offset
				//printf("diff2 = %f \n", diff2);
			}
			delay(1);
		}
		else
			delay(100);
	}
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\n"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

char* TASKS[] = {"wait","equilibrium","research","measure"};
static PI_THREAD(th_debug_values)
{
	delay(1000);
	char firsttime = 1;
	while (1) {
		delay(450);
		if (MENU == 0 && ERREUR == 0) {
			/*if (firsttime) {
				for (int i = 11; i; i--) printf("\n");
				firsttime = 0;
			}
			printf("\33[%dA", (int)11);*/
			printf("\nreflexion = %f\n", reflexion);
			printf("detection = %f\n", detection);
			//printf("reflexion = %fV\n", addc_read_v(VAL_DET));
			printf("surface T = %f \u2103C\n", tsurf);
			printf("mirror T = %f \u2103C\n", tmir);
			printf("fan = %f%%\n", fan_get_perc());
			printf("peltier = %f%%\n", perltier_get_perc());
			printf("CLEAN_MIRROR = %d\n", CLEAN_MIRROR);
			printf("CINETIQUE = %d\n", CINETIQUE);
			printf("MESURE_EN_COURS = %d\n", MESURE_EN_COURS);
			printf("task = %s\n", TASKS[etape]);
			printf("magnetic = %d\n", DEM_AUTO);

			printf("delta_tmir = %f\n", delta_tmir);
			printf("pente_tmir = %f\n", pente_tmir);

			printf("\netat = "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(GET_ETAT));
			printf("config = "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(GET_CONFIG));
			printf("control_0 = "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(GET_CONTROL_0));
			printf("control_1 = "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(GET_CONTROL_1));
			printf("interface = "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(GET_INTERFACE));
			printf("optimize = "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(GET_OPTIMIZE));

			printf("\ntime_etape = %d 1/2s\n", time_etape);
			printf("time_mesure = %ds\n", time_mesure);
			/*printf("synchro = %d\n", synchro);*/
			printf("\nxi = %f%%\n",xi);
			printf("aw_inst = %d\n", aw_inst);
			printf("aw_moy = %d\n", aw_moy);
			printf("aw_flash = %d\n", aw_flash);
			printf("aw_finale = %d\n", aw_finale);
			printf("aw_int = %d\n\n", aw_int);
		}
		else if (ERREUR) {
			//printf("\33[%dA", (int)1);
			printf("status = ERREUR\n");
			printf("CLEAN_MIRROR = %d\n", CLEAN_MIRROR);
			printf("nb_erreur = %d\n", nb_erreur);
		}
		else if (MENU) {
			//printf("\33[%dA", (int)1);
			printf("MENU\n");
		}
	}
}



void oldboard_start() {
	startI2C();
	piThreadCreate(th_gestion_noyau);
#ifdef PRINT_DEBUG
	piThreadCreate(th_debug_values);
#endif
}