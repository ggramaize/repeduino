//--------------------------------********************--------------------------------
// S/P locaux
//--------------------------------********************--------------------------------
void PLL_prog(int value) {
  digitalWrite(clockPin, LOW);  // Passe la Clock à "0"
  delay (tempo_PLL_reset);
  if (value == 0) {
    digitalWrite(dataPin, LOW); // présente un "0" aux data
  }
  else {
    digitalWrite(dataPin, HIGH); // présente un "1" aux data
  }
  delay (tempo_PLL_reset);
  digitalWrite(clockPin, HIGH);  // Valide un ######  0
  delay (tempo_PLL_reset);
  return;
}
//--------------------------------********************--------------------------------
int S_metre (int RSSI_input) {
  int  i = 1;
  while (RSSI_input > smetre[i] && i < sizeof(smetre) / 2 - 1) {
    i++;
  }
  return i;
}
//--------------------------------********************--------------------------------
// formate un nombre de 0 à 100 et 1000 dans 4 caractères (pour MP3)
char* format_4c (int i) {
  static char out[5];
  memset (out, '-', sizeof(out));
  out[4] = '\0';
  if (i >= 0 && i < 10) {
    out[0] = '0' + i;
  }
  else {
    if (i < 100) {
      out[0] = '0' + i / 10;
      out[1] = 48 + i % 10;
    }
    else {
      if (i == 100 || i == 1000) {
        out[0] = '1';
        out[1] = '0';
        out[2] = '0';
      }
      if (i == 1000) {
        out[3] = '0';
      }
    }
  }
  return out;
}
//--------------------------------********************--------------------------------
char* horodatage() {
  // affichage de l'heure '000000' ou 'NSYnn '
  static char out[7];
//  memset (out, '\0', sizeof(out));
  if (horloge_synchronisee < horloge_synchronisee_maxi) {
    out[0] = '0' + hour() / 10;
    out[1] = '0' + hour() % 10;
    out[2] = '0' + minute() / 10;
    out[3] = '0' + minute() % 10;
    out[4] = '0' + second() / 10;
    out[5] = '0' + second() % 10;
  }
  else {
    strcpy ( out, "NSY" );
    out[3] = '0' + horloge_synchronisee % 10;
    out[4] = '0' + horloge_synchronisee / 10;
    out[5] = ' ';
  }
  out[6] = '\0';
  return out;
}
/*
char* horodatage() {
  // affichage de l'heure '00:00 ' ou 'NSYnn '
  static char out[10];
  memset (out, '\0', sizeof(out));
  if (horloge_synchronisee < 35) {
    // HEURE:MINUTE:SECONDES
    strcpy ( out, doubledigit(  hour(), ':') );
    strcat ( out, doubledigit(minute(), ':') );
    strcat ( out, doubledigit(second(), ' ') );
  }
  else {
    strcpy ( out, "NSY" );
    strcat ( out, doubledigit(horloge_synchronisee, ' ') );
  }
  strcat ( out, "\0" );
  return out;
}
*/
//*******************************************************
// conversion d'une chaine de caractères en entier signé
int asc_to_int (char s[], int debut, int lon, int *ok_compteur) {
  // raz du ok_compteur si la conversion est bonne
  int i, j, k = 0;
  int ok = true;
  for (i = 0; i < lon; i++) {
    k = 10 * k;
    if (s[debut + i] >= '0' && s[debut + i] <= '9' ) {
      k += (s[debut + i] - '0');
    } else {
      if (!(i == 0 && s[debut] == '-')) ok = false;  // caractère inconnu
    }
  }
  // si signe moins en 1° caractère
  if (s[debut] == '-' ) k = -k;
  // si pas d'erreurs, on remet à 0 le compteur (données valides)
  if (ok) ok_compteur = 0;
  return k;
}

/*
int atoi(char s[])
{
int i, n=0;
for(i=0; s[i]>='0' && s[i]<='9'; i++)
  n = 10*n + (s[i] - '0');
return n;
}
*/
//*******************************************************
// incremente si inferieur au max
void incremente (int &i, int max) {
  if (i < max) i++;
  return;
}
//*******************************************************





















