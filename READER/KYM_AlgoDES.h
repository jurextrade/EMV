/****************************************************************************/
/*      DASSAULT Automatismes et Telecommunications							*/
/*--------------------------------------------------------------------------*/
/*      FICHIER  LK0DES.H	                                                */
/****************************************************************************/
/*      GENERALITES															*/
/*      ===========															*/
/*      Date de creation :      02/07/01     			                    */
/*      Derniere modification :					                            */
/*      Module :  Encryption module									*/
/*      Objet :																*/
/*          Define, proto et structures des fonctions de crypto DES			*/
/*																			*/
/*      Historique du fichier :												*/
/*																			*/
/****************************************************************************/
#ifndef CRYPT_DES
#define CRYPT_DES

/***************************************************************************/
/*     INCLUDES																*/
/***************************************************************************/

//#include <stdlib.h>
//#include <stdio.h>
#include "string.h"

#define ROW(x)			((ST*) x) -> row
#define BIT(x)			((ST*) x) -> bit
#define min(a, b)		(((a) < (b)) ? (a) : (b))
#define max(a, b)		(((a) > (b)) ? (a) : (b))


typedef struct
{
    char   row;
    char   bit[2][64];
} ST;

/* Fonctions de cryptage DES */

void vKEY_DES_Encrypt(unsigned char* pucDestination, unsigned char* pucSource, unsigned long ulSize, unsigned char* pucKeyCode);
void vKEY_DES_Decrypt(unsigned char* pucDestination, unsigned char* pucSource, unsigned long ulSize, unsigned char* pucKeyCode);
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
						unsigned char* pucLeftKey, unsigned char* pucRightKey);

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
							unsigned char* pucLeftKey, unsigned char* pucRightKey);

#endif	/*CRYPT_DES */
