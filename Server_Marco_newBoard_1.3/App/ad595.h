#pragma once
#include "include.h"
#include "addc.h"

#ifdef __cplusplus
extern "C" {
#endif

	void initialize_ad595();
	double ad595_temp_read(double v);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif