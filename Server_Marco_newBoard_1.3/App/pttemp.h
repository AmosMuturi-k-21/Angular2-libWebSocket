#pragma once
#include "include.h"
#include "addc.h"

#ifdef __cplusplus
extern "C" {
#endif

	void initialize_pt();
	double pt_temp_read();
	void pt_info();
	double pt_temp_test();
	double CTemp(double R, double R0);
	//double CTemp2(double Res, double type);
	/*double pt_temp_read2();*/

#ifdef __cplusplus
} // closing brace for extern "C"
#endif