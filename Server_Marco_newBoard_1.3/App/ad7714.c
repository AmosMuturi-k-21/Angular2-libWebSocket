
#include "proto.h"
#include "ad7714.h"

#include "addc.h"
/**************************************************************************/
/*		driver du convertisseur ANALOGIQUE NUMERIQUE              */
/*				AD7714					  */
/**************************************************************************/

//#define DEBUG

void tempo_init(void)      // tempo longue pour calibration du convertisseur
{
}

/*
void ecrit_AD7714_8(xdata char valeur)
{
	val_can=valeur;
	data_out();
  	DINCONV=1;
}

char lit_AD7714_8(void)
{
	data_in();
   return(val_can);
}
*/

void conv_pret(xdata char voie)        // attente de fin de conversion
{
}

char conv_test(xdata char voie)            // test de fin de conversion
{
}



/*
void init_voie(xdata char voie,xdata char mode_register,xdata char high_filter,xdata char low_filter)
{

 	ecrit_AD7714_8(0x20+voie);//acces en ecriture du registre de filtre haut du AD7714 avec voie
 	ecrit_AD7714_8(high_filter);
	ecrit_AD7714_8(0x30+voie);//acces en ecriture du registre de filtre bas du AD7714 avec voie 
	ecrit_AD7714_8(low_filter);
 	ecrit_AD7714_8(0x10+voie);//acces en ecriture du registre de mode du AD7714 avec voie 
 	ecrit_AD7714_8(mode_register);
}
*/

void init_voie(xdata char voie)             // initialisation d'une voie mode pseudo-differentiel , gain de 1, valeur filtre 0x400, mode 24 bits
{
}

//DEBUG
int32_t test_surface = 0xFFFFFF/1024;

unsigned long acqui_can(xdata char voie)      // acquisition donnee de 24 bits
{
	xdata union long_sp val;

	//printf("acqui_can %d\n", voie);

	int32_t raw = 0;
	switch (voie)
	{
	case 0:
		raw = addc_read_raw(VAL_MIR); // mirror
		break;
	case 1:
		
#ifdef SINGLE_ENDED
		raw = addc_read_raw(VAL_SURF); //surface
#else	// is def DIFFERENTIAL
		raw = addc_read_raw(VAL_SURF_DIFF); //surface
#endif

#ifdef DEBUG
		raw = test_surface -= perltier_get_perc();
#endif
		break;
	case 2:
		raw = addc_read_raw(VAL_DET); //reflection
		break;
	/*case 3:
		break;*/
	default:
		printf("err voie = %d\n", voie);
		break;
	}

   /*conv_pret(voie);
	
	val_can=0x58+voie;
  	data_out();
	
	data_in();	
	val.c[1]=val_can;		      // lecture registre poids fort
	data_in();
	val.c[2]=val_can;            // lecture registre poids moyen
  	data_in();
  	val.c[3]=val_can;            // lecture registre poids faible
   val.c[0]=0;*/

	//printf("raw val: %d\n",raw);

	raw <<= 8;
	memcpy(&val,&raw,4);

   return(val.ul);
}  



/*

unsigned short acqui_can(char voie)
{
	union short_sp val;

   conv_pret(voie);
	
	EA=0;
	ecrit_AD7714_8(0x58+voie); 			//acces en lecture du registre de data mode 16 bits
		
	val.c[0]=lit_AD7714_8();		      // lecture registre poids fort
	val.c[1]=lit_AD7714_8();            // lecture registre poids moyen

   EA=1;
   return(val.us);
}  
*/
/*
unsigned long acqui_can(xdata char voie_en_cours,xdata char voie_suivante)
{
	xdata union long_sp val;

	conv_pret(voie_en_cours);
	
	ecrit_AD7714_8(0x58+voie_en_cours); 			//acces en lecture du registre de data mode 24 bits
	val.c[0]=0;
	val.c[1]=lit_AD7714_8();		      // lecture registre poids fort
   val.c[2]=lit_AD7714_8();            // lecture registre poids moyen
   val.c[3]=lit_AD7714_8();            // lecture registre poids faible
	
	if(voie_en_cours==voie_suivante) return(val.ul);
	
	ecrit_AD7714_8(0x10+voie_suivante); //acces en ecriture du registre de mode du AD7714, swichtage voie suivante  
	ecrit_AD7714_8(0x00);	// mode unipolaire gain 1    	
   
   
   return(val.ul);
}  

*/
void switch_can(xdata char voie)           // commutation de voie
{
	//printf("switch_can %d\n",voie);
}


/**************************************************************************/
/*		            FIN  driver AD7714				  */
/**************************************************************************/















