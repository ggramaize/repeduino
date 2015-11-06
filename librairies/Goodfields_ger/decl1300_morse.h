// ******************
// *** GoodFields ***
// ******************

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

// *** MORSE *** V10.03.2014
/////////////// Simple Arduino CW //////////////////
// Written by Mark VandeWettering K6HX
// Email: k...@arrl.net
//
// Thx Mark de ON7EQ !
// http://www.qsl.net/on7eq/en/index.htm
/*
Codage tres astucieux !!!
Les patterns sont faits comme suit :
 - Lecture de droite a gauche 
 - bit = 0 = . dit
 - bit = 1 = - dash
 - on rajoute un bit à 1 pour finir
Lecture : 
a/ on fait d'abord p=morsetab[i].pat 
b/ si p = 1 on Arrête
c/ si le bit de poids faible de p (à droite) = 1 = dash = trait
d/ si le bit de poids faible de p(à droite) = 0 = dot = point
e/ on décale à droite de 1 bit, et on retourne en b/
exemples :
	{'C', 21},  // -.-. 0001 0101
  0001 010 1 = dash -
  0000 101 0 = dot  -.
  0000 010 1 = dash -.-
  0000 001 0 = dot  -.-.
  0000 000 1 = 1 = STOP
	{'Q', 27},	// --.- 0001 1011
  0001 101 1 = dash -
  0000 110 1 = dash --
  0000 011 0 = dot  --.
  0000 001 1 = dash --.-
  0000 000 1 = 1 = STOP
... F1FWG
*/
#ifndef decl1300_morse
#define decl1300_morse OK
struct t_mtab { char c, pat; } ;
struct t_mtab morsetab[] = {
  	{'.', 106},     // .-.-.- 0110 1010
	{'1', 62},	// -.... 0011 1110
	{'2', 60},      // --... 0011 1100
	{'3', 56},      // ---.. 0011 1000
	{'4', 48},      // ----. 0011 0000
	{'5', 32},      // ..... 0010 0000
	{'6', 33},      // -.... 0010 0001
	{'7', 35},      // --... 0010 0011
	{'8', 39},      // ---.. 0010 0111
	{'9', 47},      // ----. 0010 1111
	{'0', 63},      // ----- 0011 1111
	{'A', 6},	// .-	0000 0110
	{'B', 17},      // -... 0001 0001
	{'C', 21},	// -.-.	0001 0101
	{'D', 9},       // -..  0000 1001
	{'E', 2},       // .    0000 0010
	{'F', 20},      // ..-. 0001 0100
	{'G', 11},      // --.  0000 1011
	{'H', 16},      // .... 0001 0000
	{'I', 4},       // ..   0000 0100 
	{'J', 30},      // .--- 0001 1110
	{'K', 13},	// _._	0000 1101
	{'L', 18},      // .-.. 0001 0010
	{'M', 7},	// --	0000 0111
	{'N', 5},       // -.   0000 0101
	{'O', 15},      // ---  0000 1111
	{'P', 22},      // .--. 0001 0110
	{'Q', 27},	// --.-	0001 1011
	{'R', 10},      // ._.	0000 1010
	{'S', 8},       // ...  0000 1000
	{'T', 3},	// _	0000 0011
	{'U', 12},      // ..-  0000 1100
	{'V', 24},	// ...-	0001 1000
	{'W', 14},      // .--  0000 1110
	{'X', 25},      // -..- 0001 1001
	{'Y', 29},      // -.-- 0001 1101
	{'Z', 19},      // --.. 0001 0011
	{'!', B01110101},       // -.-.--
	{'=', B00110001},       // -...-
	{'-', B01100001},       // -....-
	{'+', B00101010},       // .-.-.
	{'#', B01000000},       // ......  ERREUR 6 ou 7 points ?
	{'#', B10000000},       // .......  ERREUR 6 ou 7 points ?
	{'$', B01101000},       // ...-.-  VA fin de de transmission
	{'*', B00100010},       // .-...  QRX attente
	{':', B01000111},       // ---...
    {'\'', B01011110},       // .----. cote = '
	{',', 115},      // --..-- 0111 0011
	{'/', 41},       // -..-.  0010 1001
	{'?', 76}        // ..--.. 0100 1100
        } ;

#define CW_N_MORSE  (sizeof(morsetab)/sizeof(morsetab[0]))
int CW_PITCH=704;               				// frequence du morse TIH = POINT(704 = FA4)
int CW_PITCH_TAAH=CW_PITCH/savart_mini;         // frequence du morse TAAH = TRAIT (- x%)
int CW_SPEED=13;		        				// en mots/minute (15=80ms le point)
int CW_DOTLEN=92; // 1200/CW_SPEED;					// . en ms
int CW_DASHLEN_PROP=30;  						// en dixièmes CW weight original 3 à 3.5
int CW_CARSPACE_PROP=90;  						// en dixièmes = normalement 7 DOT
int CW_DASHLEN=277; // CW_DASHLEN_PROP*CW_DOTLEN/10;    // _ en ms
int CW_CARSPACE=828 ;// (CW_CARSPACE_PROP - 10)*CW_DOTLEN/10;  								//
/* calculs selon SPEED (fait dans étape 1 du grafcet morse ou si modification):
    CW_SPEED = 13;
    CW_DOTLEN=1200/CW_SPEED;    // en millisecondes
    CW_DASHLEN=CW_DASHLEN_PROP*CW_DOTLEN/10;   // en millisecondes
    CW_CARSPACE=(CW_CARSPACE_PROP - 10)*CW_DOTLEN/10;
*/
#define CW_AMORTISSEUR 5 // vitesse d'amortissement de la variation de vitesse

int grafcet_cw=1;    // armé en attente à priori
unsigned int tempo_cw;
//int tempo_cw3;
unsigned int tempo_attcw;
#define tempo_attcwval 2000
char cw, cwcw=0;    // caractère en cours, cw ou musique
unsigned char pw;  // pattern en cours (unsigned pour ne pas avoir de problème avec le shift >> 1 qui doit finir par 1)
int  cw_pos;       // position en cours d'émission dans le buffer 'cw_texte'
char cw_texte[30];
char cw_texte_att[15],cw_texte_old[15];

// *** DECOdeur MORSE ***
#define seuil_keyer 200 // seuil de déclenchement de la clef CW sur entree_U1S
unsigned long morse_tip, morse_top, morse_duree;
int morse_value;	// duree signal
// duree des différents éléments, qui s'auto-règlent 
//int morse_base=90, morse_tih=90, morse_taah, morse_intersigne, morse_intercar, morse_intermot;
// mesure des durées manipulées
unsigned int tempo_cw2;
#define tempo_cw2_reset 5  // granularité des tempos de décodage
char morse_manip='?';	// ?=rien M=manip pioche U1, S=semi-auto 2 contacts U2/U3, A=auto 2 contacts U2/U3
// si le 1er contact=U2=semi-auto si =U3=auto
int morse_etat;		// etat : 0=rien 1=pioche 2=point 3=trait

//int morse_prop=35;    // proportion entre . et - en 1/10
int morse_bit=0;
char morse_car, morse_mask; // caractère en decodage
int morse_espace; // gestion espaces
char morse_texte[17];

// *** Notes de MUSIQUE *** (fréquences x 100 de la gamme No 3)
//                                  do   re   mi   fa  sol   la   si   
char note_name[] =              {  'c', 'd', 'e', 'f', 'g', 'a', 'b' };
unsigned int note_tone[] =      { 26163,29366,32963,34923,39199,44000,49388 };
float note_freq;
int duree_note=150;      // ms de base


// *** BigBen *** Declarations pour horloge type BigBen
// d'après : http://en.wikipedia.org/wiki/Westminster_Quarters
// Hertz = b3=494 e4=659 f#4=740 g#4=831 BigBen=e2=165 e3=330
int grafcet_bigben;
//            Chime      0                1                2                3                4   
char* bigben_chime[]={"$2g#4f#4e4$6b3 ","$2e4g#4f#4$6b3 ","$2e4f#4g#4$6e4 ","$2g#4e4f#4$6b3 ","$2b3f#4g#4$6e4 "};
char* bigben ="$4e3  ";    // normalement c'est e2, mais c'est trop grave pour nous !
// Selon le quart d'heure, on utilise 1,2,3 ou 4 chimes, et on commence par "le suivant", et on tourne sur les 5
// pour minutes     0   15   30   45 
int bigben_debut[]={1,   0,   1,   3};    // chime debut
int bigben_fin[]  ={4,   0,   2,   0};    // chime fin
//  chimes          1234 0    12   340
int bigben_i,bigben_n;
// ******************
// *** GoodFields ***
// ******************
#endif	// decl1300_morse
