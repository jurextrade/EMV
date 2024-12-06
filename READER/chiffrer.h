/* ############################################
    EI96 v03.00

    Diffusion Restreinte

    GIE SESAM VITALE - Tous droits réservés
   ############################################ */

/* -----------------------------------------------------------------------------------------
   EI96 - CHIFFRER.H

   DEFINITION DES CONSTANTES DU MODULE DE CHIFFREMENT 3DES


    ---------------------------------------------------------------------------------------
        Date    | Version |   Auteur    |           Objet
    ---------------------------------------------------------------------------------------
      30/05/02  |   1.0   | LHU         | Création

    --------------------------------------------------------------------------------------- */
#ifndef _CHIFFRER_H
#define _CHIFFRER_H
extern const unsigned char weak_keys[16][8];
extern const unsigned long des_skb[8][64];
extern const unsigned char odd_parity[256];

#define ECB      0x01
#define CBC      0x02
#define ECB_TDES 0x03

typedef struct des_ks_struct
{
	union
	{
		unsigned char cblock[8];
		/* make sure things are correct size on machines with
		 * 8 byte longs */
		unsigned long deslong[2];
	} ks;
	int weak_key;
} des_key_schedule[16];

typedef struct
{
int 			des_check_key;
des_key_schedule 	ks1, ks2, ks3;
unsigned char 		S_ucIV[8];

}TDES_WORK_DATA;

short CHIF_TripleDES8 (unsigned char * DataIN,
                       unsigned char * DataOUT,
                       unsigned char * KeyL,
                       unsigned char * KeyR,
                       short Mode);

short CHIF_DES8 (unsigned char *DataIN, unsigned char *DataOUT, unsigned char *CryptKey, short Mode);

unsigned char* CHIF_MAC8 (unsigned char * BufferToMac,
                          unsigned int    LgBuffer,
                          unsigned char * Key,
                          unsigned char * InitVector);

#endif
