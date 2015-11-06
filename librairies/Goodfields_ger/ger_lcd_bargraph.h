// ************************************************************************************************
/*
GoodFields . modules de gestion de bar-graphes sur LCD standards
19/03/2014
21/11/2013
// ************************************************************************************************
//#define creercaracterestype  1 // 0 à 4 (0 par défaut si vous ne le définissez pas)

// Démonstration : c'est beau, mais cela vous coute environ 3 Koctets de mémoire !!!
//#define creercaracteresdemo  1 ou 2 // si on veut un test au démarrage
// ne pas définir si on ne veut pas la démo 
***************************************************************************************************
 Caractères spéciaux interprétés par les LCD (char(x))
 45 \x2D moins (niveau 4 (de 0 à 7))
 95 \x5F souligné (niveau 1)
 
126 \x7E ~flèche à droite
127 \x7F flèche à gauche
165 \xA5 ¥ petit carré
219 \xDB Û boite vide
224 \xE0 ß symbole degré
228 \xE4 mu micro
244 \xF4 Oméga Ohms
253 \xFD ý degré en bas
255 \xFF ÿ carré plein , mais souligné

NB : le \ ne marche pas !
*/
// caractères particuliers (selon afficheurs peut être différents)
#ifndef char_plushaut
#define char_plushaut 39  // ' cote  237 // petite etoile en haut à gauche
#endif
#ifndef char_plusbas
#define char_plusbas 46 // '.'
#endif

// inclusion de bibliothèque autre
#include <ger_format.h>
// verrou
#ifndef ger_lcd_bargraph
#define ger_lcd_bargraph

// pré-déclaration nécéssaire pour la compilation
char* ger_bargraph (int valeur, float e_mini, float e_maxi, float s_mini, float s_maxi, float b_mini, float b_maxi, int nbcar, int nbchiffres, char unite, float &sortie);

// ************************************************************************************************
// création de caractères spécifiques
/* http://arduino.cc/fr/Main/LcdcreateChar
	
	creercaracterestype : 
        0 ou non défini = blanc + 5 barres simples (bargraph horizontal)
		1 = blanc + 5 pavés grossissants (bargraph horizontal)
		2 = blanc + 3 barres simples (bargraph rapide)
		3 = blanc + 7 pavés grossissants (tracé de tendances à 8 niveaux)
		4 = blanc + 7 barres horizontales (tracé de tendances à 8 niveaux)
		5 = blanc + 8 barres horizontales (tracé de tendances à 9 niveaux)

	creercaracteresdemo : 
        non défini = rien
		affichage des caractères au démarrage sur le LCD

Autres standards :
0 barre = espace = cacactère 32
5 barres soulignées = caractère 255
*/
// suite des caractères à utiliser déclarée en GLOBAL
// quelchar[0] contient le nombre de sous-échelons disponibles (non compris le 0)
//byte quelchar[10] = { 7,32,95,1,2,45,3,4,5,0 };
// NB: ne pas programmer le caractère 0, car c'est la fin de chaines de caractères
// chez COMFILE, danbs les ELCDxxx, ils utilisent les caractères 8 à 15 ...
byte quelchar[10] = { 0 };

void ger_creercaracteres() {

int i,j,n;
float x;

#ifndef creercaracterestype
#define creercaracterestype 0
#endif

// blanc + 5 barres simples grossissant horizontalement
// barres  0  1  2  3  4  5
// char() 32  1  2  3  4  5  6  7  8
//                          GR :) °C 

#if creercaracterestype == 0
byte carrepro[64] = { 
    B10000, B10000, B10000, B10000, B10000, B10000, B10000,	0,
	B11000, B11000, B11000, B11000, B11000, B11000, B11000,	0,
	B11100, B11100, B11100, B11100, B11100, B11100, B11100,	0,
	B11110, B11110, B11110, B11110, B11110, B11110, B11110,	0,
	B11111, B11111, B11111, B11111, B11111, B11111, B11111,	0,
	B11101, B10111, B10110, B10000, B10111, B10001, B11111,	0,	// GR
	B00000, B11011, B11011, B00000, B10001, B01110, B00000,	0,  // :)	
	B01000, B10100, B01000, B00011, B00100, B00100, B00011, 0 };	// °C

// quelchar[0] = nombre d'échelons (donc +1 pour le nombre de caractères)
// quelchar[1...(quelchar[0]+1)] = caractères correspondants
//byte quelchar[6] = { 5,32,1,2,3,4,5 };
quelchar[0] = 5;
// redirection
quelchar[1]=32; quelchar[2]=1; quelchar[3]=2; quelchar[4]=3; quelchar[5]=4; quelchar[6]=5;
#endif

// 5 paves grossissant horizontalement et souligné
#if creercaracterestype == 1
byte carrepro[64] = { 
	B00000, B00000, B00000, B00000, B00000, B00000, B00000, B10101,	// 0 barre
	B00000, B00000, B00000, B00000, B10000, B10000, B10000, B10101,	// 1 barre
	B00000, B00000, B00000, B11000, B11000, B11000, B11000, B10101,	// 2 barres
	B00000, B00000, B11100, B11100, B11100, B11100, B11100, B10101,	// 3 barres
	B00000, B11110, B11110, B11110, B11110, B11110, B11110, B10101,	// 4 barres
	B11111, B11111, B11111, B11111, B11111, B11111, B11111, B10101,	// 5 barres
	B00000, B11011, B11011, B00000, B10001, B01110, B00000,	0,  // :)	
    B01000, B10100, B01000, B00011, B00100, B00100, B00011, 0 };	// °C
	quelchar[0] =5;
	quelchar[1] =1; quelchar[2] =2; quelchar[3] =3; quelchar[4] =4; quelchar[5] =5; quelchar[6] =6;
#endif

// 1 à 3 barres écartées
// barres  0  1  2  3  
// char()  1  2  3  4  5  6  7  8
//                    GR :( :) GR °C 
#if creercaracterestype == 2
byte carrepro[64] = { 
	B00000, B00000, B00000, B00000, B00000, B00000, B00000, B10101,	// 0 barre
	B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10101,	// 1 barre
	B10100, B10100, B10100, B10100, B10100, B10100, B10100, B10101,	// 2 barres
	B10101, B10101, B10101, B10101, B10101, B10101, B10101, B10101,	// 3 barres
	B11101, B10111, B10110, B10000, B10111, B10001, B11111,	0,	// GR
	B00000, B11011, B11011, B00000, B01110, B10001,	B00000, 0,  // :(	
	B00000, B11011, B11011, B00000, B10001, B01110, B00000,	0,  // :)	
	B01000, B10100, B01000, B00011, B00100, B00100, B00011, 0 };	// °C
	quelchar[0] =3; quelchar[1] =1; quelchar[2] =2; quelchar[3] =3; quelchar[4] =4;
#endif

// tracés de tendances par 7 pavés grossissants verticalement
// barres  0  1  2  3  4  5  6  7
// char() 32 95  1  2  3  4  5  6  7  8
//                                :) °C 
#if creercaracterestype == 3
byte carrepro[64] = { 
	B00000, B00000, B00000, B00000, B00000, B11111, B11111,	0,	
	B00000, B00000, B00000, B00000, B11111, B11111, B11111,	0,	
	B00000, B00000, B00000, B11111, B11111, B11111, B11111,	0,	
	B00000, B00000, B11111, B11111, B11111, B11111, B11111,	0,	
	B00000, B11111, B11111, B11111, B11111, B11111, B11111,	0,	
	B11111, B11111, B11111, B11111, B11111, B11111, B11111,	0,	
	B00000, B11011, B11011, B00000, B10001, B01110, B00000,	0,  // :)	
    B01000, B10100, B01000, B00011, B00100, B00100, B00011, 0 };	// °C
	quelchar[0] =7; quelchar[1] =95; quelchar[2] =1; quelchar[3] =2; quelchar[4] =3; quelchar[5] =4; quelchar[6] =5; quelchar[7] =6;
#endif

// tracés de tendances par 7 traits montants verticalement (genre courbe)
// barres  0  1  2  3  4  5  6  7
// char() 32 95  1  2 45  3  4  5  6  7  8
//                                :( :) °C 
#if creercaracterestype == 4
byte carrepro[64] = { 
	B00000, B00000, B00000, B00000, B00000, B11111, B00000,	0,	
	B00000, B00000, B00000, B00000, B11111, B00000, B00000,	0,	
	B00000, B00000, B11111, B00000, B00000, B00000, B00000,	0,	
	B00000, B11111, B00000, B00000, B00000, B00000, B00000,	0,	
	B11111, B00000, B00000, B00000, B00000, B00000, B00000,	0,	
	B00000, B00000,	B00000, B11011, B00000, B01110, B10001, 0,  // :(	
	B11011, B00000, B00100, B10001, B01110, B00000, B00000, 0,  // :)	
    B01000, B10100, B01000, B00011, B00100, B00100, B00011, 0 };	// °C

//	B00000, B11011, B11011, B00000, B01110, B10001,	B00000, 0,  // :(	
//	B00000, B11011, B11011, B00000, B10001, B01110, B00000,	0,  // :)	

//	B00000, B00000,	B00000, B01010, B10001, B01110, B10001, 0,  // :(	
//	B11011, B00100, B10001, B01110, B00000, B00000, B00000, 0,  // :)	
    quelchar[0] =7; 
	quelchar[1] =32; quelchar[2] =95; quelchar[3] =1; quelchar[4] =2; quelchar[5] =45; quelchar[6] =3; quelchar[7] =4; quelchar[8] =5;
#endif

// tracés de tendances par 8 traits montants verticalement (genre courbe)
// ne marche pas sur tous les LCD (ligne du bas = souligné) et pas très beau !
// barres  0  1  2  3  4  5  6  7  8
// char() 32  1 95  2  3 45  4  5  6  7  8
//                                    :) °C 
#if creercaracterestype == 5
byte carrepro[64] = { 
	B00000, B00000, B00000, B00000, B00000, B00000, B00000,	B11111,	
	B00000, B00000, B00000, B00000, B00000, B11111, B00000,	0,	
	B00000, B00000, B00000, B00000, B11111, B00000, B00000,	0,	
	B00000, B00000, B11111, B00000, B00000, B00000, B00000,	0,	
	B00000, B11111, B00000, B00000, B00000, B00000, B00000,	0,	
	B11111, B00000, B00000, B00000, B00000, B00000, B00000,	0,	
	B11011, B00000, B00100, B10001, B01110, B00000, B00000, 0,  // :)	
    B01000, B10100, B01000, B00011, B00100, B00100, B00011, 0 };	// °C
	quelchar[0] =8; quelchar[1] =32; quelchar[2] =1; quelchar[3] =95; quelchar[4] =2; quelchar[5] =3; quelchar[6] =45; quelchar[7] =4; quelchar[8] =5; quelchar[9] =6;  
#endif

// apprend les caractères à l'écran LCD
 for (i=0;i<8;i++) { lcd.createChar(i+1, &carrepro[8*i]); }

// Démonstration : c'est beau, mais cela vous coute 3 K.octets environ !!!
#ifdef creercaracteresdemo
//	Serial && "creercaracteres:demo" && endli;
  lcd.clear();
// TEST CARACTERES BARGRAPH
    n=0;
  for (i=1;i<=quelchar[0]+1;i++) {
    lcd.setCursor(i-1, 0); 
	lcd && char(quelchar[i]); //affiche le caractère
    lcd.setCursor(i-1, 1);  lcd && "^" ;// _HEX(i);	// et son numero
	if (quelchar[i] < 16 & quelchar[i] > n) n = quelchar[i];	// pour autres caractères porogrammés
  }
// autres caractères programmés
  j=i-1;
  for (i=n+1;i<=8;i++) {
    lcd.setCursor(j, 0); 
	lcd && char(i); //affiche le caractère
    lcd.setCursor(j, 1);  lcd && i; // _HEX(i);	// et son numero
    j++;
  }
#if creercaracteresdemo > 1
// demo de bargraph
    j=5; n = j*quelchar[0];
	for (i=0;i<=n;i++) {
      lcd.setCursor(11, 0);  lcd && ger_bargraph (  i, 0., n, 0., n, 0., n, j, 3, ' ', x);
      lcd.setCursor(11, 1);  lcd && ger_bargraph (n-i, 0., n, 0., n, 0., n, j, 3, ' ', x);
	  delay(150);
//	  if (i==0 | i==n) delay(500);
    }
#endif
#endif
}
//**********************************************************
// conversion d'une entrée (entière) en sortie (flottante mise à l'échelle)
// et création d'une chaine de caractères bargraph
/*
valeur = entier (typiquement entrée analogique de 0 à 1023)
e_mini = valeur minimale
e_maxi = valeur maximale
s_mini = sortie correspondante à valeur minimale
s_maxi = sortie correspondante à valeur maximale
b_mini = valeur flottante de sortie correspondante au mini du bargraph
b_maxi = valeur flottante de sortie correspondante au maxi du bargraph
nbcar = nombre de caractères du bargraph
nbchiffres = nombre de chiffres de la valeur (non comprise multiplicateur+unité) 
  si NEGATIF, la valeur est mise APRES la barre, en plus !
unite = 1 caractère pour l'unité. si " " = pas de valeur
sortie : sortie (en flottant)

NB: si on affiche la valeur avec un nombre de caractères positif, cette valeur est SUPERPOSEE sur le bargraph, calé à droite si la valeur est inférieure à la moitié du bargraph, à gauche si la valeur est supérieure à cette moitié, afin de ne pas cacher l'extrémite du bargraph.
*/
char* ger_bargraph (int valeur, float e_mini, float e_maxi, float s_mini, float s_maxi, float b_mini, float b_maxi, int nbcar, int nbchiffres, char unite, float &sortie) {

static char local[21];
char buf[15];
int i,j,nbre_echelons;
    
// echelle = 
	sortie = (float) s_mini + ((valeur - e_mini) * (s_maxi - s_mini) / (e_maxi - e_mini) );
// nombre de positions possibles = de 0 à e_maxi		NPP = nbcar * quelchar[0] + 1
// nombre d'écarts entre échelons possibles 			NEE = nbcar * quelchar[0]
// valeur écart 										VEC = (b_maxi - b_mini)/(nbcar * quelchar[0])
// calcul du nombre d'échelons correspondant à la sortie
	nbre_echelons = (float) nbcar * quelchar[0] * (sortie - b_mini) / (b_maxi - b_mini);
//    nbre_echelons = constrain(nbre_echelons, 0, nbcar * quelchar[0] + 1);
// nettoyage du buffer
    memset (local,char(quelchar[1]),nbcar); // raz pavés blancs (30448)
	

// affichage d'un bargraph composé de nbcar caractères 'bloc' maximum
    i = nbre_echelons/quelchar[0];
    if (i>0) memset (local,char(quelchar[quelchar[0]+1]),i); // mise en place des blocs pleins

//    Serial << " nbre_echelons=" << nbre_echelons << " ";  // debug/test (sans LCD !!!)
//    for (j=0;j<i;j++) Serial << "#";  // bargraphe - debug/test (sans LCD !!!) 

// mise en place d'un caractère programmé parmi  
//    if (i>0 && i<nbcar) local[i] = char(nbre_echelons%quelchar[0]+1);
    if (i<nbcar) local[i] = char(quelchar[nbre_echelons%quelchar[0]+1]);
// affichage des butées hors graphe
    if (sortie > b_maxi) local[nbcar-1]='>';
    if (sortie < b_mini) local[0]='<';
   
// affichage de la valeur numerique
   i=0;
   if (unite != ' ') {
		formatt (sortie, unite, abs(nbchiffres), buf);
		i=strlen(buf); 
// économie des blancs en fin de chaine
		if (buf[i-1] == ' ' ) i--; 
		if (buf[i-1] == ' ' ) i--; 
// valeur en surimpression
        if (nbchiffres > 0) {
			for (j=0;j<i;j++) {
				if ( sortie > (b_mini+b_maxi)/2 ) {
					local[j] = buf[j];
				} else {
					local[nbcar-i+j] = buf [j];
				}
			}
			i=0;		// pour raz correcte !
// ou valeur en plus , derrière
		} else {
			for (j=0;j<i;j++) {local[nbcar+j] = buf[j]; }
		}
    }
// fin chaine
    local[nbcar+i]='\0';
// debug/test    
//for (j=0;j<nbcar+i+1;j++) Serial && j && "=" && _DEC(local[j]) && " ";
//Serial && endli; 

return local;
}
// ************************************************************************************************
/* creation d'un graphe de tendance à partir de valeurs en float
	nbcar = nombre de caractères a afficher, mais aussi nombre de valeurs utiles (maxi 20)
  valeurs = tableau de valeurs réelles (déclarer float valeurs['nbcar'])

Retourne un char* contenant le bargraph à envoyer au lcd
La mise à l'échelle est automatique
Le cadrage mini/maxi est automatique si mini >= maxi

09/09/2014 :
La valeur minimale affiche le trait le plus bas (pas le blanc, donc un caractère de moins que (quelchar[0]+1)

Pré-requis :
#define creercaracterestype 3
#include <ger_lcd_bargraph.h>
*/
char* ger_tendance (int nbcar, float mini, float maxi, float valeurs[]) {

int i,nb,hauteur;
static char local[21];
float echelle,mini0,maxi0;

	nb = min(nbcar,20);

// determination de la plus petite et de la plus grande valeur
	if (mini < maxi ) {
		mini0 = mini;
		maxi0 = maxi;
	} else {
//		mini0 = 3.4028235E+38; maxi0 = -mini0;
		mini0 = valeurs[0]; maxi0 = valeurs[0];
		for (i=1; i < nb; i++) {
			if (valeurs[i] < mini0) mini0 = valeurs[i];
			if (valeurs[i] > maxi0) maxi0 = valeurs[i];
		}
	}

// d'ou l'échelle, ou plutot l'échelon
	echelle = (maxi0-mini0)/(quelchar[0]-1);
//    Serial && " mini0,maxi0=" && mini0 && "/" && maxi0 && (quelchar[0]-1) && "=nbech ech." && echelle && endli;

// si toutes valeurs égales, on fait un trait moyen
	if ( mini0==maxi0 || abs(echelle) < abs(0.001*valeurs[nb-1]) ) {
		for (i=0; i < nb; i++) { local[i] = char(quelchar[(quelchar[0])/2]+1); }
	} else {
// cas normal
    for (i=0; i < nb; i++) {
		hauteur = int( 0.+ ((valeurs[i]-mini0+echelle/2.)/echelle)+2.) ;
		local[i] = char(quelchar[hauteur]);
//		Serial && i && ":" && hauteur && "=h v=" && valeurs[i] ;
	    if (hauteur < 2) { local[i] = char(char_plusbas); }		// '.' plus bas !
		if (hauteur > quelchar[0]+1) { local[i] = char(char_plushaut); }	// '+' plus haut!
//		Serial && "=" && int(local[i]) && endli;
    }
  }
  local[i] = '\0';
//  Serial && "-----" && endli;
  
  return local;
}
// ************************************************************************************************
#endif
