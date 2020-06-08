#include "sensors.h"
#include "ad595.h"
#include "pttemp.h"
#include "addc.h"


float delta_tmirror = 0;
float slope_tmirror = 1.0f;
float delta_tsurface = 0;
float slope_tsurface = 1.0f;

void initialize_sensors() {

}

double temp_surface() {
#ifdef SINGLE_ENDED
	return (ad595_temp_read(addc_read_v(VAL_SURF))*slope_tsurface) + delta_tsurface;
#else	// is def DIFFERENTIAL
	return (ad595_temp_read(addc_read_v(VAL_SURF_DIFF))*slope_tsurface) + delta_tsurface;
#endif
	
}

double temp_surface_nocalib() {
	
#ifdef SINGLE_ENDED
	return ad595_temp_read(addc_read_v(VAL_SURF));
#else	// is def DIFFERENTIAL
	return ad595_temp_read(addc_read_v(VAL_SURF_DIFF));
#endif
}
double temp_mirror() {
	return (pt_temp_read() * slope_tmirror) + delta_tmirror;
}
double temp_mirror_nocalib() {
	return pt_temp_read();
}