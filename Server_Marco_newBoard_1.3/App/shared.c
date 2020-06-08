#include "shared.h"

void log_error(char *msg) {
	buzzer_error();
	printf("ERR: %s\n", msg);
}
void log_warning(char *msg) {
	buzzer_alert();
	printf("WAR: %s\n", msg);
}
void log_info(char *msg) {
	printf("INF: %s\n", msg);
}

unsigned long time_ms() {
	return tms;
}