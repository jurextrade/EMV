#include "des.h"
#include "pvv.h"
#include "Total.h"
#include "utils.h"
#include "DECRYPT_KEY.h"

#include "Key_Manager.h"
#include "XS0CONV.H"
#include "KYM_algopvv.H"

#include <string.h>

ACTYS_STATUS _bPIN_GetAlgorithm(unsigned char ucExchangeFlag, unsigned char* pucAlgorithm, unsigned char* pucCryptogram);





/*****************************************************************************
*	Function	bKEY_PVV_Control_Test
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
unsigned char bKEY_PVV_Control_Test(unsigned char ucAlgorithm, unsigned char ucPinType, unsigned char* pucTrack, unsigned char* pucSecretCode, unsigned char* pucDriverCode, unsigned char* pucPVV, unsigned char* pucCrypto, unsigned char* pucRES2)
{
//	unsigned char * tucKey;
	unsigned char	ucResult;
  char	temp1;
  char	temp2;
  char	temp3;
  char	tucKey[8]={0XC2,0X74,0X7C,0X9C,0XA5,0X3F,0X83,0XE8};

//	tucKey = OS_Malloc(LENGTH_KEY);



	/*
	**	Determinate the number of key according to algorithm selected
	*/
	switch(ucAlgorithm)
	{
		case ALGORITHM_GR_KEY1:
      temp1=tucKey[0]+0x12;
      tucKey[0]=tucKey[5]-0x07;
      temp2=tucKey[1]-0x45;
      tucKey[1]=tucKey[2]+0x59;
      tucKey[2]=tucKey[3]+0x32;
      tucKey[3]=tucKey[6]+0x6E;
      temp3=tucKey[4]+0x59;
      tucKey[4]=temp2+0x75;
      tucKey[5]=tucKey[7]-0x12;
      tucKey[6]=temp3-0x14;
      tucKey[7]=temp1-0x99;
/*
      tucKey[0]=0x98;
      tucKey[1]=0xA2;
      tucKey[2]=0x64;
      tucKey[3]=0xDA;
      tucKey[4]=0x83;
      tucKey[5]=0xE0;
      tucKey[6]=0x3D;
      tucKey[7]=0x80;
*/
      break;

		case ALGORITHM_GR_KEY2:
      temp1=tucKey[0]+0x12;
      tucKey[0]=tucKey[5]-0x07;
      temp2=tucKey[1]-0x45;
      tucKey[1]=tucKey[2]+0x59;
      tucKey[2]=tucKey[3]+0x32;
      tucKey[3]=tucKey[6]+0x6E;
      temp3=tucKey[4]+0x59;
      tucKey[4]=temp2+0x75;
      tucKey[5]=tucKey[7]-0x12;
      tucKey[6]=temp3-0x14;
      tucKey[7]=temp1-0x99;
/*
      tucKey[0]=0x58;
      tucKey[1]=0xF1;
      tucKey[2]=0x38;
      tucKey[3]=0x62;
      tucKey[4]=0x4A;
      tucKey[5]=0x4C;
      tucKey[6]=0xBA;
      tucKey[7]=0xBA;
*/
      break;

		default:
//      OS_Free(tucKey);
			return FALSE;
	}

	
  /*
	**	Control of PVV
	*/
	ucResult = ucPVV_ControlGR(ucPinType, pucTrack, pucSecretCode, pucDriverCode, pucPVV, tucKey, pucRES2);

//	OS_Free(tucKey);

	return ucResult;
}


/*******************************************************************************
********************************************************************************
** FONCTION: PVV_CheckPVV_GR_EURO_Test --> Algorithme PVV (Pin Verification Value)
**                                    pour ACTYS/GR/EUROTRAFIC
**
** APPEL   :IN  puc_ISO2       --> ISO2 sur 40 octets (format ASCII) avec signe de depart
**          IN  puc_PIN        --> PIN porteur
**          IN  puc_PIN_DRIVER --> PIN chauffeur
**          OUT puc_RES2       --> Resultat intermediaire PVV pour VerifyPIN
**
** RETOUR  : kACTYS_STATUS_ERROR_PVV          --> Erreur du calcul PVV
**           kACTYS_STATUS_OK                 --> Operation effectuee
**           kACTYS_STATUS_ERROR_ISO2 			 --> Erreur sur Code echange
********************************************************************************
*******************************************************************************/
ACTYS_STATUS PVV_CheckPVV_GR_EURO_Test(unsigned char * puc_ISO2,
                                  unsigned char * puc_PIN,
                                  unsigned char * puc_PIN_DRIVER,
                                  unsigned char * puc_RES2)
{
unsigned char   ucAlgorithm;
unsigned char * puc_ISO2BCD;
unsigned char * auc_PIN_Offset;
unsigned char * auc_Cryptogram;
ACTYS_STATUS    e_status;

   auc_PIN_Offset = OS_Malloc(4);
   auc_Cryptogram = OS_Malloc(8);

   e_status = kACTYS_STATUS_OK;

   puc_ISO2BCD = OS_Malloc(LG_ISO2_BCD);

   UTS_memcpy(auc_PIN_Offset,&puc_ISO2[OFFSET_PVV_GR_EURO],4);

   sConvAsc2Bcd(puc_ISO2, puc_ISO2BCD, LG_ISO2, CONV_LEFT_JUST);

   if(_bPIN_GetAlgorithm(puc_ISO2[OFFSET_EXCHANGE_CODE], &ucAlgorithm, auc_Cryptogram) != kACTYS_STATUS_OK)
   {
      OS_Free(auc_PIN_Offset);
      OS_Free(auc_Cryptogram);
      OS_Free(puc_ISO2BCD);
      return(e_status);
   }

   if(!bKEY_PVV_Control_Test(ucAlgorithm,
   	    	        puc_ISO2[OFFSET_TYPE_PIN],
		        puc_ISO2BCD,
		        puc_PIN,
		        puc_PIN_DRIVER,
		        auc_PIN_Offset,
		        auc_Cryptogram,
		        puc_RES2))
   {
   	e_status = kACTYS_STATUS_ERROR_PVV;
   }

   OS_Free(auc_PIN_Offset);
   OS_Free(auc_Cryptogram);
   OS_Free(puc_ISO2BCD);

   return(e_status);

}