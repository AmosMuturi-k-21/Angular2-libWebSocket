#pragma once

#define  MRXA 0
#define  CSRA 1
#define  CRA  2
#define  THRA 3
#define  ACR  4
#define  IMR  5
#define  CRUR 6
#define  CTLR 7
#define  MRXB 8
#define  CSRB 9
#define  CRB  10
#define  THRB 11
// reserved      =12 
#define  OPCR 13
#define  SOPB 14
#define  ROPB 15


//registres accessible en lecture
#define  SRA 1
#define  RHRA 3
#define  ISR  5
#define  SRB 9
#define  RHRB 11
#define  IPCR 4 /*Input Port Change Register */ 

char lit(char);
void ecrit(char,char);

void init_SCC2692(void);

//char emet_bal_pret(void);
//void emet_balance(char donnee);
//char char_bal_recu(void);
//char lit_balance(void);

//char emet_pc_pret(void);
//void emet_pc(char donnee);
//char char_pc_recu(void);
//char lit_pc(void);


#define emet_pc_pret() (lit(SRB)&4)
#define emet_bal_pret() (lit(SRA)&4)
#define char_pc_recu() (lit(SRB)&1)
#define char_bal_recu() (lit(SRA)&1)
#define lit_balance() lit(RHRA)
#define emet_balance(A)  ecrit(THRA,A)
#define lit_pc() lit(RHRB)
#define emet_pc(A)  ecrit(THRB,A)


void envoi_pc(char donnee);
void envoi_bal(char donnee);
char recoit_pc(void);
char recoit_bal(void);


void send_pc(char * texte);
void send_bal(char * texte);

void set_ttl1(void);
void reset_ttl1(void);
void set_ttl2(void);
void reset_ttl2(void);
char lire_count_burette(void);/* Lecture de la sortie comptage de la burette cablée sur IP2*/



