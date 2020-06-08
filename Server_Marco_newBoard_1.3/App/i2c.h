#pragma once
#ifdef __cplusplus
extern "C" {
#endif

//#define DEBUG_RW

	void envoiI2C_rapide(char octet);
	char recoitI2C_rapide(void);

	void startI2C(void);
	void stopI2C(void);
	void slave_ack(void);
	void master_ack(void);
	void master_noack(void);

	void envoiI2C(char octet);
	char recoitI2C(void);
	void tempo_ecriture(void);
	void tempoI2C(void);
	void ecritI2C(char adrchip, char adrmem, char valeur);
	char litI2C(char adrchip, char adrmem);
	float  recup_float(char adrchip, char adr);
	short recup_short(char adrchip, char adr);
	void sauve_float(char adrchip, char adr, float valeur);
	void sauve_short(char adrchip, char adr, short valeur);
	void sauve_txt(char adrchip, char adr, char * adr_source);
	void recup_txt(char adrchip, char adr, char * adr_destination);

#ifdef __cplusplus
} // closing brace for extern "C"
#endif

