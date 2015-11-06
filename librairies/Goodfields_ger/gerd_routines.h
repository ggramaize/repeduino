//**********************************************************
// gestion de l'affichage LCD 'universel'
//**********************************************************
/*
 Ces routines permettent d'écrire des programmes portables sur des plateformes avec différents type d'afficheurs :
 UTILISATION : inclure les lignes qui suivanet dans le p/p
//******************** gerd_routines ***********************
// lcd definitions
// constantes type d'affichage LCD definies dans gerd_routines_decl.h
#include <gerd_routines_decl.h>
// choix du lcd
#define lcd_type lcd_none

// declarations dépendantes du type
#define lcd_type lcd_direct
#define lcd_col 16 // 8, 16, 18, 20
#define lcd_lig 2 // 1, 2, 4
//#define creercaracterestype 4 // 1 à 4 si non défini = pas de reprogrammation
//#define creercaracteresdemo 1 // si on veut un test au démarrage

#define lcd_type lcd_ELCDXXX
#define lcd_col 20
#define lcd_lig 4

#if lcd_type == lcd_direct
#include <LiquidCrystal.h>  // LCD
// LCD initialize the library with the numbers of the interface pins
LiquidCrystal lcd (4, 2, 6, 7, 8, 9);
// sous-programmes spécifiques pour le bargraphe
// voir dans le s/p bargraph
//#define creercaracterestype 4 // 1 à 4 si non défini = pas de reprogrammation
//#define creercaracteresdemo 1 // si on veut un test au démarrage
#endif

#ifdef creercaracterestype
#include <ger_lcd_bargraph.h>
#endif

// routines universelles
#include <gerd_routines.h>

//******************** gerd_routines * FIN *****************
routines disponibles :
 gerd_lcd (char* texte)
 gerd_lcd_colig (col, lig)
 gerd_lcd_clear()
 gerd_lcd_init (lcd_col, lcd_lig)

 */
//**********************************************************
// écriture du texte 'texte'
void gerd_lcd (char* texte) {

#if lcd_type == lcd_ELCDXXX
//  Serial && char(0xA2) && texte && char(0);  // à tester
    Serial.write(0xA2);
    Serial && texte;
    Serial.write(0);
#endif

#if lcd_type == lcd_none
    Serial && "lcd=" && texte && endli;
#endif

#if lcd_type == lcd_direct
//  lcd && texte; // à tester
    lcd.print(texte);
#endif

#if lcd_type == lcd_mult
//  lcd && texte; // à tester
    lcd.print(texte);
#endif
}
//**********************************************************
// positionnement du curseur
void gerd_lcd_colig (int col, int lig) {

#if lcd_type == lcd_ELCDXXX
//  Serial && char(0xA1) && char(col) && char(lig) && char(0);
    Serial.write(0xA1);
    Serial.write(col);
    Serial.write(lig);
#endif

#if lcd_type == lcd_none
    Serial && "lcd colig=" && col && "," && lig && endli;
#endif

#if lcd_type == lcd_direct
    lcd.setCursor (col, lig);
#endif

#if lcd_type == lcd_mult	// multiples lcd de 2 lignes de 16 caractères
#if nb_afficheurs > 8
    digitalWrite(ger_enable[3], ligne & 0B00010000 );
#endif
#if nb_afficheurs > 4
    digitalWrite(ger_enable[2], ligne & 0B00001000 );
#endif
#if nb_afficheurs > 2
    digitalWrite(ger_enable[1], ligne & 0B00000100 );
#endif
//    digitalWrite(ger_enable[0], ((ligne & 0B00000010)?HIGH:LOW) );    // 4 octets de plus
    digitalWrite(ger_enable[0], ligne & 0B00000010 );
//delay(1);  // inutile en fait
// positionnement
//    lcd.setCursor(colonne, ligne%2); // 8 octets de plus !
    lcd.setCursor(colonne, ligne & 0B00000001);
#endif
}
//**********************************************************
// effacement du lcd
void  gerd_lcd_clear() {

#if lcd_type == lcd_ELCDXXX
//  Serial && char(0xA3) && char(1)
    Serial.write(0xA3);
    Serial.write(0x01);
#endif

#if lcd_type == lcd_none
    Serial && "lcd_clear" && endli;
#endif

#if lcd_type == lcd_direct
    lcd.clear();
#endif

#if lcd_type == lcd_mult	// multiples lcd de 2 lignes de 16 caractères
    int i0=4;
#if nb_afficheurs > 8
    i0=32;
#endif
#if nb_afficheurs > 4
    i0=16;
#endif
#if nb_afficheurs > 2
    i0=8;
#endif
    for (int i=0;i<i0;i+=2) {
        gerd_lcd_colig (0,i);
        lcd.clear();
    }
#endif
}
//**********************************************************
// Initialisation
void gerd_lcd_init (int col, int lig) {

#if lcd_type >= lcd_none
// ligne série (pour traces)
    Serial.begin(9600);
    Serial && versionx && col && "c " && lig && "l" && endli;
#endif

#if lcd_type == lcd_ELCDXXX
    /*
    Mis à part 'texte', tout est en binaire donc utiliser chr(x)
    A0 = init
    A1 X Y  = positionnement en col, lig
    A2 texte 0 = affichage du texte
    A3 0C = eteindre le curseur
    A3 0E = allumer le curseur
    A3 01 = home
    A5 c c1 ... C8 0 = avec c=8 à 15, C1 C8 =octets de reprogrammation
    */
    Serial.begin(19200);
//  delay(100);
    Serial.write(0xA0);	// reset
    delay(300);
    Serial && char(0xA3) && char(0x0C); 	// eteindre curseur
#ifdef creercaracteres
//  ger_creercaracteres();
#endif
#endif

#if lcd_type == lcd_direct
// init lcd standard local
    lcd.begin(col, lig);  // initialise le LCD
    lcd.clear();
    lcd && versionx;
// attente pour bien voir
    delay(1000);
#endif

#if lcd_type == lcd_I2C
    Serial && lcd_type && " type non supporte : " && versionx && endli;
    return;
#endif

#if lcd_type == lcd_mult
    Serial && lcd_type && " type non supporte : " && versionx && endli;
    return;
#endif

// programmation des caractères LCD
#ifdef creercaracterestype
    ger_creercaracteres();
#endif

// affichage versionx
    gerd_lcd_clear ();
    delay(100);
    gerd_lcd (versionx);
    delay(2000);
}
//**********************************************************
//********************************************************************************/
// OBSOLETE : AFFICHEURS MULTIPLES
/********************************************************************************/
// OBSOLETE utiliser : void gerd_lcd (char* texte) {
// positionne le curseur
// colonne = 0 à lcd_col
// ligne = 0 à lcd_lig

// ligne 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 .. 31
// en[0] 0 0 1 1 0 0 1 1 0 0  1  1  0  0  1  1  0  0  1  1  0  0  1  1  0  0     1
// en[1] 0 0 0 0 1 1 1 1 0 0  0  0  1  1  1  1  0  0  0  0  1  1  1  1  0  0
// en[2] 0 0 0 0 0 0 0 0 1 1  1  1  1  1  1  1  0  0  0  0  0  0  0  0  1  1
// en[3] 0 0 0 0 0 0 0 0 0 0  0  0  0  0  0  0  1  1  1  1  1  1  1  1  1  1
// LIG%2 0 1 0 1 0 1 0 1 0 1  0  1  0  1  0  1
/*
  void ger_lcd_setcursor (int colonne, int ligne) {
//    Serial && ligne && ((ligne & 0B00000100)?HIGH:LOW) && ((ligne & 0B00000010)?HIGH:LOW) && endli;

#if lcd_type == lcd_mult	// multiples lcd de 2 lignes de 16 caractères
#if nb_afficheurs > 8
    digitalWrite(ger_enable[3], ligne & 0B00010000 );
#endif
#if nb_afficheurs > 4
    digitalWrite(ger_enable[2], ligne & 0B00001000 );
#endif
#if nb_afficheurs > 2
    digitalWrite(ger_enable[1], ligne & 0B00000100 );
#endif
//    digitalWrite(ger_enable[0], ((ligne & 0B00000010)?HIGH:LOW) );    // 4 octets de plus
    digitalWrite(ger_enable[0], ligne & 0B00000010 );
//delay(1);  // inutile en fait
#endif
// positionnement
//    lcd.setCursor(colonne, ligne%2); // 8 octets de plus !
    lcd.setCursor(colonne, ligne & 0B00000001);
    return;
  }
 */
/********************************************************************************/


