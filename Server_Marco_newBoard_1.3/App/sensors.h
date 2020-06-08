#pragma once
#include "include.h"

#ifdef __cplusplus
extern "C" {
#endif

	void initialize_sensors();

	double temp_surface();
	double temp_mirror();
	double temp_mirror_nocalib();
	double temp_surface_nocalib();

#ifdef __cplusplus
} // closing brace for extern "C"W
#endif