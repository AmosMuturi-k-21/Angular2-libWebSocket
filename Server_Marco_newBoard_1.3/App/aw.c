#include "aw.h"
#include "sensors.h"

 /* uint16_t aw_inst;
  uint16_t aw_flash;
  uint16_t aw_finale;
  uint16_t aw_int;

uint16_t fct_aw(float miroir,float surface)
{
	float aw;

	miroir += 273.15f;
	surface += 273.15f;

	aw = -6096.9385f*(1.0f / miroir - 1.0f / surface);
	aw -= 0.02711193f*(miroir - surface);
	aw += 0.00001673952f*((miroir*miroir) - (surface*surface));
	aw += 2.433502f*logf(miroir / surface);
	aw = expf(aw);

	return((uint16_t)((1000.0f*aw) + 0.5f));    //codage de l'aw sur 16 bits    0 à 1000
}*/

/*void calc_aw(void)
{


	aw_inst = fct_aw(temp_mirror_nocalib(), temp_surface_nocalib());

	//if(aw_inst>1000) aw_inst=1000;

	aw_int -= fifo_aw[index];           //on retire la mesure la plus ancienne de la FIFO
	aw_int += aw_inst;                  // integration la nouvelle valeur de l'aw
	fifo_aw[index] = aw_inst; 			//rangement de l'aw dans la FIFO 

	if (time_etape<80)
		aw_moy = aw_inst;
	else
		aw_moy = (aw_int + TAILLE_FIFO / 2) / TAILLE_FIFO;


	aw_moy_brut = aw_moy;
	if (aw_moy>1000) aw_moy = 1000;


	if (time_etape == 10) // aw_flash = moyenne, 5 secondes apres la detection des 3 dernieres mesure d'aw instantannee
	{

		aw_flash = fifo_aw[(index + TAILLE_FIFO - 2) % TAILLE_FIFO];
		aw_flash += fifo_aw[(index + TAILLE_FIFO - 1) % TAILLE_FIFO];
		aw_flash += aw_inst;
		aw_flash /= 3;
		if (aw_flash>1000) aw_flash = 1000;


	}

	if (ACQUISITION_POINT != 0)   // si la fonction de regulation de la reflexion l'autorise on fait des acquisitions
	{
		if (CINETIQUE == 0)
		{
			if ((time_etape - time_point) >= interval_point[mode_mesure])  //si le temps ecoul‚ entre 2 mesures valides est suffisant 
			{
				time_point = time_etape;          // rafraichissement temps du point


				if ((nb_point != 0) && (time_etape<200))   // pour eviter certaines erreurs dues a une prise de points prematuree en debut de mesure si la difference entre la mesure actuelle et la mesure precedente est trop importante on annule les mesures precedentes
				{
					nb_point--;
					if ((((short)aw_moy_brut - (short)fifo_point[nb_point%NB_POINTS]) >= -1) && (((short)aw_moy_brut - (short)fifo_point[nb_point%NB_POINTS]) <= 1)) nb_point++;
					else nb_point = 0;
				}


				fifo_point[nb_point%NB_POINTS] = aw_moy_brut; // point aw intermediaire = aw moyenne  
				nb_point++;

				NOUVEAU_POINT = 1;
			}
		}
		else
		{
			fifo_point[0] = aw_moy_brut; // point aw intermediaire = aw moyenne   
			nb_point = 1;
		}
	}
}*/