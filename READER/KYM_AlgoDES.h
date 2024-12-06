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
/*     Cette fonction crypte des donn�es � l'aide d'un triple DES			*/
/*		 avec une cl� de 128 bits, soit 2 cl�s de 64 bits					*/
/*                                                                          */
/*  -*- ATTENTION !!!! Cette fonction modifie les donn�es sources !!!! -*-  */
/*                                                                          */
/*     Interface d'appel:                                                   */
/*       Les parametres de la fonction sont les suivants:                   */
/*       (I-) pucSource		: Buffer des donn�es � crypter					*/
/*            ulSize		: Longueur des donn�es � crypter (multiple de 8)*/
/*            pucLeftKey	: Partie gauche de la cl� de 128 bits			*/
/*            pucRightKey	: Partie driote de la cl� de 128 bits			*/
/*                                                                          */
/*       (-O) pucDestination: Buffer des donn�es crypt�es					*/
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
/*     Cette fonction d�crypte des donn�es � l'aide d'un triple DES				*/
/*		 avec une cl� de 128 bits, soit 2 cl�s de 64 bits						*/
/*																				*/
/*  -*- ATTENTION !!!! Cette fonction modifie les donn�es sources !!!! -*-		*/
/*																				*/
/*     Interface d'appel:														*/
/*       Les parametres de la fonction sont les suivants:						*/
/*       (I-) pucSource		: Buffer des donn�es � d�crypter					*/
/*            ulSize		: Longueur des donn�es � d�crypter(multiple de 8)	*/
/*            pucLeftKey	: Partie gauche de la cl� de 128 bits				*/
/*            pucRightKey	: Partie driote de la cl� de 128 bits				*/
/*																				*/
/*       (-O) pucDestination: Buffer des donn�es d�crypt�es						*/
/*																				*/
/*     Compte-rendu:															*/
/*       Cette fonction ne donne pas de compte rendu							*/
/*																				*/
/********************************************************************************/
void vDeCrypter3DES(unsigned char* pucDestination, unsigned char* pucSource, unsigned long ulSize,
							unsigned char* pucLeftKey, unsigned char* pucRightKey);

#endif	/*CRYPT_DES */
