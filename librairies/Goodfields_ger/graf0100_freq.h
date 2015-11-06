//--------------------------------********************--------------------------------
// FREQUENCEMETRE INDEPENDANT

  switch (grafcet_frequencemetre) {
// si grafcet_a n'a pas une valeur correcte, on passe ici
//  case 9999: 
  default: 
    grafcet_frequencemetre = 0; // on arrête
//  break;
// attente
  case 1:
  break;

// init
 case 2:
  if (gatetime < 2 || gatetime > 1000) gatetime = 100;	// multiple de 2, maxi 1000
//  gatetime = (gatetime / 2 ) *2;	// multiple de 2
//  gatetime = gatetime && 0x03FE;	// multiple de 2 et < (3*256+254 = 1022)
  gatetime = (gatetime >> 1 ) << 1;	// multiple de 2
  FreqCounter::start(gatetime);  // Gate Time durée du comptage, 1er lancement
  grafcet_frequencemetre++;
  break;

// attente fin comptage non bloquant
 case 3:
  if (FreqCounter::f_ready == 1) {
    freq = (float) FreqCounter::f_freq*1000./gatetime;		// lecture frÃ©quence

//    LC = 1./(pow(deuxpi * freq, 2.));
//    L = LC/C   C=LC/L	
// ajustement de la durÃ©e d'Ã©chantillonage en fonction de la frÃ©quence
// - on a besoin de 6 Ã  7 chiffres maxi (prÃ©cision des float)
// - la frÃ©quence maxi admissible est 8.000.000 Hz
// - on affiche finalement que 4 chiffres pour les C et L
// - l'oscillateur (CLF_meter) tourne entre 16.000Hz(1uF) et 750.000Hz(0pF)
// - donc une précison de 10 Hz est très suffisante
//    gatetime=10;    // précision 100 Hz
//    gatetime=100;	// précision 10 Hz
//    gatetime=1000;  // précision 1 Hz
//    if (freq > 99999) gatetime=100;
//    if (freq > 999999) gatetime=1000;
//    if (freq < 10000) gatetime=100;
//    if (freq < 1000) gatetime=1000;
//    FreqCounter::start(gatetime);  // Gate Time durÃ©e du comptage : multiple de 2, maxi 1000
    grafcet_frequencemetre=1;
  }
// il serait bien d'avoir une tempo de sécurité ...
  break;

  }      // fin du "switch"
//--------------------------------********************--------------------------------

