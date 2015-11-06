//--------------------------------
// 05/11/2013
// Gestion du Morse : sortie sur sortie GBF
// et DECODEUR Morse
// et manip électronique
//
//  la génération même du Morse est faite dans un autre grafcet (grafcet_cw), 
//  car elle est simultanée et indépendante du reste !

  case 1300:	//               12       34         567     890        123456
    lcd.setCursor(0, 0); lcd && "V=" && CW_SPEED && " T=" && CW_PITCH;
    if (grafcet_cw == 0) {lcd && " OFF";} else {lcd && " ON ";}
    lcd.setCursor(0, 1); lcd && menus[1]; 
    tempo_a = CW_DOTLEN;
    grafcet_a++;
	
  case 1301:
    if (touche != ' ') {
//                                            123456789012345
      if (touche == '1') strcpy(cw_texte,"CQ CQ DE F1FWG\0");    // message test (maxi 15 car)
      if (touche == '2') strcpy(cw_texte_att,"QSL my QRK PSE\0");           // message test diffÃ©rÃ©
      if (touche == '3') CW_SPEED --;
      if (touche == '4') CW_SPEED ++;
      if (touche == '5') grafcet_cw = 9999;  // stop
      if (touche == '6') grafcet_cw = 1;     // start
      if (touche == '7') CW_PITCH /= demiton;
      if (touche == '8') CW_PITCH *= demiton;
      if (touche == '9') { CW_SPEED = 12; CW_PITCH = 704; grafcet_cw = 1; }    // reset
      grafcet_a--;
	  if (touche == '0') grafcet_a=9999;    // menu
// vitesse CW
      CW_DOTLEN=1200/CW_SPEED;
      CW_DASHLEN=CW_DASHLEN_PROP*CW_DOTLEN/10;
	  CW_CARSPACE=CW_CARSPACE_PROP*CW_DOTLEN/10;
	  CW_PITCH_TAAH=CW_PITCH/savart_mini;
      if (touche != '1' && touche != '2') strcpy(cw_texte,"TEST");
      touche = ' ';
    }
    if (tempo_a == 0) grafcet_a--;
  break;
  

