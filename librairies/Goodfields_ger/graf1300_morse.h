/* V05/11/2013
	Fonctionnement :
 Pour tester si la CW est disponible : if (grafcet_cw == 1) {

 Placer dans 'cw_texte' la chaine de caractères à mettre :
    strcpy (cw_texte,"Bonjour");        // manipulera BONJOUR 
    strncpy (cw_texte,"Bonjour",3);    // manipulera BON
    strncpy (cw_texte,"abcd",4);    // sonnera LA SI DO RE
    strncp (cw_texte,"abcc#dd#");    // sonnera LA SI DO DO# RE RE#
    strncp (cw_texte,"$2cdefga4b4c4");    // sonnera DO RE MI FA SOL LA SI DO (3 dernières notes sur la gamme suivante, gamme 3 par defaut,$2=durée)

 On peut aussi charger 'cw_texte_att' qui ne sera manipulé qu'après 'tempo_attcwval' millisecondes, si il n'a pas changé ! 

**********************
NOTES sous forme 'a' 'a#' 'a3' 'a#3' ou 'a3#'
 $n avec n=0 à 9 permet de changer la durée des notes 
 
Duree des notes : 
Nom                  Indication      Pulsations par minute 
Grave                très lent       40 - 44 
Largo / Larghetto    large           44 - 50 
Lento / Adagio       lent, à l'aise  50 - 60 
Andante / Andantino  en marchant     60 - 80 
Moderato             modérément      80 - 100 
Allegretto / Allegro animé          100 - 126 
Vivace               vif            126 - 144 
Presto / Prestissimo (très) rapide  144 - 208   
(source Wikipédia)

La vitesse doit pouvoir se régler de 40 à 200 noires/minute (soit 1500 à 300 ms par noire)

En fait, une vitesse correcte dans notre cas sera de 267 noires/minutes (225ms/noire)
soit $2 pour la noire
double-croche=$0 croche=$1 noire=$2 blanche=$4 blanche_pointée=$6 ronde=$8

'0'=75ms (tempo 800 notes/minute) 
'1'=150ms
'2'=225ms (tempo 267 notes/minute) 
'3'=300ms
...
'9'=750ms (tempo 80 notes/minute)
  
**************************************
Donc, en résumé :  
 E I S H 5 K = . .. ... .... ..... -.- 
 HI = .... .. (hi trois fois)
 cdefg = DO RE MI FA SOL (bonjour)
 cge = DO SOL MI (c'est fini) 
 
*/
switch (grafcet_cw) {
// attente caractères
  case 1:
// texte immédiat : on MORSE tout de suite
    if (strlen(cw_texte) > 0) {
      cw_texte_att[0]='\0';
      strcpy(cw_texte_old,cw_texte_att);
// mise à jour valeurs temps CW
      CW_DOTLEN = 1200/CW_SPEED;    // en millisecondes
      CW_DASHLEN = CW_DASHLEN_PROP*CW_DOTLEN/10;   // en millisecondes
	  CW_CARSPACE = (CW_CARSPACE_PROP - 10)*CW_DOTLEN/10;
// inits
      cw_pos=0;
      cwcw = 0;                     // c'est de la musique a priori !
      grafcet_cw = 2;               // et on passe à l'étape suivante
      if (debug > 0) Serial && "CW=" && cw_texte && endli;
    }

	if ( tempo_cw == 0 ) {              // si la tempo est terminée,
// on arme la tempo de surveillance a la vitesse des points, ce qui est suffisant
		tempo_cw = CW_DOTLEN;
// texte temporisé : on MORSE si pas changé depuis tempo_attcw
		if (strlen(cw_texte_att) > 0) {
			if (strcmp(cw_texte_old,cw_texte_att) != 0) {
			strcpy(cw_texte_old,cw_texte_att);
			tempo_attcw = tempo_attcwval;    // durée de stabilité avant manipulation
		}
// le texte temporisé n'a pas changé, la tempo est-elle finie ?
      if ( tempo_attcw == 0 && cw_texte_att[0] != '\0') {
        strcpy(cw_texte,cw_texte_att);
        memset (cw_texte_att,'\0',15);  // RAZ du buffer
      }
    }
  }
  break;

// passage au caractère suivant
  case 2:
  if ( tempo_cw == 0 ) {
// fin de texte
    if (cw_pos >= strlen(cw_texte) or cw_texte[cw_pos] == '\0') {
      memset (cw_texte,'\0',30); // RAZ  du buffer
      grafcet_cw=1;
      lcd.setCursor(15, 1); lcd && '_';
      break;
    }
    cw = cw_texte[cw_pos];       
    cw_pos++;
// préparation affichage sur le LCD en bas à droite
    lcd.setCursor(15, 1); //lcd && cw ;
 
// caractère suivant = espace ou silence
    if (cw == ' ') {
		if (cwcw == 0) {
			tempo_cw = duree_note;
		} else {
			tempo_cw = CW_CARSPACE;
		}
		grafcet_cw=2;
		break;
    }

#ifdef tone_mode
// changement de duree de notes 
    if ( cwcw == 0 && cw == '$') {
		if (cw_texte[cw_pos] >= '0' && cw_texte[cw_pos] <= '9') {
			duree_note = 75*(1+cw_texte[cw_pos]-'0');
		}
		cw_pos++;
		cw = cw_texte[cw_pos];
		cw_pos++;
    }
// Si c'est de la musique et que c'est des minuscules, on fait de la musique !
    if (cwcw == 0 && ((cw >= 'a' && cw <= 'g') || cw == '$') ) {
        if (cw >= 'a' && cw <= 'g') lcd && cw; // affichage sur le LCD en bas à droite
// NOTES sous forme 'a' 'a#' 'a3' ou 'a3#'
		i=0; 
		while ((note_name[i]!=cw) && (i<7)){ i++; }
		note_freq=note_tone[i]/100.;
// Dièse ?
		if (cw_texte[cw_pos] == '#') {
			note_freq=note_freq*demiton;
			cw_pos++;
		}
// gamme ?
		if (cw_texte[cw_pos] >= '1' && cw_texte[cw_pos] <= '9') {
			if (cw_texte[cw_pos] != '3') note_freq=note_freq*pow(2.,cw_texte[cw_pos]-'3');
			cw_pos++;
		}
// Dièse ?
		if (cw_texte[cw_pos] == '#') {
			note_freq=note_freq*demiton;
			cw_pos++;
		}
//		if (debug > 2) Serial && duree_note && "ms " && note_freq && " Hz" && endli;
		if (debug == 2) Serial && note_name[i] && " " && (int) note_freq && "Hz" && endli;
		tone(sortie_gbf, note_freq);  tempo_cw = duree_note;
		grafcet_cw=21;  // fin de note
		break;

// Sinon, c'est du MORSE!
    } else {
#endif
        lcd && cw; // affichage sur le LCD en bas à droite
		cwcw = 1; // c'est de la CW, pas de la musique !
		if (cw >= 'a' && cw <= 'z') cw = cw & 0XDF; // forcage en MAJUSCULES
// recherche du pattern du caractère suivant (si on trouve pas, on prends le dernier)
		for (i=0; i<CW_N_MORSE; i++) {
			if (morsetab[i].c == cw) {
				pw = morsetab[i].pat;
				grafcet_cw++;
			}        
		}
#ifdef tone_mode
    }
#endif
  }    
  break;

// TONE du bit suivant
  case 3:
	if (tempo_cw == 0) {
// caractère non fini
		if (pw != 0B00000001) {
// TIT ou TAAH
			if (pw & 1) tempo_cw = CW_DASHLEN; else tempo_cw = CW_DOTLEN;
#ifdef tone_mode
			if (pw & 1) tone(sortie_gbf, CW_PITCH_TAAH); else tone(sortie_gbf, CW_PITCH);
#else
			digitalWrite(sortie_gbf, HIGH) ;
#endif
			pw = pw >> 1;	// décalage au bit suivant ( la /2 ne permet pas de dépasser 6 signes)
			grafcet_cw++;
// fin de caractère : silence inter-caractères (on a déjà eu le silence inter-signe=1, donc CARSPACE=3-1)
		} else {
			tempo_cw = CW_CARSPACE;  
			grafcet_cw=2; 
		}
	}
  break;

// silence 1 TAAH
  case 4:
    if (tempo_cw == 0) { 
#ifdef tone_mode
		noTone(sortie_gbf); 
#else
		digitalWrite(sortie_gbf, LOW);
#endif
		tempo_cw = CW_DOTLEN;  
		grafcet_cw=3; 
    }
  break;

// fin d'une note : silence très court
  case 21:
    if (tempo_cw == 0) { 
#ifdef tone_mode
		noTone(sortie_gbf); 
#else
		digitalWrite(sortie_gbf, LOW);
#endif
		tempo_cw = duree_note/15; 
		grafcet_cw=2; 
    }
  break;

// FIN, arrêt CW
// si grafcet_cw n'a pas une valeur correcte, on passe ici, on arrête
  default: 
  case 9999:
#ifdef tone_mode
	noTone(sortie_gbf); 
#else
    digitalWrite(sortie_gbf, LOW);
#endif
    grafcet_cw=0;  
  break;
}      // fin du "switch"
 
// FIN GRAFCET MORSE CW
//--------------------------------********************--------------------------------
// carillon type BigBen
#ifdef tone_mode
switch (grafcet_bigben) {

// lancement au quart d'heure rond : faire simplement 'grafcet_bigben=1;', on s'occupe du reste
  case 1 :
    grafcet_bigben++;
    if (minute()%15 != 0 ) grafcet_bigben = 0;      // c'est pas un quart !
    break;

// lancement à toute heure : faire simplement 'grafcet_bigben=2;'
  case 2 :
// inits selon selon quart d'heure (voir decl1300_morse.h)
     bigben_n = minute()/15;    // quel quarter ?
     bigben_i = bigben_debut[bigben_n];
     grafcet_bigben++;
    break;

// DING, DING, DONG, DANG : chimes
  case 3 :
    if (grafcet_cw == 1) {
//      if (debug >0) {Serial && horodatage('d') && "BIGBEN_chime=" && bigben_i && endli;}
      strcpy(cw_texte,bigben_chime[bigben_i]);
      if (bigben_i == bigben_fin[bigben_n]) grafcet_bigben++;    // c'est fini
      bigben_i++;
      if (bigben_i > 4) bigben_i=0;
    }
    break;

// init heure ou fin
  case 4 :
    if (grafcet_cw == 1) {
      if (bigben_n == 0) {
        bigben_i = hour();
        if ( bigben_i == 0 ) bigben_i = 12;
        if ( bigben_i > 12 ) bigben_i -= 12;
//        if (debug >0) {Serial && horodatage('d') && "BIGBEN heure=" && bigben_i && endli;}
        grafcet_bigben++;
      } else {
        grafcet_bigben=0;    // FIN
      }
    }
    break;
    
// DONG, DONG, DONG ... heure
  case 5 :
    if (grafcet_cw == 1) {
      if (bigben_i > 0) {
        strcpy(cw_texte,bigben);
        bigben_i--;
      } else {
        grafcet_bigben=0;    // FIN
      }
    }
  break;  
}
#endif
//--------------------------------********************--------------------------------

