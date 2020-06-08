

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

	//// OPTIONS DE COMPILATION DU LOGICIEL EMBARQUE

#define FRANCAIS	// langues francais/anglais
	//#define ALLEMAND  // langues allemand/anglais


	//#define NO_MENU_CALIB  // permet de supprimer le menu calibration (verrouillage mode), on peut alors changer le mode de calibration
	// qu'en passant l'appareil en mode spécial par la commande 22 valeur 70 (char), ca donne acces à tous les menus, puis remmettre en mode 
	// "normal" : cde 22 valeur 67 (prosoft), valeur 68 (elite),  


	//#define SLOW_PRINT  // pour ralentir la cadence des données envoyées à l'imprimante 

#ifdef __cplusplus
} // closing brace for extern "C"
#endif