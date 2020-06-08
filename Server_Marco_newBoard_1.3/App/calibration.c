#include "calibration.h"
#include "pttemp.h"

void initialize_calib() {

}

//static int num_pt_calib = 2;


/*
void calcul_calib_2(void)
{
	//#define pas aw_flash
#define aw_rech aw_flash 


	xdata float tmir_1;
	xdata float tmir_2;
	xdata float tmir_etal_1;
	xdata float tmir_etal_2;
	xdata float delta_tmir_new;
	xdata float pente_tmir_new;
	xdata short pas;

	if (num_pt_calib == 0)    // memorisation du 1er point de calibration
	{
		num_pt_calib++;
		tsurf_1 = tsurf;
		aw_mes_1 = aw_finale_brut;
		aw_rech_1 = aw_rech;
		message = 25;
	}
	else    // memorisation du second point + etalonnage
	{
		num_pt_calib = 0;
		tsurf_2 = tsurf;
		aw_mes_2 = aw_finale_brut;
		aw_rech_2 = aw_rech;


		if (aw_rech_2 == aw_rech_1)  // si sels identiques trouvés on rejette la calibration
		{
		Erreur_calib:
			message = 21;
			ERREUR == 1;
			//bip_modul5();  
			return;
		}

		if (abs_macro((short)aw_mes_1 - (short)aw_mes_2) < 80)  // si les 2 points sont trés proches on le signale 
		{
			message = 20;
			ERREUR == 1;
			//bip_modul5();
		}




		tmir_2 = tsurf_2;
		pas = -5000;
		do    // recuperation temperature miroir mesurée du 2 eme point, par recalcul inverse c'est plus précis qu'une simple mémorisation de la dernière valeur  
		{
			tmir_2 += (float)pas / 1000.0f;
			if ((fct_aw(tmir_2, tsurf_2) < aw_mes_2) && (pas < 0)) pas /= -2;
			if ((fct_aw(tmir_2, tsurf_2) > aw_mes_2) && (pas > 0)) pas /= -2;
		} while (pas != 0);

		tmir_1 = tsurf_1;
		pas = -5000;
		do  // recuperation temperature miroir mesuré du 1 er point
		{
			tmir_1 += (float)pas / 1000.0f;
			if ((fct_aw(tmir_1, tsurf_1) < aw_mes_1) && (pas < 0)) pas /= -2;
			if ((fct_aw(tmir_1, tsurf_1) > aw_mes_1) && (pas > 0)) pas /= -2;
		} while (pas != 0);


		tmir_etal_2 = tmir_2;
		pas = 1000;
		do   // calcul temperature miroir théorique du 2 eme point
		{
			tmir_etal_2 += (float)pas / 1000.0f;
			if ((fct_aw(tmir_etal_2, tsurf_2) < aw_rech_2) && (pas < 0)) pas /= -2;
			if ((fct_aw(tmir_etal_2, tsurf_2) > aw_rech_2) && (pas > 0)) pas /= -2;
		} while (pas != 0);

		tmir_etal_1 = tmir_1;
		pas = 1000;
		do   // calcul temperature miroir théorique du 1 er point
		{
			tmir_etal_1 += (float)pas / 1000.0f;
			if ((fct_aw(tmir_etal_1, tsurf_1) < aw_rech_1) && (pas < 0)) pas /= -2;
			if ((fct_aw(tmir_etal_1, tsurf_1) > aw_rech_1) && (pas > 0)) pas /= -2;
		} while (pas != 0);

		if (ABS(tmir_1 - tmir_2) < 0.5f)  // si points trop proches on rejette la calibration
		{
			goto Erreur_calib;
		}

		pente_tmir_new = pente_tmir * (tmir_etal_1 - tmir_etal_2) / (tmir_1 - tmir_2);  // calcul de la nouvelle pente miroir
		pas = 1000;
		delta_tmir_new = delta_tmir;
		do  // calcul du nouvel offset miroir
		{
			delta_tmir_new += (float)pas / 1000.0f;
			tmir_etal_1 = pente_tmir_new / pente_tmir * (tmir_2 - delta_tmir) + delta_tmir_new;

			if ((tmir_etal_1 < tmir_etal_2) && (pas < 0)) pas /= -2;
			if ((tmir_etal_1 > tmir_etal_2) && (pas > 0)) pas /= -2;

		} while (pas != 0);

		delta_tmir = delta_tmir_new;
		pente_tmir = pente_tmir_new;

		if ((pente_tmir < 0.94f) || (pente_tmir > 1.02f))  // detection parametre hors tolérance
		{
			message = 22;
			ERREUR == 1;
			//bip_modul5();  
			//num_pt_calib = 0;  
			//return;
		}

		if ((delta_tmir < -5.0f) || (delta_tmir > 5.0f))  // detection parametre hors tolérance
		{
			message = 22;
			ERREUR == 1;
			//bip_modul5();  
			//num_pt_calib = 0;  
			//return;
		}

		if (ERREUR == 0)
		{
			message = 23;
			//bip_modul6();  
		}
	}
}*/