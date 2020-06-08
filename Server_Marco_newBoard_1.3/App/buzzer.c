#include "buzzer.h"
#include "opt.h"

static unsigned int buzzer_delay = 0;
//static unsigned int FREQ = 3000;

void buzzer_set_freq(int f) {
	opt.BUZZ_FREQ = f;
	if (opt.BUZZ_FREQ > 5000) {
		opt.BUZZ_FREQ = 5000;
	}
}

	void initialize_buzzer() {
		pinMode(GPIO_BUZZER, OUTPUT);
		softToneCreate(GPIO_BUZZER);
		buzzer_stop();
	}
	void buzzer_start() {
		softToneWrite(GPIO_BUZZER, opt.BUZZ_FREQ);
	}
	void buzzer_stop() {
		softToneWrite(GPIO_BUZZER, 0);
	}

	void buzzer_song(int list[]) {
		int i = 0;
		while (list[i] != 0) {
			opt.BUZZ_FREQ = list[i];
			unsigned int dur = list[i++];

			/*if (dur > 1000)*/ dur = 100 + dur % (1000-100);
			/*if (FREQ > 5000)*/ opt.BUZZ_FREQ = 300 + (int)opt.BUZZ_FREQ % (5000-300);

			printf("freq = %d\nlen = %d\n", opt.BUZZ_FREQ,dur);

			buzzer_start();
			delay(dur);
			buzzer_stop();

			i++;
		}
	}

	static PI_THREAD(th_buzzer)
	{
		buzzer_start();
		delay(buzzer_delay);
		buzzer_stop();
		buzzer_delay = 0;
	}

	static PI_THREAD(th_buzzer_alert)
	{
		for (int i = 0; i < 2; i++) {
			buzzer_start();
			delay(200);
			buzzer_stop();
			delay(200);
		}

		buzzer_delay = 0;
	}
	static PI_THREAD(th_buzzer_err)
	{
		for (int i = 0; i < 5; i++) {
			buzzer_start();
			delay(100);
			buzzer_stop();
			delay(100);
		}

		buzzer_delay = 0;
	}

	void buzzer(int ms) {
		if (buzzer_delay != 0) return;
		buzzer_delay = ms;
		piThreadCreate(th_buzzer);
	}

	void buzzer_error() {
		if (buzzer_delay != 0) return;
		buzzer_delay = 1;
		piThreadCreate(th_buzzer_err);
	}

	void buzzer_alert() {
		if (buzzer_delay != 0) return;
		buzzer_delay = 1;
		piThreadCreate(th_buzzer_alert);
	}