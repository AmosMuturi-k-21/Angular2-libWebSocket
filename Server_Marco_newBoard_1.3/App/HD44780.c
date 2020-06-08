#include "HD44780.h"
#include <string.h>


bit test_display(void)   // test si presence d'un afficheur
{
#ifdef OLD_MODE_LCD
	return 1;
#else
	return 0;
#endif
}

bit test_busy(void)      // test afficheur pret
{
	return 0;
}

void wait_busy(void)     // attente afficheur pret avec watch dog 
{
}

void cde_display(int donnee)   // ecriture registre commande de l'afficheur
{
	//printf("display cmd = %d\n",donnee);

	if (donnee == CLEAR) {
		lcd_clear();
	}else
		lcd_setcursor(donnee);
	//writeCommand(donnee);
}

void data_display(char donnee)   // write data register of the display
{ 
	lcd_putchar(donnee);
	lcd_fflush();
}

/*char read_display(void)         // lecture registre data de l'afficheur
{
	EA=0;
	wait_busy();
	RD=1;
	WR=1;
	P0=0xff;
	EXT_CS=1;
   buffer_rx.uc[0]=P0;
	buffer_rx.uc[0]=P0;
	buffer_rx.uc[0]=P0;
	EXT_CS=0;
   RD=1;
   WR=1;
   EA=1;
   return(buffer_rx.uc[0]);
}*/







void init_display(void)
{
   //wait_busy();
	/*cde_display(0x38);//FUNCTION SET mode 8 bits 1 ligne 
   //wait_busy();
   cde_display(0x01);//DISPLAY CLEAR
   //wait_busy();
	cde_display(0x0c);//DISPLAY CONTROL  display on , cursor off, no blink
	//wait_busy();
	cde_display(0x06);//ENTRY MODE SET increment , no shift
	*/

}

void txt_display(char *p_char)     // envoi chaine de caracteres sur l'afficheur
{
	while(*p_char!=0)
	{
	 	//wait_busy();
		lcd_putchar(*p_char);
	 	p_char++;
	}
	lcd_fflush();
}


void efface_ligne(char ligne)             // effacement de 20 caracteres continu
{
	lcd_setcursor(ligne);
	txt_display("                    ");
	//txt_display("--------------------");
}		
	


void affiche_ligne(char * p_char, xdata char ligne)     // affichage chaine de caracteres avec attribut commande
{
	lcd_setcursor(ligne);
	txt_display(p_char);
}

void affiche_titre(char * p_char, xdata char ligne)    // affichage chaine de caracteres avec centrage et attribut commande
{
	lcd_setcursor(ligne+10-strlen(p_char)/2);
	txt_display(p_char);
}












