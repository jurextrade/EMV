/* ***************************************************************************
* Thales-e-Transactions
* ---------------------------------------------------------------------------
* Station Manager
* ---------------------------------------------------------------------------
* Key Manager
* ---------------------------------------------------------------------------
* KYM_DEF.h : Public Definitions of the Key Manager
* ---------------------------------------------------------------------------
* File history:
*  01/04/04 APR : Creation
* *************************************************************************** */


/***************************************************************************/
/*     INCLUDES																*/
/***************************************************************************/
#ifndef READKEY_H
#define READKEY_H

//#include "RITYPES.H"

/************************
*    DEFINITIONS
************************/
/*
**	LIST OF ALGORITHMS
*/
typedef enum
{
	ALGORITHM_DKV=1,
	ALGORITHM_UTA,
	ALGORITHM_GR_KEY1,
	ALGORITHM_GR_KEY2,
	ALGORITHM_DKV_OLD,
}eAlgorithms;


/*
**	LIST OF KEYS
*/
//typedef enum
//{
//	TK=1,
//	KEY1,
//	KEY2,
//	MKscdbl,
//	MKgr,
//	M_DKV,
//	M_UTA
//}eKeys;


/************************
*    PUBLIC INTERFACE
************************/
unsigned char bKEY_PVV_Control(unsigned char ucAlgorithm,unsigned char ucPinType, unsigned char* pucTrack,unsigned char* pucSecretCode,unsigned char* pucDriverCode, unsigned char* pucPVV, unsigned char* pucCrypto, unsigned char* pucRES2);
//extern short sKEY_ReadKey(unsigned char ucNumKey, unsigned char* pucKey, unsigned char* pucCryptogram);
//extern void vKEY_CalculateMAC(unsigned char* pucKey,unsigned char* pucData, unsigned short usDataLength, unsigned char* pucRES1);
//extern void vKEY_DES_Encrypt(unsigned char* pucDestination, unsigned char* pucSource, unsigned long ulSize, unsigned char* pucKeyCode);
//extern void vKEY_DES_Decrypt(unsigned char* pucDestination, unsigned char* pucSource, unsigned long ulSize, unsigned char* pucKeyCode);

#endif	/* READKEY */
