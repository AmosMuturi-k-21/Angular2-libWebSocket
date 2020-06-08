#include "stdio.h"
#include <math.h>

#define THE_VREF		2.5
#define G_LMP_VALMIR	1.33333	// the LMP stage gain at the VALMIR channel

FILE *fp;

double getR2_diff(double R1, double Vdiff)
{
	// ensure Vdiff is positive here
	if (Vdiff < 0)
		Vdiff = -Vdiff;

	return ( (R1 * (THE_VREF - Vdiff)) / Vdiff );
}

double CTemp(double R, double R0)
{
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

void pt_temp_read(double v) 
{
	double readV;

	readV = v;
	readV /= G_LMP_VALMIR;
	
	fprintf(fp,"%.3f\t%.3f\t", readV, v);
	
	double R2 = getR2_diff(1000.0, readV);
	fprintf(fp,"%.3f\t", R2);
	
	double result = (CTemp(R2, 1000.0));
	fprintf(fp,"%.3f\n", result);
			
}


int main(int argc, char *argv[])
{
	int i, n;
	double v, vstep;
	
	fp = fopen("valmir.txt","w");
	fprintf(fp,"VALMIR_at_2x10CON_[V]\tVdiff_at_ADS1256_[V]\tR2_[Ohm]\tTemp_[°C]\n");
	n = 500;
	vstep = 2.5 / (double)n;
	v= 0.0;
	for (i = 0; i < n; i++)
	{
		pt_temp_read(v);
		v += vstep;
	}
	fclose(fp);
	
	return(0);
}

