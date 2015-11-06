//************************************
// Reception Serial et/ou I2C
#ifndef ger_msg_size
#define ger_msg_size 33
#endif
// assez bizarrement, il faut initialiser la variable suivante, sinon ca marche pas !!! ( à vérifier)
char ger_msg[ger_msg_size] = {'0',0};
int ger_msg_recu;
// int affiche_msg_recu_lcd = 0; // indique à msg_traitement d'afficher les 'A' sur le lcd
// reception serial avec timeout
char ger_inByte;
#ifndef ger_serial_timeout
#define ger_serial_timeout 50    //ms
#endif
//unsigned long ger_serial_last_ms_char;    // pour timeout
//int jmsg=0;  // pointeur remplissage

// buffer de reception ligne série
int jmsg;
//unsigned long ger_serial_last_ms_char;    // pour timeout
uint32_t ger_serial_last_ms_char;    // pour timeout

// buffers pour mise à l'heure (cf msg_traitement.h)
int hmsdmy[6];

// ************************************************************************************************
// traitement de reception sur le port Serial
// la fin de message est obtenue par timeout (voir au-dessus : ger_serial_timeout)
// tous les caractères non imprimables (de contrôle) sont ignorés
// appeler ce sous-programme toutes les millisecondes pour une transmission à 9600 bauds
void serial_reception() {
//   static int jmsg=0;
//   static unsigned long ger_serial_last_ms_char;    // pour timeout

// réception voie série sur time-out

   if (Serial.available() > 0) {
     ger_inByte = Serial.read();
     if ((0x20 <= ger_inByte) && (ger_inByte <= 0x126)) { // strip off non-ASCII, such as CR or LF
       if (jmsg > ger_msg_size-2) jmsg=ger_msg_size-2;    // pour éviter les débordements !
//	   if (debug>1) Serial && "Srec1=" && jmsg && "=" && ger_inByte && " ms=" && millis() && "<" && ger_serial_timeout && endli;
       ger_msg[jmsg++] = ger_inByte;
       ger_msg[jmsg] = 0; // update end of line
     }
//	   if (debug>1)      Serial && F("Srec4:jmsg=") && jmsg && " =" && ger_msg && " ms=" && millis() && ">" && ger_serial_last_ms_char && "+" && ger_serial_timeout && endli;
     ger_serial_last_ms_char = millis(); // stroke the inter character timeout.
	 ger_msg_recu=0;

// time-out ?
   } else {
    if (((millis() - ger_serial_last_ms_char) > ger_serial_timeout) && jmsg > 0 ) { 
//      if (debug>0) Serial && F("Srec2:jmsg=") && jmsg && " =" && ger_msg && " ms=" && millis() && ">" && ger_serial_last_ms_char && endli;
//      if (debug>0) Serial && F("Srec3:diff=") && (millis() - ger_serial_last_ms_char) && endli;
      ger_msg_recu=jmsg;
      jmsg=0;
    }
  }
}
// ************************************************************************************************


  
  