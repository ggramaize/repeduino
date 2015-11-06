// OBSOLETE - voir modules ger_...

// ************************************************************************************************
// SOUS-PROGRAMMES V03/11/2013
// serial_reception : amélioré, fin de message = *
// http://www.cplusplus.com/reference/  Aide en C/C++

// PROBLEMES A TESTER :
// barregraph à 1 seul caractère, si trop de car en sortie (barre+valeur > 21 car .... cf 201_test_bargraph

// Usage :

//**********************************************************
// DECLARATIONS
// il serait judicieux de rajouter des flags pour les autres paquets de fonctions (time, format ... quoique celles-là soient basqieus)
//#define ger_clavier A0	// entrée ana du clavier si on veut qu'il soit géré (voir source pour autres paramètres is pas défaut)
//#define ger_bargraph OUI // a définir si on veut utiliser ces fonctions
//#include "decl_sp.h"

//**********************************************************
// SOUS-PROGRAMMES
// gestion du temps, formatages, gestion clavier
//#include "sp.h"

// ************************************************************************************************
// ************************************************************************************************
/* Programme au -ieme de seconde, gestion des temporisations
 NB : le débordement du compteur en (unsigned long) a lieu tous les 50 jours environ ...
 lorsqu'il arrive à  la valeur 2^32-1 = 4294967295 = 49j 17h 2m 47s
 delta devient négatif, euh non, très grand : 
 previousMillis = 4294967290
 millis() = 4294967290 + 10(par exemple) = 4294967300 (-4294967296)=> 4 
 delta = 4 - 4294967290 = 6 ! OK ca marche toujours !
*/
void ger_top_centieme () {
	unsigned long delta;
	unsigned long currentMillis = millis();

// Top Millième de secondes
	delta = currentMillis - previous1;
	if (delta >= 1) {
// ms dans la seconde
        milliseconde += delta;
// save the last time you come here
		previous1 = currentMillis;   
		top_1 = delta;
	}	// FIN 1ms

// Top Centième de secondes
	delta = currentMillis - previous10;
	if (delta >= 10) {
// save the last time you come here
		previous10 = currentMillis;   
		top_10 = delta;

// Top 250ms
		delta = currentMillis - previous250;
		if (delta >= 250) {
			previous250 = currentMillis;   
			top_250ms = true;
		}	// FIN 250 ms
	}	// FIN 10ms
   
// Top Dixième de seconde
	delta = currentMillis - previous100;
	if (delta >= 100) {
		previous100 = currentMillis;   
		top_100ms = true;
    
// Top 500ms
		delta = currentMillis - previous500;
		if (delta >= 500) {
			previous500 = currentMillis;   
			top_500ms = true;
		}	// FIN 500 ms
	}	// FIN 100ms

// Top Seconde
	if (second() != previousSecond) {
		previousSecond = second();
		top_1000ms = true;
		milliseconde = 0;

// Top 55 secondes
		if (previousSecond == 55) {
			top_55s=true;
		}

// Top Minute
		if (previousSecond == 0) {
			top_60000ms=true;
/*
// indicateur si horloge synchroniée
			if (horloge_synchro_tempo < 1) {
				horloge_synchro_tempo = horloge_synchro_tempo_reset;
//          	horloge_synchronisee = 0;  // indique si la mise à  l'heure a été effectuée
			} else {
				horloge_synchro_tempo--;
			}
*/
}	// FIN minute
	}	// FIN seconde
  return;
}

// ************************************************************************************************
// affichage horloge dans un char
char* horodatage(char format) {
//                 iimax = 012345678901234567890 = index du dernier caractère (' ' final)
// 'd' date et heure       01/04/2013 10:15:28_\0
// 'h' heure               10:15:28_\0
// 'c' centiemes/milliemes 15:28.584_\0
// 'm' miilièmes           .999_\0
// 'e' date et heure       01/04/13 10:15:28_

static char out[22]; 
char valuef[3],valueff[4];
int ii,iimax=0;

// INIT
  memset (out,'\0',sizeof(out));
// JOUR:MOIS:ANNEE
  if (format == 'd' || format == 'e') {
    dtostrf(day(),2,0,valuef); strcpy( out, valuef ); strcat( out, "/" );
    dtostrf(month(),2,0,valueff); strcat( out, valueff ); strcat( out, "/" );
    if (format == 'd') { dtostrf(year(),4,0,valuef); iimax+=11;} else {dtostrf(year()-2000,2,0,valuef); iimax+=9;}
    strcat( out, valuef ); strcat( out, " " );
  }
// HEURE:
  if (format != 'c') {
    dtostrf(hour(),2,0,valuef); 
    if (format == 'd' || format == 'e') {strcat( out, valuef );} else { strcpy( out, valuef );}
    strcat( out, ":" );
    iimax+=3;
  }
// MINUTE:SECONDES
//  if (format == 'd' || format == 'h' || format == 'c' || format == 'e') {
    dtostrf(minute(),2,0,valueff);
    if (format == 'c') {strcpy( out, valueff );} else { strcat( out, valueff );}
    strcat( out, ":" );
    dtostrf(second(),2,0,valuef); strcat( out, valuef );
    iimax+=5;
//  }
// MINUTES:SECONDES.MILLIEMES ou MILLIEMES SEULS
  if (format == 'c' || format == 'm') {
    dtostrf(milliseconde,3,0,valueff); 
    if (format == 'm') { strcpy( out, valueff ); iimax+=3; } else { strcat( out, "." ); strcat( out, valueff ); iimax+=4; }
  }
// FIN
  strcat( out, " " );
// rajout zeros  sauf séparation date et heure 
  for (ii=0;ii<iimax;ii++) {if (ii != 8 && ii != 10 && out[ii] == ' ') out[ii]='0'; }
// debug
//  Serial << "(124) out=" << out << "=" << endl;  
  return out;
}
/*
PROJET
    _ est à la place de ' ' espace
	toutes les chaines ont terminées par \0
	les zéros non significatifs sont ajoutés
						 012345678901234567890123456789 iimax iisauf
 'd' date et heure       01/04/2013_10:15:28_			17 10
 'e' date et heure       01/04/13 10:15:28_				15  8
 'h' heure               10:15:28_						6	_
 'c' centiemes/milliemes 15:28.584_						7	_
 'm' millièmes           999_\0							1	_
 'j'                     Lun 01 Avr 2013				4	3
 'k'                     Mercredi 01 Septembre 2013_	10	9
						 12345678901234567890123456789
// DATE
switch (format) {
case 'j':
	strcat( out, dayShortStr(weekday())); strcat( out, " " );
case 'k':
	strcat( out, dayLongStr(weekday())); strcat( out, " " );
}
switch (format) {
case 'd':
case 'e':
dtostrf(day(),2,0,vday); strcat( out, vday ); strcat( out, "/" );
dtostrf(month(),2,0,vmonth); strcat( out, vmonth ); strcat( out, "/" );
case 'j':
case 'k':
	dtostrf(day(),2,0,vday); strcat( out, vday ); strcat( out, " " );
	strcat( out, monthShortStr(month())); strcat( out, " " );
	strcat( out, monthLongStr(month())); strcat( out, " " );
}

// HEURE:MINUTE:SECONDE
switch (format) {
case 'd':
case 'e':
case 'h':
dtostrf(hour(),2,0,vday);

case 'c':
dtostrf(minute(),2,0,vmonth);
dtostrf(second(),4,0,vyear);
}

//
switch (format) {
case 'e':
dtostrf(year(),2,0,vyear-2000);
case 'd':
case 'j':
case 'k':
dtostrf(year(),4,0,vyear);
}


switch (format) {
case 'd':
    strcpy( out, vday ); strcat( out, "/" );
    strcat( out, vmonth ); strcat( out, "/" );
    strcat( out, vyear ); strcat( out, " " );
    iimax=17;
case 'e':
    strcpy( out, vday ); strcat( out, "/" );
    strcat( out, vmonth ); strcat( out, "/" );
    strcat( out, vyear ); strcat( out, " " );
    strcat( out, valuef );
    strcat( out, " " );
    iimax=15;

case 'j':	
	strcat( out, dayShortStr(weekday())); strcat( out, " " );
	strcpy( out, vday ); strcat( out, " " );
	strcat( out, monthShortStr(month())); strcat( out, " " );
}
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  if(isLongFormat)
    Serial.print(dayStr(weekday()));
  else  
   Serial.print(dayShortStr(weekday()));
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  if(isLongFormat)
     Serial.print(monthStr(month()));
  else
     Serial.print(monthShortStr(month()));
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
  
*/
// ************************************************************************************************
/*
// Obsolète
// affichage horloge sur la sortie serie, sans fin de ligne
void horodatage_serie(char format) {
   Serial.print (horodatage(format));
}
// ************************************************************************************************
// Obsolète
// affichage heure seulement sur la sortie serie, sans fin de ligne
void horodatage_serial() {
 horodatage_serie('h');
}
*/
// ************************************************************************************************
// ************************************************************************************************
#ifdef ger_clavier
/* scrutation et filtrage de clavier analogique

  utilisation "monocoup" :
  if (touche == '1') { ...; touche=' ';}

  utilisation avec "répétition" :
  if (touche_repeat == '1') { ...; touche_repeat=' ';}
*/

void ger_scrute_clavier(){

int i,j;

// clavier - décalage pour filtrage sur NO de touche (c'est plus stable que sur valeur)
  for (i=ger_clavier_nb-1;i>0;i--) { ntouche_mem[i] = ntouche_mem[i-1]; }
  ntouche_mem[0] = ntouche;

  itouche = analogRead(ger_clavier);
// DEBUG  Serial <<  "ger_scrute_clavier U=" << itouche << endl; delay(100);
  
// On applique la formule n = ( valeur_ana + demi_écart ) / écart
   ntouche = (itouche + ger_clavier_delta/2) / ger_clavier_delta;

/*
	ntouche=-1;
  do {
    ntouche++;
  } while (itouche > ((xtouche[ger_clavier_nb]+xtouche[ger_clavier_nb+1])/2) && ntouche < ger_clavier_nb);
*/

// TRACE pour calibrage des touches (mettre debug à 2)
//  if (debug > 1 && itouche < 1020 ) Serial << "ger_scrute_clavier U=" << itouche << " n=" << ntouche << " l=" << ltouche[ntouche] << " delta=" << ger_clavier_delta << " ecart=" << (itouche-ntouche*ger_clavier_delta) << endl;

// si la touche est stable (X tours)
  j=1;
  for (i=ger_ntouche_nb-1;i>=0;i--) { if (ntouche!=ntouche_mem[i]) j=0; }
  if (j==1) {
// si la touche a varié
    if (ltouche[ntouche] != vtouche ) {
//    if (itouche < 1000 ) Serial << itouche << "=" << ntouche << endl;
      vtouche = ltouche[ntouche];
// touche valide
      if (ntouche < ger_clavier_nb) { 
        touche = vtouche;
        touche_repeat = vtouche;
        tempo_repeat=800;  // duree avant repetition
// arret répétition
      } else {
        touche_repeat = ' ';
      }
// si la touche est la màªme : repeat ?
    } else {
      if (ntouche < ger_clavier_nb && tempo_repeat == 0 ) {
        touche_repeat = vtouche;
        tempo_repeat=500;  // vitesse repetition
      }
    }
// debug
//  } else {
//  Serial << itouche6 << " " << itouche5 << " " << itouche4 << " " << itouche3 << " " << itouche2 << " " << itouche1 << " " << itouche << endl;
  }
  return;
}
#endif
// ************************************************************************************************
// ************************************************************************************************
/*
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
// ************************************************************************************************
/*
Convertit  la valeur 'float' "v" avec "nbchiffres" digits significatifs
L'"unite" est placée à la place du point décimal, avec le multiplicatreur éventuel.
Le signe - est également traité.
Donc "nbchiffres" [signe] + digits + [multiplicateur] + unite = "nbchiffres+1 à 3" caractères
Résultat dans 'buf'.
Usage :
char valuef[15];
    freq=123456.789;    formatf(freq,'c',5,valuef); // valuef="123Kc45\0"   (5->7 car)
    freq=1.23456789;    formatf(freq,'c',8,valuef); // valuef="1c2345678 \0" (8->10 car)
    freq=-0.0123456789; formatt(freq,'A',5,valuef); // valuef="-12mA345\0" (5->8 car)

	Serial << "F=" << valuef;                       // sur voie série
    lcd.setCursor(0, 0);  lcd << "F=" << valuef;    // sur LCD
    strcpy(cw_texte,valuef);                        // en MORSE

V 2013.10.30
*/
void formatt(float v,char unite,int nbchiffres,char buf[15])
{
//        mult =            0    1    2    3    4    5   6     7     8    9
//        multiplicateur =  Peta Tera Giga Mega Kilo 1   milli micro nano pico
const char multiple[10] = {'P', 'T', 'G', 'M', 'k', ' ','m',  'u',  'n', 'p'};

float vloc;
 int mult=5,nbdec,pospoint,i;
  vloc=abs(v);
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
   nbdec = nbchiffres-1; pospoint=1;              //           9.999  9.999999
   if (vloc >= 10 ) { nbdec--;  pospoint=2; }        //           99.99  99.99999
   if (vloc >= 100 ) { nbdec--; pospoint=3; }         //           999.9  999.9999
// conversion FLOAT CHAR
   dtostrf(abs(vloc),nbchiffres,nbdec,buf);
// ici, nbdec devient la longueur obtenue (rien à voir avec le nombre de décimales)
   nbdec=strlen(buf);

// si multiple, on décale   999.  pospoint=3 nbdec=4
//   Serial << "1  v=" << v << " buf=" << buf << "* pospoint=" << pospoint << endl;
   i=nbdec+1;
   if (mult!=5) {
      for (i=nbdec+1;i>pospoint;i--) buf[i]=buf[i-1];  // 999..
      buf[pospoint]=multiple[mult];       // 999m.
      buf[pospoint+1]=unite;              // 999mU
      if (nbdec<pospoint+1) nbdec++; 
   } else {
// on remplace le . par l'unité
      buf[pospoint]=unite;
      buf[nbdec]=' ';    // pour assurer toujours la même longueur
   }
   buf[nbdec+1]='\0';    // 999mU0
// négatif ?
//   Serial << "2 v=" << v << " buf=" << buf << "* nbdec=" << nbdec << endl;
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
// ************************************************************************************************
// traitement de reception sur le port Serial
// la fin de message est obtenue par timeout
// tous les caractères non imprimables (de contrôle) sont ignorés
void serial_reception() {
   static int jmsg=0;
   static unsigned long ger_serial_last_ms_char;    // pour timeout

// réception voie série sur time-out

   if (Serial.available() > 0) {
     ger_inByte = Serial.read();
     if ((0x20 <= ger_inByte) && (ger_inByte <= 0x126)) { // strip off non-ASCII, such as CR or LF
       if (jmsg > ger_msg_size-2) {
//       if (jmsg < 0 || jmsg > ger_msg_size-2) {
          jmsg=0;    // pour éviter les débordements !
       }
//	   Serial && "recu=" && jmsg && "=" && ger_inByte << endli;
       ger_msg[jmsg++] = ger_inByte;
       ger_msg[jmsg] = 0; // update end of line
     }
     ger_serial_last_ms_char = millis(); // stroke the inter character timeout.

// time-out ?
   } else {
    if (((millis() - ger_serial_last_ms_char) > ger_serial_timeout) && jmsg > 1 ) { 
      if (debug>1) Serial && F("ger_msg=") && jmsg && "=" && ger_msg && "=" && millis() && ">" && ger_serial_last_ms_char && endli;
      ger_msg_recu=jmsg;
      jmsg=0;
    }
  }

}
// ************************************************************************************************
// ************************************************************************************************
#ifdef ger_bargraph		// a définir si on veut utiliser ces fonctions
//#define creercaracterestype  //
//#define creercaracteresdemo  // si on veut un test au démarrage
// ************************************************************************************************
// création de caractères spécifiques
/* http://arduino.cc/fr/Main/LcdcreateChar
	
	creercaracterestype : 
        non défini = 5 barres simples
		1 = 5 pavés grossissants
		2 = 3 barres simples

	creercaracteresdemo : 
        non défini = rien
		affichage des caractères au démarrage sur le LCD

Autres standards :
0 barre = espace = cacactère 32
5 barres soulignées = caractère 255
*/
void creercaracteres() {

// barres simples
#ifndef creercaracterestype
byte carrepro1[8] = { B00000, B00000, B00000, B00000, B00000, B00000, B00000};	// 0 barre
byte carrepro2[8] = { B10000, B10000, B10000, B10000, B10000, B10000, B10000};	// 1 barre
byte carrepro3[8] = { B11000, B11000, B11000, B11000, B11000, B11000, B11000};	// 2 barres
byte carrepro4[8] = { B11100, B11100, B11100, B11100, B11100, B11100, B11100};	// 3 barres
byte carrepro5[8] = { B11110, B11110, B11110, B11110, B11110, B11110, B11110};	// 4 barres
byte carrepro6[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111};	// 5 barres
#define nbre_sousechelons 5
#endif

// pave grossissant et souligné
#if creercaracterestype == 1
byte carrepro1[9] = { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B10101};	// 0 barre
byte carrepro2[9] = { B00000, B00000, B00000, B00000, B10000, B10000, B10000, B10101};	// 1 barre
byte carrepro3[9] = { B00000, B00000, B00000, B11000, B11000, B11000, B11000, B10101};	// 2 barres
byte carrepro4[9] = { B00000, B00000, B11100, B11100, B11100, B11100, B11100, B10101};	// 3 barres
byte carrepro5[9] = { B00000, B11110, B11110, B11110, B11110, B11110, B11110, B10101};	// 4 barres
byte carrepro6[9] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B10101};	// 5 barres
#define nbre_sousechelons 5
#endif

#if creercaracterestype == 2
// nos nouveaux caractères, 4 niveaux
byte carrepro1[9] = { B00000, B00000, B00000, B00000, B00000, B00000, B00000, B10101};	// 0 barre
byte carrepro2[9] = { B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10101};	// 1 barre
byte carrepro3[9] = { B10100, B10100, B10100, B10100, B10100, B10100, B10100, B10101};	// 2 barres écartées
byte carrepro4[9] = { B10101, B10101, B10101, B10101, B10101, B10101, B10101, B10101};	// 3 barres écartées
byte carrepro5[1] = { 0};
byte carrepro6[1] = { 0};
#define nbre_sousechelons 3
#endif

// autres caracrtères
byte carrepro7[8] = { B01000, B10100, B01000, B00011, B00100, B00100, B00011 };	// °C
byte carrepro8[8] = { B11101, B10111, B10110, B10000, B10111, B10001, B11111 };	// logo GR

// NB: ne pas programmer le caractère 0, car c'est la fin de chaines de caractères
// apprend les caractères à l'écran LCD
  lcd.createChar(1, carrepro1);
  lcd.createChar(2, carrepro2); 
  lcd.createChar(3, carrepro3); 
  lcd.createChar(4, carrepro4); 
  lcd.createChar(5, carrepro5);
  lcd.createChar(6, carrepro6); 
  lcd.createChar(7, carrepro7); 
  lcd.createChar(8, carrepro8);

#ifdef creercaracteresdemo
// TEST CARACTERES
  for (int i=1;i<9;i++) {
    lcd.setCursor(i-1, 0);  lcd && char(i); // lcd.write((uint8_t) char(i)); //affiche le caractère
    lcd.setCursor(i-1, 1);  lcd && i;
	delay(100);
  }
	delay(1000);
/*
// demo bargraph
	for (i=0;i<40;i++) {
      lcd.setCursor(8, 0);  lcd && bargraph_barre_valeur (i, 0., 40., 0., 10., 0., 10., 8, 3, char(8), x);
	  delay(100);
    }
*/
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
*/
char* bargraph_barre_valeur (int valeur, float e_mini, float e_maxi, float s_mini, float s_maxi, float b_mini, float b_maxi, int nbcar, int nbchiffres, char unite, float &sortie) {

static char local[21];
char buf[15];
int i,j,nbre_echelons;
    
    sortie = ((valeur - e_mini) * (s_maxi - s_mini) / (e_maxi - e_mini) ) + s_mini;
//
    nbre_echelons = (float) nbcar * nbre_sousechelons * (sortie - b_mini) / (b_maxi - b_mini);
    nbre_echelons = constrain(nbre_echelons, 0, nbcar * nbre_sousechelons);
// nettoyage du buffer
    memset (local,char(1),nbcar); // raz pavés blancs (30448)
//	for (j=0;j<nbcar;j++) {local[j] = char(1); } // raz pavés blancs (30458)
	

// affichage d'un bargraph composé de nbcar caractères 'bloc' maximum
    i = nbre_echelons/nbre_sousechelons;
    if (i>0) memset (local,char(nbre_sousechelons+1),i); // mise en place des blocs pleins

//    Serial << " nbre_echelons=" << nbre_echelons << " ";  // debug/test (sans LCD !!!)
//    for (j=0;j<i;j++) Serial << "#";  // bargraphe - debug/test (sans LCD !!!) 

// mise en place d'un caractère programmé parmi  
    if (i>0 && i<nbcar) local[i] = char(nbre_echelons%nbre_sousechelons+1);

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
// en surimpression
        if (nbchiffres > 0) {
			for (j=0;j<i;j++) {
				if ( sortie > (b_mini+b_maxi)/2 ) {
					local[j] = buf[j];
				} else {
					local[nbcar-i+j] = buf [j];
				}
			}
			i=0;		// pour raz correcte !
// en plus , derrière
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
#endif // ger_bargraph
// ************************************************************************************************
  // traitement I2C
/*
  if (i2c_nbr > 0) {
    strncpy(filename,i2c_bufr,i2c_nbr);
    result = MP3player.playMP3(filename);
// check result, see readme for error codes.
            if(result != 0) {
              Serial.print(F("Error code: "));
              Serial.print(result);
              Serial.println(F(" when trying to play track"));
            } else {
// envoi OK à I2C

            }

  }
*/
// ************************************************************************************************
// traitement de reception sur le port Serial
// la fin de message est '*' ou CR ou LF ou tout autre caractère non-imprimable (< espace)

/*
// synchro de l'horloge :
void serial_reception() {
 int i,j,k;
 static int jmsg;

// reception message se terminant par LF ou CR ou *
   j=Serial.available();
   if (j > 0) {
     if (jmsg < 0 || jmsg>=ger_msg_size) jmsg=0;
// récupération de tous les caractères disponibles
     for (i=0;i<j;i++) {
       ger_msg[jmsg] = Serial.read();
//	   if (debug>0) Serial << "rec:" << ger_msg[jmsg] << "," << jmsg << endl;  
// fin de message
       if (ger_msg[jmsg] == '*' || ger_msg[jmsg] < ' ') { //== 0x0A || ger_msg[jmsg] == 0x0D) {
			ger_msg[jmsg] = '*';
			ger_msg_recu=jmsg;         
			jmsg=0;
// on pointe sur le suivant
       } else {
         jmsg++; if (jmsg>=ger_msg_size) jmsg=0;
       }
     }
   }
// TRAITEMENT DES MESSAGES RECUS a inclure juste après
// #include "msg_traitement.h" // spécifique à chaque application
        
}
*/   
// T1262347200  //noon Jan 1 2010  ( secondes depuis le 01/01/1970
//  1261440000 sec/40 ans (le décalage donne 10 jours, soit bien 10 années bissextiles)
//       86400 sec/jour
//    31536000 sec/an
// T1293969600  //noon Jan 1 2011
// T1325505600  //noon Jan 1 2012
// T1357041600  //noon Jan 1 2013
// T1388577600  //noon Jan 1 2014 
// ************************************************************************************************
/*
// conversion FLOAT CHAR
//   dtostrf(value,nbchiffres,nbdecimales,buffer);
//   dtostrf(3.141592,5,2,buffer);  // " 3.14"
//   dtostrf(3.141592,5,0,buffer);  // "    3"
//   dtostrf(3.141592,0,0,buffer);  // "3"

Convertit la valeur 'float' "v" avec "nbchiffres" digits significatifs suivi de l'"unite"
Donc "nbchiffres" caractères
Résultat dans 'buf'.
Usage :
char valuef[15];
    freq=123456.789;    formati(freq,'c',6,valuef); // valuef="123456\0"
    freq=12.3456789;    formati(freq,'c',8,valuef); // valuef="       12\0"
    Serial << "F=" << valuef;                       // sur voie série
    lcd.setCursor(0, 0);  lcd << "F=" << valuef;    // sur LCD
    strcpy(cw_texte,valuef);                        // en MORSE
*/
/*
void formati(int v,int nbchiffres,char buf[15])
{
//      dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);

 float vloc;
  vloc=v;
   if (vloc < pow(10,nbchiffres)) nbdec++;          //           99.99  99.99999
   if (vloc < 10 ) nbdec++;          //           9.999  9.999999
// conversion FLOAT CHAR
   dtostrf(vloc,nbchiffres,nbdec,buf);
// ici, nbdec devient la longueur obtenue (rien à  voir avec le nombre de décimales)
   nbdec=strlen(buf);
   if (mult!=5) {
     buf[nbdec+1]=unite; buf[nbdec]=multiple[mult];
   } else {
     buf[nbdec]=unite; buf[nbdec+1]=multiple[mult];
   }
// fin de CHAR
   buf[nbdec+2]='\0';
//   Serial << "*buf=" << buf << "*l=" << strlen(buf) << "*" << endl;
   return;
}
*/
// ************************************************************************************************
// pour transférer des données entre 2 buffers, avec décalage des débuts
/* exemples :
int buff_transfert (entree, 0, 10, sortie, 0, 10)
        sortie = ................
        entree = ABCDEFGHIJKLMNOP     
        sortie = ABCDEFGHIJ......
//               0123456789012345
int buff_transfert (entree, 4, 6, sortie, 0, 5)
        sortie = ................
        entree = ABCDefghijKLMNOP     
        sortie = efghi...........
//               0123456789012345
int buff_transfert (entree, 4, 6, sortie, 2, 5)
        sortie = ................
        entree = ABCDefghijKLMNOP     
        sortie = ..efghi.........
//               0123456789012345
//
//  buff_transfert (socketrecp, 4, socketrecp_ok, busp_fifo[busp_fifo_in].trame, 5, 16);
*/
/*
int buff_transfert (char* entree, int debut, int longueur, char* slot, int slotdebut, int nb_maxi) {
      int l,j;
// "longueur_maxi" est le nombre maximal de caractères admis par "slot"
   l = longueur-debut; if (l > nb_maxi) { l = nb_maxi; }
   if (l > 0) {
// copie "longueur" caractères de "entree" dans "slot"
      for (j = 0; j < l; j++) { slot[j+slotdebut] = entree[j+debut];  }
      return true;
// rien à faire
      } else {
      return false;
   }
}
*/
/*
//==============================================================================
int asc2tob(int asc1,int asc0)
// transforme 2 octets ASCII 00..FF en valeur numérique 0-255
{
// Convert lowercase to uppercase
   if(asc1 > 'F') asc1 -= 'a'-'A';
   if(asc0 > 'F') asc0 -= 'a'-'A';

// Convert only 0-F
//   if((asc1 > '9' || asc1 < '0') && (asc1 > 'F' || asc1 < 'A')) asc1 = '0'; 
//   if((asc0 > '9' || asc0 < '0') && (asc0 > 'F' || asc0 < 'A')) asc0 = '0'; 

// Convert 0-9, A-F to 0x0-0xF
   if(asc1 > '9') 
      asc1 -= 'A' - 10;
   else
      asc1 -= '0';

   if(asc0 > '9') 
      asc0 -= 'A' - 10;
   else
      asc0 -= '0';

// Convert only 0x0-0xF
   if(asc1 > 0x0F || asc1 < 0) asc1 = 0; 
   if(asc0 > 0x0F || asc0 < 0) asc0 = 0; 

// Concatenate
   return (asc1<<4) |  asc0;
}
*/
// ************************************************************************************************
// ARCHIVES, NOTES ...
// ************************************************************************************************
/*
// AUTRES CONVERSIONS
char* dtostrf  ( double  __val,  
  signed char  __width,  
  unsigned char  __prec,  
  char *  __s  
 ) 
 
 char* itoa  ( int  __val,  
  char *  __s,  
  int  __radix  
 ) 

char* ltoa  ( long int  __val,  
  char *  __s,  
  int  __radix  
 ) 

dtostrf(vloc,nbchiffres,nbdec,cw_texte);
   nbdec=strlen(cw_texte);
   cw_texte[nbdec]=multiple[mult];
   cw_texte[nbdec+1]=unite;
   cw_texte[nbdec+2]='\0';
   }
*/
/*
// Transforme la date et l'heure "lisible" en format UNIX pour mise à  l'heure et date
time_t setupArduinoTime(int y,int m, int j, int h, int mi, int se) {
  tmElements_t tm;
  tm.Year = y-1970;
  tm.Month = m;
  tm.Day = j;
  tm.Hour = h;
  tm.Minute = mi;
  tm.Second = se;
  time_t time = makeTime(tm);
  return time;
}
//+ voici le s/p original qui provient d'une bibliothèque pour GPS :
time_t gpsTimeToArduinoTime(){
// returns time_t from gps date and time with the given offset hours
  tmElements_t tm;
  int year;
  gps.crack_datetime(&year, &tm.Month, &tm.Day, &tm.Hour, &tm.Minute, &tm.Second, NULL, NULL);
  time_t time = makeTime(tm);
  return time + (offset * SECS_PER_HOUR);
}
*/
// ************************************************************************************************
// ************************************************************************************************



