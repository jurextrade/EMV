/*
***************************************************************************
*      Thales e-transactions
*--------------------------------------------------------------------------
*      FICHIER  DESFUNCTION.C
****************************************************************************
*      GENERALITES
*      ===========
*      Date de creation :      02/07/01
*      Module :  Function DES
*      Objet :
*          fonctions de crypto DES
*
*      Historique du fichier :
*
****************************************************************************
*/

/***************************************************************************/
/*     INCLUDES																*/
/***************************************************************************/
//#include <stdio.h>
#include <string.h>
//#include <stdlib.h>

//#include "ritypes.h"

#include "KYM_AlgoDES.H"
#include "Total.h"
#include "utils.h"

/*
 * DES algorithm
 */

void SP_DES_Des		(int type, char* key, char* tinp, char* tout, int tlen);
void SP_DES_DesTransp(ST* p, unsigned char* rom tab, int n);
void SP_DES_Desf		(int type, int n, ST* k, ST* t);
void SP_DES_DesRotdec(ST* k);
void SP_DES_DesRotenc(ST* k);

const char initTr[] =
{
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};

const char finalTr[] =
{
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25
};

const char swap[] =
{
    33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32
};

const char keyTr1[] =
{
    57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4
};

const char keyTr2[] =
{
    14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

const char etr[] =
{
    32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
};

const char ptr[] =
{
    16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10,
    2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25
};

const char s[8][64] =
{

    {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
    0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
    4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
    15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},

    {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
    3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
    0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
    13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},

    {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
    13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
    13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
    1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},

    {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
    13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
    10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
    3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},

    {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
    14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
    4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
    11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},

    {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
    10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
    9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
    4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},

    {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
    13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
    1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
    6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},

    {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
    1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
    7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
    2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
};

const char rots[] =
{
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2 , 1
};

/*
 * DES algorithm
 */
/* 0/1 , clef     , source    , dest      , Taille */
void SP_DES_Des(int type, char* key, char* tinp, char* tout, int tlen)
{
    int	i,x, l;
    //ST	*kbit;
    //ST	*tbit;
    ST*	k ;//= &kbit;
    ST*	t ;//= &tbit;

    k = (ST*) OS_Malloc(sizeof(ST));
    t = (ST*) OS_Malloc(sizeof(ST));

    tlen &= ~7;
    ROW (k) = 0;
    ROW (t) = 0;

    for (i = 0; i < 64; i++)
    {
		BIT (k)[0][i] = (key[i >> 3] >> (7 - (i & 7))) & 1;
    }

    SP_DES_DesTransp (k, (unsigned char*)keyTr1, 56);           /* initial key transposition */

    for (l= 0; l < tlen; l += 8, tinp += 8, tout += 8)
    {
		for (i = 0; i < 64; i++)
		{
			BIT (t)[ROW (t)][i] = (tinp[i >> 3] >> (7 - (i & 7))) & 1;
		}

		SP_DES_DesTransp (t, (unsigned char*)initTr, 64);    /* initial text transposition */

		for (i = 0; i < 16; i++)       /* here come the 16 iterations */
		{
    		SP_DES_DesTransp (t, (unsigned char*)swap, 64);  /* swap left right halves */
			SP_DES_Desf (type, i, k, t);     /* compute f(r[i-1],k[i]) */
			ROW (t) ^= 1;

			for (x = 32; x < 64; x++)
				BIT (t)[ROW (t)][x] ^= BIT (t)[ROW (t) ^ 1][x];
		}

		SP_DES_DesTransp (t, (unsigned char*)swap, 64);      /* swap left right halves */
		SP_DES_DesTransp (t, (unsigned char*)finalTr, 64);   /* final transposition */

		for (i = 0; i < 8; i++)
			for (tout[i] = 0, x = 0; x < 8; x++)
				tout[i] |= BIT (t)[ROW (t)][(i << 3) + x] << (7 - x);
    }

    OS_Free(k);
    OS_Free(t);
}


void SP_DES_DesTransp (ST* p, unsigned char * rom tab, int n)
{
unsigned char uc_Tampon;
    while (--n >= 0)
    {
	uc_Tampon=tab[n];
		BIT (p)[ROW (p) ^ 1][n] = BIT (p)[ROW (p)][uc_Tampon - 1];
    }
    ROW (p) ^= 1;
}


void SP_DES_DesRotenc (ST* k)
{
    int    i;

    for (i = 0; i < 55; i++)
		BIT (k)[ROW (k) ^ 1][i] = BIT (k)[ROW (k)][i + 1];

    BIT (k)[ROW (k) ^ 1][27] = BIT (k)[ROW (k)][0];
    BIT (k)[ROW (k) ^ 1][55] = BIT (k)[ROW (k)][28];
    ROW (k) ^= 1;
}


void SP_DES_DesRotdec(ST* k)
{
    int    i;

    for (i = 1; i < 56; i++)
		BIT (k)[ROW (k) ^ 1][i] = BIT (k)[ROW (k)][i - 1];

    BIT (k)[ROW (k) ^ 1][0] = BIT (k)[ROW (k)][27];
    BIT (k)[ROW (k) ^ 1][28] = BIT (k)[ROW (k)][55];
    ROW (k) ^= 1;
}


void SP_DES_Desf(int type, int n, ST *k, ST* t)			/* f(r[i-1], k[i]) */
{
    int    i, x;

    SP_DES_DesTransp (t, (unsigned char*)etr, 48);         /* expand data block to 48 bits */

    if (!type)
    {
     for (i=rots[n]; i > 0; i--)
         SP_DES_DesRotenc (k);
    }

    SP_DES_DesTransp (k, (unsigned char*)keyTr2, 48);

    for (i = 0; i < 48; i++)
		BIT (t)[ROW (t)][i] ^= BIT (k)[ROW (k)][i];

    ROW (k) ^= 1;

    if (type)
    {
		for (i = rots[15-n]; i > 0; i--)
			SP_DES_DesRotdec (k);
    }

    for (i = 0; i < 8; i++)
    {
		x = BIT (t)[ROW (t)][6 * i] << 5;
		x |= BIT (t)[ROW (t)][6 * i + 5] << 4;

		for (n = 1; n < 5; n++)
			x |= BIT (t)[ROW (t)][6 * i + n] << (4 - n);

		for (n = 0; n < 4; n++)
			BIT (t)[ROW (t)][(i << 2) + n] = (s[i][x] >> (3 - n)) & 1;
    }

    for (i = 0; i < 32; i++)
		BIT (t)[ROW (t)][i + 32] = BIT (t)[ROW (t)][ptr[i] - 1];
}

/*------------------------------------------------------------------------
PURPOSE           : DES encoding
INPUT PARAMETERS  : Buffer to crypt
					ulSize of the buffer
					Key Number (0 -> 7)
					Part of the Key (PART_RIGHT_KEY or PART_LEFT_KEY)
OUTPUT PARAMETERS : Buffer crypted
RETURN VALUE      : (none)
------------------------------------------------------------------------*/
void vKEY_DES_Encrypt(unsigned char* pucDestination, unsigned char* pucSource, unsigned long ulSize, unsigned char* pucKeyCode)
{
	unsigned long ulDefaut, ulReste;

	ulReste  = ulSize % 8;
	ulDefaut = ulSize - ulReste;

	SP_DES_Des(0, (char*)pucKeyCode, (char*)pucSource, (char*)pucDestination, (int)ulDefaut);

	if (ulReste > 0)
		{
		UTS_memcpy(pucDestination+(unsigned char)ulDefaut, pucSource+(unsigned char)ulDefaut, (unsigned short)ulReste);
		}
}


/*------------------------------------------------------------------------
PURPOSE           : DES decoding
INPUT PARAMETERS  : Buffer to decrypt
					ulSize of the buffer
OUTPUT PARAMETERS : Buffer decrypted
RETURN VALUE      : (none)
------------------------------------------------------------------------*/
void vKEY_DES_Decrypt(unsigned char* pucDestination, unsigned char* pucSource, unsigned long ulSize, unsigned char* pucKeyCode)
{
	unsigned long ulDefaut, ulReste;

	ulReste  = ulSize % 8;
	ulDefaut = ulSize - ulReste;

	SP_DES_Des(1, (char*)pucKeyCode, (char*)pucSource, (char*)pucDestination, (int)ulDefaut);

	if (ulReste > 0)
		{
		UTS_memcpy(pucDestination+(unsigned char)ulDefaut, pucSource+(unsigned char)ulDefaut, (unsigned short)ulReste);
		}
}

/****************************************************************************/
/*     FONCTION vCrypter3DES												*/
/*     =====================			                                    */
/*     Cette fonction crypte des données à l'aide d'un triple DES			*/
/*		 avec une clé de 128 bits, soit 2 clés de 64 bits					*/
/*                                                                          */
/*  -*- ATTENTION !!!! Cette fonction modifie les données sources !!!! -*-  */
/*                                                                          */
/*     Interface d'appel:                                                   */
/*       Les parametres de la fonction sont les suivants:                   */
/*       (I-) pucSource		: Buffer des données à crypter					*/
/*            ulSize		: Longueur des données à crypter (multiple de 8)*/
/*            pucLeftKey	: Partie gauche de la clé de 128 bits			*/
/*            pucRightKey	: Partie driote de la clé de 128 bits			*/
/*                                                                          */
/*       (-O) pucDestination: Buffer des données cryptées					*/
/*                                                                          */
/*     Compte-rendu:                                                        */
/*       Cette fonction ne donne pas de compte rendu                        */
/*                                                                          */
/****************************************************************************/
void vCrypter3DES(unsigned char* pucDestination, unsigned char* pucSource, unsigned long ulSize,
						unsigned char* pucLeftKey, unsigned char* pucRightKey)
{
	vKEY_DES_Encrypt(pucDestination, pucSource, ulSize, pucLeftKey);

	vKEY_DES_Decrypt(pucSource, pucDestination, ulSize, pucRightKey);

	vKEY_DES_Encrypt(pucDestination, pucSource, ulSize, pucLeftKey);
}

/********************************************************************************/
/*     FONCTION vDeCrypter3DES													*/
/*     =====================													*/
/*     Cette fonction décrypte des données à l'aide d'un triple DES				*/
/*		 avec une clé de 128 bits, soit 2 clés de 64 bits						*/
/*																				*/
/*  -*- ATTENTION !!!! Cette fonction modifie les données sources !!!! -*-		*/
/*																				*/
/*     Interface d'appel:														*/
/*       Les parametres de la fonction sont les suivants:						*/
/*       (I-) pucSource		: Buffer des données à décrypter					*/
/*            ulSize		: Longueur des données à décrypter(multiple de 8)	*/
/*            pucLeftKey	: Partie gauche de la clé de 128 bits				*/
/*            pucRightKey	: Partie driote de la clé de 128 bits				*/
/*																				*/
/*       (-O) pucDestination: Buffer des données décryptées						*/
/*																				*/
/*     Compte-rendu:															*/
/*       Cette fonction ne donne pas de compte rendu							*/
/*																				*/
/********************************************************************************/
void vDeCrypter3DES(unsigned char* pucDestination, unsigned char* pucSource, unsigned long ulSize,
							unsigned char* pucLeftKey, unsigned char* pucRightKey)
{

	vKEY_DES_Decrypt(pucDestination, pucSource, ulSize, pucLeftKey);

	vKEY_DES_Encrypt(pucSource, pucDestination, ulSize, pucRightKey);

	vKEY_DES_Decrypt(pucDestination, pucSource, ulSize, pucLeftKey);
}

/* Fin des fonctions DES */
