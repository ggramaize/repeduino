
// constantes type d'affichage LCD
#define lcd_ELCDXXX 1	// lcd série 19200 bds (Ex PIC BASIC)
#define lcd_serie 2		// lcd sur port série
//
#define lcd_none 3		// donc émission sur le port série (pour tests)
#define lcd_direct 4	// lcd câblé sur l'ARDUINO (avec 6 lignes)
#define lcd_7SEG 5		// lcd 7 segments géants (lcd jumbo)
#define lcd_I2C 6		// lcd sur port I2C
#define lcd_mult 7		// lcd 2lignes de 16 multiples

// constantes selon type de carte ETHERNET
#define interface_ethernet_W5100 1		// cartes actuelles et standards ARDUINO
#define interface_ethernet_ENC28J60 2	// cartes anciennes et asiatiques ..
// et gestion des IP
#define ip_mode_FIXE 1
#define ip_mode_DHCP 2
