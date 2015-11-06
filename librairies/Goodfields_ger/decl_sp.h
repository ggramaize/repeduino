// OBSOLETE - voir modules ger_...
// déclarations de variables globales propres à sp.h
// 23/07/2013
//************************************
// *** TEMPS *** gestion du temps
int top_1=0;             // nb de ms pour traitements au 1/1000 seconde dans le P/P
int top_10=0;            // nb de ms pour traitements au 1/100 seconde dans le P/P
int top_100ms=false;     // top pour traitements au 1/10 seconde dans le P/P
// int top_250ms=false;     // top pour traitements au 1/4 seconde dans le P/P
// int top_500ms=false;     // top pour traitements au 1/2 seconde dans le P/P
#ifndef ger_top_light 
int top_1000ms=false;    // top pour traitements à la seconde dans le P/P
int top_60000ms=false;   // top pour traitements à la minute dans le P/P
int top_55s=false;   	 // top pour traitements 5 secondes avant la minute dans le P/P
int milliseconde;        // millisecondes depuis début de seconde
int previousSecond;      // mémoire secondes de l'horloge
#endif
unsigned long previous1, previous10, previous100;
// unsigned long previous250, previous500;
char* jours[] = {"Dim","Lun","Mar","Mer","Jeu","Ven","Sam"} ;

// pour gérer les tempos faire pour les tempos 1ms, 10ms : 
// if (top_10) {
//   GER_TEMPO (tempo_X,top_10)
//   top_10 = 0;
// }
#define GER_TEMPO(tempo,top) if (tempo > top ) { tempo -= top; } else { tempo = 0; }
// pour gérer les tempos plus lentes : 
// if (top_1000ms) {
//    top_1000ms = false;
//   GER_TEMPOL (tempo_X)
// }
#define GER_TEMPOL(tempo) if (tempo > 0 ) { tempo --; }
	
/*
int horloge_synchronisee=0;  // 1=indique si la mise à  l'heure a été effectuée
int horloge_synchro_tempo=0;
int horloge_synchro_tempo_reset=61; // temps en minutes après lequel l'horloge est supposée non synchronisée
*/

int gmt_decalage=-2;    // heure d'été
//int gmt_decalage=-1;    // heure d'hiver

//************************************
// *** LECTURE et CONVERSION d'entrées analogiques ***
// cette MACRO lit l'entree_U', multiplie par 'coef_U' et rends le résultat en float dans 'U'
// si on a un débordement, on retourne une valeur très grande (999999.) pour avoir un affichage d'erreur
#define GER_ANAREAD(U,entree_U,coef_U)  U = analogRead(entree_U);  if (U > 1022) {U=999999.;} else {U = coef_U * U;}

//************************************
// *** CLAVIER *** clavier d'entrée
// clavier à touches et diodes sur entrée analogique
// ATTENTION ! definir l'entrée clavier si on veut qu'il soit géré : #define ger_clavier A0	// entrée ana du clavier

#ifdef ger_clavier

// clavier par défaut si on ne définit rien : 10 touches à diodes BAT47/54 tirage pullup seul ou 10-20kOhms
#ifndef ger_ntouche_nb
#define ger_ntouche_nb 3          // nombre de memoires anti-rebond
#endif

#ifndef ger_clavier_nb
#define ger_clavier_nb 10			// nombre de touches
#endif

#ifndef ger_clavier_delta
#define ger_clavier_delta 41		// écart entre valeurs
#endif

#ifndef LISTE_TOUCHE
#define LISTE_TOUCHE() char ltouche[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' '};
#endif

int itouche,vtouche,ntouche,tempo_repeat;
int ntouche_mem[ger_ntouche_nb];      // mémoires pour anti-rebond
// touche renvoyée
char touche=' ',touche_repeat=' ';

/*

// clavier 5 touches avec 6 diodes type COURBON NOIR (1° touche inexistante en général)
#define ger_clavier A0
#define ger_ntouche_nb 6
#define ger_clavier_nb 6
#define ger_clavier_delta 109
//#define LISTE_TOUCHE() char ltouche[] = {' ', '3', '4', '2', '1', '5', ' '}; // 1=AUX 2=INV 3=v 4=^ 5=VAL
// +- compatible 10 touches les 5 touches correspondent aux 4 centrales d'un clavier 10 touches + VAL=touche '0'
#define LISTE_TOUCHE() char ltouche[] =  {' ', '5', '6', '4', '3', '0', ' '}; 
// 0,  118, 217, 318, 408, 498, 1023 valeurs expérimentales tirage par la pullup seulement
//delta=118  108  106  102  100 => 109
// 0,  120, 221, 319, 414, 506, 1023 valeurs expérimentales tirage par 10Kohms
//delta=120  110  106  103  101 => 110

// CLAVIER 10 TOUCHES A DIODES 0,2V
#define ger_clavier A0
#define ger_ntouche_nb 6
#define ger_clavier_nb 10
#define ger_clavier_delta 41
#define LISTE_TOUCHE() char ltouche[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' '};
// 0,  61, 108, 154, 199, 243, 287, 330, 372, 414, 456, 1023 valeurs expérimentales II
// 0,  45,  90, 136, 181, 226, 270, 313, 354, 396, 437, 1023 valeurs expérimentales I
// 0,  41,  82, 123, 164, 205, 246, 287, 328, 369, 410, 1023 valeur *41      

// CLAVIER 4+16 TOUCHES A DIODES 0,2V
#define ger_clavier A0
#define ger_ntouche_nb 6
#define ger_clavier_nb 20
#define ger_clavier_delta 41
#define LISTE_TOUCHE() char ltouche[] = {'T', 'U', 'V', 'W', '*', '0', '#', 'D', '7', '8', '9', 'C', '4', '5', '6', 'B', '1', '2', '3', 'A'};    // nbtouche

*/
// macro de définition des touches retournées
LISTE_TOUCHE()

#endif
//************************************
// Reception Serial et/ou I2C
#define ger_msg_size 33
// assez bizarrement, il faut initialiser la variable suivante, sinon ca marche pas !!!
char ger_msg[ger_msg_size] = {'V','i','d','e',0};
int ger_msg_recu=0;
// int affiche_msg_recu_lcd = 0; // indique à msg_traitement d'afficher les 'A' sur le lcd
// reception serial avec timeout
char ger_inByte;
#ifndef ger_serial_timeout
#define ger_serial_timeout 50    //ms
#endif
//unsigned long ger_serial_last_ms_char;    // pour timeout
//int jmsg=0;  // pointeur remplissage

// mise à l'heure (cf msg_traitement.h)
  int hmsdmy[6];

//************************************
// Constantes sympas avec une précision suffisante (Arduino gère 7 à 8 decimales)
// Pi = 3.14159 26535 89793 23846 26433 83279 50288 41971 69399 37510 58209 74944 59230 78164 06286 20899 86280 34825 34211 70679 82148 08651 ...
// à peu près 2 Pi
#ifndef deuxpi
#define deuxpi TWO_PI // 6.28318530 //6,283185307179586476925286766559  
#endif
// racine carree de 2
#ifndef racine2
#define racine2 1.414213562 //1,4142135623730950488016887242097
#endif
// MUSIQUE
// rapport de fréquence entre 2 notes séparées d'un demi-ton = 2 puissance 1/12 
#ifndef demiton
#define demiton 1.059463093 //1,0594630943592952645618252949463
#endif

/*
Il existe en standard :

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
*/

/*
// rapport de fréquence entre 2 notes séparées d'un SAVART = 10 puissance 1/1000
// il y a 25 savarts entre 2 notes séparées d'un demi-ton, 301,03 SAVARTS dans un octave
#ifndef savart
#define savart 1.002305238 // 1.0023052380778996719154048893281 
#endif
// rapport de fréquence entre 2 notes séparées d'un CENT = 2 puissance 1/1200 
// il y a 100 cents entre 2 notes séparées d'un demi-ton, 1200 CENTS dans un octave
#ifndef savart_cent
#define savart_cent 1.0005777895 // 1,0005777895065548592967925757932
#endif
// NB : l'oreille humaine peut détecter environ 0,3% de variation = 1,3 SAVARTS = 5 CENTS = 1,0028922878693670715023923823933
// soit environ 20 niveaux détectables entre 2 notes séparées d'un demi-ton.
// en fait, pour que cela soit facilement décelable, je prends 4 SAVARTS = 16 CENTS
// Voir : http://fr.wikipedia.org/wiki/Cent_et_savart
*/
#ifndef savart_mini
#define savart_mini 1.009284801 // 1,0092848012118741755557384622396 // 16 CENTS
//#define savart_mini 1.008701983 // 1.0087019837903990132289428069278 // 15 CENTS
#endif
//************************************



