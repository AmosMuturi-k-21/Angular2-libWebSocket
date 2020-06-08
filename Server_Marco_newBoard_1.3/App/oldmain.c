#include "vardef.h"
#include "math.h"


void reception_commande(void);

uint8_t Mod_MeCom_SetTemp(uint8_t , float );
void Mod_MeCon_EnableOutputStage(uint8_t );
			

 
//@TODO check
void init_compteurs(void)
{
  /*EA=0;
  TMOD=0x21; 
  IP=0;
  EX0=0;
  EX1=0;
  ET0=0; 
  ET1=0;*/
//  TR0=1;
    //Enable timer1 overflow interupts
  
  //BP_GMOD=0;
 // EXEN2=1;
  //EBP_G=1;

}
//@TODO check
void init_RS232_9600(void)
/*
*	DESCRIPTION	:	initialisation de la liaison serie
*/
{
//initialisation du port serie 9600bds,
//   8bits de datas
//    1 de start(0),
//    1 de stop (1)

}

void init_AD7714(void)   // initialisation du convertisseur analogique-numerique
{
	/*EA=0;
	if(DISPLAY!=0) efface_ligne(L3);
	if(DISPLAY!=0) affiche_titre("1/5",L3);
	
	tempo_init();
	init_voie(2);
	if(DISPLAY!=0) affiche_titre("2/5",L3);

   init_voie(1);
	if(DISPLAY!=0) affiche_titre("3/5",L3);
	
	init_voie(3);
	if(DISPLAY!=0) affiche_titre("4/5",L3);
	
	init_voie(0);
	tempo_init();
	EA=1;*/
}

void logo(void)
{

	
	cde_display(CLEAR);
	affiche_titre("Find us on:",L1);
	affiche_titre("www.gbxonline.com",L2);	
	DELAY_DISPLAY;;
	cde_display(CLEAR);
   affiche_titre(gbx,L1);

#ifdef FRANCAIS

   affiche_titre("v 3.62F",L2); 	
#endif 
#ifdef ALLEMAND
   affiche_titre("v 3.62G",L2); 	
#endif 
   DELAY_DISPLAY;;
   
   efface_ligne(L2);
 
   switch(litI2C(I2C_TETE,OPTION))
   {
    	case 'A':
    	case 'B':
    		affiche_titre("FA-st Lab Light",L2);	
         break;
      case 'C':
      	affiche_titre("FA-st Lab Prosoft",L2);
      	break;
      case 'D':
      	affiche_titre("FA-st Lab Elite",L2);
      	affiche_date_courante();
      	break;
      case 'E':
      	affiche_titre("FA-st Lab Regu",L2);
         break;

   }
   
   DELAY_DISPLAY;;
   
   efface_ligne(L3);
   efface_ligne(L2); 
   affiche_titre(identifiant,L2);

   

}

//@TODO
void envoi_pc(char carac)    // envoi d'un caractere sur la liaison serie
{
   /*TI=0;
   SBUF=carac;
   while(DATA_TX==0) {}
   DATA_TX=0;*/
}

/*

void send_pc(char * texte)
{
 char i;
 i=0;
 
 while (texte[i]!=0) 
 {
    envoi_pc(texte[i]);
    i++;
 }
 //envoi_pc(13);
 //envoi_pc(10);

}
*/

/*
void send_pc(char * texte)
{
 char i;
 i=0;
 
 while (texte[i]!=0) 
 {
    TI=0;
    SBUF=texte[i];
    while(DATA_TX==0) {}
    DATA_TX=0;
    i++;
 }
 //envoi_pc(13);
 //envoi_pc(10);

}

*/



//@TODO check
void reset_carte(void)
{
	oldboard_start();
 //reset_on();
 init_variables();
 init_compteurs();

 init_display(); 
 DISPLAY=lcd_working();

 //reset_off();
 
 init_RS232_9600();
}


void init_carte(void)
{

 if(DISPLAY!=0) logo();

 init_AD7714();

 moteur_on();

 if(DISPLAY!=0) cde_display(CLEAR);

}

//#PRAGMA optimizespeed

/*void bip(xdata char amplitude,xdata char frequence, xdata char duree)   // emission d'un bip (ou tempo avec amplitude=0)
{
	xdata char i;
   xdata char j;
 
   ET0=0;
   
	while(duree!=0)
	{
		moteur_bip-=amplitude;
		sort_moteur_bip();
		
		for(i=0;i<frequence;i++)
		{
			for(j=0;j<50;j++);
		}
	   
	   moteur_bip+=amplitude;
		sort_moteur_bip();
		
		for(i=0;i<frequence;i++)
		{
			for(j=0;j<50;j++);
		}
	   
	   duree--;
	}
}*/

//#PRAGMA optimizesize


//cristian
void auto_calib() {	
}

double tm_buttons = -1;
double tm_reset_click = -1;
char button_pressed = 0;
char old_button_status = 1;

void gestion_bp(void) // this function manages the operation of the 2 buttons of the device in normal mode and result table  
{
   xdata char i, j;
	xdata char pas;

   if(MENU!=0) return;
   if(TABLEAU!=0) // if mode array result
   {

      if((BP_D==0)&&(BP_G==0))      // back to main screen
      {
       	TABLEAU=0;
         while(BP_D==0) { DELAY };
         while(BP_G==0) { DELAY };
         M_BP_D=0;
         M_BP_G=0;
       	bip(volume_key,10,35);
       	cde_display(CLEAR);
       	return;
      }


   	if(BP_D==0)                   // scrolling avant
   	{
   	 	if(M_BP_D!=0)
   	 	{
   	 		M_BP_D=0;
   	 		pas=nb_mesure-ligne_select-1;
   	 		if(pas>=1)
   	 		{
   	 		 	pas=1;
   	 			ligne_select+=pas; 	
   	 			bip(volume_key,7,35);
   	 		}
   	 	}	
   	}	
   	else M_BP_D=1;
   
   	if(BP_G==0)                    // scrolling arriere
   	{
   	 	if(M_BP_G!=0)
   	 	{
   	 		M_BP_G=0;
   	 		pas=ligne_select;
   	 		if(pas>=1)
   	 		{
   	 		 	pas=1;
   	 		 	ligne_select-=pas; 	
   	 		 	bip(volume_key,7,35);
   	 		}
   	 	}	
   	}	
   	else M_BP_G=1;
      
      return;
   } 	
   	
// si mode normal

   if(MESURE_EN_COURS==0) // managing start-up based on single, double, or extension of the right button
   {
	   if (BP_D) {
		   tm_buttons = -1;
	   }else
			if (tm_buttons == -1)
			tm_buttons = time_ms();

	   if (tm_reset_click && BP_D && (time_ms() - tm_reset_click > 500)) {
			   tm_reset_click = -1;
			   button_pressed = 0;

			   /*if (button_pressed == 1) {
				   CINETIQUE = 0;
				   printf("click\n");
			   }

			   if (button_pressed >= 2) {
				   CINETIQUE = 1;
				   printf("double click\n");
			   }*/
	   }

	   if (old_button_status != BP_D) {
		   button_pressed++;
		   old_button_status = BP_D;
		   if (tm_reset_click == -1)
			   tm_reset_click = time_ms();
	   }

		if(BP_D==0)
		{
 		   if(M_BP_D==0)
	 		{
	 			if(tm_buttons != -1)
	 			{	
	 				M_BP_D=1;
	 				CALIB=1;
	 				CINETIQUE=0;
	 				CYCLE_INT=1;
	 		      return;
	 		   }
	 		   else
	 		   	return;
	 		}
 		   
Calib: 		   
 		   
 		   if(CALIB==1)
 		   {
 		    	if((tm_buttons != -1 && ((time_ms() - tm_buttons) >= 4000)) || (num_pt_calib == 1))
 		    	{	
				printf("calib: tm_buttons = %f\n", tm_buttons);
				printf("calib: num_pt_calib = %d\n", num_pt_calib);
				tm_buttons = -1;
 		      	auto_noyau();
 		      	M_BP_D=0;
       		 	cde_display(CLEAR);
     		   	affiche_ligne(txt_message[(char)LANGUE][26],L0);	
       		 	cde_display(L1);

       		 	for(i=0, j=0; tab_liste[mode_calibration & 0x7F][i] != EOF; i++)  // affichage des sels de references possibles pour la calibration
       		 	{
      		 	   #define num_sel_trouve nb_point	
      		 		//auto_noyau();
      		 		//transfert_pc();  		
      		 		
      		 		if((i != num_sel_trouve) || (num_pt_calib == 0))	
      		 		{
 				 			txt_display(sel_ascii[tab_liste[mode_calibration & 0x7F][i]]); //affichage des sels possibles avec suppression sel 1er point
       		 			j++;
       		 		   switch(j)
       		 			{
       		 				case 3:
       		 					cde_display(L2);
       		 					break;
       		 				case 6:
       		 					cde_display(L3);
       		 					break;
       		 				default:
       		 					data_display(' ');
       		 					break;
       		 			}	
       		 		}
        		 	}
 
					bip_modul();
								
       		 	while(BP_D==0)
       		 	{
       		 		//auto_noyau();
       		 		//transfert_pc();
					DELAY
       		 		
       		 	};
      		 	
       		 	CALIB=0;
       		 	
       		 	while((CALIB==0)&&(BP_D!=0))
       		 	{
       		 		
       		 		efface_ligne(L0);
       		 		if((mode_calibration >= 128)&&((time_etape>>2)%2  == 1)) affiche_titre(txt_message[(char)LANGUE][27+num_pt_calib],L0);
      		 		else affiche_ligne(txt_message[(char)LANGUE][26],L0);	       		 		
       		 		
       		 		
       		 		auto_noyau();
       		 		transfert_pc();  		 		
       		 		
       		 		if(DEM_AUTO!=M_DEM_AUTO)   // detection transition
	               {
							M_DEM_AUTO=DEM_AUTO;
	                 							
	
							if(M_DEM_AUTO==0)  // tête position basse
							{
								switch(opt.AUTOSTART)
								{
									case 0:
            						break;
         						case 1:
         						case 3:
										CALIB=1;
										break;
									case 2:
									case 4:
										#define M_POS NOUVEAU_POINT
										if(M_POS==0) M_POS=1;
										else
										{
											M_POS=0;
											CALIB=1;
										}
			   						break;	
       		 	   		}
       		 			}
       		 		}
					DELAY
       		 	}
       		 	CALIB=1;

       		 	while(BP_D==0)
       		 	{
       		 		auto_noyau();
       		 		transfert_pc();  		 
					DELAY
       		 	};
       		 	
       		 	cde_display(CLEAR);
       		 	
       		 	calib_auto_std();
       	      return; 	
 		   	}
 		   }
	 		
	 		if(CINETIQUE==1)  
	 		{
 				M_BP_D=0;
 				if(litI2C(I2C_TETE,OPTION)<'C')
 					 	mesure_std();	
 				else cinetique_std();

				tm_buttons = -1;
 		      return;
	 		}
			
		
		}
      else
      {
        	if(M_BP_D==0) return;

       	CINETIQUE=1;
 			


        	if(time_etape!=0)
        	{
         	M_BP_D=0;
         	if(num_pt_calib != 0) goto Calib; 
         	mesure_std(); 	
            return;
         }
      }
   }
	else
	{
		if (BP_D) {
			tm_buttons = -1;
		}
		else
			if (tm_buttons == -1)
				tm_buttons = time_ms();

	 	if(BP_D==0)
	 	{
		 	if(tm_buttons != -1 && (time_ms() - tm_buttons > 0)||(etape!=0))
		 	{
		 		M_BP_D=0;
		 		arret_utilisateur();
	      }
	   }
	}
}


void gestion_demarrage_auto(void)
// this function manages the automatic start operation according to the selected mode
{
   #define M_POS NOUVEAU_POINT       // utilisation d'un bit non utilise a ce stade
	
	PURGE=DEM_AUTO;
	if(DEM_AUTO==M_DEM_AUTO) return;  // detection transition

	//check gpio
	delay(30);
	if (DEM_AUTO==M_DEM_AUTO) return;  // detection transition

	printf("magnetic detected\n");
	
	M_DEM_AUTO=DEM_AUTO;

	
	if(M_DEM_AUTO==0)  // tête position basse
	{
		TSURF_MEM=0;
		moteur_on();
		
		switch(opt.AUTOSTART)
		{
			case 0:
				printf("autostart is disabled\n");
            break;
         case 1:
				mesure_std();
				break;
			case 2:
				if(M_POS==0) M_POS=1;      // demarrage un coup sur 2
				else
				{
					M_POS=0;
					mesure_std();
				}
			   break;
			case 3:	
				cinetique_std();
				break;
			case 4:
				if(M_POS==0) M_POS=1;		// demarrage un coup sur 2
				else
				{
					M_POS=0;
					cinetique_std();
				}
		      break;
		}
	}
	else     // tete position haute
	{
		int abort = MESURE_EN_COURS;

		M_BP_D=0;
		printf("operation aborted\n");
		arret_utilisateur();
		time_etape=0;

		//UI
		/*if (abort)
			ui_error_msg(ui_aborted_magnetic);*/
	}
}	
	

	


//@TODO call when send
void gestion_commande(void)// interrupt routine called when a character is received or sent on the serial link
{
	/*if(RI!=0)
	{
		reception_commande();
		RI=0;
		(*p_cde[commande])();
   }
	if(TI!=0)
	{
		TI=0;
      DATA_TX=1;
   }   */
}

void reception_commande(void)     // gestion des commandes envoyer par le PC
{

   if(TRANS_PARAM!=0) return;    // si transmission de parametre en cours on ne passe pas par le decodage de commande

  	commande=0;
      
   switch(etape_communication)
   {
      		case 0: 
      				if(SBUF=='G') etape_communication++;
      		      else etape_communication=0;
      		      break;
      		case 1:
       		      if(SBUF=='B') etape_communication++;
      		      else etape_communication=0;
      		      if(SBUF=='G') etape_communication=1;
      		      break;
      		case 2:
      				if(SBUF=='X') etape_communication++;
      		      else etape_communication=0;
      		      if(SBUF=='G') etape_communication=1;
      		      break;
      		case 3:
      	         if(SBUF<NB_CDE) commande=SBUF;
       	         else commande=ERREUR_CDE;
      	         etape_communication=0;
      	         break;
   }

}
  		


	   





void affiche_message(void)      // affichage des differents messages
{

  	if((MESURE_EN_COURS==0) && (message!=0))
  	{
  	 	cde_display(CLEAR);
  	 	affiche_titre(txt_message[(char)LANGUE][message],L1);
		DELAY_DISPLAY
 	 	return;
  	
  	}
  
  
  
  
  


   if(CLEAN_MIRROR!=0)
   {
   	if(nb_erreur < 3) affiche_titre(txt_message[(char)LANGUE][15],L1); 	
   	else affiche_titre(txt_message[(char)LANGUE][2],L1);
	DELAY_DISPLAY
	}
   else
   {
		if(CALIB==1)
   	{
     		affiche_titre(txt_action[(char)LANGUE][2],L1); 
			DELAY_DISPLAY
   	}
		if(CINETIQUE==1)
  	 	{
   		affiche_titre(txt_action[(char)LANGUE][1],L1); 	
		DELAY_DISPLAY
   	}
   	if((CALIB==0)&&(CINETIQUE==0))
   	{	
   		affiche_titre(txt_action[(char)LANGUE][0],L1); 
		DELAY_DISPLAY
   	}
		if(type_capteur=='C')
		{	
			if(etape == 3)
			{
				if ((tmir - ttete) > -0.25f) {
					affiche_titre(txt_message[(char)LANGUE][16], L1); // alarme risque condensation
					DELAY_DISPLAY
				}
			}
			else
			{
				if ((tsurf - ttete) > 0.25f) {
					affiche_titre(txt_message[(char)LANGUE][16], L1); // alarme risque condensation
					DELAY_DISPLAY
				}
			}
			
		}
		affiche_titre(txt_etape[(char)LANGUE][etape],L2); 
		DELAY_DISPLAY
 	}
}

/*
void affiche_colonne(char ligne)
{
	cde_display(ligne+3);
   data_display(124);
 	cde_display(ligne+9);
   data_display(124);
   cde_display(ligne+14);
   data_display(124);
   
}
*/

void affiche_tableau(void)        // affiche des 3 lignes du tableau de resultat
{
   
   formate_mesure();


   affiche_ligne(txt_tableau[(char)LANGUE],L0);
	auto_noyau();


	

	affiche_ligne_tableau(ligne_select,L1);
	auto_noyau();
//	affiche_colonne(L1);


	affiche_ligne_tableau(ligne_select+1,L2);
	auto_noyau();
//	affiche_colonne(L2);



	affiche_ligne_tableau(ligne_select+2,L3);
	auto_noyau();
//	affiche_colonne(L3);

}

void affiche_ligne_tableau(xdata char index_ligne, xdata char ligne)    // affichage d'une ligne du tableau de resultat
{
	xdata union fiche *ptr_fiche;
	
	if(index_ligne > nb_mesure) 
	{
		efface_ligne(ligne);
		return;
	} 
	
	if(index_ligne==nb_mesure)// if selected line is the current measurement
	{
		if((MESURE_EN_COURS==0)||(num_tache<=6)) // it is blinking
		{
			efface_ligne(ligne);
			return;
   	}
      ptr_fiche=&fiche;
   }
   else
   		ptr_fiche=&tableau_result[index_ligne];

	for(ptr_ascii=0;ptr_ascii<20;ptr_ascii++) buffer_ascii[ptr_ascii]=' ';
	
	ptr_ascii=0;
	if((ptr_fiche->mesure.etat&0x04)==0) // if it is not a calibration
	{
		
		ushort_ascii(ptr_fiche->mesure.numero);

	}
	else  // si la ligne est le resultat d'une calibration on affiche 'Cal' 
		txt_ascii(txt_message[(char)LANGUE][12]);


	buffer_ascii[ptr_ascii]=' ';
	ptr_ascii=4;
	if(ptr_fiche->mesure.aw!=0)     // si la mesure finale est disponible 
	{

		if((ptr_fiche->mesure.etat&0x04)==0)    // si ce n'est pas une calibration
		{
			float_ascii((float)ptr_fiche->mesure.aw/1000.0f,3);
		}
		else   // si calibration on affiche la valeur theorique du sel contenu alors dans le numero de mesure  
		{
			float_ascii((float)ptr_fiche->mesure.numero/1000.0f,3);
		}
	}
	
	buffer_ascii[ptr_ascii]=' ';
	ptr_ascii=10;
	float_ascii((float)ptr_fiche->mesure.tsurf/100.0f+0.05f,1);

	buffer_ascii[ptr_ascii]=' ';
	ptr_ascii=15;
	delay_ascii(ptr_fiche->mesure.duree);
   buffer_ascii[ptr_ascii++]=' ';
   buffer_ascii[20]=0;
	
	
	affiche_ligne(buffer_ascii,ligne);
}






void affiche_variables(void)     // affichage ecran normal
{
		//cde_display(CLEAR);
		
		if(num_tache==0) efface_ligne(L0);
		if((numero_mesure!=0)||(time_mesure!=0))
		{
			affiche_ligne(txt_variable[(char)LANGUE][0],L0);
			ptr_ascii=0;
			delay_ascii(time_mesure);
			if(time_mesure>=3600) // extension de l'affichage du temps a 8 caracteres (5+3)
			{
				if(time_mesure>=360000) buffer_ascii[ptr_ascii++]=(time_mesure/60)%10+'0';		
				buffer_ascii[ptr_ascii++]=39;
				buffer_ascii[ptr_ascii++]=(time_mesure/10)%6+'0';
				if(time_mesure<360000) buffer_ascii[ptr_ascii++]=time_mesure%10+'0';			
				buffer_ascii[ptr_ascii]=0;
			}
			
			txt_display(buffer_ascii);
			
 		}

   	auto_noyau(); 
   
   	if(etape!=2)
   	{
   		if(num_tache==0) efface_ligne(L1);
   		affiche_ligne(txt_variable[(char)LANGUE][1],L1);
   		ptr_ascii=0;
   		if(TSURF_MEM==0)    
   		{
   			float_ascii(tsurf,2);
   			data_ascii(223);
   			data_ascii('C');
   			txt_display(buffer_ascii); 
   		}
   		else   
   		{

   			if(time_etape%2!=0) txt_display("        ");
   			else 
   			{	
   				float_ascii((float)tableau_result[nb_mesure-1].mesure.tsurf/100.0f+0.05f,1);
   				data_ascii(223);   // signe °
   				data_ascii('C');
   				txt_display(buffer_ascii);
   			}
   		}
   		
   		data_display(' ');
   	}
    
   	auto_noyau();
   
   	if(num_tache==0) efface_ligne(L2);
   	if(etape==3)
   	{
   		affiche_ligne(txt_variable[(char)LANGUE][2],L2);
   		ptr_ascii=0;
   		float_ascii((float)aw_moy/1000.0f,3);
               	
   		txt_display(buffer_ascii);
   	}
		if(aw_finale!=0)
		{
         affiche_ligne(txt_variable[(char)LANGUE][3],L2);
         ptr_ascii=0;
         float_ascii((float)aw_finale/1000.0f,3);
			if((TSURF_MEM!=0)&&(time_etape%2!=0)) txt_display("        ");
			else txt_display(buffer_ascii);
		}

		auto_noyau();
	   
	   if(num_tache==0) efface_ligne(L3);
		if((MODE_FLASH!=0)&&(CALIB==0)&&(CINETIQUE==0)&&(aw_flash!=0)) 
   	{
         affiche_ligne(txt_variable[(char)LANGUE][4],L3);
         ptr_ascii=0;
         float_ascii((float)aw_flash/1000.0f,3);
    		if((TSURF_MEM!=0)&&(time_etape%2!=0)&&(aw_finale==0)) txt_display("        ");
			else txt_display(buffer_ascii);
   	}
		if((MESURE_EN_COURS==0)&&(CALIB!=0)&&(aw_finale!=0)&&(aw_flash!=0)&&(M_BP_D==0))
		{
         affiche_ligne(txt_variable[(char)LANGUE][5],L3);
        
         switch(time_etape%4)
         {
          	case 0:
          	  	ptr_ascii=0;
          	  	float_ascii((float)aw_flash/1000.0f,3);	  		
					txt_display(buffer_ascii);
			      break;
			   case 2:
			   	txt_display(sel_ascii[tab_liste[mode_calibration & 0x7F][nb_point]]);	
			      break;
			   default:
			   	txt_display("        ");
			   	break;
			}
		}
}




void affiche_mesure(void)    // gestion du sequencement de l'affichage (messages en alternance) 
{
	
	if(DISPLAY==0) return;


   switch(time_etape%16)
   {
   	case 3:
   	case 4:
   	 		if((MESURE_EN_COURS!=0)||(CLEAN_MIRROR!=0)||(message!=0)) affiche_message();
         	else
            {
             	if(TABLEAU==0) affiche_variables();
            	else affiche_tableau();	
            }            
            break;
   	case 2:
   	case 5:
   	      if((MESURE_EN_COURS!=0)||(CLEAN_MIRROR!=0))
   	      {
   				cde_display(CLEAR);
   				break;
   	      }
   	default:
  	        	if(TABLEAU==0) affiche_variables();
            else affiche_tableau();
  	
  	
  	}
}







static char read_param_value(char numero)   // lecture des paramtres contenu dans la tete
{
		switch(numero)
		{
			case TAB_RESULTS:
				return(0);
			case TAB_REGU:
				return(0);
			case TAB_HISTO:
				return(0);
			case TAB_MOTEUR:
				//return(litI2C(I2C_TETE,VIT_MOTEUR));
				return opt.FAN_SPEED;
	   	case TAB_MODE_MES:
	   		//return(litI2C(I2C_TETE,MODE_MESURE));	
			return opt.MEASURE_MODE;
	   	case TAB_MODE_CINET:
	   		//return(litI2C(I2C_TETE,MODE_CINETIQUE));
			return opt.KINETIC_MODE;
	   	case TAB_MODE_CALIB:   
	   		//return(litI2C(I2C_TETE,MODE_CALIB));
			return opt.CALIBRATION_MODE;
	   	case TAB_DEM_AUTO:
	   		//return(litI2C(I2C_TETE,MODE_DEMARRAGE));	
			return opt.AUTOSTART;
	   	case TAB_AUTO_PRINT:
	   		return(litI2C(I2C_TETE,AUTO_PRINT));	   		
	   	case TAB_VOL_MES:
	   		return(litI2C(I2C_TETE,VOLUME_BIP));
 			case TAB_VOL_KEY:
	   		return(litI2C(I2C_TETE,VOLUME_KEY));
		   case TAB_LANGUE:
		   	return(litI2C(I2C_TETE,ADR_LANGUE));
		}
}






void write_param_value(xdata char numero, xdata char value)   // ecriture des modifications ou lancement des fonctions demandees
{
		switch(numero)
		{
			case TAB_RESULTS:
				switch(value)
				{
					case 0:
					 	TABLEAU=1;
				 		if(nb_mesure!=0) ligne_select=nb_mesure-1;
					   break;
					case 1:
						imprime_tableau();
					   break;
					case 2:
						transfert_tableau();
				      break;
				 	case 3:
				 		stat_histo(0);
				 		break;
				 	case 4:
				 		efface_tableau();
				 		break;
				}
			   break;

			case TAB_REGU:
				cde_display(L2);
				data_display(' ');
				cde_display(L2+19);
				data_display(' ');	     			
				switch(value)
				{
					case 0:
						bip(volume_key,7,35);
					 	select_consigne_regu();
					 
					   //ecritI2C(I2C_TETE,SEL_CONSIGNE_REGU,value);
					   break;
					case 1:
						start_regu();
						ecritI2C(I2C_TETE,REGU_ON_OFF,1);
						moteur_on();
						bip_modul2();
					   break;
					case 2:
						stop_regu();
						ecritI2C(I2C_TETE,REGU_ON_OFF,0);
						bip_modul4();
				      break;
				}
			   break;

			
			case TAB_HISTO:
				cde_display(L2);
				data_display(' ');
				cde_display(L2+19);
				data_display(' ');	     
				
				switch(value)
				{
					case 0:
					 	select_date();
					   break;
					case 1:
						affiche_histo();
					   break;
					case 2:
						imprime_histo();
					   break;
					case 3:
						transfert_histo();
				      break;
				 	case 4:
				 		stat_histo(1);
				 		break;
				 	case 5:
				 		reglage_date();
				 		bip_modul2();
				 		break;
				}
			   break;
			case TAB_MOTEUR:
				//ecritI2C(I2C_TETE,VIT_MOTEUR,value);
				opt.FAN_SPEED = value;
				sync_opt();
				time_etape = 0;
				//moteur_on();
						
	   	   break;
	   	case TAB_MODE_MES:
	   		//ecritI2C(I2C_TETE,MODE_MESURE,value);	
			opt.MEASURE_MODE = value;
			sync_opt();
	   	   break;
	   	case TAB_MODE_CINET:
	   		//ecritI2C(I2C_TETE,MODE_CINETIQUE,value);	
			opt.KINETIC_MODE = value;
			sync_opt();
	   	   break;
	   	case TAB_MODE_CALIB:   
	   		mode_calibration = value;
	   		//ecritI2C(I2C_TETE,MODE_CALIB,value);	
			opt.CALIBRATION_MODE = value;
			sync_opt();
	   	   break;
	   	case TAB_DEM_AUTO:
	   		//ecritI2C(I2C_TETE,MODE_DEMARRAGE,value);	
			opt.AUTOSTART = value;
			sync_opt();
	   	   break;
	   	case TAB_AUTO_PRINT:
	   		ecritI2C(I2C_TETE,AUTO_PRINT,value);		
	   	   break;
	   	case TAB_VOL_MES:
	   		ecritI2C(I2C_TETE,VOLUME_BIP,value);		
	   	   break;
 			case TAB_VOL_KEY:
	   		ecritI2C(I2C_TETE,VOLUME_KEY,value);		
	   	   volume_key=value;
	   	   break;
 		   case TAB_LANGUE:
 		   	LANGUE=value;
 		   	ecritI2C(I2C_TETE,ADR_LANGUE,value);		
	   	   break;
 		
 		   default:
 		   	return;
 		}	
      if(numero>=3)
      		bip_modul2();    // bip signalant une ecriture     
      cde_display(CLEAR);  // Ajout 03/10/2011		
      		
      		
}

void gestion_menu(void)      // gestion globale du menu utilsateur
{
	xdata char menu;
	xdata char choix;
	xdata char niveau;
	xdata char i;
//unsigned short tempo;
	
   //EA=0;
	MENU=1;    // affichage menu active

	menu=0;
	niveau=1;
	M_BP_G=0;
	bip(volume_key,7,35);
   cde_display(CLEAR);
	
	
	while((option_menu_min[menu]>litI2C(I2C_TETE,OPTION))/* && (option_menu_max[menu]<=litI2C(I2C_TETE,OPTION))*/)     //  si le menu ne correspond pas a l'option de l'appareil ( option light par ex) on passe au suivant
   {
     	menu++;
     	if(option_menu_min[menu]==0) menu=0;
   }
	while(niveau!=0)
	{
		
		affiche_titre("MENU",L0);
		efface_ligne(L1);
		affiche_titre(txt_menu[(char)LANGUE][menu],L1);   // affichage du nom du menu en cours de selection
		if(niveau==1) 
		{
				cde_display(L1);       // affichage des fleches 
				data_display(126);
				cde_display(L1+19);
				data_display(127);
				
				choix=read_param_value(menu);    

				i=0;
				while((val_choix[menu][i]!=choix)&&(option_choix[menu][i]!=0))//  si le choix ne correspond pas a l'option de l'appareil ( option light par ex) on passe au suivant
				{
				 	i++;
				}
				
				if(val_choix[menu][i]==choix) 
				{
					choix=i;
				}
				else
				{
					choix=0;
            	write_param_value(menu,val_choix[menu][choix]);
            }


		}
		
		efface_ligne(L2);
	   if(option_menu_min[menu]!=ESC)      
		{
			
			affiche_titre(txt_choix[(char)LANGUE][menu][choix],L2);  // affichage du choix du menu en cours 	
      	if(niveau==2) 
			{
				cde_display(L2);
				data_display(126);
				cde_display(L2+19);
				data_display(127);
			}
      }
		
		switch(menu)
		{
			case TAB_REGU:
			 	ptr_ascii=0;
			 	//txt_ascii("A");
			 	float_ascii(tab_regu[litI2C(I2C_TETE,SEL_CONSIGNE_REGU)],1);
				affiche_regu_temp();  
   			DATE_SELECT=0;  						
		      break;
		   case TAB_HISTO:
	      	if(DATE_SELECT==0)
   	   	{
					affiche_date_courante();
    			} 
         	else
         	{
         		read_enregistrement(&fiche,adresse_jour);      // affichage de la date selectionnee
         	
         		ptr_ascii=0;
         		date_ascii(fiche.mesure.date);
         		affiche_titre(buffer_ascii,L3);
	         }		   
				break;
			default:
				efface_ligne(L3);
				DATE_SELECT=0;
				break;	
		}

     
     	if(BP_D==0)        // gestion des validations et changements de niveau
     	{
      	if(M_BP_D!=0)     // si detection appui sur bouton droit   
      	{
      		M_BP_D=0;
//				efface_ligne(L1);          // rafraichissement affichage
//  				affiche_titre(txt_menu[(char)LANGUE][menu],L1);
//				efface_ligne(L2);
//				affiche_titre(txt_choix[(char)LANGUE][menu][choix],L2);
    			
    			if(niveau==1)   // si niveau selection menu 
     			{
					if(option_menu_min[menu]!=ESC)  // si on est positionne sur une sortie on sort sinon on passe au niveau selection choix 
						niveau++;	
     	 			else niveau=0;			
        		}
        		else  // si niveau selection du choix d'un menu selectionne 
        		{
     				
     				if(option_choix[menu][choix]!=ESC)
            	{
	
     					write_param_value(menu,val_choix[menu][choix]);
        				if(TABLEAU!=0) niveau=0;
        				else
        				{
        					
        					if((menu!=TAB_RESULTS)&&(menu!=TAB_REGU)&&(menu!=TAB_HISTO)) niveau--;    // le parametre a ete ecrit on retourne au niveau selection du menu
        					
        					//if(menu==TAB_HISTO) efface_ligne(L3);  // enlevé le 16/06/10
        				}
        			
        	 		}
        	 		else
        	 		{
        	 			niveau--;//niveau=0;  //16/06/10
        		      DATE_SELECT=0;
        		   }
        		}
        		bip(volume_key,7,35);
//     		   efface_ligne(L1);
//      		efface_ligne(L2);	
     		}
     	}
     	else M_BP_D=1;
      
     	if(BP_G==0)      // gestion du defilement des possibilites de selection
     	{
     		if(M_BP_G!=0)    // si detection appui bouton gauche
     		{
     			M_BP_G=0;

     			if(niveau==1)  
     			{
     		   	do
     		   	{
     		    		menu++;
     		    		if(option_menu_min[menu]==0) menu=0; // si la liste des possibilites est finie (caractere 0) retour au debut
     		   	} while((option_menu_min[menu]>litI2C(I2C_TETE,OPTION)) /*&& (option_menu_max[menu]>litI2C(I2C_TETE,OPTION))*/);
     			}
     			else
     			{
      	 		do
     		   	{
     		    	   choix++;
     		    		/*if((menu!=TAB_HISTO)||(DATE_SELECT!=0)) choix++;  //enlevé 16/06/10
     		    		else
     		    		{
     		    		 	if(choix==0) choix+=5;
     		    		 	else choix++;
     		    		}*/
     		    		
     		    		if(option_choix[menu][choix]==0) choix=0;   // si la liste des possibilites est finie (caractere 0) retour au debut
     		   	} while(option_choix[menu][choix]>litI2C(I2C_TETE,OPTION));
    			}
     			bip(volume_key,7,35);
//     		   efface_ligne(L1);
//      		efface_ligne(L2);	
     		}
     	

     	}
     	else M_BP_G=1;

		transfert_pc();

		DELAY
	}
   
   while(BP_D==0)
   {
   	tempo_cycle();
	DELAY
   }  
   cde_display(CLEAR);
   MENU=0;
}



void select_consigne_regu(void)
{
	xdata char choix;
	xdata char scrute;

	while(BP_D==0) {tempo_cycle(); DELAY}
	M_BP_D=1;
		
	choix = litI2C(I2C_TETE,SEL_CONSIGNE_REGU);

  	efface_ligne(L3);
	cde_display(L3);
	data_display(126);
	cde_display(L3+19);
	data_display(127);

	do
	{
		scrute = 1;
		ptr_ascii=0;
  		float_ascii(tab_regu[choix],1);
		
		while(scrute!=0)
		{

			auto_noyau();
			transfert_pc();
			auto_noyau();
			affiche_regu_temp();

						
			if((BP_G==0)&&(M_BP_G!=0))
   		{
       		choix++;
       		if(choix>=NB_CHOIX_REGU) choix=0;
      		bip_modul2(); 
    	   	scrute = 0;
      	}
			M_BP_G=BP_G;
      	if(BP_D==0) scrute=0;
      	M_BP_D=BP_D;

		DELAY
		}
		//bip(0,5,10);	
	}
	while(BP_D!=0);
	
	ecritI2C(I2C_TETE,SEL_CONSIGNE_REGU, choix);
	consigne_regu = tab_regu[choix];
	sauve_float(I2C_TETE,CONSIGNE_REGU,	consigne_regu); 	
	
   bip(volume_key,7,35);
   cde_display(L3);
	data_display(' ');
	cde_display(L3+19);
	data_display(' ');
}


void select_date(void)
{
  	xdata char scrute;
  	xdata unsigned short nb_essai;
   xdata unsigned long date;
   //char erreur;
   
	while(BP_D==0) {tempo_cycle(); DELAY}
	M_BP_D=1;
	
  	adresse_jour_suiv=adresse_courante;
  	
   do
   {  	
     	scrute=1;
     	efface_ligne(L3);
     	
     	if(adresse_jour_suiv==0) adresse_jour_suiv=adresse_courante;
		
		
		adresse_jour=read_enregistrement_prec(&fiche,adresse_jour_suiv);
      
      if(fiche.mesure.date==VIDE)
      {
      	adresse_jour=0;
         affiche_titre(txt_message[(char)LANGUE][4],L3);	
         return;
      }

      date=fiche.mesure.date;
		nb_essai=480;

		do
  		{
  	  		adresse_jour=read_enregistrement_prec(&fiche,adresse_jour);
     		if(fiche.mesure.date==VIDE) nb_essai=0; 
     		cde_display(L3);

  	  		if(nb_essai%16 == 0)
  	  		{
  	  			auto_noyau();
  	  			transfert_pc();
  	  			txt_display("Scan: ");
     			ptr_ascii=0;
     			ushort_ascii(adresse_jour);
				txt_display(buffer_ascii);
  	  		}
  	  		nb_essai--;				
 			
    	} while(((date/86400) == (fiche.mesure.date/86400))&&(nb_essai!=0));       // recherche transition date
   	
   	adresse_jour=read_enregistrement_suiv(&fiche,adresse_jour);   
		if(adresse_jour==adresse_courante) 
		{
			adresse_jour=0;
		   read_enregistrement(&fiche,adresse_jour);
		}
		
		//erreur=0; 

		DATE_SELECT=1;
		ptr_ascii=0;
      date_ascii(fiche.mesure.date);
      efface_ligne(L3);
      affiche_titre(buffer_ascii,L3);
		
	
		cde_display(L3);
		data_display(126);
		cde_display(L3+19);
		data_display(127);
		

      
		while(scrute!=0)
		{
	     
			auto_noyau();
			transfert_pc();
			   	  			     
	     
	     
	      if((BP_G==0)&&(M_BP_G!=0))
   	   {
         //	if(erreur==0)
         	//{
         		adresse_jour_suiv=adresse_jour;
         		M_BP_G=0;
      			bip(volume_key,7,35);
      	   	scrute=0;
      	 //  }
      	}
			M_BP_G=BP_G;
      	if(BP_D==0) scrute=0;
      	M_BP_D=BP_D;
		DELAY
      }
      
   
   }
   while(BP_D!=0);
   
   bip(volume_key,7,35);
   cde_display(L3);
	data_display(' ');
	cde_display(L3+19);
	data_display(' ');



}




void affiche_histo(void)
{
   xdata unsigned short adresse_ligne1;
   xdata unsigned short adresse_ligne;

   adresse_ligne1=adresse_jour;


   do
   {
  	   cde_display(CLEAR);
      affiche_ligne(txt_histo[(char)LANGUE],L0);
  	
  	   adresse_ligne=adresse_ligne1;
   	read_enregistrement(&fiche,adresse_ligne);
   	affiche_ligne_histo(&fiche,L1);
   	
   	adresse_ligne=read_enregistrement_suiv(&fiche,adresse_ligne);
   	if(adresse_ligne != adresse_jour_suiv) 
   	{
   		affiche_ligne_histo(&fiche,L2);
   	
   		adresse_ligne=read_enregistrement_suiv(&fiche,adresse_ligne);
   		if(adresse_ligne != adresse_jour_suiv) affiche_ligne_histo(&fiche,L3);
      }
      
      while((BP_G==M_BP_G)&&(BP_D==M_BP_D)) { DELAY }
      
      if((BP_G==0)&&(M_BP_G!=0)) 
   	{
         if(adresse_ligne1!=adresse_jour)
	      {
	         adresse_ligne1=read_enregistrement_prec(&fiche,adresse_ligne1);
	      	bip(volume_key,7,35);
      	}
      }
     	M_BP_G=BP_G;
     	
     	if((BP_D==0)&&(M_BP_D!=0)) 
   	{
  			
         if((adresse_ligne!=adresse_jour_suiv)&&(adresse_ligne!=adresse_courante))
	      {
	      	adresse_ligne1=read_enregistrement_suiv(&fiche,adresse_ligne1);
	      	bip(volume_key,7,35);
			}
      }
     	M_BP_D=BP_D;
		DELAY
   } while((BP_D!=0)||(BP_G!=0));      // retour ecran principal
   
   while(BP_D==0) { DELAY }
   while(BP_G==0) { DELAY }

   cde_display(CLEAR);
}






void affiche_ligne_histo(union fiche * ptr, xdata char ligne)
{
	
	if(ptr->mesure.date==VIDE)
	{
	 	efface_ligne(ligne);
	 	return;
	}
	
	for(ptr_ascii=0;ptr_ascii<20;ptr_ascii++) buffer_ascii[ptr_ascii]=' ';
	
	ptr_ascii=0;	
	if((ptr->mesure.etat&0x04)==0)
		ushort_ascii(ptr->mesure.numero);
	else 
		txt_ascii(txt_message[(char)LANGUE][12]);

	
	buffer_ascii[ptr_ascii]=' ';	
	ptr_ascii=4;
	if(ptr->mesure.aw!=0)
	{

		if((ptr->mesure.etat&0x04)==0)
		{
			float_ascii((float)ptr->mesure.aw/1000.0f,3);
		}
		else 
		{
			float_ascii((float)ptr->mesure.numero/1000.0f,3); 		
		}
	}
	
	buffer_ascii[ptr_ascii]=' ';
	ptr_ascii=10;
	float_ascii((float)ptr->mesure.tsurf/100.0f+0.05f,1);

	buffer_ascii[ptr_ascii]=' ';
	ptr_ascii=15;
	time_ascii(ptr->mesure.date);
   buffer_ascii[ptr_ascii++]=' ';
   buffer_ascii[20]=0;
	affiche_ligne(buffer_ascii,ligne);

}	






/*unsigned long recup_date(void)
{
   xdata unsigned long date;
   xdata char temp;



	temp=litI2C(ADR_HORLOGE,0);
	temp=hexa_bcd(temp,0x7f);
	date =  (unsigned long)temp ;

	temp=litI2C(ADR_HORLOGE,1);
	temp=hexa_bcd(temp,0x7f);
	date +=  temp * (unsigned long)60;

	temp=litI2C(ADR_HORLOGE,2);
	temp=hexa_bcd(temp,0x3f);
	date +=  temp * (unsigned long)3600;

	temp=litI2C(ADR_HORLOGE,4);
	temp=hexa_bcd(temp,0x3f);
	date +=  temp * (unsigned long)86400;

	temp=litI2C(ADR_HORLOGE,5);
	temp=hexa_bcd(temp,0x3f);
	date +=  temp * (unsigned long)2764800;

	temp=litI2C(ADR_HORLOGE,6);
	temp=hexa_bcd(temp,0xff);
	date +=  temp * (unsigned long)44236800;
   
   return(date);
}*/

/*void set_date(unsigned long date)
{
	char temp;

	temp=date%60;
	temp=bcd_hexa(temp,0x00);
	ecritI2C(ADR_HORLOGE,0,temp);
 			 		
	temp=(date/(unsigned long)60)%60;
	temp=bcd_hexa(temp,0x00);
	ecritI2C(ADR_HORLOGE,1,temp);
  			 		
	temp=(date/(unsigned long)3600)%24;
	temp=bcd_hexa(temp,0x00);
	ecritI2C(ADR_HORLOGE,2,temp);
  			 		
	temp=(date/(unsigned long)86400)%32;
	temp=bcd_hexa(temp,0x00);
	ecritI2C(ADR_HORLOGE,4,temp);
  			 		
	temp=(date/(unsigned long)2764800)%16;
	temp=bcd_hexa(temp,0x00);
	ecritI2C(ADR_HORLOGE,5,temp);
  			 		
	temp=(date/(unsigned long)44236800);
	temp=bcd_hexa(temp,0x00);
	ecritI2C(ADR_HORLOGE,6,temp);
} */ 			 		






char bcd_hexa(char valeur,char masque)
{
	valeur = (valeur/10)*16 + valeur % 10 ;
	valeur = valeur | masque;
	return(valeur);

}

char hexa_bcd(char valeur,char masque)
{
	valeur = valeur & masque;
	valeur = (valeur/16)*10 + valeur % 16;
	return(valeur);



}








	
void transfert_tableau(void)
{
	xdata char i;

	
	for(i=0;i<nb_mesure;i++)
	{
	
		fiche.mesure.date=tableau_result[i].mesure.date;
		fiche.mesure.numero=tableau_result[i].mesure.numero;
		fiche.mesure.aw=tableau_result[i].mesure.aw;
		fiche.mesure.tsurf=tableau_result[i].mesure.tsurf;
		fiche.mesure.duree=tableau_result[i].mesure.duree;
		fiche.mesure.etat=tableau_result[i].mesure.etat;
		fiche.mesure.config=tableau_result[i].mesure.config;
		
		trame_FI(i);
	
	}	

}


//@TODO
void reglage_date(void)
{

	xdata char index;
	xdata char val;

	index=0;
   
	cde_display(L3);
	data_display(126);
	cde_display(L3+19);
	data_display(127);	     
	
   while(index!=16)
   {
    	DELAY
		date_courante=recup_date();
		ptr_ascii=0;
		date_ascii(date_courante);
		txt_ascii("   ");
		time_ascii(date_courante); 
   	
   	if(num_tache<=8)
		{
			affiche_titre(buffer_ascii,L3);	
		}
		else
		{
	 		affiche_ligne(" ",L3+index+2);
		}
	
	
   	if(BP_G==0)
   	{
    	 if(M_BP_G!=0)	
    	 {	
    		
    		M_BP_G=0;
	 		val=buffer_ascii[index]-'0';
			val++;
	 		
	 		switch(index)
	 		{
				
				case 0:
					val%=4;
					val=val*16;
         		val+=buffer_ascii[1]-'0';
         		if(val>0x31) val=0x01;
         		//ecritI2C(ADR_HORLOGE,4,val);
         		break;
				case 1:	
					val%=10;
         		val+=(buffer_ascii[0]-'0')*16;
         		if(val>0x31) val=0x30;
         		//ecritI2C(ADR_HORLOGE,4,val);
					break;
				case 3:
					val%=2;
					val=val*16;
         		val+=buffer_ascii[4]-'0';
         		if(val>0x12) val=0x01;
         		//ecritI2C(ADR_HORLOGE,5,val);					
					break;
				case 4:
					val%=10;
         		val+=(buffer_ascii[3]-'0')*16;
         		if(val>0x12) val=0x10;
         		//ecritI2C(ADR_HORLOGE,5,val);					
					break;
				case 6:
					val%=10;
					val=val*16;
         		val+=buffer_ascii[7]-'0';
         		//ecritI2C(ADR_HORLOGE,6,val);										
					break;
				case 7:
					val%=10;
         		val+=(buffer_ascii[6]-'0')*16;
         		//ecritI2C(ADR_HORLOGE,6,val);										
					break;
				case 11:
					val%=3;
					val=val*16;
         		val+=buffer_ascii[12]-'0';
         		if(val>0x23) val=0x00;
         		//ecritI2C(ADR_HORLOGE,2,val);															
					break;
				case 12:
					val%=10;
         		val+=(buffer_ascii[11]-'0')*16;
         		if(val>0x23) val=0x20;
         		//ecritI2C(ADR_HORLOGE,2,val);												
					break;
				case 14:
					val%=6;
					val=val*16;
         		val+=buffer_ascii[15]-'0';
         		//ecritI2C(ADR_HORLOGE,1,val);													
					break;
				case 15:
					val%=10;
         		val+=(buffer_ascii[14]-'0')*16;
         		//ecritI2C(ADR_HORLOGE,1,val);																			
					break;
	 
		 	}
        	bip(volume_key,7,35);
		 }
		}
		else M_BP_G=1;
	
   	if(BP_D==0)
   	{
    	 if(M_BP_D!=0)
    	 {	
    		M_BP_D=0;
	 		switch(index)	
	 		{
				case 0:
					index++;
         		break;
				case 1:	
					index=3;
					break;	 	
				case 3:
					index++;
         		break;
				case 4:	
					index=6;
					break;		
				case 6:
					index++;
         		break;
				case 7:	
					index=11;
					break;
				case 11:
					index++;
         		break;
				case 12:	
					index=14;
					break;	
				case 14:
					index++;
         		break;
				case 15:	
					index=16;
				break;	
	 		}		
    	   bip(volume_key,7,35);
    	 }
		}
		else M_BP_D=1;

		DELAY
	}
}


void imprime_histo(void)
{


	unsigned short adresse;
   ERREUR=0;
   while (BP_D == 0) { DELAY };

	adresse=adresse_jour;

	data_printer(LF);
	
   imprime_entete();
	
	while((adresse!=adresse_jour_suiv)&&(ERREUR==0))
	{
    	read_enregistrement(&fiche,adresse);	
    	annulation_impression();
    	if(fiche.mesure.date!=0) imprime_fiche();
    	adresse=inc_fifo(adresse);
   }
   entete_ok = 0;
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	ERREUR=0;

}

void imprime_tableau(void)
{
   xdata char i;
   xdata char nb_total;
   ERREUR=0;
   nb_total=0;
   while(BP_D==0){ DELAY };

  	imprime_entete();
   data_printer(LF);
	imprime_colonne();
   annulation_impression();

   
	while((nb_total!=nb_mesure)&&(ERREUR==0))
	{
		annulation_impression();
    	for(i=0;i<16;i++) 
			fiche.index[i]=tableau_result[nb_total].index[i];
    	
    	imprime_fiche();
      nb_total++;
   }
   entete_ok = 0;
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
	data_printer(LF);
   ERREUR=0; 
}






void stat_histo(xdata char mode)
{

	xdata unsigned long aw_moy;
	xdata unsigned short aw_min;
	xdata unsigned short aw_max;

	xdata long tsurf_moy;
	xdata short tsurf_min;
	xdata short tsurf_max;

	xdata unsigned short nb_total;
	xdata unsigned short adresse;
	xdata char i;
	
	aw_moy=0;
	aw_max=0;
	aw_min=1000;
	tsurf_moy=0;
	tsurf_max=0;
	tsurf_min=32000;
	nb_total=0;
	adresse=adresse_jour;
	
	
	
	while(((adresse!=adresse_jour_suiv)&&(mode==1))||((nb_total!=nb_mesure)&&(mode==0)))
	{
		if(mode==0) 
		{

		 	for(i=0;i<16;i++) 
		 		fiche.index[i]=tableau_result[nb_total].index[i];
		}
		else 
		{
		 	read_enregistrement(&fiche,adresse);
			adresse=inc_fifo(adresse);
		}
		if(fiche.mesure.date!=VIDE)
		{
			nb_total++;
			aw_moy+=fiche.mesure.aw;
			if(fiche.mesure.aw > aw_max) aw_max=fiche.mesure.aw;
			if(fiche.mesure.aw < aw_min) aw_min=fiche.mesure.aw;
		
			tsurf_moy+=fiche.mesure.tsurf;
			if(fiche.mesure.tsurf > tsurf_max) tsurf_max=fiche.mesure.tsurf;
			if(fiche.mesure.tsurf < tsurf_min) tsurf_min=fiche.mesure.tsurf;
		}
	}

	cde_display(CLEAR);
	
	if(nb_total!=0)
	{
		aw_moy/=nb_total;
		tsurf_moy/=nb_total;
	}	
   else 
   {
   	affiche_titre(txt_message[(char)LANGUE][4],L3);	
   	return;
	}
	
	if(mode!=0)
	{
   	ptr_ascii=0;
      date_ascii(fiche.mesure.date);
      affiche_titre(buffer_ascii,L3);
   	
	}

	
	affiche_ligne(txt_stat[(char)LANGUE],L0);
	
	 
	affiche_ligne("Ts",L1);
	ptr_ascii=0;
	float_ascii(((float)tsurf_min)/100.0f+0.05f,1);
	affiche_ligne(buffer_ascii,L1+3);

	ptr_ascii=0;
	float_ascii(((float)tsurf_max)/100.0f+0.05f,1);
	affiche_ligne(buffer_ascii,L1+9);

	ptr_ascii=0;
	float_ascii(((float)tsurf_moy)/100.0f+0.05f,1);
	affiche_ligne(buffer_ascii,L1+15);

	affiche_ligne("Aw",L2);
	ptr_ascii=0;
	float_ascii(((float)aw_min)/1000.0f,3);
	affiche_ligne(buffer_ascii,L2+3);
	
	ptr_ascii=0;
	float_ascii(((float)aw_max)/1000.0f,3);
	affiche_ligne(buffer_ascii,L2+9);

	ptr_ascii=0;
	float_ascii(((float)aw_moy)/1000.0f,3);
	affiche_ligne(buffer_ascii,L2+15);
   
   while(BP_D==0){ DELAY }
   while(BP_D!=0){ tempo_cycle(); DELAY}

   cde_display(CLEAR);

}

void annulation_calibration(void)
{
   
  	delta_tmir=recup_float(I2C_TETE,DELTA_TMIR);  // on recupère les parametres "usine"
  	pente_tmir=recup_float(I2C_TETE,PENTE_TMIR);

   
   sauve_float(I2C_TETE,DELTA_TMIR_USER,delta_tmir);   
   sauve_float(I2C_TETE,PENTE_TMIR_USER,pente_tmir);	 	
   
  // init_display();
   //init_variables();

//   data_display(CLEAR);

   affiche_titre(txt_message[(char)LANGUE][18],L1);
   DELAY_DISPLAY;;
   affiche_titre(txt_message[(char)LANGUE][5],L2);
   DELAY_DISPLAY;;
   data_display(CLEAR);
}




char old_measure_en_course =0;
char pressed = 0;

void Start()
{
	static int tc = 0;
	float actualTemp;
	float setpointTemp;
	float deltaTemp;
	float tm;




// if(TXD3==0) set_boot_vector();
 
 if((BP_G==0)&&(BP_D==0)&&(DEM_AUTO!=0)) 
 {
	 log_info("update_soft");
	update_soft();
	bip_modul2();
	while (1) { DELAY };

 }
 
 reset_carte();

 if((BP_G!=0)&&(BP_D==0)) 
 {
	 log_info("annulation_calibration");
   annulation_calibration();
   bip_modul2();
 
 }
 
 
 if((BP_G==0)&&(BP_D!=0)) 
 {
	 log_info("Mode Essai");
   mode_essai = 1;
   affiche_titre("Mode Essai",L1);
   DELAY_DISPLAY
 	ptr_ascii = 0;
 	buffer_ascii[ptr_ascii] = CR;
 	ptr_ascii++;
 	buffer_ascii[ptr_ascii] = LF;
 	ptr_ascii++;
   txt_ascii(" Tsurf Ttete Tmir  Aw    Time  Delay");
 	buffer_ascii[ptr_ascii] = CR;
 	ptr_ascii++;
 	buffer_ascii[ptr_ascii] = LF;
 	ptr_ascii++;
 	buffer_ascii[ptr_ascii] = 0;
   txt_printer(buffer_ascii);
   
   
   bip_modul2();
   DELAY_DISPLAY;;
   data_display(CLEAR);
 
   
 }
 
 
 init_carte();
 
 EA=1;
 TR0=1;
 
 if(litI2C(I2C_TETE,OPTION)>='D') rech_fin_enregistrement();


// read_histo(0);
 //sim_histo();






 /*log_info("starting loop");
 printf("DISPLAY=%d\n", DISPLAY);
 printf("TABLEAU=%d\n", TABLEAU);
 printf("MENU=%d\n", MENU);*/

 unsigned int btnmenu = BP_G;
 int btnok = BP_D;
 int btnmag = DEM_AUTO;

 // settings related to Peltier Board control by IR temp sensor
 tc = TSET_COUNTS - 1;
 // set the desired temperature at surface (defined in vardef.h)
 setpointTemp = (float)TSETPOINT;
 // preset previous deltaT
 g_prev_deltaTemp = 0.0;

 // enable the the Peltier board's output stage
 Mod_MeCon_EnableOutputStage(0);

 do
 {
 	EA=1;
 	TR0=1;

	transfert_pc();

	/*butons*/
	if (btnmenu != BP_G) {
		btnmenu = BP_G;
		printf("INF: button menu [%d]\n", btnmenu);
	}
	if (btnok != BP_D) {
		btnok = BP_D;
		printf("INF: button ok [%d]\n", btnok);
	}
	if (btnmag != DEM_AUTO) {
		btnmag = DEM_AUTO;
		printf("INF: button mag [%d]\n", btnmag);
	}

#ifdef OLD_MODE_LCD
	if (ws_clients_connected() == 0)
	gestion_bp(); // scan the pushbuttons
#endif OLD_MODE_LCD

	gestion_demarrage_auto();   // scan du contacteur reed

#ifdef OLD_MODE_LCD
	//if (ws_clients_connected() == 0) {
		if (DISPLAY != 0)
		{
			affiche_mesure();
			if (ws_clients_connected() == 0) {
				if ((BP_G == 0) && (TABLEAU == 0)) gestion_menu();
			}
		}
	//}
#endif

	// **** code to manage peltier temperature setting by IR sensor readings ****
		
		tc++;
		// a novel SetTemp command is issued to the Peltier Board every TSET_COUNTS iterations
		// in order to avoid overworking the temperature controller (defined in vardef.h)
		if (tc >= TSET_COUNTS)
		{
			tc = 0;

			// get actual tsurf reading
			acqui_surface();		// this function needs to be called only if the GUI does not call it routinely
			actualTemp = tsurf;
			// compute the delta between desired and actual temperatures
			deltaTemp = setpointTemp - actualTemp;
			// round to 0.1°C
			tm = roundf(deltaTemp * 10) / 10;
			deltaTemp = tm;
			// store the delta value in a global var for debugging via web.c
			g_deltaTemp = deltaTemp;
			// if deltaT varied set the novel target temperature to the TEC-1091 board
			if (deltaTemp != g_prev_deltaTemp)
			{
				// update previous reading
				g_prev_deltaTemp = deltaTemp;
				// force the Peltier board to set the desired temperature plus measured delta
				//MeCom_SetTemp(0, setpointTemp + deltaTemp);
				Mod_MeCom_SetTemp(0, setpointTemp + deltaTemp);
				// print info for debug purposes
//				printf("\nraw data acquired: %.1f mV", g_readV*1000);
//				printf("\nsetpointT: %.1f °C\tactualT: %.1f °C\tdeltaTemp: %.1f\tTEC-1091 temp set to: %.1f °C", setpointTemp, actualTemp, deltaTemp, setpointTemp + deltaTemp);
			}
		}
		
	// **** end of code to manage peltier temperature setting by IR sensor readings ****

   DELAY

 } while(1);

#ifdef DOLOG
   if (fl != NULL) fclose(fl);
#endif

}   
      	

uint8_t Mod_MeCom_SetTemp(uint8_t Address, float temp) 
{
	uint8_t result = 0;
	MeParFloatFields temperature;
	MeParLongFields start;
	MeParFloatFields Fields;
	int32_t Inst = 1;

	if (temp < -10) temp = -10;
	if (temp > 60) temp = 60;

	if (MeCom_TEC_Tem_TargetObjectTemp(Address, Inst, &Fields, MeGetLimits))
	{	
		Fields.Value = temp;
	
		if (MeCom_TEC_Tem_TargetObjectTemp(Address, Inst, &Fields, MeSet))
		{
			printf("\nTEC Object Temperature: New Value: %f\n", Fields.Value);
		}
	}
}


void Mod_MeCon_EnableOutputStage(uint8_t Address)
{
	MeParFloatFields Fields;
	int32_t Inst = 1;

	if (MeCom_TEC_Ope_OutputStageEnable(Address, Inst, &Fields, MeGetLimits))
	{
		Fields.Value = 1;		
		if (MeCom_TEC_Ope_OutputStageEnable(Address, Inst, &Fields, MeSet))
		{			
			printf("\nTEC Output Enable Status: New Value: %d\n", Fields.Value);
		}
	}
}

