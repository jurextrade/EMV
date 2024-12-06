#include "des.h"
#include "pvv.h"
#include "Total.h"
#include "utils.h"
#include "DECRYPT_KEY.h"

#include "Key_Manager.h"
#include "XS0CONV.H"
#include "KYM_algopvv.H"

#include <string.h>

int AtoBCD(unsigned char* BCDstr,unsigned char* ASCstr, int iNmax);
short BCDtoA(unsigned char *pucBcd, unsigned char *pucAsc, unsigned char ucAscLen);


/*******************************************************************************
********************************************************************************
** FONCTION: bLuhnControl --> Controle du code Lhun pour Reference Client
**
**
** APPEL   :IN  pucBuffer       --> Buffer a controler
**          IN  ucLgBuffer      --> Lg du buffer a controler
**
** RETOUR  : kACTYS_STATUS_OK          				--> Controle OK
**           kACTYS_STATUS_ERROR_REF_CLIENT        --> erreur ref client
********************************************************************************
*******************************************************************************/
ACTYS_STATUS bLuhnControl (unsigned char* pucBuffer, unsigned char ucLgBuffer)
{
unsigned short usTotal, usDouble, usNbCar, usIndic;
ACTYS_STATUS   e_Status;

	usTotal = 0;
	usNbCar = ucLgBuffer - 1;
	usIndic = 1;

	do
	{
		if ((usIndic%2) == 0)
		{
			usTotal += (unsigned short) (* (pucBuffer+usNbCar-usIndic) - '0');
		}
		else
		{
			usDouble = (unsigned short) (2* (* (pucBuffer+usNbCar-usIndic) - '0'));
			if (usDouble >= 10)
			{
				usTotal += ((usDouble%10) + 1);
			}
			else
			{
				usTotal += usDouble;
			}
		}
		usIndic++;
	}
	while (usIndic <= usNbCar);

	usDouble = (unsigned short) (* (pucBuffer+usNbCar) - '0');

	if (((usTotal + usDouble) % 10) == 0)
	{
		e_Status = kACTYS_STATUS_OK;
	}
	else
	{
		e_Status = kACTYS_STATUS_ERROR_REF_CLIENT;
	}

	return (e_Status);
}

void CalculPINTemp(unsigned char * puc_PINTemp,unsigned char * puc_PIN, unsigned char * puc_OFFSET)
{
int i;

      // Calcul du PIN Temporaire PIN_temp[i] = PIN[i] - Offset[i]
      for(i=0;i<4;i++)
      {
         if( puc_PIN[i] < puc_OFFSET[i] )
            puc_PIN[i]+=10;

         puc_PINTemp[i] = (puc_PIN[i] - puc_OFFSET[i]) + 0x30;
      }
}

unsigned char FindBCDInHEXA(unsigned char * puc_BufferHEXA, unsigned char * puc_BuffBCD, unsigned char uc_LgHEXA,unsigned char uc_LgBCD)
{
unsigned char uc_count;
unsigned char uc_digit;
unsigned char uc_NbdigitOK;
int i;

   uc_NbdigitOK=0;
   uc_count=0;
   while((uc_NbdigitOK<uc_LgBCD) && (uc_count<uc_LgHEXA))
   {
      //pour les 2 quartets de l'octet analyse
      for(i=0;i<2;i++)
      {
         if(i==0)
            uc_digit = puc_BufferHEXA[uc_count] >> 4;
         else
            uc_digit = puc_BufferHEXA[uc_count] & 0x0f;

         // quartet decimal ?
         if(uc_digit <= 0x09)
         {
            puc_BuffBCD[uc_NbdigitOK] = uc_digit+0x30;

            uc_NbdigitOK++; //digit OK

            if( uc_NbdigitOK == uc_LgBCD )
               break;
         }
      }
      uc_count++;
   }

   if(uc_NbdigitOK !=uc_LgBCD)
      return(1);
   else
      return(0);

}



/*
Conversion d'un octet bcd en unsigned char
*/
unsigned char BcdToHexa(unsigned char bcd)
{
unsigned char	hexa;

   hexa = ((bcd >> 4) * 10) + (bcd & 0x0F);

   return(hexa);
}

/*
Conversion des 2 premiers octets d'un buffer bcd en unsigned int
*/
unsigned int BufBcdToInt(unsigned char *buf)
{
unsigned int	nb = 0;

   nb = (unsigned int)BcdToHexa(buf[1]) + (((unsigned int)BcdToHexa(buf[0]))*100);

   return( nb );
}

/*
Conversion des 4 premiers octets d'un buffer ASCII (decimal) en unsigned int
*/
unsigned int BufASCIIDeciToInt(unsigned char *buf_ascii)
{
unsigned int	nb = 0;

   nb = (buf_ascii[3]-0x30) + (buf_ascii[2]-0x30)*10 + (buf_ascii[1]-0x30)*100 + (buf_ascii[0]-0x30)*1000;

   return( nb );
}

/*
Conversion d'un unsigned int sur 4 digits en buffer bcd de 2 octets
*/
void IntToBufASCIIDeci(unsigned char *buf, unsigned int ui_nb)
{
   buf[0] = ui_nb/1000 + 0x30;
   ui_nb -= (ui_nb/1000)*1000;

   buf[1] = (ui_nb)/100 + 0x30;
   ui_nb -= (ui_nb/100)*100;

   buf[2] = ui_nb/10 + 0x30;
   ui_nb -= (ui_nb/10)*10;

   buf[3] = ui_nb + 0x30;
}


/*
Conversion d'un unsigned int sur 5 digits en buffer bcd de 2 octets et demi
*/
void IntToBufASCIIDeci5(unsigned char *buf, unsigned int ui_nb)
{
   buf[0] = ui_nb/10000 + 0x30;
   ui_nb -= (ui_nb/10000)*10000;

   buf[1] = ui_nb/1000 + 0x30;
   ui_nb -= (ui_nb/1000)*1000;

   buf[2] = (ui_nb)/100 + 0x30;
   ui_nb -= (ui_nb/100)*100;

   buf[3] = ui_nb/10 + 0x30;
   ui_nb -= (ui_nb/10)*10;

   buf[4] = ui_nb + 0x30;
}


int ValHexa(unsigned char buf)
{
	int Valeur;

	if (buf>='A')
		Valeur = buf-55;
	else
		Valeur = buf-'0';
	
	return Valeur;
}

char IntToHexa(unsigned int nb)
{
	char Hexa;

	if (nb>=10)
		Hexa = 55+nb;
	else
		Hexa = nb+'0';
	
	return Hexa;
}

/*****************************************************************************
*	Function	_bPIN_GetAlgorithm
*---------------------------------------------------------------------------
*	Purpose	Get algorithm used
*---------------------------------------------------------------------------
*	Input Parameters:
*	ucExchangeFlag		Indicator of exchange to determinate key which must be used
*
*	Output Parameters:
*	pucAlgorithm			algorithm
*	pucCryptogram
*
*	Return Codes
*	TRUE or FALSE
******************************************************************************/
ACTYS_STATUS _bPIN_GetAlgorithm(unsigned char ucExchangeFlag, unsigned char* pucAlgorithm, unsigned char* pucCryptogram)
{
ACTYS_STATUS  e_Status;
unsigned char tucCrypto_KEY1[8] ="\xEA\x60\x0F\x35\x6F\x91\xC9\x3D";
unsigned char tucCrypto_KEY2[8] ="\xC9\x5A\x9A\x75\x89\x21\x2C\x07";


	e_Status = kACTYS_STATUS_OK;

	/*
	**	Choice the key which must be use
	*/
	switch(ucExchangeFlag)
	{
		case '1':
		case '5':
			/*
			**	Use KEY1
			*/
			*pucAlgorithm = ALGORITHM_GR_KEY1;
			UTS_memcpy(pucCryptogram, tucCrypto_KEY1, LG_KEY);
		break;
		case '6':
			/*
			**	Use KEY2
			*/
			*pucAlgorithm = ALGORITHM_GR_KEY2;
			UTS_memcpy(pucCryptogram, tucCrypto_KEY2, LG_KEY);
		break;
		default:
			/*
			**	Invalid card
			*/
			/* Should never happen */
			e_Status = kACTYS_STATUS_ERROR_ISO2;
		break;
	};

	return(e_Status);
}



/*******************************************************************************
********************************************************************************
** FONCTION: PVV_CheckPVV_GR_EURO --> Algorithme PVV (Pin Verification Value)
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
ACTYS_STATUS PVV_CheckPVV_GR_EURO(unsigned char * puc_ISO2,
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

   if(!bKEY_PVV_Control(ucAlgorithm,
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

/*******************************************************************************
********************************************************************************
** FONCTION: PVV_CheckPVV_AVIA --> Algorithme PVV (Pin Verification Value)
**                                pour carte AVIA
**
** APPEL   :IN  puc_ISO2       --> ISO2 sur 39 octets (format ASCII) sans signe de depart
**          IN  puc_PIN        --> PIN porteur
**
** RETOUR  : kACTYS_STATUS_ERROR_PVV          --> Erreur du calcul PVV
**           kACTYS_STATUS_OK                 --> Operation effectuee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS PVV_CheckPVV_AVIA(unsigned char * puc_ISO2,
                               unsigned char * puc_PIN)
{
unsigned char   auc_PIN_Offset[4]={'1','7','8','9'};
ACTYS_STATUS    e_status;
unsigned char   uc_Tampon1;
unsigned char   uc_Tampon2;
int i;

   e_status = kACTYS_STATUS_OK;

	// Complement a 10 du code PIN
	for(i=0;i<LENGTH_CODE;i++)
	{
		if(puc_PIN[i]!='0')
			puc_PIN[i]=0x30 +( 10 - ( puc_PIN[i] - 0x30 ) );
	}

   // permutation des chiffres 1/3 et 2/4
   uc_Tampon1 = puc_PIN[0];
   uc_Tampon2 = puc_PIN[2];
   puc_PIN[2] = uc_Tampon1;
   puc_PIN[0] = uc_Tampon2;
   uc_Tampon1 = puc_PIN[1];
   uc_Tampon2 = puc_PIN[3];
   puc_PIN[3] = uc_Tampon1;
   puc_PIN[1] = uc_Tampon2;

	// ajout sans retenue de la valeur fixe 1789
	for(i=0;i<LENGTH_CODE;i++)
	{
		uc_Tampon1 =  (puc_PIN[i]-'0') + (auc_PIN_Offset[i]-'0');

		//pas de retenue
		if( uc_Tampon1 >= 10 )
         uc_Tampon1 -= 10;

		// Affectation nouvelle valeur
		puc_PIN[i] = uc_Tampon1+'0';
	}

	// ajout sans retenue des octets 11-14 de la piste ISO2
	for(i=0;i<LENGTH_CODE;i++)
	{
		uc_Tampon1 =  (puc_PIN[i]-'0') + (puc_ISO2[i+OFFSET_NUM_CARTE_AVIA]-'0');

		//pas de retenue
		if( uc_Tampon1 >= 10 )
         uc_Tampon1 -= 10;

		// Affectation nouvelle valeur
		puc_PIN[i] = uc_Tampon1+'0';
	}

	uc_Tampon1 = 0;
	for(i=0;i<LENGTH_CODE;i++)
	{
		if(puc_PIN[i] != puc_ISO2[i+OFFSET_CODE_CRYPTE_AVIA])
			uc_Tampon1=1;
	}

	if( uc_Tampon1 != 0)
	   e_status = kACTYS_STATUS_ERROR_PVV;

   return(e_status);

}


/*******************************************************************************
********************************************************************************
** FONCTION: PVV_CheckPVV_DKV --> Controle Algo PVV pour DKV
**                                Cide chauffeur ??
**
** APPEL   :IN  puc_ISO2       --> ISO2 sur 40 octets (format ASCII) avec signe de depart
**                                 avec signe de depart
**          IN  puc_PIN        --> PIN porteur
**          IN  puc_PIN_DRIVER --> PIN chauffeur
**          IN  uc_TypeDKV     --> DKV CLASSIC / SELECTION
**
** RETOUR  : kACTYS_STATUS_ERROR_PVV          --> Erreur du calcul PVV
**           kACTYS_STATUS_OK                 --> Operation effectuee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS PVV_CheckPVV_DKV(unsigned char * puc_ISO2,
                              unsigned char * puc_PIN,
                              unsigned char * puc_PIN_DRIVER,
                              unsigned char   uc_TypeDKV)
{
unsigned char   auc_PIN_Offset[4];
ACTYS_STATUS    e_status;
unsigned char   auc_Crypto_DKV[8] = CRYPTO_MDKV;
unsigned char * puc_ISO2BCD;

   e_status = kACTYS_STATUS_OK;

   puc_ISO2BCD = OS_Malloc(LG_ISO2_BCD);

   UTS_memcpy(auc_PIN_Offset,&puc_ISO2[OFFSET_PVV_DKV],4);

   //SELECTION DKV
   if( uc_TypeDKV == DKV_SELECTION_CARD )
   {
      sConvAsc2Bcd(puc_ISO2, puc_ISO2BCD, LG_ISO2, CONV_LEFT_JUST);

      if(!bKEY_PVV_Control(ALGORITHM_DKV,
			  						'0',					/* Useless Parameter for DKV */
		     	 	 				puc_ISO2BCD,
		     	  					puc_PIN,
		     	  					NULL,					/* Useless Parameter for DKV */
		     	  					auc_PIN_Offset,
		     	  					(unsigned char*)auc_Crypto_DKV,
		     	  					NULL))				/* Useless Parameter for DKV */
      {
   	   e_status = kACTYS_STATUS_ERROR_PVV;
      }
   }
   //CLASSIC DKV
   else
   {
      //-------------- Gestion Date expiration ------------------//
//MODIF JMA 01/02/05: date expiration - 1 mois pour anomalie 347
      // Mois expiration = Janvier ?
      if( (puc_ISO2[18]=='0') && (puc_ISO2[19]=='1') )
      {
      	//Decembre
      	puc_ISO2[18] ='1';
      	puc_ISO2[19] ='2';

      	//Unite Annee a zero
      	if(puc_ISO2[17] == '0')
      	{
      		//si annee 00
      		if(puc_ISO2[16] == '0')
      			puc_ISO2[16] = '9';
      		else
      		   puc_ISO2[16]--;

            puc_ISO2[17]='9';
         }
         else
            puc_ISO2[17]--;
      }
      // Mois expiration different de janvier
      else
      {
         if( puc_ISO2[19] == '0')
         {
         	puc_ISO2[19] = '9';
         	puc_ISO2[18]--;
         }
         else
            puc_ISO2[19]--;
      }
      //-------------- Gestion Date expiration ------------------//

      sConvAsc2Bcd(puc_ISO2, puc_ISO2BCD, LG_ISO2, CONV_LEFT_JUST);

      if(!bKEY_PVV_Control(ALGORITHM_DKV_OLD,
  		          				'0',					/* Useless Parameter for DKV Old*/
		          				puc_ISO2BCD,
		          				puc_PIN,
		          				puc_PIN_DRIVER,
		          				NULL,					/* Useless Parameter for DKV Old*/
		          				NULL,					/* Useless Parameter for DKV Old*/
		          				NULL))				/* Useless Parameter for DKV Old*/
      {
   	   e_status = kACTYS_STATUS_ERROR_PVV;
      }
   }

   OS_Free(puc_ISO2BCD);

   return(e_status);
}

/*******************************************************************************
********************************************************************************
** FONCTION: PVV_CheckPVV_UTA --> Algorithme PVV (Pin Verification Value)
**                                pour UTA
**
** APPEL   :IN  puc_ISO2       --> ISO2 sur 40 octets (format ASCII) sans signe de depart
**          IN  puc_PIN        --> PIN porteur
**				IN	 uc_FlagTest	 --> 0: carte reelle
**											  1: carte test
**
** RETOUR  : kACTYS_STATUS_ERROR_PVV          --> Erreur du calcul PVV
**           kACTYS_STATUS_OK                 --> Operation effectuee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS PVV_CheckPVV_UTA(unsigned char * puc_ISO2,
                              unsigned char * puc_PIN,
                              unsigned char   uc_FlagTest)
{
unsigned char * puc_ISO2BCD;
unsigned char   auc_PIN_Offset[4];
unsigned char   auc_Crypto_UTA[8];
ACTYS_STATUS    e_status;


	// TEST CARTE UTA REELLE ou TEST
	#if BICA
		if(uc_FlagTest == 0)
			UTS_memcpy2(auc_Crypto_UTA,cauc_CRYPTO_MUTA,LG_KEY); //carte reelle
		else
			UTS_memcpy2(auc_Crypto_UTA,cauc_CRYPTO_MUTA_TEST,LG_KEY); //carte test
   #else  // TOTAL -> carte reelle
      UTS_memcpy2(auc_Crypto_UTA,cauc_CRYPTO_MUTA,LG_KEY); //carte reelle
   #endif

   e_status = kACTYS_STATUS_OK;

   puc_ISO2BCD = OS_Malloc(LG_ISO2_BCD);

   UTS_memcpy(auc_PIN_Offset,&puc_ISO2[OFFSET_PVV_UTA],4);

   sConvAsc2Bcd(puc_ISO2, puc_ISO2BCD, LG_ISO2, CONV_LEFT_JUST);


   if(!bKEY_PVV_Control(ALGORITHM_UTA,
		      '0',					/* Useless Parameter for UTA */
		      puc_ISO2BCD,
		      puc_PIN,
		      NULL,					/* Useless Parameter for UTA */
		      auc_PIN_Offset,
		      (unsigned char*)auc_Crypto_UTA,
		      NULL))				/* Useless Parameter for UTA */
   {
   	e_status = kACTYS_STATUS_ERROR_PVV;
   }

   OS_Free(puc_ISO2BCD);

   return(e_status);

}

void CHIF_DES8_LOMO (unsigned char *DataIN, unsigned char *DataOUT ) {

  char	abcdef[8]={0X45,0X91,0X70,0X51,0X73,0X91,0X10,0X52};
  
  CHIF_DES8 (DataIN , DataOUT, abcdef,  DES_ENCRYPT);
}

/*******************************************************************************
********************************************************************************
** FONCTION: PVV_CheckPVV_LOMO --> Algorithme PVV (Pin Verification Value)
**                                pour LOMO
**
** APPEL   :IN  puc_ISO2       --> ISO2 sur 40 octets (format ASCII) sans signe de depart
**          IN  puc_PIN        --> PIN porteur
**
** RETOUR  : kACTYS_STATUS_ERROR_PVV          --> Erreur du calcul PVV
**           kACTYS_STATUS_OK                 --> Operation effectuee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS PVV_CheckPVV_LOMO(unsigned char * puc_ISO2,
                              unsigned char * puc_PIN)
{
	char szPINBlock[16];
	char PINBlock[8];
	char DataOut1[16];
	char DataOut2[8];
	char CDKey[4];
	int i;

	ACTYS_STATUS    e_status;
	
    e_status = kACTYS_STATUS_ERROR_PVV;
	if (puc_ISO2[0]>='0' && puc_ISO2[0]<='9')
		UTS_memcpy(szPINBlock,&puc_ISO2[0],16);		//PAN de la carte
	else
		UTS_memcpy(szPINBlock,&puc_ISO2[1],16);		//PAN de la carte
	
	AtoBCD((unsigned char*)PINBlock,(unsigned char*)szPINBlock,-16);  //PAN reduit
	
	CHIF_DES8_LOMO ((unsigned char*)PINBlock,  DataOut2);  //DES 
	
    BCDtoA(DataOut2,DataOut1,16);  // DES Agrandit

	UTS_memcpy(CDKey,DataOut1,4);
	
	CDKey[0]=IntToHexa(ValHexa(CDKey[0])%10);
	CDKey[1]=IntToHexa(ValHexa(CDKey[1])%10);
	CDKey[2]=IntToHexa(ValHexa(CDKey[2])%10);
	CDKey[3]=IntToHexa(ValHexa(CDKey[3])%10);
	
	
	
	CDKey[0]=IntToHexa((ValHexa(CDKey[0]) + ValHexa(puc_PIN[0]))%10);		
	
	CDKey[1]=IntToHexa((ValHexa(CDKey[1]) + ValHexa(puc_PIN[1]))%10);
	
	CDKey[2]=IntToHexa((ValHexa(CDKey[2]) + ValHexa(puc_PIN[2]))%10);
	
	CDKey[3]=IntToHexa((ValHexa(CDKey[3]) + ValHexa(puc_PIN[3]))%10);
	
	
	if (CDKey[0]==puc_ISO2[30] && CDKey[1]==puc_ISO2[31] && CDKey[2]==puc_ISO2[32] && CDKey[3]==puc_ISO2[33]) {
		e_status = kACTYS_STATUS_OK;
	}
	else {
		e_status = kACTYS_STATUS_ERROR_PVV;
	}
	return(e_status);
}

/*******************************************************************************
********************************************************************************
** FONCTION: PVV_CheckPVV_CSC --> Algorithme PVV (Pin Verification Value)
**                                pour CSC
**
** APPEL   :IN  puc_ISO2       --> ISO2 sur 40 octets (format ASCII) sans signe de depart
**          IN  puc_PIN        --> PIN porteur

**
** RETOUR  : kACTYS_STATUS_ERROR_PVV          --> Erreur du calcul PVV
**           kACTYS_STATUS_OK                 --> Operation effectuee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS PVV_CheckPVV_CSC(unsigned char * puc_ISO2,
                              unsigned char * puc_PIN)
{		
	unsigned char   Table1[10]={'6','2','9','4','0','3','D','1','8','C'};		
	unsigned char   Table2[10]={'C','B','D','A','6','0','2','1','7','F'};		
	unsigned char   CardNumber[5]; //fghij
	unsigned char   NumberStep3[12]; //klmnopqrstuv
	unsigned int   NumberStep4[4]; //wxyz
	unsigned char   NumberStep5[5]; //wxyz
	unsigned int NumberSecurity;
	ACTYS_STATUS    e_status;
	e_status = kACTYS_STATUS_OK;
	
	//Step 2
	if (puc_ISO2[0]=='B' || puc_ISO2[0]==' ' || puc_ISO2[0]==';')
	{
		CardNumber[0]=Table1[puc_ISO2[12]-'0']; //f
		CardNumber[1]=Table2[puc_ISO2[13]-'0']; //g
		CardNumber[2]=Table1[puc_ISO2[14]-'0']; //h
		CardNumber[3]=Table2[puc_ISO2[15]-'0']; //i
		CardNumber[4]=Table1[puc_ISO2[16]-'0']; //j
	}
	else
	{
		CardNumber[0]=Table1[puc_ISO2[11]-'0']; //f
		CardNumber[1]=Table2[puc_ISO2[12]-'0']; //g
		CardNumber[2]=Table1[puc_ISO2[13]-'0']; //h
		CardNumber[3]=Table2[puc_ISO2[14]-'0']; //i
		CardNumber[4]=Table1[puc_ISO2[15]-'0']; //j
	}
	


	//Step 3
    NumberStep3[0]=IntToHexa((ValHexa(CardNumber[3])+ValHexa(CardNumber[4]))%16); //k	
	NumberStep3[1]=IntToHexa((ValHexa(CardNumber[0])+ValHexa(CardNumber[2]))%16); //l	
	NumberStep3[2]=IntToHexa((ValHexa(CardNumber[1])+ValHexa(CardNumber[4]))%16); //m
	NumberStep3[3]=IntToHexa((ValHexa(CardNumber[2])+ValHexa(CardNumber[3]))%16); //n	
	NumberStep3[4]=IntToHexa((ValHexa(CardNumber[0])+ValHexa(CardNumber[4]))%16); //o	
	NumberStep3[5]=IntToHexa((ValHexa(CardNumber[1])+ValHexa(CardNumber[2]))%16); //p	
	NumberStep3[6]=IntToHexa((ValHexa(CardNumber[0])+ValHexa(CardNumber[1]))%16); //q	
	NumberStep3[7]=IntToHexa((ValHexa(CardNumber[1])+ValHexa(CardNumber[3]))%16); //r	
	NumberStep3[8]=IntToHexa((ValHexa(CardNumber[2])+ValHexa(CardNumber[4]))%16); //s	
	NumberStep3[9]=IntToHexa((ValHexa(CardNumber[0])+ValHexa(CardNumber[1])+ValHexa(CardNumber[2]))%16); //t
	NumberStep3[10]=IntToHexa((ValHexa(CardNumber[2])+ValHexa(CardNumber[3])+ValHexa(CardNumber[4]))%16); //u	
	NumberStep3[11]=IntToHexa((ValHexa(CardNumber[0])+ValHexa(CardNumber[3]))%16); //v	


	//Step 4

	NumberStep4[0]=(ValHexa(NumberStep3[0])^ValHexa(NumberStep3[1])^ValHexa(NumberStep3[2]))%4;
	NumberStep4[1]=(ValHexa(NumberStep3[3])^ValHexa(NumberStep3[4])^ValHexa(NumberStep3[5]))%16;
	NumberStep4[2]=(ValHexa(NumberStep3[6])^ValHexa(NumberStep3[7])^ValHexa(NumberStep3[8]))%16;
	NumberStep4[3]=(ValHexa(NumberStep3[9])^ValHexa(NumberStep3[10])^ValHexa(NumberStep3[11]))%16;

		
	//Step 5  Convert Hex ==> Dec
    NumberSecurity=(NumberStep4[0]*16*16*16)+(NumberStep4[1]*16*16)+(NumberStep4[2]*16)+(NumberStep4[3]);
		
	IntToBufASCIIDeci5(NumberStep5, NumberSecurity);		
	
	if (NumberStep5[3]==puc_PIN[0] && NumberStep5[4]==puc_PIN[1] && NumberStep5[1]==puc_PIN[2] && NumberStep5[2]==puc_PIN[3]) {
		e_status = kACTYS_STATUS_OK;
	}
	else {
		e_status = kACTYS_STATUS_ERROR_PVV;
	}

	return(e_status);
}

char *ultoa (unsigned long value, char *string, int radix)
    {
    char *dst;
    char digits[32];
    int i, n;

    dst = string;
    if (radix < 2 || radix > 36)
	{
	*dst = 0;
	return (string);
	}
    i = 0;
    do
	{
	n = value % (unsigned long)radix;
        digits[i++] = (n < 10 ? (char)n+'0' : (char)n-10+'a');
	value /= (unsigned long)radix;
	} while (value != 0);
    while (i > 0)
	*dst++ = digits[--i];
    *dst = 0;
    return (string);
}


char *itoa (int value, char *string, int radix)
    {
    char *dst;
    char digits[32];
    unsigned x;
    int i, n;

    dst = string;
    if (radix < 2 || radix > 36)
	{
	*dst = 0;
	return (string);
	}
    if (radix == 10 && value < 0)
	{
	*dst++ = '-';
	x = -value;
	}
    else
	x = value;
    i = 0;
    do
	{
	n = x % (unsigned)radix;
	digits[i++] = (n < 10 ? (char)n+'0' : (char)n-10+'a');
	x /= (unsigned)radix;
	} while (x != 0);
    while (i > 0)
	*dst++ = digits[--i];
    *dst = 0;
    return (string);
}

//-------------------------------------------------------------------------------------------------
int IToStr(int Num, unsigned char* Buffer, int Length)
{
char Tmp_Buf[7];
int i;
int RealLen;
   if(Length<0)
   {
      Length=-Length;
      ultoa((unsigned long)(unsigned int)Num,Tmp_Buf,10);
   }
   else
      itoa(Num,Tmp_Buf,10);
   RealLen=strlen((char*)Tmp_Buf);
   if(RealLen > Length) return(-1);
   
   for(i=0;i<Length;i++) {
     Buffer[Length]='0';
   }
   UTS_memcpy(&Buffer[Length-RealLen],Tmp_Buf,RealLen);
   return(1);
}


void CHIF_DES8_ARIS (unsigned char *DataIN, unsigned char *DataOUT );
/*******************************************************************************
********************************************************************************
** FONCTION: PVV_CheckPVV_ARIS --> Algorithme PVV (Pin Verification Value)
**                                pour ARIS
**
** APPEL   :IN  puc_ISO2       --> ISO2 sur 40 octets (format ASCII) sans signe de depart
**          IN  puc_PIN        --> PIN porteur
**				
**
** RETOUR  : kACTYS_STATUS_ERROR_PVV          --> Erreur du calcul PVV
**           kACTYS_STATUS_OK                 --> Operation effectuee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS PVV_CheckPVV_ARIS(unsigned char * puc_ISO2,
                              unsigned char * puc_PIN)
{
char*	pDelim2;
char szPINBlock[16];
char PINBlock[8];
//char DataOut1[8];
char DataOut2[8];
int i;
unsigned int code;
unsigned char codeChar[5];
ACTYS_STATUS    e_status;

unsigned char Chartmp;

for (i=0;i<40;i++)
{
  if (puc_ISO2[i]=='=' || puc_ISO2[i]=='D')
  {
    pDelim2=&puc_ISO2[i];
    i=45;
  }
}

if (i==40)
{
  e_status = kACTYS_STATUS_ERROR_PVV;

  return e_status;
}

UTS_memcpy(szPINBlock,pDelim2-12,12);		//12 last digit	of PAN
UTS_memcpy(&szPINBlock[12],pDelim2+1,4);	//ExpDate YYMM

AtoBCD((unsigned char*)PINBlock,(unsigned char*)szPINBlock,-16);

CHIF_DES8_ARIS ((unsigned char*)PINBlock,  DataOut2);

for (i=0;i<16;i++)
{  
  szPINBlock[0]=0; 
}

IToStr((int)*(unsigned short*)(&DataOut2[6]), (unsigned char*)szPINBlock, -5);   //szPINBlock - user PIN

if (szPINBlock[1]==puc_PIN[0] && szPINBlock[2]==puc_PIN[1] && szPINBlock[3]==puc_PIN[2] && szPINBlock[4]==puc_PIN[3]) {
  e_status = kACTYS_STATUS_OK;
}
else {
  e_status = kACTYS_STATUS_ERROR_PVV;
}


return e_status;
}


void CHIF_DES8_BICA (unsigned char *DataIN, unsigned char *DataOUT );
/*******************************************************************************
********************************************************************************
** FONCTION: PVV_CheckPVV_BICA --> Algorithme PVV (Pin Verification Value)
**                                pour ARIS
**
** APPEL   :IN  puc_ISO2       --> ISO2 sur 40 octets (format ASCII) sans signe de depart
**          IN  puc_PIN        --> PIN porteur
**				
**
** RETOUR  : kACTYS_STATUS_ERROR_PVV          --> Erreur du calcul PVV
**           kACTYS_STATUS_OK                 --> Operation effectuee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS PVV_CheckPVV_BICA(OS_HANDLE    * pP_Handle,unsigned char * puc_ISO2,
                              unsigned char * puc_PIN)
{
ACTYS_STATUS    e_status;
char szPANBlock[16];
char DonneeSortieDES[16];
char ResultatSelection[6];
char szPINOffset[6];
char PANBlock[8];
char DataOut1[8];
char CalculatePin[6];
int i,j;


if (puc_ISO2[0]=='B')
{
  UTS_memcpy(szPANBlock,&puc_ISO2[3],16);	//16 last digit	of PAN
  UTS_memcpy(szPINOffset,&puc_ISO2[25],6);	//Pin OFFSet
}
else
{
  UTS_memcpy(szPANBlock,&puc_ISO2[2],16);	//16 last digit	of PAN
  UTS_memcpy(szPINOffset,&puc_ISO2[24],6);	//Pin OFFSet
}
/*
API_UTILS_SendOnUART0( 'X');
for (i=0;i<6;i++) {
  API_UTILS_SendOnUART0( szPINOffset[i]);
}
API_UTILS_SendOnUART0( 'X');

API_UTILS_SendOnUART0( 'X');
for (i=0;i<16;i++) {
  API_UTILS_SendOnUART0( szPANBlock[i]);
}
API_UTILS_SendOnUART0( 'X');
*/

AtoBCD((unsigned char*)PANBlock,(unsigned char*)szPANBlock,-16);

/*API_UTILS_SendOnUART0( 'X');
for (i=0;i<8;i++) {
  API_UTILS_SendOnUART0( PANBlock[i]);
}
API_UTILS_SendOnUART0( 'X');
*/

CHIF_DES8_BICA ((unsigned char*)PANBlock,  DataOut1);
/*
API_UTILS_SendOnUART0( 'X');
for (i=0;i<8;i++) {
  API_UTILS_SendOnUART0( DataOut1[i]);
}
API_UTILS_SendOnUART0( 'X');
*/
BCDtoA(DataOut1,DonneeSortieDES,16);

/*
API_UTILS_SendOnUART0( 'X');
for (i=0;i<16;i++) {
  API_UTILS_SendOnUART0( DonneeSortieDES[i]);
}
API_UTILS_SendOnUART0( 'X');
*/


// Resultat Selection
j=0;
for (i=0;i<16 && j<6;i++)
{
  if (DonneeSortieDES[i]>='0' && DonneeSortieDES[i]<='9')
  {
    ResultatSelection[j]=DonneeSortieDES[i];
    j++;
  }
}
if (j<6)
{
  for (i=0;i<16 && j<6;i++)
  {
    if (DonneeSortieDES[i]=='A')
    {
      ResultatSelection[j]='0';
      j++;
    } else if (DonneeSortieDES[i]=='B')
    {
      ResultatSelection[j]='1';
      j++;
    } else if (DonneeSortieDES[i]=='C')
    {
      ResultatSelection[j]='2';
      j++;
    } else if (DonneeSortieDES[i]=='D')
    {
      ResultatSelection[j]='3';
      j++;
    } else if (DonneeSortieDES[i]=='E')
    {
      ResultatSelection[j]='4';
      j++;
    } else if (DonneeSortieDES[i]=='F')
    {
      ResultatSelection[j]='5';
      j++;
    }

  }  
}

/*
API_UTILS_SendOnUART0( 'X');
for (i=0;i<6;i++) {
  API_UTILS_SendOnUART0( ResultatSelection[i]);
}
API_UTILS_SendOnUART0( 'X');
*/

// OFFset Pin
for (j=0;j<6;j++)
{
  CalculatePin[j]=( ( ((szPINOffset[j] - 0x30) +  (ResultatSelection[j]- 0x30)) %10)+0x30);
}

/*
API_UTILS_SendOnUART0( 'X');
for (i=0;i<6;i++) {
  API_UTILS_SendOnUART0( CalculatePin[i]);
}
API_UTILS_SendOnUART0( 'X');
*/


if (CalculatePin[0]==puc_PIN[0] && CalculatePin[1]==puc_PIN[1] && CalculatePin[2]==puc_PIN[2] && CalculatePin[3]==puc_PIN[3]) {
  e_status = kACTYS_STATUS_OK;
}
else {
  e_status = kACTYS_STATUS_ERROR_PVV;
}


return e_status;
}


//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
// BCD to ASCII conversion table
const unsigned char BCD_Ascii[17] = "0123456789ABCDEF";


short BCDtoA(unsigned char *pucBcd, unsigned char *pucAsc, unsigned char ucAscLen)
{
 unsigned char ucNbCar;

 // calcul of bytes number to be converted
 ucNbCar = (unsigned char) (ucAscLen / 2);
 while (ucNbCar-- != 0)
 {
  *(pucAsc++) = BCD_Ascii [*pucBcd >> 4];
  *(pucAsc++) = BCD_Ascii [*(pucBcd++) & 0x0F];
 }

 return OK;
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
int AtoBCD(unsigned char* BCDstr,unsigned char* ASCstr, int iNmax)
{
int i=0;
int j=0;
char tmpChar=0;
int Sign=iNmax;
   *BCDstr=0;     //Для выравнивания по правому краю
   if(iNmax < 0)
   {
    iNmax=-iNmax;
    if(iNmax & 0x0001)
      goto  RIGHT_JUST;
   }

   while(i<iNmax)
   {
      tmpChar=ASCstr[i]-'0';
      if(Sign < 0)      //Если длина отрицательная
      {
	 if(tmpChar>9) tmpChar-=7;  //Превращение символов A..F в цифры
	 if((tmpChar<0) || (tmpChar>15))
	 {
	    return(-1);
	 }

      }
      else
      {
	 if((tmpChar<0) || (tmpChar>'>')) //допускаются разделители 2-дорожки МП
	 {
	    BCDstr[j]=0xF0;   //Строка завершается 0xF
	    break;
	 }
      }
      BCDstr[j] = tmpChar << 4;
      i++;
      if(i>=iNmax)
      {
	 BCDstr[j] |= 0xF;
	 break;
      }
RIGHT_JUST:
      tmpChar=ASCstr[i]-'0';
      if(Sign < 0)      //Если длина отрицательная
      {
	 if(tmpChar>9) tmpChar-=7;  //Превращение символов A..F в цифры
	 if((tmpChar<0) || (tmpChar>15))
	 {
	    return(-1);
	 }

      }
      else
      {
	 if((tmpChar<0) || (tmpChar>'>')) //допускаются разделители 2-дорожки МП
	 {
	    BCDstr[j] |= 0xF;    //Строка завершается 0xF
	    break;
	 }
      }
      BCDstr[j] |= tmpChar;
      i++;
      j++;
   }
   return(i);
}
