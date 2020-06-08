#include "proto.h"
#include "HD44780.h"
#include "stdio.h"
#include "shared.h"

code char mask[8]={1,2,4,8,16,32,64,128};


void tempoI2C(void);


void tempoI2C(void)
{
 	//for(i=0;i<10;i++){}
}


static FILE *fp;

void startI2C(void)        // emission bit start 
{
	if (fp > 0) return; //already started

	fp = fopen("/home/pi/flash.bin", "rb+");

	if (fp <= 0) {
		FILE *temp = fopen("/home/pi/flash.bin", "wb+");
		if (temp != 0) {
			fseek(temp, 0L, SEEK_SET);
			log_info("init flash.bin to 0");
			char zero[256] = { 0 };
			fwrite(zero, 256, 1, temp);
			fflush(temp);
			fclose(temp);
		}
		fp = fopen("/home/pi/flash.bin", "rb+");
		if (fp <= 0)
			log_error("file open flash.bin");
		return;
	}

	fseek(fp, 0L, SEEK_SET);
}
//@TODO
void stopI2C(void)			// emission bit stop 
{
      /*SDA=0;
      tempoI2C();
 		SCL=1;
		tempoI2C();
		SDA=1; 
		tempoI2C();*/
}

void slave_ack(void)            // reception de l'acknoledge du peripherique 
{
		/*char compteur;

		SDA=1;   // en attente de reception
		compteur=0;
		tempoI2C();
		SCL=1;
		while((SDA!=0)&&(compteur<250))
 		{
 		 	compteur++;	
 		}	
      if(compteur==250) BUSY=1;
      else BUSY=0;
      SCL=0;
		tempoI2C();*/

     
}

void master_noack(void)             // emission du non-acknoledge
{
		/*SCL=1;
		tempoI2C();
		tempoI2C();
		SCL=0;
		tempoI2C();*/

}

void master_ack(void)				// emission de l'acknoledge
{
	/*SDA=0;
	tempoI2C();
	SCL=1;
	tempoI2C();
	tempoI2C();
	SCL=0;
	tempoI2C();*/
}



void envoiI2C(char octet)              // serialisation en emission d'un octet sur le bus
//
//	ENTREE			:	octet	
//	DESCPRIPTION	:	envoi un caractere sur le bus I2C
{
	/*char i;
	i=8;
	do 
  	{
  		i--;
  		if ( (mask[i] & octet ) !=0 ) {SDA=1;} else {SDA=0;}
  		tempoI2C();
  		SCL=1;
  		tempoI2C();
  		tempoI2C();
  		SCL=0;
 	}
  	while (i!=0);
   tempoI2C();*/

	log_info("envoiI2C");

}


char recoitI2C(void)					// recuperation d'un octet serialise sur le bus
/*
*	ENTREE			:	octet	
*	DESCPRIPTION	:	recoit un caractÞre sur le bus I2C
*				( SDA et SCL =0 en entrÚe et en sorties )
*/
{
	/*char i;
   char octet;

	SDA=1; //en attente de reception
	octet=0;
	i=8;
	do 
  	{
  		i--;
  		SCL=1;
  		tempoI2C();

  		if (SDA!=0)
  			octet=octet+mask[i]; 
		else
			tempoI2C();

		SCL=0;
		tempoI2C();
  	}
  	while (i!=0);

   return(octet);*/
	log_info("recoitI2C");
}


void tempo_ecriture(void)              // temporisation d'ecriture pour circuit flash le necessitant
{
 	/*unsigned int i;
 	for(i=0;i<10000;i++){}*/
}

void ecritI2C(char adrchip,char adrmem,char valeur)       // ecriture d'un octet pour peripherique adressage 8 bits
/*
*	ENTREE		:	char adrchip, adrmem, valeur
*	SORTIEE		:	char * erreur	
*	DESCRIPTION	:	ecrit (char) valeur sur le bus I2C
*				( SDA et SCL =0 en entree et en sorties )
*/
{
#ifdef DEBUG_RW
	printf("i2c write %d::%d = %d\n", adrchip, adrmem, valeur);
#endif
	fseek(fp, adrmem, SEEK_SET);
	if (!fwrite(&valeur, 1, 1, fp)) {
		log_error("file write fail");
	}
   
}

char litI2C(char adrchip,char adrmem)         // lecture d'un octet sur peripherique adressage 8 bits
/*
*	ENTREE			:	char adrchip, adrmem, * valeur	
*	SORTIE			:	char *erreur
*	DESCPRIPTION	:	lit (char) valeur sur le bus I2C
*				( SDA et SCL =0 en entrÚe et en sorties )
*/
{
   char valeur;

   fseek(fp, adrmem, SEEK_SET);
   if (!fread(&valeur, 1, 1, fp)) {
	   log_error("file read fail");
   }
#ifdef DEBUG_RW
   printf("i2c read %d::%d = %d\n", adrchip, adrmem, valeur);
#endif

   return(valeur);
}



float  recup_float(char adrchip,char adr)       // recuperation d'une donnee de type float sur peripherique adressage 8 bits
{
   union long_sp temp;
	char i;
	
	for(i=0;i<4;i++)
	{
 		temp.uc[i]=litI2C(adrchip,adr+i);
 		if(BUSY==1) i=4;
	}

 	return(temp.fl);

}


short recup_short(char adrchip,char adr)
{

   union short_sp temp;
	char i;

	for(i=0;i<2;i++)
	{
 		temp.uc[i]=litI2C(adrchip,adr+i);
 		if(BUSY==1) i=2;
	}

 	return(temp.s);

}
void sauve_float(char adrchip,char adr, float valeur)   // ecriture d'une donnee de type float sur peripherique adrressage 8 bits
{
  union long_sp temp;
	char i;
   
   temp.fl=valeur;

	for(i=0;i<4;i++)
	{
 		ecritI2C(adrchip,adr+i,temp.uc[i]);
 		if(BUSY==1) i=4;
	}

	fflush(fp);
}

void sauve_short(char adrchip,char adr, short valeur)
{
   union short_sp temp;
	char i;
   
   temp.s=valeur;

	for(i=0;i<2;i++)
	{
 		ecritI2C(adrchip,adr+i,temp.uc[i]);      //ordre inverse pour compatibilite avec fast/1
 		if (BUSY==1) i=2;
	}

	fflush(fp);
}

void sauve_txt(char adrchip,char adr, char * adr_source)
{
	char i;
	i=0;
	do
	{
 		ecritI2C(adrchip,adr+i,*(adr_source+i)); 
 		if(*(adr_source+i)==0) i=15;
		i++;
	} while((BUSY==0)&&(i<16));

	fflush(fp);
}


void recup_txt(char adrchip,char adr, char * adr_destination)
{
	char i;
	i=0;
	do
	{
 		*(adr_destination+i)=litI2C(adrchip,adr+i);  
 		if(*(adr_destination+i)==0) i=15;
 		i++;
	} while((BUSY==0)&&(i<16));
	*(adr_destination+15)=0;
}
















