//************************************************************************************************
// gestion reception Serial et gestion des envois au MP3
//*************************************************
/* TRAITEMENT DES MESSAGES RECUS
Sur la voie série, on récupère les messages avec la routine 'serial_reception'
  dans le buffer 'ger_msg' (33 octets par défaut)
  la fin de message est détectée par time-out 'ger_serial_timeout'

*/
if (ger_msg_recu > 0) {
  ger_msg[ger_msg_recu] = '\0';
  if (debug > 0) Serial && ger_msg_recu && "recus:" && ger_msg && " mp3_buffer_recu=" && mp3_buffer_recu && endli;

  switch (ger_msg[0]) {

    // Mise à l'heure
    // message = T08,29,55,11,12,13         Thh:mm:ss_dd/mm/yy
    //           123456789012345678
    case 'T' :
      //        if (ger_msg_recu > 17) {
      // conversion basique, mais efficace
      j = 1;
      for (i = 0; i < 6; i++) {
        hmsdmy[i] = 10 * (ger_msg[j] - '0') + (ger_msg[j + 1] - '0');
        j += 3;
      }
      // plus élégant, mais 170 octets de plus !!!
      //          hmsdmy[0] = atoi(strtok (ger_msg,"T,.-_;:/"));
      //          for(i=1; i < 6; i++) { hmsdmy[i] = atoi(strtok (NULL,"T,.-_;:/"));}

      // voir si on ne dépasse pas les bornes des limites, non mais !
      hmsdmy[0] = constrain(hmsdmy[0], 0, 23);
      hmsdmy[1] = constrain(hmsdmy[1], 0, 59);
      hmsdmy[2] = constrain(hmsdmy[2], 0, 59);
      hmsdmy[3] = constrain(hmsdmy[3], 1, 31);
      hmsdmy[4] = constrain(hmsdmy[4], 1, 12);
      //          hmsdmy[5] = constrain(hmsdmy[5], 0, 99);
      // mise à l'heure ARDUINO
      setTime(hmsdmy[0], hmsdmy[1], hmsdmy[2], hmsdmy[3], hmsdmy[4], hmsdmy[5]);
      // mise à l'heure de la RTC DS1307 éventuelle
#ifdef RTC1307
      nowx = now();
      RTC.adjust(nowx.unixtime() + gmt_decalage * 3600); // mise à l'heure RTC par l'heure PC locale
#endif
      // indicateur de synchro OK
      horloge_synchronisee = 0;
    //        }
    //    break; // pas de break pour avoir message de réponse

    // test de présence (watch dog ou ARDUINO MAH)
    // réponse (pour ARDUINO_mah.exe)
    // message = ?
    case '?':
      Serial && "aRDUINO " && versionx && endli;
      break;

    // Météo
    /*
    message = PppppXttttThhhhHllllL
              123456789012345678901
    pppp = pression en millibars ou hPa  '1013'  pour 1013 mB  ' 987' pour 987 mB
    tttt = température en dixièmes de degrés  ' 256' pour 25,6°C  '-103' pour -10,3°C
    hhhh = humidité drelative de '   0' à ' 100' en %
    llll = luminosité relative de '   0' à ' 100' en %
    X,T,H,L = un caractère parmi (H+=-B ) pour 'forte hausse', 'hausse', 'stable', 'baisse', 'forte baisse', si pas d'indication = espace
    exemples :
    P0990=0152=0100=0010=
    P1013+0186+0080-0080H
    */
    case 'P' :
      if (ger_msg_recu == 21) {
        pression = asc_to_int (ger_msg, 1, 4, &ok_pression);
        // non fait : gestion des tendances (B-=+H)
        temper = asc_to_int (ger_msg, 6, 4, &ok_temper);
        humid = asc_to_int (ger_msg, 11, 4, &ok_humid);
        lumin = asc_to_int (ger_msg, 16, 4, &ok_lumin);
      }
      break;
    // test
    case 'Z' :
      emettre_indicatif = true;
      break;
    // pour messages MUSIQUE ou MORSE ou AUTRES
    // message = M................*
    case 'M':
      // MP3
      if (debug > 0) {
        Serial && horodatage() && " M=" && ger_msg && endli;
      }
      // MP3 dispo ou STOP
      if ( mp3_buffer_recu == 0 || (ger_msg[1] == 'S' && ger_msg[2] == 'T')) {
        mp3_buffer_recu = ger_msg_recu - 1; // ((ger_msg_recu-1)/4)*4
        for (i = 0; i < mp3_buffer_recu; i++) mp3_buffer[i] = ger_msg[i + 1];
        if (debug > 0) {
          Serial && "M " && mp3_buffer_recu && "=" && mp3_buffer && endli;
        }
      }
      break;

    // message = X0/X1 AUTORISATION DE TRANSMISSION
    case 'X':
      if (ger_msg[1] == '0' || ger_msg[1] == '1') TX_autorisation = ger_msg[1] - '0';
      if ( mp3_buffer_recu == 0 ) {
        if (TX_autorisation == 1) {
          mp3_buffer_recu = 4;
          strncpy (mp3_buffer, "TXON", mp3_buffer_recu);
        }
        else {
          mp3_buffer_recu = 4;
          strncpy (mp3_buffer, "TXOF", mp3_buffer_recu);
        }
        Serial && "a" && mp3_buffer && endli;
      }
      if (debug > 0) {
        Serial && horodatage() && " ARDUINO Xmit=" &&  ger_msg[0] && endli;
      }
      break;

    // message = D0/D1/D2...  DEBUG
    case 'D':
      if ( mp3_buffer_recu == 0 ) {
        if (ger_msg[1] >= '0' && ger_msg[1] <= '9') {
          debug = ger_msg[1] - '0';
          mp3_buffer_recu = 8;
          strncpy (mp3_buffer, "DBUG0---", mp3_buffer_recu);
          mp3_buffer[4] = ger_msg[1];
        }
        Serial && horodatage()  && " ARDUINO Debug=" && debug && endli;
      }
      break;
    // forcage envoi indicatif
    case 'I':
      emettre_indicatif = true;
      break;
  }		// switch

  // libération buffer
  memset (ger_msg, '\0', sizeof(ger_msg)); // RAZ  du buffer
  ger_msg_recu = 0;
}    // ger_msg_recu > 0

// T1262347200  //noon Jan 1 2010  ( secondes depuis le 01/01/1970
//  1261440000 sec/40 ans (le décalage donne 10 jours, soit bien 10 années bissextiles)
//       86400 sec/jour
//    31536000 sec/an
// T1293969600  //noon Jan 1 2011
// T1325505600  //noon Jan 1 2012
// T1357041600  //noon Jan 1 2013
// T1388577600  //noon Jan 1 2014
// ************************************************************************************************
//**************************************************
// traitement des envois au MP3
// mp3_buffer_recu   1234 5678 \0
// buffer_avance 0123 4567 8

// un mp3 est en cours
if ( MP3player.isPlaying() ) {

  // gestion temporisation entre les mp3
  tempo_inter_mp3 = tempo_inter_mp3_reset;

  // recu commande STOP, pour arrêter le MP3 si en cours
  //     if (mp3_buffer_recu == 4 &&
  if ( mp3_buffer[0] == 'S' && mp3_buffer[1] == 'T' && mp3_buffer[2] == 'O' && mp3_buffer[3] == 'P') {
    Serial.println(F("Stop MP3"));
    MP3player.stopTrack();
    mp3_buffer_recu = 0;
    mp3_buffer_i = 0;
    mp3_buffer[0] = '\0';
    digitalWrite(BF, BF_OFF);
  }

} // FIN un mp3 est en cours
// pas de MP3 en cours de lecture
else {
  if ( tempo_inter_mp3 == 0) {
    // recu un ordre de lire du MP3
    if ( mp3_buffer_recu > 0 ) {
      //    if (debug > 1) Serial && F("lancement MP3=") && mp3_buffer_i && "/" && mp3_buffer_recu && "=" && mp3_buffer && endli;
      // commande matrice BF pour la BF MP3
      digitalWrite(BF, BF_ON);
      // on n'a pas fini le buffer, on passe au suivant
      if ( mp3_buffer_i < mp3_buffer_recu ) {
        // par paquet de 4 caractères : c'est un nom de fichier ! on complète avec .MP3 et on joue
        memset (mp3_filename, '\0', sizeof(mp3_filename)); // RAZ  du buffer
        strncpy (mp3_filename, &mp3_buffer[mp3_buffer_i], 4);
        mp3_buffer_i += 4;
        strncat (mp3_filename, ".MP3", 4);
        //       if (debug > 1) Serial && F("Jouons:") && mp3_filename && endli;
        mp3_result = MP3player.playMP3(mp3_filename);
        // affichage erreur
        if (mp3_result != 0) {
          Serial && F("MP3err:") && mp3_result && ":" && mp3_filename && ":" && endli;
        }
      } // FIN if ( mp3_buffer_i < mp3_buffer_recu )
      // on a fini le buffer
      else {
        mp3_buffer_recu = 0;
        mp3_buffer_i = 0;
        mp3_buffer[0] = '\0';
        memset (mp3_buffer, '\0', mp3_buffer_recu);
      } // on a fini le buffer
    }  // FIN mp3_buffer_recu > 0

    // on a fini, et plus rien à faire : coupure matrice BF
    else {
      digitalWrite(BF, BF_OFF);
    } // FIN coupure BF
  } // FIN tempo_inter_mp3 == 0
} // FIN pas de MP3 en cours de lecture
//**************************************************

