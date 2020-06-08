#include "proto.h"

#include "ad7714.h"
#include <math.h>

#include "HD44780.h"

#include "vardef.h"



#define NB_ETAPE 4
#define NB_TACHE 15
#define NB_SURF 8

#define K_VT			0.01	// V/°C
#define G_LMP_TSURF		4.1666	// the LMP stage gain at the TSURF channel
#define G_LMP_VALDET	1.33333	// the LMP stage gain at the VALDET channel

#define PELTIER_MIN		-32000
#define PELTIER_MAX		 32000


//#pragma optimizesize
char findex = 0;

xdata signed char fifo_xi[TAILLE_FIFO + 1] = { 0 };
xdata unsigned short fifo_aw[TAILLE_FIFO+1] = { 0 };
xdata unsigned short fifo_point[NB_POINTS+1] = { 0 }; 
//xdata short fifo_tsurf[NB_TSURF];
//xdata long tsurf_int;
//xdata char index_tsurf;

/*#define PELTIER_SPEED_F12 1
#define ACCELERATION 1.0075f*/

/*use this timer to set/check mirror temp calibration*/
#define MIRROR_TEMP_STAGE_END 255
#define HEAT_STAGE_RAMP 1.25
char MirrorTempCalibrated = 0;
double MirrorTempTimer = 0;
double AUTO_MIRROR_SHIFT = 1;
double HeatStagePeltier = 0;
double MirrorTempRamp = 0;
double oldMirrorTemp = -255;
/*ms to wait at measure startup*/
#define MIRROR_AUTO_CALIB_TIME 30000
#define HEAT_STAGE_TIME 10000

#define PELTIER_SPEED_F12 1
#define ACCELERATION 1.0075f //#define ACCELERATION 1.0075f
//#define ACCELERATION 1.1f //#define ACCELERATION 1.1f

void acqui_reflexion(void);

code void ( * code p_tache[NB_ETAPE][NB_TACHE])(void)=// TABLE OF TASKS
{
 
	{                                            // etape attente
		acqui_miroir,
		rien, //regu_miroir @TODO disabled to reach equilibrium
		rien,
		rien,
		gestion_demarrage,
		acqui_reflexion,
		rien,
		rien,
		rien,
		rien,	
		acqui_surface,
		calcul_pente_temperature,
		regulation_peltier,
   	rien,
   	gestion_noyau	
	}, 
	{                                            // etape equilibre
		acqui_miroir,
		regu_miroir,
		rien,
		rien,
		gestion_equilibre,
		acqui_reflexion,
		rien,
		rien,
		rien,
		rien,	
		acqui_surface,
		calcul_pente_temperature,
		regulation_peltier,
   	rien,
   	gestion_noyau	
	},
	{  														// etape recherche
		rech_rapide,                                     
		rien,
		rien,
		rech_rapide,  
		rien,
		rien,
		rech_rapide,
		rien,
		rien,
		rech_rapide,  
		rien,
		rien,
		rech_rapide,  
		acqui_miroir,//was rien
		gestion_noyau	 
	},
	{                                            // etape mesure
		acqui_miroir,
		rien,
		calc_aw,
		rien,
		acqui_reflexion,
		regu_reflexion,
		calc_aw_finale,
		calib_auto,
		rien,
		acqui_surface,
		calcul_pente_temperature,
		regulation_peltier,
		fin_mesure, 
   	gestion_noyau	   
	}

};






void gestion_evenement(void)
{
 EA=0;
 if(CYCLE_INT!=0) // an interruption of the normal cycle has been detected, the current cycle is raz, the acquisitions blocked over the entire cycle
 {
	CYCLE_INT=0;
	ACQUISITION=0;
	switch_can(0);
	ETAPE_H=0;
 	ETAPE_L=0;
 	num_tache=0;
 	time_etape=0;
 }	
 EA=1;
}

int oldetap = -1;
static char* TASKS[] = { "wait","equilibrium","research","measure" };

 //@TODO call this function
void noyau_temps_reel(void) //interrupt 1 setting 1  // called by timer 0, about 30 times per second
{

 ET0=0;      // inhibition 
 
 
// gestion_bp();  // scan des boutons poussoirs
// gestion_demarrage_auto();   // scan du contacteur reed
 gestion_evenement();    // gestion de l'interruption du cycle normal


 //if (num_tache == 0)
	//printf("control_0/etape = %d\n", (ETAPE_H << 1 | ETAPE_L) & 0x03);

 (*p_tache[etape][num_tache])();    // lancement de la tache
 //(*p_tache[(ETAPE_H<<1 | ETAPE_L)&0x03][num_tache])();    // lancement de la tache

 
 num_tache++;  // incrementation numero de tache

 if (oldetap != etape) {
	 oldetap = etape;
	 printf("INF: current task [%s]\n", TASKS[oldetap]);
 }
}



void rien(void) 
{}

#define FREQ_TIME 0.25
float fix_time = 0;

//@TODO fix time
void gestion_noyau(unsigned long timepassed)
{
	fix_time+= FREQ_TIME;
	if (fix_time >= 0.5) {
		time_etape++; // incremental time step in about 1/2 second 
		time_system++;
		fix_time = 0;
	}
	num_tache=FIN; // it's the last spot we start again

  	findex++;
  	if(findex>=TAILLE_FIFO)  // gestion du pointeur de FIFO
  		findex=0;
  	
  	if(MESURE_EN_COURS!=0) // incrementation measurement time in seconds 
  	{
		time_mesure_fl += FREQ_TIME;//0.5//SECONDS_CICLE; // increment of the precise value of the cycle time
  		time_mesure=(unsigned long)time_mesure_fl;  
   }
   ACQUISITION=1; // it is authorized if they were not the acquisitions of the CAN for the next cycle
}







//@TODO
void moteur(xdata char vit)    // stop, start or change fan speed
{
	//if(vit==0) MOTEUR=0;
	//else MOTEUR=1;
	moteur_bip=vit;
	//sort_moteur_bip(); // call the assembly routine
	fan_set_perc(vit);
	//printf("moteur = %d%%\n",vit);
	//printf("ERR: moteur missing\n");
}


void moteur_on(void)   // start the fan at the memorized speed
{
	moteur(opt.FAN_SPEED); //168 chip, 182 motor
}



	
void gestion_equilibre(void)   
{
		xdata unsigned short synchro;		
		
		if(CINETIQUE==0)
		{
		 	synchro=0;
		}
		else
		{
		   synchro=(interval_mes[mode_cinet]-duree_mes[mode_cinet])*2; // calculation of the rest period if kinetic mode
		}
		
		if (MirrorTempCalibrated == 0)
			AUTO_MIRROR_SHIFT = 0;

		/* auto calibration of mirror temp */
		if (MirrorTempCalibrated ==0 && (time_ms() - MirrorTempTimer >= MIRROR_AUTO_CALIB_TIME)) {
			/*reset timer*/
			MirrorTempTimer = time_ms();
			MirrorTempCalibrated = 1;

			acqui_miroir();
			acqui_surface();

			double tsurface = tsurf;
			double tmirror = tmir;

			/*apply auto shift without saving the value*/
			AUTO_MIRROR_SHIFT = tsurface - tmirror;

			printf("tmirror autoshift = [%f]\n", AUTO_MIRROR_SHIFT);
			time_etape = 0; //reset time

			printf("heating stage\n");
			HeatStagePeltier = 1;

			MirrorTempCalibrated = MIRROR_TEMP_STAGE_END;
			time_etape = 0; //reset time
		}

		/*heat stage*/
		/*if (MirrorTempCalibrated == 1) {
			// heat the mirror to max temp
			peltier_set_perc(HeatStagePeltier);
			HeatStagePeltier *= HEAT_STAGE_RAMP;
			if (HeatStagePeltier >= 100) HeatStagePeltier = 100;

			// check temperature acceleration 
			if (oldMirrorTemp > -255) {
				MirrorTempRamp = tmir - oldMirrorTemp;
				if (MirrorTempRamp < 0) MirrorTempRamp = 0;
			}
			oldMirrorTemp = tmir;

			if (tmir + MirrorTempRamp >= 70) {
				HeatStagePeltier = 0;
				peltier_set_perc(HeatStagePeltier); //stop peltier
			}
			else if (HeatStagePeltier == 0) {
				HeatStagePeltier = 1;
			}

			if ((time_ms() - MirrorTempTimer >= HEAT_STAGE_TIME)) {
				printf("heating stage end\n");
				MirrorTempCalibrated = MIRROR_TEMP_STAGE_END;
				time_etape = 0; //reset time
			}
		}*/


		if(MirrorTempCalibrated == MIRROR_TEMP_STAGE_END && time_etape>=(synchro+40))     // temps d'equilibre suffisant
		{
			if((xi>=-2.0f)&&(xi<=2.0f))     // si l'asservissement temperature miroir correct
    		{
	    			init_etape_recherche();  // lancement etape recherche rapide
   	  	}	
   	}
}



void gestion_demarrage(void)
{
	xdata unsigned short synchro;
	
	if(MESURE_EN_COURS!=0)
	{
		TSURF_MEM=0;
      
      
		if(CINETIQUE==0)
		{
		 	synchro=0;
		}
		else
		{
		   synchro=(interval_mes[mode_cinet]-duree_mes[mode_cinet])*2;
		}
		
		if(time_etape>=synchro)
		{
				ETAPE_L=1;       // passage a l'etape equilibre
				EQUILIBRE=1;
				moteur_on();
		}
		else
		{
		      EQUILIBRE=0;
		      if(REGU_COUPELLE == 0) moteur(0);
		}

	}
	else
	{
		if(DEM_AUTO==0)   // si tete en position basse
		{
				
				if(time_etape>=60) 
				{
					EQUILIBRE=0;
					if(REGU_COUPELLE == 0) moteur(0);    // arret moteur si mesure finie et delai de 30 secondes ecoule on arrete le moteur
					else if(time_etape>=600) moteur(0);
            }           


      }
      else     // si tete en position haute
      {
			
				if(REGU_COUPELLE != 0) 
      		{
      			if(time_etape < 3) 
      			{
      				efface_ligne(L3);
      				affiche_titre(txt_message[(char)LANGUE][17],L3); 
      		   }
      		}
      		if(time_etape>=600)
      		{
      		 	EQUILIBRE=0;
      		 	moteur(0);	  // arret moteur si tete en position haute et delai de 5 minutes ecoule
      		}
            else
            {
            	EQUILIBRE=1; 
            		
            	moteur_on(); //demarrage purge

				}
      		if((OPTION_H!=0)&&(time_etape==10))
     			{
					bip(volume_key,14,50);  // bip purge ok
					bip(0,14,10);
					bip(volume_key,10,50);
				}      		

		}
   }
}



void init_etape_attente(void)
{
	xdata char i;
	printf("init_etape_attente\n");
   aw_moy=0;                        
	facteur_p_cent=20.0f; // facteur de conversion du parametre xi :  0�celsius -> 0%   5�celsius -> 100%
   peltier=0;
   time_etape=0;
   aw_inst=0;
   ETAPE_L=0;
   ETAPE_H=0;
   FLASH=0;
   recup_config();

   //reset calibration
   MirrorTempTimer = time_ms();
   MirrorTempCalibrated = 0;

   peltier_set_perc(0); //stop peltier
   //AUTO_MIRROR_SHIFT = 0;

   //if((OPTION_H!=0)&&(OPTION_L!=0)) date_courante=recup_date();
	//else date_courante=0;
	
	for(xi_int=0,xi_pel=0,i=0;i<TAILLE_FIFO;i++) fifo_xi[i]=0;
	for(aw_int=0,i=0;i<TAILLE_FIFO;i++) fifo_aw[i]=0;

}




void regu_miroir(void)
{
	if (MirrorTempCalibrated != MIRROR_TEMP_STAGE_END) return; //we need to auto calibrate first!

   xdata signed char xi_lim;
   xdata signed char xi_prec;

		   float readV, loc_tmir, loc_tsurf;
		   loc_tmir = pt_temp_single_read();

		   readV = -addc_read_v(VAL_SURF_DIFF);
		   readV += valsurf_offset;
		   loc_tsurf = readV / (K_VT * G_LMP_TSURF);

		   xi = (loc_tmir - loc_tsurf)*facteur_p_cent;    		// calcul erreur 
		  
		   printf("\n%.1f\t%.3f\t%.3f", xi, loc_tmir, loc_tsurf);

/*
	xi=(tmir-tsurf)*facteur_p_cent;    		// calcul erreur 
//	printf("\nregu_miroir(), err: %.1f, tmir: %.3f, tsurf: %.3f", xi, tmir, tsurf);
	printf("\n%.1f\t%.3f\t%.3f", xi, tmir, tsurf);
*/

   if(EQUILIBRE!=0) xi-=100.0f;  // ajout 5 degres soit 100 % pendant l'equilibre	
   
   xi_lim=(signed char)xi;
	if(xi>100.0f) xi_lim=100;                       // limitation de l'erreur a + ou - 100%
	if(xi<-100.0f) xi_lim=-100;
	
	//FIFO XI
	
	xi_int-=(short)fifo_xi[findex];
	xi_int+=(short)xi_lim;             // integration de l'erreur de l'asservissement
	fifo_xi[findex]=xi_lim;
   xi_prec=fifo_xi[(findex+TAILLE_FIFO-1)%TAILLE_FIFO];   // recuperation erreur precedente dans FIFO
   
   //printf("\nregu_miroir(), xi_lim: %d, xi_int: %d, xi_prec: %d", xi_lim, xi_int, xi_prec);
   printf("\t%d\t%d\t%d", xi_lim, xi_int, xi_prec);

		float facteur = 15.0;
		float errperc = 0.95;
		peltier -= (short)(((float)xi_lim - (float)xi_prec * errperc) * facteur);     // asservissement temperature (erreur - 93% de l'erreur precedent) * gain 

   
//   peltier-=(short)(((float)xi_lim-(float)xi_prec*0.93f)*50.0f);     // asservissement temperature (erreur - 93% de l'erreur precedent) * gain 
   if(xi>0.0f) peltier-=6 * PELTIER_SPEED_F12;  // correction minimum permettant de corriger l'erreur statique due au manque de precision des variables xi_lim et xi_prec
   else peltier+=6 * PELTIER_SPEED_F12;        // on oscille donc autour de 0% d'erreur
   
   
   //if(peltier>6000) peltier=6000;       // limitation des valeurs min et max 
   //if(peltier<-6000) peltier=-6000;
   if (peltier > PELTIER_MAX) peltier = PELTIER_MAX;       // limitation des valeurs min et max 
   if (peltier < PELTIER_MIN) peltier = PELTIER_MIN;


   //printf(", peltier: %d", peltier);
   printf("\t%d", peltier);
   //sort_peltier();
   peltier_set_power16(peltier); // ##
}

void init_etape_recherche(void)
{
	printf("init_etape_recherche\n");
	xdata char i;
	message = 0;
	for(xi_int=0,xi_pel,i=0;i<TAILLE_FIFO;i++) fifo_xi[i]=0;
	for(i=0;i<NB_POINTS;i++) fifo_point[i]=0;

	ppm_sec=0;
	nb_point=0;
	aw_flash=0;
	aw_inst=0;
	aw_finale=0;
	xi=7.5f; // xi will now represent the search speed initially 7.5 but will be incrusted during the search 
	peltier-=2000; // the mirror is suddenly cooled by a fixed value to accelerate the fast search
	
	float loc_reflexion;
	float readV;
	readV = -addc_read_v(VAL_DET_DIFF);
	readV /= G_LMP_VALDET;
	readV += valdet_offset;
	loc_reflexion = map(readV, 0, THE_VREF, 250, 0) + opt.DELTA_VALUE_IR;
	if (loc_reflexion > 10.0f)  // fonctionnement normal 
	{
		detection = loc_reflexion * (1.0f - ((float)dseuil / 1000.0f));         // calcul de la valeur de detection en fonction du reglage de seuil
		facteur_p_cent = 100.0f / (loc_reflexion*(float)(dseuil / 1000.0f));  // calcul pour formatage de l'erreur de reflexion de l'etape mesure -> 100% pour reflexion sans rosee, 0% reflexion = detection 
	}
/*
	if(reflexion>10.0f)  // fonctionnement normal 
	{
		detection=reflexion*(1.0f-((float)dseuil/1000.0f));         // calcul de la valeur de detection en fonction du reglage de seuil
		facteur_p_cent=100.0f/(reflexion*(float)(dseuil/1000.0f));  // calcul pour formatage de l'erreur de reflexion de l'etape mesure -> 100% pour reflexion sans rosee, 0% reflexion = detection 
	} */
   else  // mode test sans reflexion t�te ouverte
   {
 		detection=-100.0f;         // valeur volontairement inatteignable
 		facteur_p_cent=1.0f;  // valeur arbitraire 
   }

	ETAPE_L=0;                           // passage a l'etape recherche rapide
	ETAPE_H=1;                           
	time_etape=0;								// mise a zero de l'horloge d'etape
	CLEAN_MIRROR=0;
	ERREUR=0;
   
} 

void rech_rapide(void)
{
 	//#define ACCELERATION 1.0100f //#define ACCELERATION 1.0075f
 	#define POINTAGE_BASE 60.0f
	#define K0  9.73f
	#define K1  -0.0423f
	#define K2  0.001788f
	
/*
 	acqui_reflexion();
 
 	if(reflexion>detection)    // si seuil de detection pas encore atteint
*/
	float readV;
	float loc_reflexion;
	readV = -addc_read_v(VAL_DET_DIFF);
	readV /= G_LMP_VALDET;
	readV += valdet_offset;
	loc_reflexion = map(readV, 0, THE_VREF, 250, 0) + opt.DELTA_VALUE_IR;
	if (loc_reflexion > detection)    // si seuil de detection pas encore atteint
 	{
  		if(peltier>-32000)		// limitation valeur min 
  		{
  			peltier-=(short)xi;
  	  		xi*=ACCELERATION;  // acceleration progressive	de 0.75 % � chaque passage
			printf("\nACCEL, ");
 	   }
 	}
 	else                    
 	{
// 		#define K0 6.6
// 		#define K1 0.08 
// 		#define K2 0.0006 
//			#define K0 8.25   
//			#define K1 0.0975
//			#define K2 0.00075          		
// 		#define K0 5.28
// 		#define K1 0.064
// 		#define K2 0.00048
 		

			// rose point finds, we warm up the mirror sharply proportionally to the pointing variable
			// and instantaneous velocity corrected by a 2nd degree polynomial
 		
 		peltier+= ((float)pointage_flash + POINTAGE_BASE) * ( K0 + xi * ( K1 + xi * K2 ));
		printf(", rech_rapide_else peltier: %d", peltier);
     	ETAPE_L=1;
     	ETAPE_H=1;                                  // passage � l'etape mesure 
 	}
   
  
 /*
   if(peltier>6000) peltier=6000;					// limitation valeur max 
   printf(" rech_rapide, peltier: %d", peltier);
   sort_peltier();
*/
	if (peltier > PELTIER_MAX) peltier = PELTIER_MAX;       // #@#
	if (peltier < PELTIER_MIN) peltier = PELTIER_MIN;       // #@#

	printf("\t rech_rapide, peltier %d", peltier);       // #@#
	peltier_set_power16(peltier); // #@#

   if(time_etape>150)           // if the dewpoint not found after 150 cycles//si le point de rosee n'a pas ete trouve apres 150 cycles on arrete la mesure
   {
	   printf("ERR: CLEAN_MIRROR\n");
    	nb_erreur = 3;
    	CLEAN_MIRROR=1;
    	ERREUR=1;
      MESURE_EN_COURS=0;
   	CYCLE_INT=1;
   	init_etape_attente();

	//inform ui error
	ui_event(ui_event_measure_error);
	}
   
   
   if(ETAPE_L==1) init_etape_mesure();
}



void init_etape_mesure(void)
{
	printf("init_etape_mesure\n");
	num_tache=2;                    
	time_point=0;      
   time_etape=0;
   NOUVEAU_POINT=0;
   BOOST_H=1;
   BOOST_L=1;

#ifdef DOLOG
   if ((fl = fopen("logdata.txt", "w")) == NULL)
	   printf("\nUnable to open file logdata.txt");
   else
   {
	   printf("\nLogging data into file logdata.txt");
	   fprintf(fl, "reflexion\tdetection\tpeltier\taw_inst\tloc_tmir");
   }
#endif

}



void regu_reflexion(void)      // regulation de la rosee sur le miroir
{

	
	xdata short correction_P;     
   xdata short correction_I;
   xdata signed char xi_lim;
	xdata signed char temp;
//	short xi_moy_locale;
	//double readV;

	float loc_tmir;
	loc_tmir = pt_temp_single_read();

	float readV;
	float loc_reflexion;
	readV = -addc_read_v(VAL_DET_DIFF);
	readV /= G_LMP_VALDET;
	readV += valdet_offset;
	loc_reflexion = map(readV, 0, THE_VREF, 250, 0) + opt.DELTA_VALUE_IR;
	xi=(loc_reflexion-detection)*facteur_p_cent;     // calcul de l'erreur de l'asservissement en % ( 0% = seuil de detection atteint, > 0% = pas ou pas assez de rosee,

//	xi=(reflexion-detection)*facteur_p_cent;     // calcul de l'erreur de l'asservissement en % ( 0% = seuil de detection atteint, > 0% = pas ou pas assez de rosee, < 0% trop de rosee) 
	xi_lim=(signed char)xi;
	if(xi>100.0f) xi_lim=100;                       // limitation de l'erreur a + ou - 100%
	if(xi<-100.0f) xi_lim=-100;
	

	xi_pel-=(short)fifo_xi[(findex+TAILLE_FIFO-DUREE_INTEGRATION)%TAILLE_FIFO];    // integration de l'erreur de l'asservissement sur 10 mesures
	xi_pel+=(short)xi_lim;
	xi_int-=(short)fifo_xi[findex];
	xi_int+=(short)xi_lim;             // integration de l'erreur de l'asservissement taille totale de la fifo soit 40 mesures
	fifo_xi[findex]=xi_lim;

//	printf("\nregu_reflexion\t%.1f\t%.3f\t%.3f", xi, loc_reflexion, detection);
//	printf("\t%d\t%d\t%d\t%.3f", xi_lim, xi_int, xi_pel, loc_tmir);

   ACQUISITION_POINT=0;
   
   if(time_etape<10) return;

	correction_P=(short)(xi_lim-fifo_xi[(findex+TAILLE_FIFO-1)%TAILLE_FIFO]);                    // asservissement, opposition � la variation de reflexion  

   
	for(temp=1,XI_DERIVE_POS=0,XI_DERIVE_NEG=0,XI_POS=0,XI_NEG=0;temp<=(TAILLE_FIFO-8);temp++)        // optimisation, recherche pics de l'erreur et pics de derivee de l'erreur negative et positive
																																	  // dans FIFO asservissement, les 8 dernieres valeurs ne sont pas prises en compte
	{
   	xi_lim=fifo_xi[(findex+temp)%TAILLE_FIFO];

		if(temp!=1)
		{
			if(xi_lim-(fifo_xi[(findex+temp-1)%TAILLE_FIFO])>8) XI_DERIVE_POS=1;       // si derivee de l'erreur superieure � 8%
			if(xi_lim-(fifo_xi[(findex+temp-1)%TAILLE_FIFO])<-8) XI_DERIVE_NEG=1;      // si derivee inferieure � -8%
      }

		if(xi_lim>=50) XI_POS=1;      // si une erreur superieure � 50%
		if(xi_lim<=-50) XI_NEG=1;     // si une erreur inferieure � -50%
	}




	

 
   if(time_etape>=80) ACQUISITION_POINT=1;
 
   temp=xi_int/TAILLE_FIFO;
	if((temp<=-20)||(temp>=20)) ACQUISITION_POINT=0;      // on invalide la mise en memoire de l'aw moyenne si la moyenne de l'erreur sur 40 mesures depasse 20%

   
   temp=xi_pel/DUREE_INTEGRATION;  								// calcul erreur moyenne a destination du facteur integral de l'asservissement
   if(temp>20)                                          	// limitation du terme integral � 20% de l'erreur moyenne
   {
      correction_I=20;
   }
   else
   {
   	if(temp<-20)
			correction_I=-20;
      else
      {   
        correction_I=temp;                              	// asservissement (facteur integral), correction en fonction de erreur moyenne
      }
   }



   
   //unsigned char DATA = GET_OPTIMIZE;
   //ptr.c=(data signed char *)OPTIMIZE;
   
   if((BOOST_H!=0)||(BOOST_L!=0))   // acceleration en debut de mesure    boost = 3 puis 2 puis 1 puis 0   
   {
      switch(GET_OPTIMIZE & 0x30)     // test de boost
      {
       	case 0x30:
       		correction_P*=6;
       		correction_I*=30;	
       		if(xi>10.0f) BOOST_L=0;
            break;
       	case 0x20:
				correction_P*=4;
       		correction_I*=10;	
            if(xi<-10.0f)
            {
               BOOST_H=0;
               BOOST_L=1;
            }
            break;
      	case 0x10:
				correction_P*=2;
       		correction_I*=4;	
       		if(xi>10.0f) BOOST_L=0;
            break;
   	}
		peltier-=(short)((float)correction_P*(float)gain_i*0.0125f);  //application de la correction P, proportionnellement � gain_i (c'est bien gain_i et non gain_p ici)
   	peltier-=(short)((float)correction_I*(float)gain_i*0.00125f); //application de la correction I, proportionnellement � gain_i 
   }
   else     //  apres debut de mesure boost=0
   {
  		switch(GET_OPTIMIZE & 0x0f)  // cette variable contient les resulats de l'optimisation 
   	{
   	
      	case 0x08:       // pas de variation importante mais erreur importante on augmente le gain general
   		case 0x04:
     			correction_P*=2;
        		correction_I*=3;
       	  	break;

         /*
   		case 0x03:
   		case 0x07:
   		case 0x0b:
   		case 0x0f:     // oscillation rapide on attenue le gain general
   	   	if(time_etape>150)
   	   	{
   	   		correction_P/=2;
   	   		correction_I/=2;
    	   	}
    	   	break;

   		case 0x0e:     // oscillation lente on attenue le gain integral
   		case 0x0d:
   			if(time_etape>150)
   			{
   				correction_P*=2;
   				correction_I/=2;
   	   	}
   	   	break;

         */
   		default:      // regime normal
   	   	break;
   
   	}

   peltier-=(short)((float)correction_P*(float)gain_p*0.0125f);
   peltier-=(short)((float)correction_I*(float)gain_i*0.00125f);
   
   }
    
   
   
   
   if(peltier>3000) peltier=3000;
   if(peltier<-32000) peltier=-32000;	

//	printf(" regu_reflexion, peltier: %d", peltier);
	printf("\t%d", peltier);
	sort_peltier();
/*
	//printf("\nregu_reflexion, peltier %d", peltier);       
	printf("\t%d", peltier);
	peltier_set_power16(peltier); 
 */
#ifdef DOLOG	
	printf("\n%.3f\t%.3f\t%d\t%d\t%.3f\t%.3f", loc_reflexion, detection, peltier, aw_inst, loc_tmir, tmir);
	fprintf(fl, "\n%.3f\t%.3f\t%d\t%d\t%.3f\t%.3f", loc_reflexion, detection, peltier, aw_inst, loc_tmir, tmir);
#endif
	
}	
	

unsigned short fct_aw(xdata float miroir, xdata float surface)
{
 xdata float aw;
 
 miroir+=273.15f;
 surface+=273.15f;
  
 aw=-6096.9385f*(1.0f/miroir-1.0f/surface);
 aw-=0.02711193f*(miroir-surface);
 aw+=0.00001673952f*((miroir*miroir)-(surface*surface));
 aw+=2.433502f*logf(miroir/surface);
 aw=expf(aw);

 short aw_result = ((short)((1000.0f*aw)+0.5f));    //codage de l'aw sur 16 bits    0 � 1000

 return aw_result * opt.SLOPE_AW + opt.DELTA_AW;
}


void calc_aw(void)
{

 
 aw_inst=fct_aw(tmir,tsurf);

//if(aw_inst>1000) aw_inst=1000;

 aw_int-=fifo_aw[findex]; // remove the oldest measure from the FIFO
 aw_int+=aw_inst;                  // integration la nouvelle valeur de l'aw
 fifo_aw[findex]=aw_inst; 			//rangement de l'aw dans la FIFO 

 if (time_etape < 80)
	 aw_moy = aw_inst;
 else
	 //aw_moy=(aw_int+TAILLE_FIFO/2)/TAILLE_FIFO;
	 aw_moy = aw_int / TAILLE_FIFO;
 
 
 aw_moy_brut = aw_moy;
 if(aw_moy>1000) aw_moy=1000; 
 
 
 if(time_etape==10) // aw_flash = moyenne, 5 secondes apres la detection des 3 dernieres mesure d'aw instantannee
 {
 	 
 	 aw_flash=fifo_aw[(findex+TAILLE_FIFO-2)%TAILLE_FIFO];
 	 aw_flash+=fifo_aw[(findex+TAILLE_FIFO-1)%TAILLE_FIFO];
 	 aw_flash+=aw_inst;
    aw_flash/=3;
    if(aw_flash>1000) aw_flash=1000; 

 
 }
 
 if(ACQUISITION_POINT!=0)   // si la fonction de regulation de la reflexion l'autorise on fait des acquisitions
 {
 	if(CINETIQUE==0)
 	{
		if ((time_etape-time_point)>=interval_point[mode_mesure])  //si le temps ecoul� entre 2 mesures valides est suffisant 
		{
			time_point=time_etape;          // rafraichissement temps du point
			
			
			if((nb_point!=0)&&(time_etape<200))   // pour eviter certaines erreurs dues a une prise de points prematuree en debut de mesure si la difference entre la mesure actuelle et la mesure precedente est trop importante on annule les mesures precedentes
			{
				nb_point--;
				if((((short)aw_moy_brut-(short)fifo_point[nb_point%NB_POINTS])>=-1)&&(((short)aw_moy_brut-(short)fifo_point[nb_point%NB_POINTS])<=1)) nb_point++;
			   else nb_point=0;
			}
			
						
			fifo_point[nb_point%NB_POINTS]=aw_moy_brut; // point aw intermediaire = aw moyenne  
     	   nb_point++;

     	   NOUVEAU_POINT=1;
     	}
  }
  else
  {
  	 	fifo_point[0]=aw_moy_brut; // point aw intermediaire = aw moyenne   
      nb_point=1;
  }
 } 
}   	





void calc_aw_finale(void)
{
   xdata unsigned short Sy;
   xdata unsigned short Sxy;
   xdata float pente;
   xdata char ind_point;


 
 	if(CINETIQUE!=0)
 	{

 		if((time_mesure%duree_mes[mode_cinet])==0)     // si temps de mesure en cinetique ok la mesure est finie 
      {
 				if(nb_point!=0)
 				{
 					nb_erreur = 0;
 					aw_finale=fifo_point[0];
 					if((aw_finale-aw_flash+(short)pointage_flash)<255)  
      			{
      				if(((short)aw_finale-(short)aw_flash+(short)pointage_flash)>0)	
      					pointage_flash+=(signed char)((float)aw_finale-(float)aw_flash);	  // correction du pointage de la recherche rapide en fct l'ecart entre aw finale et mesure flash
            		else                                                                   // sans saugegarde du parametre
            			pointage_flash=0;
      			}
      			else 
         			pointage_flash=255;
 				}	
 				else
 				{

 					pointage_flash=125;
 					nb_erreur++;
 					CLEAN_MIRROR=1;   
 					ERREUR==1;
 				}
 				aw_flash=0;
 				MESURE_EN_COURS=0;
 				
   	}
   	aw_finale_brut = aw_finale;
   	if(aw_finale>1000) aw_finale=1000; 
 	   return;
 	}
   else
   {	
   	if(time_mesure>=duree_max[mode_mesure])     // le temps imparti a la mesure est depasse 
  		{
  			MESURE_EN_COURS=0;
  			//if(mode_mesure!=3) ERREUR=1;
  		}   
   }
   
   
   
   if((MESURE_EN_COURS!=0)&&(NOUVEAU_POINT==0)) return;
   NOUVEAU_POINT=0;   
   

   

   if(nb_point<NB_POINTS)
   {
    	if(nb_point>=2)
    	{
    		pente=((float)fifo_point[nb_point-1]-(float)fifo_point[nb_point-2]);
   	}
   }
   else
   {
   	
   	
   	#define aw_max Sy
      #define aw_min Sxy
/*  	for(ind_point=0,aw_min=2000,aw_max=0;ind_point<NB_POINTS;ind_point++) // calcul aw min et max si ecart trop important on rejette la mesure
		{
			if(fifo_point[ind_point]>aw_max) aw_max=fifo_point[ind_point];
			if(fifo_point[ind_point]<aw_min) aw_min=fifo_point[ind_point];
		}
 	
 	   if((aw_max-aw_min)<8) ERREUR=0;
 	   else ERREUR=1;
*/ 	
   	for(ind_point=0,Sy=0,Sxy=0;ind_point<NB_POINTS;ind_point++)      // calcul de Sy et Sxy pour droite de regression lineaire, methode des moindres carres   
   	{
  			Sy+= fifo_point[(nb_point-ind_point-1)%NB_POINTS];
  			Sxy+=ind_point * fifo_point[(nb_point-ind_point-1)%NB_POINTS];
   	}

   	pente=((float)Sy*(SX/DENOMI)-(float)Sxy*(NB_POINTS/DENOMI));      // les valeurs sont calcules pour 10 points, les coeficients Sx et Sxx sont fixes
   }



	ppm_sec=(signed char)(pente*2000.0f/(float)interval_point[mode_mesure]);        // conversion de la pente en ppm/s

	if(pente> ( 125.0f / 2000.0f * interval_point[mode_mesure])) ppm_sec=125;
	if(pente< ( -125.0f / 2000.0f * interval_point[mode_mesure])) ppm_sec=125;
	
	if(nb_point>=NB_POINTS) 
	{
		
      if(mode_mesure!=3)
   	{

   			if(time_mesure>=(ABS((short)ppm_sec)*(duree_max[mode_mesure]/(short)ppm_max[mode_mesure])))// si la pente exprimee en ppm/s est inf�rieur au critere la mesure est finie
   			{                                                                                          // sur cette nouvelle version le critere de stabilite est proportionel au temps 
   		                                                                                           // de mesure pour une meilleure repetabilite 
					  	MESURE_EN_COURS=0;
   			}

		}
//  	}
//   	else
//   	{
//   		if(ABS(ppm_sec)<=(ppm_max[mode_mesure]/3))   // si la pente (ppm/s) est inf�rieur au critere de calibration la mesure est finie
//   		{
//      	/*if(ERREUR==0)*/ MESURE_EN_COURS=0;
//   		}   
//   	}
   } 
   
//   if(time_mesure<duree_min[mode_mesure]) MESURE_EN_COURS=1;  
                                                                   
   if(MESURE_EN_COURS==0)               // extrapolation de l'aw, l'indice 0 represente la derniere mesure l'indice 9 la premiere
   {
     	if(nb_point<NB_POINTS)
			// if the 10 points could not be taken, the last serves as the final value 
     	{
     		aw_finale=fifo_point[(nb_point-1)];
     	   pointage_flash=125;
     	   nb_erreur++;
     	   CLEAN_MIRROR=1;   
     	   ERREUR=1;   
		   printf("ERR: points < 10\n");
     	}
     	else   // extrapolation de l'aw, l'indice 0 represente la derniere mesure l'indice 9 la premiere
     	{
     		nb_erreur = 0;
     		aw_finale=(short)((float)niveau_extrap[mode_mesure]*pente+(float)Sy*(SXX/DENOMI)-(float)Sxy*(SX/DENOMI)+0.5f);      // droite de tendance de type y=ax+b avec x = -10, x represente le niveau d'extrapolation 
			aw_finale_brut = aw_finale;
			

      	if((aw_finale-aw_flash+pointage_flash)<(short)255)
      	{
      		if((aw_finale-aw_flash+pointage_flash)>(short)0)	
      			pointage_flash+=(signed char)(aw_finale-aw_flash);	  // correction du pointage de la recherche rapide en fct l'ecart entre aw finale et mesure flash
            else 
            	pointage_flash=0;
      	}
      	else 
         	pointage_flash=255;
      
      }  
      
      if(aw_finale>1000) aw_finale=1000; 
      
      if((CALIB==0)&&(ERREUR==0)) ecritI2C(I2C_TETE,POINTAGE_FLASH,pointage_flash);
      if(MODE_FLASH==0) aw_flash=0;
   }
}


   
void calib_auto(void)
{
#define aw_theo aw_inst
#define aw_rech aw_flash

	if(MESURE_EN_COURS!=0) return;
	
	if(CALIB==0) return;
	
	cde_display(CLEAR);
  	affiche_titre(txt_message[(char)LANGUE][24],L1);
	
	rech_sel();
	
   if(mode_calibration < 128)  // si calibration en 1 point
   {
		calcul_calib_1();
		if (message != 21) {
			//sauve_float(I2C_TETE, DELTA_TMIR_USER, delta_tmir);
			printf("save calibration 1p delta_tmir =%f\n", delta_tmir);
			opt.DELTA_TEMP_MIRROR = delta_tmir;
			sync_opt();
		}
		//@TODO check
		//CALIB = 0;
		num_pt_calib = 0;
	}
	else
	{
		int last = num_pt_calib;
		calcul_calib_2();	
		if(message != 21 && num_pt_calib == 0)
	   {
			//sauve_float(I2C_TETE,DELTA_TMIR_USER,delta_tmir);   // ecriture de l'offset miroir corrige
			//sauve_float(I2C_TETE,PENTE_TMIR_USER,pente_tmir);   // ecriture de la pente miroir corrige	   
			printf("save calibration 2p delta_tmir =%f\n", delta_tmir);
			printf("save calibration 2p pente_tmir =%f\n", pente_tmir);
			opt.DELTA_TEMP_MIRROR = delta_tmir;
			opt.SLOPE_TEMP_MIRROR = pente_tmir;
			sync_opt();
	   }
		//@TODO check
		//CALIB = 0;
		if (last == 1)
		num_pt_calib = 0;

	}
//aw_theo = aw_rech;
}

void rech_sel(void)
{
#define aw_theo aw_inst
#define aw_rech aw_flash
#define num_sel_trouve nb_point

	xdata float ind_temp;
	xdata short ecart_min;
	xdata char ind_tab;
	xdata char num_sel;

   if(tsurf>=0.0f)
   {
   	if(tsurf<=40.0f) ind_temp=tsurf/5.0f;          // indice 0 -> 0�C  ... indice 8 -> 40�C     
      else ind_temp=8.0f;
   }
   else ind_temp=0.0f;
   
   ind_tab=(char)ind_temp;
   
   for(num_sel=0, ecart_min=1000; tab_liste[mode_calibration & 0x7F][num_sel] != EOF; num_sel++)  // recherche parmi les tables de sels disponibles 
   {
   	aw_theo  = (unsigned short)((float)(tab_sel[tab_liste[mode_calibration & 0x7F][num_sel]][ind_tab])*(1.0f-ind_temp+(float)ind_tab)+(float)(tab_sel[tab_liste[mode_calibration & 0x7F][num_sel]][ind_tab + 1])*(ind_temp-(float)ind_tab)+0.5f);  // calcul aw theorique du sel selectionne en fonction de la temperature   

      if(abs_macro((short)aw_theo-(short)aw_finale_brut) < ecart_min)        // recherche de l'ecart le plus faible entre valeur mesure et sel teste
      {
        	num_sel_trouve = num_sel;
        	aw_rech = aw_theo;	                       
         ecart_min = abs_macro((short)aw_finale_brut-(short)aw_rech);      
      }	
   }

}	

void calcul_calib_1(void)
{
 
	xdata float tmir_1;
	xdata short pas;
#define aw_rech aw_flash

   tmir_1 = tsurf_1;
   pas = -5000;
  	do  // calcul temperature miroir mesur�e 
  	{
   	tmir_1 +=(float)pas / 1000.0f;
   	if ( (fct_aw(tmir_1 , tsurf) < aw_finale_brut) && (pas < 0) ) pas /= -2;
   	if ( (fct_aw(tmir_1 , tsurf) > aw_finale_brut) && (pas > 0) ) pas /= -2;
   } while (pas != 0);


  	pas=1000;        // cette variable est utilisee comme pas de recherche dichotomique
  	tmir_1-=delta_tmir;   // on retranche l'offset actuel
  	do    // methode cacul inverse par dichotomie
  	{
     	delta_tmir+=(float)pas/1000.0f;
     	if ( (fct_aw(tmir_1 + delta_tmir, tsurf) < aw_rech) && (pas < 0) ) pas /= -2;
     	if ( (fct_aw(tmir_1 + delta_tmir, tsurf) > aw_rech) && (pas > 0) ) pas /= -2;
  	} while (pas != 0);
//	tmir_1 += delta_tmir;  // on rajoute le nouvel offset
	
	

  	if((delta_tmir < -5.0f) || (delta_tmir > 5.0f))  // detection parametre hors tol�rance
  	{
  	 	message = 22;
  	 	ERREUR==1;
		//@UI
		ui_event(ui_event_calibration_error);
  	 	//bip_modul5();  
	}	
	else
	{
	 	message = 23;
	 	//bip_modul6();  
	}				
}


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
	  
   if(num_pt_calib==0)    // memorisation du 1er point de calibration
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

 		
 		if(aw_rech_2 == aw_rech_1) // if identical salts found we reject the calibration
   	{
Erreur_calib:
   	 	message = 21;
   	 	ERREUR==1;
		//@UI
		ui_event(ui_event_calibration_error_salt);
   	 	//bip_modul5();  
   	 	return;
		}
			
   	if(abs_macro((short)aw_mes_1 - (short)aw_mes_2) < 80) // if the 2 points are very close it is reported
   	{
   	 	message = 20;
   	 	ERREUR==1;
		//@UI
		ui_event(ui_event_calibration_error_close_salt);
   	 	//bip_modul5();
   	}

			


   	tmir_2 = tsurf_2;
   	pas = -5000;
  		do
			// recuperation temperature mirror measurement of the 2 nd point, by reverse recalculation it is more precise than a simple memorization of the last value  
  		{
     		tmir_2 +=(float)pas / 1000.0f;
     		if ( (fct_aw(tmir_2 , tsurf_2) < aw_mes_2) && (pas < 0) ) pas /= -2;
     		if ( (fct_aw(tmir_2 , tsurf_2) > aw_mes_2) && (pas > 0) ) pas /= -2;
   	} while (pas != 0);

   	tmir_1 = tsurf_1;
   	pas = -5000;
  		do  // recuperation temperature miroir mesur� du 1 er point
  		{
     		tmir_1 +=(float)pas / 1000.0f;
     		if ( (fct_aw(tmir_1 , tsurf_1) < aw_mes_1) && (pas < 0) ) pas /= -2;
     		if ( (fct_aw(tmir_1 , tsurf_1) > aw_mes_1) && (pas > 0) ) pas /= -2;
   	} while (pas != 0);


   	tmir_etal_2 = tmir_2;
   	pas = 1000;
  		do   // calcul temperature miroir th�orique du 2 eme point
  		{
     		tmir_etal_2 +=(float)pas / 1000.0f;
     		if ( (fct_aw(tmir_etal_2 , tsurf_2) < aw_rech_2) && (pas < 0) ) pas /= -2;
     		if ( (fct_aw(tmir_etal_2 , tsurf_2) > aw_rech_2) && (pas > 0) ) pas /= -2;
   	} while (pas != 0);

   	tmir_etal_1 = tmir_1;
   	pas = 1000;
  		do   // calcul temperature miroir th�orique du 1 er point
  		{
     		tmir_etal_1 +=(float)pas / 1000.0f;
     		if ( (fct_aw(tmir_etal_1 , tsurf_1) < aw_rech_1) && (pas < 0) ) pas /= -2;
     		if ( (fct_aw(tmir_etal_1 , tsurf_1) > aw_rech_1) && (pas > 0) ) pas /= -2;
   	} while (pas != 0);

		if(ABS(tmir_1 - tmir_2) < 0.5f)  // si points trop proches on rejette la calibration
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

		if((pente_tmir < 0.94f) || (pente_tmir > 1.02f)) // detection out of tolerance
		{
			message = 22;
			ERREUR==1;
			//@UI
			ui_event(ui_event_calibration_error);
	 			//bip_modul5();  
   		 	//num_pt_calib = 0;  
   		 	//return;
		}			
			 
		if((delta_tmir < -5.0f) || (delta_tmir > 5.0f))  // detection out of tolerance
		{
			message = 22;
			ERREUR==1;
			//@UI
			ui_event(ui_event_calibration_error);
	 			//bip_modul5();  
 			//num_pt_calib = 0;  
			//return;
		}						
			
		if(ERREUR == 0)
		{
		 		message = 23;
		 		//bip_modul6();  
		}					
   }
}


void write_tableau(void)
{
	xdata char i;
	xdata union fiche *ptr_fiche;
	
	nb_mesure++;

	if(nb_mesure>NB_MESURE) 
	{
	 	nb_mesure=NB_MESURE;
	 	
	 	for(i=0;i<(NB_MESURE-1);i++)
	 	{
	 	 	tableau_result[i].mesure.date=tableau_result[i+1].mesure.date;
	 	 	tableau_result[i].mesure.etat=tableau_result[i+1].mesure.etat;
	 	 	tableau_result[i].mesure.config=tableau_result[i+1].mesure.config;
	 	 	tableau_result[i].mesure.numero=tableau_result[i+1].mesure.numero;
	 		tableau_result[i].mesure.aw=tableau_result[i+1].mesure.aw;
	 		tableau_result[i].mesure.tsurf=tableau_result[i+1].mesure.tsurf;
	 		tableau_result[i].mesure.duree=tableau_result[i+1].mesure.duree;
	 	}

	}
  
   ptr_fiche=&tableau_result[nb_mesure-1];
   
   for(i=0;i<16;i++)
   {
   	ptr_fiche->index[i]=fiche.index[i];
   }
  
  
 	ligne_select=nb_mesure-1;



}

void formate_mesure(void)     // la mesure en cours est mise en place dans la structure fiche (organisation des mesures sous forme compactee) 
{
		
		if(SEND_MESURE!=0) return; // la mesure n'est pas fini d'etre envoyee 
		//fiche.mesure.aw=0;
		
		if(MESURE_EN_COURS!=0)
		{
			fiche.mesure.aw=aw_moy;
		}
		else
		{
			fiche.mesure.aw=aw_finale;
       		date_courante=recup_date();
		}
		fiche.mesure.date=date_courante;
		
		if(FLASH!=0) 
				fiche.mesure.aw=aw_flash;

		if(CALIB==0) fiche.mesure.numero=numero_mesure;
      else fiche.mesure.numero=aw_flash;

		fiche.mesure.tsurf=(short)(100.0f*tsurf);
		fiche.mesure.duree=time_mesure;
		fiche.mesure.etat=r_etat;
		fiche.mesure.config=r_config;
}		

//cristian
void end_measure_ui() {
	if (MESURE_EN_COURS == 0) {
		if (!ERREUR)
			ui_event(ui_event_measure_end);
		else
			ui_event(ui_event_measure_error);
	}
}

void fin_mesure(void)
{
	if((time_etape==10)&&(MODE_FLASH!=0)&&(CALIB==0)&&(CINETIQUE==0))    // enregistrement de la mesure flash
  	{
			
			FLASH=1;
  	      TSURF_MEM=1;     // memorisation et clignoment de la mesure de temperature de surface
  	      INC_NUM=1;       // on permet l'incremention du numero de mesure lors d'un lancement ulterieur d'une autre mesure
  	      formate_mesure();   // mise en forme des donnees avant sauvegarde
  	      write_tableau();    // ecriture dans le tableau de resultat
  	      if((OPTION_H!=0)&&(OPTION_L!=0)) write_histo();  // ecriture dans le circuit memoire I2C de l'historique
         SEND_MESURE=1; 

   		bip(litI2C(I2C_TETE,VOLUME_BIP),20,50);	

		//inform gui end measure
		end_measure_ui();

	      return;
	}

   if(time_etape==50) TSURF_MEM=0;    // suppression de la memorisation et du clignotement
   
      
   
	if(MESURE_EN_COURS!=0) return;              // si la mesure n'est pas finie on rend la main
   
 
	init_etape_attente();

	if(aw_finale!=0)    // enregistrement de la mesure finale
	{
		FLASH=0;
	   TSURF_MEM=1;	// memorisation et clignotement de la mesure de temperature de surface
	   INC_NUM=1;		// on permet l'incremention du numero de mesure lors d'un lancement ulterieur d'une autre mesure
	   formate_mesure();// mise en forme des donnees avant sauvegarde
	   write_tableau();// ecriture dans le tableau de resultat
	   if((OPTION_H!=0)&&(OPTION_L!=0)) write_histo(); // ecriture dans le circuit memoire I2C de l'historique
		
	   SEND_MESURE=1; 
                                                            
		/*if(litI2C(I2C_TETE,AUTO_PRINT) == 1) 
		{
//			data_printer(LF);
			imprime_entete();
			auto_print();
			data_printer(LF);
			data_printer(LF);
			data_printer(LF);
			data_printer(LF);
			data_printer(LF);
			data_printer(LF);
			data_printer(LF);
			data_printer(LF);
			data_printer(LF);
		}*/
		
		/*if(litI2C(I2C_TETE,AUTO_PRINT) == 2)
		{
			if(entete_ok == 0)
			{
//				data_printer(LF);
				imprime_entete();
				data_printer(LF);
				imprime_colonne();
   			annulation_impression();
   			entete_ok = 1;
			}
			imprime_fiche();
		}*/

		/*if(mode_essai != 0)
		{
			ptr_ascii=0;
	
			buffer_ascii[ptr_ascii]=' ';
			ptr_ascii++;
			float_ascii(tsurf + 0.005f,2);
	

			buffer_ascii[ptr_ascii]=' ';
			ptr_ascii++;
			float_ascii(ttete + 0.005f,2);
			
			buffer_ascii[ptr_ascii]=' ';
			ptr_ascii++;
			float_ascii(tmir + 0.005f,2);

			buffer_ascii[ptr_ascii]=' ';
			ptr_ascii++;			
			if((fiche.mesure.etat&0x04)==0)
				float_ascii((float)fiche.mesure.aw/1000.0f,3);
			else
				float_ascii((float)fiche.mesure.numero/1000.0f,3);			
		
		
			buffer_ascii[ptr_ascii]=' ';
			ptr_ascii++;
			time_ascii(fiche.mesure.date);		
			
			buffer_ascii[ptr_ascii]=' ';
			ptr_ascii++;
			delay_ascii(fiche.mesure.duree);

			
			buffer_ascii[ptr_ascii] = CR;
			ptr_ascii++;
			buffer_ascii[ptr_ascii] = LF;
			ptr_ascii++;

			buffer_ascii[ptr_ascii]=0;
			

			txt_printer(buffer_ascii);	
				
		}		*/
		




	}
/*	
	if(CALIB!=0)
	{
		switch(message)
		{
		 	case 23:
		 		bip_modul6();
		 		break;
		 	default:
		 		bip_modul5();
		 		break;
	
		}
	 	return;
	}	
*/	

	if(CINETIQUE!=0) 
	{
		if(time_mesure<duree_cinet[mode_cinet]) MESURE_EN_COURS=1;
	   if(duree_cinet[mode_cinet]==0) MESURE_EN_COURS=1;
//	   if(time_mesure>43200) time_mesure%=43200; 
	}
	
	if(MESURE_EN_COURS==0)
	{
		//EA=0;
		bip(litI2C(I2C_TETE,VOLUME_BIP),14,50);
		//EA=1;
		bip(0,14,20);
		//EA=0;
		bip(litI2C(I2C_TETE,VOLUME_BIP),20,50);
      //EA=1;
   }
   else
   {
   	//EA=0;
   	bip(litI2C(I2C_TETE,VOLUME_BIP),20,50);	
      //EA=1;
   }

	//inform gui end measure
	end_measure_ui();

}

 
#define FS 16777216.0f//(16777216.0f / 1.05f)

void acqui_miroir(void)   
//
// RESULT: updates the temperature of the mirror in � C, preparation for reflection acquisition
// DESCRIPTION: retrieves the numerical value given by the converter and calculates the corresponding temperature
//
{
 	if(ACQUISITION==0) return;
 	
 	/*tmir=(float)acqui_can(0);
 	switch_can(2);*/
 	tmir= pt_temp_read() /** opt.SLOPE_CALIBRATION*/ * opt.SLOPE_TEMP_MIRROR /*+ opt.DELTA_CALIBRATION*/ + opt.DELTA_TEMP_MIRROR + AUTO_MIRROR_SHIFT;  // en Celsius
}


double aver_tsArr(void)
{
	double ret = 0.0;
	int i;

	for (i = 0; i < N_AVER_TSURF; i++)
		ret += tsArr[i];
	ret /= (double)N_AVER_TSURF;

	return (ret);
}


void acqui_surface(void) 
//
// RESULT: updates variable 'tsurf', the surface temperature in degrees Celsius
// DESCRIPTION: return the value of the temperature at the sample surface 
// 
// This routine works with the new board featuring a differential input stage
// at the ADC converter with G_LMP_TSURF gain.
// Temperature value is recovered as the ADC reading divided by (K_VT * G_LMP_TSURF)
// (see documentation)
//
{
	static double last_aver = 0.0;
	double ts;
	double tsav;
	static int i=0;
	static int firstrun = 1;
	double tmp1, tmp2;
	

	if(ACQUISITION==0) return;
   
  
	if((type_capteur=='C') || (mode_essai != 0))
	{

		if(TYPE_C==0)     // reading of values infra red thermometer 10mV/C
		{
			// get next sample and perform moving average on samples
			#ifdef SINGLE_ENDED
				ts = ad595_temp_read(addc_read_v(VAL_SURF)) ;    
			#else	// is def DIFFERENTIAL
				ts = -addc_read_v(VAL_SURF_DIFF);	
				ts += valsurf_offset;
			#endif

			// store new data in the last-Nsamples-array
			tsArr[i++] = ts;
			// check rollover
			if (i == N_AVER_TSURF)
			{
				i = 0;
				firstrun = 0;
			}
			// calculate tsav as the average of the last-Nsamples-array
			tsav = aver_tsArr();
			// but if the array was not filled yet, assign to tsav last single sample
			if (firstrun)
				tsav = ts;
#ifdef NO_AVERAGING
			tsav = ts;
#endif
			// calculate tsurf 
			tsurf = tsav / (K_VT * G_LMP_TSURF);

			// adjust tsurf with offset and slope from the GUI 
			tsurf = tsurf *  opt.SLOPE_TEMP_SURF +  opt.DELTA_TEMP_SURF;    // en Celsius

			// debug printfs
			printf("\nif(C), V at tsurf pins 4-5: %.3f V", tsav);
			printf("\ttemp: %.3f [°C]", tsurf);

			TYPE_C = 1;
		}
		else			// acquisition d'une sonde pt100 ou pt1000 
		{
				ttete=(float)acqui_can(3);
				switch_can(0);
				ttete=( 1.0f /( KONST ) ) * ( 1.0f / ( FS / ttete - 1.0f ) - 1.0f ) * pente_ttete + delta_ttete; 	// en Celsius
				TYPE_C = 0;
		}

		return;
	}  
  
  
   if(type_capteur!='B')     // acquisition du thermometre infrarouge 10mV/C
   {
		// get next sample 
		#ifdef SINGLE_ENDED
			 ts = ad595_temp_read(addc_read_v(VAL_SURF));
		#else	// is def DIFFERENTIAL
			 ts = -addc_read_v(VAL_SURF_DIFF);
			 ts += valsurf_offset;
		#endif

		// store new data in the last-Nsamples-array
		tsArr[i++] = ts;
		// check rollover
		if (i == N_AVER_TSURF)
		{
			i = 0;
			firstrun = 0;
		}
		// calculate tsav as the average of the last-Nsamples-array
		tsav = aver_tsArr();
		// but if the array was not filled yet, assign to tsav last single sample
		if (firstrun)
			tsav = ts;
#ifdef NO_AVERAGING
		tsav = ts;
#endif
		// calculate tsurf 
		tsurf = tsav / (K_VT * G_LMP_TSURF);

		// adjust tsurf with offset and slope from the GUI 
		tsurf = tsurf *  opt.SLOPE_TEMP_SURF +  opt.DELTA_TEMP_SURF;    // en Celsius

		// debug printfs ***********************************************************************
/*
		tmp1 = -addc_read_v(VAL_DET_DIFF);
		tmp1 /= G_LMP_VALDET;
		tmp1 += valdet_offset;
		//tmp1 = tmp1 + THE_VREF;

		acqui_reflexion();	// to update global val 'reflexion'

		tmp2 = -addc_read_v(VAL_MIR_DIFF);
		tmp2 /= 1.33333;	// LMP8350 stage gain
		tmp2 += valmir_offset;
		
		printf("\nVALSURF: %.4f V  T_surf: %.4f[°C] \tVALDET: %.4f V  REFLX: %.4f\tVALMIR: %.4f V  T_mir: %.4f[°C] ", tsav, tsurf, tmp1, reflexion, tmp2, pt_temp_read());
*/
		//printf("\nif(B), V at tsurf pins 4-5: %.3f V", tsav);
		//printf("\ttemp: %.3f [°C]", tsurf);

		// *** the following two lines are intended for DEBUG and must be normally commented out	  
		//tsurf = addc_read_v(VAL_DET_DIFF);	// bare ADS1256 reading, remove after test !
		//printf("\nif(B), V at det pins 0-1: %.3f V", tsurf);
		//tsurf = addc_read_v(VAL_MIR_DIFF);	// bare ADS1256 reading, remove after test !
		//printf("\n\t V at mir pins 2-3: %.3f V", tsurf);
		//tsurf = addc_read_v(VAL_SURF_DIFF);	// bare ADS1256 reading, remove after test !
		//printf("\n\t V at tsurf pins 5-6: %.3f V", tsurf);		
		// ************************end of debug printfs *****************************************
   }
   else			// acquisition d'une sonde pt100 ou pt1000 
   {
    	tsurf=(float)acqui_can(3);
 		switch_can(0);
 		tsurf=( 1.0f /( KONST ) ) * ( 1.0f / ( FS / tsurf - 1.0f ) - 1.0f ) * pente_ttete + delta_ttete;	// en Celsius
   }
   
}

/*		this is the latest acqui_surface function related to 2018 hardware
void acqui_surface(void) 
//
// RESULT: updates the surface temperature in � C, preparation for the acquisition of the mirror
// DESCRIPTION: return the value of the temperature to the surface of the sample
//
{
	static double last_aver = 0.0;
	double ts;
	double tsav;
	static int i=0;
	static int firstrun = 1;
	

	if(ACQUISITION==0) return;
   
  
	if((type_capteur=='C') || (mode_essai != 0))
	{

   	if(TYPE_C==0)     // reading of values infra red thermometer  10mV/C//acquisition du thermometre infrarouge 10mV/C
   	{
		// get next sample 
		#ifdef SINGLE_ENDED
			ts = ad595_temp_read(addc_read_v(VAL_SURF)) ;    
		#else	// is def DIFFERENTIAL
			g_readV = addc_read_v(VAL_SURF_DIFF);
			ts = ad595_temp_read(g_readV);
		#endif

		// store new data in the last-Nsamples-array
		tsArr[i++] = ts;
		// check rollover
		if (i == N_AVER_TSURF)
		{
			i = 0;
			firstrun = 0;
		}
		// calculate tsav as the average of the last-Nsamples-array
		tsav = aver_tsArr();
		// but if the array was not filled yet, assign to tsav last single sample
		if (firstrun)
			tsav = ts;

		// calculate tsurf the original way
		//tsurf = tsav * pente_tsurf * opt.SLOPE_TEMP_SURF + delta_tsurf + opt.DELTA_TEMP_SURF;    // en Celsius

		// calculate tsurf with just offset and slope from the GUI 
		tsurf = tsav *  opt.SLOPE_TEMP_SURF +  opt.DELTA_TEMP_SURF;    // en Celsius
		
		
			// *** the following two lines are intended for DEBUG and must be normally commented out
		    tsurf = tsav;	// bare ADS1256 reading, remove after test !
			printf("\nif(C), V at tsurf pins 5-6: %.3f V", tsurf);

			
//			#ifdef SINGLE_ENDED
//				tsurf = ad595_temp_read(addc_read_v(VAL_SURF)) * pente_tsurf * opt.SLOPE_TEMP_SURF + delta_tsurf + opt.DELTA_TEMP_SURF;    // en Celsius
//			#else	// is def DIFFERENTIAL
//				tsurf = ad595_temp_read(addc_read_v(VAL_SURF_DIFF)) * pente_tsurf * opt.SLOPE_TEMP_SURF + delta_tsurf + opt.DELTA_TEMP_SURF;    // en Celsius
//			#endif
				
				//tsurf= ad595_temp_read(addc_read_v(VAL_SURF)) * pente_tsurf * opt.SLOPE_TEMP_SURF + delta_tsurf + opt.DELTA_TEMP_SURF;    // en Celsius
				//tsurf = ttete + (tsurf - ttete) * pente_correction_tsurf; // correction of the surface temperature as a function of the deviation with the head temperature								

			; TYPE_C = 1;
   	}
   	else			// acquisition d'une sonde pt100 ou pt1000 
   	{
    		ttete=(float)acqui_can(3);
 			switch_can(0);
 			ttete=( 1.0f /( KONST ) ) * ( 1.0f / ( FS / ttete - 1.0f ) - 1.0f ) * pente_ttete + delta_ttete; 	// en Celsius
			TYPE_C = 0;
   	}
   
   	return;
	}  
  
  
   if(type_capteur!='B')     // acquisition du thermometre infrarouge 10mV/C
   {
	   // get next sample 
	   #ifdef SINGLE_ENDED
			ts = ad595_temp_read(addc_read_v(VAL_SURF));
	   #else	// is def DIFFERENTIAL
			g_readV = addc_read_v(VAL_SURF_DIFF);
			ts = ad595_temp_read(g_readV);
	   #endif

	   // store new data in the last-Nsamples-array
	   tsArr[i++] = ts;
	   // check rollover
	   if (i == N_AVER_TSURF)
	   {
		   i = 0;
		   firstrun = 0;
	   }
	   // calculate tsav as the average of the last-Nsamples-array
	   tsav = aver_tsArr();
	   // but if the array was not filled yet, assign to tsav last single sample
	   if (firstrun)
		   tsav = ts;

	   // calculate tsurf the original way
	   //tsurf = tsav * pente_tsurf * opt.SLOPE_TEMP_SURF + delta_tsurf + opt.DELTA_TEMP_SURF;    // en Celsius
	   
	   // calculate tsurf with just offset and slope from the GUI 
	  //tsurf = tsav * opt.SLOPE_TEMP_SURF + opt.DELTA_TEMP_SURF;    // en Celsius
	  tsurf = tsav + opt.DELTA_TEMP_SURF;   // test with only Shift

	   // *** the following two lines are intended for DEBUG and must be normally commented out	  
	   tsurf = addc_read_v(VAL_DET_DIFF);	// bare ADS1256 reading, remove after test !
	   printf("\nif(B), V at det pins 0-1: %.3f V", tsurf);
	   tsurf = addc_read_v(VAL_MIR_DIFF);	// bare ADS1256 reading, remove after test !
	   printf("\n\t V at mir pins 2-3: %.3f V", tsurf);
	   tsurf = addc_read_v(VAL_SURF_DIFF);	// bare ADS1256 reading, remove after test !
	   printf("\n\t V at tsurf pins 5-6: %.3f V", tsurf);

	   
//		#ifdef SINGLE_ENDED
//			tsurf = ad595_temp_read(addc_read_v(VAL_SURF)) * pente_tsurf * opt.SLOPE_TEMP_SURF + delta_tsurf + opt.DELTA_TEMP_SURF;    // en Celsius
//		#else	// is def DIFFERENTIAL
//			tsurf = ad595_temp_read(addc_read_v(VAL_SURF_DIFF)) * pente_tsurf * opt.SLOPE_TEMP_SURF + delta_tsurf + opt.DELTA_TEMP_SURF;    // en Celsius
//		#endif
		
	   //tsurf= ad595_temp_read(addc_read_v(VAL_SURF)) * pente_tsurf * opt.SLOPE_TEMP_SURF + delta_tsurf + opt.DELTA_TEMP_SURF;    // en Celsius
   }
   else			// acquisition d'une sonde pt100 ou pt1000 
   {
    	tsurf=(float)acqui_can(3);
 		switch_can(0);
 		tsurf=( 1.0f /( KONST ) ) * ( 1.0f / ( FS / tsurf - 1.0f ) - 1.0f ) * pente_ttete + delta_ttete;	// en Celsius
   }
      
}
*/


double aver_dtArr(void)
{
	double ret = 0.0;
	int i;

	for (i = 0; i < N_AVER_DET; i++)
		ret += dtArr[i];
	ret /= (double)N_AVER_DET;

	return (ret);
}


void acqui_reflexion(void)  
//
// RESULT: updates reflection value, preparation for surface acquisition
// DESCRIPTION: measures the presence of dew
//
{
	static double last_aver = 0.0;
	double dt;
	double dtav;
	static int i = 0;
	static int firstrun = 1;
	double readV;

 	if(ACQUISITION==0) return;
 	
	// get next sample 
	#ifdef SINGLE_ENDED
		dt = addc_read_v(VAL_DET);
	#else	// is def DIFFERENTIAL
		readV = -addc_read_v(VAL_DET_DIFF);
		readV /= G_LMP_VALDET;
		readV += valdet_offset;
		//printf("\n\tVALDET: %.3f V  ", readV);
		dt = readV; //+ THE_VREF;
	#endif

	// store new data in the last-Nsamples-array
	dtArr[i++] = dt;
	// check rollover
	if (i == N_AVER_DET)
	{
		i = 0;
		firstrun = 0;
	}
	// calculate dtav as the average of the last-Nsamples-array
	dtav = aver_dtArr();
	// but if the array was not filled yet, assign to dtav last single sample
	if (firstrun)
		dtav = dt;
#ifdef NO_AVERAGING
	dtav = dt;
#endif
	// update var reflexion
	reflexion = dtav;


 	//reflexion=(float)acqui_can(2);
	/*
	#ifdef SINGLE_ENDED
		reflexion = addc_read_v(VAL_DET);
	#else	// is def DIFFERENTIAL
		reflexion = addc_read_v(VAL_DET_DIFF);
		reflexion = reflexion + THE_VREF;
	#endif
	*/
	
 	if(etape!=2) // if the search is not fast the converter is not switched since this step only uses the acquisition of reflection
 	{
 		if(TYPE_C==0)
 			switch_can(1);   // commutation voie thermometre infrarouge
 	   else
 	   	switch_can(3);	// commutation voie pt 100 ou pt 1000 secondaire
 	}
 	//reflexion= ( 250.0f - reflexion / ( FS / 250.0f )); // formatting given between 0.0 to 250.0
	//printf("reflexion raw = %f\n", reflexion);
	

	#ifdef SINGLE_ENDED
		reflexion = map(reflexion, 0, addc_read_v(VAL_REF), 250, 0) + opt.DELTA_VALUE_IR;
	#else	// is def DIFFERENTIAL
		reflexion = map(reflexion, 0, THE_VREF, 250, 0) + opt.DELTA_VALUE_IR;
	#endif

	//reflexion = map(reflexion, 0, 100, 0, 250);
}






/*

void acqui_miroir(void)   
//
//	RESULTAT	: 	met a jour la temperature du miroir en �C, preparation a l'acquisition de reflexion
//	DESCRIPTION	:	recupere la valeur numerique donnee par le convertisseur et calcul la temperature correspondante
//
{
 	tmir=(float)acqui_can(0);
 	switch_can(2);
 	tmir=( 1.0 /( KONST * 1000.0 ) ) * ( 1.0 / ( 65536.0 / tmir - 1.0 ) - 1.0 ) * (float)pente_tmir + (float)delta_tmir / 100.0;

}

void acqui_surface(void) 
//
// RESULTAT	: 	met a jour la temperature de surface en �C, preparation a l'acquisition du miroir
// DESCRIPTION	: 	renvoi la valeur de la temperature a la surface de l'echantillon
//
{
   //short val;
   
   if(TYPE_C==0)
   {
   	//val=acqui_can(1);
   	tsurf=acqui_can(1);
		switch_can(0);
		tsurf= (float)val / ( 65536.0 / 250.0 * 1000.0 ) * (float)pente_tsurf + (float)delta_tsurf / 100.0;
   }
   else
   {
    	//val=acqui_can(3);
 		tsurf=acqui_can(3);
 		switch_can(0);
 		tsurf=( 1.0 /( KONST * 1000.0 ) ) * ( 1.0 / ( 65536.0 /(float)valtsurf - 1.0 ) - 1.0 ) * (float)pente_tsurf + (float)delta_tsurf / 100.0;	
   }


   
   val=(short)(tsurf*100.0);
   
   tsurf_int-=fifo_tsurf[index_tsurf];           //on retire la mesure la plus ancienne de la FIFO
 	tsurf_int+=val;                  // integration la nouvelle valeur de tsurf
 	fifo_tsurf[index_tsurf]=val; 			//rangement de tsurf  
   index_tsurf++;
   if(index_tsurf>=NB_TSURF) index_tsurf=0;


   tsurf=((float)tsurf_int)/(100.0*NB_TSURF);

}

void acqui_reflexion(void)  
//
// RESULTAT	:	met a jour la valeur de reflexion, preparation a l'acquisition de surface
//	DESCRIPTION	:	mesure la presence de rosee
//
{
 	reflexion=(float)acqui_can(2);
 	if(etape!=2)  
 	{
 		if(TYPE_C==0)
 			switch_can(1);
 	   else
 	   	switch_can(3);
 	}
 	reflexion= ( 250.0 - reflexion / ( 65536.0 / 250.0 )); 
}

*/



void regulation_peltier(void)
{

	if (MirrorTempCalibrated != MIRROR_TEMP_STAGE_END) return; //we need to auto calibrate first!

	xdata float buffer_calcul;

   if(REGU_COUPELLE==0) return;
	if(DEM_AUTO!=0) return;// if head is in the high position, the regu is paused
	
	
	regu-=(short)regu_correction_P;	

	buffer_calcul  = consigne_regu;
	buffer_calcul -= tsurf;

	if(buffer_calcul > 5.0f) // capping of the error to be corrected in fct of the slope setpoint
	{
	 	buffer_calcul = 5.0f;
   }

	if(buffer_calcul < -5.0f)
	{
	 	buffer_calcul = -5.0f;	
   }

	// error represents here the ceiling difference between the temperature setpoint and the measured temperature

   regu_correction_P=(long)((float)regu_gain_P*buffer_calcul); // opposition to temperature variation

//   peltier_correction_P*=10.0f;         // opposition a variation de temperature

   if(((long)regu+regu_correction_P)>(long)32767) regu_correction_P=(long)32767 - (long)regu;
   if(((long)regu+regu_correction_P)<(long)(-32768)) regu_correction_P=(long)(-32768) - (long)regu;
   regu+=(short)regu_correction_P;
    
   if(TOP_REGU == 0)
   {
   	sort_regu();
    	return; 
   }
   TOP_REGU = 0;
  
  	buffer_calcul *= consigne_rampe;// opposition a variation de temperature
   buffer_calcul *= 0.2f;// opposition a variation de temperature
   buffer_calcul -= mesure_rampe; // before this calculation buffer_calcul represents the slope setpoint

// after this calculation_buffer represents the error to be corrected
  	regu_correction_I=(long)((float)regu_gain_I*buffer_calcul);// steering as a function of slope
 	if(((long)regu+regu_correction_I)>(long)32767) regu_correction_I=(long)32767 - (long)regu;
 	if(((long)regu+regu_correction_I)<(long)(-32768)) regu_correction_I=(long)(-32768) - (long)regu;
	regu+=(short)regu_correction_I;
	
	sort_regu();
}


void calcul_pente_temperature(void)
{
	static xdata float tsurf_prec = 0.0f;
	static xdata unsigned long time_system_prec=0;
	
 	if(time_system < time_system_prec) return;    // calcul toutes les 5 secondes
	time_system_prec = time_system + 10;

	TOP_REGU = 1;
	mesure_rampe = tsurf - tsurf_prec;
	mesure_rampe *= 12.0f; 
	tsurf_prec = tsurf;
} 

	
 /*
float calcul_rtd(unsigned short x)
{
	return ( 1.0f / ( KONST * ( 65536.0f / (float)x - 1.0f) ) - INV_KONST );	
}
   */















