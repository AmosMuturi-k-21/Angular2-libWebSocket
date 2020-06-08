#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "proto.h"
#include "lcd.h"

	bit test_display(void);
	bit test_busy(void);
	void cde_display(int donnee);
	void data_display(char donnee);
	//char read_display(void);   
	void wait_busy(void);
	void init_display(void);
	void txt_display(char *p_char);
	void logo_star_wars(void);

	void affiche_ligne(char *, char);
	void affiche_titre(char *, char);
	void efface_ligne(char);
	void time_display(union ptr_sp val);
	void delay_display(union ptr_sp val);

	void delay_display_2(union ptr_sp val);
	void float_display(union ptr_sp val, char nbdecimale);
	void ushort_display(union ptr_sp val);




#define L0 (LCD_COL * 0)
#define L1 (LCD_COL * 1)
#define L2 (LCD_COL * 2)
#define L3 (LCD_COL * 3)

#define CLEAR -1


#ifdef __cplusplus
} // closing brace for extern "C"
#endif









