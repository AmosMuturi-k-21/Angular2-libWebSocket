
#pragma once

/*
LOGICIEL EMBARQUE POUR TOUS MODELES DE FAST-LAB: Version actuelle compilée le 19/06/13 Fichier "FASTLAB V362.HEX"

HISTORIQUE DES VERSIONS:

V3.62: Correction d'un bug, la pente et l'offset miroir n'étaient plus réglable par le logiciel PC depuis la V3.60, problème passé inapercu jusque la.
V3.61: Correction problème de plantage lors de l'utilisation du logiciel PC, problème due au dépassement de pile systeme, résolu en laissant plus d'espace à la pile. 
V3.60: Ajout d'un menu calibration avec possibilité de calibrer en 2 points + de nombreuses possibilités de calibration sur sels saturés ou non, voir fichier "Manuel FA-st Lab-français v3.1.docx"
V3.50: Prise en charge de la régulation par effet peltier V2, voir fichier "Doc regu FastLab V2.1.docx"
V3.40: Mise en place du logiciel embarqué sur la nouvelle filière de developpement Raisonnance kit 6.1


PROTOCOLE DE COMMUNICATION PC <-> Flex333 avec logiciel embarqué fast v 3.60 et ultérieur

4 types de trame sont utilises:
    - 1 trame de taille variable (trame cde) comprenant eventuellement 1 parametre de taille variable
    - 1 trame (trame echo) de 4 caracteres est envoyee apres chaque commande
      y comprit la commande TRANSFERT_ON
    - 1 trame (trame info) de 40 caracteres comprenant diverses informations est envoyée de maniere
      repetitive aprés une commande TRANSFERT_ON 
    - 1 trame (trame resultat) de 40 caracteres compremant le resultat de la mesure
      ainsi que l'identifiant de l'appareil

EN EMMISION cote PC:

TRAME CDE       | G  | B  | X  |Cde |Parametre                 |
Taille          | 1  | 1  | 1  |  1 | 0 à 4 ou chaine de carac.|
Type            |char|char|char|char|variable                  |
Valeur          |'G' |'B' |'X' |    |                          |



EN RECEPTION cote PC:


TRAME ECHO      | E  | C  |Etat  |Cde |
Taille          | 1  | 1  |  1   | 1  |
Type            |char|char|8 bits|char|
Valeur          |'E' |'C' |      |    |

TRAME INFO      | A  | W  |Etat  |Config| ...
Taille          | 1  | 1  |  1   | 1    |
Type            |char|char|8 bits| char |
Valeur          |'A' |'W' |      |      |


TRAME RESULTAT  | F  | I  |Etat|Config|identifiant | date |numero|  AW  | tsurf|duree|extension    
Taille          | 1  | 1  |  1 |   1  |     16     |   4  |  2   |  2   |   2  |  4  |   6 	
Type            |char|char|8 b | 8 b  |chaine carac|ulong |ushort|ushort|ushort|ulong|non utilise
Valeur          |'F' |'I' |    |      |            |codage|      |X1000 | X100 |sec. |








Structure de la variable Etat:

numero bit    |   7    |  6           |   5     |    4   |     3         |    2    |    1     |    0    |
booleen       | ERREUR | CLEAN_MIRROR |         |        |MESURE_EN_COURS| CALIB   |CINETIQUE | FLASH   |

ERREUR:     0 pas d'erreur, 1 la commande en cours ne s'execute pas normalement
CLEAN_MIRROR: 1 le miroir doit être nettoye
CALIB: 1 calibration en cours
MESURE_EN_COURS: 0 l'appareil est en stand-by 
CINETIQUE: 1 cinetique en cour
FLASH: 1 C'est une mesure Flash


Structure de la variable Config:

numero bit     |   7     |  6       |   5      |    4     |     3     |    2       |    1       |    0       |
booleen        |OPTION_H | OPTION_L |          | LANGUE   |  TYPE_C   | MODE_FLASH | MODE_MES_H | MODE_MES_L |

TYPE_C: temp echantillon: 0 capteur infrarouge, 1 sonde platine
OPTION_H OPTION_L: 0 ou 1 light, 2 prosoft, 3 elite
LANGUE: 0 francais ou allemand, 1 anglais

MODE_FLASH MODE_MES_H MODE_MES_L: mode de 0 a 7 


*/
#define NB_CDE 42

// liste des commandes disponibles


#define ATTENTE 0
#define ARRET 1                   // arret mesure
#define MESURE_ON 2               // lancement mesure 
#define CINETIQUE_ON 3            // lancement cinetique

#define CALIB_AUTO_ON 5           // lancement calibration automatique 

#define PASSERELLE_ISP 4          // lancement routine de reprogrammation par liaison serie

#define SET_GAIN_P 6              // param char   0 a 255
#define SET_GAIN_I 7              // param char   0 a 255

#define TRANSFERT_OFF 8
#define TRANSFERT_ON 9  

#define SET_OFFSET_MIROIR 10      // param short en 1/100 °celsius
#define SET_PENTE_MIROIR  11      // param short en 1/1000
#define SET_OFFSET_SURFACE 12     // param short en 1/100 °celsius
#define SET_PENTE_SURFACE  13     // param short en 1/1000
 #define SET_SEUIL_DETECTION 14    // param char   0 a 255 soit 0 a 25.5 %

#define SET_VIT_MOTEUR 15         // param char   1 a 3 valeurs preprogrammer si autre valeur,
                                  // celle-ci est directement envoye sur le cna
#define SET_MODE_MESURE 16        // param char   0 a 7
#define SET_MODE_CINETIQUE 17     // param char   0 a 7
#define SET_MODE_DEMARRAGE 18     // param char   0 a 4
#define SET_VOLUME_BIP 19         // param char   0 a 5
#define SET_VOLUME_KEY 20         // param char   0 a 5
#define SET_LANGUE 21             // param char   0 a 1
#define SET_OPTION 22             // param char   'A' a 'Z'   A et B light , C prosoft, D elite, E servive
#define SET_TYPE_C 23             // param char   'A' a 'Z'   A capteur classique, B sonde de profondeur
#define SET_CODE_ACCES 24         // param short pour passage prosoft-elite idem FAST/1 voir reglage FAST2000
#define SET_IDENTIFIANT 25        // chaine de caracteres max 15 carateres doit comportee les 7 chiffres du numero de serie
#define SET_DATE 26               // param long reglage date/heure

                                  // CODAGE
                                  // ( Annee - 2000 ) * 44236800 + Mois * 2764800 + Jour * 86400 + Heure * 3600 + Mn * 60 + Sec
                                  // DECODAGE
                                  // Annee   = (param / 44236800) + 2000
                                  // Mois    = (param / 2764800) % 16;
                                  // Jour    = (param / 86400) % 32;
                                  // Heure   = (param / 3600) % 24;
                                  // Minute  = (param / 60) % 60;
                                  // Seconde =  param % 60;
                                  
#define SET_DATE_DEBUT 27         // param long date de debut de transfert codage idem SET_DATE
#define SET_DATE_FIN 28           // param long date de fin de transfert codage idem SET_DATE 

#define TRANSFERT_HISTORIQUE 29
#define EFFACE_HISTORIQUE 30
#define SIM_HISTO 31

#define STOP_REGU 32 
#define START_REGU 33

#define SET_CONSIGNE_REGU 34      // parametre short, en 1/100 degree °C, exemple valeur 2500 => 25°C 
#define SET_RAMPE_REGU 35         // parametre short, en 1/100 degree °C / mn, valeur defaut 5000

#define SET_REGU_GAIN_P 36        // parametre short, valeur defaut -1500
#define SET_REGU_GAIN_I 37        // parametre short, valeur defaut -100

#define REGU_SETTING_OFF 38       // permet de revenir a la normale apres commande 39  
#define REGU_SETTING_ON 39        // permet de regler les parametres offset et pente temperature tete et pente correction voir fichier "Doc regu FastLab V2.1.docx"

#define SET_MODE_CALIB 40         // param type char, permet de selectionner un mode de calibration: valeurs autotisées 0,1,2,3,4,5 (1 point) et 128,129,130,131,132,133 (2 points)
                                  // utile dans le cas ou le menu calibration est rendu inaccessible par l'option #define NO_MENU_CALIB 

#define ERREUR_CDE 41


