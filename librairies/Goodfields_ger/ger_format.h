/* Version 04/09/2015
 *** formatages
char* format_4d (int val) // formate un nombre de -999 à 9999 dans 4 caractères

 *** constantes
 Il existe en standard :
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

 *** macros
 GER_ANAREAD
 GER_ANAREAD2
*/
// ************************************************************************************************
// ************************************************************************************************
#ifndef ger_format
#define ger_format "OK"

// caractères particuliers (selon afficheurs peut être différents) voir dans ger_lcd_bargraph.h
#ifndef char_mu
#define char_mu 228
#endif

#ifndef char_ohms
#define char_ohms 243
#endif
// ************************************************************************************************
// Constantes sympas avec une précision suffisante (Arduino gère 7 à 8 decimales)
// Pi = 3.14159 26535 89793 23846 26433 83279 50288 41971 69399 37510 58209 74944 59230 78164 06286 20899 86280 34825 34211 70679 82148 08651 ...
// à peu près 2 Pi
#ifndef deuxpi
#define deuxpi TWO_PI // 6.28318530 //6,283185307179586476925286766559  
#endif
#ifndef radian_degres
#define radian_degres = 360./TWO_PI;
#endif
// racine carree de 2
#ifndef racine2
#define racine2 1.414213562 //1,4142135623730950488016887242097
#endif
// MUSIQUE
// rapport de fréquence entre 2 notes séparées d'un demi-ton = 2 puissance 1/12 
#ifndef demiton
#define demiton 1.05946309 //1,0594630943592952645618252949463
#endif

/*
// rapport de fréquence entre 2 notes séparées d'un SAVART = 10 puissance 1/1000
// il y a 25 savarts entre 2 notes séparées d'un demi-ton, 301,03 SAVARTS dans un octave
#ifndef savart
#define savart 1.002305238 //0778996719154048893281 
#endif
// rapport de fréquence entre 2 notes séparées d'un CENT = 2 puissance 1/1200 
// il y a 100 cents entre 2 notes séparées d'un demi-ton, 1200 CENTS dans un octave
#ifndef savart_cent
#define savart_cent 1.0005777895 //065548592967925757932
#endif
// NB : l'oreille humaine peut détecter environ 0,3% de variation = 1,3 SAVARTS = 5 CENTS = 1,0028922878693670715023923823933
// soit environ 20 niveaux détectables entre 2 notes séparées d'un demi-ton.
// en fait, pour que cela soit facilement décelable, je prends 4 SAVARTS = 15 CENTS
// Voir : http://fr.wikipedia.org/wiki/Cent_et_savart
*/
#ifndef savart_mini
#define savart_mini 1.00870200 // 1.0087019837903990132289428069278 // 15 CENT
#endif
// ************************************************************************************************
// *** LECTURE et CONVERSION d'entrées analogiques ***
// cette MACRO lit l'entree_U', multiplie par 'coef_U' et rends le résultat en float dans 'U'
// si on a un débordement, on retourne une valeur très grande (1./0.) pour avoir un affichage d'erreur
#define GER_ANAREAD2(U,entree_U,coef_U)  U = analogRead(entree_U);  if (U > 1022) {U=1./0.;} else {U = coef_U * U;}
// sans débordement
#define GER_ANAREAD(U,entree_U,coef_U)  U = analogRead(entree_U) * coef_U;
// ************************************************************************************************	
/*
'formatt' Convertit  la valeur 'float' "v" avec "nbchiffres" digits significatifs
L' "unite" est placée à la place du point décimal, avec le multiplicateur éventuel.
Le signe - est également traité.
Donc [signe] + digits + [multiplicateur] + unite = "nbchiffres+1 à 3" caractères au final
Résultat dans 'buf'.

Usage :
char valuef[15];
    freq=123456.789;    formatf(freq,'c',5,valuef); // valuef="123Kc45\0"   (5->7 car)
    freq=1.23456789;    formatf(freq,'V',8,valuef); // valuef="1V2345678 \0" (8->10 car)
    freq=-0.0123456789; formatt(freq,'A',5,valuef); // valuef="-12mA345\0" (5->8 car)

	Serial << "F=" << valuef;                       // sur voie série
    lcd.setCursor(0, 0);  lcd << "F=" << valuef;    // sur LCD
    strcpy(cw_texte,valuef);                        // en MORSE
	
char(x) interprété par les LCD :
126 ~flèche à droite
127 flèche à gauche
165 ¥ petit carré
219 Û boite vide
224 ß symbole degré
228 mu micro
244 Oméga Ohms
253 ý degré en bas
255 ÿ carré plein , mais souligné

V 2013.10.30
*/
void formatt(float v,char unite,int nbchiffres,char buf[15])
{
//        mult =            0    1    2    3    4    5   6     7     8    9
//        multiplicateur =  Peta Tera Giga Mega Kilo 1   milli micro nano pico
const char multiple[10] = {'P', 'T', 'G', 'M', 'k', ' ','m',  char(char_mu),  'n', 'p'};
float vloc;
int mult=5,nbdec,pospoint,i;

// erreur
  if (nbchiffres < 2) { strcpy(buf,"!\0"); return;}
  vloc=abs(v);
// si =0.000000, on garde mult=5
  if (vloc != 0.0) {
// si valeur inférieure à 1, on multiplie
    while (vloc < .999 && mult < 9) {
      vloc = vloc*1000;
      mult++;
    }
// si valeur grande, on divise
    while (vloc > 999. && mult > 0) {
      vloc = vloc/1000;
      mult--;
    }
// erreur
    if (nbchiffres == 2 && vloc > 99) { strcpy(buf,"!!\0"); return;}
  }
// nombre de décimales selon valeur   // ex nbchiffres=4    7
   nbdec = nbchiffres-1; pospoint=1;              //           9.999  9.999999
   if (vloc >= 10 ) { nbdec--;  pospoint=2; }        //           99.99  99.99999
   if (vloc >= 100 ) { nbdec--; pospoint=3; }         //           999.9  999.9999
// conversion FLOAT CHAR
   dtostrf(abs(vloc),nbchiffres,max(0,nbdec),buf);
//   Serial && "nbchiffres=" && nbchiffres && " nbdec=" && nbdec && " v=" && v && " buf=" && buf && "* pospoint=" && pospoint && endli;
// ici, nbdec devient la longueur obtenue (rien à voir avec le nombre de décimales)
   nbdec=strlen(buf);

// si multiple, on décale   999.  pospoint=3 nbdec=4
//   i=nbdec+1;
	if (unite != ' ') {		// pas d'unité : pas de multiple
      if (mult!=5) {
        for (i=nbdec+1;i>pospoint;i--) buf[i]=buf[i-1];  // 999..
        buf[pospoint++]=multiple[mult];       // 999m.
        buf[pospoint++]=unite;                // 999mU
      } else {
// on remplace le . par l'unité
        buf[pospoint++]=unite;
//        buf[max(nbdec,pospoint++)]=' ';    // pour assurer toujours la même longueur
        if (nbdec<pospoint+1) nbdec++; 
        buf[nbdec]=' ';    // pour assurer toujours la même longueur
      }
      buf[max(nbdec+1,pospoint)]='\0';    // 999mU0
	}
/*
	buf[pospoint]=multiple[mult];       // 999m.
      buf[pospoint+1]=unite;              // 999mU
      if (nbdec<pospoint+1) nbdec++; 
   } else {
// on remplace le . par l'unité
      buf[pospoint]=unite;
      if (nbdec<pospoint+1) nbdec++; 
      buf[nbdec]=' ';    // pour assurer toujours la même longueur
   }
   buf[nbdec+1]='\0';    // 999mU0
*/   
   
//   Serial && "B v=" && v && " mult=" && mult && " buf=" && buf && "* pospoint=" && pospoint && " nbdecB=" && nbdec && endli;
// négatif ?
   if (v < 0.) {
      for (i=nbdec+2;i>0;i--) buf[i]=buf[i-1];  // 9999mU0
      buf[0]='-';      // -999mU0
   }
//   Serial << "4 v=" << v << " buf=" << buf << "* l=" << strlen(buf) << endl;
   return;
}
/*
// peut-être aussi simple ?
{
  int i,lon;
  char * pch;
  formatf(v,unite,nbchiffres,buf);
  lon=strlen (buf);
  if (buf[lon-1] ==  ' ') {
!!! problème de char * 
    pch= strstr (buf,".");
    strncpy(pch,buf[lon-2],1); buf[lon-2]='\0'; buf[lon-1]='\0';
  } else {
    memmove (pch+2,pch+1,strlen(pch)-2); // décalage de 1 à droite
    strncpy(pch+1,buf[lon-1],1); buf[lon-1]='\0';
  }
   Serial << "formatt *buf=" << buf << "*l=" << strlen(buf) << "*" << endl;
   return;
}
*/
/*
int i,j;
float xx[] = {-1000., -100., -10.,-0.5, 0., 1.5, 4.95, 12.34, 567.8901, 1000000.} ;
char valueg[15];
// tests
for (i=1;i<6;i++) {
Serial && "--------------- i=" && i && endli;
  for (j=0;j<6;j++) {
  formatt(xx[j], 'x', i, valueg);
  Serial && xx[j] && "[" && valueg && "] ";
}
  Serial && endli;
}
*/
// ************************************************************************************************
// ************************************************************************************************
// OBSOLETES
// ************************************************************************************************
// ************************************************************************************************
/*
OBSOLETE
**** PREFEREZ formatt(), plus complet, plus compact (en sortie et en longueur code binaire) !

Convertit  la valeur 'float' "v" avec "nbchiffres" digits significatifs suivi de l'"unite"
Donc "nbchiffres" digits + point + multiplicateur + unite = "nbchiffres+3" caractères
Résultat dans 'buf'.
Usage :
char valuef[15];
    freq=123456.789;    formatf(freq,'c',5,valuef); // valuef="123.45Kc\0"
    freq=1.23456789;    formatf(freq,'c',8,valuef); // valuef="1.2345678c \0"
    freq=0.0123456789;  formatt(freq,'A',5,valuef); // valuef="12.345mA\0"

	Serial << "F=" << valuef;                       // sur voie série
    lcd.setCursor(0, 0);  lcd << "F=" << valuef;    // sur LCD
    strcpy(cw_texte,valuef);                        // en MORSE
*/
void formatf(float v,char unite,int nbchiffres,char buf[15])
{
//      dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);

 float vloc;
 int mult=5,nbdec;
//   mult =            0    1    2    3    4    5   6     7     8    9
//   multiplicateur =  Peta Tera Giga Mega Kilo 1   milli micro nano pico
 char multiple[10] = {'P', 'T', 'G', 'M', 'k', ' ','m',  'u',  'n', 'p'};
  vloc=v;
// si =0.000000, on garde mult=5
  if (vloc != 0.0) {
// si valeur inférieure à 1, on multiplie
    while (vloc < 1. && mult < 9) {
      vloc = vloc*1000;
      mult++;
    }
// si valeur grande, on divise
    while (vloc >= 1000. && mult > 0) {
      vloc = vloc/1000;
      mult--;
    }
  }
// nombre de décimales selon valeur   // ex nbchiffres=4    7
   nbdec = nbchiffres-1;              //           9.999  9.999999
   if (vloc > 10 ) nbdec--;           //           99.99  99.99999
   if (vloc > 100 ) nbdec--;          //           999.9  999.9999
// conversion FLOAT CHAR
   dtostrf(vloc,nbchiffres,nbdec,buf);
// ici, nbdec devient la longueur obtenue (rien à voir avec le nombre de décimales)
   nbdec=strlen(buf);
   if (mult!=5) {
      buf[nbdec]=multiple[mult]; buf[nbdec+1]=unite;
   } else {
      buf[nbdec]=unite; buf[nbdec+1]=multiple[mult];
   }
// fin de CHAR
   buf[nbdec+2]='\0';
//   Serial << "*buf=" << buf << "*l=" << strlen(buf) << "*" << endl;
   return;
}
//--------------------------------********************--------------------------------
// formate un nombre de -999 à 9999 dans 4 caractères
char* format_4d (int val) {
  static char out[5];
  int j, v;
  memset (out, ' ', sizeof(out));
  v = abs(val);
  for (j = 3; j >= 0; j--) {
    out[j] = '0' + v % 10;
    v = v / 10;
  }
  out[4] = '\0';
  if (val < 0) out[0] = '-';
  return out;
}
//--------------------------------********************--------------------------------

#endif