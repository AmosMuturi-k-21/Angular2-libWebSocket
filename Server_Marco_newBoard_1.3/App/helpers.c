#include "vardef.h"

void recup_config(void)    // cette fonction permet de charger la variable config en fonction des donnees de la tete
{
	//r_config = 0;
	SET_CONFIG(0);
	if (CINETIQUE == 0) {
		SET_CONFIG(/*litI2C(I2C_TETE, MODE_MESURE)*/ opt.MEASURE_MODE);
	}
	else {
		SET_CONFIG(/*litI2C(I2C_TETE, MODE_CINETIQUE)*/ opt.KINETIC_MODE);
	}

	printf("MEASURE_MODE = [%d]\n", opt.MEASURE_MODE);
	printf("KINETIC_MODE = [%d]\n", opt.KINETIC_MODE);
	printf("GET_CONFIG = [%d]\n", GET_CONFIG);

	LANGUE = litI2C(I2C_TETE, ADR_LANGUE);

	switch (litI2C(I2C_TETE, OPTION))
	{
	case 'A':
	case 'B':
		OPTION_H = 0;
		OPTION_L = 0;
		break;
	case 'C':
		OPTION_H = 1;
		OPTION_L = 0;
		break;
	case 'D':
	case 'E':
		OPTION_H = 1;
		OPTION_L = 1;
		break;
	}
	type_capteur = litI2C(I2C_TETE, ADR_TYPE_C);

	if (type_capteur == 'B') TYPE_C = 1;
	else TYPE_C = 0;

	printf("GET_CONFIG = [%d]\n", GET_CONFIG);
}

void write_enregistrement(union fiche * ptr, unsigned short adrmem)
{
	char i;
	union short_sp adr;
	char checksum;

	EA = 0;

	adr.us = adrmem;
	adr.us *= 16;


	startI2C();


	envoiI2C(ADR_MEM);
	slave_ack();

	if (BUSY == 0)
	{
		envoiI2C(adr.c[0]);
		slave_ack();
	}
	if (BUSY == 0)
	{
		envoiI2C(adr.c[1]);
		slave_ack();
	}

	checksum = 0;
	for (i = 0; i<16; i++)
	{
		if (i != 10) checksum += ptr->index[i];
	}

	for (i = 0; i<16; i++)
	{
		if (BUSY == 0)
		{
			if (i != 10) envoiI2C(ptr->index[i]);
			else envoiI2C(checksum);

			slave_ack();

		}
	}

	stopI2C();

	EA = 1;
	tempo_ecriture();
}

unsigned short inc_fifo(unsigned short pointeur)
{
	pointeur++;
	if (pointeur >= NB_FICHE) pointeur = 0;
	return(pointeur);

}
unsigned short dec_fifo(unsigned short pointeur)
{
	if (pointeur != 0) pointeur--;
	else pointeur = NB_FICHE - 1;
	return(pointeur);

}

void write_histo(void)
{
	write_enregistrement(&fiche, adresse_courante);
	adresse_courante = inc_fifo(adresse_courante);

}

void imprime_entete(void)
{
	txt_printer("GBX Fast Lab");
	data_printer(CR);
	data_printer(LF);
	annulation_impression();
	txt_printer(txt_message[(char)LANGUE][13]);
	txt_printer(identifiant);
	data_printer(CR);
	data_printer(LF);
	annulation_impression();
	txt_printer(txt_message[(char)LANGUE][14]);
	ptr_ascii = 0;
	date_ascii(fiche.mesure.date);
	txt_printer(buffer_ascii);
	txt_printer(" - ");
	ptr_ascii = 0;
	time_ascii(fiche.mesure.date);
	txt_printer(buffer_ascii);
	data_printer(CR);
	data_printer(LF);


}

void auto_print(void)
{
	ptr_ascii = 0;
	txt_ascii(txt_message[(char)LANGUE][19]);
	if ((fiche.mesure.etat & 0x02) == 0)
	{
		if ((fiche.mesure.etat & 0x01) != 0) txt_ascii(txt_message[(char)LANGUE][11]);
		else
			txt_ascii(txt_choix[(char)LANGUE][TAB_MODE_MES][fiche.mesure.config & 0x03]);
	}
	else
	{
		txt_ascii(txt_message[(char)LANGUE][10]);
	}

	txt_printer(buffer_ascii);
	data_printer(CR);
	data_printer(LF);


	data_printer(CR);

	ptr_ascii = 0;
	txt_ascii(txt_number[(char)LANGUE]);
	if ((fiche.mesure.etat & 0x04) == 0)
		ushort_ascii(fiche.mesure.numero);
	else
		txt_ascii(txt_message[(char)LANGUE][12]);

	txt_printer(buffer_ascii);
	data_printer(CR);
	data_printer(LF);

	ptr_ascii = 0;
	txt_ascii(txt_delay[(char)LANGUE]);
	delay_ascii(fiche.mesure.duree);
	txt_printer(buffer_ascii);
	data_printer(CR);
	data_printer(LF);



	ptr_ascii = 0;
	txt_printer("T[C] : ");
	float_ascii((float)fiche.mesure.tsurf / 100.0f + 0.05f, 1);
	txt_printer(buffer_ascii);
	data_printer(CR);
	data_printer(LF);

	ptr_ascii = 0;
	txt_printer("AW   : ");
	if ((fiche.mesure.etat & 0x04) == 0)
		float_ascii((float)fiche.mesure.aw / 1000.0f, 3);
	else
		float_ascii((float)fiche.mesure.numero / 1000.0f, 3);
	txt_printer(buffer_ascii);
	data_printer(CR);
	data_printer(LF);
}

void data_printer(char carac) // send a character on the serial link (intended for the printer) with time delay
{
	if (ERREUR != 0) return;
	printf("%c", carac);
}

void imprime_colonne(void)
{
	data_printer(CR);
	txt_printer(txt_print[(char)LANGUE]);
	data_printer(CR);
	data_printer(LF);
}
//@TODO check
void annulation_impression(void)
{
	if (BP_D == 0)
	{
		affiche_titre(txt_message[(char)LANGUE][8], L3);
		bip(volume_key, 14, 50);
		bip(0, 100, 100);
		ERREUR = 1;
		DELAY_DISPLAY
	}
}

void txt_printer(char * texte)
{
	xdata char i;
	i = 0;

	while (texte[i] != 0)
	{
		data_printer(texte[i]);
		i++;
	}

}

void imprime_fiche(void)
{
	data_printer(CR);
	for (ptr_ascii = 0; ptr_ascii<40; ptr_ascii++) buffer_ascii[ptr_ascii] = ' ';

	ptr_ascii = 0;

	if ((fiche.mesure.etat & 0x04) == 0)
		ushort_ascii(fiche.mesure.numero);
	else
		txt_ascii(txt_message[(char)LANGUE][12]);

	buffer_ascii[ptr_ascii] = ' ';
	ptr_ascii = 5;

	if ((fiche.mesure.etat & 0x04) == 0)
		float_ascii((float)fiche.mesure.aw / 1000.0f, 3);
	else
		float_ascii((float)fiche.mesure.numero / 1000.0f, 3);

	buffer_ascii[ptr_ascii] = ' ';
	ptr_ascii = 12;
	float_ascii((float)fiche.mesure.tsurf / 100.0f + 0.05f, 1);

	buffer_ascii[ptr_ascii] = ' ';
	ptr_ascii = 18;
	time_ascii(fiche.mesure.date);

	buffer_ascii[ptr_ascii] = ' ';
	ptr_ascii = 25;
	delay_ascii(fiche.mesure.duree);


	buffer_ascii[ptr_ascii] = ' ';
	ptr_ascii = 32;

	if ((fiche.mesure.etat & 0x02) == 0)
	{
		if ((fiche.mesure.etat & 0x01) != 0) txt_ascii(txt_message[(char)LANGUE][11]);
		else
			txt_ascii(txt_choix[(char)LANGUE][TAB_MODE_MES][fiche.mesure.config & 0x03]);
	}
	else
	{
		txt_ascii(txt_message[(char)LANGUE][10]);
	}
	buffer_ascii[40] = 0;
	txt_printer(buffer_ascii);
	data_printer(CR);
	data_printer(LF);


}

void arret_utilisateur(void)
{
	xdata char message_arret;

	if (MESURE_EN_COURS != 0)
	{
		log_info("aborting measure request");
		CYCLE_INT = 1;
		MESURE_EN_COURS = 0;
		num_pt_calib = 0;
		message = 0;
		init_etape_attente();

		//@UI
		ui_event(ui_event_measure_aborted);
		bip(volume_key, 14, 50);

		if (DISPLAY != 0)
		{
			cde_display(CLEAR);
			if (commande != ARRET)
			{
				message_arret = 0;
				if (CINETIQUE != 0) message_arret = 1;
				if (CALIB != 0) message_arret = 2;
				affiche_titre(txt_arret[(char)LANGUE][message], L1);
				bip(0, 100, 100);
				DELAY_DISPLAY
				cde_display(CLEAR);
			}
		}
	}
	if (commande == ARRET) ECHO_CDE = 1;
}

void mesure_std(void)    // lancement d'une mesure simple
{
	printf("mesure_std started\n");
	CYCLE_INT = 1;

	if (INC_NUM != 0) numero_mesure++;
	INC_NUM = 0;
	
	ERREUR = 0;

	message = 0;
	time_mesure = 0;
	time_mesure_fl = 0.0;
	CALIB = 0;
	MESURE_EN_COURS = 1;
	CINETIQUE = 0;
	aw_finale = 0;
	aw_flash = 0;
	bip(volume_key, 10, 35);

	init_etape_attente();
	pointage_flash = litI2C(I2C_TETE, POINTAGE_FLASH);
	if (commande == MESURE_ON) ECHO_CDE = 1;

	//@UI
	ui_event(ui_event_measure_started);
}

void cinetique_std(void)      // lancement d'une cinetique
{
	printf("cinetique_std started\n");
	if (litI2C(I2C_TETE, OPTION)<'C')
	{
		erreur_cde();
	}
	else
	{
		CYCLE_INT = 1;

		if (INC_NUM != 0) numero_mesure++;
		INC_NUM = 0;

		ERREUR = 0;

		message = 0;
		time_mesure = 0;
		time_mesure_fl = 0.0;
		CALIB = 0;
		MESURE_EN_COURS = 1;
		CINETIQUE = 1;
		aw_finale = 0;
		aw_flash = 0;
		bip(volume_key, 10, 35);
		bip(0, 10, 35);
		bip(volume_key, 10, 35);

		init_etape_attente();

		if (commande == CINETIQUE_ON) ECHO_CDE = 1;

		//@UI
		ui_event(ui_event_measure_started);
	}

}

void recup_donnee_tete(void)  // retrieval of the data from the head if not initialized it is initialized
{
	xdata char temp;

	if (recup_short(I2C_TETE, ADR_VERSION) >= 350)
	{
		log_info("ADR_VERSION >= 350");
		pointage_flash = litI2C(I2C_TETE, POINTAGE_FLASH);
		gain_p = litI2C(I2C_TETE, GAIN_P);
		gain_i = litI2C(I2C_TETE, GAIN_I);
		volume_key = litI2C(I2C_TETE, VOLUME_KEY);

		consigne_regu = recup_float(I2C_TETE, CONSIGNE_REGU);
		consigne_rampe = recup_float(I2C_TETE, CONSIGNE_RAMPE);
		regu_gain_P = recup_short(I2C_TETE, REGU_GAIN_P);
		regu_gain_I = recup_short(I2C_TETE, REGU_GAIN_I);


		delta_ttete = recup_float(I2C_TETE, DELTA_TTETE);
		pente_ttete = recup_float(I2C_TETE, PENTE_TTETE);

		pente_correction_tsurf = recup_float(I2C_TETE, PENTE_CORR_TSURF);


		REGU_COUPELLE = litI2C(I2C_TETE, REGU_ON_OFF);


	}
	else // update detectee on reinitialize some parameters
	{
		if (BUSY == 0) {
			affiche_titre(txt_message[(char)LANGUE][0], L3);
			DELAY_DISPLAY
		}
		log_info("re-init config");


		sauve_short(I2C_TETE, REGU_GAIN_P, regu_gain_P);
		sauve_short(I2C_TETE, REGU_GAIN_I, regu_gain_I);
		consigne_regu = 20.0f;
		sauve_float(I2C_TETE, CONSIGNE_REGU, consigne_regu);   // sinon on l'initialise
		consigne_rampe = 10.0f;
		sauve_float(I2C_TETE, CONSIGNE_RAMPE, consigne_rampe);
		ecritI2C(I2C_TETE, SEL_CONSIGNE_REGU, 1);

		ecritI2C(I2C_TETE, POINTAGE_FLASH, pointage_flash);

		ecritI2C(I2C_TETE, VIT_MOTEUR, val_choix[TAB_MOTEUR][0]);
		ecritI2C(I2C_TETE, MODE_MESURE, val_choix[TAB_MODE_MES][1]);
		ecritI2C(I2C_TETE, MODE_CINETIQUE, val_choix[TAB_MODE_CINET][0]);
		ecritI2C(I2C_TETE, MODE_DEMARRAGE, val_choix[TAB_DEM_AUTO][0]);
		ecritI2C(I2C_TETE, AUTO_PRINT, 0);
		ecritI2C(I2C_TETE, VOLUME_BIP, val_choix[TAB_VOL_MES][2]);
		ecritI2C(I2C_TETE, VOLUME_KEY, val_choix[TAB_VOL_KEY][2]);
		volume_key = val_choix[TAB_VOL_KEY][2];

		temp = litI2C(I2C_TETE, ADR_LANGUE);
		if ((temp<'0') || (temp>'1'))
			ecritI2C(I2C_TETE, ADR_LANGUE, 1);

		temp = litI2C(I2C_TETE, ADR_TYPE_C);
		if ((temp<'A') || (temp>'D'))
			ecritI2C(I2C_TETE, ADR_TYPE_C, 'A');

		temp = litI2C(I2C_TETE, OPTION);
		if ((temp<'A') || (temp>'E'))
			ecritI2C(I2C_TETE, OPTION, 'C');


		sauve_float(I2C_TETE, DELTA_TTETE, 0.0f/*delta_tsurf*/);
		sauve_float(I2C_TETE, PENTE_TTETE, 1.0f/*pente_tsurf*/);
		sauve_float(I2C_TETE, PENTE_CORR_TSURF, 1.0f/*pente_tsurf*/);


		ecritI2C(I2C_TETE, REGU_ON_OFF, 0);


	}
	recup_config();

	if (BUSY != 0) {
		affiche_titre(txt_message[(char)LANGUE][1], L3);
		DELAY_DISPLAY
	}
	




	if (recup_short(I2C_TETE, CODE_INIT) == 0x3930) // if the head has already been initialized we retrieve the data 
	{
		log_info("head is ready");

		/*delta_tmir = recup_float(I2C_TETE, DELTA_TMIR);
		pente_tmir = recup_float(I2C_TETE, PENTE_TMIR);*/

		delta_tmir = opt.DELTA_TEMP_MIRROR;
		pente_tmir = opt.SLOPE_TEMP_MIRROR;

		delta_tsurf = recup_float(I2C_TETE, DELTA_TSURF);
		pente_tsurf = recup_float(I2C_TETE, PENTE_TSURF);

		dseuil = (char)(recup_float(I2C_TETE, DSEUIL)*1000.0f);    // dseuil en flottant pour compatibilite avec FAST/1 converti en char  
		gain_p = litI2C(I2C_TETE, GAIN_P);
		gain_i = litI2C(I2C_TETE, GAIN_I);

	}
	else
	{
		log_info("init head");
		sauve_float(I2C_TETE, DELTA_TMIR, 0.0f/*delta_tmir*/); // otherwise it is initialized
		sauve_float(I2C_TETE, PENTE_TMIR, 1.0f/*pente_tmir*/);
		sauve_float(I2C_TETE, DELTA_TSURF, 0.0f/*delta_tsurf*/);
		sauve_float(I2C_TETE, PENTE_TSURF, 1.0f/*pente_tsurf*/);

		sauve_float(I2C_TETE, DELTA_TTETE, 0.0f/*delta_tsurf*/);
		sauve_float(I2C_TETE, PENTE_TTETE, 1.0f/*pente_tsurf*/);

		sauve_float(I2C_TETE, DSEUIL, 0.01f/*(float)(dseuil)/1000.0*/);
		ecritI2C(I2C_TETE, GAIN_P, gain_p);
		ecritI2C(I2C_TETE, GAIN_I, gain_i);
		sauve_txt(I2C_TETE, ADR_IDENTIFIANT, "0000000");
		sauve_short(I2C_TETE, CODE_INIT, 0x3930);

	}
	recup_txt(I2C_TETE, ADR_IDENTIFIANT, identifiant);


	if (recup_short(I2C_TETE, ADR_VERSION) >= 360)
	{
		log_info("ADR_VERSION >= 360");
		mode_calibration = /*litI2C(I2C_TETE, MODE_CALIB);*/ opt.CALIBRATION_MODE;
		/*delta_tmir = recup_float(I2C_TETE, DELTA_TMIR_USER);
		pente_tmir = recup_float(I2C_TETE, PENTE_TMIR_USER);*/
		delta_tmir = opt.DELTA_TEMP_MIRROR;
		pente_tmir = opt.SLOPE_TEMP_MIRROR;
	}
	else
	{
		ecritI2C(I2C_TETE, MODE_CALIB, 0);
		sauve_float(I2C_TETE, DELTA_TMIR_USER, delta_tmir);   // sinon on l'initialise
		sauve_float(I2C_TETE, PENTE_TMIR_USER, pente_tmir);

	}

	if (recup_short(I2C_TETE, ADR_VERSION) != VERSION)	sauve_short(I2C_TETE, ADR_VERSION, VERSION);

	printf("delta_tmir = %f\n", delta_tmir);
	printf("pente_tmir = %f\n", pente_tmir);
	printf("delta_tsurf = %f\n", delta_tsurf);
	printf("pente_tsurf = %f\n", pente_tsurf);

}

//@TODO check
void init_variables(void)
{

	/*ptr.uc = ETAT;
	*ptr.uc = 0;
	ptr.uc = CONFIG;
	*ptr.uc = 0;
	ptr.uc = CONTROL_0;
	*ptr.uc = 0;
	ptr.uc = CONTROL_1;
	*ptr.uc = 0;
	ptr.uc = INTERFACE;
	*ptr.uc = 0;
	ptr.uc = OPTIMIZE;
	*ptr.uc = 0;*/

	INC_NUM = 1;


	efface_tableau();
	recup_donnee_tete();
	init_etape_attente();


}

void update_soft(void)
{
	//set_boot_vector();
	init_display();
	init_variables();
	// 	data_display(CLEAR);

	affiche_titre(txt_message[(char)LANGUE][6], L1);
	affiche_titre(txt_message[(char)LANGUE][7], L2);
	while (1) { delay(100); }
}

void calib_auto_std(void)        // lancement d'une calibration
{
	if (mode_calibration < 128) {
		log_info("calibration mode: 1p");
	}else
		log_info("calibration mode: 2p");

	if (num_pt_calib == 0)
		log_info("1 point calib starting");

	if (num_pt_calib == 1)
		log_info("2 point calib starting");

	if (num_pt_calib > 1)
		log_info("BUG: 2+ point calib starting");

	CYCLE_INT = 1;
	ERREUR = 0;
	time_mesure = 0;
	time_mesure_fl = 0.0;
	CALIB = 1;
	CINETIQUE = 0;
	MESURE_EN_COURS = 1;
	aw_finale = 0;
	aw_flash = 0;
	bip_modul();
	init_etape_attente();
	if (commande == CALIB_AUTO_ON) ECHO_CDE = 1;

	//@UI
	ui_event(ui_event_calibration_started);

}

void bip_modul(void)
{
	bip(volume_key, 14, 8);
	bip(volume_key, 10, 10);
	bip(volume_key, 14, 8);
	bip(volume_key, 10, 10);
	bip(volume_key, 14, 8);
	bip(volume_key, 10, 10);
}

void bip_modul4(void)
{
	bip(volume_key, 15, 12);
	bip(volume_key, 13, 16);
	bip(volume_key, 20, 10);
	bip(volume_key, 26, 8);
}


void bip_modul2(void)
{
	bip(volume_key, 20, 10);
	bip(volume_key, 15, 15);
	bip(volume_key, 13, 20);
}

void bip_modul3(void)
{
	bip(volume_key, 20, 15);
	bip(volume_key, 15, 20);
	bip(volume_key, 20, 15);
}

void set_char(void)              // gestion des parametres de commande type char 8 bits
{

	if (TRANS_PARAM == 0)
	{
		TRANS_PARAM = 1;
		return;
	}
	buffer_rx.uc[0] = SBUF;	// recuperation du parametre
	RI = 0;
	switch (commande)
	{
	case SET_GAIN_P:
		gain_p = buffer_rx.uc[0];
		ecritI2C(I2C_TETE, GAIN_P, gain_p);
		break;
	case SET_GAIN_I:
		gain_i = buffer_rx.uc[0];
		ecritI2C(I2C_TETE, GAIN_I, gain_i);
		break;
	case SET_SEUIL_DETECTION:
		dseuil = buffer_rx.uc[0];
		sauve_float(I2C_TETE, DSEUIL, (float)dseuil / 1000.0);
		break;
	case SET_VIT_MOTEUR:
		if ((buffer_rx.uc[0] >= 1) && (buffer_rx.uc[0] <= 3)) ecritI2C(I2C_TETE, VIT_MOTEUR, val_choix[TAB_MOTEUR][3 - buffer_rx.uc[0]]);
		else ecritI2C(I2C_TETE, VIT_MOTEUR, buffer_rx.uc[0]);
		if (etape == 0) time_etape = 0;
		moteur_on();
		break;
	case SET_MODE_MESURE:
		ecritI2C(I2C_TETE, MODE_MESURE, buffer_rx.uc[0]);
		break;
	case SET_MODE_CINETIQUE:
		ecritI2C(I2C_TETE, MODE_CINETIQUE, buffer_rx.uc[0]);
		break;
	case SET_MODE_DEMARRAGE:
		ecritI2C(I2C_TETE, MODE_DEMARRAGE, buffer_rx.uc[0]);
		break;
	case SET_VOLUME_BIP:
		ecritI2C(I2C_TETE, VOLUME_BIP, val_choix[TAB_VOL_MES][buffer_rx.uc[0]]);
		break;
	case SET_VOLUME_KEY:
		ecritI2C(I2C_TETE, VOLUME_KEY, val_choix[TAB_VOL_KEY][buffer_rx.uc[0]]);
		volume_key = val_choix[TAB_VOL_KEY][buffer_rx.uc[0]];
		break;
	case SET_LANGUE:
		ecritI2C(I2C_TETE, ADR_LANGUE, buffer_rx.uc[0]);

		break;
	case SET_OPTION:
		if (CODE_OK != 0)
		{
			ecritI2C(I2C_TETE, OPTION, buffer_rx.uc[0]);
			bip_modul2();
		}
		else
		{
			bip(volume_key, 40, 50);
			ERREUR = 1;
		}
		break;
	case SET_TYPE_C:
		ecritI2C(I2C_TETE, ADR_TYPE_C, buffer_rx.uc[0]);
		type_capteur = buffer_rx.uc[0];
		if (type_capteur == 'B') TYPE_C = 1;
		else TYPE_C = 0;

	case SET_MODE_CALIB:
		mode_calibration = buffer_rx.uc[0];
		ecritI2C(I2C_TETE, MODE_CALIB, mode_calibration);
		break;


	}

	recup_config();

	TRANS_PARAM = 0;
	ECHO_CDE = 1;
}

void transfert_off(void)// stop transferring data to the PC
{
	TRANSFERT = 0;
	ECHO_CDE = 1;
}
void transfert_on(void) // launch data transfer
{
	TRANSFERT = 1;
	ECHO_CDE = 1;
}

void set_short(void) // gestion des parametres de commande type short 16 bits
{

	if (TRANS_PARAM == 0)
	{
		TRANS_PARAM = 1;
		return;
	}
	buffer_rx.uc[etape_communication] = SBUF;
	RI = 0;
	etape_communication++;
	if (etape_communication >= 2)
	{
		switch (commande)
		{
		case SET_OFFSET_MIROIR:
			delta_tmir = (float)buffer_rx.s[0] / 100.0f;
			sauve_float(I2C_TETE, DELTA_TMIR, delta_tmir);
			sauve_float(I2C_TETE, DELTA_TMIR_USER, delta_tmir);   // sinon on l'initialise
			break;
		case SET_PENTE_MIROIR:
			if (regu_setting != 0)
			{
				pente_correction_tsurf = (float)buffer_rx.s[0] / 1000.0f;
				sauve_float(I2C_TETE, PENTE_CORR_TSURF, pente_correction_tsurf);
			}
			else
			{
				pente_tmir = (float)buffer_rx.s[0] / 1000.0f;
				sauve_float(I2C_TETE, PENTE_TMIR, pente_tmir);
				sauve_float(I2C_TETE, PENTE_TMIR_USER, pente_tmir);
			}


			break;
		case SET_OFFSET_SURFACE:
			if ((type_capteur == 'B') || (regu_setting != 0))
			{
				delta_ttete = (float)buffer_rx.s[0] / 100.0f;
				sauve_float(I2C_TETE, DELTA_TTETE, delta_ttete);
			}
			else
			{
				delta_tsurf = (float)buffer_rx.s[0] / 100.0f;
				sauve_float(I2C_TETE, DELTA_TSURF, delta_tsurf);
			}

			break;
		case SET_PENTE_SURFACE:
			if ((type_capteur == 'B') || (regu_setting != 0))
			{
				pente_ttete = (float)buffer_rx.s[0] / 1000.0f;
				sauve_float(I2C_TETE, PENTE_TTETE, pente_ttete);
			}
			else
			{
				pente_tsurf = (float)buffer_rx.s[0] / 1000.0f;
				sauve_float(I2C_TETE, PENTE_TSURF, pente_tsurf);
			}

			break;
		case SET_CODE_ACCES:
			etape_communication = 0;
			while (((identifiant[etape_communication]<'0') || (identifiant[etape_communication]>'9')) && (etape_communication<10))
			{
				etape_communication++;
			}
			buffer_rx.s[0] -= 26100;

			if (etape_communication <= 10)
			{
				etape_communication++;

				if ((identifiant[etape_communication] >= '0') && (identifiant[etape_communication] <= '9')) buffer_rx.s[0] += (identifiant[etape_communication] - '0') * 1000;
				etape_communication++;
				if ((identifiant[etape_communication] >= '0') && (identifiant[etape_communication] <= '9')) buffer_rx.s[0] += (identifiant[etape_communication] - '0') * 1100;
				etape_communication++;
				if ((identifiant[etape_communication] >= '0') && (identifiant[etape_communication] <= '9')) buffer_rx.s[0] += (identifiant[etape_communication] - '0') * 110;
				etape_communication++;
				if ((identifiant[etape_communication] >= '0') && (identifiant[etape_communication] <= '9')) buffer_rx.s[0] += (identifiant[etape_communication] - '0') * 11;
				etape_communication++;
				if ((identifiant[etape_communication] >= '0') && (identifiant[etape_communication] <= '9')) buffer_rx.s[0] += (identifiant[etape_communication] - '0');
			}

			if (buffer_rx.s[0] == 0)
			{
				CODE_OK = 1;
			}

			if (CODE_OK == 0)
			{

				bip(volume_key, 40, 50);
				ERREUR = 1;
			}
			else
			{
				bip_modul3();
				ERREUR = 0;
			}
			break;

		case SET_CONSIGNE_REGU:
			consigne_regu = (float)buffer_rx.s[0] / 100.0f;
			sauve_float(I2C_TETE, CONSIGNE_REGU, consigne_regu);
			break;
		case SET_RAMPE_REGU:
			consigne_rampe = (float)buffer_rx.s[0] / 100.0f;
			sauve_float(I2C_TETE, CONSIGNE_RAMPE, consigne_rampe);
			break;
		case SET_REGU_GAIN_P:
			regu_gain_P = buffer_rx.s[0];
			sauve_short(I2C_TETE, REGU_GAIN_P, regu_gain_P);
			break;
		case SET_REGU_GAIN_I:
			regu_gain_I = buffer_rx.s[0];
			sauve_short(I2C_TETE, REGU_GAIN_I, regu_gain_I);
			break;

		}
		etape_communication = 0;
		TRANS_PARAM = 0;
		ECHO_CDE = 1;
	}
}

void affiche_date_courante(void)
{
	date_courante = recup_date();
	ptr_ascii = 0;
	date_ascii(date_courante);
	txt_ascii("   ");
	time_ascii(date_courante);
	affiche_titre(buffer_ascii, L3);
}

void set_long(void)  // gestion des parametres de commande type long 32 bits
{


	unsigned short nb_essai;

	if (TRANS_PARAM == 0)
	{
		TRANS_PARAM = 1;
		return;
	}
	buffer_rx.uc[etape_communication] = SBUF;
	RI = 0;
	etape_communication++;
	if (etape_communication >= 4)
	{

		switch (commande)
		{
		case SET_DATE:
			set_date(buffer_rx.ul);

			affiche_date_courante();

			bip_modul2();
			bip(0, 100, 100);

			break;
		case SET_DATE_DEBUT:
			for (nb_essai = NB_FICHE, adresse_jour = adresse_courante; (TRANS_PARAM != 0) && (nb_essai != 0); nb_essai--)
			{
				adresse_jour = read_enregistrement_prec(&fiche, adresse_jour);
				if ((fiche.mesure.date < buffer_rx.ul) || (adresse_jour == adresse_courante)) TRANS_PARAM = 0;
			}
			if (adresse_jour != adresse_courante) adresse_jour = read_enregistrement_suiv(&fiche, adresse_jour);
			else
			{
				adresse_jour = 0;
				read_enregistrement(&fiche, adresse_jour);
			}
			break;
		case SET_DATE_FIN:
			for (nb_essai = NB_FICHE, adresse_jour_suiv = adresse_courante; (TRANS_PARAM != 0) && (nb_essai != 0); nb_essai--)
			{
				adresse_jour_suiv = read_enregistrement_prec(&fiche, adresse_jour_suiv);
				if (fiche.mesure.date <= buffer_rx.ul) TRANS_PARAM = 0;
			}
			adresse_jour_suiv = read_enregistrement_suiv(&fiche, adresse_jour_suiv);

			break;
		}

		etape_communication = 0;
		TRANS_PARAM = 0;
		ECHO_CDE = 1;

	}

}


void set_identifiant(void)    // gestion du parametre de type chaine de caracteres de la commande set_identifiant 
{
	if (TRANS_PARAM == 0)
	{
		TRANS_PARAM = 1;
		return;
	}
	identifiant[etape_communication] = SBUF;
	RI = 0;

	if (identifiant[etape_communication] == 0)
	{
		sauve_txt(I2C_TETE, ADR_IDENTIFIANT, identifiant);
		etape_communication = 0;
		bip_modul3();
		TRANS_PARAM = 0;
		ECHO_CDE = 1;
	}
	else etape_communication++;
}

void transfert_histo(void)
{
	xdata unsigned short adresse;

	adresse = adresse_jour;

	while (adresse != adresse_jour_suiv)
	{
		read_enregistrement(&fiche, adresse);
		if (fiche.mesure.date != VIDE) trame_FI(adresse);
		adresse = inc_fifo(adresse);
	}
	if (commande == TRANSFERT_HISTORIQUE) ECHO_CDE = 1;
}

void efface_histo(void)
{

	unsigned short scan;


	for (scan = 0; scan<16; scan++)
	{
		fiche.index[scan] = 0x00;

	}


	for (scan = 0; scan < NB_FICHE; scan++)
	{

		cde_display(L3);
		txt_display(txt_message[(char)LANGUE][9]);
		ptr_ascii = 0;
		ushort_ascii(scan);
		txt_display(buffer_ascii);


		write_enregistrement(&fiche, scan);

	}
	efface_ligne(L3);

	adresse_courante = 0;
	ECHO_CDE = 1;
}

void sim_histo(void)
{
	unsigned long duree;
	unsigned short aw;
	short tsurf;
	unsigned short fiche_adr;
	char numero;
	fiche_adr = 0;
	numero = 1;
	date_courante = 180176000;
	aw = 100;
	tsurf = 2000;
	duree = 600;


	do
	{

		fiche.mesure.tsurf = tsurf;
		fiche.mesure.date = date_courante;
		fiche.mesure.aw = aw;
		fiche.mesure.duree = duree;
		fiche.mesure.etat = 0;
		fiche.mesure.config = 0;

		fiche.mesure.numero = numero;


		write_enregistrement(&fiche, fiche_adr);
		fiche_adr = inc_fifo(fiche_adr);

		ptr_ascii = 0;
		date_ascii(fiche.mesure.date);
		affiche_ligne(buffer_ascii, L3);


		aw += 1;
		if (aw>1000) aw = 100;

		tsurf += 10;
		if (tsurf>3000) tsurf = 2000;

		//duree+=600;
		//if(duree>7200) duree = 600;

		if ((date_courante / 86400) != ((date_courante + 1800) / 86400)) numero = 0;

		date_courante += 1800;
		numero++;



	} while (date_courante< (180176000 + 10000000));
	ECHO_CDE = 1;
}

void start_regu(void)
{
	REGU_COUPELLE = 1;
}

void stop_regu(void)
{
	REGU_COUPELLE = 0;
	regu = 0;
	sort_regu();
}

void regu_setting_on(void)
{
	regu_setting = 1;
	ECHO_CDE = 1;

}

void regu_setting_off(void)
{
	regu_setting = 0;
	ECHO_CDE = 1;

}

void erreur_cde(void)            // commande du PC invalide
{
	ERREUR = 1;
	ECHO_CDE = 1;
	bip(volume_key, 40, 50);
}

void rech_fin_enregistrement(void)
{
	static unsigned long last_date;
	static unsigned short last_adresse;
	static unsigned short scan;
	static char nb_essai;
	adresse_courante = 0;

	last_date = 0;

	nb_essai = 5;

	date_courante = recup_date();

	for (scan = 0; (scan < NB_FICHE) && (nb_essai != 0); scan++)
	{

		auto_noyau();
		cde_display(L3);
		txt_display("Scan: ");
		ptr_ascii = 0;
		ushort_ascii(adresse_courante);
		txt_display(buffer_ascii);
		txt_display("  ");


		read_enregistrement(&fiche, adresse_courante);

		adresse_courante = inc_fifo(adresse_courante);

		if ((fiche.mesure.date >= last_date) && (fiche.mesure.date != VIDE) && (fiche.mesure.date <= date_courante))
		{
			last_adresse = adresse_courante;
			last_date = fiche.mesure.date;
		}

		if (fiche.mesure.date != VIDE)
		{
			nb_essai = 5;
			ptr_ascii = 0;
			date_ascii(fiche.mesure.date);
			txt_display(buffer_ascii);
		}
		else
		{
			nb_essai--;
			txt_display("        ");
		}
	}
	efface_ligne(L3);

	if (last_date != 0)
		adresse_courante = last_adresse;
	else
		adresse_courante = 0;
}








void read_enregistrement(union fiche * ptr, unsigned short adrmem)
{
	char i;
	union short_sp adr;
	char checksum;

	EA = 0;

	adr.us = adrmem;
	adr.us *= 16;



	startI2C();

	envoiI2C(ADR_MEM);     // envoi de l'adresse du circuit
	slave_ack();

	if (BUSY == 0)
	{
		envoiI2C(adr.c[0]);   // envoi du poids fort de l'adresse mémoire 
		slave_ack();
	}
	if (BUSY == 0)
	{
		envoiI2C(adr.c[1]);    // envoi du poids faible de l'adresse mémoire 
		slave_ack();
	}
	if (BUSY == 0)
	{
		startI2C();
		envoiI2C(ADR_MEM + 1);        // demande de lecture
		slave_ack();
	}


	checksum = 0;
	for (i = 0; i<16; i++)                // recuperation des 16 caracteres de l'enregistrement de la mesure  
	{
		if (BUSY == 0)
		{
			ptr->index[i] = recoitI2C();
			if (i != 15) master_ack();
			else master_noack();

			if (i != 10) checksum += ptr->index[i];   // recuperation du check-sum
		}
	}
	checksum -= ptr->index[10];
	ptr->index[10] = 0;

	stopI2C();

	if ((BUSY != 0) || (checksum != 0))  ptr->mesure.date = 0;  // si probleme circuit memoire ou check-sum incorrect invalidation de la mesure

	EA = 1;
}



unsigned short read_enregistrement_prec(union fiche * ptr, unsigned short adresse)
{
	char nb_essai;
	nb_essai = 5;
	do
	{
		adresse = dec_fifo(adresse);
		nb_essai--;
		if (nb_essai == 0) adresse = adresse_courante;
		read_enregistrement(ptr, adresse);

	} while ((ptr->mesure.date == VIDE) && (nb_essai != 0));
	return(adresse);
}

unsigned short read_enregistrement_suiv(union fiche * ptr, unsigned short adresse)
{
	char nb_essai;
	nb_essai = 5;
	do
	{
		adresse = inc_fifo(adresse);
		nb_essai--;
		if (nb_essai == 0) adresse = adresse_courante;
		read_enregistrement(ptr, adresse);

	} while ((ptr->mesure.date == VIDE) && (nb_essai != 0));
	return(adresse);
}

void affiche_regu_temp(void)
{
	static xdata char i;
	if ((REGU_COUPELLE == 0) || (i>10)) affiche_titre(buffer_ascii, L3);
	else affiche_titre("     ", L3);
	if (i>20) i = 0;
	else i++;
}


void efface_tableau(void)
{
	char i;
	for (i = 0; i<NB_MESURE; i++)
	{
		tableau_result[i].mesure.date = 0;
		tableau_result[i].mesure.numero = 0;
		tableau_result[i].mesure.aw = 0;
		tableau_result[i].mesure.tsurf = 0;
		tableau_result[i].mesure.duree = 0;
		tableau_result[i].mesure.etat = 0;
		tableau_result[i].mesure.config = 0;
	}
	nb_mesure = 0;
	ligne_select = 0;
}

void tempo_cycle(void)
{
}
void tempo_printer(void)     // tempo adaptee a la cadence d'impression de l'imprimante
{
}

void transfert_pc(void)     // envoi d'une trame mesure
{

	if (ECHO_CDE != 0)
	{
		trame_EC();
		ECHO_CDE = 0;
	}

	if (TRANSFERT == 0)
	{
		tempo_cycle();
		SEND_MESURE = 0;
		return;
	}

	if (SEND_MESURE != 0)
	{
		trame_FI(0);
		SEND_MESURE = 0;
	}

	trame_AW();

}


void trame_EC(void)        // envoi d'une trame echo
{
	buffer_tx.uc[0] = 'E';
	buffer_tx.uc[1] = 'C';
	buffer_tx.uc[2] = r_etat;
	buffer_tx.uc[3] = commande;
	envoi_buffer();
}

void trame_FI(unsigned short adresse)     // envoi d'une trame FI
{

	buffer_tx.uc[0] = 'F';
	buffer_tx.uc[1] = 'I';
	buffer_tx.uc[2] = fiche.index[14];

	if (SEND_MESURE != 0) buffer_tx.uc[2] |= 0x08;    // bit mesure en cours = 1;
	else buffer_tx.uc[2] &= 0xF7;                  // bit mesure en cours = 0;

	buffer_tx.uc[3] = fiche.index[15];
	envoi_buffer();

	buffer_tx.uc[0] = identifiant[0];
	buffer_tx.uc[1] = identifiant[1];
	buffer_tx.uc[2] = identifiant[2];
	buffer_tx.uc[3] = identifiant[3];
	envoi_buffer();

	buffer_tx.uc[0] = identifiant[4];
	buffer_tx.uc[1] = identifiant[5];
	buffer_tx.uc[2] = identifiant[6];
	buffer_tx.uc[3] = identifiant[7];
	envoi_buffer();

	buffer_tx.uc[0] = identifiant[8];
	buffer_tx.uc[1] = identifiant[9];
	buffer_tx.uc[2] = identifiant[10];
	buffer_tx.uc[3] = identifiant[11];
	envoi_buffer();

	buffer_tx.uc[0] = identifiant[12];
	buffer_tx.uc[1] = identifiant[13];
	buffer_tx.uc[2] = identifiant[14];
	buffer_tx.uc[3] = identifiant[15];
	envoi_buffer();

	buffer_tx.uc[0] = fiche.index[0];
	buffer_tx.uc[1] = fiche.index[1];
	buffer_tx.uc[2] = fiche.index[2];
	buffer_tx.uc[3] = fiche.index[3];
	envoi_buffer();

	buffer_tx.uc[0] = fiche.index[4];
	buffer_tx.uc[1] = fiche.index[5];
	buffer_tx.uc[2] = fiche.index[6];
	buffer_tx.uc[3] = fiche.index[7];
	envoi_buffer();

	buffer_tx.uc[0] = fiche.index[8];
	buffer_tx.uc[1] = fiche.index[9];
	buffer_tx.uc[2] = fiche.index[10];
	buffer_tx.uc[3] = fiche.index[11];
	envoi_buffer();

	buffer_tx.uc[0] = fiche.index[12];
	buffer_tx.uc[1] = fiche.index[13];

	buffer_tx.us[1] = adresse;
	envoi_buffer();


	buffer_tx.ul = 0;
	envoi_buffer();

}



void trame_AW(void)

{

	buffer_tx.uc[0] = 'A';
	buffer_tx.uc[1] = 'W';
	buffer_tx.uc[2] = r_etat;
	buffer_tx.uc[3] = r_config;
	envoi_buffer();

	// INFOS RELATIVES AUX ENTREES SORTIES

	if (regu_setting != 0)
	{
		buffer_tx.s[0] = (short)(ttete*100.0);
	}
	else
	{
		buffer_tx.s[0] = (short)(tmir*100.0);
	}
	buffer_tx.s[1] = (short)(tsurf*100.0);
	envoi_buffer();

	buffer_tx.s[0] = (short)(reflexion*100.0);
	buffer_tx.s[1] = (short)(detection*100.0);
	envoi_buffer();

	if (REGU_COUPELLE) buffer_tx.s[0] = regu;
	else buffer_tx.s[0] = peltier;
	buffer_tx.uc[2] = 0;
	buffer_tx.uc[3] = numero_param;
	envoi_buffer();

	// INFOS DIVERSES MULTIPLEXEES

	switch (numero_param)
	{
	case 0:
		buffer_tx.ul = time_mesure;

		break;
	case 1:
		buffer_tx.s[0] = (short)(delta_tmir*100.0f);


		if (regu_setting != 0)
		{
			buffer_tx.s[1] = (short)(pente_correction_tsurf*1000.0f);
		}
		else
			buffer_tx.s[1] = (short)(pente_tmir*1000.0f);


		break;
	case 2:

		if ((type_capteur == 'B') || (regu_setting != 0))
		{
			buffer_tx.s[0] = (short)(delta_ttete*100.0f);
			buffer_tx.s[1] = (short)(pente_ttete*1000.0f);;
		}
		else
		{
			buffer_tx.s[0] = (short)(delta_tsurf*100.0f);
			buffer_tx.s[1] = (short)(pente_tsurf*1000.0f);;
		}


		break;
	case 3:
		buffer_tx.uc[0] = dseuil;
		buffer_tx.uc[1] = pointage_flash;

		if ((type_capteur == 'B') || (regu_setting != 0))
		{
			buffer_tx.uc[2] = gain_p;
			buffer_tx.uc[3] = gain_i;
		}
		else
		{
			buffer_tx.uc[2] = gain_p;
			buffer_tx.uc[3] = gain_i;
		}
		break;
	case	4:
		buffer_tx.us[0] = numero_mesure;
		//buffer_tx.us[1]=total_mesure;
		break;

	case 5:
		buffer_tx.ul = date_courante;
		break;

	default:
		buffer_tx.uc[0] = identifiant[numero_param * 4 - 24];
		buffer_tx.uc[1] = identifiant[numero_param * 4 - 23];
		buffer_tx.uc[2] = identifiant[numero_param * 4 - 22];
		buffer_tx.uc[3] = identifiant[numero_param * 4 - 21];
		break;



	}
	numero_param++;
	if (numero_param >= 10) numero_param = 0;
	envoi_buffer();


	// INFOS RELATIVES AU SYSTEME 


	buffer_tx.s[0] = time_etape;
	buffer_tx.uc[2] = etape;
	buffer_tx.uc[3] = num_tache;
	envoi_buffer();



	// INFOS RELATIVE A L'ASSERVISSEMENT DE REFLEXION


	//@TODO erase ptr from project!!!
	buffer_tx.s[0] = (short)(xi*100.0f);
	buffer_tx.c[2] = xi_int / TAILLE_FIFO;
	//ptr.c = OPTIMIZE;
	//buffer_tx.uc[3] = *ptr.c & 0x0f;
	envoi_buffer();


	// INFOS RELATIVE AUX MESURES DE L'AW 


	buffer_tx.s[0] = aw_inst;
	buffer_tx.s[1] = aw_flash;
	envoi_buffer();


	buffer_tx.s[0] = aw_moy_brut;

	if (nb_point != 0)
		buffer_tx.s[1] = fifo_point[(nb_point - 1) % NB_POINTS];
	else
		buffer_tx.s[1] = 0;

	envoi_buffer();


	buffer_tx.uc[0] = nb_point;
	buffer_tx.c[1] = ppm_sec;

	buffer_tx.s[1] = aw_finale;

	envoi_buffer();




}

//@TODO launch interrupt
void auto_noyau(void) // authorize an interrupt requested by the real-time kernel
{

	EA = 1;
	ET0 = 1;
	ET0 = 1;
	ET0 = 0;

}

//@TODO send pc
void envoi_buffer(void) // send the 4-byte buffer for communication with the PC
{
	/*ET0 = 1;
	TI = 0;
	SBUF = buffer_tx.uc[0];
	while (DATA_TX == 0) {}
	DATA_TX = 0;
	TI = 0;
	SBUF = buffer_tx.uc[1];
	while (DATA_TX == 0) {}
	DATA_TX = 0;
	TI = 0;
	SBUF = buffer_tx.uc[2];
	while (DATA_TX == 0) {}
	DATA_TX = 0;
	TI = 0;
	SBUF = buffer_tx.uc[3];
	while (DATA_TX == 0) {}
	DATA_TX = 0;
	ET0 = 0;*/
	log_error("envoi_buffer not implemented");

}