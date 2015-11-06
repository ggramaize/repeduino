//--------------------------------
// Deco décodeur MORSE
// afficheur :
// -.-.-.-c ppp/ttt
// CQCQCQDEF1FWGPSE
// on teste sur l'entrée U1+U2+U3

  case 1400:
// vitesse CW
//    CW_DOTLEN=1200/CW_SPEED;
//    CW_DASHLEN=CW_DASHLEN_PROP*CW_DOTLEN/10;
    lcd.clear();
/* 80ms= 1200:80 = 15 mots/minute
 morse_tih        |--<|     >               1=de 1 à 2,5
 morse_taah	      |---|---|-<-|   |   |   > 3=de 2,5 à 6
 morse_intersigne |--<|     >               1=de 1 à 2,5
 morse_intercar	  |---|---|-<-|   |       > 3=de 2,5 à 6
 morse_intermot	  |---|---|---|---|---|---<---|   |   |   |   |   |   |   >  7=de 6 à 14
 ms               0  80 160 240 320 400 480 560 640 720 800 880 960.040.120
 les < > indiquent les plages de réglage automatique	
	morse_taah = morse_tih*3;
    morse_intersigne = morse_tih;
    morse_intercar = morse_tih*3;
    morse_intermot = morse_tih*7;

22/12/2014 : entrées alimentées en 5Volts, dont U1 calibre 20V donc analogRead > 200, les U2 et U3 = digitalRead 
*/
    morse_espace=0; morse_bit=0; morse_car=0; morse_mask=1;
// mise à jour valeurs temps CW
    CW_SPEED = 13;
	CW_DOTLEN = 1200/CW_SPEED;    // en millisecondes
    CW_DASHLEN = CW_DASHLEN_PROP*CW_DOTLEN/10;   // en millisecondes

	memset ( morse_texte,'_',sizeof(morse_texte)); morse_texte[sizeof(morse_texte)-1] = '\0';
#if debugc > 2
Serial && horodatage_d() && modex[mode] && " base=" && CW_DOTLEN && "ms" && endli; 
#endif
    morse_manip='?';			// Manu Semi Auto

	// Pull Up des entrées
     digitalWrite(entree_U1,HIGH);
     digitalWrite(entree_U2,HIGH);
     digitalWrite(entree_U3,HIGH);
	lcd && "?";
    grafcet_a++;
  break;

// choix du mode au premier contact
// 0=manip pioche U1, 1=semi-auto 2 contacts U2/U3, 2=auto 2 contacts U2/U3
//      if (debug > 0) Serial && "morse_etat="  && morse_etat  && " morse_manip="  && morse_manip  && endli; 
// si le 1er contact U1=pioche_manuelle  U2=semi-auto  U3=auto
  case 1401:
    if ( tempo_cw2 == 0) {
     tempo_cw2 = 100;
	 morse_etat = 0;
	if (analogRead(entree_U1) < seuil_keyer) morse_manip = 'M'; 
	if (!digitalRead(entree_U2)) morse_manip = 'S'; 
	if (!digitalRead(entree_U3)) morse_manip = 'A'; 
     if (morse_manip != '?' ) grafcet_a++;
	}
  break;
 
// affichage valeurs réglées
  case 1402:
// en semi_auto, on ajuste le DOT
        if (morse_manip == 'S') CW_DOTLEN = CW_DASHLEN*10 / CW_DASHLEN_PROP;
// butées vitesse
		if ( CW_DOTLEN < 40) CW_DOTLEN=40;
		if ( CW_DOTLEN > 200) CW_DOTLEN=200;
		if ( CW_DASHLEN < CW_DOTLEN*5/2) CW_DASHLEN = CW_DOTLEN*3;
		if ( CW_DASHLEN > CW_DOTLEN*5) CW_DASHLEN = CW_DOTLEN*4;
		CW_CARSPACE = CW_CARSPACE_PROP*CW_DOTLEN/10;
// affichage
     lcd.setCursor(0, 0); lcd && _FLOAT(1200./CW_DOTLEN,1) && " " && _FLOAT((float) CW_DASHLEN/CW_DOTLEN,1) && " " && CW_DOTLEN && morse_manip && CW_DASHLEN && "  ";
	 morse_espace=3;
     tempo_cw2 = tempo_cw2_reset;             // on rearme la tempo
     grafcet_a++;
  break;

// attente signal montant
  case 1403:
    if (tempo_cw2 == 0 ) {
      grafcet_a = 1409;  // traitement clavier
      morse_etat = 0;	// a priori, on considère la clef relachée
	  switch (morse_manip) {
// pioche
	  case 'M':
        if (analogRead(entree_U1) < seuil_keyer) morse_etat = 1;
//	    if (digitalRead(entree_U1)) morse_etat = 1;
		break;
// semi-auto ou auto
	  case 'S':
	  case 'A':
	    if (!digitalRead(entree_U2)) morse_etat = 2;
	    if (!digitalRead(entree_U3)) morse_etat = 3;
		break;
	  }
//      if (debug > 0) Serial && "U123="  && U1  && " "  && U2 && " "  && U3 && endli; 
//      if (debug > 0 && morse_manip>0) 
//Serial && "morse_etat="  && morse_etat  && " morse_manip="  && morse_manip  && endli; 

// selon type de manip on rearme la tempo
		tempo_cw2 = tempo_cw2_reset;	// manu             
		if (morse_etat == 2) tempo_cw2 = CW_DOTLEN;	// auto ou semi-auto
		if (morse_etat == 3) tempo_cw2 = CW_DASHLEN;	// auto
		if (morse_etat == 3 && morse_manip == 'S') tempo_cw2 = CW_DASHLEN*9/10;		// semi-auto : on réduit un peu pour pouvoir accélérer
//Serial && "(1402) " && grafcet_a && " tempo_cw2="  && tempo_cw2   && " etat=" && morse_etat && endli; 

// le signal monte
      if (morse_etat > 0) {
        morse_espace=0;
        morse_tip=millis();
#ifdef tone_mode
        if (morse_etat == 3 ) {tone(sortie_gbf, CW_PITCH_TAAH);} else {tone(sortie_gbf, CW_PITCH);}
#else
        digitalWrite(sortie_gbf, HIGH) ;
#endif
// reset ligne décodage signes
        if (morse_bit == 0)  {lcd.setCursor(0, 0); lcd && "                "; morse_car=0; morse_mask=1;}

// raz top pour éviter cyclage ...
        morse_top=morse_tip;
        grafcet_a=1404;		// attente descente signal

// le signal est toujours bas
      } else {
// calcul de la duree de l'attente
		morse_duree = millis()-morse_top;

// si l'attente est plus grande que , c'est une fin de caractère
		if ( morse_espace<1 && morse_duree > (CW_DOTLEN*5/2) ) {
			morse_espace = 1;
// on ajoute le '1' final à gauche pour obtenir le pattern à chercher
			morse_car = morse_car | morse_mask;
// recherche caractère recu (le dernier de la table est '?' si on ne trouve pas)
            if (debug > 0) Serial && "Mcar=" && _BIN(morse_car) && endli; 
			i=0;
			do { i++; } while (i<CW_N_MORSE-1 && morse_car != morsetab[i].pat);
// affichage caractère et vitesse
			lcd.setCursor(7, 0); lcd && morsetab[i].c && " " && CW_DOTLEN && morse_manip && CW_DASHLEN;
// suppression d'un blanc dans les premiers caractères
//			j=0; while (j<9 && morse_texte[j] != ' ') {j++;} // recherche 1er blanc
// décalage texte à gauche
//            if (j==9) j=0;	// si pas de blanc trouvé
//            for (k=j;k<15;k++) { morse_texte[k]=morse_texte[k+1]; }
            for (k=0;k<15;k++) { morse_texte[k]=morse_texte[k+1]; }
/*
0123456789012345
Q CQ CQ DE F1FWG	
*/
// affichage caractère
			morse_texte[15]=morsetab[i].c; lcd.setCursor(0, 1); lcd && morse_texte;
			morse_bit=0; // init pour car suivant
			if (morse_manip == 'S' || morse_manip == 'A') tempo_cw2 = CW_CARSPACE;
		}
       
// si l'attente est plus grande que 7 base, c'est un espace entre mots
		if (morse_espace<2 && morse_duree >= CW_CARSPACE ){
			morse_espace=2;
			for (j=1;j<16;j++) { morse_texte[j-1]=morse_texte[j]; }  // décalage texte à gauche
			morse_texte[15]=' '; lcd.setCursor(0, 1); lcd && morse_texte;
// turn on the cursor:
//			lcd.setCursor(15,1); lcd.cursor();
		    Serial && morse_texte && endli;
			if (morse_manip == 'S' || morse_manip == 'A') tempo_cw2 = CW_DOTLEN;
		}
// affichage paramètres si attente très longue
		if (morse_espace<3 && morse_duree >= 15*CW_CARSPACE) {
			grafcet_a=1402;
			morse_espace=3;
		}
	  }	// U1 > 
//Serial && "(1402) " && grafcet_a && " tempo_cw2="  && tempo_cw2   && " etat=" && morse_etat && endli; 
    } // tempo == 0
  break;

// attente fin signal et fin tempo en auto
  case 1404:
    if (tempo_cw2 == 0) {

//	if (!digitalRead(entree_U1) && !digitalRead(entree_U2) && !digitalRead(entree_U3)) morse_etat = 0;
	  switch (morse_manip) {
// pioche
	  case 'M':
	    if (analogRead(entree_U1) > seuil_keyer) morse_etat = 0;
//	    if (!digitalRead(entree_U1)) morse_etat = 0;
		break;
// semi-auto : point . à durée forcée, trait _ à durée contrôlée par le manipulateur
	  case 'S':
	    if (morse_etat == 2) morse_etat = 0;
	    if (morse_etat == 3 && digitalRead(entree_U3)) morse_etat = 0;
		break;
// auto : temps forcé pour . et _ , et répétition automatique si maintenu
	  case 'A':
	    morse_etat = 0;
		break;
	  }

// signal n'est plus présent
      if (morse_etat == 0) {
        morse_top=millis();
#ifdef tone_mode
        noTone(sortie_gbf);
#else
        digitalWrite(sortie_gbf, LOW) ;
#endif
        morse_duree = morse_top-morse_tip;
        lcd.setCursor(morse_bit, 0);
// TIH
		if (morse_duree < ((CW_DOTLEN+CW_DASHLEN)/2) ){
			if ( morse_duree >= (CW_DOTLEN/2) ) {  
				lcd && ".";
				CW_DOTLEN = (CW_AMORTISSEUR * CW_DOTLEN + morse_duree)/(CW_AMORTISSEUR + 1); // variation amortie !
			} else {
				lcd && "'";	// vraiment trop court
			}
//          Serial && morse_duree && "." && CW_DOTLEN && endli; 
// TAAH  
        } else {
            morse_car = morse_car | morse_mask; 
            if (morse_duree < CW_DASHLEN*2) {
              lcd && "-";
// en semi_auto, on a aussi ajustement
              CW_DASHLEN = (CW_AMORTISSEUR*CW_DASHLEN+morse_duree) /(CW_AMORTISSEUR+1);// variation amortie !
//              if (morse_manip==2) CW_DOTLEN = CW_DASHLEN*10 / CW_DASHLEN_PROP;
            } else {
              lcd && "=";	// vraiment trop long
            }
//          Serial && morse_duree && "-" && CW_DOTLEN && endli; 
        }
// on pointe sur le bit suivant, on decale le mask de 1 à gauche
		morse_mask = morse_mask << 1;
		morse_bit++;    
		if (morse_bit > 6) morse_bit=0;
		grafcet_a=1409;
	  }
// selon type de manip
		tempo_cw2 = tempo_cw2_reset;             // on rearme la tempo
		if (morse_manip == 'S' || morse_manip == 'A') tempo_cw2 = CW_DOTLEN;
//Serial && "(1403) " && grafcet_a && " tempo_cw2="  && tempo_cw2  && " etat=" && morse_etat && endli; 
    }
  break;   

// traitement clavier si pas d'évolution notable du signal MORSE
  case 1409:
    if (tempo_cw2 == 0 ) {
		tempo_cw2 = tempo_cw2_reset;             // on rearme la tempo
		grafcet_a = 1403;

		if (touche == '0') {
#ifdef tone_mode
			noTone(sortie_gbf);
#else
			digitalWrite(sortie_gbf, LOW) ;
            digitalWrite(entree_U1,LOW);
		    digitalWrite(entree_U2,LOW);
		    digitalWrite(entree_U3,LOW);
#endif
			grafcet_a=9999;    // pour raz grafcet
		}
// reset mode
		if (touche == '9') {
			grafcet_a=1400;
		}
// autres
		if (touche != ' ') {
			touche = ' ';
		}
//Serial && "(1409) " && grafcet_a && " tempo_cw2="  && tempo_cw2  && endli; 
	}
  break;   
//--------------------------------
