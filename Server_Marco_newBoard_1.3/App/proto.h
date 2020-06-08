#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "app.h"

#define OLD_MODE_LCD
//#define PRINT_DEBUG

#define abs_fl(A) ((A) >= 0.0 ? (A) : (-(A)))

#define ABS(A) ((A) >= 0 ? (A) : (-(A)))
//#define VIDE 0xFFFFFFFF

#define VIDE 0

#define VERSION 360

#define NB_LANGUE 2
#define NB_MENU 12
#define NB_CHOIX 12

#define NB_SELS 27
#define NB_LISTE 6
#define TAILLE_LISTE 10

#define NB_FICHE 2000
#define NB_MESURE 25
#define TAILLE_FIFO 40
//#define TAILLE_FIFO 80
#define DUREE_INTEGRATION 12

#define VAL_CNA 0x10
 
#define START 1
#define STOP 0
#define DEBUT 0
#define FIN 255
#define ESC 27

#define CR 13
#define LF 10
#define EOF -1


//extra for conpatible Cristian
extern unsigned char EA;
extern unsigned char ET0;
extern unsigned char RI;
extern unsigned char TR0;

#define DELAY delay(10);
#define DELAY_DISPLAY delay(2000);
#define DELAY_DISPLAY_S delay(1000);
#define SECONDS_CICLE 0.49152
//@TODO
//read 1 byte from command buffer
#define SBUF (0)
#define DEM_AUTO ((unsigned int)button_is_press(GPIO_MAG))
//cancella
#define BP_D ((unsigned int)button_is_press(GPIO_OK))
#define BP_G ((unsigned int)button_is_press(GPIO_MENU))
#define data
#define xdata
#define bit unsigned char
#define sbit signed char
#define code
//end extra

//#define HUIT_POINTS




//#define NB_TSURF 4



#define DIX_POINTS


#if defined HUIT_POINTS 
#define NB_POINTS 8
#define SX 28.0
#define SXX 140.0
#define DENOMI 336.0 //NB_POINTS*SXX-SX*SX
#endif




#if defined DIX_POINTS 
#define NB_POINTS 10
#define SX 45.0
#define SXX 285.0
#define DENOMI 825.0
#endif

#define OFFSET_ETAT 2
#define OFFSET_CDE 3

#define CODE_INIT 250
#define DELTA_TMIR 80
#define DELTA_TMIR_USER 84
#define PENTE_TMIR 208
#define PENTE_TMIR_USER 212
#define DELTA_TSURF 0
#define PENTE_TSURF 204
#define DSEUIL 4
#define POINTAGE_FLASH 90

#define DELTA_TTETE 10
#define PENTE_TTETE 14
#define PENTE_CORR_TSURF 18

#define AUTO_PRINT 188
#define MODE_CALIB 189	   	


#define GAIN_P 162 
#define GAIN_I 166
#define CONSIGNE_REGU 168
#define CONSIGNE_RAMPE 172
#define REGU_GAIN_P 196
#define REGU_GAIN_I 198
#define SEL_CONSIGNE_REGU 176

#define MEM_CONFIG 177
#define VOLUME_BIP 178
#define VOLUME_KEY 179
#define MODE_MESURE 180
#define MODE_CINETIQUE 181
#define VIT_MOTEUR 182
#define MODE_DEMARRAGE 183
#define ADR_LANGUE 184 
#define OPTION 185
#define ADR_TYPE_C 186

#define REGU_ON_OFF 187
#define ADR_IDENTIFIANT 46



#define TAB_RESULTS 0
#define TAB_REGU 1
#define TAB_HISTO 2
#define TAB_MOTEUR 3
#define TAB_MODE_MES 4
#define TAB_MODE_CINET 5
#define TAB_MODE_CALIB 6
#define TAB_DEM_AUTO 7
#define TAB_AUTO_PRINT 8
#define TAB_VOL_MES 9
#define TAB_VOL_KEY 10
#define TAB_LANGUE 11





#define ADR_VERSION 252


//#define pointage_flash_BASE 25.0

//#define BAUD9600 239

#define SECONDE_PAR_CYCLE 0.49152

#define BAUD9600 243

#define I2C_TETE 168
#define ADR_MEM 160
#define ADR_HORLOGE 208


#define MES_PPM_MAX 10
#define CAL_PPM_MAX 2
#define KONST 0.00385f

#define abs_macro(A) ((A) >= 0 ? (A) : (-(A)))
#define abs_macro_fl(A) ((A) >= 0.0 ? (A) : (-(A)))
#define etape (r_control_0 & 0x03)

#define mode_cinet (r_config & 0x07)
#define mode_mesure (r_config & 0x03)

extern union long_sp
{
	float fl;
	long l;
	unsigned long ul;
	short s[2];
	unsigned short us[2];
   signed char c[4];
   unsigned char uc[4];
};

extern union short_sp
{
	short s;
   unsigned short us;
   signed char c[2];
   unsigned char uc[2];
};

//extern union sh_sp


/*extern union ptr_sp
{
	data double * dl;
	data float * fl;
	data long * l;
	data unsigned long * ul;
	data short * s;
   data unsigned short * us;
   data signed char * c;
	data unsigned char * uc;

};*/
/*
extern union ptr_x
{
	xdata double * dl;
	xdata float * fl;
	xdata long * l;
	xdata unsigned long * ul;
	xdata short * s;                                                                      
   xdata unsigned short * us;
   xdata signed char * c;
	xdata unsigned char * uc;

};
*/



extern struct mesure
{
	unsigned long date;
	unsigned short numero;
	unsigned short aw;
	short tsurf;
	unsigned long duree;
   char etat;
	char config;
	
};

extern union fiche
{
 	struct mesure mesure;
// 	unsigned short adresse;
 	char index[16];

};




extern xdata signed char fifo_xi[TAILLE_FIFO+1];
extern xdata unsigned short fifo_aw[TAILLE_FIFO+1];
extern xdata unsigned short fifo_point[NB_POINTS+1];
//extern xdata short fifo_tsurf[8];
//extern xdata long tsurf_int;
//extern xdata char index_tsurf;


/*
extern xdata unsigned short mesure_numero[NB_MESURE+1];
extern xdata unsigned char mesure_status[NB_MESURE+1];
extern xdata unsigned char mesure_config[NB_MESURE+1];
extern xdata unsigned short mesure_aw[NB_MESURE+1];
extern xdata short mesure_surf[NB_MESURE+1];
extern xdata unsigned long mesure_duree[NB_MESURE+1];
*/
extern xdata char ptr_ascii;
extern xdata char buffer_ascii[50];

extern xdata char identifiant[16];


extern xdata union fiche tableau_result[NB_MESURE];
//extern code union fiche * data ptr_histo;

extern xdata union fiche fiche;
extern xdata unsigned short adresse_courante;
extern xdata unsigned short adresse_jour;
extern xdata unsigned short adresse_jour_suiv;
extern xdata unsigned long date_courante; 
extern xdata double time_mesure_fl;




extern code char gbx[5];
//extern code char * code txt_calibration2;
extern code short tab_liste[NB_LISTE][TAILLE_LISTE];    // numéro des sels de chaque liste disponible
extern code unsigned short tab_sel[NB_SELS][9];
extern code char * code sel_ascii[NB_SELS];
extern code char * code txt_tableau[NB_LANGUE];
extern code char * code txt_histo[NB_LANGUE];
extern code char * code txt_print[NB_LANGUE];
extern code char * code txt_delay[NB_LANGUE];
extern code char * code txt_number[NB_LANGUE];
extern code char * code txt_stat[NB_LANGUE];
//extern code char * code txt_mode[NB_LANGUE][2];
extern code char * code txt_action[NB_LANGUE][3];
extern code char * code txt_arret[NB_LANGUE][3];
extern code char * code txt_message[NB_LANGUE][29];
extern code char * code txt_etape[NB_LANGUE][4];
extern code char * code txt_variable[NB_LANGUE][6];
extern code char * code txt_menu[NB_LANGUE][NB_MENU+1];
extern code char * code txt_choix[NB_LANGUE][NB_MENU][NB_CHOIX];
extern code char val_choix[NB_MENU][NB_CHOIX];
extern code char option_menu[NB_MENU+2];
extern code char option_choix[NB_MENU][NB_CHOIX+1];




extern code char interval_point[4];
extern code unsigned short duree_max[4];
extern code unsigned short duree_min[4];
extern code char niveau_extrap[4];
extern code signed char ppm_max[3];
extern code unsigned short duree_mes[8];
extern code unsigned short interval_mes[8];
extern code unsigned short duree_cinet[8];

extern code float tab_regu[];


// TRAME

#define ETAT 0x20;

/*#define FLASH(b) etat &= b*/
#define GET_ETAT ((ERREUR<<7 | CLEAN_MIRROR<<6 | TOP_REGU<<5 | REGU_COUPELLE<<4 | MESURE_EN_COURS<<3 | CALIB<<2 | CINETIQUE<<1 | FLASH)&0xFF)
extern bit FLASH;
extern bit CINETIQUE;
extern bit CALIB;
extern bit MESURE_EN_COURS;
extern bit REGU_COUPELLE; 
extern bit TOP_REGU;
extern  bit CLEAN_MIRROR;
extern  bit ERREUR;

#define CONFIG 0x21

#define GET_CONFIG (((OPTION_H<<7 /*& 128*/) | (OPTION_L<<6 /*& 64*/) | (0<<5 /*& 32*/) | (LANGUE<<4 /*& 16*/) | (TYPE_C<<3 /*& 0x08*/) | (MODE_FLASH<<2 /*& 0x4*/) | (MODE_MES_H<<1 /*& 0x02*/) | (MODE_MES_L /*& 0x01*/))&0xFF)
#define SET_CONFIG(b) {MODE_MES_L = (b)>>0 & 0x01; MODE_MES_H = (b)>>1 & 0x01; MODE_FLASH = (b)>>2 & 0x01; TYPE_C = (b)>>3 & 0x01; LANGUE = (b)>>4 & 0x01; OPTION_L = (b)>>6 & 0x01; OPTION_H = (b)>>7 & 0x01;}
extern  bit MODE_MES_L;
extern  bit MODE_MES_H;
extern  bit MODE_FLASH;
extern  bit TYPE_C;
extern  bit LANGUE;

extern  bit OPTION_L;
extern  bit OPTION_H;


#define CONTROL_0 0x22
#define GET_CONTROL_0 ((PURGE<<7 | EQUILIBRE<<6 | DISPLAY<<5 | BUSY<<4 | ACQUISITION<<3 | CYCLE_INT<<2 | ETAPE_H<<1 | ETAPE_L)&0xFF)
extern  bit ETAPE_L;       
extern  bit ETAPE_H;       
extern  bit CYCLE_INT;
extern  bit ACQUISITION;
extern  bit BUSY;
extern  bit DISPLAY;
extern  bit EQUILIBRE;
extern  bit PURGE;

#define CONTROL_1 0x23;
#define GET_CONTROL_1 ((M_BP_D<<7 | M_BP_G<<6 | M_DEM_AUTO<<5 | DATE_SELECT<<4 | MENU<<3 | TABLEAU<<2 | INC_NUM<<1 | TSURF_MEM)&0xFF)
extern  bit TSURF_MEM;
extern  bit INC_NUM;
extern  bit TABLEAU;
extern  bit MENU;
extern  bit DATE_SELECT;
extern  bit M_DEM_AUTO;
extern  bit M_BP_G;
extern  bit M_BP_D;

#define INTERFACE 0x24;
#define GET_INTERFACE ((SEND_MESURE<<5 | CODE_OK<<4 | DATA_TX<<3 | TRANSFERT<<2 | TRANS_PARAM<<1 | ECHO_CDE)&0xFF)
extern  bit ECHO_CDE;
extern  bit TRANS_PARAM;
extern  bit TRANSFERT;
extern  bit DATA_TX;
extern  bit CODE_OK;
extern  bit SEND_MESURE;


#define OPTIMIZE 0x25;
#define GET_OPTIMIZE ((NOUVEAU_POINT<<7 | ACQUISITION_POINT<<6 | BOOST_H<<5 | BOOST_L<<4 | XI_POS<<3 | XI_NEG<<2 | XI_DERIVE_POS<<1 | XI_DERIVE_NEG)&0xFF)
extern bit XI_DERIVE_NEG;
extern bit XI_DERIVE_POS;
extern bit XI_NEG;
extern bit XI_POS;
extern bit BOOST_L;
extern bit BOOST_H;
extern bit ACQUISITION_POINT;
extern bit NOUVEAU_POINT;



extern data short peltier;
extern data signed char moteur_bip;
extern data char val_can;
extern data char val_char;
extern data short regu;

#define r_etat GET_ETAT
#define r_config GET_CONFIG
#define r_control_0 GET_CONTROL_0
#define r_control_1 GET_CONTROL_1
#define r_interface GET_INTERFACE
#define r_optimize GET_OPTIMIZE

//extern data char etat;
extern data char commande;
//extern data char config;

//extern data char control_0;
//extern data char control_1;
//extern data char interface;

extern data unsigned short ptr_asm;
extern data char val_asm;


extern data float tmir;
extern xdata float delta_tmir;
extern xdata float pente_tmir;
extern data float tsurf;
extern xdata float delta_tsurf;
extern xdata float pente_tsurf;
extern xdata float reflexion;
extern xdata float detection;
extern xdata unsigned short aw_inst;
extern xdata unsigned short aw_flash;
extern xdata unsigned short aw_finale;
extern xdata unsigned short aw_int;
#define tsurf_mem aw_int


extern xdata char mode_essai;



extern data float facteur_p_cent;
extern data float xi;
extern data short xi_int;
extern data short xi_pel;



extern data short time_etape;
extern data short time_point;
extern xdata unsigned long time_mesure;
extern xdata unsigned long time_system;

extern xdata unsigned short numero_mesure;

extern data char dseuil; 
extern data char pointage_flash;
extern data char num_tache;
//extern data char index;

extern data char nb_point;
extern data signed char ppm_sec;

extern data char val_char;
extern data union long_sp buffer_tx;
extern data union long_sp buffer_rx;
//extern data union ptr_sp ptr;

extern data unsigned char gain_p;
extern data unsigned char gain_i;
extern data union ptr_x ptx;


extern data union short_sp val;

extern data char ligne_select;
extern data char nb_mesure;

extern data unsigned short aw_moy;

extern xdata unsigned long date_courante;

extern xdata char volume_key;
extern xdata char nb_erreur;

extern xdata short val_regu;
extern xdata float consigne_regu;
//xdata float temperature_mesuree_peltier;
extern xdata float consigne_rampe;
extern xdata float mesure_rampe;
extern xdata long regu_correction_P;
extern xdata long regu_correction_I;
extern xdata short regu_gain_P;
extern xdata short regu_gain_I;


extern xdata float ttete;
extern xdata float delta_ttete;
extern xdata float pente_ttete;


extern xdata char regu_setting;
extern xdata float pente_correction_tsurf;
extern xdata char type_capteur;

extern xdata char entete_ok;


extern xdata unsigned char mode_calibration;
extern xdata char num_pt_calib;

extern xdata float tsurf_1;
extern xdata float tsurf_2;
extern xdata unsigned short aw_mes_1;
extern xdata unsigned short aw_mes_2;
extern xdata unsigned short aw_rech_1;
extern xdata unsigned short aw_rech_2;

extern xdata unsigned short aw_finale_brut;
extern xdata unsigned short aw_moy_brut;

extern xdata char message;

void Start(void);

void auto_noyau(void);
void tempo_init(void);
void reset_on(void);
void reset_off(void);
void sort_moteur_bip(void);

void rech_rapide(void);
void regu_miroir(void);
void regu_reflexion(void);
void acqui_miroir(void);
void acqui_surface(void);
void acqui_reflexion(void);

void switch_tmir(void);
void switch_tsurf(void);
void switch_reflexion(void);
void switch_can(char);


void rien(void);
void gestion_noyau(unsigned long);
void noyau_temps_reel(void);
void calc_aw(void);
void calc_aw_finale(void);
void calib_auto(void);
void fin_mesure(void);

void init_etape_attente(void);
void init_etape_equilibre(void);
void init_etape_recherche(void);
void init_etape_mesure(void);

void gestion_demarrage(void);
void gestion_equilibre(void);
void determine_commande(void);
void gestion_commande(void);
void set_offset_miroir(void);
void set_pente_miroir(void);
void set_offset_surface(void);
void set_pente_surface(void);
void set_seuil_detection(void);
void arret_utilisateur(void);
void mesure_std(void);
void cinetique_std(void);
void calib_auto_std(void);
void erreur_cde(void);
void transfert_on(void);
void transfert_off(void);
void bip(char ,char ,char);

void set_config(void);
void volume_key_down(void);
void volume_key_up(void);
void volume_bip_down(void);
void volume_bip_up(void);

void moteur_on(void);
void moteur_off(void);
void recup_config(void);


void envoi_buffer(void);

//void param(union ptr_sp p_param,char taille);

void ecritI2C(char adrchip,char adrmem,char valeur);
char litI2C(char adrchip,char adrmem);

float recup_float(char adrchip,char adr);
short recup_short(char adrchip,char adr);
void sauve_float(char adrchip,char adr, float valeur);
void sauve_short(char adrchip,char adr, short valeur);
void recup_txt(char adrchip,char adr, char * adr_destination);
void sauve_txt(char adrchip,char adr, char * adr_source);

//void conv_fl_str(union ptr_sp val,char * a,char nbdecimale);


void check_bp(void);
void recup_config(void);
void set_gain(void);
void set_volume_key(void);
void set_volume_bip(void);
void set_vitesse_moteur(void);

void set_char(void);
void set_short(void);

void gestion_bp(void);
void gestion_demarrage_auto(void);


void passerelle_isp(void);

void gestion_menu(void);
void gestion_choix(void);

void moteur(char);
void message_arret(void);

void affiche_ligne_tableau(char index_ligne, char ligne);

void set_identifiant(void);
void efface_tableau(void);


void rech_fin_enregistrement(void);
void write_histo(void); 
void read_histo(char);



void read_enregistrement(union fiche * ptr,unsigned short);
void write_enregistrement(union fiche * ptr,unsigned short);
unsigned short read_enregistrement_suiv(union fiche * ptr,unsigned short);
unsigned short read_enregistrement_prec(union fiche * ptr,unsigned short);



void select_date(void);

void affiche_ligne_histo(union fiche * ptr, char ligne);
void affiche_histo(void);
unsigned short inc_fifo(unsigned short);
unsigned short dec_fifo(unsigned short);



char * calc_date (unsigned long date);
void efface_histo(void);

void set_long(void);
void transfert_histo(void);
void transfert_tableau(void);

char bcd_hexa(char valeur,char masque);
char hexa_bcd(char valeur,char masque);

void sim_histo(void);
void formate_mesure(void);

time_t recup_date(void);
void set_date(time_t);
void gection_evenement(void);
void update_soft(void);
void annulation_calibration(void);
void stat_histo(char);


char * txt_ascii(char *);
char * data_ascii(char);
char * date_ascii(unsigned long);
char * time_ascii(unsigned long);
char * delay_ascii(unsigned long);
char * delay_ascii_2(unsigned long);
char * float_ascii(float,char);
char * ushort_ascii(unsigned short);

void data_printer(char);
void txt_printer(char *);
void imprime_entete(void);
void imprime_fiche(void);
void imprime_colonne(void);

void auto_print(void);
void imprime_histo(void);
void imprime_tableau(void);
void reglage_date(void);

void affiche_date_courante(void);

void trame_EC(void);
void trame_AW(void);
void trame_FI(unsigned short);
void transfert_pc(void);

void start_regu(void);
void stop_regu(void);
void select_consigne_regu(void);
void affiche_regu_temp(void);
void regulation_peltier(void);
void calcul_pente_temperature(void);

void init_carte(void);

void regu_setting_on(void);
void regu_setting_off(void);

void etalonnage_tmir_aw(void);


void calcul_calib_1(void);
void calcul_calib_2(void);
void rech_sel(void);

int read_hw_ini_file(void);
double pt_temp_single_read(void);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif