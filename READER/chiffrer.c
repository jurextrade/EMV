/* ############################################
    EI96 v03.00

    Diffusion Restreinte

    GIE SESAM VITALE - Tous droits réservés
   ############################################ */

/* -----------------------------------------------------------------------
    EI96 - CHIFFRER.C

    Gestion de l'ordre de lecture des droits de la carte Vitale

    -------------------- Liste des fonctions privées ----------------------

    ---------------------Liste des fonctions publiques --------------------
    CHIF_3DES
    --------------------------- Fin de liste  -----------------------------
    -----------------------------------------------------------------------
          Date    | Version |   Auteur   |           Objet
    -----------------------------------------------------------------------
        30/05/02  |   1.00  | LH         | Création
    ----------------------------------------------------------------------- */
/* ------------
    Bibliotheque
    ------------ */

#include "chiffrer.h"
#include "des.h"
#include "OS.h"
#include "utils.h"



#include <string.h>

const unsigned char weak_keys[16][8]={
	/* weak keys */
	{0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01},
	{0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE},
	{0x1F,0x1F,0x1F,0x1F,0x0E,0x0E,0x0E,0x0E},
	{0xE0,0xE0,0xE0,0xE0,0xF1,0xF1,0xF1,0xF1},
	/* semi-weak keys */
	{0x01,0xFE,0x01,0xFE,0x01,0xFE,0x01,0xFE},
	{0xFE,0x01,0xFE,0x01,0xFE,0x01,0xFE,0x01},
	{0x1F,0xE0,0x1F,0xE0,0x0E,0xF1,0x0E,0xF1},
	{0xE0,0x1F,0xE0,0x1F,0xF1,0x0E,0xF1,0x0E},
	{0x01,0xE0,0x01,0xE0,0x01,0xF1,0x01,0xF1},
	{0xE0,0x01,0xE0,0x01,0xF1,0x01,0xF1,0x01},
	{0x1F,0xFE,0x1F,0xFE,0x0E,0xFE,0x0E,0xFE},
	{0xFE,0x1F,0xFE,0x1F,0xFE,0x0E,0xFE,0x0E},
	{0x01,0x1F,0x01,0x1F,0x01,0x0E,0x01,0x0E},
	{0x1F,0x01,0x1F,0x01,0x0E,0x01,0x0E,0x01},
	{0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1,0xFE},
	{0xFE,0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1}};

const DES_LONG des_skb[8][64]={
	{
	/* for C bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
	0x00000000L,0x00000010L,0x20000000L,0x20000010L,
	0x00010000L,0x00010010L,0x20010000L,0x20010010L,
	0x00000800L,0x00000810L,0x20000800L,0x20000810L,
	0x00010800L,0x00010810L,0x20010800L,0x20010810L,
	0x00000020L,0x00000030L,0x20000020L,0x20000030L,
	0x00010020L,0x00010030L,0x20010020L,0x20010030L,
	0x00000820L,0x00000830L,0x20000820L,0x20000830L,
	0x00010820L,0x00010830L,0x20010820L,0x20010830L,
	0x00080000L,0x00080010L,0x20080000L,0x20080010L,
	0x00090000L,0x00090010L,0x20090000L,0x20090010L,
	0x00080800L,0x00080810L,0x20080800L,0x20080810L,
	0x00090800L,0x00090810L,0x20090800L,0x20090810L,
	0x00080020L,0x00080030L,0x20080020L,0x20080030L,
	0x00090020L,0x00090030L,0x20090020L,0x20090030L,
	0x00080820L,0x00080830L,0x20080820L,0x20080830L,
	0x00090820L,0x00090830L,0x20090820L,0x20090830L,
	},{
	/* for C bits (numbered as per FIPS 46) 7 8 10 11 12 13 */
	0x00000000L,0x02000000L,0x00002000L,0x02002000L,
	0x00200000L,0x02200000L,0x00202000L,0x02202000L,
	0x00000004L,0x02000004L,0x00002004L,0x02002004L,
	0x00200004L,0x02200004L,0x00202004L,0x02202004L,
	0x00000400L,0x02000400L,0x00002400L,0x02002400L,
	0x00200400L,0x02200400L,0x00202400L,0x02202400L,
	0x00000404L,0x02000404L,0x00002404L,0x02002404L,
	0x00200404L,0x02200404L,0x00202404L,0x02202404L,
	0x10000000L,0x12000000L,0x10002000L,0x12002000L,
	0x10200000L,0x12200000L,0x10202000L,0x12202000L,
	0x10000004L,0x12000004L,0x10002004L,0x12002004L,
	0x10200004L,0x12200004L,0x10202004L,0x12202004L,
	0x10000400L,0x12000400L,0x10002400L,0x12002400L,
	0x10200400L,0x12200400L,0x10202400L,0x12202400L,
	0x10000404L,0x12000404L,0x10002404L,0x12002404L,
	0x10200404L,0x12200404L,0x10202404L,0x12202404L,
	},{
	/* for C bits (numbered as per FIPS 46) 14 15 16 17 19 20 */
	0x00000000L,0x00000001L,0x00040000L,0x00040001L,
	0x01000000L,0x01000001L,0x01040000L,0x01040001L,
	0x00000002L,0x00000003L,0x00040002L,0x00040003L,
	0x01000002L,0x01000003L,0x01040002L,0x01040003L,
	0x00000200L,0x00000201L,0x00040200L,0x00040201L,
	0x01000200L,0x01000201L,0x01040200L,0x01040201L,
	0x00000202L,0x00000203L,0x00040202L,0x00040203L,
	0x01000202L,0x01000203L,0x01040202L,0x01040203L,
	0x08000000L,0x08000001L,0x08040000L,0x08040001L,
	0x09000000L,0x09000001L,0x09040000L,0x09040001L,
	0x08000002L,0x08000003L,0x08040002L,0x08040003L,
	0x09000002L,0x09000003L,0x09040002L,0x09040003L,
	0x08000200L,0x08000201L,0x08040200L,0x08040201L,
	0x09000200L,0x09000201L,0x09040200L,0x09040201L,
	0x08000202L,0x08000203L,0x08040202L,0x08040203L,
	0x09000202L,0x09000203L,0x09040202L,0x09040203L,
	},{
	/* for C bits (numbered as per FIPS 46) 21 23 24 26 27 28 */
	0x00000000L,0x00100000L,0x00000100L,0x00100100L,
	0x00000008L,0x00100008L,0x00000108L,0x00100108L,
	0x00001000L,0x00101000L,0x00001100L,0x00101100L,
	0x00001008L,0x00101008L,0x00001108L,0x00101108L,
	0x04000000L,0x04100000L,0x04000100L,0x04100100L,
	0x04000008L,0x04100008L,0x04000108L,0x04100108L,
	0x04001000L,0x04101000L,0x04001100L,0x04101100L,
	0x04001008L,0x04101008L,0x04001108L,0x04101108L,
	0x00020000L,0x00120000L,0x00020100L,0x00120100L,
	0x00020008L,0x00120008L,0x00020108L,0x00120108L,
	0x00021000L,0x00121000L,0x00021100L,0x00121100L,
	0x00021008L,0x00121008L,0x00021108L,0x00121108L,
	0x04020000L,0x04120000L,0x04020100L,0x04120100L,
	0x04020008L,0x04120008L,0x04020108L,0x04120108L,
	0x04021000L,0x04121000L,0x04021100L,0x04121100L,
	0x04021008L,0x04121008L,0x04021108L,0x04121108L,
	},{
	/* for D bits (numbered as per FIPS 46) 1 2 3 4 5 6 */
	0x00000000L,0x10000000L,0x00010000L,0x10010000L,
	0x00000004L,0x10000004L,0x00010004L,0x10010004L,
	0x20000000L,0x30000000L,0x20010000L,0x30010000L,
	0x20000004L,0x30000004L,0x20010004L,0x30010004L,
	0x00100000L,0x10100000L,0x00110000L,0x10110000L,
	0x00100004L,0x10100004L,0x00110004L,0x10110004L,
	0x20100000L,0x30100000L,0x20110000L,0x30110000L,
	0x20100004L,0x30100004L,0x20110004L,0x30110004L,
	0x00001000L,0x10001000L,0x00011000L,0x10011000L,
	0x00001004L,0x10001004L,0x00011004L,0x10011004L,
	0x20001000L,0x30001000L,0x20011000L,0x30011000L,
	0x20001004L,0x30001004L,0x20011004L,0x30011004L,
	0x00101000L,0x10101000L,0x00111000L,0x10111000L,
	0x00101004L,0x10101004L,0x00111004L,0x10111004L,
	0x20101000L,0x30101000L,0x20111000L,0x30111000L,
	0x20101004L,0x30101004L,0x20111004L,0x30111004L,
	},{
	/* for D bits (numbered as per FIPS 46) 8 9 11 12 13 14 */
	0x00000000L,0x08000000L,0x00000008L,0x08000008L,
	0x00000400L,0x08000400L,0x00000408L,0x08000408L,
	0x00020000L,0x08020000L,0x00020008L,0x08020008L,
	0x00020400L,0x08020400L,0x00020408L,0x08020408L,
	0x00000001L,0x08000001L,0x00000009L,0x08000009L,
	0x00000401L,0x08000401L,0x00000409L,0x08000409L,
	0x00020001L,0x08020001L,0x00020009L,0x08020009L,
	0x00020401L,0x08020401L,0x00020409L,0x08020409L,
	0x02000000L,0x0A000000L,0x02000008L,0x0A000008L,
	0x02000400L,0x0A000400L,0x02000408L,0x0A000408L,
	0x02020000L,0x0A020000L,0x02020008L,0x0A020008L,
	0x02020400L,0x0A020400L,0x02020408L,0x0A020408L,
	0x02000001L,0x0A000001L,0x02000009L,0x0A000009L,
	0x02000401L,0x0A000401L,0x02000409L,0x0A000409L,
	0x02020001L,0x0A020001L,0x02020009L,0x0A020009L,
	0x02020401L,0x0A020401L,0x02020409L,0x0A020409L,
	},{
	/* for D bits (numbered as per FIPS 46) 16 17 18 19 20 21 */
	0x00000000L,0x00000100L,0x00080000L,0x00080100L,
	0x01000000L,0x01000100L,0x01080000L,0x01080100L,
	0x00000010L,0x00000110L,0x00080010L,0x00080110L,
	0x01000010L,0x01000110L,0x01080010L,0x01080110L,
	0x00200000L,0x00200100L,0x00280000L,0x00280100L,
	0x01200000L,0x01200100L,0x01280000L,0x01280100L,
	0x00200010L,0x00200110L,0x00280010L,0x00280110L,
	0x01200010L,0x01200110L,0x01280010L,0x01280110L,
	0x00000200L,0x00000300L,0x00080200L,0x00080300L,
	0x01000200L,0x01000300L,0x01080200L,0x01080300L,
	0x00000210L,0x00000310L,0x00080210L,0x00080310L,
	0x01000210L,0x01000310L,0x01080210L,0x01080310L,
	0x00200200L,0x00200300L,0x00280200L,0x00280300L,
	0x01200200L,0x01200300L,0x01280200L,0x01280300L,
	0x00200210L,0x00200310L,0x00280210L,0x00280310L,
	0x01200210L,0x01200310L,0x01280210L,0x01280310L,
	},{
	/* for D bits (numbered as per FIPS 46) 22 23 24 25 27 28 */
	0x00000000L,0x04000000L,0x00040000L,0x04040000L,
	0x00000002L,0x04000002L,0x00040002L,0x04040002L,
	0x00002000L,0x04002000L,0x00042000L,0x04042000L,
	0x00002002L,0x04002002L,0x00042002L,0x04042002L,
	0x00000020L,0x04000020L,0x00040020L,0x04040020L,
	0x00000022L,0x04000022L,0x00040022L,0x04040022L,
	0x00002020L,0x04002020L,0x00042020L,0x04042020L,
	0x00002022L,0x04002022L,0x00042022L,0x04042022L,
	0x00000800L,0x04000800L,0x00040800L,0x04040800L,
	0x00000802L,0x04000802L,0x00040802L,0x04040802L,
	0x00002800L,0x04002800L,0x00042800L,0x04042800L,
	0x00002802L,0x04002802L,0x00042802L,0x04042802L,
	0x00000820L,0x04000820L,0x00040820L,0x04040820L,
	0x00000822L,0x04000822L,0x00040822L,0x04040822L,
	0x00002820L,0x04002820L,0x00042820L,0x04042820L,
	0x00002822L,0x04002822L,0x00042822L,0x04042822L,
	}};

const unsigned char odd_parity[256]={
  1,  1,  2,  2,  4,  4,  7,  7,  8,  8, 11, 11, 13, 13, 14, 14,
 16, 16, 19, 19, 21, 21, 22, 22, 25, 25, 26, 26, 28, 28, 31, 31,
 32, 32, 35, 35, 37, 37, 38, 38, 41, 41, 42, 42, 44, 44, 47, 47,
 49, 49, 50, 50, 52, 52, 55, 55, 56, 56, 59, 59, 61, 61, 62, 62,
 64, 64, 67, 67, 69, 69, 70, 70, 73, 73, 74, 74, 76, 76, 79, 79,
 81, 81, 82, 82, 84, 84, 87, 87, 88, 88, 91, 91, 93, 93, 94, 94,
 97, 97, 98, 98,100,100,103,103,104,104,107,107,109,109,110,110,
112,112,115,115,117,117,118,118,121,121,122,122,124,124,127,127,
128,128,131,131,133,133,134,134,137,137,138,138,140,140,143,143,
145,145,146,146,148,148,151,151,152,152,155,155,157,157,158,158,
161,161,162,162,164,164,167,167,168,168,171,171,173,173,174,174,
176,176,179,179,181,181,182,182,185,185,186,186,188,188,191,191,
193,193,194,194,196,196,199,199,200,200,203,203,205,205,206,206,
208,208,211,211,213,213,214,214,217,217,218,218,220,220,223,223,
224,224,227,227,229,229,230,230,233,233,234,234,236,236,239,239,
241,241,242,242,244,244,247,247,248,248,251,251,253,253,254,254};

/* ------------------------------------------------------------------------------
     CHIF_SETKKEY():  Fonction de mise à la clé et initilisation du vecteur IVS

     Paramètres :
     IN  :
     IN  : ucBuffer             = Buffer à chiffrer
     IN  : usLgBuffer           = Longueur du buffer à chiffrer




     Code de retour :    néant
    ----------------------------------------------------------------------------- */
void CHIF_SETKKEY(  unsigned char *pucCle1,unsigned char *pucCle2,TDES_WORK_DATA * p_WorkData)
{
    /* mise à la clé */
    des_set_key(pucCle1,p_WorkData->ks1,p_WorkData);
	if(pucCle2!=NULL) {
      des_set_key(pucCle2,p_WorkData->ks2,p_WorkData);
	}

    /* initialisation du vecteurs IV */
    memset(p_WorkData->S_ucIV,0,8);

}

/*******************************************************************************
********************************************************************************
** FONCTION: CHIF_DES8 --> Permet le chiffrement DES
**
** APPEL   : DataIN               -> Pointeur sur le buffer d'entrée (8 octets)
**           DataOUT              -> Pointeur sur le buffer de sortie(8 octets)
**                                   !!! Il doit être alloué lui aussi !!!
**           Key                  -> Pointeur sur la clé de chiffrement(8 octets)
**           Mode                 -> Mode de chiffrement
**                                   (0 (ENC0) chiffrement, 1 (DEC1) déchiffrement)
**
** RETOUR  : 0 réussi, 1 échoué
********************************************************************************
*******************************************************************************/
short CHIF_DES8 (unsigned char *DataIN, unsigned char *DataOUT, unsigned char *CryptKey, short Mode)
{
TDES_WORK_DATA * p_WorkData;

	p_WorkData = (TDES_WORK_DATA*) OS_Malloc(sizeof(TDES_WORK_DATA));
   if( p_WorkData == NULL )
      return(1);

   p_WorkData->des_check_key=0;

	CHIF_SETKKEY(CryptKey,NULL,p_WorkData);

	des_ecb_encrypt(DataIN,DataOUT,p_WorkData->ks1,Mode);

	OS_Free(p_WorkData);

	return(0);
}

/*******************************************************************************
********************************************************************************
** FONCTION: CHIF_TripleDES8 --> Permet le chiffrement Triple DES
**
** APPEL   : DataIN               -> Pointeur sur le buffer d'entrée (8 octets)
**           DataOUT              -> Pointeur sur le buffer de sortie(8 octets)
**                                   !!! Il doit être alloué lui aussi !!!
**           KeyL                 -> Pointeur sur la clé gauche
**                                   (ou partie gauche d'une clé sur 16 octet)
**           KeyR                 -> Pointeur sur la clé droite
**                                   (ou partie droite d'une clé sur 16 octet)
**           Mode                 -> Mode de chiffrement
**                                   (0 (ENC0) chiffrement, 1 (DEC1) déchiffrement)
** RETOUR  : 0 réussi, 1 échoué
********************************************************************************
*******************************************************************************/
short CHIF_TripleDES8 (unsigned char * DataIN,
                       unsigned char * DataOUT,
                       unsigned char * KeyL,
                       unsigned char * KeyR,
                       short Mode)
{
TDES_WORK_DATA * p_WorkData;

	p_WorkData = (TDES_WORK_DATA*) OS_Malloc(sizeof(TDES_WORK_DATA));
   if( p_WorkData == NULL )
      return(1);

   p_WorkData->des_check_key=0;

	CHIF_SETKKEY(KeyL,KeyR,p_WorkData);

	des_ecb2_encrypt(DataIN, DataOUT,p_WorkData->ks1,p_WorkData->ks2,Mode);

	OS_Free(p_WorkData);

   return(0);
}

/*******************************************************************************
********************************************************************************
** FONCTION: CHIF_MAC8 --> Calcul le MAC (signature) d'un buffer
**
** APPEL   : BufferToMac           -> Pointeur sur le buffer d'entrée
**           LgBuffer              -> Longueur du buffer d'entrée
**           Key                   -> Pointeur sur la clé (8 octets)
**           InitVector            -> Vecteur d'initialisation du MAC
**
** RETOUR  : Le MAC sur 8 octets ou NULL si échoué
********************************************************************************
*******************************************************************************/
unsigned char* CHIF_MAC8 (unsigned char * BufferToMac,
                          unsigned int    LgBuffer,
                          unsigned char * Key,
                          unsigned char * InitVector)
{
int NbBlock;                 /* Nombre de blocs de 8 octets                 */
int Padding;                 /* Padding pour avoir un multiple de 8 octets  */
int i,j;
unsigned char *DataIN;       /* Pointeur temporaire de calcul               */
unsigned char *DataOUT;      /* Pointeur temporaire de calcul               */
unsigned char *pL_NoWarning;
TDES_WORK_DATA * p_WorkData;

  p_WorkData = (TDES_WORK_DATA*) OS_Malloc(sizeof(TDES_WORK_DATA));
  if( p_WorkData == NULL )
     return(NULL);

   p_WorkData->des_check_key=0;

  CHIF_SETKKEY(Key,NULL,p_WorkData);

  /* On calcul le nombre de bloc à chiffrer pour effectuer le MAC */
  NbBlock = LgBuffer / 8;

  /* On calcul le padding à effectuer sur le buffer */
  Padding=8-LgBuffer % 8;

  DataOUT = (unsigned char *) OS_Malloc(8);

  if (DataOUT == NULL)
  {
    OS_Free(p_WorkData);
    return(NULL);
  }

  DataIN = (unsigned char *) OS_Malloc(8);

  if (DataIN == NULL)
  {
    OS_Free(p_WorkData);
    OS_Free(DataOUT);
    return(NULL);
  }

  /* Initialisation de la premiere boucle de chiffrement */
  pL_NoWarning = memcpy(DataOUT,InitVector,8);

  /* Boucle de chiffrement CBC pour le calcul du MAC */
  for (i=0;i<NbBlock;i++)
  {
    /* On copie les données du buffer par bloc de 8 pour le chiffrement DES   */
    UTS_memcpy(DataIN,&BufferToMac[i*8],8);

    /* On effectue le XOR entre le bloc en cours et le résultat du chiffrement
       précédent */
    for (j=0;j<8;j++)
    {
      DataIN[j] ^= DataOUT[j];
    }
    /* Chiffrement du bloc resultant avec la clé gauche (de 8 octets) */
    des_ecb_encrypt(DataIN,DataOUT,p_WorkData->ks1,DES_ENCRYPT);
  }
  /* Il reste une boucle si le buffer un padding est requis
     (!! attenttion si multiple de 8, on ajoute 8 zéros ???)*/
  if (Padding !=0)
  {
    /* On remplie de 0 */
    pL_NoWarning = memset(DataIN,0x00,8);
    /* On copie le reste du buffer */
    UTS_memcpy(DataIN,&BufferToMac[LgBuffer - LgBuffer%8 ], LgBuffer%8);
    /* On execute le dernier Xor avec le dernier résultat obtenue */
    for (j=0;j<8;j++)
    {
      DataIN[j] ^= DataOUT[j];
    }
    des_ecb_encrypt(DataIN,DataOUT,p_WorkData->ks1,DES_ENCRYPT);
  }
  UTS_Free(&DataIN);
  OS_Free(p_WorkData);

  return (DataOUT);

}
