#include "peltier.h"
#include "addc.h"

#define MAX_POWER 4096.0f
//#define MAX_POWER 8192.0f
#define MIN_POWER (-MAX_POWER)

#define MAX_POWER16 32000.0f
#define MIN_POWER16 (-MAX_POWER16)

#define POWER_LIMIT (MAX_POWER/(DVR_MAX_OUTPUT_A/PELTIER_MAX_A))
#define MIN_POWER_LIMIT (-(MAX_POWER/(DVR_MAX_OUTPUT_A/PELTIER_MAX_A)))

static int peltier_direction = 0;
static int peltier_pow = 9999;

//#define PELTIER_DEBUG

//void initialize_peltier() {
//	pinMode(PELTIER_PWM, PWM_OUTPUT);
//	pwmSetMode(PWM_MODE_MS);
//	pwmSetRange(MAX_POWER);
//	pwmSetClock(128); //128
//	pinMode(PELTIER_DIR, OUTPUT);
//	//pinMode(PELTIER_DIR2, OUTPUT);
//	pwmWrite(PELTIER_PWM, 0);
//
//	peltier_set_power(0); //disable peltier at startup
//}

//changed by ShanSong: use DAC instead of PWM
void initialize_peltier() {
	printf("\n*********** inside initialize peltier");
	peltier_set_power(0); //disable peltier at startup
}

//void peltier_set_power(float power) {
//	if (power == peltier_pow) return;
//
//	peltier_pow = power;
//	if (peltier_pow < 0) {
//		peltier_pow *= -1;
//		peltier_direction = 1;
//	}
//	else peltier_direction = 0;
//
//	if (peltier_pow > MAX_POWER)
//		peltier_pow = MAX_POWER;
//
//	if (peltier_pow > POWER_LIMIT)
//		peltier_pow = POWER_LIMIT;
//
//#ifdef PELTIER_DEBUG
//	if (peltier_direction == 0)
//		printf("peltier = %f%%\n", ((power / MAX_POWER)*100));
//	else
//		printf("peltier = -%f%%\n", ((power / MAX_POWER) * 100));
//#endif
//
//	digitalWrite(PELTIER_DIR, peltier_direction);
//	pwmWrite(PELTIER_PWM, (int)peltier_pow);
//	/*printf("\n%d\n", peltier_pow);
//	printf("%f\n", POWER_LIMIT);*/
//}

//changed by ShanSong: use DAC instead of PWM
void peltier_set_power(float power) {
	if (power == peltier_pow) return;

	peltier_pow = power;
	if (peltier_pow < 0) {
		peltier_pow *= -1;
		peltier_direction = 1;
	}
	else peltier_direction = 0;

	if (peltier_pow > MAX_POWER)
		peltier_pow = MAX_POWER;

	if (peltier_pow > POWER_LIMIT)
		peltier_pow = POWER_LIMIT;

#ifdef PELTIER_DEBUG
	if (peltier_direction == 0)
		printf("peltier = %f%%\n", ((power / MAX_POWER) * 100));
	else
		printf("peltier = -%f%%\n", ((power / MAX_POWER) * 100));
#endif

	//digitalWrite(PELTIER_DIR, peltier_direction);
	//pwmWrite(PELTIER_PWM, (int)peltier_pow);
	/*printf("\n%d\n", peltier_pow);
	printf("%f\n", POWER_LIMIT);*/

	//power is from -4096 to +4096, need t convert it to voltage output of the DAC.
	//MAX1969 peltier current is calculated as
	//ITEC = (VOS1 - VCS) / RSENSE = (VCTLI - 1.50) / (10 x RSENSE)
	//Rsense = 50mOhm
	//max peltier current is 1.4A
	//so the max VCTLI range is 1.5+/-0.7V = 0.8~2.2V.
	float voltage = power / MAX_POWER * 0.7f + 1.5f;  
	//printf("power = %f%%\t", (power));
	//printf("\tvoltage = %.3f V\t", (voltage));
	printf("\tp %.1f\t%.4f", power, voltage);
	addc_dac_set(voltage);
	//printf("\n*********** exit set_power");
	
}


void peltier_set_power16(float power) {
	if (power == peltier_pow) return;

	peltier_pow = power;
	if (peltier_pow < 0) {
		peltier_pow *= -1;
		peltier_direction = 1;
	}
	else peltier_direction = 0;

	if (peltier_pow > MAX_POWER16)
		peltier_pow = MAX_POWER16;

	if (peltier_pow > POWER_LIMIT)
		peltier_pow = POWER_LIMIT;

	//power is from -32000 to +32000, need t convert it to voltage output of the DAC.
	//MAX1968 peltier current is calculated as
	//ITEC = (VOS1 - VCS) / RSENSE = (VCTLI - 1.50) / (10 x RSENSE)
	//Rsense = 50mOhm
	//max peltier current is 1.4A
	//so the max VCTLI range is 1.5+/-0.7V = 0.8~2.2V.
	float voltage = power / MAX_POWER16 * 0.7f + 1.5f;
	//printf("power = %f%%\t", (power));
	//printf("\tvoltage = %.3f V\t", (voltage));
	printf("\tp %.1f\t%.4f", power, voltage);
	addc_dac_set(voltage);
	//printf("\n*********** exit set_power");

}


void peltier_set_perc(float power) {
	if (power < -100) power = -100;
	if (power > 100) power = 100;
	peltier_set_power((POWER_LIMIT/100.0F)*power);
}

unsigned int peltier_get_power() {
	return peltier_pow;
}

float perltier_get_perc() {
	int temp = peltier_pow;
	int dir = peltier_direction;

	if (dir == 0)
		return ((temp / MAX_POWER) * 100.0f);
	else
		return -((temp / MAX_POWER) * 100.0f);
}