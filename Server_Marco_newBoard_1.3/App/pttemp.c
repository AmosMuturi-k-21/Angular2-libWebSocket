#include "pttemp.h"
#include "vardef.h"

#define G_LMP_VALMIR		1.33333	// the LMP stage gain at the VALMIR channel


double getR2(double R1, double Vin, double Vout)
{
	return ((Vout * R1) / (Vin - Vout));
}


double getR2_diff(double R1, double Vdiff)
{
	// ensure Vdiff is positive here
	if (Vdiff < 0)
		Vdiff = -Vdiff;

	//return ( (R1 * (THE_VREF - Vdiff)) / Vdiff );
	return ( R1 * Vdiff / (THE_VREF - Vdiff) );
}


double CTemp(double R, double R0){
	//double A = 0.00385; //double A = 3.9083E-3;
	double A = 3.9083E-3;
	//double A = 0.00385;
	double B = -5.775E-7;
	double T;
	R = R / R0;
	T = 0.0 - A;
	T += sqrt((A*A) - 4.0 * B * (1.0 - R));
	T /= (2.0 * B);

	if (T>-200 && T<200) { //0,200
		return T;
	}
	else { //???
		T = 0.0 - A;
		T -= sqrt((A*A) - 4.0 * B * (1.0 - R));
		T /= (2.0 * B);
		return T;
	}
}

void initialize_pt() {

}



double aver_mrArr(void)
{
	double ret = 0.0;
	int i;

	for (i = 0; i < N_AVER_MIR; i++)
		ret += mrArr[i];
	ret /= (double)N_AVER_MIR;

	return (ret);
}




/*double pt_temp_read() {
	double R2 = getR2(1000.0, addc_read_v(VAL_INT), addc_read_v(VAL_MIR));
	double result = (CTemp(R2, 1000.0)) / 2;
	return result;
}*/
double pt_temp_read() 
{
	double readV;
	double mrav;
	static int i = 0;
	static int firstrun = 1;

	#ifdef SINGLE_ENDED
		double R2 = getR2(1000.0, addc_read_v(VAL_REF), addc_read_v(VAL_MIR));
	#else	// is def DIFFERENTIAL
	readV = -addc_read_v(VAL_MIR_DIFF);
	readV /= G_LMP_VALMIR;
	readV += valmir_offset;
	
	// store new data in the last-Nsamples-array
	mrArr[i++] = readV;
	// check rollover
	if (i == N_AVER_DET)
	{
		i = 0;
		firstrun = 0;
	}
	// calculate dtav as the average of the last-Nsamples-array
	mrav = aver_mrArr();
	// but if the array was not filled yet, assign to dtav last single sample
	if (firstrun)
		mrav = readV;
#ifdef NO_AVERAGING
	mrav = readV;
#endif

	double R2 = getR2_diff(1000.0, mrav);
	//printf("\nV at valmir pins 2-3: %.3f V, R2: %.2lf", readV, R2);
	#endif

	double result = (CTemp(R2, 1000.0));
	//printf("\tT: %.3f C", result);
	return result;
}

double pt_temp_test() {
	double R2 = getR2(1000.0, 2.5010, 1.3152);
	printf("1000 %f\n",CTemp(R2, 1000.0));

	R2 = getR2(999.0, 2.5010, 1.3152);
	printf("999 %f\n", CTemp(R2, 1000.0));

	R2 = getR2(1001.0, 2.5010, 1.3152);
	printf("1001 %f\n", CTemp(R2, 1000.0));

	/*printf("alg2\n");

	R2 = getR2(1000.0, 2.5010, 1.3152);
	printf("1000 %f\n", CTemp2(R2, 1000.0));

	R2 = getR2(999.0, 2.5010, 1.3152);
	printf("999 %f\n", CTemp2(R2, 1000.0));

	R2 = getR2(1001.0, 2.5010, 1.3152);
	printf("1001 %f\n", CTemp2(R2, 1000.0));*/
}

void pt_info() {
	//printf("valRef: %f\n", addc_read_v(VAL_REF));
	//printf("valMir: %f\n", addc_read_v(VAL_MIR));
	//printf("R1: %f\n", 1000.0);
	double readV;

	#ifdef SINGLE_ENDED
		double R2 = getR2(1000.0, addc_read_v(VAL_REF), addc_read_v(VAL_MIR));
	#else	// is def DIFFERENTIAL
		readV = -addc_read_v(VAL_MIR_DIFF);
		readV /= G_LMP_VALMIR;
		readV += valmir_offset;

		double R2 = getR2_diff(1000.0, readV);
	#endif
	
	printf("VAL_MIR_DIFF: %.3f V\n", readV);
	printf("TMIR: %.3f\n", CTemp(R2,1000.0));
}

double pt_temp_single_read(void)
{
	double readV;
	double mrav;
	static int i = 0;
	static int firstrun = 1;

#ifdef SINGLE_ENDED
	double R2 = getR2(1000.0, addc_read_v(VAL_REF), addc_read_v(VAL_MIR));
#else	// is def DIFFERENTIAL
	readV = -addc_read_v(VAL_MIR_DIFF);
	readV /= G_LMP_VALMIR;
	readV += valmir_offset;

	double R2 = getR2_diff(1000.0, readV);
	//printf("\nV at valmir pins 2-3: %.3f V, R2: %.2lf", readV, R2);
#endif

	double result = (CTemp(R2, 1000.0));
	//printf("\tT: %.3f C", result);
	return result;
}