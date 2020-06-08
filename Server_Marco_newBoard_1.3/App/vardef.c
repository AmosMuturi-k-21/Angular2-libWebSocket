#include "vardef.h"
//#include <I2C.h>


//void conv_int_str(int valeur,char * a);
//void conv_long_str(long valeur,char * a);

 unsigned char EA = 0;
 unsigned char ET0 = 0;
 unsigned char RI = 0;
 unsigned char TR0 = 0;


// configuration booléenne de la variable bool (adresse 0x24)
 
 //etat
/*at 0x00*/ bit FLASH = 0;
/*at 0x01*/ bit CINETIQUE = 0;
/*at 0x02*/ bit CALIB = 0;
/*at 0x03*/ bit MESURE_EN_COURS = 0;
/*at 0x04*/ bit REGU_COUPELLE = 0;
/*at 0x05*/ bit TOP_REGU = 0;
/*at 0x06*/ bit CLEAN_MIRROR = 0;
/*at 0x07*/ bit ERREUR = 0;


//config
/*at 0x08 */bit MODE_MES_L = 0;
/*at 0x09 */bit MODE_MES_H = 0;
/*at 0x0a */bit MODE_FLASH = 0;
/*at 0x0b */bit TYPE_C = 0;
/*at 0x0c */bit LANGUE = 0;
//at 0x0d bit 0
/*at 0x0e */bit OPTION_L = 0;
/*at 0x0f */bit OPTION_H = 0;

//CONTROL_0
/*at 0x10 */bit ETAPE_L = 0;
/*at 0x11 */bit ETAPE_H = 0;
/*at 0x12 */bit CYCLE_INT = 0;
/*at 0x13 */bit ACQUISITION = 0;
/*at 0x14 */bit BUSY = 0;
/*at 0x15 */bit DISPLAY = 0;
/*at 0x16 */bit EQUILIBRE = 0;
/*at 0x17 */bit PURGE = 0;

//CONTROL_1
/*at 0x18 */bit TSURF_MEM = 0;
/*at 0x19 */bit INC_NUM = 0;
/*at 0x1a */bit TABLEAU = 0;
/*at 0x1b */bit MENU = 0;
/*at 0x1c */bit DATE_SELECT = 0;
/*at 0x1d */bit M_DEM_AUTO = 1;
/*at 0x1e */bit M_BP_G = 0;
/*at 0x1f */bit M_BP_D = 0;

//INTERFACE
/*at 0x20 */bit ECHO_CDE = 0;
/*at 0x21 */bit TRANS_PARAM = 0;
/*at 0x22 */bit TRANSFERT = 0;
/*at 0x23 */bit DATA_TX = 0;
/*at 0x24 */bit CODE_OK = 0;
/*at 0x25 */bit SEND_MESURE = 0;

//OPTIMIZE
/*at 0x28 */bit XI_DERIVE_NEG = 0;
/*at 0x29 */bit XI_DERIVE_POS = 0;
/*at 0x2a */bit XI_NEG = 0;
/*at 0x2b */bit XI_POS = 0;
/*at 0x2c */bit BOOST_L = 0;
/*at 0x2d */bit BOOST_H = 0;
/*at 0x2e */bit ACQUISITION_POINT = 0;
/*at 0x2f */bit NOUVEAU_POINT = 0;



/*data char etat;
data char config;
data char control_0;
data char control_1;
data char interface;
data char optimize;*/

/*at 0x26 */data short peltier = 0;
/*at 0x28 */data signed char moteur_bip = 0;
/*at 0x2a */data char val_can = 0;
/*at 0x2c */data short regu = 0;

//at 0x2a data unsigned short ptr_asm;
//at 0x2c data char val_asm;

//data char voie_en_cours=0;


data float tmir = 0.0f;
xdata float delta_tmir = 0.0f;
xdata float pente_tmir = 1.0f;
data float tsurf = 0.0f;
xdata float delta_tsurf = 0.0f;
xdata float pente_tsurf = 1.0f;
xdata float reflexion = 0.0f;
xdata float detection = 0.0f;

xdata float delta_ttete = 0.0f;
xdata float pente_ttete = 1.0f;

xdata float ttete = 0.0f;
xdata char mode_essai = 0;


xdata unsigned short aw_inst = 0;
xdata unsigned short aw_flash = 0;
xdata unsigned short aw_finale = 0;
xdata unsigned short aw_int = 0;



data float facteur_p_cent = 0.0f;
data float xi = 0.0f;
data short xi_int = 0;
data short xi_pel = 0;

data char dseuil = 1;
data char pointage_flash = 128;

//signed char xi_moy;

xdata unsigned long time_system = 0;
data short time_etape = 0;
data short time_point = 0;

xdata unsigned long time_mesure = 0;

xdata unsigned short numero_mesure = 0;

data char num_tache = 0;
//data char index = 0;


data char nb_point = 0;
data signed char ppm_sec = 0;

//data union ptr_sp ptr;


data unsigned char gain_p = 100;
data unsigned char gain_i = 100;

data char etape_communication = 0;
//char etape_commande;
data char commande = 0;
data union long_sp buffer_tx;
data union long_sp buffer_rx;

//data union short_sp val=0;

data char numero_param = 0;



//data union ptr_x ptx;


data char ligne_select = 0;
data char nb_mesure = 0;

xdata char nb_erreur = 0;



//union long_sp temp;

/*
xdata unsigned char mesure_status[NB_MESURE+1];
xdata unsigned char mesure_config[NB_MESURE+1];
xdata unsigned short mesure_numero[NB_MESURE+1];
xdata unsigned short mesure_aw[NB_MESURE+1];
xdata short mesure_surf[NB_MESURE+1];
xdata unsigned long mesure_duree[NB_MESURE+1];
xdata unsigned long mesure_date[NB_MESURE+1];
*/

xdata char identifiant[16];
xdata char buffer_ascii[50];
xdata char ptr_ascii;
xdata union fiche tableau_result[NB_MESURE];
xdata union fiche fiche;

xdata unsigned short adresse_courante;
xdata unsigned short adresse_jour;
xdata unsigned short adresse_jour_suiv;

//xdata union fiche * data ptr_histo;

xdata unsigned long date_courante = 0;
xdata double time_mesure_fl;


data unsigned short aw_moy = 0;

xdata char volume_key;

//xdata short val_regu = 0;
xdata float consigne_regu = 20.0f;
xdata float consigne_rampe = 5.0f;
xdata float mesure_rampe = 0.0f;
xdata long regu_correction_P = 0;
xdata long regu_correction_I = 0;
xdata short regu_gain_P = -1500;
xdata short regu_gain_I = -100;


xdata char regu_setting = 0;
xdata char type_capteur = 'A';
xdata float pente_correction_tsurf = 1.0f;
xdata char sauve_etat_regu = 0;


xdata char entete_ok = 0;

xdata unsigned char mode_calibration = 0;
xdata char num_pt_calib = 0;

xdata float tsurf_1;
xdata float tsurf_2;

xdata unsigned short aw_mes_1;
xdata unsigned short aw_mes_2;
xdata unsigned short aw_rech_1;
xdata unsigned short aw_rech_2;

xdata unsigned short aw_finale_brut;
xdata unsigned short aw_moy_brut;

xdata char message;

code char gbx[5] = { 255,'G','B','X',255 };

//at 0xB000 code * code adr_histo;



code short tab_liste[NB_LISTE][TAILLE_LISTE] =    // numéro des sels de chaque liste disponible
{
	{
		0,
		1,
		4,
		6,
		EOF
	},
	{
		2,
		4,
		7,
		EOF
	},
	{
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7,
		EOF
	},
	{
		11,
		13,
		17,
		21,
		23,
		25,
		EOF
	},
	{
		8,
		10,
		14,
		16,
		19,
		22,
		24,
		EOF
	},
	{
		9,
		11,
		15,
		16,
		18,
		20,
		22,
		23,
		26,
		EOF
	},




};







code unsigned short tab_sel[NB_SELS][9] =    // valeurs milli. aw  theoriques de differents sels de 0 a 40°C par pas de 5°C
{
	{							// LiCl indice 0
		113,
		113,
		113,
		113,
		113,
		113,
		113,
		113,
		112
	},
	{							// MgCl2 indice 1
		338,
		336,
		335,
		333,
		331,
		328,
		324,
		321,
		316
	},
	{							// K2CO3 indice 2
		431,
		431,
		432,
		432,
		432,
		432,
		432,
		432,
		432
	},
	{							// MgNO3 indice 3
		604,
		589,
		574,
		559,
		544,
		529,
		514,
		499,
		484
	},
	{							// NaCl indice 4
		757,
		757,
		757,
		756,
		755,
		753,
		751,
		748,
		747
	},

	{							// KCl indice 5
		885,
		877,
		868,
		859,
		851,
		843,
		836,
		830,
		823
	},
	{							// K2SO4 indice 6
		988,
		985,
		982,
		979,
		976,
		973,
		970,
		967,
		964
	},
	{							// H2O indice 7
		1000,
		1000,
		1000,
		1000,
		1000,
		1000,
		1000,
		1000,
		1000
	},
	{							// 0.1 indice 8
		100,
		100,
		100,
		100,
		100,
		100,
		100,
		100,
		100
	},

	{							// 0.125 indice 9
		125,
		125,
		125,
		125,
		125,
		125,
		125,
		125,
		125
	},
	{							// 0.20 indice 10
		200,
		200,
		200,
		200,
		200,
		200,
		200,
		200,
		200
	},
	{							// 0.25 indice 11
		250,
		250,
		250,
		250,
		250,
		250,
		250,
		250,
		250
	},
	{							// 0.30 indice 12
		300,
		300,
		300,
		300,
		300,
		300,
		300,
		300,
		300
	},
	{							// 0.34 indice 13
		340,
		340,
		340,
		340,
		340,
		340,
		340,
		340,
		340
	},
	{							// 0.35 indice 14
		350,
		350,
		350,
		350,
		350,
		350,
		350,
		350,
		350
	},
	{							// 0.40 indice 15
		400,
		400,
		400,
		400,
		400,
		400,
		400,
		400,
		400
	},
	{							// 0.50 indice 16
		500,
		500,
		500,
		500,
		500,
		500,
		500,
		500,
		500
	},
	{							// 0.53 indice 17
		530,
		530,
		530,
		530,
		530,
		530,
		530,
		530,
		530
	},
	{							// 0.60 indice 18
		600,
		600,
		600,
		600,
		600,
		600,
		600,
		600,
		600
	},
	{							// 0.65 indice 19
		650,
		650,
		650,
		650,
		650,
		650,
		650,
		650,
		650
	},
	{							// 0.70 indice 20
		700,
		700,
		700,
		700,
		700,
		700,
		700,
		700,
		700
	},
	{							// 0.76 indice 21
		760,
		760,
		760,
		760,
		760,
		760,
		760,
		760,
		760
	},
	{							// 0.80 indice 22
		800,
		800,
		800,
		800,
		800,
		800,
		800,
		800,
		800
	},
	{							// 0.90 indice 23
		900,
		900,
		900,
		900,
		900,
		900,
		900,
		900,
		900
	},
	{							// 0.90 indice 24 
		950,
		950,
		950,
		950,
		950,
		950,
		950,
		950,
		950
	},
	{							// 0.98 indice 25 
		980,
		980,
		980,
		980,
		980,
		980,
		980,
		980,
		980
	},
	{							// 1.00 indice 26
		1000,
		1000,
		1000,
		1000,
		1000,
		1000,
		1000,
		1000,
		1000
	}
};




code char * code sel_ascii[NB_SELS] =
{
	"LiCl",
	"MgCl2",
	"K2CO3",
	"MgNO3",
	"NaCl",
	"KCl",
	"K2SO4",
	"H2O",
	"0.100",
	"0.125",
	"0.200",
	"0.250",
	"0.300",
	"0.340",
	"0.350",
	"0.400",
	"0.500",
	"0.530",
	"0.600",
	"0.650",
	"0.700",
	"0.760",
	"0.800",
	"0.900",
	"0.950",
	"0.980",
	"1.000"
};




/*
code char * code txt_titre[4]=
{
"Configuration",
"Calibration",
"Exportation",
"Sortir"
};

*/







//code char * code txt_exportation = "Exportation en cours";

code char * code txt_tableau[NB_LANGUE] =
{
#ifdef FRANCAIS
	"Num Aw    T[C] Duree",
#endif
#ifdef ALLEMAND
	"Nr. Aw    T[C] Dauer",
#endif
	"No  Aw    T[C] Delay"
};

code char * code txt_histo[NB_LANGUE] =
{
#ifdef FRANCAIS
	"Num Aw    T[C] Heure",
#endif
#ifdef ALLEMAND
	"Nr. Aw    T[C] Zeit",
#endif
	"No  Aw    T[C] Time"
};

code char * code txt_print[NB_LANGUE] =
{
#ifdef FRANCAIS
	"Num  Aw     T[C]  Heure  Duree  Mode",
#endif
#ifdef ALLEMAND
	"Nr.  Aw     T[C]  Zeit   Dauer  Modus",
#endif
	"No   Aw     T[C]  Time   Delay  Mode"
};

code char * code txt_delay[NB_LANGUE] =
{
#ifdef FRANCAIS
	"Duree: ",
#endif
#ifdef ALLEMAND
	"Dauer: ",
#endif
	"Delay: "

};


code char * code txt_number[NB_LANGUE] =
{
#ifdef FRANCAIS
	"Num  : ",
#endif
#ifdef ALLEMAND
	"Nr.  : ",
#endif
	"No   : "

};

code char * code txt_stat[NB_LANGUE] =
{
#ifdef FRANCAIS
	"   Min   Max   Moy  ",
#endif
#ifdef ALLEMAND
	"   Min   Max   Mitt.",
#endif
	"   Min   Max   Aver."
};

code char * code txt_menu[NB_LANGUE][NB_MENU + 1] =
{
#ifdef FRANCAIS
	{
		"Tableau resultats",
		"Regulation",
		"Historique",
		"Vitesse ventil.",
		"Mode mesure",
		"Mode cinetique",
		"Mode calibration",
		"Demarrage auto",
		"Impression auto",
		"Volume mesure",
		"Volume clavier",
		"Langue",
		"[RETOUR]"
	},
#endif

#ifdef ALLEMAND
	{
		"Ergebnis Tabelle",
		"Verordnung Cup",
		"Historie",
		"Ventilator Geschw.",
		"Messmodus",
		"Kinetik Modus",
		"Calib Modus",
		"Auto Start",
		"Auto Drucken",
		"Lautstarke Messung",
		"Lautstarke Knopfe",
		"Sprache",
		"[ZURUCK]"
	},


#endif	

	{
		"Results table",
		"Regulation cup",
		"History",
		"Fan Speed",
		"Measurement mode",
		"Kinetic mode",
		"Calib mode",
		"Auto-start",
		"Auto-print",
		"Measurement volume",
		"Key volume",
		"Language",
		"[ESCAPE]"
	}

};


code char * code txt_choix[NB_LANGUE][NB_MENU][NB_CHOIX] =
{
	{

#ifdef FRANCAIS

		{
			"Afficher",
			"Imprimer",
			"Exporter",
			"Statistiques",
			"Effacer",
			"[RETOUR]"
		},
		{
			"Select. Temp.",
			"Activer",
			"Arreter",
			"[RETOUR]",
		},
		{
			"Selection date",
			"Afficher",
			"Imprimer",
			"Exporter",
			"Statistiques",
			"Reglage date/heure",
			"[RETOUR]",
		},
		{
			"Rapide",
			"Moyenne",
			"Lente"

		},
		{
			"2->5 mn",
			"3->10 mn ",
			"6->30 mn",
			"Fixe 10 mn",
			"2->5 mn + Flash",
			"3->10 mn + Flash",
			"6->30 mn + Flash",
			"Fixe 10 mn + Flash"
		},
		{
			"15 mn (2 mn 30)",
			"30 mn (2 mn 30)",
			"1 h (5 mn)",
			"2 h (10 mn)",
			"4 h (15 mn)",
			"8 h (30 mn)",
			"Continu (60 mn)",
			"Continu (2 mn 30)"
		},
		{
			"1 point, liste 1",
			"1 point, liste 2",
			"1 point, liste 3",
			"1 point, liste 4",
			"1 point, liste 5",
			"1 point, liste 6",
			"2 points, liste 1",
			"2 points, liste 2",
			"2 points, liste 3",
			"2 points, liste 4",
			"2 points, liste 5",
			"2 points, liste 6",
		},
		{
			"Inactif",
			"Mesure mode 1",
			"Mesure mode 2",
			"Cinetique mode 1",
			"Cinetique mode 2"
		},
		{
			"Non",
			"Mode 1",
			"Mode 2"

		},

		{
			"0",
			"1",
			"2",
			"3",
			"4",
			"5"
		},
		{
			"0",
			"1",
			"2",
			"3",
			"4",
			"5"
		}
	,
	{
		"Francais",
		"Anglais"
	}
	},
#endif
#ifdef ALLEMAND
	{
		"Anzeigen",
		"Drucken",
		"Export",
		"Statistik",
		"Loschen",
		"[ZURUCK]"
	},
	{
		"Temperatur",
		"Start",
		"Stop",
		"[ZURUCK]"
	},
	{
		"Datum Wahlen",
		"Anzeigen",
		"Drucken",
		"Export",
		"Statistik",
		"Anpassen Dat./Zeit",
		"[ZURUCK]"
	},
	{
		"Schnell",
		"Mittel",
		"Langsam"

	},
	{
		"2->5 mn",
		"3->10 mn ",
		"6->30 mn",
		"Fest 10 mn",
		"2->5 mn + Flash",
		"3->10 mn + Flash",
		"6->30 mn + Flash",
		"Fest 10 mn + Flash"
	},
	{
		"15 mn (2 mn 30)",
		"30 mn (2 mn 30)",
		"1 h (5 mn)",
		"2 h (10 mn)",
		"4 h (15 mn)",
		"8 h (30 mn)",
		"Fortlaufend (60 mn)",
		"Fortlauf.(2 mn 30)"
	},
	{
		"1 Punkt, Liste 1",
		"1 Punkt, Liste 2",
		"1 Punkt, Liste 3",
		"1 Punkt, Liste 4",
		"1 Punkt, Liste 5",
		"1 Punkt, Liste 6",
		"2 Punkte, Liste 1",
		"2 Punkte, Liste 2",
		"2 Punkte, Liste 3",
		"2 Punkte, Liste 4",
		"2 Punkte, Liste 5",
		"2 Punkte, Liste 6",
	},
	{
		"Aus",
		"Messmodus 1",
		"Messmodus 2",
		"Kinetik Modus 1",
		"Kinetik Modus 2"
	},
	{
		"Nein",
		"Modus 1",
		"Modus 2"
	},


	{
		"0",
		"1",
		"2",
		"3",
		"4",
		"5"
	},
	{
		"0",
		"1",
		"2",
		"3",
		"4",
		"5"
	}
	,
	{
		"Deutsch",
		"Englisch"
	}
},

#endif

{
	{
		"Display",
		"Print",
		"Export",
		"Statistics",
		"Erase",
		"[ESCAPE]"
	},
	{
		"Select. Temp.",
		"Start",
		"Stop",
		"[ESCAPE]",
	},
	{
		"Select date",
		"Display",
		"Print",
		"Export",
		"Statistics",
		"Date/Time adjust",
		"[ESCAPE]"
	},
	{
		"Fast",
		"Medium",
		"Slow"

	},
	{
		"2->5 mn",
		"3->10 mn ",
		"6->30 mn",
		"Fixed 10 mn",
		"2->5 mn + Flash",
		"3->10 mn + Flash",
		"6->30 mn + Flash",
		"Fix. 10 mn + Flash"
	},
	{
		"15 mn (2 mn 30)",
		"30 mn (2 mn 30)",
		"1 h (5 mn)",
		"2 h (10 mn)",
		"4 h (15 mn)",
		"8 h (30 mn)",
		"Continuous (60 mn)",
		"Continu. (2 mn 30)"
	},
	{
		"1 point, list 1",
		"1 point, list 2",
		"1 point, list 3",
		"1 point, list 4",
		"1 point, list 5",
		"1 point, list 6",
		"2 points, list 1",
		"2 points, list 2",
		"2 points, list 3",
		"2 points, list 4",
		"2 points, list 5",
		"2 points, list 6",
	},
	{
		"Off",
		"Measurement mode 1",
		"Measurement mode 2",
		"Kinetic mode 1",
		"Kinetic mode 2"

	},
	{
		"Off",
		"Mode 1",
		"Mode 2"
	},

	{
		"0",
		"1",
		"2",
		"3",
		"4",
		"5"
	},
	{
		"0",
		"1",
		"2",
		"3",
		"4",
		"5"
	},
#ifdef FRANCAIS
	{
		"French",
		"English"
	}
#endif
#ifdef ALLEMAND
{
	"German",
		"English"
}
#endif
}
};










code char val_choix[NB_MENU][NB_CHOIX] =
{
	{// RESULTS 0
		0,
		1,
		2,
		3,
		4
	},
	{ //REGU 1
		0,
		1,
		2
	},

	{ //HISTO 2
		0,
		1,
		2,
		3,
		4,
		5
	},
	{ //FAN 3
		90,
		60,
		40
	},
	{//MODE MEASURE 4
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7
	},
	{ // MODE CINETIC 5
		0,
		1,
		2,
		3,
		4,
		5,
		6,
		7
	},
	{ //calibration 6
		0,
		1,
		2,
		3,
		4,
		5,
		128,
		129,
		130,
		131,
		132,
		133
	},



	{ //dem auto 7
		0,
		1,
		2,
		3,
		4
	},
	{
		0,
		1,
		2
	},
	{
		0,
		8,
		16,
		32,
		64,
		128
	},
	{
		0,
		3,
		6,
		12,
		24,
		48
	},
	{
		0,
		1
	}


};


// actuellement 5 niveaux d'options 

//A version de base sans demarrage auto
//B version de base avec demarrage auto
//C version prosoft
//D version elite
//E version regulation peltier


code char option_menu_min[NB_MENU + 2] =
{
	'C',
#ifndef NO_MENU_REGU
	'E',
#else	
	'G',
#endif	
	'D',
	'C',
	'C',
	'C',
#ifndef NO_MENU_CALIB
	'C',
#else
	'F',
#endif
	'B',
	'D',
	'A',
	'A',
	'A',
	ESC,
	0
};

code char option_menu_max[NB_MENU + 2] =
{
	'F',
	'F',
	'F',
	'F',
	'F',
	'F',
	'F',
	'F',
	'F',
	'F',
	'F',
	'F',
	ESC,
	0
};




code char option_choix[NB_MENU][NB_CHOIX + 1] =
{
	{
		'C',
		'D',
		'C',
		'D',
		'C',
		ESC,
		0

	},

	{
		'E',
		'E',
		'E',
		ESC,
		0

	},

	{
		'D',
		'D',
		'D',
		'D',
		'D',
		'D',
		ESC,
		0

	},
	{
		'C',
		'C',
		'C',
		0
	},
	{
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		0
	},
	{
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		0
	},
	{
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		'C',
		0
	},
	{
		'B',
		'B',
		'B',
		'C',
		'C',
		0
	},
	{
		'A',
		'A',
		'A',
		0
	},
	{
		'A',
		'A',
		'A',
		'A',
		'A',
		'A',
		0
	},
	{
		'A',
		'A',
		'A',
		'A',
		'A',
		'A',
		0
	},
	{
		'A',
		'A',
		0
	}
};





code char * code txt_message[NB_LANGUE][29] =
{
#ifdef FRANCAIS
	{
		"MISE A JOUR TETE",
		"TETE NON RECONNUE",
		"NETTOYER LE MIROIR",
		"MODE INAPPROPRIE",
		"VIDE",
		"RECALIBRER SVP",
		"PRET POUR CHARGEMENT",
		"ETEIGNER L'APPAREIL",
		"IMPRESSION ANNULEE",
		"EFFACEMENT: ",
		"Cinet.",
		"Flash",
		"Cal.",
		"Ident: ",
		"Date : ",
		"ATTENTION INSTABIL.",
		"RISQUE DE CONDENS.",
		"REGULATION TEMP STAB",
		"REINITIALISATION OK",
		"Mode : ",
		"POINTS TRES PROCHES",
		"ERREUR CALIBRATION",
		"LIMITE TOLERANCE",
		"CALIBRATION OK",
		"PATIENTEZ SVP",
		"METTRE UN SECOND SEL",
		"Mettre un sel parmi:",
		"Point 1/2",
		"Point 2/2",
	},
#endif
#ifdef ALLEMAND
	{
		"UPDATE PARAMETER",
		"KOPF FEHLER",
		"SPIEGEL SAUBERN",
		"UNGULTIGER MODUS",
		"LEER",
		"BITTE NEUKALIBRIEREN",
		"BEREIT ZUM UPDATE",
		"STROM ABSCHALTEN",
		"DRUCK ABGEBROCHEN",
		"LOSCHEN: ",
		"Kinetic",
		"Flash",
		"Kal."
		"ID   : ",
	"Datum: ",
	"MESSFEHLER",
	"GEFAHR DER KONDENS.",
	"REGULATION TEMP STAB",
	"REINITIALISIERUNG OK",
	"Modus: ",
	"PUNKTE ZEHR NAHE",
	"ERROR KALIBRIERUNG",
	"TOLERANZGRENZE",
	"KALIBRIERUNG OK",
	"BITTE WARTEN",
	"LEGTE EIN SWEITES SALZ",
	"Salz unterlegen:",
	"Point 1/2",
	"Point 2/2",
	},

#endif

	{
		"UPDATE PARAMETERS",
		"HEAD ERROR",
		"CLEAN MIRROR, PLS",
		"INAPPROPRIATE MODE",
		"EMPTY",
		"RECALIBRATE, PLS",
		"READY FOR UPDATE",
		"SWITCH OFF POWER",
		"PRINT CANCELLED",
		"ERASING: ",
		"Kinetic",
		"Flash",
		"Cal",
		"ID   : ",
		"Date : ",
		"WARNING: UNSTABLE",
		"RISK OF CONDENSATION",
		"REGULATION TEMP STAB",
		"REINITIALIZATION OK",
		"Mode : ",
		"POINTS VERY CLOSE",
		"ERROR CALIBRATION",
		"TOLERANCE LIMIT",
		"CALIBRATION OK",
		"PLEASE WAIT",
		"PUT A SECOND SALT",
		"Put a salt among:",
		"Point 1/2",
		"Point 2/2",
	}
};


code char * code txt_action[2][3] =
{
#ifdef FRANCAIS
	{
		"MESURE EN COURS",
		"CINETIQUE EN COURS",
		"CALIBRATION EN COURS"
	},
#endif

#ifdef ALLEMAND
	{
		"MESSUNG LAUFT",
		"KINETIK LAUFT",
		"KALIBRIERUNG LAUFT"
	},

#endif

	{
		"MEASUREMENT RUNNING",
		"KINETIC RUNNING",
		"CALIBRATION RUNNING"
	}

};

code char * code txt_arret[2][3] =
{
#ifdef FRANCAIS
	{
		"MESURE STOPPEE",
		"CINETIQUE STOPPEE",
		"CALIBRATION STOPPEE"
	},
#endif
#ifdef ALLEMAND
	{
		"MESSUNG ABGEBROCHEN",
		"KINETIK ABGEBROCHEN",
		"KALIBRIERUNG ABGEBR."
	},

#endif
	{
		"MEASUREMENT ABORTED",
		"KINETIC ABORTED",
		"CALIBRATION ABORTED"
	}
};


code char * code  txt_etape[2][4] =
{
#ifdef FRANCAIS
	{
		"Etape repos",
		"Etape equilibre",
		"Etape recherche",
		"Etape mesure"
	},
#endif
#ifdef ALLEMAND
	{
		"Ruhe Phase",
		"Gleichgewichts Phase",
		"Such Phase",
		"Messphase"
	},
#endif
	{
		"Rest stage",
		"Equilibrium stage",
		"Seek stage",
		"Measuring stage"

	}
};




code char * code txt_variable[2][6] =
{
#ifdef FRANCAIS
	{
		"Tps mesure: ",
		"T. surface: ",
		"Aw moyenne: ",
		"Aw mesuree: ",
		"Aw flash  : ",
		"Aw theor. : "
	},
#endif
#ifdef ALLEMAND	
	{
		"Dauer     : ",
		"Temperatur: ",
		"Aw Mittel.: ",
		"Aw Gemess.: ",
		"Aw Blitz  : ",
		"Aw Theor. : "
	},

#endif
	{
		"Duration  : ",
		"Surface T.: ",
		"Aw average: ",
		"Aw mesured: ",
		"Aw flash  : ",
		"Aw theor. : "
	}
};
/*
code char * code txt_mode[2][2]=
{
#ifdef FRANCAIS
{
"Flash",
"Cinet."
},

#endif
#ifdef ALLEMAND
{
"Blitz",
"Kinetic"
},
#endif
{
"Flash",
"Kinetic"
}

};

*/


code unsigned short duree_cinet[8] =         // duree totale maximum de la cinetique en fonction du mode
{
	900,
	1800,
	3600,
	7200,
	14400,
	28800,
	0,
	0
};


code unsigned short interval_mes[8] =       // interval entre 2 mesures en fonction du mode cinetique
{
	150,
	150,
	300,
	600,
	900,
	1800,
	3600,
	150
};



code unsigned short duree_mes[8] =        // duree de la mesure en seconde
{
	150,
	150,
	300,
	300,
	300,
	300,
	300,
	150
};

code signed char ppm_max[3] =            // limite de variation de l'aw en fonction mode de mesure (critere de stabilite)
{
	45,
	35,
	15,
};

code char interval_point[4] =         // intervalle des points en 1/2 secondes en fonction du mode de mesure
{
	10,
	20,
	60,
	40
};

code unsigned short duree_max[4] =       // duree maximum en seconde de la mesure en fonction du mode de mesure
{
	300,
	600,
	1800,
	600
};

code float tab_regu[NB_CHOIX_REGU] =
{

	10.0f,
	15.0f,
	17.5f,
	20.0f,
	21.0f,
	22.0f,
	22.5f,
	23.0f,
	24.0f,
	25.0f,
	27.5f,
	30.0f,
	32.5f,
	35.0f,
	37.5f,
	40.0f,

};






/*
code unsigned short duree_min[4]=		// duree minimum en seconde de la mesure en fonction du mode de mesure
{
120,
180,
360,
600
};
*/
code char niveau_extrap[4] =
{
	25,
	15,
	15,
	15
};

code void(*code p_cde[NB_CDE])(void) =        // TABLEAU DE COMMANDES voir fichier protocole
{
	rien,
	arret_utilisateur,
	mesure_std,
	cinetique_std,
	update_soft,
	calib_auto_std,
	set_char,
	set_char,
	transfert_off,
	transfert_on,
	set_short,
	set_short,
	set_short,
	set_short,
	set_char,
	set_char,
	set_char,
	set_char,
	set_char,
	set_char,
	set_char,
	set_char,
	set_char,
	set_char,
	set_short,
	set_identifiant,
	set_long,
	set_long,
	set_long,
	transfert_histo,
	efface_histo,
	sim_histo,
	stop_regu,
	start_regu,
	set_short,
	set_short,
	set_short,
	set_short,
	regu_setting_off,
	regu_setting_on,
	set_char,
	erreur_cde
};
