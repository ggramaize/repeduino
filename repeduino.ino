#define versionx "V14d 11.2015"
//**********************************************************
// F5KRY-F4ASS-F1FWG
// ??? encore PSHIIIIT au 1er essai ?
// ??? voir anti - bavard qui se fait couper lors du traitement message (STOP)
/*
 Logique de Relais F5KRY . F1FWG . F4ASS

 ARDUINO UNO , interface ArduinoISP
 shield Sparkfun MP3

 compilé avec ARDUINO V1.6.4
 mp3 sur shield Sparkfun mp3 Player
 mise à l'heure par DCF-77 ou voie série (voir _19_DCF77_mini_afficheur)

 Voir le répertoire en annexe contenant les librairies nécessaires

 A AJUSTER :
 - contenu des messages MP3 dans la carte micro-SD
 - éventuellement variable de compilation ou commutateur de sélection sur une entrée ANA
 pour choisir le relais concerné (Hf, Vhf, Uhf)

 Compilation :
 IDE V1.0.6 = 26228 octets
 IDE V1.6.2 = 24594 ocetets / 1653 RAM
 IDE V1.6.2 + DCF-77 = 26854 octets / 1738 RAM
 IDE V1.6.4 = 25240 / 1641 RAM
 */

#define quel_relais HF51
//#define quel_relais VHF
//#define quel_relais UHF

// messages en CW et musiquettes
//#define INDICATIF_LONG "F1ZEV DFN JN25EJ 42 FRANCE"
//                             123456789012345678901234567890
#define INDICATIF_LONG        "bonkex51"
#define INDICATIF_COURT       "bonj"
#define INDICATIF_BYEBYE      "fi73"
#define INDICATIF_QRM         "qrmx"
#define INDICATIF_ANTIBAVARD  "abav"
//                             123456789012345678901234567890
#define MUSIQUE_DEMARRAGE     "xdon"
#define MUSIQUE_ARRET         "xxVAxfin"
//#define MUSIQUE_DEBUG         "dbug"
//#define MUSIQUE_AUTORISATION  "txon"      // TRANSMIT X1/X0
//                             123456789012345678901234567890

/*
A SUIVRE :
 Pour le décodage DTMF à base de 8870, on peut utiliser une entrée ANA avec un réseau R+2R
 Je pense que l'on peut même mettre le bit de validation en 5° bit (poids 16)
 500(16) + 1000(8) + 2000(4) + 4000(2) + 8000(1) Ohms .
 Vérifier la sortance du 8870, doubler les valeurs si besoin
 On peut alors utiliser le sous-programme ger_clavier pour récupérer les codes (cf F1FWG)

 ATTENTION : Serial à 57600 bds !!! (vitesse maxi des Arduino Mini Pro 3,3V)
 Mise à l'heure : envoyer sur la ligne série : (voir mp3_traitement.h)
 Thh:mm:ss_dd/mm/anyss

 MP3 :
 Ce programme attends :
 - dans le 'mp3_buffer'
 - une commande de forme XXXXYYYYZZZZ...
 - exemple :  mp3_buffer_recu = 12; strncpy(mp3_buffer,"bonj25--degc",mp3_buffer_recu);
 - Chaque paquet de 4 lettres est le nom du fichier .mp3 à jouer, placé sur la carte sd
 - Le buffer est actuellement taillé pour 15 fichiers ('mp3_buffer_long'=60 caractères).
 - Lorsque le fichier est lancé, pour arrêter le MP3 en cours :
 - faire par programme : strcpy(mp3_buffer,"STOP"); mp3_buffer_recu = 4;
 Sur la ligne série, voir mp3_traitement.h, en particulier :
 - 'MXXXXYYYYZZZZ' joue ces 3 fichiers MP3 comme décrit ci-dessus
 - 'STOP' arrête le mp3 en cours

 Exemples à envoyer :
 ====----====--16====----====--32====----====--48====----====--64====----====--80====----====--96
 bonkex51pres9---100-90--8---mbarhaustemp17--degcbaif
 ppar25--wattstabiles11--heur18--minufintfi73
 repo53--xxxK
 mz03
 mz07
 dq01
 1---2---3---4---5---6---7---8---9---10--11--12--13--14--15--
 16--17--18--19--20--21--22--23--24--25--26--27--28--29--30--
 31--32--33--34--35--36--37--38--39--40--41--42--43--44--45--
 46--47--48--49--50--51--52--53--54--55--56--57--58--59--60--
 61--62--63--64--65--66--67--68--69--70--71--72--73--74--75--
 76--77--78--79--80--81--82--83--84--85--86--87--88--89--90--
 91--92--93--94--95--96--97--98--99--
 100-100-1---100-2---100-3---100-4---100-5---100-6---100-7---

 Nota : le buffer de l'interface de développement ne fait que 63 octets

 Entrées sorties utilisées :
 0 TX Serial
 1 RX Serial
 2 MP3-DREQ                         // Data Request Pin: Player asks for more data
 3 DATA PLL : programmation vers TX             // libre si SJ2 ouvert (MIDI-In)
 4 BF : commut BF MP3 vers TX (inverse de RX vers TX)     //libre si SJ1 ouvert (GPI01)
 5 TX : vers TX
 6 MP3-CS : MP3 chip select         // Control Chip Select Pin (for accessing SPI Control/Status registers)
 7 MP3-DCS : MP3 data chip select   // Data Chip Select / BSYNC Pin

 8 MP3-RST : MP3 Reset              // Reset is active low
 9 SD-CS : SD card chip select
 10 libre !
 11 MOSI bus SPI (comm carte SD et MP3)
 12 MISO bus SPI (comm carte SD et MP3)
 13 SCK bus SPI (et Led ARDUINO de base)

 A0 RSSI : depuis RX
 A1 CLOCK PLL : programmation vers TX
 A2 LATCH PLL : programmation vers TX
 A3 DTMF : futur
 A4 DCF77 : horloge (ou bus I2C)
 A5 bus I2C

 ATTENTION ! le point commun de la sortie BF est 'GBUF', qui n'est PAS le GND !!!
 Il y a lieu de consulter le document 'vs10XXan_output.pdf' page 6(10) pour faire un étage de sortie compatible .

 Description: This new revision of the MP3 player shield retains the awesome MP3 decoding abilities of
 the last version but adds the storage functionality of the SD card shield. Now you can pull MP3 files
 from an microSD card and play them using only one shield, effectively turning any Arduino into
 a fully functional stand-alone  MP3 player! The MP3 Shield still utilizes the VS1053B MP3 audio decoder IC to decode audio files.
 The VS1053 is also capable of decoding Ogg Vorbis/MP3/AAC/WMA/MIDI audio and encoding IMA ADPCM
 and user-loadable Ogg Vorbis.se

 The VS1053 receives its input bitstream through a serial input bus (SPI).
 After the stream has been decoded by the IC, the audio is sent out to both a 3.5mm stereo headphone jack,
 as well as a 2-pin 0.1" pitch header.

 As the Serial Monitor is typically default with no CR or LF, this sketch uses intercharacter
 time out as to determine when a full string has been entered to be processed.
 */

//**********************************************************
// BIBLIOTHEQUES
/*
Bibliothèques ajoutées au kit de base V1.0.6 :

 Goodfields_ger
 Streaming_ger
 SFEMP3Shield
 SdFat
 */

// incluses dans le package Arduino de base
// *** LCD *** afficheur LCD
//#include <LiquidCrystal.h>  // LCD

// ajoutées
// *** GENERAL *** formatage des entrees sorties ASCII
#include <Streaming_ger.h>

// *** TEMPS ***
#include <Time_ger.h>
#include <ger_time.h>

// pour la communication avec le décodeur MP3
#include <SPI.h>

// Pour la carte SD : Add the SdFat Libraries
#include <SdFat.h>
#include <SdFatUtil.h>

// and the MP3 Shield Library
#include <SFEMP3Shield.h>

/*
// Below is not needed if interrupt driven. Safe to remove if not using.
 #if defined(USE_MP3_REFILL_MEANS) && USE_MP3_REFILL_MEANS == USE_MP3_Timer1
 #include <TimerOne.h>
 #elif defined(USE_MP3_REFILL_MEANS) && USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer
 #include <SimpleTimer.h>
 #endif
 */

// *** DCF 77 ***
// ne pas définir si pas d'horloge DCF-77
// #define DCF77_synchro OUI
// ET !!! mettre le paquet suivant en commentaire (le # ifdef ne marche pas bien)
/*
#ifdef DCF77_synchro
#define VERBOSE_DEBUG 1	     // Verbose = pour debug DCF, décommenter dans utils.cpp si besoin
#include "DCF77_ger.h"
#define DCF_PIN A4	         // Connection pin to DCF 77 device
#define DCF_INTERRUPT 0		 // Interrupt number associated with pin
// constructor et inits
DCF77 DCF = DCF77(DCF_PIN, DCF_INTERRUPT);

int dcf_memoire = -1;
// int attente_misalheure=999;// nombre de minutes sans synchro

// pour stocker l'heure
time_t DCFtime, DCFUTCtime;
// *** DCF 77 *** fin
#endif
*/
//**********************************************************
// DECLARATIONS
// *** GENERAL ***
/*  :
 0= pas de debug
 1= minimal pour voir les étapes de grafcet
 2= détaillé (valeurs de fréquences BF)
 3= de test (affichage permanent RSSI>mini/2)
 */
int debug = 0;

//----------------------------------------------
/**
 * \brief Object instancing the SdFat library.
 *
 * principal object for handling all SdCard functions.
 */
SdFat sd;

/**
 * \brief Object instancing the SFEMP3Shield library.
 *
 * principal object for handling all the attributes, members and functions for the library.
 */
SFEMP3Shield MP3player;
//int16_t mp3_last_ms_char; // milliseconds of last recieved character from Serial port.
//int8_t mp3_buffer_pos; // next position to recieve character from Serial port.

//----------------------------------------------
/**
 * \brief Setup the Arduino Chip's feature for our use.
 *
 * After Arduino's kernel has booted initialize basic features for this
 * application, such as Serial port and MP3player objects with .begin.
 * Along with displaying the Help Menu.
 *
 * \note returned Error codes are typically passed up from MP3player.
 * Whicn in turns creates and initializes the SdCard objects.
 *
 * \see
 * \ref Error_Codes
 */

//----------------------------------------------
// gestion des commandes vers le MP3
#define mp3_buffer_long 62 // 4 x N + 1 (NB: le buffer de l'émulateur série de L'IDE est de 64)
char mp3_buffer[mp3_buffer_long] = {0}; // 0000111122223333...+null
int mp3_buffer_i = 0; // pointeur dans le buffer d'exécution
int mp3_buffer_recu = 0; // nombre de caractères dans mp3_buffer, reception terminée
char mp3_filename[9];  // XXXX.XXX+null
uint8_t mp3_result;    // code erreur de retour 0=Ok

// délai entre chainage de mp3 (petit blanc)
#define tempo_inter_mp3_reset 100 // ms 200 va bien.
int tempo_inter_mp3;

//----------------------------------------------
// *** CLAVIER et LCD ***
//#define ger_clavier A0	// entrée ana du clavier si on veut qu'il soit géré
//#define ger_bargraph OUI // a définir si on veut utiliser ces fonctions
//#include "ger_time.h"

//-------------------
// mode balise : pour essais avec un seul émetteur (FT480 modifié/ F1FWG)
// si mode_balise est defini, on a un fonctionnement pour test avec 1 seul émetteur (renvoi du report)
//#define mode_balise OUI

// ces 'define' sont des 'MACROS' qui définissent une partie de code-source en mode 'BALISE'
// emission pour mode balise
#define BALISE_TX_ON { if (TX_autorisation==1) {digitalWrite(TX, TX_ON);} tempo_general = 250; delay(250); }

// Coupure TX pour mode balise
// pendant 200ms, on a des retours HF sur le FT480 ...
#define BALISE_TX_OFF { digitalWrite(TX, TX_OFF); tempo_general = 250; delay(250); }

//-------------------
// *** GENERAL ***

/******************/
// ENTREES SORTIES
// toutes les entrées sorties de l'ARDUINO UNO sont en 5 volts.
// une bonne protection consiste à mettre une résistance de 1kOhms en série avec les E/S pour éviter toute destruction

/* F1FWG : noter que le choix de 13 pour le TX, de 1 pour le PLL et 0 pour la BF n'est pas très judicieux
 car ces 3 pinoches clignotent vaillamment pendant la programmation de l'ARDUINO,
 et 0 et 1 si on utilise le moniteur série (Serial.print ...)
 ce qui n'est pas très sympa pour le PA du relais !!!.
 */

#if quel_relais == HF51
// ces 3 pin sont faites pour programmer la fréquence (pour le relais 50Mhz / F4ASS made)
int dataPin = 3;   //Pin connected to DATA PLL
int clockPin = A1; //Pin connected to Clock PLL
int latchPin = A2; //Pin connected to Latch PLL
#endif

// récupération du RSSI
int analogInPin = A0;  // entree niveau signal de réception ANA0 entre 0 et 5v (S-metre)
// NB : sur le FT480, on a une tension qui varie de 0 à 4 Volts environ, donc j'ai simplement mis une R en série
// un étalement de 200 à 600 en valeur récupérée semble OK (pas généreux globalement).
// voir plus bas l'échelle

int TX = 5;           // Pin de commande de l'emission (TX actif si HIGH)
// pour le FT480, j'ai mis un transistor collecteur ouvert sur le PTT
#define TX_ON HIGH		// (actif si HIGH)
#define TX_OFF LOW

int BF = 4;            // Pin de Commande de la matrice BF pour le MP3
// BF est BF_ON si besoin du MP3 ou BF du Rx non transmise au TX
#define BF_ON HIGH		// (actif si HIGH)
#define BF_OFF LOW	        // (actif si LOW)
// pour le FT480 :inutile, c'est pas un relais !

///////////// init des variables ///////////
// S-mètre : en fonction du RSSI recu, on passera un "K" différent
// la 1° valeur sert pour créer un 'hystéresis' afin d'éviter de couper sur un signal fluctuant autour de la 2° valeur
// niveaux de déclenchement du squelch

// FT480 balise
//#define squelch_off 1		// pour arret
//#define squelch_on 5		// pour mise en route
//int smetre[] =      {150, 200, 220, 240, 260, 280, 300, 320, 340, 360, 400};    // FT480
// version pour les 'piocheurs'
//char cwmetre[] =   { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'K'};
/*
// version basique (- . .. ... .... ..... -.-)
 int smetre[] =      {150, 200, 280, 360, 440, 520, 600};    // FT480
 char cwmetre[] =   { 'O', 'E', 'I', 'S', 'H', '5', 'K'};
 */

// F4ASS / relais 51 MHz
//int SQL=500;        // Niveau déclenchement du squelch (0-1024) 390 pour -115dBm (F4ASS)
#define squelch_off 2		// pour arret
#define squelch_on 5		// pour mise en route
int smetre[] =        {
  420, 450, 480, 510, 540, 570, 600, 630, 660, 690, 720
};    // relais 51 Mhz
//  0    1    2    3    4    5    6    7    8    9   9+
//int smetre[] =      {390, 420, 450, 475, 500, 525, 550, 575, 600, 625};    // autre échelle plus optimiste ?

// temporisations en millisecondes
int tempo_general;                                  // ms. tempo du grafcet general
int tempo_montee_relais_reset              =   50;  // ms. tempo montée du relais
int tempo_ouverture, tempo_ouverture_reset =  250;  // ms. tempo pour éviter ouverture sur parasite
int tempo_fermeture, tempo_fermeture_reset =  500;  // ms. tempo pour éviter fermeture sur QSB mobile
int tempo_maintien, tempo_maintien_reset   = 6000;  // ms. tempo de maintien du relais avant arrêt
int tempo_passage_K, tempo_passage_K_reset =  500;  // ms. tempo avant passage du K
int kpasse = true;      // mémoire K passé

// temporisations en SECONDES
int tempo_ab, tempo_ab_reset                         =  180;  // sec. tempo anti bavard (en secondes)
int tempo_reprise, tempo_reprise_reset               =   20;  // sec. tempo de purgatoire anti-bavard et sécurité
int tempo_TXmaxi, tempo_TXmaxi_reset                 = 1200;  // sec. Sécurité : temps maxi en TX de 20mn sans retombée

// indicateur horloge
int horloge_synchronisee_maxi = 30 ; // maximum admis sans ynchro (si plus, on ne dit plus l'heure)
int horloge_synchronisee = horloge_synchronisee_maxi - 2; // minutes depuis dernière synchronisation
// (on aura donc une valeur "bidon" énoncée au 1er essai juste après démarrage)

// météo
int pression = 1013, ok_pression = horloge_synchronisee;
int temper = 215, ok_temper = horloge_synchronisee;
int humid = 55, ok_humid = horloge_synchronisee;
int lumin = 78, ok_lumin = horloge_synchronisee;

// géré par l'horloge au niveau minutes
int repete_indicatif;                     // passage toutes les x minutes rondes
int emettre_indicatif = false;                  // demande passage indicatif

//int j = 0;              // compteur utilisé pour le chargement PLL (permet de la charger qu'une fois)
int tempo_PLL_reset = 10;  // valeur de temporisation de chargement PLL ( en millisecondes) tempo_general

// signal recu
unsigned int RSSI_inB;		// valeur Brute
unsigned int RSSI_in;		// valeur validée
unsigned int RSSImoy = 0;		// valeur lissée dans le temps
unsigned int RSSI_old = 1;    	// mémoire
#define  RSSI_nb 50 	// maxi = 65536/1024 = 60 nombre de valeurs prises pour calcul moyenne

int TX_autorisation = 1;                  // autorisation transmission, valeur par défaut au démarrage

// grafcets
int grafcet_general = 1, grafcet_general_old;

// variables diverses
int i, j;

//**********************************************************
// SOUS-PROGRAMMES
// gestion du temps, formatages, gestion clavier
#include "ger_format.h"
//#include "ger_clavier.h"
#define ger_serial_timeout 5    //ms
#include "ger_serial.h"

// routines locales
#include "mp3_routines.h"
//**********************************************************
// INITIALISATIONS
void setup() {

  /*
  // pour essais, tempos plus courtes
  tempo_ab_reset                    = 15;   // sec. tempo anti bavard (en secondes)
  tempo_reprise_reset               = 10;  // sec. tempo de purgatoire anti-bavard et sécurité
  tempo_TXmaxi_reset                = 60;   // sec. Sécurité : temps maxi en TX de 20mn sans retombée (en secondes)
  repete_indicatif                  = 2;    // passage toutes les x (minutes rondes)
  */
  tempo_ab_reset                    =  180;  // sec. tempo anti bavard (en secondes)
  tempo_reprise_reset               =   20;  // sec. tempo de purgatoire anti-bavard et sécurité
  tempo_TXmaxi_reset                = 1200;  // sec. Sécurité : temps maxi en TX de 20mn sans retombée (en secondes)
  repete_indicatif                  = 10;    // passage indicatif et infos toutes les x (minutes rondes)
  // obsolete  tempo_reprise_rapide_reset =  15; //180;  // sec. tempo de reprise rapide si arrêt depuis peu

  // ligne série (pour traces et/ou réception informations (voir mp3_traitement.h)
  Serial.begin(57600);  // 57600 est la vitesse maximale admise par les Arduino à 8Mhz
  //  note : les lignes précédées de 'a' ou 'b' sont destinées à un afficheur 'ARDUINO série' (1° et 2° ligne)
  //  Il y a des 'gris-gris' sur la sortie série (sans doute utilisée par le shield MP3)
  Serial && versionx && endli;

  //Définition des e/s
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(TX, OUTPUT);
  // commande matrice BF pour la BF MP3
  pinMode(BF, OUTPUT);

#ifdef mode_balise
  repete_indicatif = 5;
  //  CW_SPEED=CW_SPEED*5;		        // en mots/minute (15=80ms le point)
  //  duree_note0=duree_note0/5;      // ms de base
  TX_autorisation = 0;

  Serial && horodatage() && " MODE BALISE" && endli;
#endif

#ifdef DCF77_synchro
  pinMode(DCF_PIN, INPUT);
  digitalWrite(DCF_PIN, HIGH);   // pour mise en service de la résistance de pullup
  //  DCF.Start(); on va se passer d'interruption, donc on ne fait pas ca ! ...
  //gmt_decalage = gmt_hiver;
#endif
  // mise à l'heure pour démarrage : on va avoir teste dans 10 sec.
  setTime( 10, 59, 50, 1, 1, 2015);

  digitalWrite(TX, LOW); // Sécurité coupure du TX

  //---------------------------------------------
  // Serial.print(F("Free RAM = ")); // available in Version 1.0 F() bases the string to into Flash, to use less SRAM.
  // Serial.print(FreeRam(), DEC);  // FreeRam() is provided by SdFatUtil.h
  //   Serial.println(F(" Should be a base line of 1029, on ATmega328 when using fINTx"));

  //Initialize the SdCard.
  if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt(); // selection vitesse
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir"); // récupération répertoire

  //Initialize the MP3 Player Shield
  mp3_result = MP3player.begin();

  /*
  //Normalement, tout se passe bien, donc on attends le message de démarrage ...
   //check result, see readme for error codes.
   if(mp3_result != 0) {
   Serial.print(F("Error code: "));  Serial.print(result); Serial.println(F(" when trying to start MP3 player"));
   if( result == 6 ) {
   Serial.println(F("Warning: patch file not found, skipping.")); // can be removed for space, if needed.
   Serial.println(F("Use the \"d\" command to verify SdCard can be read")); // can be removed for space, if needed.
   }
   }
   */
  /*
  #if (0)
   // Typically not used by most shields, hence commented out.
   Serial.println(F("Applying ADMixer patch."));
   if(MP3player.ADMixerLoad("admxster.053") == 0) {
   Serial.println(F("Setting ADMixer Volume."));
   MP3player.ADMixerVol(-3);
   }
   #endif
   */

  // plus ou moins de volume au démarrage
  //   delay(500);
  union twobyte mp3_vol; // create key_command existing variable that can be both word and double byte of left and right.
  mp3_vol.word = MP3player.getVolume(); // returns a double uint8_t of Left and Right packed into int16_t
  // mp3_vol.byte[1] += 0; // écouteurs confortable
  // mp3_vol.byte[1] -= 30;  // haut-parleur fort
  mp3_vol.byte[1] -= 5; // relais 51 MHz correct
  // mp3_vol.byte[1] -30   -20   -10    0    +10   +20   +30
  // resultat :      -5dB  -10dB -15dB -20dB -25dB -30dB -35dB

  // push byte[1] into both left and right assuming equal balance.
  MP3player.setVolume(mp3_vol.byte[1], mp3_vol.byte[1]); // commit new volume
  /*
  Serial.print(F("Volume changed to -"));
   Serial.print(mp3_vol.byte[1]>>1, 1);
   Serial.println(F("dB"));
   */
  /*
  // bip de démarrage
   MP3player.enableTestSineWave(200); // + grand = + grave !
   delay(200);
   MP3player.disableTestSineWave();
   */

  // message de démarrage (c'est plus élégant)
  MP3player.playMP3("mp3o.mp3");
  //    serial_last_ms_char = millis(); // stroke the inter character timeout.
  //    mp3_buffer_i = 0; // start the command string at zero length.

  Serial && "b" && horodatage() && "SetupOK" && endli;
  while ( MP3player.isPlaying() ) {
    delay(1000);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************
// BOUCLE GENERALE
void loop() {

  /*
  // Below is only needed if not interrupt driven. Safe to remove if not using.
   #if defined(USE_MP3_REFILL_MEANS) \
   && ( (USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer) \
   ||   (USE_MP3_REFILL_MEANS == USE_MP3_Polled)      )
   MP3player.available();
   Serial.println(F("MP3player.available"));
   #endif
   */
  // gestion temps, temporisations (en ms) et horloge
  ger_top_centieme ();

  //--------------------------------
  // chaque centieme de seconde, top10 retourne le nombre de millisecondes passées :
  // pourquoi pas 10 à chaque coup ? si le programme est lent, on peut avoir dépassement, donc on essaie de récupérer au mieux !
  if (top_10 > 0) {
    // gestion des TEMPORISATIONS (en ms mais précise à 10ms près)
    //     GER_TEMPO(tempo_cw,top_10 );
    //     GER_TEMPO(tempo_attcw,top_10 );
    GER_TEMPO(tempo_general, top_10 );
    GER_TEMPO(tempo_maintien, top_10 );
    GER_TEMPO(tempo_passage_K, top_10 );
    GER_TEMPO(tempo_ouverture, top_10 );
    GER_TEMPO(tempo_fermeture, top_10 );
    GER_TEMPO(tempo_inter_mp3, top_10 );

    // surveillance/decodage de la DCF77
    // il suffit d'appeler la routine à chaque changement d'état du DCF, que l'on va scruter à 10ms
#ifdef DCF77_synchro
    if (digitalRead(DCF_PIN) != dcf_memoire) {
      DCF.int0handler();
      dcf_memoire = digitalRead(DCF_PIN);
      // Serial && ";" && horodatage() && "DCF77=" && dcf_memoire && endli;

      if (dcf_memoire) {
        // Mise à l'heure ? Check if new DCF77 time is available
        DCFtime = DCF.getTime();
        // si ok
        if (DCFtime != 0) {
          setTime(DCFtime);
          //      attente_misalheure=0;
          horloge_synchronisee = 0;
          if (debug > 0) Serial && horodatage() && " Time is updated" && endli;
        }
      }
    }
#endif

    // RECEPTION VOIE SERIE
    serial_reception();
    // TRAITEMENT DES MESSAGES RECUS par la voie série et des MP3 à envoyer
#include "mp3_traitement.h"

    // raz mémoire
    top_10 = 0;
  }

  //--------------------------------
  // chaque dixieme de seconde :
  if (top_100ms) {
    top_100ms = false;

    // gestion du squelch, ou plutôt du niveau de signal recu
    // FT480 : filtrer par 10K série + condo 220nF à 2uF
    RSSI_inB = analogRead(analogInPin);     // Lecture de la broche RSSI
    // ouverture du relais si > sur smetre[squelch_on] pendant 'tempo_ouverture_reset'
    // il suffit de tester : if (tempo_ouverture==0)
    if (RSSI_inB < smetre[squelch_on] ) {
      tempo_ouverture = tempo_ouverture_reset;
    }
    // fermeture du relais si < sous smetre[squelch_off] pendant 'tempo_fermeture_reset'
    // il suffit de tester : if (tempo_fermeture==0)
    if (RSSI_inB > smetre[squelch_off] ) {
      tempo_fermeture = tempo_fermeture_reset;
    }
    // élaboration d'un RSSI_in validé
    if (tempo_fermeture == 0) {
      RSSI_in = 0;
    }
    else {
      if (RSSI_inB > smetre[0]) RSSI_in = RSSI_inB;  // seulement si il y a du signal
      // RSSImoy = lissage sur plusieurs secondes
      //         RSSImoy est remis à 0 lors du passage du K

      if (RSSImoy == 0) RSSImoy = RSSI_in;   // pour initialiser si besoin
      RSSImoy = ((RSSI_nb - 1) * RSSImoy + RSSI_in) / RSSI_nb; // pour lisser dans le temps
    }

    // RECEPTION VOIE SERIE
    //    serial_reception();

    // TRAITEMENT DES MESSAGES RECUS par la voie série et des MP3 à envoyer
    //#include "mp3_traitement.h"

  }  // fin dixième de seconde

  //--------------------------------
  // chaque seconde :
  if (top_1000ms) {
    top_1000ms = false;

    // gestion des TEMPORISATIONS (en secondes)
    GER_TEMPOL(tempo_ab);
    GER_TEMPOL(tempo_reprise);
    GER_TEMPOL(tempo_TXmaxi);
    //    GER_TEMPOL(tempo_reprise_rapide);

  }   // fin seconde


  //--------------------------------
  // chaque minute :
  if (top_60000ms) {
    // temps depuis lequel on n'a pas eu de synchro / valeur transmise
    incremente (horloge_synchronisee, 99);
    incremente (ok_pression, 99);
    incremente (ok_temper, 99);
    incremente (ok_humid, 99);
    incremente (ok_lumin, 99);
    // indicatif toutes les 'repete_indicatif' minutes
    if ( minute() % repete_indicatif == 0 ) emettre_indicatif = true;
    top_60000ms = false;
  }  // fin minute

  //--------------------------------********************--------------------------------
  //  GRAFCET DE FONCTIONNEMENT GENERAL
  //*************************************************

  // trace
  if ( debug > 0 && grafcet_general != grafcet_general_old) {
    grafcet_general_old = grafcet_general;
    //     horodatage_serial(); Serial && "grafcet_general=" && grafcet_general && endli;
    Serial && ";" && horodatage() && "grafcet_general=" && grafcet_general && " mp3=" && MP3player.isPlaying() && endli; // && " TRR=" && tempo_reprise_rapide && endli;
  }
  //*************************************************
  // pendant ces parties de grafcet, lors de messages MP3,
  //   si on a un signal, on écourte et on donne priorité au traffic !
  if ( tempo_ouverture == 0 && (grafcet_general == 18 || grafcet_general == 26  || grafcet_general == 27  || grafcet_general == 28 )) {
    strcpy(mp3_buffer, "STOP");
    mp3_buffer_recu = 4;
    grafcet_general = 20; // on va en ouverture
  }

  switch (grafcet_general) {

    ////////////////// Chargement PLL une seule fois au démarrage
    // cette séquence est particulière au TX mis en oeuvre !
    case 1:
#if quel_relais == HF51
      // 51.950 mémoire 44
      digitalWrite(latchPin, HIGH); // mode prog

      PLL_prog(0);
      PLL_prog(0);
      PLL_prog(1);
      PLL_prog(0);

      PLL_prog(1);
      PLL_prog(1);
      PLL_prog(0);
      PLL_prog(0);

      delay (tempo_PLL_reset);
      digitalWrite(latchPin, LOW);

      delay (tempo_PLL_reset);
      digitalWrite(clockPin, LOW);
      delay (tempo_PLL_reset);
      digitalWrite(clockPin, HIGH);  // Valide ###### La prog

      delay (tempo_PLL_reset);
      digitalWrite(latchPin, HIGH);

      delay (tempo_PLL_reset);
      // peut-on tout RAZ ???
      /*
      digitalWrite(latchPin, LOW);
       digitalWrite(clockPin, LOW);
       digitalWrite(dataPin, LOW);
       */
#endif

      //      tempo_reprise_rapide = 0;
      grafcet_general = 19;      // on coupe bien tout et on passe en STANDBY
      break;

    /////////////////// COUPURE TX STOP
    // reset tempo
    case 18 :
      if (mp3_buffer_recu == 0) {
        //      tempo_reprise_rapide = tempo_reprise_rapide_reset;
        grafcet_general++;
      }
      break;
    // coupure effective
    case 19 :
      if (tempo_general == 0 && mp3_buffer_recu == 0) {
        Serial && "b" && horodatage() && "TX off" && endli;
        digitalWrite(TX, TX_OFF);     // Coupure du TX
        digitalWrite(BF, BF_OFF);    // arret redirection de la BF
#ifdef mode_balise
        BALISE_TX_OFF
#endif
        grafcet_general++;         // on passe en STANDBY
      }
      break;
    // T09:59:57.28.06.15
    /////////////////// STANDBY, attente demande de passage en émission
    case 20 :
      //      if (emettre_indicatif) Serial && horodatage() && "Emettre indicatif !!! " && tempo_reprise && " " && tempo_general && endli;

      // si la tempo_ouverture est terminée
      //   ou si on doit émettre l'indicatif
      //  et si pas purgatoire de l'anti-bavard (tempo_reprise)
      if ((tempo_ouverture == 0 || emettre_indicatif) && tempo_reprise == 0 && tempo_general == 0) {
        if ( debug > 1) {
          Serial && horodatage() && "Start" && endli;
        }
        // charge la valeur de l'anti bavard
        tempo_ab = tempo_ab_reset;
        // charge la valeur de la tempo maxi
        tempo_TXmaxi = tempo_TXmaxi_reset;    // Sécurité : temps maxi en TX sans retombée (en secondes)
        // et on passe en émission
        grafcet_general++;
      }
      /*
      // surveillance RSSI même si fermé  (debug)
            if ( debug > 2 && analogRead(A0) > smetre[squelch_off] / 2 && tempo_general == 0) {
              Serial && "b" && horodatage() && "STBY A0=" && analogRead(A0) && "=" && RSSI_in && "/" && RSSImoy && endli;
              tempo_general = 200;
            }
      */
      break;

    /////////////////// Passage en émission
    case 21 :
      // on passe en émission
#ifdef mode_balise
      //      tempo_reprise_rapide = 1; // pour aller plus vite en mode balise
      // on passe en émission seulement à la retombée, ou si reprise rapide
      if ( RSSI_in < smetre[squelch_off] || tempo_reprise_rapide > 0) {
#endif
        kpasse = false;
        tempo_passage_K = tempo_passage_K_reset;
        tempo_maintien = tempo_maintien_reset;

        // pour emission indicatif rapidement après mise en TX
        if (emettre_indicatif) {
          tempo_maintien = 1500;
          tempo_passage_K = 0;
          kpasse = true;
        }
        Serial && "b" && horodatage() && "TX ON=" && RSSI_in && endli;
        grafcet_general++;    // on passe directement en émission

        // BF du RX sur le TX
        // (seulement si signal en entrée, sinon c'est envoi indicatif, ceci évite le PSHIIIIT au démarrage)
        if ( RSSI_in > 0 ) {
          digitalWrite(BF, BF_OFF);
        } else {
          digitalWrite(BF, BF_ON);
        }
        if (TX_autorisation == 1) digitalWrite(TX, HIGH); // Passe en emission
        tempo_general = tempo_montee_relais_reset; // tempo de montée du relais
#ifdef mode_balise
        BALISE_TX_ON
      }
#endif
      break;

    /////////////////// SURVEILLANCE de la TRANSMISSION
    // on est dans cette etape pendant toute la transmission
    case 22 :
      // signal ok, on maintien
      if ( tempo_general == 0 && tempo_fermeture != 0 ) {
        // affichage du S-mètre toutes les secondes
        if ( debug == 0 && tempo_general == 0 ) {
          i = S_metre (RSSI_in);
          j = S_metre (RSSImoy);
          // ahh:mm BF S9~9 789 799
          // 123456789123456
          Serial && "a" && horodatage() && "BF S" && i && "~" && j && " " && RSSI_in && "~" && RSSImoy && endli;
          tempo_general = 1000;
        }
        // maintien en émission avec transmission BF
        kpasse = false;
        tempo_passage_K = tempo_passage_K_reset;
        tempo_maintien = tempo_maintien_reset;
        //        digitalWrite(BF, BF_ON);    // redirection de la BF (bascule la BF sur l'emetteur)

        // plus de signal (tempo_fermeture finie) , on coupe la BF
      }
      else {
        //        digitalWrite(BF, BF_OFF); // arret redirection de la BF ( remets la BF en haute impédance)
        tempo_ab = tempo_ab_reset;  // recharge la valeur de l'anti bavard
        //        tempo_reprise_rapide = tempo_reprise_rapide_reset;// tempo pour reprise rapide
      }

      // plus de signal depuis un peu de temps, on passe un K
      // si RSSImoy == 0 on est en passage automatique des X minutes
      if ( !kpasse && tempo_passage_K == 0 && mp3_buffer_recu == 0 && RSSImoy > smetre[0]) {
#ifdef mode_balise
        BALISE_TX_ON
#endif
        // pour envoyer 5X et K selon RSSI
        i = S_metre (RSSImoy);
        if (i < 10) {
          strcpy(mp3_buffer, "59--xxxk");
          mp3_buffer[1] = 48 + i;          // 50 à 59
        }
        else {
          strcpy(mp3_buffer, "pl59xxxk");  // 59 plus
        }  // S mètre
        mp3_buffer_recu = 8;
        Serial && "a" && horodatage() && "K S" && i && endli; // " " && RSSImoy && endli;
        kpasse = true;
        RSSImoy = 0;
#ifdef mode_balise
        grafcet_general++;  // attente fin CW, et retour en attente
        //      tempo_general=tempo_montee_relais_reset; // tempo de montée du relais
#endif
      }

      // plus de signal depuis longtemps
      // envoi de fin d'émission ou indicatif
      if ( tempo_maintien == 0 && mp3_buffer_recu == 0) {
#ifdef mode_balise
        BALISE_TX_ON
#endif
        Serial && "b" && horodatage() && "BYE" && endli;
        grafcet_general = 24;
      }

      // anti-bavard
      if (tempo_ab == 0 && mp3_buffer_recu == 0) {
#ifdef mode_balise
        BALISE_TX_ON
#endif
        digitalWrite(BF, BF_OFF);
        Serial && "b" && horodatage() && "ABAV" && endli;
        strcpy(mp3_buffer, INDICATIF_ANTIBAVARD);
        mp3_buffer_recu = strlen(INDICATIF_ANTIBAVARD);
        tempo_general = 100;
        tempo_reprise = tempo_reprise_reset;
        grafcet_general = 18;  // attente fin message et coupure
      }

      // QRM : cela veut-il dire que l'on ne peut pas rester plus de 20 minutes continues en QSO ? OUI !
      // Sécurité : temps maxi en TX de 20mn sans retombée
      if (tempo_TXmaxi == 0 && mp3_buffer_recu == 0) {
#ifdef mode_balise
        BALISE_TX_ON
#endif
        digitalWrite(BF, BF_OFF);
        Serial && "b" && horodatage() && "QRM*" && endli;
        strcpy(mp3_buffer, INDICATIF_QRM);
        mp3_buffer_recu = strlen(INDICATIF_QRM);
        tempo_general = 100;
        tempo_reprise = tempo_reprise_reset;
        grafcet_general = 18;  // attente fin message et coupure
      }

      break;

    /*
    /////////////////// attente fin messages avant coupure (anti bavard et QRM)
        case 23:
          if (tempo_ab == 0 && mp3_buffer_recu == 0) {
            //        if (TX_autorisation == 1) digitalWrite(TX, LOW);  // coupe l'emission
            grafcet_general = 18;  // coupure
          }
          break;
    */

    /////////////////// Indicatif relais
    case 24:
      if (mp3_buffer_recu == 0) {
        grafcet_general++;      // passage grand indicatif
      }
      break;


    // avant INDICATIF :  heure si synchronisee
    case 25 :
      if (tempo_general == 0 && mp3_buffer_recu == 0) {
        // il est : heure minute
        if (horloge_synchronisee < horloge_synchronisee_maxi) {
          strcpy(mp3_buffer, "iles");
          strcat(mp3_buffer, format_4c(hour()));
          strcat(mp3_buffer, "heur");
          mp3_buffer_recu = 12;
          if (minute() > 0) {
            strcat(mp3_buffer, format_4c(minute()));
            mp3_buffer_recu += 4;
          }
          //          Serial && "b" && horodatage() && "HR:" && mp3_buffer && endli ;
          Serial && "bHR:" && mp3_buffer && endli ;
        }
        // sinon, message de base
        else {
          strcpy(mp3_buffer, MUSIQUE_DEMARRAGE);
          mp3_buffer_recu = strlen(MUSIQUE_DEMARRAGE);
          Serial && "b" && horodatage() && "Start " && endli;
        }
        grafcet_general++;
      }
      break;

    // GRAND ou PETIT INDICATIF
    case 26 :
      if (tempo_general == 0 && mp3_buffer_recu == 0 ) {
        Serial && "b" && horodatage() ;
        // indicatif long (chaque x minutes)
        if ( emettre_indicatif ) {
          //          emettre_indicatif = false;

          strcpy(mp3_buffer, INDICATIF_LONG);
          mp3_buffer_recu = strlen(INDICATIF_LONG);
          Serial && "Long ";
          grafcet_general = 31;  // meteo
        }
        else {
          // indicatif court et pas de meteo
          strcpy(mp3_buffer, INDICATIF_COURT);
          mp3_buffer_recu = strlen(INDICATIF_COURT);
          Serial && "Court" && endli;
          grafcet_general++;    // fin
        }
      }
      break;

    /////////////////// attente fin émission CW et passage en standby
    // attente fin message
    case 27 :
      if (tempo_general == 0 && mp3_buffer_recu == 0) {
        tempo_ab = tempo_ab_reset;      // reset anti-bavard
        tempo_maintien = tempo_maintien_reset;
        grafcet_general++;
#ifdef mode_balise
        BALISE_TX_OFF
#endif
        if ( emettre_indicatif ) emettre_indicatif = false;
      }
    //      break;

    // musique de fin : de fin0 à fin5
    case 28 :
      if (tempo_general == 0 && mp3_buffer_recu == 0) {
        strcpy(mp3_buffer, "fin");
        mp3_buffer[3] = 48 + millis() % 6; // de fin0 à fin5
        mp3_buffer_recu = 4;
        //      strcpy(mp3_buffer,MUSIQUE_ARRET);
        //      mp3_buffer_recu = strlen(MUSIQUE_ARRET);
        grafcet_general = 18;    // vers COUPURE
      }
      break;

    /////////////////// meteo : pression, temperature, humidite, lumière
    case 31:
      if (tempo_general == 0 && mp3_buffer_recu == 0) {
        Serial && "Meteo" && endli;
        if (ok_pression < horloge_synchronisee_maxi) {
          strcpy(mp3_buffer, "pres");
          mp3_buffer_recu = 16;
          if (pression >= 1000) {
            strcat(mp3_buffer, "1000");
          } else {
            strcat(mp3_buffer, format_4c((pression / 100) % 10));
            strcat(mp3_buffer, "100-");
            mp3_buffer_recu += 4;
          }
          strcat(mp3_buffer, format_4c (pression % 100));
          strcat(mp3_buffer, "mbar");
        }
        grafcet_general++;
      }
      break;
    case 32:
      if (tempo_general == 0 && mp3_buffer_recu == 0) {
        if (ok_temper < horloge_synchronisee_maxi) {
          strcpy(mp3_buffer, "temp");
          mp3_buffer_recu = 12;
          if (temper < 0) {
            strcat(mp3_buffer, "moin");
            mp3_buffer_recu += 4;
          }
          strcat(mp3_buffer, format_4c(abs(temper) / 10));
          strcat(mp3_buffer, "degr");
          if (temper % 10 != 0) {
            strcat(mp3_buffer, format_4c(abs(temper % 10)));
            mp3_buffer_recu += 4;
          }
        }
        grafcet_general++;
      }
      break;
    case 33:
      if (tempo_general == 0 && mp3_buffer_recu == 0) {
        if (ok_humid < horloge_synchronisee_maxi) {
          strcpy(mp3_buffer, "humi");
          strcat(mp3_buffer, format_4c(humid));
          strcat(mp3_buffer, "prct");
          mp3_buffer_recu = 12;
        }
        grafcet_general++;
      }
      break;
    case 34:
      if (tempo_general == 0 && mp3_buffer_recu == 0) {
        if (ok_lumin < horloge_synchronisee_maxi) {
          strcpy(mp3_buffer, "lumi");
          strcat(mp3_buffer, format_4c(lumin));
          strcat(mp3_buffer, "prct");
          mp3_buffer_recu = 12;
        }
        grafcet_general++;
      }
      break;
    case 35:
      grafcet_general = 27;  // final
      break;

    ///////////////////  si grafcet_a n'a pas une valeur correcte, on passe ici
    default:
      Serial && ";" && horodatage() && "ERREUR grafcet_general=" && grafcet_general && endli;
      digitalWrite(TX, TX_OFF);       // Coupure du TX de sécurité
      digitalWrite(BF, BF_OFF);      // arret redirection de la BF
      //    digitalWrite(CW, LOW);       // on dé-valide la BF CW
      grafcet_general = 1;         // on retourne au debut
      break;
  } // fin switch grafcet_general
  //--------------------------------********************--------------------------------
  // gestion envoi au MP3
#include "mp3_traitement.h"

  //--------------------------------********************--------------------------------
} // fin loop() GENERALE

