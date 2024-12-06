/****************************************************************************/
/*			THALES e-TRANSACTIONS											*/
/*--------------------------------------------------------------------------*/
/*			FILE ALGOPVV.C													*/
/****************************************************************************/
/*	GENERALITIES															*/
/*																			*/
/*	Creation date:		01/04/04	APR											*/
/*																			*/
/*	Module : Calculate PVV according algorithm PVV									*/
/*																			*/
/*	File history :																*/
/*	01/04/04	APR		File creation											*/
/*  26/04/04 Bch 	ucPVV_ControlDKV initialisation modifications  */
/****************************************************************************/

#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>

#include "XS0CONV.H"

//#include "KYM_readkey.H"
#include "KYM_algopvv.H"
#include "KYM_AlgoDES.H"
#include "KEY_Manager.H"

#include "Key.h"
#include "DECRYPT_KEY.h"

//MODIF: DES wynid
#include "Des.h"
#include "utils.h"
#include "TLV.h"

/*
Conversion d'un long en ascii numerique
*/
void LongToAsciiDeci(unsigned long hexa, unsigned char *ascii, unsigned short lg_ascii)
{
unsigned long 		nb;
short			cpt;

    nb = hexa;
    for(cpt=(lg_ascii-1); cpt>=0; cpt--)
    {
    	ascii[cpt] = (unsigned char)(nb%10) + '0';
    	nb = nb/10;
   	}
}

/*****************************************************************************
*	Function	vKEY_CalculateMAC
*---------------------------------------------------------------------------
*	Purpose:	MAC algorithm with DEA-1 according to ISO 8731-1
*---------------------------------------------------------------------------
*	Input Parameters:
*	pucKey				KEY1 or KEY2 according to exchange indicator
*	pucData				Data are different according to PIN type
*	usDataLength			Length of data must be eight's multiple
*
*	Output Parameters:
*	pucRES1				Result of algorithm
*
*	Return Codes
*	NONE
******************************************************************************/
void vKEY_CalculateMAC(unsigned char* pucKey, unsigned char* pucData, unsigned int usDataLength, unsigned char* pucRES1)
{
unsigned char * tucResult;
unsigned char * tucRES1;
unsigned char *	pucBlock;
unsigned char	i,j;
unsigned int	usLoop;
unsigned int	usRest;
unsigned short us_erreur;

	tucResult = OS_Malloc(LENGTH_KEY);
	tucRES1 = OS_Malloc(LENGTH_KEY);

	/*
	**	Number of loop needs to get MAC
	*/
	usLoop=(usDataLength/LENGTH_KEY);
	usRest=(usDataLength%LENGTH_KEY);

	if(usRest>0)
	{
		usLoop+=1;
	}

	/*
	**	Initialization block (zero-filled)
	*/
	memset(tucResult, 0, LENGTH_KEY);
	for(i=0;i<usLoop;i++)
	{
		pucBlock = &pucData[i*LENGTH_KEY];
		for(j=0;j<LENGTH_KEY;j++)
		{
			tucResult[j] ^= pucBlock[j];
		}
//MODIF: DES wynid
		//vKEY_DES_Encrypt(tucRES1, tucResult, LENGTH_KEY, pucKey);
		us_erreur =CHIF_DES8 ( tucResult,
                  	        tucRES1,
                  	        pucKey,
                  	        DES_ENCRYPT);

		memcpy(tucResult, tucRES1, LENGTH_KEY);
	}

	memcpy(pucRES1, tucResult, LENGTH_KEY);

	OS_Free(tucResult);
	OS_Free(tucRES1);
}

/*****************************************************************************
*	Function	vPVV_PrepareOffset
*---------------------------------------------------------------------------
*	Purpose:	Prepare data to offset calculation
*---------------------------------------------------------------------------
*	Input Parameters:
*	pucTrack						ISO2 track in BCD format
*	pucSecretCode				Secret code keyed in ASCII
*	pucDriverCode				Driver code keyed in ASCII
*
*	Output Parameters:
*	pucVal						Value expected by function to calculate offset
*
*	Return Codes
*	NONE
******************************************************************************/
void vPVV_PrepareOffset(unsigned char* pucTrack,unsigned char* pucDriverCode,unsigned char* pucVal)
{
	/*
	**	PIN type = '4'
	**
	**	Repeat on a total of 48 digits :
	**	6 digits for issuer
	**	6 digits for customer number
	**	4 digits of PIN code keyed by holder
	*/
	memcpy(pucVal,pucTrack,LENGTH_ISSUER_CODE);
	sConvAsc2Bcd(pucDriverCode,&pucVal[LENGTH_ISSUER_CODE],LENGTH_CODE,CONV_RIGHT_JUST);
	memcpy(&pucVal[8],pucVal,LENGTH_KEY);
	memcpy(&pucVal[16],pucVal,LENGTH_KEY);

}

/*****************************************************************************
*	Function	vPVV_PreparePVV
*---------------------------------------------------------------------------
*	Purpose:	Prepare data to PVV calculation
*---------------------------------------------------------------------------
*	Input Parameters:
*	pucTrack						ISO2 track in BCD format
*	pucSecretCode				Secret code keyed in ASCII
*	ucTypePIN					Output value is different according to PIN type
*
*	Output Parameters:
*	pucVal						Value expected by function to calculate PVV
*
*	Return Codes
*	NONE
******************************************************************************/
void vPVV_PreparePVV(unsigned char* pucTrack,unsigned char* pucSecretCode,unsigned char ucTypePIN, unsigned char* pucVal)
{
unsigned char * tucValue;
unsigned char	tucBcd[2];

	tucValue = OS_Malloc(48);

	if(ucTypePIN==TYPE_PIN_9)
	{
		/*
		**	PIN type ='9'
		**
		**	37 digits of iso2 track read in file C1 on Actys card
		**	6 digits for customer number
		**	4 digits for card number
		**	1 digit for the duplicate number
		*/
		sConvBcd2Asc(pucTrack,tucValue,48,CONV_RIGHT_JUST);
		memcpy(&tucValue[LENGTH_TRACK],&tucValue[INDEX_CUSTOMER_CODE],LENGTH_CUSTOMER_CODE);
		memcpy(&tucValue[LENGTH_TRACK+LENGTH_CUSTOMER_CODE],&tucValue[INDEX_CARD_NUMBER],LENGTH_CARD_NUMBER);
		tucValue[LENGTH_TRACK+LENGTH_CUSTOMER_CODE+LENGTH_CARD_NUMBER]=tucValue[INDEX_DUPLICATA_NUMBER];
		sConvAsc2Bcd(tucValue,pucVal,48,CONV_RIGHT_JUST);
	}
	else
	{
		/*
		**	PIN type ='0' to '4'
		**
		**	The value given to MAC algorithm is composed by :
		**	4 digits of PIN code keyed by holder
		**	37 digits of iso2 track read in file C1 on Actys card
		**	4 digits of PIN code key by holder again
		**	3 digits at the constant value 'FFF'
		*/
		sConvAsc2Bcd(pucSecretCode,pucVal,LENGTH_CODE,CONV_RIGHT_JUST);
		memcpy(&pucVal[2],pucTrack,20);
		sConvAsc2Bcd(pucSecretCode,tucBcd,LENGTH_CODE,CONV_RIGHT_JUST);
		pucVal[20]&=0xF0;
		pucVal[20]|=(tucBcd[0]>>4);
		pucVal[21]=((tucBcd[0]<<4)|(tucBcd[1]>>4));
		pucVal[22]=((tucBcd[1]<<4)|0x0F);
		pucVal[23]=0xFF;
	}

	OS_Free(tucValue);
}

/*****************************************************************************
*	Function	_vAUT_FindOffset
*---------------------------------------------------------------------------
*	Purpose:	Find offset
*---------------------------------------------------------------------------
*	Input Parameters:
*	pucRES					Adding of MAC and DES algorithm results
*
*	Output Parameters:
*	pucOffsetPVV				Offset or PVV found
*
*	Return Codes
*	NONE
******************************************************************************/
void vPVV_FindOffsetPVV(unsigned char* pucRES, unsigned char* pucOffsetPVV)
{
unsigned char	i;
unsigned char	ucNbBytes;
unsigned char * tucRES;

	tucRES = OS_Malloc(LENGTH_KEY*4);

	ucNbBytes=0;

	sConvBcd2Asc(pucRES,tucRES,LENGTH_KEY*4,CONV_RIGHT_JUST);

	/*
	**	Look for number in RES
	*/
	for(i=0;i<LENGTH_KEY*4;i++)
	{
		/*
		**	Offset don't exceed 4 bytes
		*/
		if(ucNbBytes>3)
			return;

		/*
		**	Offset is only number suite
		*/
		if((tucRES[i]>='0')&&(tucRES[i]<='9'))
		{
			pucOffsetPVV[ucNbBytes]=tucRES[i];
			ucNbBytes++;
		}
	}

	OS_Free(tucRES);
}

/*****************************************************************************
*	Function	vAUT_PrepareTrack
*---------------------------------------------------------------------------
*	Purpose:	Troncate data in ISO2 track to PVV calculation
*---------------------------------------------------------------------------
*	Input Parameters:
*	pucTrack					ISO2 track in format BCD
*
*	Output Parameters:
*	PvvTrack					modified track
*
*	Return Codes
*	NONE
******************************************************************************/
void vPVV_PrepareTrack(unsigned char* pucTrack, unsigned char* PvvTrack)
{
unsigned char * tucTrack;

	tucTrack = OS_Malloc(LENGTH_TRACK_MAX+1);

	sConvBcd2Asc(pucTrack, tucTrack, LENGTH_TRACK_MAX, CONV_RIGHT_JUST);

	/*
	**	Suppress PVV
	*/
	tucTrack[29]='0';
	tucTrack[30]='0';
	tucTrack[31]='0';
	tucTrack[32]='0';

	if(tucTrack[36]=='1')
	{
		tucTrack[33]='0';
		tucTrack[34]='0';
		tucTrack[35]='0';
	}

	sConvAsc2Bcd(tucTrack, PvvTrack, LENGTH_TRACK_MAX, CONV_RIGHT_JUST);

	OS_Free(tucTrack);

}

/*****************************************************************************
*  Function ucCalculatePinCode
* ---------------------------------------------------------------------------
*  Purpose: Calculate the PIN code
* ---------------------------------------------------------------------------
*  Input Parameters:
*	pucDataInput			Result of DES and MAC algorithms
*	pucPINOffset			PIN read on track
*
*  Output Parameters:
*	pucPINCode			PIN code calculated
*
*  Return Codes
*  NONE
*
******************************************************************************/
unsigned char ucCalculatePinCode(unsigned char* pucDataInput, unsigned char* pucPINOffset, unsigned char* pucPINCustomer)
{
unsigned char	i;
unsigned char	ucNbBytes;
unsigned char * tucRES;
unsigned char	ucSecretCodeBCD;
unsigned char	ucOffsetBCD;
unsigned char	ucTemp;
unsigned char * tucPinCode;

	tucRES = OS_Malloc(LENGTH_KEY*2);
	tucPinCode = OS_Malloc(LENGTH_CODE);

	ucNbBytes=0;
	ucSecretCodeBCD = 0;
	ucOffsetBCD = 0;
	ucTemp = 0;

	sConvBcd2Asc(pucDataInput, tucRES, LENGTH_KEY*2, CONV_RIGHT_JUST);

	/*
	**	Recherche du nombre
	*/
	for(i=0 ; i<LENGTH_KEY*2 ; i++)
		{
		/*
		**	Offset doesn't exceed 4 bytes
		*/
		if(ucNbBytes>3)
			{
			break;
			}

		/*
		**	Offset is only number suite
		*/
		if((tucRES[i]>='0')&&(tucRES[i]<='9'))
			{
			tucPinCode[ucNbBytes]=tucRES[i];
			ucNbBytes++;
			}
		}

	/* calcul du code pin client en soustrayant le code secret du pin offset modulo 10*/
	for(i = 0 ; i <LENGTH_CODE ; i++)
	{
		/*
		**	Format PIN code and PIN offset in BCD
		*/
		ucSecretCodeBCD =  tucPinCode[i]-'0';
		ucOffsetBCD = pucPINOffset[i]-'0';

		if(ucOffsetBCD < ucSecretCodeBCD)
		{
			ucOffsetBCD += 10;
		}

		/*
		**	Substract a byte of PIN code at a byte of PIN offset
		*/
		ucSecretCodeBCD = ucOffsetBCD-ucSecretCodeBCD;

		/*
		**	PIN code formated in ASCII
		*/
		tucPinCode[i] = ucSecretCodeBCD + '0';
	}

	OS_Free(tucRES);

	/*
	**	Compare PIN code calculated to PIN Offset read on the track
	*/
	if(memcmp(tucPinCode, pucPINCustomer, LENGTH_CODE) == 0)
	{
		OS_Free(tucPinCode);
		return TRUE;
	}

	OS_Free(tucPinCode);
	return FALSE;

}

/*****************************************************************************
*  Function ucCalculatePinCodeUTA
* ---------------------------------------------------------------------------
*  Purpose: Create a UTA secret code
* ---------------------------------------------------------------------------
*  Input Parameters:
*	pucDataInput : buffer de donnees servant à calculer le code pin client
*	pucPINOffset
*	pucPINCustomer
*
*
*  Output Parameters:
*	NONE
*
*  Return Codes
*  TRUE or FALSE
*
******************************************************************************/
unsigned char ucCalculatePinCodeUTA(unsigned char* pucDataInput, unsigned char* pucPINOffset, unsigned char* pucPINCustomer)
{
unsigned char	i;
unsigned char	ucNbBytes;
unsigned char * tucRES;
unsigned char	ucSecretCodeBCD;
unsigned char	ucOffsetBCD;
unsigned char	ucTemp;
unsigned char * tucPinCode;

	tucRES = OS_Malloc(LENGTH_KEY*2);
	tucPinCode = OS_Malloc(LENGTH_CODE);

	ucNbBytes=0;
	ucSecretCodeBCD = 0;
	ucOffsetBCD = 0;
	ucTemp = 0;

	sConvBcd2Asc(pucDataInput,tucRES,LENGTH_KEY*2,CONV_RIGHT_JUST);

	/*
	**	Recherche du nombre
	*/
	for(i=0;i<LENGTH_KEY*2;i++)
	{
		/*
		**	Offset don't exceed 4 bytes
		*/
		if(ucNbBytes>3)
		{
			break;
		}

		/*
		**	Offset is only number suite
		*/
		if((tucRES[i]>='0')&&(tucRES[i]<='9'))
		{
			tucPinCode[ucNbBytes]=tucRES[i];
			ucNbBytes++;
		}
	}

	/* calcul du code pin client en ajoutant le code secret du pin offset modulo 10*/
	for(i = 0; i <LENGTH_CODE; i++)
	{
		/*
		**	Format PIN code and PIN offset in BCD
		*/
		ucSecretCodeBCD =  tucPinCode[i]-'0';
		ucOffsetBCD = pucPINOffset[i]-'0';

		/*
		**	Add a byte of PIN code at a byte of PIN offset
		*/
		ucSecretCodeBCD = ucOffsetBCD+ucSecretCodeBCD;

		/*
		**	PIN code is composed by number so hexadecimal values is reduced
		**	in decimal values
		*/
		if(ucSecretCodeBCD > 9)
			{
				ucSecretCodeBCD -= 10;
			}

		/*
		**	PIN code formated in ASCII
		*/
		tucPinCode[i] = ucSecretCodeBCD + '0';
	}

	OS_Free(tucRES);

	/*
	**	Compare PIN code calculated to PIN Offset read on the track
	*/
	if(memcmp(tucPinCode,pucPINCustomer,LENGTH_CODE)==0)
	{
		OS_Free(tucPinCode);
		return TRUE;
	}

	OS_Free(tucPinCode);
	return FALSE;

}

/*****************************************************************************
*
*								PUBLIC FUNCTIONS
*
******************************************************************************
*/


/*****************************************************************************
*	Function	sKEY_ReadKey
*---------------------------------------------------------------------------
*	Purpose:	Get a selected key
*---------------------------------------------------------------------------
*	Input Parameters:
*	ucNumKey				Number of key
*	pucCryptogram			Cryptogram of key
*
*	Output Parameters:
*	pucKey					Key decrypted
*
*	Return Codes
*	OK				OK
*	ERR_PARAM				parameters error
******************************************************************************/
short sKEY_ReadKey(unsigned char ucNumKey, unsigned char* pucKey, unsigned char* pucCryptogram)
{
unsigned char * tucKeyLeft;
unsigned char * tucKeyRight;
short	sCr;

	tucKeyLeft = OS_Malloc(LENGTH_KEY);
	tucKeyRight = OS_Malloc(LENGTH_KEY);

	switch(ucNumKey)
	{
		case KEY1:
		case KEY2:
		case M_DKV:
		case M_UTA:
			/*
			**	An other function allow to verify PVV calculation so these keys aren't need
			**	for anyone secondary application
			*/
			sCr=NOK;
			break;
		default:
			/*
			**	Read crypted key
			*/
			sCr = sReadKey(ucNumKey,tucKeyLeft,tucKeyRight);
			if(sCr==OK)
			{
				/*
				**	Decrypted key for secondary application use
				*/
//MODIF : DES wynid
				CHIF_TripleDES8 (	pucCryptogram,
                  			   pucKey,
                  			   tucKeyLeft,
                  			   tucKeyRight,
                  			   DES_DECRYPT);
			}
			break;
	}

	OS_Free(tucKeyLeft);
	OS_Free(tucKeyRight);

	return sCr;
}


/*****************************************************************************
*	Function	ucPVV_ControlDKV
*---------------------------------------------------------------------------
*	Purpose:	Calculate PVV and compare with PVV input for DKV cards
*---------------------------------------------------------------------------
*	Input Parameters:
*	ucPinType				Type of PIN indicating if offset calculation is required
*	pucTrack					ISO2 track in format BCD
*	pucSecretCode			Secret code in ASCII
*	pucPINOffset				PIN offset read on track
*	pucKey					Key DKV
*
*	Output Parameters:
*	NONE
*
*	Return Codes
*	TRUE or FALSE
******************************************************************************/
unsigned char ucPVV_ControlDKV(unsigned char* pucTrack, unsigned char* pucSecretCode, unsigned char* pucPINOffset, unsigned char* pucKey)
{
	unsigned char * tucDesData;
	unsigned char * tucEncryptData;
	unsigned char * tucASCIITampon;
	unsigned char   uc_Erreur;
	unsigned short  us_cr;

	tucDesData = OS_Malloc(LENGTH_KEY);
	tucEncryptData = OS_Malloc(LENGTH_KEY);
	tucASCIITampon = OS_Malloc((LENGTH_KEY+1)*2);

	/* On construit le buffer de données sur lequel on applique le DES */
	/* Le 1er caractere de la piste n'est pas pris en compte */
	sConvBcd2Asc(pucTrack, tucASCIITampon, (LENGTH_KEY+1)*2, CONV_LEFT_JUST);

	sConvAsc2Bcd(&tucASCIITampon[1],tucDesData, LENGTH_KEY*2, CONV_LEFT_JUST);

	/* On applique un algorithme DES suivant la norme DES ANSI X3.92 */
//MODIF: DES wynid
	//vKEY_DES_Encrypt(tucEncryptData, tucDesData, LENGTH_KEY, pucKey);
	us_cr = CHIF_DES8 ( 	tucDesData,
           					tucEncryptData,
           					pucKey,
           					DES_ENCRYPT);

	uc_Erreur = ucCalculatePinCode(tucEncryptData, pucPINOffset, pucSecretCode);

	OS_Free(tucASCIITampon);
	OS_Free(tucDesData);
	OS_Free(tucEncryptData);

	/* Calcul du code PIN client */
	return uc_Erreur;
}

/*****************************************************************************
*	Function	ucPVV_ControlUTA
*---------------------------------------------------------------------------
*	Purpose:	Calculate PVV and compare with PVV input for UTA cards
*---------------------------------------------------------------------------
*	Input Parameters:
*	ucPinType				Type of PIN indicating if offset calculation is required
*	pucTrack					ISO2 track in format BCD
*	pucSecretCode			Secret code in ASCII
*	pucPINOffset				PIN offset read on track
*	pucKey					Key DKV
*
*	Output Parameters:
*	NONE
*
*	Return Codes
*	TRUE or FALSE
******************************************************************************/
unsigned char ucPVV_ControlUTA(unsigned char ucPinType, unsigned char* pucTrack,unsigned char* pucSecretCode,unsigned char* pucPINOffset,unsigned char* pucKey)
{
unsigned char * tucDesData;
unsigned char * tucEncryptData;
unsigned char * tucASCIITampon;
unsigned char * tucPIN;
unsigned char	i;
unsigned char   uc_Erreur;

	tucDesData = OS_Malloc(LENGTH_KEY);
	tucEncryptData = OS_Malloc(LENGTH_KEY);
	tucASCIITampon = OS_Malloc((LENGTH_KEY+1)*2);
	tucPIN = OS_Malloc(LENGTH_CODE);

	/* On construit le buffer de données sur lequel on applique le DES */
	/* Le 1er caractere de la piste n'est pas pris en compte */
	sConvBcd2Asc(pucTrack, tucASCIITampon, (LENGTH_KEY+1)*2,CONV_LEFT_JUST);
	tucASCIITampon[16]='0';

	sConvAsc2Bcd(&tucASCIITampon[1],tucDesData, (LENGTH_KEY*2), CONV_LEFT_JUST);

	/* On applique un algorithme DES suivant la norme DES ANSI X3.92 */
//MODIF: DES wynid
	//vKEY_DES_Encrypt(tucEncryptData, tucDesData, LENGTH_KEY, pucKey);
	CHIF_DES8 ( tucDesData,
           		tucEncryptData,
           		pucKey,
           		DES_ENCRYPT);
	/* On récupère le PIN qui est en hexadecimal */
	tucPIN[0] = tucEncryptData[0]>>4;
	tucPIN[1] = tucEncryptData[0]&0x0F;
	tucPIN[2] = tucEncryptData[1]>>4;
	tucPIN[3] = tucEncryptData[1]&0x0F;

	/*
	**	Le PIN doit être composé de chiffres
	*/
	for(i=0;i<LENGTH_CODE;i++)
		{
			if(tucPIN[i] > 9)
				{
					tucPIN[i] -= 10;
				}
		}

	/*
	**	Mise sous forme BCD du PIN
	*/
	tucEncryptData[0] = tucPIN[0]<<4;
	tucEncryptData[0] |= tucPIN[1];
	tucEncryptData[1] = tucPIN[2]<<4;
	tucEncryptData[1] |= tucPIN[3];

	uc_Erreur = ucCalculatePinCodeUTA(tucEncryptData, pucPINOffset, pucSecretCode);

	OS_Free(tucDesData);
	OS_Free(tucEncryptData);
	OS_Free(tucASCIITampon);
	OS_Free(tucPIN);

	/* Calcul du code PIN client */
	return uc_Erreur;
}

/*****************************************************************************
*	Function	ucPVV_ControlGR
*---------------------------------------------------------------------------
*	Purpose:	Calculate PVV and compare with PVV input for GR cards
*---------------------------------------------------------------------------
*	Input Parameters:
*	ucPinType				Type of PIN indicating if offset calculation is required
*	pucTrack					ISO2 track in format BCD
*	pucSecretCode			Secret code in ASCII
*	pucDriverCode			Driver code in ASCII
*	pucPVV					PVV read on track
*	pucKey					Key1, Key2, Key DKV or Key UTA according to needs
*
*	Output Parameters:
*	NONE
*
*	Return Codes
*	TRUE or FALSE
******************************************************************************/
unsigned char ucPVV_ControlGR(unsigned char ucPinType, unsigned char* pucTrack,unsigned char* pucSecretCode,unsigned char* pucDriverCode,unsigned char* pucPVV,unsigned char* pucKey,unsigned char* pucRES2)
{
//MODIF: pile reduite
unsigned char * tucValMac;//[LENGTH_MAC_INPUT];
unsigned char * tucRES1;
unsigned char * tucRES;
unsigned char * tucOffset;
unsigned char * tucSecretCode;
//MODIF: pile reduite
unsigned char * tucTrack;//[LENGTH_TRACK_MAX];
unsigned char * tucPVV;
unsigned char	i;

	tucTrack = OS_Malloc(LENGTH_TRACK_MAX);
	tucValMac = OS_Malloc(LENGTH_MAC_INPUT);
	tucRES1 = OS_Malloc(LENGTH_KEY);
	tucRES = OS_Malloc(LENGTH_KEY*2);
	tucOffset = OS_Malloc(LENGTH_CODE+1);
	tucSecretCode = OS_Malloc(LENGTH_CODE+1);
	tucPVV = OS_Malloc(LENGTH_PVV);

	memset(tucValMac, 0, LENGTH_MAC_INPUT);
	UTS_memcpy(tucSecretCode, pucSecretCode, LENGTH_CODE);
	tucSecretCode[LENGTH_CODE]=0;

	/*
	**	Need to change ISO2 track before to do PVV calculation
	*/
	vPVV_PrepareTrack(pucTrack,tucTrack);

	if(ucPinType==TYPE_PIN_4)
	{
		/*
		**	Preparing value needs to input according to card type pin
		**	Warning : track is in BCD format
		*/
		vPVV_PrepareOffset(tucTrack, pucDriverCode, tucValMac);

		/*
		**	Use MAC algorithm to obtain RES1
		*/
		vKEY_CalculateMAC(pucKey, tucValMac, LENGTH_MAC_INPUT, tucRES1);
		/*
		**	Use DES algorithm to obtain RES2
		*/
//MODIF: DES wynid
		//vKEY_DES_Encrypt(pucRES2, tucRES1, LENGTH_KEY,pucKey);
		CHIF_DES8 ( tucRES1,
           			pucRES2,
           			pucKey,
           			DES_ENCRYPT);
		/*
		**	Find Offset
		*/
//MODIF: compil pour THALES
		//sprintf((char*)tucRES,"%.8s%.8s",(char*)tucRES1,(char*)pucRES2);
		memcpy(tucRES,tucRES1,8);
		memcpy(tucRES+8,pucRES2,8);
		vPVV_FindOffsetPVV(tucRES,tucOffset);

		/*
		**	Calculate temporary PIN
		*/
		for(i=0;i<LENGTH_CODE;i++)
		{
			tucOffset[i] = tucOffset[i] - '0';
			tucSecretCode[i] = tucSecretCode[i] - '0';

			if(tucSecretCode[i] < tucOffset[i])
			{
				tucSecretCode[i] += 10;
			}

			/*
			**	Substract a byte of PIN offset at a byte of PIN code
			*/
			tucSecretCode[i] = ((tucSecretCode[i]-tucOffset[i])+0x30);
		}
	}

	/*
	**	Prepare value in format expected by PVV algorithm
	*/
	vPVV_PreparePVV(tucTrack, tucSecretCode, ucPinType, tucValMac);

	/*
	**	Use MAC algorithm to obtain RES1
	*/
	vKEY_CalculateMAC(pucKey, tucValMac, LENGTH_MAC_INPUT, tucRES1);

	/*
	**	Use DES algorithm to obtain RES2
	*/
//MODIF: DES pour wynid
	//vKEY_DES_Encrypt(pucRES2, tucRES1, LENGTH_KEY,pucKey);
	CHIF_DES8 ( tucRES1,
        			pucRES2,
        			pucKey,
        			DES_ENCRYPT);
	/*
	**	Find PVV in RES (=RES1+RES2)
	*/
	UTS_memcpy(tucRES, tucRES1, LENGTH_KEY);
	UTS_memcpy(&tucRES[LENGTH_KEY], pucRES2, LENGTH_KEY);
	vPVV_FindOffsetPVV(tucRES, tucPVV);

        OS_Free(tucTrack);
        OS_Free(tucValMac);
	OS_Free(tucRES1);
	OS_Free(tucRES);
	OS_Free(tucOffset);
	OS_Free(tucSecretCode);

	if(memcmp(tucPVV, pucPVV, LENGTH_PVV)==0)
	{
		OS_Free(tucPVV);
		return TRUE;
	}

	OS_Free(tucPVV);
	return FALSE;

}

/* ------------------------------------------------------------------------
    UTIL_AtoL() :   conversion d'une chaine en un entier. La conversion se
                    fait dans la base choisie. Le resultat de la conversion
                    est retourne par la fonction
     parametres:
                IN  : asc       : pointeur sur la chaine devant etre convertie
                IN  : base      : base de conversion
                OUT : ulConvert : chaine convertie en unsigned long
    ------------------------------------------------------------------------- */
unsigned long UTIL_AtoL(char *asc, unsigned char ucBase)
{
    unsigned long ulConvert = 0, ulInd = 0;

    while (asc[ulInd] != 0)
    {
        if (asc[ulInd] <= '9')
            ulConvert = ucBase * ulConvert + ((unsigned long)asc[ulInd++]) - 48;
        else
            ulConvert = ucBase * ulConvert + ((unsigned long)asc[ulInd++]) - 55;
    };

    return (ulConvert);
}

/****************************************************************************/
/*	FUNCTION	lConvAtol														*/
/****************************************************************************/
/*	Converts a string to a long.													*/
/*																			*/
/*	Input :																	*/
/*		(I-) pucBuf	adress of the string to convert								*/
/*		(I-) usLg		the string length											*/
/*																			*/
/*	Result : the converted value of the input string									*/
/****************************************************************************/
long lConvAtol(unsigned char* pucBuf, unsigned short usLg)
{
unsigned char * tucBufConv;
unsigned long   ul_Long;

	tucBufConv = OS_Malloc(30);

	if (usLg > (30 -1))
	{
		usLg = (30 -1);
	}
	memset(tucBufConv, 0, 30);
	UTS_memcpy(tucBufConv, pucBuf, usLg);

//MODIF: pour compil THALES
	//return atol((char*)tucBufConv);
	ul_Long = UTIL_AtoL((char*)tucBufConv,10);

	OS_Free(tucBufConv);

	return(ul_Long);
}

/*****************************************************************************
*	Function	ucPVV_EncipheredDriverNumber
*---------------------------------------------------------------------------
*	Purpose	Encipher the driver number read on track
*---------------------------------------------------------------------------
*	Input Parameters:
*	pucDriverNumber				Number of driver present on track
*
*	Output Parameters
*	pucEncipheredDriverNumber		Enciphered number of driver
*	plDriverNumber
*
*	Return Codes
*	TRUE or FALSE
******************************************************************************/
unsigned char ucPVV_EncipheredDriverNumber(unsigned char* pucDriverNumber, unsigned char* pucEncipheredDriverNumber, long* plDriverNumber)
	{
	unsigned char	bResult;

	bResult = TRUE;

	switch(pucDriverNumber[3])
		{
			case '0':
			case '3':
			case '6':
			case '9':
//MODIF: pour compil THALES
				//sprintf((char*)pucEncipheredDriverNumber,"%c%c%c%c",pucDriverNumber[2],pucDriverNumber[3],pucDriverNumber[1],pucDriverNumber[0]);
				pucEncipheredDriverNumber[0]=pucDriverNumber[2];
				pucEncipheredDriverNumber[1]=pucDriverNumber[3];
				pucEncipheredDriverNumber[2]=pucDriverNumber[1];
				pucEncipheredDriverNumber[3]=pucDriverNumber[0];
				break;

			case '2':
			case '4':
			case '8':
//MODIF: pour compil THALES
				//sprintf((char*)pucEncipheredDriverNumber,"%c%c%c%c",pucDriverNumber[3],pucDriverNumber[2],pucDriverNumber[0],pucDriverNumber[1]);
				pucEncipheredDriverNumber[0]=pucDriverNumber[3];
				pucEncipheredDriverNumber[1]=pucDriverNumber[2];
				pucEncipheredDriverNumber[2]=pucDriverNumber[0];
				pucEncipheredDriverNumber[3]=pucDriverNumber[1];
				break;

			case '1':
			case '5':
			case '7':
//MODIF: pour compil THALES
				//sprintf((char*)pucEncipheredDriverNumber,"%c%c%c%c",pucDriverNumber[1],pucDriverNumber[3],pucDriverNumber[2],pucDriverNumber[0]);
				pucEncipheredDriverNumber[0]=pucDriverNumber[1];
				pucEncipheredDriverNumber[1]=pucDriverNumber[3];
				pucEncipheredDriverNumber[2]=pucDriverNumber[2];
				pucEncipheredDriverNumber[3]=pucDriverNumber[0];
				break;

			default:
				bResult = FALSE;
				break;
		}

	if(bResult)
		{
		*plDriverNumber = lConvAtol(pucEncipheredDriverNumber, 4);
		}

	return bResult;

	}

/*****************************************************************************
*	Function	ucPVV_EncipheredCustomerNumber
*---------------------------------------------------------------------------
*	Purpose	Encipher the Customer number read on track
*---------------------------------------------------------------------------
*	Input Parameters:
*	pucCustomerNumber				Number of Customer present on track
*
*	Output Parameters
*	pucEncipheredCustomerNumber		Enciphered number of Customer
*	plCustomerNumber
*
*	Return Codes
*	TRUE or FALSE
******************************************************************************/
unsigned char ucPVV_EncipheredCustomerNumber(unsigned char* pucCustomerNumber, unsigned char* pucEncipheredCustomerNumber, long* plCustomerNumber)
	{
	unsigned char	bResult;

	bResult = TRUE;

	switch(pucCustomerNumber[5])
		{
			case '0':
			case '3':
			case '6':
			case '9':
//MODIF: pour compil THALES
				//sprintf((char*)pucEncipheredCustomerNumber,"%c%c%c%c%c%c",pucCustomerNumber[0],pucCustomerNumber[5],pucCustomerNumber[2],pucCustomerNumber[4],pucCustomerNumber[3],pucCustomerNumber[1]);
				pucEncipheredCustomerNumber[0]=pucCustomerNumber[0];
				pucEncipheredCustomerNumber[1]=pucCustomerNumber[5];
				pucEncipheredCustomerNumber[2]=pucCustomerNumber[2];
				pucEncipheredCustomerNumber[3]=pucCustomerNumber[4];
				pucEncipheredCustomerNumber[4]=pucCustomerNumber[3];
				pucEncipheredCustomerNumber[5]=pucCustomerNumber[1];
				break;

			case '2':
			case '4':
			case '8':
//MODIF: pour compil THALES
				//sprintf((char*)pucEncipheredCustomerNumber,"%c%c%c%c%c%c",pucCustomerNumber[0],pucCustomerNumber[2],pucCustomerNumber[5],pucCustomerNumber[1],pucCustomerNumber[4],pucCustomerNumber[3]);
				pucEncipheredCustomerNumber[0]=pucCustomerNumber[0];
				pucEncipheredCustomerNumber[1]=pucCustomerNumber[2];
				pucEncipheredCustomerNumber[2]=pucCustomerNumber[5];
				pucEncipheredCustomerNumber[3]=pucCustomerNumber[1];
				pucEncipheredCustomerNumber[4]=pucCustomerNumber[4];
				pucEncipheredCustomerNumber[5]=pucCustomerNumber[3];
				break;

			case '1':
			case '5':
			case '7':
//MODIF: pour compil THALES
				//sprintf((char*)pucEncipheredCustomerNumber,"%c%c%c%c%c%c",pucCustomerNumber[0],pucCustomerNumber[4],pucCustomerNumber[1],pucCustomerNumber[3],pucCustomerNumber[2],pucCustomerNumber[5]);
				pucEncipheredCustomerNumber[0]=pucCustomerNumber[0];
				pucEncipheredCustomerNumber[1]=pucCustomerNumber[4];
				pucEncipheredCustomerNumber[2]=pucCustomerNumber[1];
				pucEncipheredCustomerNumber[3]=pucCustomerNumber[3];
				pucEncipheredCustomerNumber[4]=pucCustomerNumber[2];
				pucEncipheredCustomerNumber[5]=pucCustomerNumber[5];
				break;

			default:
				bResult = FALSE;
				break;
		}

	if(bResult)
		{
		*plCustomerNumber = lConvAtol(pucEncipheredCustomerNumber, 6);
		}

	return bResult;

	}

/*****************************************************************************
*	Function	ucPVV_EncipheredExpirationYearNumber
*---------------------------------------------------------------------------
*	Purpose	Encipher the expiration year read on track
*---------------------------------------------------------------------------
*	Input Parameters:
*	pucExpirationYear				Expiration year present on track
*
*	Output Parameters
*	pucEncipheredExpirationYear		Enciphered expiration year of Customer
*	plExpirationYear
*
*	Return Codes
*	TRUE or FALSE
******************************************************************************/
unsigned char ucPVV_EncipheredExpirationYearNumber(unsigned char* pucExpirationYear, unsigned char* pucEncipheredExpirationYear, long* plExpirationYear)
	{
	unsigned char	bResult;

	bResult = TRUE;

	switch(pucExpirationYear[1])
		{
			case '0':
			case '3':
			case '4':
			case '8':
			case '9':
//MODIF: pour compil THALES
				//sprintf((char*)pucEncipheredExpirationYear,"%c%c",pucExpirationYear[0],pucExpirationYear[1]);
				pucEncipheredExpirationYear[0]=pucExpirationYear[0];
				pucEncipheredExpirationYear[1]=pucExpirationYear[1];

				*plExpirationYear = lConvAtol(pucEncipheredExpirationYear, 2);
				break;

			case '1':
			case '2':
			case '5':
			case '6':
			case '7':
//MODIF: pour compil THALES
				//sprintf((char*)pucEncipheredExpirationYear,"%c%c",pucExpirationYear[1],pucExpirationYear[0]);
				pucEncipheredExpirationYear[0]=pucExpirationYear[1];
				pucEncipheredExpirationYear[1]=pucExpirationYear[0];
				*plExpirationYear = lConvAtol(pucEncipheredExpirationYear, 2);
				*plExpirationYear = -(*plExpirationYear);
				break;

			default:
				bResult = FALSE;
				break;
		}

	return bResult;

	}

/*****************************************************************************
*	Function	ucPVV_ControlDKV_Old
*---------------------------------------------------------------------------
*	Purpose	Determinate if pin code is correct for an old DKV card
*---------------------------------------------------------------------------
*	Input Parameters:
*	pucTrack				Track of old DKV card
*	PinCode				Pin code keyed by customer
*	pucDriverNumber		Code keyed by driver
*
*	Output Parameters
*	NONE
*
*	Return Codes
*	TRUE or FALSE
******************************************************************************/
unsigned char ucPVV_ControlDKV_Old(unsigned char* pucTrack, unsigned char* pucPinCode, unsigned char* pucDriverNumber)
	{
	unsigned char * tucEncipheredCustomerNumber;
	unsigned char * tucEncipheredDriverNumber;
	unsigned char	tucEncipheredExpirationYear[2];
	unsigned char * tucKH;
	unsigned char * tucASCIITampon;
	long	lDriverNumber, lCustomerNumber, lExpirationYear;
	long	lKH, lCO, lKK, lK0, lFH, lVH, lK6, lF1, lF2, lF3, lF4;
	long	lTemp1, lTemp2, lTemp3, lTemp4, lTemp5;

	tucEncipheredCustomerNumber = OS_Malloc(6);
	tucEncipheredDriverNumber = OS_Malloc(4);
	tucKH = OS_Malloc(8);
	tucASCIITampon = OS_Malloc(LENGTH_TRACK_MAX);

	lDriverNumber = 0L;
	lCustomerNumber = 0L;
	lExpirationYear = 0L;

	sConvBcd2Asc(pucTrack, tucASCIITampon, LENGTH_TRACK_MAX, CONV_LEFT_JUST);

	if(!ucPVV_EncipheredDriverNumber(pucDriverNumber, tucEncipheredDriverNumber, &lDriverNumber))
	{
		return FALSE;
	}

	if(!ucPVV_EncipheredCustomerNumber(&tucASCIITampon[4], tucEncipheredCustomerNumber, &lCustomerNumber))
	{
		return FALSE;
	}

	if(!ucPVV_EncipheredExpirationYearNumber(&tucASCIITampon[16], tucEncipheredExpirationYear, &lExpirationYear))
	{
		return FALSE;
	}

	/*
	**	CO
	**	KK * (K0 K6)
	**	100000
	**	FH * (F4 - F3 + 11)
	**	(F3 + F2 - 7) * (K0 - (F2 - F1 + 5) * (F1 +F4 - 3))
	**	VH * (K0 F4)
	*/
	lCO = lConvAtol(pucPinCode, 4);
	lKK = lConvAtol(&tucASCIITampon[7], 3);
	lK0 = 105;
	lK6 = lConvAtol(&tucASCIITampon[9], 1);
	lFH = lDriverNumber;
	lVH = lExpirationYear;
	lF1 = lConvAtol(&pucDriverNumber[0], 1);
	lF2 = lConvAtol(&pucDriverNumber[1], 1);
	lF3 = lConvAtol(&pucDriverNumber[2], 1);
	lF4 = lConvAtol(&pucDriverNumber[3], 1);

	lTemp1 = lKK * (lK0 - lK6);
	lTemp2 = 100000;
	lTemp3 = lFH * (lF4 - lF3 + 11);
	lTemp4 = ((lF3 + lF2 -7) * (lK0 - (lF2 - lF1 + 5) * (lF1 + lF4 - 3)));
	lTemp5 = (lVH * (lK0 - lF4));

	lKH = lCO + lTemp1 + lTemp2 + lTemp3 - lTemp4 + lTemp5;

	lKH += 10000000;

//MODIF: pour compil THALES
	//sprintf((char*)tucKH, "%d",lKH);
	LongToAsciiDeci(lKH, tucKH, 8);

	OS_Free(tucEncipheredDriverNumber);
	OS_Free(tucASCIITampon);

	if(memcmp(&tucKH[4], &tucEncipheredCustomerNumber[2], 4) != 0)
	{
		OS_Free(tucEncipheredCustomerNumber);
		OS_Free(tucKH);
		return FALSE;
	}

	OS_Free(tucKH);
	OS_Free(tucEncipheredCustomerNumber);
	return TRUE;

}

/*****************************************************************************
*	Function	bKEY_PVV_Control
*---------------------------------------------------------------------------
*	Purpose:	Control the PVV
*---------------------------------------------------------------------------
*	Input Parameters:
*	ucAlgorithm				Algorithm
*	ucPinType				Type of PIN indicating if offset calculation is required
*	pucTrack					ISO2 track in format BCD
*	pucSecretCode			Secret code in ASCII
*	pucDriverCode			Driver code in ASCII
*	pucPVV					PVV or PIN offset read on track
*	pucCrypto				Cryptogram of key
*
*	Output Parameters:
*	pucRES2					RES2 is only used by Actys card for external authenticate
*
*	Return Codes
*	TRUE or FALSE
******************************************************************************/
unsigned char bKEY_PVV_Control(unsigned char ucAlgorithm, unsigned char ucPinType, unsigned char* pucTrack, unsigned char* pucSecretCode, unsigned char* pucDriverCode, unsigned char* pucPVV, unsigned char* pucCrypto, unsigned char* pucRES2)
{
	unsigned char * tucKey;
	unsigned char * tucKeyLeft;
	unsigned char * tucKeyRight;
	unsigned char * tucCrypto;
	unsigned char	ucNumKey;
	unsigned char	ucResult;
	short	sCr;

	tucKey = OS_Malloc(LENGTH_KEY);
	tucKeyLeft = OS_Malloc(LENGTH_KEY);
	tucKeyRight = OS_Malloc(LENGTH_KEY);
	tucCrypto = OS_Malloc(LENGTH_KEY);

	/*
	**	Determinate the number of key according to algorithm selected
	*/
	switch(ucAlgorithm)
	{
		case ALGORITHM_DKV:
			ucNumKey = M_DKV;
			break;

		case ALGORITHM_DKV_OLD:
			ucNumKey = M_DKV;
			break;

		case ALGORITHM_UTA:
			ucNumKey = M_UTA;
			break;

		case ALGORITHM_GR_KEY1:
			ucNumKey = KEY1;
			break;

		case ALGORITHM_GR_KEY2:
			ucNumKey = KEY2;
			break;

		default:
			return FALSE;
			break;
	}

	if(ucAlgorithm != ALGORITHM_DKV_OLD)
	{
		/*
		**	Look for wanted key
		*/
		sCr = sReadKey(ucNumKey, tucKeyLeft, tucKeyRight);

		if(sCr != OK)
		{
			return FALSE;
		}

		/*
		**	Decript key
		*/
		UTS_memcpy(tucCrypto, pucCrypto, 8);
//MODIF: DES wynid
		//vDeCrypter3DES(tucKey, tucCrypto, LENGTH_KEY, tucKeyLeft, tucKeyRight);
		CHIF_TripleDES8 (	tucCrypto,
                			tucKey,
                			tucKeyLeft,
                			tucKeyRight,
                			DES_DECRYPT);

	}

	OS_Free(tucKeyLeft);
	OS_Free(tucKeyRight);
	OS_Free(tucCrypto);
	/*
	**	Control of PVV
	*/
	switch(ucAlgorithm)
		{
		case ALGORITHM_DKV:
			ucResult = ucPVV_ControlDKV(pucTrack, pucSecretCode, pucPVV, tucKey);
			break;

		case ALGORITHM_DKV_OLD:
			ucResult = ucPVV_ControlDKV_Old(pucTrack, pucSecretCode, pucDriverCode);
			break;

		case ALGORITHM_UTA:
			ucResult = ucPVV_ControlUTA(ucPinType, pucTrack, pucSecretCode, pucPVV, tucKey);
			break;

		case ALGORITHM_GR_KEY1:
		case ALGORITHM_GR_KEY2:
			ucResult = ucPVV_ControlGR(ucPinType, pucTrack, pucSecretCode, pucDriverCode, pucPVV, tucKey, pucRES2);
			break;

		default:
			ucResult = FALSE;
			break;
		};

	OS_Free(tucKey);

	return ucResult;
}

/*	End of ALGOPVV	*/

