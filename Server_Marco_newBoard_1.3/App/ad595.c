#include "ad595.h"

void initialize_ad595() {

}
double ad595_temp_read(double v) {
	return (v*1000)/10.0; //addc_read_v(VAL_SURF)
}