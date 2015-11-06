//************************************
// *** CLAVIER *** clavier d'entr?e VErsion 07/2015 avec ROTOPUSH
// clavier à touches et diodes sur entrée analogique
// ATTENTION ! définir l'entrée clavier si on veut qu'il soit géré :
#ifndef ger_clavier
#define ger_clavier A0	// entrée ana du clavier
#endif

// clavier par d?faut si on ne d?finit rien : 10 touches à diodes BAT47/54 tirage pullup seul ou 10-20kOhms
#ifndef ger_clavier_nb
#define ger_clavier_nb 10			// nombre de touches
#endif

#ifdef ger_ntouche_nb
#define ger_memoire_nb ger_ntouche_nb         // nombre de memoires anti-rebond (ancien nom)
#endif
#ifndef ger_memoire_nb
#define ger_memoire_nb 3          // nombre de memoires anti-rebond
#endif

#ifndef ger_clavier_delta
#define ger_clavier_delta 41		// ?cart entre valeurs
#endif
// valeur maxi clavier au repos =
//int ger_clavier_max = min((ger_clavier_nb+1)*ger_clavier_delta,1000);

#ifndef ger_clavier_repetition
#define ger_clavier_repetition 500		// ms entre r?tition automatique
#endif

#ifndef LISTE_TOUCHE
#define LISTE_TOUCHE() char ltouche[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' '};
#endif

    /* roto push
    j'ai r?p? dans des claviers d'ordis hs des boutons rotatifs utilis?initialement pour changer le volume.
    Ce sont des boutons de 30mm environ, surlesquels on peut aussi appuyer (clic !)
    Cet appui peut ?e g? comme une simple touche - ok - c'est facile.
    La rotation, elle, d?vre 2 contacts : 3 broches, celle de gauche = G.

    lorsque G monte (front montant),
      si on a D il faut descendre
      si on n'a pas D, il faut monter
    */

#ifdef ROTOPUSH
int rototouche;	// mémoire touche
#define ROTOPUSHADD 2	// 2 touches de plus
#else
#define ROTOPUSHADD 0
#endif

//int itouche
int ntouche; 
unsigned int tempo_repeat;
int ntouche_mem[ger_memoire_nb];      // m?moires pour anti-rebond
// touche renvoyée
char touche = ' ', touche_repeat = ' ', vtouche = ' ';
/*

// CLAVIER 3 TOUCHES + ROTOPUSH A DIODES 0,6V type 1N4148
#define ger_clavier A0
#define ger_memoire_nb 2
#define ger_clavier_nb 4		// ne pas compter les 2 signaux du ROTOPUSH
#define ger_clavier_delta 160
#define LISTE_TOUCHE() char ltouche[] = {'0', '1', '2', 'V', '+', '-', ' '};

// clavier 1+5 touches avec 6 diodes type 1N4148 COURBON NOIR (1? touche masse directe inexistante en g?n?ral)
#define ger_clavier A0
#define ger_memoire_nb 6	// nombre de m?moires anti-rebond (6=normal 2=pour clavier aliment? par enable LCD)
#define ger_clavier_nb 6	// nombre de touches du clavier
// valeurs relev? : 141 244 344 440 532
#define ger_clavier_delta 109 
// (ger_clavier_nb+1) valeurs representant les touches attendues, la derni?re = ' '
#define LISTE_TOUCHE() char ltouche[] = {' ', '3', '4', '2', '1', '5', ' '}; // 1=AUX 2=INV 3=v 4=^ 5=VAL (COURBON NOIR)
// +- compatible 10 touches les 5 touches correspondent aux 4 centrales d'un clavier 10 touches + VAL=touche '0'
//#define LISTE_TOUCHE() char ltouche[] =  {' ', '5', '6', '4', '3', '0', ' '};
// 0,  118, 217, 318, 408, 498, 1023 valeurs exp?rimentales tirage par la pullup seulement
//delta=118  108  106  102  100 => 109
// 0,  120, 221, 319, 414, 506, 1023 valeurs exp?rimentales tirage par 10Kohms
//delta=120  110  106  103  101 => 110

// CLAVIER 10 TOUCHES A DIODES 0,2V type BAT47
#define ger_clavier A0
#define ger_memoire_nb 6
#define ger_clavier_nb 10
#define ger_clavier_delta 41
#define LISTE_TOUCHE() char ltouche[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' '};
// 0,  61, 108, 154, 199, 243, 287, 330, 372, 414, 456, 1023 valeurs exp?rimentales II
// 0,  45,  90, 136, 181, 226, 270, 313, 354, 396, 437, 1023 valeurs exp?rimentales I
// 0,  41,  82, 123, 164, 205, 246, 287, 328, 369, 410, 1023 valeur *41

// CLAVIER 4+16 TOUCHES A DIODES 0,2V type BAT47
#define ger_clavier A0
#define ger_memoire_nb 6
#define ger_clavier_nb 20
#define ger_clavier_delta 41
#define LISTE_TOUCHE() char ltouche[] = {'T', 'U', 'V', 'W', '*', '0', '#', 'D', '7', '8', '9', 'C', '4', '5', '6', 'B', '1', '2', '3', 'A'};    // nbtouche

*/
// macro de d?finition des touches retournées
LISTE_TOUCHE()

// ************************************************************************************************
/* USAGE : scrutation et filtrage de clavier analogique

  * initialiser l'entr?avec pullup :
   pinMode(ger_clavier, INPUT);
   digitalWrite(ger_clavier, HIGH); // r?stance de pullup

  * g?r la tempo et appeler la scrutation dans la zone 10ms :
   GER_TEMPO(tempo_repeat,top_10)
   ger_scrute_clavier();

  * utilisation "monocoup" :
   if (touche == '1') { ...; touche=' ';}

  * utilisation avec "r?tition" :
   if (touche_repeat == '1') { ...; touche_repeat=' ';}
*/

void ger_scrute_clavier() {

  int i;
  // clavier - d?calage pour filtrage sur NO de touche (c'est plus stable que sur valeur)
  for (i = ger_memoire_nb - 1; i > 0; i--) {
    ntouche_mem[i] = ntouche_mem[i - 1];
  }
  ntouche_mem[0] = ntouche;	// avant-derni?re valeur
  // Lecture entr?e et on applique la formule n = ( valeur_ana + demi_?cart ) / ?cart
  //    Serial && analogRead(ger_clavier) && endli;
  ntouche = (analogRead(ger_clavier) + ger_clavier_delta / 2) / ger_clavier_delta;
  ntouche = min(ntouche, ger_clavier_nb + ROTOPUSHADD);
  // recherche stabilite (anti-rebond) (X tours)
  i = 0;
  do {
    if (ntouche != ntouche_mem[i]) i = -10;	// non stable, et fin de do while
    i++;
  } while (i > 0 && i < ger_memoire_nb); // si stable, i est positif == ger_memoire_nb

  // si la touche est stable
  if (i > 0) {
    // a-t-elle vari? ?
    if (ltouche[ntouche] != vtouche ) {
      vtouche = ltouche[ntouche];
      // touche valide APPUYEE
      if (ntouche < ger_clavier_nb) {
        touche = vtouche;
        tempo_repeat = 1000;  // duree avant repetition
        // touche RELACHEE = pas de touche (tension ?lev?e), arret r?p?tition
        // ou ROTOTOUCHE
      } else {
        touche_repeat = ' ';
#ifdef ROTOPUSH
        if (ntouche == ger_clavier_nb + 1) {
//              Serial && "130:" && ntouche && rototouche && endli;
          if (rototouche == 0) rototouche = ntouche;
          touche = ' ';	
        } else {
          if (ntouche == ger_clavier_nb) {
            if (rototouche == 0) {
//              Serial && "140:" && ntouche && rototouche && endli;
              rototouche = ntouche;
              touche = ' ';	// on ignore la touche dans ce cas-l?
            } else {
//              Serial && "144:" && ntouche && rototouche && endli;
              touche = ' ';	// on ignore la touche dans ce cas-l?
            }
          } else {
            if (rototouche != 0 ) {
//              Serial && "147:" && ntouche && rototouche && endli;
              touche = ltouche[rototouche];
              rototouche = 0;
            }
          }
        }
#endif
      }
      // si la touche est la m?me
    } else {
      // fin de tempo de r?p?tition ?
      if (ntouche < ger_clavier_nb && tempo_repeat == 0 ) {
        touche_repeat = vtouche;
        tempo_repeat = ger_clavier_repetition; // vitesse repetition
      }
    }
  }
  return;
}
// ************************************************************************************************

