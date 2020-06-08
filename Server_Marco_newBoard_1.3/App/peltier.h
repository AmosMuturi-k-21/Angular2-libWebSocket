#pragma once
#include "include.h"

#ifdef __cplusplus
extern "C" {
#endif

	void initialize_peltier();
	void peltier_set_power(float);
	void peltier_set_power16(float);
	void peltier_set_perc(float);
	unsigned int peltier_get_power();
	float perltier_get_perc();

#ifdef __cplusplus
} // closing brace for extern "C"
#endif