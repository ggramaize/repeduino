// GoodFields_ger
// version : 07/07/2015 : version light optionnelle corrigée
// version : 11/09/2014 : amélioration milliseconde, version light optionnelle
// version : 30/03/2014 : mise au point gmt-decalage_determine
// version : 10/03/2014 amélioration du calage des tempos, suppression 250ms et 500ms
/*
// ************************************************************************************************
// GESTION DU TEMPS : dans votre loop() , mettre au moins ceci : (gestion de 'milliseconde')

   ger_top_centieme();

#define ger_top_light 0 // pour alléger le code  (pas de millisseconde, top_55, top_minute) # 200 octets

// ***************
// TEMPORISATIONS :
// pour gérer les tempos précises à 1, 10, 100ms, 1000ms, les valeurs de tempos sont en millisecondes
 if (top_1) {
   GER_TEMPO (tempo_X1,top_1)	// top_1 est un int contenant les ms passées
   top_1 = 0;
 }
// Nb : en général, top_1 = 2 (du à la gestion des ms de l'ARDUINO)
if (top_10) {
   GER_TEMPO (tempo_X,top_10)	// top_10 est un int contenant les ms passées
   top_10 = 0;
   action;
 }
 if (top_100ms) {
   GER_TEMPO (tempo_X,top_100ms)	// top_100ms est un int contenant les ms passées
   top_10 = 0;
   action;
 }

// pour gérer les tempos plus lentes, par exemple en SECONDES :
 if (top_1000ms) {
    GER_TEMPOL (tempo_X1000)
// ou
    GER_TEMPOL (tempo_en_ms,top_1000ms) // top_1000ms est un int contenant les ms passées
    top_1000ms = 0;
 }
// pour gérer les évenements plus lents (chaque minute)
 if (top_55s ou top_60000ms) {
    top_55s ou top_60000ms = false;	// true/false
    GER_TEMPOL (tempo_X)
    action;
 }

// utilisation d'une temporisation simple :
	if (condition) tempo_simple = reset_valeur;
	if (tempo_simple==0) {		// tempo terminée
	   action_de_fin_de_tempo;
	}

// utilisation d'une temporisation cyclique (cadencement):
	if (tempo_cyclique ==0) {	// tempo finie
	   tempo_cyclique = valeur_de_reset;		// relance de la tempo
	   action_de_fin_de_tempo;
	}

// ***************
// HORODATAGE
Pour faciliter le travail, les routines suivantes sont disponibles :
 'horodatage_X()' retourne un 'char*' contenant ce qu'il y a entre crochets ci-dessous,
    et terminé par un espace et un '\0'. L = nombre de caractères utiles (y compris ' ' non compris '\0')
       12345678901234567890
 L X   contenu-------------\0
20 d  [20/03/2014 17:23:28 ]
18 e  [20/03/14 17:23:28 ]
18 x  [01/04/13;10:15:25;]	// pour logger vers feuille excel (.csv)
 9 h  [17:23:28 ]
10 c  [23:28.053 ]
 4 m  [.053 ]
16 l  [Jeu 20 Mar 2014 ]
 9 hx [17:23:28 ]
18 ex [20/03/14 17:23:28 ]
       12345678901234567890
15 lcd[20/03 17:23:28x]
      [20/03 16:23 UTC]
	  [Jeu 20 Mar 2014]
       12345678901234567890

NB 1 : le mode 'e' est sympa pour mettre dans un fichier excel ou .csv (terminer par ';' et remplacer les . par des ,)
NB 2 : pour minimiser votre code, n'utilisez qu'un seul format X dans un programme

// ***************
// HEURE d'ETE et d'HIVER

// ************************************************************************************************
// NOTES DIVERSES

Quelques valeurs intéréssantes. Voir aussi le s/p 'horodatage'

time_t t;
 setTime (17,23,00,20,03,2014);
 t = now();
 Serial && weekday(t) && " " && dayStr(weekday(t)) && " " && dayShortStr(weekday(t)) && endli;
 Serial && month(t) && " " && monthStr(month(t)) && " " && monthShortStr(month(t)) && endli;

A FAIRE :	voir dans Time_ger.h comment est géré : (pour la DS1307 et le recepteur DCF-77)

timeStatus();       // indicates if time has been set and recently synchronized
                    // returns one of the following enumerations:
    timeNotSet      // the time has never been set, the clock started at Jan 1 1970
    timeNeedsSync   // the time had been set but a sync attempt did not succeed
    timeSet         // the time is set and is synced

	Time and Date values are not valid if the status is timeNotSet. Otherwise values can be used but
the returned time may have drifted if the status is timeNeedsSync.

setSyncProvider(getTimeFunction);  // set the external time provider
setSyncInterval(interval);         // set the number of seconds between re-sync

timeStatus_t timeStatus(); // indicates if time has been set and recently synchronized
void    setSyncProvider( getExternalTime getTimeFunction); // identify the external time provider
void    setSyncInterval(time_t interval); // set the number of seconds between re-sync

int horloge_synchronisee=0;  // 1=indique si la mise à  l'heure a été effectuée
int horloge_synchro_tempo=0;
int horloge_synchro_tempo_reset=61; // temps en minutes après lequel l'horloge est supposée non synchronisée
// ************************************************************************************************
*/

#ifndef ger_time
#define ger_time "OK"

// *** TEMPS *** gestion du temps
unsigned int top_1=0;         // nb de ms pour traitements au 1/1000 seconde dans le P/P
unsigned int top_10=0;        // nb de ms pour traitements au 1/100 seconde dans le P/P
unsigned int top_100ms=0;     // top pour traitements au 1/10 seconde dans le P/P
unsigned int milliseconde;        // millisecondes depuis début de seconde
unsigned long previous1=0, previous10, previous100;
#ifndef ger_top_light
int top_1000ms=0;    // top pour traitements à la seconde dans le P/P
int top_60000ms=0;   // top pour traitements à la minute dans le P/P
int top_55s=0;   	 // top pour traitements 5 secondes avant la minute dans le P/P
unsigned int previousSecond;      // mémoire secondes de l'horloge
unsigned long previous1000;
#endif
// NOTE : true c'est 1 et false c'est 0
// int variable
// if (variable) Serial.println("TRUE")  // affiche TRUE si variable <> 0

#define GER_TEMPO(tempo, top) if (tempo > top ) { tempo -= top; } else { tempo = 0; }
#define GER_TEMPOL(tempo) if (tempo > 0 ) { tempo --; }

// On définit sur les 2 lignes suivantes les décalages de l'heure locale par rapport à l'heure UTC
// UTC = heure locale - gmt_decalage
#define gmt_ete 2
#define gmt_hiver 1
// au démarrage on a cette valeur par défaut
#define gmt_startup 0
int gmt_decalage = gmt_startup;    // heure d'été/hiver non réglée ( heure locale = UTC )
// NB : ceci est établi pour le fuseau horaire CET/CEST Madrid-Paris-Berlin
// Pour un autre fuseau horaire, il faudrait prendre les bonnes valeurs, et le test (dans la routine ger_decalage_determine) pourrait être à modifier également ...
time_t tutc;    // pour calcul et manipulation du temps UTC (tutc =  now() - gmt_decalage*3600;)

// ************************************************************************************************
// ************************************************************************************************
/* Programme au -ieme de seconde, gestion des temporisations
 Questionnement :
 Le débordement du compteur en (unsigned long) a lieu tous les 50 jours environ ...
 lorsqu'il arrive à  la valeur 2^32-1 = 4294967295 = 49j 17h 2m 47s
 delta devient négatif, euh non, très grand, exemple :
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

#ifndef ger_top_light
// init des mémoires au démarrage ( car millis() a déjà avancé !)
        if (previous1 == 0) {
            previous1 = currentMillis;
            previous10 = currentMillis;
            previous100 = currentMillis;
            previousSecond = second();
            milliseconde = 0;
            delta=1;
        }
#endif
// ms dans la seconde
//      if (milliseconde < 999) milliseconde ++; //+= delta;
//		if (milliseconde > 999) Serial && "milliseconde = " && milliseconde && endli;
//		if (milliseconde > 999) milliseconde = 999;

// save the last time you come here
        previous1 = currentMillis; //+1-delta;
        top_1 += delta;		// on cumule si on n'a pas remis à 0 dans le P/P

// Top Centième de secondes
        delta = currentMillis - previous10;
        if (delta >= 10) {
            //	    if (delta >10) Serial && "top_10ms: " && delta && " pre=" && previous10 && endli;
// save the last time you come here
            previous10 = currentMillis; //+10-delta;
            //	    if (delta >10) Serial && "--------: " && delta && " pre=" && previous10 && endli;
            top_10 += delta;

#ifndef ger_top_light
// Top Seconde synchronisé au 0 à 10ms près
            if (second() != previousSecond) {
                previous1000 = currentMillis;
                previousSecond = second();
                top_1000ms = 1000;
                milliseconde = 0;
// Top 55 secondes
                if (previousSecond == 55) {
                    top_55s = true;
                }
// Top Minute
                if (previousSecond == 0) {
                    top_60000ms = true;
                }	// FIN minute
            }	// FIN seconde
#endif
// Top Dixième de seconde, synchronisé avec les secondes
            delta = currentMillis - previous100;
#ifndef ger_top_light
            if ((delta >= 100) or top_1000ms) {
#endif
#ifdef ger_top_light
            if (delta >= 100) {
#endif
                //	    if (delta >100) Serial && "top_100ms: " && delta && " pre=" && previous100 && endli;
                previous100 = currentMillis; // +100-delta;
                //	    if (delta >100) Serial && "---------: " && delta && " pre=" && previous100 && endli;
                top_100ms += delta;		// on cumule si on n'a pas remis à 0 dans le P/P
            }	// FIN 100ms
        }	// FIN 10ms

// ms dans la seconde
#ifndef ger_top_light
        milliseconde = currentMillis - previous1000;
#endif
    }	// FIN 1ms
    return;
}

/*
// indicateur si horloge synchroniée
				if (horloge_synchro_tempo < 1) {
					horloge_synchro_tempo = horloge_synchro_tempo_reset;
	//          	horloge_synchronisee = 0;  // indique si la mise à  l'heure a été effectuée
				} else {
					horloge_synchro_tempo--;
				}
*/
// ************************************************************************************************
// ************************************************************************************************
// routine de mise en forme de :
// value en 2 chiffres avec 0 devant si < 10
// ajout de sep si >= ' ' (espace)
char* doubledigit(int value, char sep) {
    static char local[4];
//if (value > 0 & value < 100) {
    local[0]='0'+value/10;
    local[1]='0'+value%10;
    if (sep >= ' ') {
        local[2]=sep;
        local[3]='\0';
    } else {
        local[2]='\0';
    }
//	Serial && value && sep && "=doubledigit=" && local && "=" && endli;
//}
    return local;
}
// ************************************************************************************************
// 'hx' heure
// '10:15:28 0'
// '1234567890'

// avec argument, c'est l'heure transmise
char* horodatage_hx (time_t t) {
    static char out[10];
    memset (out,'\0',sizeof(out));
// HEURE:MINUTE:SECONDES
    strcpy ( out, doubledigit(  hour(t),':') );
    strcat ( out, doubledigit(minute(t),':') );
    strcat ( out, doubledigit(second(t),' ') );
    strcat ( out, "\0" );
    return out;
}
// sans argument, c'est l'heure actuelle
char* horodatage_h () {
    return horodatage_hx (now());
}

// ************************************************************************************************
// 'd' date et heure
// 01/04/2013 10:15:28_0
// 123456789012345678912
char* horodatage_d () {
    static char out[22];
    char valuef[5];
    time_t t = now();
    int y;

    memset (out,'\0',sizeof(out));
// JOUR/MOIS/ANNEE
    strcpy ( out, doubledigit(day(t),'/') );
    strcat ( out, doubledigit(month(t),'/') );
    dtostrf(year(t),4,0,valuef);
    strcat( out, valuef );
    strcat( out, " \0" );
    strcat( out, horodatage_h() );
    strcat( out, "\0" );

    return out;
}
// ************************************************************************************************
// 'x' date et heure pour excel
// 01/04/13;10:15:25;0
// 123456789012345678912
char* horodatage_x () {
    static char out[19];
//char valuef[5];
    time_t t = now();
    int y;
    memset (out,'\0',sizeof(out));
// JOUR/MOIS/ANNEE
    strcpy ( out, doubledigit(day(t),'/') );
    strcat ( out, doubledigit(month(t),'/') );
    y = year(t);
    if (y>=2000) y = y- 2000;
    strcat ( out, doubledigit(y,';') );
// HEURE:MINUTE:SECONDES
    strcat ( out, doubledigit(  hour(t),':') );
    strcat ( out, doubledigit(minute(t),':') );
    strcat ( out, doubledigit(second(t),';') );
    strcat ( out, "\0" );
    return out;
}
// ************************************************************************************************
// 'ex' date et heure
// 01/04/13 10:15:28_0
// 1234567890123456789
char* horodatage_ex (time_t t) {
    static char out[19];
    int y;
    memset (out,'\0',sizeof(out));
// JOUR/MOIS/ANNEE
    strcpy ( out, doubledigit(day(t),'/') );
    strcat ( out, doubledigit(month(t),'/') );
    y = year(t);
    if (y>=2000) y = y-2000;
    if (y>=1900) y = y-1900;
    strcat ( out, doubledigit(y,' ') );
    /* -180 octets
    	dtostrf(day(t),2,0,valuef); if(valuef[0]==' ') valuef[0]='0';
    		strcpy( out, valuef ); strcat( out, "/" );
        dtostrf(month(t),2,0,valuef); if(valuef[0]==' ') valuef[0]='0';
    		strcat( out, valuef ); strcat( out, "/" );
        dtostrf(year(t)-2000,2,0,valuef); if(valuef[0]==' ') valuef[0]='0';
    		strcat( out, valuef ); strcat( out, " " );
    */
    strcat( out, horodatage_hx(t) );
    strcat( out, "\0" );
    return out;
}

// sans argument, c'est l'heure actuelle
char* horodatage_e () {
    return horodatage_ex (now());
}
// ************************************************************************************************
// 'l' jour literal
// Jeu 20 Mar 2014_0
// 12345678901234567
char* horodatage_l () {
    static char out[18];
    char valuef[5];
    time_t t = now();
    memset (out,'\0',sizeof(out));
    strcpy ( out, dayShortStr(weekday(t)) );
    strcat( out, " " );
    strcat ( out, doubledigit(day(t),' ') );
// -50 octets    dtostrf(day(t),2,0,valuef); if(valuef[0]==' ') valuef[0]='0';
//		strcat( out, valuef ); strcat( out, " " );
    strcat( out, monthShortStr(month(t)) );
    strcat( out, " " );
    dtostrf(year(t),4,0,valuef);
    strcat( out, valuef );
    strcat( out, " \0" );
    return out;
}
// ************************************************************************************************
// 'lcd' special lcd alterné heure / UTC / date (15 caractères)
/*  1234567890123456
   [20/03 10:45:27x]
   [20/03 09:45:25U]
   [Jeu 20 Mar 2014]
    x =e pour été =h pour hiver =U pour heure UTC/GMT
La 1° ligne est affichée pendant les 'locale' secondes
La 2° ensuite, sur une période de 'utc' secondes
La 3° ensuite, sur une période de 'ddat' secondes

Par exemple :
lcd.setCursor(0, 1); lcd && horodatage_lcd (8,0,2);
Affichage : heure 8s, date 2s

lcd.setCursor(0, 1); lcd && horodatage_lcd (5,2,3);
Affichage : heure LOCALE 5s, heure UTC 2s, date 3s
0 1 2 3 4 5 6 7 8 9
L L L L L U U d d d

Les : sont remplacés par ' ' si millisecondes > 500ms
Les : sont remplacés par '^' en heure UTC
*/
char* horodatage_lcd (int locale, int utc, int ddat) {
    static char out[16];
    time_t t;
    int y,st,utcok;
    memset (out,'\0',sizeof(out));
    t = now();
    st = second()%(locale+utc+ddat);
    utcok = false;
    if ( utc > 0 and st >= locale and st < (locale+utc) ) {
        t = t - gmt_decalage*3600;
        utcok=true;
    }
    if ( ( locale > 0 or utc > 0 ) and st < (locale+utc) ) {
// JOUR/MOIS
        strcpy ( out, doubledigit(day(t),'/') );
        strcat ( out, doubledigit(month(t),' ') );
// heure minutes secondes LOCALE
        strcat( out, horodatage_hx(t) );
        if ( !utcok ) {
            out[14]= ((gmt_decalage==gmt_ete)?'e':'h');
// heure minutes UTC
        } else {
//	   strncat( out, horodatage_hx(t), 5 );
//	   strcat( out, "U");
            if (out[8] == ':') {
                out[8]= '^';
                out[11]= '^';
            }
            out[14]= 'U';
        }
// effecement séparateurs si ms > 499
        if (milliseconde > 499) {
            out[8]= ' ';
            out[11]= ' ';
        }
    } else {
// jour No mois	année en lettres
        strcpy ( out, horodatage_l());
    }
    strcat( out, "\0" );
//	Serial && locale && "<" && utc && "<" && st && " /" && toutes && " :" && out && endli;
    return out;
}
// ************************************************************************************************
// 'm' .dcm
// .999_0
// 123456
char* horodatage_m () {
    static char out[6];
    char valuef[4];
//time_t t = now();
    memset (out,'\0',sizeof(out));
// MILLIEMES
    dtostrf(milliseconde,3,0,valuef);
    if (valuef[0]==' ') valuef[0]='0';
    if (valuef[1]==' ') valuef[1]='0';
    strcpy( out, "." );
    strcat( out, valuef );
    strcat( out, " \0" );
    return out;
}
// ************************************************************************************************
// 'c' mm:ss.dcm
// 15:28.999_0
// 12345678901
char* horodatage_c () {
    static char out[11];
    time_t t = now();
    memset (out,'\0',sizeof(out));
// MINUTE:SECONDES
    strcpy ( out, doubledigit(minute(t),':') );
    strcat ( out, doubledigit(second(t),0) );
// MILLIEMES
    strcat( out, horodatage_m() );
    strcat( out, "\0" );
    return out;
}
// ************************************************************************************************
// 'blind' pour synthhèse voicale
// Mercerdi 14 Janvier 2015 10 heure 45 minute
// 234567 10 234567 20 234567 30 234567 40 234567 50
char* horodatage_blind () {
    static char out[46],valuef[10];
    strcpy ( out, dayStr(weekday()) );
    strcat( out, " " );
    strcat ( out, itoa(day(),valuef,10) );
    strcat( out, " " );
    strcat ( out, monthStr(month()) );
    strcat( out, " " );
    dtostrf(year(),4,0,valuef);
    strncat( out, valuef, 4 );
    strcat( out, " " );

    strcat ( out, itoa(hour(),valuef,10) );
    strcat ( out, " heure " );
    strcat ( out, itoa(minute(),valuef,10) );
    strcat ( out, " minute\0" );

    return out;

}
// ************************************************************************************************
// détermination de l'heure d'été/hiver : positionne la variable gmt_decalage
/* appel conseillé dans le top minute de cette façon :
// chaque minute :
   if (top_60000ms) {
	   top_60000ms=false;
       gmt_decalage_determine();
//    GER_TEMPOL (tempo_minute)
   }  // fin 60000.

// affichage heure locale et UTC et été / hiver:
time_t tutc = now() - gmt_decalage*3600;
      Serial && "local=" && horodatage_d() && "UTC=" && horodatage_hx(tutc) && endli;
      Serial && ((gmt_decalage==gmt_ete)?" ete":((gmt_decalage==gmt_hiver)?" hiv":" ???")) && endli;

 Voir listing de sortie de '201_test_gertime' en fin de listing
*/
//
void gmt_decalage_determine() {
    int mem_gmt_decalage;
// Serial && "gmt_decalage_determine" && endli;
    if ( ((hour() == 2 | hour() == 3) & minute() == 0) | (gmt_decalage == gmt_startup) ) // != gmt_ete & gmt_decalage != gmt_hiver) )
// Serial && "gmt_decalage_determine:GO" && endli;

// l'ARDUINO a son horloge sur l'heure locale, donc on calcule UTC
        tutc = now() - gmt_decalage*3600;
    mem_gmt_decalage = gmt_decalage;

    /*
         if  (month(tutc) == 3 & (day(tutc)-weekday(tutc)) < 24) Serial && "cas 1 hiver" && endli;
         if  (month(tutc) == 3 & ((day(tutc)-weekday(tutc)) == 24) & hour(tutc) >= 1) Serial && "cas 2 ete" && endli;
         if  (month(tutc) == 3 & (day(tutc)-weekday(tutc)) > 24) Serial && "cas 3 ete" && endli;
         if  (month(tutc) > 3 & month(tutc) < 10) Serial && "cas 4 ete franc" && endli;
         if  (month(tutc) == 10 & (day(tutc)-weekday(tutc)) < 24 ) Serial && "cas 5 ete" && endli;
         if  (month(tutc) == 10 & ((day(tutc)-weekday(tutc)) == 24) & hour(tutc) <= 1) Serial && "cas 6 ete" && endli;
         if  (month(tutc) == 10 & (day(tutc)-weekday(tutc)) > 24 ) Serial && "cas 7 hiver" && endli;

    weekday	 2  3  4  5  6  7  1  2  3  4  5  6  7  1  2  3  4  5  6  7  1  2  3  4  5  6  7  1
    	     L  m  M  J  V  S  D  L  m  M  J  V  S  D  L  m  M  J  V  S  D  L  m  M  J  V  S  D
    day           20 21 22 23 24 25 26 27 28 29 30 31
                     20 21 22 23 24 25 26 27 28 29 30 31
                        20 21 22 23 24 25 26 27 28 29 30 31
                           20 21 22 23 24 25 26 27 28 29 30 31
                              20 21 22 23 24 25 26 27 28 29 30 31
                              19 20 21 22 23 24 25 26 27 28 29 30 31
                              18 19 20 21 22 23 24 25 26 27 28 29 30 31

    weekday	 2  3  4  5  6  7  1  2  3  4  5  6  7  1  2  3  4  5  6  7  1  2  3  4  5  6  7  1
    	     L  m  M  J  V  S  D  L  m  M  J  V  S  D  L  m  M  J  V  S  D  L  m  M  J  V  S  D
    day-weekday   16 16 16 16 16 23 23 23 23 23 23 23 30
                     15 15 15 15 22 22 22 22 22 22 22 29 29
                        14 14 14 21 21 21 21 21 21 28 28 28
                           13 13 20 20 20 20 20 20 27 27 27 27
                              12 19 19 19 19 19 19 26 26 26 26 26
                              11 18 18 18 18 18 18 25 25 25 25 25 25
                              10 17 17 17 17 17 17 17 24 24 24 24 24 24 24
    */
// on teste sur l'heure UTC, qui ne varie pas, elle !
// fuseau horaire CET/CEST Madrid-Paris-Berlin
    if ( (month(tutc) == 3 & (( weekday(tutc) == 1 & (day(tutc)-weekday(tutc)) >= 24 & hour(tutc) >= 1 )
                              | ( weekday(tutc) > 1  & (day(tutc)-weekday(tutc)) >= 24) ) )
            | (month(tutc) > 3 & month(tutc) < 10)
            | (month(tutc) == 10 & (( weekday(tutc) > 1  & (day(tutc)-weekday(tutc) < 24) )
                                    | ( weekday(tutc) == 1 & (day(tutc)-weekday(tutc)) >= 24 & hour(tutc) < 1 ) ) ) ) {
        gmt_decalage = gmt_ete;
    } else {
        gmt_decalage = gmt_hiver;
    }

// si le décalage est différent de celui que l'on a trouvé en entrée
// ( en particulier au démarrage gmt_decalage=gmt_startup )
    if (mem_gmt_decalage != gmt_decalage) {
#ifdef RTC1307
// remise à l'heure ARDUINO à partir de l'horloge RTC DS1307
        DateTime nowx = RTC.now();
// convertir nowx pour remise à l'heure ARDUINO
        setTime(nowx.unixtime() + gmt_decalage*3600);
#else
// remise à l'heure ARDUINO
// on ne décale que si mem_gmt_decalage != gmt_startup , ce qui veut dire que l'on change d'heure
// si mem_gmt_decalage == gmt_startup, on est  en démarrage (décalage inconnu)
// 7200*gmt_decalage-10800 prends les valeurs 3600 et -3600
        if ( mem_gmt_decalage != gmt_startup ) adjustTime(3600*(gmt_decalage-mem_gmt_decalage));
#endif
    }

    /* POUR DEBUG
    	 tutc = now() - gmt_decalage*3600;
         Serial && "***** local=" && horodatage_d() && "UTC=" && hour(tutc) && ":" && minute(tutc) && ":" && second(tutc) && " GMT=" && gmt_decalage && ((gmt_decalage==2)?" ete":((gmt_decalage==1)?" hiv":" ???")) && endli;
    */
    return;
}
/*
201_test_gertime
01:59:52   grafcet_a=0
horodatage_d=01/01/2014 01:59:52
horodatage_e=01/01/14 01:59:52
horodatage_h=01:59:52
horodatage_c=59:52.001
horodatage_m=.001
horodatage_l=Mer 01 Jan 2014
01/01/2014 01:59:53   gmt=0 ?????
01/01/2014 01:59:54   gmt=0 ?????
01/01/2014 01:59:55   gmt=0 ?????
01/01/2014 01:59:56   gmt=0 ?????
01/01/2014 01:59:57   gmt=0 ?????
01/01/2014 01:59:58   gmt=0 ?????
01/01/2014 01:59:59   gmt=0 ?????
01/01/2014 02:00:00   gmt=1 hiver
01/01/2014 02:00:01   gmt=1 hiver
01/01/2014 02:00:02   gmt=1 hiver
01/01/2014 02:00:03   gmt=1 hiver
01/01/2014 02:00:04   gmt=1 hiver
01/01/2014 02:00:05   gmt=1 hiver
===== passage en ETE ?
30/03/2014 01:59:57   gmt=1 hiver
30/03/2014 01:59:58   gmt=1 hiver
30/03/2014 01:59:59   gmt=1 hiver
30/03/2014 03:00:00   gmt=2 *ete*
30/03/2014 03:00:01   gmt=2 *ete*
30/03/2014 03:00:02   gmt=2 *ete*
===== passage en HIVER ?
26/10/2014 02:59:57   gmt=2 *ete*
26/10/2014 02:59:58   gmt=2 *ete*
26/10/2014 02:59:59   gmt=2 *ete*
26/10/2014 02:00:00   gmt=1 hiver
26/10/2014 02:00:01   gmt=1 hiver
26/10/2014 02:00:02   gmt=1 hiver
*/
// ************************************************************************************************
// ************************************************************************************************
// routines obsolètes (depuis JAN 2014), à conserver pour compatibilité
// ************************************************************************************************
// affichage horloge dans un char
// OBSOLETE, bien que compatible !, utilisez les horodatage_x ()
// nouveaux modules plus économiques en mémoire !

/*
char* horodatage(char format) {
   if (format == 'd') { return horodatage_d(); }
   if (format == 'e') { return horodatage_e(); }
   if (format == 'h') { return horodatage_h(); }
   if (format == 'l') { return horodatage_l(); }
   if (format == 'c') { return horodatage_c(); }
   if (format == 'm') { return horodatage_m(); }
   if (format == 'x') { return horodatage_x(); }
  return "Err";
}
*/

#endif