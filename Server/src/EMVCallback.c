#include "EMV.h"
#include "Actys.h"

// send login to Router


int Send_Login(MXCom* pcom, EMVClient* pclient)
{
	MXMessage* pmessage;
	BUFFERPARM	Buffer;
	char		message[100];

	int			lReturn = 0;


	printf("Sending Login Indentification\n");
	sprintf(message, "*LOGIN^%s^%s^%s*", LoginServer, pclient->UserName, pclient->UserPassword);

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)&message; // (char*)malloc(strlen(message));
	//memcpy(Buffer.BufferContent, message, dataSize);

	pmessage = MXCreateMessage(pcom->MX, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	MXSend(pcom->MX, pcom, pmessage);

	pmessage = MXRecv(pcom->MX, pcom);

	BUFFERPARM* RecvBuffer = (BUFFERPARM*)MXGetValue(pmessage, "Buffer", 1);
	if (strncmp(RecvBuffer->BufferContent, "-1", 2) == 0)
	{
		printf("Indentification Failed\n");
		lReturn = 0;
	}
	else
	{
		printf("Indentification Succeeded\n");
		lReturn = 1;
	}
	strncpy(pclient->UserID, RecvBuffer->BufferContent, RecvBuffer->BufferSize);
	pclient->UserID[RecvBuffer->BufferSize] = 0;

	free(RecvBuffer->BufferContent);
	free(RecvBuffer);

	return lReturn;
}

int OnRecvUserInfo(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	printf("Receive Userinfo from point of sale\n");

	STRING UserName		 = (STRING)MXGetValue(pmessage, "UserName", 1);
	STRING UserPassword  = (STRING)MXGetValue(pmessage, "UserPassword", 1);

	strcpy(pclient->UserName, UserName);
	strcpy(pclient->UserPassword, UserPassword);


	
	if (Send_Login(EMVRooterCom, pclient) != 1)
	{
		printf("Problem in User Information transaction will be treated *****OFFLINE****\n");
	}
	//Send_Start(EMVRooterCom, pclient);


	return 1;
}

int OnRecvTransaction(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;
	EMVTransaction* ptransaction;
	
	printf("Receive transaction from point of sale\n");

	BYTE Type = *(BYTE*)MXGetValue(pmessage, "Type", 1);
	STRING Currency = (STRING)MXGetValue(pmessage, "Currency", 1);
	STRING Amount = (STRING)MXGetValue(pmessage, "Amount", 1);
	BYTE Media = *(BYTE*)MXGetValue(pmessage, "Media", 1);


	ptransaction = EMVInitTransaction(pemv, Type, Currency, Amount, Media);

	//DOUDOU	
		//ptransaction->SecurityRelatedControlInformation[8];   
	EMVSetTransaction(pemv, pclient, ptransaction);
	return 1;
}

int OnRecvATR(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	printf("Receive ATR from point of sale\n");

	STRING strATR = (STRING)MXGetValue(pmessage, "Atr", 1);



	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, "Received ATR from client %s\n", strATR);
		s_printf(smessage, pclient, "%s", strace);
	}

	if (strATR[0] == 0)
		CallACTYSS(pemv, pclient);
	else
		EMVOnRecvCardDetectionAndReset(pemv, pclient, (BYTE*)strATR, strlen(strATR));

	return 1;
}

int OnRecvCommand (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	BYTE P1						= *(BYTE*)MXGetValue(pmessage, "P1", 1) ;
	BYTE P2						= *(BYTE*)MXGetValue(pmessage, "P2", 1) ;

	BUFFERPARM* Buffer			= (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);
	
	int			outSize			= *(LONG*)MXGetValue (pmessage, "Size", 1);
	BYTE*		outData			= (BYTE*)Buffer->BufferContent;

	if (pemv->DebugEnabled)
	{
		EMVTraceAPDU(pclient, 0, 0, 0, 0, outData, outSize, 1);
	}

	if (P2 == 0x36)
		EMVOnRecvATC(pemv, pclient, P1, P2, outData, outSize);
	else
	if (P2 == 0x13)
		EMVOnRecvLastOnlineATC(pemv, pclient, P1, P2, outData, outSize);
	else
	if (P2 == 0x17)
		EMVOnRecvPinTryCounter(pemv, pclient, P1, P2, outData, outSize);

	free (Buffer->BufferContent);
	free (Buffer);
	return 1;
}



int OnSendACFirst (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	if (pemv->DebugEnabled)
	{
		BYTE Cla					= *(BYTE*)MXGetValue(pmessage, "Cla", 1) ;
		BYTE Ins					= *(BYTE*)MXGetValue(pmessage, "Ins", 1) ;
		BYTE P1						= *(BYTE*)MXGetValue(pmessage, "P1", 1) ;
		BYTE P2						= *(BYTE*)MXGetValue(pmessage, "P2", 1) ;
		BUFFERPARM* Buffer			= (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);
		BYTE		outSize			= *(BYTE*)MXGetValue (pmessage, "Size", 1);
		BYTE*		outData			= (BYTE*)Buffer->BufferContent;
		
		pclient = EMVGetClientFromCom (pemv, pcom);
		EMVTraceAPDU (pclient, Cla, Ins, P1, P2, outData, outSize, 0);
		free (Buffer->BufferContent);
		free (Buffer);
	}
	return 1;
}
int OnRecvACFirst (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	BYTE Cla					= *(BYTE*)MXGetValue(pmessage, "Cla", 1) ;
	BYTE Ins					= *(BYTE*)MXGetValue(pmessage, "Ins", 1) ;
	BUFFERPARM* Buffer			= (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);
	
	int			outSize			= *(LONG*)MXGetValue (pmessage, "Size", 1);
	BYTE*		outData			= (BYTE*)Buffer->BufferContent;
	

	if (pemv->DebugEnabled)
	{
		EMVTraceAPDU(pclient, 0, 0, 0, 0, outData, outSize, 1);
	}

	EMVOnRecvACFirst (pemv, pclient, outData, outSize);
	free (Buffer->BufferContent);
	free (Buffer);
	return 1;

}

int OnSendACSecond (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	if (pemv->DebugEnabled)
	{
		BYTE Cla					= *(BYTE*)MXGetValue(pmessage, "Cla", 1) ;
		BYTE Ins					= *(BYTE*)MXGetValue(pmessage, "Ins", 1) ;
		BYTE P1						= *(BYTE*)MXGetValue(pmessage, "P1", 1) ;
		BYTE P2						= *(BYTE*)MXGetValue(pmessage, "P2", 1) ;
		BUFFERPARM* Buffer			= (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);
		BYTE		outSize			= *(BYTE*)MXGetValue (pmessage, "Size", 1);
		BYTE*		outData			= (BYTE*)Buffer->BufferContent;
		
		pclient = EMVGetClientFromCom (pemv, pcom);
		EMVTraceAPDU (pclient, Cla, Ins, P1, P2, outData, outSize, 0);
		free (Buffer->BufferContent);
		free (Buffer);
	}
	return 1;
}


int OnRecvACSecond (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	BYTE Cla					= *(BYTE*)MXGetValue(pmessage, "Cla", 1) ;
	BYTE Ins					= *(BYTE*)MXGetValue(pmessage, "Ins", 1) ;
	BUFFERPARM* Buffer			= (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);
	
	int			outSize			= *(LONG*)MXGetValue (pmessage, "Size", 1);
	BYTE*		outData			= (BYTE*)Buffer->BufferContent;
	

	if (pemv->DebugEnabled)
		EMVTraceAPDU (pclient, 0, 0, 0, 0, outData, outSize, 1);

	EMVOnRecvACSecond (pemv, pclient, outData, outSize);
	free (Buffer->BufferContent);
	free (Buffer);
	EMVTerminateTransaction (pemv, pclient);
	return 1;

}


int OnSendAPDU (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;
	
	if (pemv->DebugEnabled)
	{
		BYTE Cla					= *(BYTE*)MXGetValue(pmessage, "Cla", 1) ;
		BYTE Ins					= *(BYTE*)MXGetValue(pmessage, "Ins", 1) ;
		BYTE P1						= *(BYTE*)MXGetValue(pmessage, "P1", 1) ;
		BYTE P2						= *(BYTE*)MXGetValue(pmessage, "P2", 1) ;
		BUFFERPARM* Buffer			= (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);
		BYTE		outSize			= *(BYTE*)MXGetValue (pmessage, "Size", 1);
		BYTE*		outData			= (BYTE*)Buffer->BufferContent;
		
		EMVTraceAPDU (pclient, Cla, Ins, P1, P2, outData, outSize, 0);
		free (Buffer->BufferContent);
		free (Buffer);
	}
	return 1;

}


int OnRecvAPDU (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	BYTE Cla					= *(BYTE*)MXGetValue(pmessage, "Cla", 1) ;
	BYTE Ins					= *(BYTE*)MXGetValue(pmessage, "Ins", 1) ;
	WORD outSize				= *(WORD*)MXGetValue(pmessage, "Size", 1);
	BUFFERPARM* Buffer			= (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);


	BYTE* outData				= (BYTE*)Buffer->BufferContent;
	
	if (pemv->DebugEnabled)
		EMVTraceAPDU (pclient, 0, 0, 0, 0, outData, outSize, 1);

	if (pclient->Step == EMV_STEP_READ_APPLICATION_DATA)
	{
		EMVOnRecvReadApplicationData (pemv, pclient, outData, outSize);
	}
	else
	if (pclient->Step == EMV_STEP_APPLICATION_SELECTION)
	{
		EMVOnRecvApplicationSelection (pemv, pclient, outData, outSize);
	}
	else
	if (pclient->Step == EMV_STEP_CANDIDATE_LIST_CREATION) 
	{
		EMVOnRecvCandidateListCreation (pemv, pclient, outData, outSize);
	}
	free (Buffer->BufferContent);
	free (Buffer);
	return 1;
}



int OnRecvVerify (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;
	
	BUFFERPARM* Buffer			= (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);
	BYTE		outSize			= (BYTE)Buffer->BufferSize;
	BYTE*		outData			= (BYTE*)Buffer->BufferContent;
	
	if (pemv->DebugEnabled)
		EMVTraceAPDU (pclient, 0, 0, 0, 0, outData, outSize, 1);
	
	EMVOnRecvVerify (pemv, pclient, outData, outSize);

	return 1;
}


int OnRecvError (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;
	STRING strError = (STRING)MXGetValue (pmessage, "Error", 1);
	printf ("Error from client %s\n", strError);
	return 1;
}

//CB2A

int OnRecvReply (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;
	BYTE*		Identifier =  (BYTE*)MXGetValue(pmessage, "Identifier", 1) ;
	BUFFERPARM* Buffer	   = (BUFFERPARM *)MXGetValue (pmessage, "Buffer", 1);
	
	if (pemv->DebugEnabled)
	{
		printf("Receiving Response From Acquirer\n");
	}
	EMVScriptProcessing (pemv, pclient);
	
	free (Buffer->BufferContent);
	free (Buffer);

	return 1;
}

int OnRecvAppliSelection (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;
	CHAR		Index =  *(CHAR*)MXGetValue(pmessage, "Index", 1) ;
	
	EMVSelectApplication (pemv, pclient, (int)Index);
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////////

int OnLoadAcceptor (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVAcceptor* pAcceptor;
	EMVAcquirer* pAcquirer;
	EMV* pemv = (EMV*)applicationfield;

	if (*(BYTE*)MXGetValue (pmessage, "ParOpen",  1) != '[') return 1;	
	
	pAcceptor = EMVInitAcceptor (pemv);
	pAcquirer = EMVInitAcquirer (pemv);



	strncpy (pAcceptor->SIRET, (char*)MXGetValue (pmessage, "SIRET",  1), 14);
	strncpy (pAcceptor->SystemAcceptationIdentification, (char*)MXGetValue (pmessage, "SystemAcceptationIdentification",  1), 8);
	strncpy (pAcceptor->MerchantIdentifier, (char*)MXGetValue (pmessage, "MerchantIdentifier",  1), 15);
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "MerchantCategoryCode",  1),		4, 2, "b", pAcceptor->MerchantCategoryCode);
	strncpy (pAcceptor->MerchantContractNumber, (char*)MXGetValue (pmessage, "MerchantContractNumber",  1), 7);
	strncpy (pAcceptor->TerminalIdentification, (char*)MXGetValue (pmessage, "TerminalIdentification",  1), 8);

	strncpy (pAcceptor->MerchantNameAndLocation, (char*)MXGetValue (pmessage, "MerchantNameAndLocation",  1), 40);  // variable should end with 0
	
	CharArrayToBCDArray ((char*)MXGetValue (pmessage, "AcquirerIdentifier",  1),		11, 11, "n", pAcquirer->AcquirerIdentifier);
	pemv->pAcceptor = pAcceptor;
	pemv->pAcquirer = pAcquirer;

	return 1;
}
int OnLoadApplications (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	BYTE	AID[16];
	BOOL    found = FALSE;
	int		AIDLen = 0;

	char	sPix[23];
	char	sRid[11];
	char	sAid[33];
	char	sPriority[2];
	int		j;
	int		i = 0;


	EMVApplication* pApplication;
	EMV* pemv = (EMV*)applicationfield;

	if (*(BYTE*)MXGetValue (pmessage, "ParOpen",  1) != '[') return 1;

	memset (sRid, 0, 11);
	memset (sPix, 0, 23);
	memset (sAid, 0, 33);
	memset (sPriority, 0, 2);

	strncpy (sRid, (char*)MXGetValue (pmessage, "RID",  1), 10);
	strncpy (sPix, (char*)MXGetValue (pmessage, "PIX",  1), 21);

	strcat (sAid, sRid);
	strcat (sAid, sPix);

	while (i < 32 && sAid[i] != ' ') i++;
	AIDLen = (i + 1) / 2;
	
	CharArrayToHexaArray (sAid,	i, AIDLen, "b", AID);

	pApplication = EMVGetApplicationFromAID (pemv, AID);
	if (!pApplication)
	{
		pApplication = EMVInitApplication (pemv);
		memcpy(pApplication->RID, AID, 5);
		EMVAddApplication (pemv, pApplication);
	}
	

	for (j = 0; j < pApplication->AIDCount; j++)
		if (pApplication->AID[j].Length == AIDLen && memcmp (pApplication->AID[j].AID, AID, AIDLen) == 0)
			found = TRUE;
	if (!found)
	{
		memcpy (pApplication->AID[j].AID, AID, AIDLen);
		pApplication->AID[j].Length = AIDLen;
        pApplication->AID[j].applicationSelectionIndicator = 1;
		j = pApplication->AIDCount;
		pApplication->AIDCount++;
	}

	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "ApplicationVersionNumber",  1),	4, 2, "b", pApplication->AID[j].ApplicationVersionNumber);
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "Priority",  1),	2, 1, "b",	sPriority);
	pApplication->AID[j].Priority = (BYTE)sPriority[0];
	pApplication->AID[j].ForceTransaction = *(BYTE*)MXGetValue (pmessage, "ForceTransaction",  1);

/*


	memcpy(visa->TransactionReferenceCurrency, "x09\x78", 2);
	visa->TransactionReferenceCurrencyExponent = 2;
	memcpy(cb->TransactionReferenceCurrency, "x09\x78", 2);
	cb->TransactionReferenceCurrencyExponent = 2;
	memcpy(mc->TransactionReferenceCurrency, "x09\x78", 2);
	mc->TransactionReferenceCurrencyExponent = 2;
	*/
	return 1;
}

int OnLoadTacs (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	BYTE	AID[16];
	BOOL    found = FALSE;
	int		AIDLen = 0;

	char	sPix[23];
	char	sRid[11];
	char	sAid[33];

	int		j;
	int		i = 0;


	EMVApplication* pApplication;
	EMV* pemv = (EMV*)applicationfield;

	if (*(BYTE*)MXGetValue (pmessage, "ParOpen",  1) != '[') return 1;


	memset (sRid, 0, 11);
	memset (sPix, 0, 23);
	memset (sAid, 0, 33);


	
	strncpy (sRid, (char*)MXGetValue (pmessage, "RID",  1), 10);
	strncpy (sPix, (char*)MXGetValue (pmessage, "PIX",  1), 21);

	strcat (sAid, sRid);
	strcat (sAid, sPix);
	while (i < 32 && sAid[i] != ' ') i++;
	AIDLen = (i + 1) / 2;
	
	CharArrayToHexaArray (sAid,	i, AIDLen, "b", AID);

	pApplication = EMVGetApplicationFromAID (pemv, AID);
	if (!pApplication)
	{
		pApplication = EMVInitApplication (pemv);
		memcpy(pApplication->RID, AID, 5);
		EMVAddApplication (pemv, pApplication);
	}
	

	for (j = 0; j < pApplication->AIDCount; j++)
		if (pApplication->AID[j].Length == AIDLen && memcmp (pApplication->AID[j].AID, AID, AIDLen) == 0)
		{
			found = TRUE;
			break;
		}
	if (!found)
	{
		memcpy (pApplication->AID[j].AID, AID, AIDLen);
		pApplication->AID[j].Length = AIDLen;
		j = pApplication->AIDCount;
		pApplication->AIDCount++;
	}

	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "TerminalActionCodeDenial",  1),	10, 5, "b", pApplication->AID[j].TerminalActionCodeDenial);
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "TerminalActionCodeOnline",  1),	10, 5, "b", pApplication->AID[j].TerminalActionCodeOnline);
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "TerminalActionCodeDefault",  1),10, 5, "b", pApplication->AID[j].TerminalActionCodeDefault);
	return 1;
}

int OnLoadExceptionCards (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVExceptionCard* pExceptionCard;
	EMV* pemv = (EMV*)applicationfield;

	if (*(BYTE*)MXGetValue (pmessage, "ParOpen",  1) != '[') return 1;

	pExceptionCard = (EMVExceptionCard*)malloc (sizeof (EMVExceptionCard));


	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "Pan",  1),		20, 10, "b", pExceptionCard->Pan);
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "Status",  1),	1,  1,  "b", &pExceptionCard->Status);

	ListInsert (&pemv->ExceptionCardList, pExceptionCard);

	return 1;
}

int OnLoadCurrencies (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	BYTE FirstChar;

	EMV* pemv = (EMV*)applicationfield;
	FirstChar = *(BYTE*)MXGetValue (pmessage, "ParOpen",  1);

	if (*(BYTE*)MXGetValue (pmessage, "ParOpen",  1) != '[') return 1;

	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "CurrencyExponent",  1),		1, 1, "b", &pemv->ApplicationCurrencyExponent);
	CharArrayToBCDArray ((char*)MXGetValue (pmessage, "CurrencyCode",  1),	3,  3, "n",			pemv->ApplicationCurrencyCode);

	return 1;
}


int OnLoadRangeBins (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	int i = 0;
	EMVRangeBin* pRangeBin;
	EMV* pemv = (EMV*)applicationfield;
	

	if (*(BYTE*)MXGetValue (pmessage, "ParOpen",  1) != '[') return 1;

	pRangeBin = (EMVRangeBin*)malloc (sizeof (EMVRangeBin));

	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "FromBin",  1),			19, 10, "b",	pRangeBin->FromBin);
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "ToBin",  1),			19, 10, "b",pRangeBin->ToBin);
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "Status",  1),			2,  1, "b",	&pRangeBin->Status);
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "TreatmentCode",  1),	2,  1, "b",	&pRangeBin->TreatmentCode);

	ListInsert (&pemv->RangBinList, pRangeBin);

	return 1;
}

int OnLoadAuthorityPublicKeys (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	char	sRid[11];
	BYTE	RID[5];
	EMVAuthorityPublicKey* pAuthorityPublicKey;
	EMV* pemv = (EMV*)applicationfield;
	EMVApplication* pApplication;

	if (*(BYTE*)MXGetValue (pmessage, "ParOpen",  1) != '[') return 1;

	pAuthorityPublicKey = (EMVAuthorityPublicKey*)malloc (sizeof (EMVAuthorityPublicKey));

	memset (sRid, 0, 11);

	strncpy (sRid, (char*)MXGetValue (pmessage, "RID",  1), 10);
	CharArrayToHexaArray (sRid,	10, 5, "b", RID);

	pApplication = EMVGetApplicationFromRID (pemv, RID);
	if (!pApplication)
	{
		pApplication = EMVInitApplication (pemv);
		memcpy(pApplication->RID, RID, 5);
		EMVAddApplication (pemv, pApplication);
	}


	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "CertificationAuthorityPublicKeyIndex",  1) + 4, 2, 1, "b", &pAuthorityPublicKey->CertificationAuthorityPublicKeyIndex);
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "IssuerPublicKeyExponent",  1),	6,  3, "b",	pAuthorityPublicKey->IssuerPublicKeyExponent);
	pAuthorityPublicKey->IssuerPublicKeyModulusSize = atoi ((char*)MXGetValue (pmessage, "IssuerPublicKeyModulusSize",  1));
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "IssuerPublicKeyModulus",  1),	512,  256, "b",	pAuthorityPublicKey->IssuerPublicKeyModulus);
	CharArrayToHexaArray ((char*)MXGetValue (pmessage, "IssuerPublicKeyRemainder",  1),	256,  128, "b",	pAuthorityPublicKey->IssuerPublicKeyRemainder);
	ListInsert (&pApplication->AuthorityPublicKeys, pAuthorityPublicKey);

	return 1;

}
