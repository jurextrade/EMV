#define _CRT_SECURE_NO_WARNINGS
#include "Card.h"

#define REPEAT_TEST 1 
#define MAX_ATR_SIZE			33

int t = 0;

BYTE transactiontype;
char amount[100];
char currency[4];
  

CC* CCInit(MX* pmx)
{
	CC* pcc = (CC*)malloc(sizeof(CC));
	memset(pcc, 0, sizeof(CC));
	  
	pcc->pMX = pmx;
	pcc->pRouterCom = NULL;

	pcc->pCardContext = CardContext_Init();
	if (pcc == NULL)
	{
		return NULL;
	}
	EMVReadApduErrorFile(pcc->pCardContext);
	return pcc;
}


void CCEnd(CC* pcc)
{
	CardContext_End(pcc->pCardContext);
	free(pcc);
}

CardContext* CardContext_Init()
{
	LONG            lReturn;
	SCARDCONTEXT    hSC;

	CardContext* pCardContext = (CardContext*)malloc(sizeof(CardContext));

	lReturn = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hSC);

	if (SCARD_S_SUCCESS != lReturn)
	{
		free(pCardContext);
		s_printf(smessage, "%s", "Failed SCardEstablishContext\n");
		s_printf(smessage, "%s\n", CardStrError(lReturn));
		return NULL;
	}

	pCardContext->hContext = hSC;
	pCardContext->ReadersCount = 0;
	pCardContext->pReaders = NULL;
	pCardContext->TimeOut = INFINITE;
	pCardContext->pCurrentCard = NULL;
	pCardContext->ShouldRelase = 0;
	pCardContext->appApduErrors = NULL;

	return pCardContext;
}

LONG CardContext_End(CardContext* pCardContext)
{
	LONG            lReturn;
	lReturn = SCardReleaseContext(pCardContext->hContext);

	if (SCARD_S_SUCCESS != lReturn)
	{
		s_printf(smessage, "%s", "Failed SCardReleaseContext\n");
		return lReturn;
	}
	if (pCardContext->pCurrentCard) {
		free(pCardContext->pCurrentCard);
		pCardContext->pCurrentCard = NULL;
	}

	return SCARD_S_SUCCESS;
}


int Card_Init(CardContext* pCardContext, CARD* pCard)
{
	SCARDHANDLE		hCardHandle;
	DWORD           dwAP;
	LONG            lReturn;
	BYTE            MediaType;
	lReturn = SCardConnect(pCardContext->hContext, pCardContext->pCurrentState->szReader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCardHandle, &dwAP);

	if (SCARD_S_SUCCESS != lReturn)
	{
		lReturn = SCardReconnect(hCardHandle, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_LEAVE_CARD, &dwAP);

		if (lReturn != SCARD_S_SUCCESS && 
			lReturn != SCARD_E_TIMEOUT)
		{
			s_printf(smessage, "%s\n", CardStrError(lReturn));
			return lReturn;
		}
	}
	// Use the connection.
	// Display the active protocol.
	switch (dwAP)
	{

		case SCARD_PROTOCOL_T0:
			MediaType = EMV_POS_ENTRY_MODE_ICC_CVV_can_be_checked;
			s_printf(smessage, "%s", "Active protocol T=0\n");
			break;

		case SCARD_PROTOCOL_T1:
			MediaType = EMV_POS_ENTRY_MODE_contactless;
			s_printf(smessage, "%s", "Active protocol T=1\n");
			break;

		default:
			MediaType = EMV_POS_ENTRY_MODE_Unknown;
			s_printf(smessage, "%s", "Active protocol unnegotiated or unknown\n");
			break;
	}

	pCard->pCom					= NULL;
	pCard->strATR[0]			= 0;
	pCard->hCardHandle			= hCardHandle;
	pCard->MediaType			= MediaType;
	pCard->Request.dwProtocol	= dwAP;
	pCard->Request.cbPciLength	= sizeof(pCard->Request);
	pCardContext->pCurrentCard  = pCard;
	return lReturn;
}

void Card_End(CARD* pCard)
{
	//	SCardFreeMemory(pCard->hContext, CurrentCard->State->szReader);
	//  free (CurrentCard->Readers);
//	free(CurrentCard);
}



int Readers_Init(CardContext* pCardContext)
{
	LONG            lReturn;
	DWORD           cch = SCARD_AUTOALLOCATE;

	lReturn = SCardListReaders(pCardContext->hContext, SCARD_ALL_READERS, (LPTSTR)&pCardContext->pReaders, &cch);

	if (lReturn != SCARD_S_SUCCESS || *pCardContext->pReaders == '\0')
	{
		s_printf(smessage, "%s\n", CardStrError(lReturn));
		s_printf(smessage, "%s", "Plug In a Reader and type a char\n>");
		return -1;
	}

	LPTSTR          pReader = pCardContext->pReaders;

	while (*pReader != '\0')
	{
		SCARD_READERSTATE		State;
		State.dwCurrentState	= SCARD_STATE_UNAWARE;
		State.dwEventState		= SCARD_STATE_UNKNOWN;				
		State.szReader			= pReader;
		s_printf(smessage, "Reader: %s\n", pReader);
		pReader = pReader + strlen(pReader) + 1;

		pCardContext->ReadersStates[pCardContext->ReadersCount] = State;
		pCardContext->ReadersCount++;
	}
	pCardContext->pCurrentState = &pCardContext->ReadersStates[0];
	return 0;
}

int OnCardConnected (CC* pcc)
{
	DWORD           dwState, dwProtocol;
	DWORD           cch;
	DWORD           cByte = 32;
	BYTE		    bAttr[32];

	CardContext*	pCardContext = pcc->pCardContext;

	CARD* pCard = pCardContext->pCurrentCard == NULL ? (CARD*)malloc(sizeof(CARD)) : pCardContext->pCurrentCard;

	LONG            lReturn;
	
	
	Send_Start(EMVRooterCom);

	lReturn = Card_Init(pCardContext, pCard);  

	if (lReturn != SCARD_S_SUCCESS) {
		return lReturn;
	}

	cch = 200;

// connect inserted card 
// Determine the status.
// hCardHandle was set by an earlier call to SCardConnect.

	lReturn = SCardStatus(pCard->hCardHandle, pCardContext->pReaders, &cch, &dwState, &dwProtocol, (LPBYTE)&bAttr, &cByte);

	if (SCARD_S_SUCCESS != lReturn && lReturn != SCARD_E_TIMEOUT)
	{
		s_printf(smessage, "%s\n", CardStrError(lReturn));
		return lReturn;
	}

// Examine retrieved status elements.
// Look at the reader name and card state.

	printf("%s\n", pCardContext->pCurrentState->szReader);
	switch (dwState)
	{
		case SCARD_ABSENT:
			s_printf(smessage, "%s", "Card absent.\n");
			break;
		case SCARD_PRESENT:
			s_printf(smessage, "%s", "Card present.\n");
			break;
		case SCARD_SWALLOWED:
			s_printf(smessage, "%s", "Card swallowed.\n");
			break;
		case SCARD_POWERED:
			s_printf(smessage, "%s", "Card has power.\n");
			break;
		case SCARD_NEGOTIABLE:
			s_printf(smessage, "%s", "Card reset and waiting PTS negotiation.\n");
			break;
		case SCARD_SPECIFIC:
			s_printf(smessage, "%s", "The card has been reset and specific communication protocols have been established.\n");
			break;
		default:
			s_printf(smessage, "%s", "Unknown or unexpected card state.\n");
			break;
		}
// look ATR
	unsigned char			strAtr[200];
	strAtr[0] = 0;

	if ((int)CardCorrectATR(bAttr, cByte))
	{

		memset(strAtr, 0, sizeof(strAtr));
		ByteArrayToHexStr(bAttr, 0, cByte, strAtr);

		s_printf(smessage, "ATR ok : %s \n", (char*)strAtr);
		strcpy(pCard->strATR, (char*)strAtr);
	}
	else
	{
		s_printf(smessage, "%s", "ATR wrong.\n");
		//return -1;
	}


	if (!Connect_EMVServer(pcc, pCard))
	{
		s_printf(smessage, "%s", "Can not connect to EMV server \n");
		return -1;
	}

	MXMessage* pmessage;

	printf("Send Userinfo to EMV Server\n");

	pmessage = MXCreateMessage(pCard->pCom->MX, "APDU", "UserInfo");
	MXSetValue(pmessage, "UserName",     1, pcc->UserName);
	MXSetValue(pmessage, "UserPassword", 1, pcc->UserPassword);
	MXSend(pCard->pCom->MX, pCard->pCom, pmessage);



	printf("Send Transaction to EMV Server\n");

	pmessage = MXCreateMessage(pCard->pCom->MX, "APDU", "SendTransaction");
	MXSetValue(pmessage, "Type",	 1, &transactiontype);
	MXSetValue(pmessage, "Currency", 1, currency);
	MXSetValue(pmessage, "Amount",   1, amount);
	MXSetValue(pmessage, "Media",    1, &pCard->MediaType);
	MXSend(pCard->pCom->MX, pCard->pCom, pmessage);


	printf("Send ATR to EMV Server\n");

	pmessage = MXCreateMessage(pCard->pCom->MX, "APDU", "SendATR");
	MXSetValue(pmessage, "Atr", 1, strAtr);
	MXSend(pCard->pCom->MX, pCard->pCom, pmessage);


	return 0;
}

void OnCardDisconnected(CARD* pCard)
{
	if (pCard->pCom)
	{
		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		s_printf(smessage, "%s", "Card was removed\n");// card was disconnected
		pCard->pCom = NULL;
	}
}


char CardAPDU (CARD* pCard, unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2,
            			    unsigned char dataSize, unsigned char* data, int* outDataSize, unsigned char* outData)
{
	BYTE pbRecv[258] = {0};
	DWORD dwRecv = 300;
	BYTE pbSend[256] = {cla, ins, p1, p2, dataSize};

	DWORD dwSend = 5 + dataSize;
	LONG lReturn;

	*outDataSize = 0;

	memcpy(pbSend + 5, data, dataSize);

	lReturn = SCardTransmit(pCard->hCardHandle, &pCard->Request, pbSend, dwSend, NULL, pbRecv, &dwRecv);
	if (SCARD_S_SUCCESS != lReturn)
	{
		s_printf(smessage, "%s\n", CardStrError(lReturn));
		s_printf(smessage, "%s", "Failed SCardTransmit\n");
		return 0;
	}
	if (dwRecv >= 2 && pbRecv[0] == 0x6C)
	{
		BYTE pbSend2[256] = {cla, ins, p1, p2, pbRecv[1]};
		dwSend = 5;
		dwRecv = 258;
		lReturn = SCardTransmit(pCard->hCardHandle, &pCard->Request, pbSend2, dwSend,	NULL, pbRecv, &dwRecv);
		if (SCARD_S_SUCCESS != lReturn)
		{
    		s_printf(smessage, "%s\n", CardStrError(lReturn));
			s_printf(smessage, "%s", "Failed SCardTransmit\n");
			return 0;
		}

	}
	if (dwRecv >= 2 && pbRecv[0] == 0x61)
	{
		BYTE pbSend2[256] = {0x00, 0xC0, 0x00, 0x00, pbRecv[1]};
		DWORD dwSend = 5;
		dwRecv = 258;
		lReturn = SCardTransmit(pCard->hCardHandle, &pCard->Request, pbSend2, dwSend, NULL, pbRecv, &dwRecv);
		if (SCARD_S_SUCCESS != lReturn)
		{
    		s_printf(smessage, "%s\n", CardStrError(lReturn));
			s_printf(smessage, "%s", "Failed SCardTransmit\n");
			return 0;
		}

	}
	memcpy(outData, pbRecv, dwRecv);
	*outDataSize += dwRecv;

	return 1;
}

char CardCorrectATR(unsigned char* bufATR, int size)
{
	if (size < 4)
		return 0;

	if (bufATR[0] != 0x3B && bufATR[0] != 0x3F)
		return 0;

	if ((bufATR[1] >> 4) == 0x06)
	{
		// T0 protocol
		if (bufATR[2] != 0x00)
			return 0;

		return 1;
	}
	else if ((bufATR[1] >> 4) == 0x0E)
	{
		// T1 protocol
		if (size < 9)
			return 0;
		if (bufATR[2] != 0x00)
			return 0;
		if (bufATR[4] != 0x81)
			return 0;
		if (bufATR[5] != 0x31)
			return 0;
		if (bufATR[6] < 0x10 || bufATR[6] > 0xFE)
			return 0;

		return 1;
	}

	// bufATR[1] wrong
	return 0;
}

int OnSendAPDU (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending APDU Answer to Server \n");
	return 1;

}

int OnSendACFirst (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending Generate AC First Answer to Server \n");
	return 1;

}

int OnSendACSecond (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending Generate AC Second Answer to Server \n");
	return 1;

}

int OnSendVerify (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending Verify Answer to Server \n");
	return 1;

}


int OnSendTransaction (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending Transaction  to Server \n");
	return 1;

}

int OnSendATR (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending ATR  to Server \n");
	return 1;

}

int OnRecvSendCommand (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{

	int outDataSize;
	unsigned char outData[258];
	char strError[250];
	BUFFERPARM OutBuffer;
	MXMessage* Outpmessage;
    CARD* pCard = (CARD*)applicationfield;

	BYTE	p1   = *(BYTE *)MXGetValue (pmessage, "P1", 1);
	BYTE	p2   = *(BYTE *)MXGetValue (pmessage, "P2", 1);

	s_printf(smessage, "%s", "Receiving APDU Command from Server \n");
	
	Send_APDU(EMVRooterCom, 0x00, 0xCA, p1, p2, 0, outData, 0);

	if (!CardAPDU (pCard, 0x00, 0xCA, p1, p2, 0, (unsigned char*)"", &outDataSize, outData))
	{
		sprintf(strError, "%s", "APDU failed, transmission error\n");
		
		Outpmessage = MXPutMessage (pcom, "APDU", "SendError");
		MXSetValue (Outpmessage, "Error", 1, strError);

		MXCloseCom (pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	// Response data must at least have SW1 SW2
	if (outDataSize < 2)
	{
		sprintf(strError, "%s", "APDU failed, wrong size\n");
		
		Outpmessage = MXPutMessage (pcom, "APDU", "SendError");
		MXSetValue (Outpmessage, "Error", 1, strError);
		
		MXCloseCom (pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	s_printf(smessage, "%s", "Response APDU From Card \n");

	OutBuffer.BufferType	= 'B';
	OutBuffer.BufferSize	= outDataSize;
	OutBuffer.BufferContent = (char*)&outData;

	Outpmessage = MXPutMessage (pcom, "APDU", "RecvCommand");
	MXSetValue (Outpmessage, "Size",    1, &outDataSize);
	MXSetValue (Outpmessage, "Data",    1, &OutBuffer);
    MXSetValue (Outpmessage, "P1",		1, &p1) ;
    MXSetValue (Outpmessage, "P2",		1, &p2) ;

	Send_APDU(EMVRooterCom, 0x00, 0xCA, p1, p2, outDataSize, outData, 1);

	return 1;
}


int OnRecvAPDU (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	int outDataSize;
	unsigned char outData[2000];
	char strError[250];
	
	MXMessage* Outpmessage;
	BUFFERPARM OutBuffer;
    CARD* pCard = (CARD*)applicationfield;


	BYTE	Cla			= *(BYTE *)MXGetValue (pmessage, "Cla", 1);
	BYTE	Ins			= *(BYTE *)MXGetValue (pmessage, "Ins", 1);
	BYTE	P1			= *(BYTE *)MXGetValue (pmessage, "P1", 1);
	BYTE	P2			= *(BYTE *)MXGetValue (pmessage, "P2", 1);
	BYTE	Size		= *(BYTE *)MXGetValue (pmessage, "Size", 1);
	BUFFERPARM* Buffer  = (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);

	s_printf (smessage, "%s", "Receiving APDU Command from Terminal \n");
	Send_APDU (EMVRooterCom, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, 0);

	if (!CardAPDU (pCard, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, &outDataSize, (unsigned char*)&outData))
	{
		sprintf(strError, "%s", "APDU failed, transmission error\n");

		Outpmessage = MXPutMessage (pcom, "APDU", "SendError");
		MXSetValue (Outpmessage, "Error", 1, strError);

		MXCloseCom (pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	// Response data must at least have SW1 SW2
	if (outDataSize < 2)
	{
		sprintf(strError, "%s", "APDU failed, wrong size\n");
		
		Outpmessage = MXPutMessage (pcom, "APDU", "SendError");
		MXSetValue (Outpmessage, "Error", 1, strError);
		
		MXCloseCom (pCard->pCom->MX, pCard->pCom);
		return 1;
	}

	s_printf(smessage, "%s", "Response APDU From Card \n");

	OutBuffer.BufferType	= 'B';
	OutBuffer.BufferSize    = outDataSize ;
	OutBuffer.BufferContent = (char*)&outData;
	
	Outpmessage = MXPutMessage (pcom, "APDU", "R-APDU");
	MXSetValue (Outpmessage, "Cla",  1, &Cla);
	MXSetValue (Outpmessage, "Ins",  1, &Ins);
	MXSetValue (Outpmessage, "Size", 1, &outDataSize);
	MXSetValue (Outpmessage, "Data", 1, &OutBuffer);

    free (Buffer->BufferContent);
    free (Buffer);

	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, outDataSize, outData, 1);
	
	return 1;
}


int OnRecvACFirst (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{

	int outDataSize;
	unsigned char outData[258];
	char strError[250];
	
	MXMessage* Outpmessage;
	BUFFERPARM OutBuffer;
    CARD* pCard = (CARD*)applicationfield;


	BYTE	Cla  = *(BYTE *)MXGetValue (pmessage, "Cla", 1);
	BYTE	Ins  = *(BYTE *)MXGetValue (pmessage, "Ins", 1);
	BYTE	P1   = *(BYTE *)MXGetValue (pmessage, "P1", 1);
	BYTE	P2   = *(BYTE *)MXGetValue (pmessage, "P2", 1);
	BYTE	Size = *(BYTE *)MXGetValue (pmessage, "Size", 1);
	BUFFERPARM* Buffer = (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);

	s_printf(smessage, "%s", "Receiving GENERATE AC FIRST Command from Server \n");
	
	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, 0);

	if (!CardAPDU (pCard, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, &outDataSize, outData))
	{
		sprintf(strError, "%s", "APDU failed, transmission error\n");

		Outpmessage = MXPutMessage (pcom, "APDU", "SendError");
		MXSetValue (Outpmessage, "Error", 1, strError);

		MXCloseCom (pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	// Response data must at least have SW1 SW2
	if (outDataSize < 2)
	{
		sprintf(strError, "%s", "APDU failed, wrong size\n");
		
		Outpmessage = MXPutMessage (pcom, "APDU", "SendError");
		MXSetValue (Outpmessage, "Error", 1, strError);
		
		MXCloseCom (pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	s_printf(smessage, "%s", "Response APDU From Card \n");


	OutBuffer.BufferType	= 'B';
	OutBuffer.BufferSize	= outDataSize;
	OutBuffer.BufferContent = (char*)&outData;

	Outpmessage = MXPutMessage (pcom, "APDU", "RecvACFirst");
	MXSetValue (Outpmessage, "Cla",  1, &Cla);
	MXSetValue (Outpmessage, "Ins",  1, &Ins);
	MXSetValue (Outpmessage, "Size", 1, &outDataSize);
	MXSetValue (Outpmessage, "Data", 1, &OutBuffer);

	free(Buffer->BufferContent);
	free(Buffer);

	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, outDataSize, outData, 1);

	return 1;
}

int OnRecvACSecond (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{

	int outDataSize;
	unsigned char outData[258];
	char strError[250];
	
	MXMessage* Outpmessage;
	BUFFERPARM OutBuffer;
    CARD* pCard = (CARD*)applicationfield;


	BYTE	Cla  = *(BYTE *)MXGetValue (pmessage, "Cla", 1);
	BYTE	Ins  = *(BYTE *)MXGetValue (pmessage, "Ins", 1);
	BYTE	P1   = *(BYTE *)MXGetValue (pmessage, "P1", 1);
	BYTE	P2   = *(BYTE *)MXGetValue (pmessage, "P2", 1);
	BYTE	Size = *(BYTE *)MXGetValue (pmessage, "Size", 1);
	BUFFERPARM* Buffer = (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);

	printf("Receiving GENERATE AC SECOND Command from Server \n");

	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, 0);

	if (!CardAPDU (pCard, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, &outDataSize, outData))
	{
		sprintf(strError, "%s", "APDU failed, transmission error\n");
		
		Outpmessage = MXPutMessage (pcom, "APDU", "SendError");
		MXSetValue (Outpmessage, "Error", 1, strError);
		
		MXCloseCom (pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	// Response data must at least have SW1 SW2
	if (outDataSize < 2)
	{
		sprintf(strError, "%s", "APDU failed, wrong size\n");
		
		Outpmessage = MXPutMessage (pcom, "APDU", "SendError");
		MXSetValue (Outpmessage, "Error", 1, strError);
		
		MXCloseCom (pCard->pCom->MX, pCard->pCom);
		return 1;
	}


	OutBuffer.BufferType    = 'B';
	OutBuffer.BufferSize    = outDataSize;
	OutBuffer.BufferContent = (char*)&outData;

	Outpmessage = MXPutMessage (pcom, "APDU", "RecvACSecond");
	MXSetValue (Outpmessage, "Cla",  1, &Cla);
	MXSetValue (Outpmessage, "Ins",  1, &Ins);
	MXSetValue (Outpmessage, "Size", 1, &outDataSize);
	MXSetValue (Outpmessage, "Data", 1, &OutBuffer);

	free(Buffer->BufferContent);
	free(Buffer);

	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, outDataSize, outData, 1);

	return 1;
}

int OnRecvVerify (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	BUFFERPARM OutBuffer;
	int outDataSize;
	unsigned char outData[258];
	char strError[250];
	char c;
	unsigned char PinCode[10];
	MXMessage* Outpmessage;
    CARD* pCard = (CARD*)applicationfield;
	BYTE	Enciphered  = *(BYTE *)MXGetValue (pmessage, "Enciphered", 1);
	int i = 0;

	memset (PinCode, 0xFF, 10);
	
	s_printf(smessage, "%s", "Receiving Verify Command from Server \n");

	fseek(stdin,0,SEEK_END);
	printf(smessage, "Enter Pin Code : ");
	while ((c = getchar()) != '\n')
	{
		PinCode[i] = c;
		i++;
	}		

	if (!CardAPDU (pCard, 0x00, 0x20, 0x00, 0x08, 0x08, PinCode, &outDataSize, outData))
	{
		sprintf(strError, "%s", "APDU failed, Verify\n");

		Outpmessage = MXPutMessage (pcom, "APDU", "SendError");
		MXSetValue (Outpmessage, "Error", 1, strError);
		
		MXCloseCom (pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	// Response data must at least have SW1 SW2
	if (outDataSize < 2)
	{
		sprintf(strError, "%s", "APDU Verify failed, wrong size\n");
		
		Outpmessage = MXPutMessage (pcom, "APDU", "SendError");
		MXSetValue (Outpmessage, "Error", 1, strError);
		
		MXCloseCom (pCard->pCom->MX, pCard->pCom);
		return 1;
	}

	OutBuffer.BufferType	= 'B';
	OutBuffer.BufferSize	= outDataSize;
	OutBuffer.BufferContent = (char*)&outData;

	Outpmessage = MXPutMessage (pcom, "APDU", "RecvVerify");
	MXSetValue (Outpmessage, "Size",  1, &outDataSize);
	MXSetValue (Outpmessage, "Data",  1, &OutBuffer);

	return 1;
}

int OnRecvSendAppliSelection (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	int idx;
	char charindex;
	MXMessage* Outpmessage;

	BYTE	count  = *(BYTE *)MXGetValue (pmessage, "Count", 1);
	STRING  label  = (STRING)MXGetValue (pmessage, "Label", 1);

	if (count == 1)
	{
		s_printf(smessage, "Confirm select app %s (y/n): " , label);
		charindex = getchar();
		if (charindex != 'y')
			charindex = -1; 
		else
			charindex = 0; 
	}
	else
	{
		s_printf(smessage, "%s", "Select application from list:\n");

		for (idx = 0; idx < count; idx++)
		{
			BYTE	priority = *(BYTE*)MXGetValue(pmessage, "Priority", idx + 1);
			label = (STRING)MXGetValue(pmessage, "Label", idx + 1);
			printf("%d (priority %d) - %s\n", idx, priority, label);
		}
		s_printf(smessage, "%s", "Index: ");
		fseek(stdin, 0, SEEK_END);
		charindex = getchar() - '0';
	}

	Outpmessage = MXPutMessage(pcom, "APDU", "RecvAppliSelection");
	MXSetValue (Outpmessage, "Index", 1, &charindex);	
	
	return 1;
}

int CardRead ()
{
	SCARDHANDLE			hCardHandle;
	SCARDCONTEXT		hContext;
	SCARD_READERSTATE_A rgReaderStates[1];
	unsigned long		dwReaderLen, dwState, dwProt, dwAtrLen;

	unsigned long		dwPref, dwReaders;
	char				*pcReaders, *mszReaders;
	unsigned char		pbAtr[MAX_ATR_SIZE];
	const char			*mszGroups;
	long				rv;
	int					i, p, iReader;
	int					iList[16];

	// int t = 0;

	s_printf(smessage, "%s", "\nMUSCLE PC/SC Lite Test Program\n\n");

	s_printf(smessage, "%s", "Testing SCardEstablishContext    : ");
	rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);

	s_printf(smessage, "%s\n", CardStrError(rv));

	if (rv != SCARD_S_SUCCESS)
	{
		return -1;
	}

	s_printf(smessage, "%s", "Testing SCardGetStatusChange \n");
	s_printf(smessage, "%s", "Please insert a working reader   : ");
	
	rv = SCardGetStatusChange(hContext, INFINITE, 0, 0);

	s_printf(smessage, "%s\n", CardStrError(rv));

	if (rv != SCARD_S_SUCCESS)
	{
		SCardReleaseContext(hContext);
		return -1;
	}

	s_printf(smessage, "%s", "Testing SCardListReaders         : ");

	mszGroups = 0;
	rv = SCardListReaders(hContext, mszGroups, 0, &dwReaders);

	s_printf(smessage, "%s\n", CardStrError(rv));

	if (rv != SCARD_S_SUCCESS)
	{
		SCardReleaseContext(hContext);
		return -1;
	}

	mszReaders = (char *) malloc(sizeof(char) * dwReaders);
	rv = SCardListReaders(hContext, mszGroups, mszReaders, &dwReaders);

	if (rv != SCARD_S_SUCCESS)
	{
		SCardReleaseContext(hContext);
		return -1;
	}

	/*
	 * Have to understand the multi-string here 
	 */
	p = 0;
	for (i = 0; i < (int)dwReaders - 1; i++)
	{
		++p;
		printf("Reader %02d: %s\n", p, &mszReaders[i]);
		iList[p] = i;
		while (mszReaders[++i] != 0) ;
	}

#ifdef REPEAT_TEST
	if (t == 0)
	{
#endif

		do
		{
			s_printf(smessage, "%s", "Enter the reader number          : ");
			

			scanf("%d", &iReader);
			s_printf(smessage, "%s", "\n");

			if (iReader > p || iReader <= 0)
			{
				s_printf(smessage, "%s", "Invalid Value - try again\n");
			}
		}
		while (iReader > p || iReader <= 0);

#ifdef REPEAT_TEST
		t = 1;
	}
#endif

	rgReaderStates[0].szReader = &mszReaders[iList[iReader]];
	rgReaderStates[0].dwCurrentState = SCARD_STATE_EMPTY;

	s_printf(smessage, "%s", "Waiting for card insertion         \n");
	rv = SCardGetStatusChange(hContext, INFINITE, rgReaderStates, 1);

	s_printf(smessage, "                                 : %s\n",	CardStrError(rv));

	if (rv != SCARD_S_SUCCESS)
	{
		SCardReleaseContext(hContext);
		return -1;
	}

	s_printf(smessage, "%s", "Testing SCardConnect             : ");
	rv = SCardConnect(hContext, &mszReaders[iList[iReader]], SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,	&hCardHandle, &dwPref);

	s_printf(smessage, "%s\n", CardStrError(rv));

	if (rv != SCARD_S_SUCCESS)
	{
		SCardReleaseContext(hContext);
		return -1;
	}

	s_printf(smessage, "%s", "Testing SCardStatus              : ");

	dwReaderLen = 50;
	pcReaders = (char *) malloc(sizeof(char) * 50);

	rv = SCardStatus(hCardHandle, pcReaders, &dwReaderLen, &dwState, &dwProt, pbAtr, &dwAtrLen);

	s_printf(smessage, "%s\n", CardStrError(rv));

	printf("Current Reader Name              : %s\n",  pcReaders);
	printf("Current Reader State             : %lx\n", dwState);
	printf("Current Reader Protocol          : %lx\n", dwProt - 1);
	printf("Current Reader ATR Size          : %lx\n", dwAtrLen);
	printf("Current Reader ATR Value         : %s\n", "");

	for (i = 0; i < (int)dwAtrLen; i++)
	{
		printf("%02X ", pbAtr[i]);
	}
	s_printf(smessage, "%s", "\n");

	if (rv != SCARD_S_SUCCESS)
	{
		SCardDisconnect(hCardHandle, SCARD_RESET_CARD);
		SCardReleaseContext(hContext);
	}

	s_printf(smessage, "%s", "Testing SCardDisconnect          : ");
	rv = SCardDisconnect(hCardHandle, SCARD_UNPOWER_CARD);

	s_printf(smessage, "%s\n", CardStrError(rv));

	if (rv != SCARD_S_SUCCESS)
	{
		SCardReleaseContext(hContext);
		return -1;
	}

	s_printf(smessage, "%s", "Testing SCardReleaseContext      : ");
	rv = SCardReleaseContext(hContext);

	s_printf(smessage, "%s\n", CardStrError(rv));

	if (rv != SCARD_S_SUCCESS)
	{
		return -1;
	}

	s_printf(smessage, "%s",  "\n");
	s_printf(smessage, "%s", "PC/SC Test Completed Successfully !\n");

	return 0;
}


char* CardStrError (const long pcscError)
{
	static char strError[75];

	switch (pcscError)	{
	    case SCARD_S_SUCCESS:
		    (void)strncpy(strError, "Command successful.", sizeof(strError));
		    break;
	    case SCARD_E_CANCELLED:
		    (void)strncpy(strError, "Command cancelled.", sizeof(strError));
		    break;
	    case SCARD_E_CANT_DISPOSE:
		    (void)strncpy(strError, "Cannot dispose handle.", sizeof(strError));
		    break;
	    case SCARD_E_INSUFFICIENT_BUFFER:
		    (void)strncpy(strError, "Insufficient buffer.", sizeof(strError));
		    break;
	    case SCARD_E_INVALID_ATR:
		    (void)strncpy(strError, "Invalid ATR.", sizeof(strError));
		    break;
	    case SCARD_E_INVALID_HANDLE:
		    (void)strncpy(strError, "Invalid handle.", sizeof(strError));
		    break;
	    case SCARD_E_INVALID_PARAMETER:
		    (void)strncpy(strError, "Invalid parameter given.", sizeof(strError));
		    break;
	    case SCARD_E_INVALID_TARGET:
		    (void)strncpy(strError, "Invalid target given.", sizeof(strError));
		    break;
	    case SCARD_E_INVALID_VALUE:
		    (void)strncpy(strError, "Invalid value given.", sizeof(strError));
		    break;
	    case SCARD_E_NO_MEMORY:
		    (void)strncpy(strError, "Not enough memory.", sizeof(strError));
		    break;
	    case SCARD_F_COMM_ERROR:
		    (void)strncpy(strError, "RPC transport error.", sizeof(strError));
		    break;
	    case SCARD_F_INTERNAL_ERROR:
		    (void)strncpy(strError, "Internal error.", sizeof(strError));
		    break;
	    case SCARD_F_UNKNOWN_ERROR:
		    (void)strncpy(strError, "Unknown error.", sizeof(strError));
		    break;
	    case SCARD_F_WAITED_TOO_LONG:
		    (void)strncpy(strError, "Waited too long.", sizeof(strError));
		    break;
	    case SCARD_E_UNKNOWN_READER:
		    (void)strncpy(strError, "Unknown reader specified.", sizeof(strError));
		    break;
	    case SCARD_E_TIMEOUT:
		    (void)strncpy(strError, "Command timeout.", sizeof(strError));
		    break;
	    case SCARD_E_SHARING_VIOLATION:
		    (void)strncpy(strError, "Sharing violation.", sizeof(strError));
		    break;
	    case SCARD_E_NO_SMARTCARD:
		    (void)strncpy(strError, "No smart card inserted.", sizeof(strError));
		    break;
	    case SCARD_E_UNKNOWN_CARD:
		    (void)strncpy(strError, "Unknown card.", sizeof(strError));
		    break;
	    case SCARD_E_PROTO_MISMATCH:
		    (void)strncpy(strError, "Card protocol mismatch.", sizeof(strError));
		    break;
	    case SCARD_E_NOT_READY:
		    (void)strncpy(strError, "Subsystem not ready.", sizeof(strError));
		    break;
	    case SCARD_E_SYSTEM_CANCELLED:
		    (void)strncpy(strError, "System cancelled.", sizeof(strError));
		    break;
	    case SCARD_E_NOT_TRANSACTED:
		    (void)strncpy(strError, "Transaction failed.", sizeof(strError));
		    break;
	    case SCARD_E_READER_UNAVAILABLE:
		    (void)strncpy(strError, "Reader is unavailable.", sizeof(strError));
		    break;
	    case SCARD_W_UNSUPPORTED_CARD:
		    (void)strncpy(strError, "Card is not supported.", sizeof(strError));
		    break;
	    case SCARD_W_UNRESPONSIVE_CARD:
		    (void)strncpy(strError, "Card is unresponsive.", sizeof(strError));
		    break;
	    case SCARD_W_UNPOWERED_CARD:
		    (void)strncpy(strError, "Card is unpowered.", sizeof(strError));
		    break;
	    case SCARD_W_RESET_CARD:
		    (void)strncpy(strError, "Card was reset.", sizeof(strError));
		    break;
	    case SCARD_W_REMOVED_CARD:
		    (void)strncpy(strError, "Card was removed.", sizeof(strError));
		    break;
    //	case SCARD_W_INSERTED_CARD:
    //		(void)strncpy(strError, "Card was inserted.", sizeof(strError));
    //		break;
	    case SCARD_E_UNSUPPORTED_FEATURE:
		    (void)strncpy(strError, "Feature not supported.", sizeof(strError));
		    break;
	    case SCARD_E_PCI_TOO_SMALL:
		    (void)strncpy(strError, "PCI struct too small.", sizeof(strError));
		    break;
	    case SCARD_E_READER_UNSUPPORTED:
		    (void)strncpy(strError, "Reader is unsupported.", sizeof(strError));
		    break;
	    case SCARD_E_DUPLICATE_READER:
		    (void)strncpy(strError, "Reader already exists.", sizeof(strError));
		    break;
	    case SCARD_E_CARD_UNSUPPORTED:
		    (void)strncpy(strError, "Card is unsupported.", sizeof(strError));
		    break;
	    case SCARD_E_NO_SERVICE:
		    (void)strncpy(strError, "Service not available.", sizeof(strError));
		    break;
	    case SCARD_E_SERVICE_STOPPED:
		    (void)strncpy(strError, "Service was stopped.", sizeof(strError));
		    break;
	    case SCARD_E_NO_READERS_AVAILABLE:
		    (void)strncpy(strError, "Cannot find a smart card reader.", sizeof(strError));
		    break;
	    default:
		    (void)strncpy(strError, "Unkown error: 0x%08lX", sizeof(strError));

	}

	strError[sizeof(strError)-1] = '\0';

	return strError;
}



// Check ATR, detect whether ATR apply to emv card
// Return: 1 ok, 0 wrong ATR


/*
if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_UNAWARE) == SCARD_STATE_UNAWARE)
		{
			s_printf(smessage, "%s\n", "SCARD_STATE_UNAWARE");
		}
		if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_IGNORE) == SCARD_STATE_IGNORE)
		{
			s_printf(smessage, "%s\n", "SCARD_STATE_IGNORE");
		}
		if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_UNAVAILABLE) == SCARD_STATE_UNAVAILABLE)
		{
			s_printf(smessage, "%s\n", "SCARD_STATE_UNAVAILABLE");
		}
		else
		if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_ATRMATCH) == SCARD_STATE_ATRMATCH)
		{
			s_printf(smessage, "%s\n", "SCARD_STATE_ATRMATCH");
		}
		else
		if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_EXCLUSIVE) == SCARD_STATE_EXCLUSIVE)
		{
			s_printf(smessage, "%s\n", "SCARD_STATE_EXCLUSIVE");
		}
		else
		if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_INUSE) == SCARD_STATE_INUSE)
		{
			s_printf(smessage, "%s\n", "SCARD_STATE_INUSE");
		}
		if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_MUTE) == SCARD_STATE_MUTE)
		{
			s_printf(smessage, "%s\n", "SCARD_STATE_MUTE");
		}
		if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_UNPOWERED) == SCARD_STATE_UNPOWERED)
		{
			s_printf(smessage, "%s\n", "SCARD_STATE_UNPOWERED");
		}


*/