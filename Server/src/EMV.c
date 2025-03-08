#include "EMV.h"

// Candidate applications

EMVSelectApplicationInfo standartCandidate;
EMVSelectApplicationInfo candidateApplications[MAX_CANDIDATE_APPLICATIONS];
int						candidateApplicationCount;
int						indexApplicationSelected;

#define  EurCode   "x09\x78"
#define  FranceCode "x02\x50"

//=========================================================================================================================================

EMV* EMVInit(MX* pmx)
{
	EMV* pemv = (EMV*)malloc(sizeof(EMV));

	memset(pemv, 0, sizeof(EMV));



	pemv->pMX				= pmx;
	pemv->pRouterCom		= NULL;

	pemv->ApplicationsCount = 0;
	pemv->Applications		= 0;

	pemv->Terminals			= 0;
	pemv->TerminalsCount	= 0;

	memset(&pemv->Settings, 0, sizeof(EMVSettings));
	pemv->Settings.appSelectionUsePSE = 1;
	pemv->Settings.appSelectionSupportConfirm = 1;
	pemv->Settings.appSelectionPartial = 1;
	pemv->Settings.appSelectionSupport = 1;

	EMVSetDebugEnabled(pemv, 1);


	EMVSetFunctionAPDU(pemv, EMVSendAPDU);


	EMVReadTagFile(pemv);				 //TAGS.csv
	EMVReadApduErrorFile(pemv);		     //SW1SW2.csv
	EMVReadFile(pemv);					 //FILES.csv


	EMVLoadAcceptor(pemv);				//SIT_D753.wp
	EMVLoadRangeBins(pemv);			    //APB_D236.wp
	EMVLoadExceptionCards(pemv);		//APL_D253.wp
	EMVLoadAuthorityPublicKeys(pemv);	//EPK_D782.wp

	EMVLoadTerminals(pemv);
	EMVLoadApplications(pemv);			//EPV_D787.wp
	EMVLoadTacs(pemv);					//EPT_D778.wp
	EMVLoadCurrencies(pemv);			//MON_D747.wp



	EMVInitHost(pemv, pemv->pAcquirer, "127.0.0.1", 8000);


	pemv->pCB2A = CB2AInit(1);

	EMVTraceAcceptor(pemv);
	EMVTraceAcquirer(pemv);
	EMVTraceApplications(pemv);

	return pemv;
}


void EMVEnd(EMV* pemv)
{
	if (pemv->DebugEnabled)
	{
		printf("Destroy allocated data...\n");
	}

	while (pemv->Clients)
	{
		EMVClient* pclient = (EMVClient*)pemv->Clients->car;
		EMVEndClient(pemv, pclient);
	}
	while (pemv->Applications)
	{
		EMVApplication* papplication = (EMVApplication*)pemv->Applications->car;
		EMVEndApplication(pemv, papplication);
	}
	while (pemv->Terminals)
	{
		EMVTerminal* pterminal = (EMVTerminal*)pemv->Terminals->car;
		EMVEndTerminal(pemv, pterminal);
	}
	EMVEndAcquirer(pemv, pemv->pAcquirer);

	EMVDestroySettings(pemv);

	CB2AEnd(pemv->pCB2A);

}


void EMVLoadTerminals (EMV* pemv)
{
	EMVTerminal* pTerminal = EMVInitTerminal (pemv);
    EMVTerminal Terminal  = {"12345678", {0x02, 0x50}, {0xC1, 0x00, 0xF0, 0xA0, 0x01}, {0x60, 0xB8, 0xC8}, 0x22, {0x26, 0x40, 0x40, 0x00}};
	*pTerminal = Terminal;
	EMVAddTerminal (pemv, pTerminal);
}

EMVAuthorityPublicKey* EMVGetAuthorityPublicKeyFromKeyIndex (EMV* pemv, EMVClient* pclient, int keyindex)
{

	List* AuthorityPublicKeys = pclient->pApplication->AuthorityPublicKeys;

	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, "Find the issuer public key certificate from issuer with index %d", keyindex);
		s_printf(smessage, pclient, "%s", strace);
		Send_Info(EMVRooterCom, pclient, "INFO", strace);
	}
	
	while (AuthorityPublicKeys)
	{
		EMVAuthorityPublicKey* PublicKey = (EMVAuthorityPublicKey*)AuthorityPublicKeys->car;

		if (PublicKey->CertificationAuthorityPublicKeyIndex == keyindex)
			return PublicKey;
		AuthorityPublicKeys = AuthorityPublicKeys->cdr;
	}
	return NULL;
}

EMVRangeBin* EMVGetRangeBinFromPan (EMV* pemv, EMVClient* pclient, BYTE* pan, int TagPanSize)
{
	List* lRangeBin = pemv->RangBinList;
	

	while (lRangeBin)
	{
		EMVRangeBin* pRangeBin = (EMVRangeBin*)lRangeBin->car;
		if (strncmp ((char*)pan,(char*) pRangeBin->FromBin, TagPanSize) >= 0 &&
			strncmp ((char*)pan, (char*)pRangeBin->ToBin, TagPanSize) >= 0)
			return pRangeBin;
		lRangeBin = lRangeBin->cdr;
	}
	return NULL;
}

EMVExceptionCard* EMVGetExceptionPanFromPan (EMV* pemv, EMVClient* pclient, BYTE* pan, int TagPanSize)
{
	List* HotList = pemv->ExceptionCardList;


	while (HotList)
	{
		EMVExceptionCard*  pexceptionpan = (EMVExceptionCard*)HotList->car;

		if (strncmp ((char*)pan, (char*)pexceptionpan->Pan, TagPanSize) == 0)
			return pexceptionpan;
		HotList = HotList->cdr;
	}
	return NULL;
}

void EMVAddApplication (EMV* pemv, EMVApplication* papplication)
{
	if (!pemv) return;

	ListInsert (&pemv->Applications, papplication);
	pemv->ApplicationsCount++;
}

void EMVAddTerminal (EMV* pemv, EMVTerminal* pTerminal)
{
	if (!pemv) return;

	ListInsert (&pemv->Terminals, pTerminal);
	pemv->TerminalsCount++;
}


void EMVSetTag(EMVClient* pclient, unsigned short tag, BYTE* data, int size) {
	TLVSetTag(pclient->pTLV, tag, data, size);
	Send_Tag(EMVRooterCom, pclient, tag, data, size);
}

void EMVSetAcceptor (EMV* pemv, EMVClient* pclient, EMVAcceptor* pacceptor)
{
	pemv->pAcceptor = pacceptor;
	EMVSetTag(pclient, TAG_SIRET,						(BYTE*)pacceptor->SIRET, 14);
	EMVSetTag(pclient, TAG_IDSA,                        (BYTE*)pacceptor->SystemAcceptationIdentification, 8);
	EMVSetTag(pclient, TAG_MERCHANT_CONTRACT_NUMBER,    (BYTE*)pacceptor->MerchantContractNumber, 7);

	EMVSetTag(pclient, TAG_MERCHANT_CATEGORY_CODE,	pacceptor->MerchantCategoryCode, 2);
	EMVSetTag(pclient, TAG_MERCHANT_IDENTIFIER,		(BYTE*)pacceptor->MerchantIdentifier, strlen(pacceptor->MerchantIdentifier));
	EMVSetTag(pclient, TAG_MERCHANT_NAME_AND_LOCATION,(BYTE*)pacceptor->MerchantNameAndLocation, strlen(pacceptor->MerchantNameAndLocation));
}

void EMVSetAcquirer (EMV* pemv, EMVClient* pclient, EMVAcquirer* pacquirer)
{
	pemv->pAcquirer = pacquirer;
	EMVSetTag(pclient, TAG_ACQUIRER_IDENTIFIER,						(BYTE*)pacquirer->AcquirerIdentifier, 6);
}


void EMVSetPointOfSale (EMV* pemv, EMVClient* pclient, EMVPointOfSale* ppointofsale)
{
	pclient->pPointOfSale = ppointofsale;


	EMVSetTag(pclient, TAG_IDPA,                        (BYTE*)ppointofsale->PointAcceptationIdentification, 8);
	EMVSetTag(pclient, TAG_SA_NUMBER,					(BYTE*)ppointofsale->SystemAcceptationLogicalNumber, 3);
	EMVSetTag(pclient, TAG_PA_NUMBER,					(BYTE*)ppointofsale->PointAcceptationLogicalNumber, 3);


}

void EMVSetTerminal (EMV* pemv, EMVClient* pclient, EMVTerminal* pTerminal)
{
	pclient->pTerminal = pTerminal;

	EMVSetTag(pclient, TAG_TERMINAL_COUNTRY_CODE,				pTerminal->TerminalCountryCode, 2);
	EMVSetTag(pclient, TAG_TERMINAL_CAPABILITIES,				pTerminal->TerminalCapabilities, 3);
	EMVSetTag(pclient, TAG_ADDITIONAL_TERMINAL_CAPABILITIES,	pTerminal->AdditionalTerminalCapabilities, 5);
	EMVSetTag(pclient, TAG_TERMINAL_TYPE,						&pTerminal->TerminalType, 1);
	EMVSetTag(pclient, TAG_IFD_SERIAL_NUMBER,					(BYTE*)pTerminal->IFDSerialNumber, strlen(pTerminal->IFDSerialNumber));
	EMVSetTag(pclient, TAG_TTQ,                                 pTerminal->TTQ, 4);

	if (pemv->DebugEnabled)
	{
		EMVTraceTerminal(pclient);
	}
}

void EMVSetTransaction (EMV* pemv, EMVClient* pclient, EMVTransaction* pTransaction)
{
	pclient->pTransaction = pTransaction;

	EMVSetTag(pclient, TAG_TRANSACTION_CURRENCY_CODE,		pTransaction->TransactionCurrencyCode, 2);
	EMVSetTag(pclient, TAG_TRANSACTION_CURRENCY_EXPONENT,	&pTransaction->TransactionCurrencyExponent, 1);
	EMVSetTag(pclient, TAG_TRANSACTION_TIME,				pTransaction->TransactionTime, 3);
	EMVSetTag(pclient, TAG_TRANSACTION_DATE,				pTransaction->TransactionDate, 3);
	EMVSetTag(pclient, TAG_TRANSACTION_TYPE,				&pTransaction->TransactionType, 1);
	EMVSetTag(pclient, TAG_TRANSACTION_SEQUENCE_COUNTER,	pTransaction->TransactionSequenceCounter, 3);
	EMVSetTag(pclient, TAG_AMOUNT_AUTHORIZED,				pTransaction->AmountAuthorized, 6);
	EMVSetTag(pclient, TAG_AMOUNT_OTHER,					pTransaction->AmountOther, 6);
	EMVSetTag(pclient, TAG_UNPREDICTABLE_NUMBER,			pTransaction->UnpredictableNumber, 4);
	EMVSetTag(pclient, TAG_ICC_DYNAMIC_NUMBER,			pTransaction->ICCDynamicNumber, 4);
	EMVSetTag(pclient, TAG_DATA_AUTHENTIFICATION_CODE,	pTransaction->DataAuthentificationCode, 2);
	EMVSetTag(pclient, TAG_POS_ENTRY_MODE,	            &pTransaction->POSEntryMode, 1);

	if (memcmp(pTransaction->TransactionCurrencyCode, pemv->ApplicationCurrencyCode, 2) != 0)
	{
		pclient->DifferentCurrency = 1;
	}

	if (pemv->DebugEnabled)
	{
		EMVTraceTransaction(pclient);
	}
}

void EMVSetApplication (EMV* pemv, EMVClient* pclient, EMVApplication* pApplication)
{
	pclient->pApplication = pApplication;
	

	EMVSetTag(pclient, TAG_TERMINAL_ACTION_CODE_DEFAULT,			pApplication->AID[pApplication->IndexAIDSelected].TerminalActionCodeDefault, 5);
	EMVSetTag(pclient, TAG_TERMINAL_ACTION_CODE_DENIAL,				pApplication->AID[pApplication->IndexAIDSelected].TerminalActionCodeDenial, 5);
	EMVSetTag(pclient, TAG_TERMINAL_ACTION_CODE_ONLINE,				pApplication->AID[pApplication->IndexAIDSelected].TerminalActionCodeOnline, 5);
	EMVSetTag(pclient, TAG_APPLICATION_VERSION_NUMBER_TERMINAL,		pApplication->AID[pApplication->IndexAIDSelected].ApplicationVersionNumber, 2);
	

	EMVSetTag(pclient, TAG_TERMINAL_FLOOR_LIMIT,						pApplication->TerminalFloorLimit, 4);
	EMVSetTag(pclient, TAG_TERMINAL_IDENTIFICATION,					(BYTE*)pApplication->TerminalIdentification, strlen(pApplication->TerminalIdentification));		
	EMVSetTag(pclient, TAG_TERMINAL_RISK_MANAGEMENT_DATA,				pApplication->TerminalRiskManagementData, pApplication->TerminalRiskManagementDataSize);
	EMVSetTag(pclient, TAG_TRANSACTION_REFERENCE_CURRENCY_CODE,		pApplication->TransactionReferenceCurrency, 2);
	EMVSetTag(pclient, TAG_TRANSACTION_REFERENCE_CURRENCY_EXPONENT,	&pApplication->TransactionReferenceCurrencyExponent, 1);

	if (pemv->DebugEnabled)
	{
		EMVTraceApplication(pemv, pApplication, pApplication->IndexAIDSelected);
		
		int outSize;

		BYTE* EMV_TVR_TACONLINE = (BYTE*)TLVGetTag(pclient->pTLV, TAG_TERMINAL_ACTION_CODE_ONLINE, &outSize);
		BYTE* EMV_TVR_TACDENIAL = (BYTE*)TLVGetTag(pclient->pTLV, TAG_TERMINAL_ACTION_CODE_DENIAL, &outSize);
		BYTE* EMV_TVR_TACDFAULT = (BYTE*)TLVGetTag(pclient->pTLV, TAG_TERMINAL_ACTION_CODE_DEFAULT, &outSize);
		EMVTraceTVR(pclient, (EMV_BITS*)EMV_TVR_TACDENIAL, "TAC_Denial");
		EMVTraceTVR(pclient, (EMV_BITS*)EMV_TVR_TACONLINE, "TAC_Online");
		EMVTraceTVR(pclient, (EMV_BITS*)EMV_TVR_TACDFAULT, "TAC_Default");
	}

}


//=========================================================================================================================================

void EMVSetTSI(EMV* pemv, EMVClient* pclient, unsigned short tsiflag, int set)
{
	int TraceChanged = 0;
	if (!set)
	{
		if ((pclient->EMV_TSI[0] & (tsiflag & 0x00FF)) != 0)
			TraceChanged = 1;
		pclient->EMV_TSI[0] &= ~(tsiflag & 0x00FF);
	}
	else
	{
		if ((pclient->EMV_TSI[0] & (tsiflag & 0x00FF)) == 0)
			TraceChanged = 1;
		pclient->EMV_TSI[0] |= tsiflag & 0x00FF;
	}

	if (TraceChanged && pemv->DebugEnabled)
	{
		EMVTraceTSIAdded(pclient, tsiflag & 0x00FF);
	}

}

void EMVSetTVR(EMV* pemv, EMVClient* pclient, unsigned short tvrflag, int set)
{
	int TraceChanged = 0;
	if (!set)
	{
		if ((pclient->EMV_TVR[tvrflag >> 8] & (tvrflag & 0x00FF)) != 0)
			TraceChanged = 1;
		pclient->EMV_TVR[tvrflag >> 8] &= ~(tvrflag & 0x00FF);
	}
	else
	{
		if ((pclient->EMV_TVR[tvrflag >> 8] & (tvrflag & 0x00FF)) == 0)
			TraceChanged = 1;
		pclient->EMV_TVR[tvrflag >> 8] |= tvrflag & 0x00FF;
	}

	if (TraceChanged && pemv->DebugEnabled)
	{
		EMVTraceTVRAdded(pclient, tvrflag >> 8, tvrflag & 0x00FF);
	}

}


//=========================================================================================================================================

void EMVSetAuthorityPublicKeys (EMV* pemv, EMVApplication* pApplication)
{
	

}

EMVApplication* EMVGetApplicationFromAID (EMV* pemv, BYTE* AID)
{
	List* pApplications = pemv->Applications;

	while (pApplications)
	{
		EMVApplication* pApplication = (EMVApplication*)pApplications->car;
		if (memcmp (pApplication->RID, AID, 5) == 0)
			return pApplication;
		pApplications = pApplications->cdr;
	}
	return NULL;

}

EMVApplication* EMVGetApplicationFromRID (EMV* pemv, BYTE* RID)
{
	List* pApplications = pemv->Applications;

	while (pApplications)
	{
		EMVApplication* pApplication = (EMVApplication*)pApplications->car;
		if (memcmp (pApplication->RID, RID, 5) == 0)
			return pApplication;
		pApplications = pApplications->cdr;
	}
	return NULL;

}

MXMessage* EMVSendAPDU (EMV* pemv, EMVClient* pclient,  unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2, unsigned char dataSize, const unsigned char* data)
{
	MXMessage* pmessage;
	BUFFERPARM Buffer;


	Buffer.BufferType    = 'B';
    Buffer.BufferSize    = dataSize; 
    Buffer.BufferContent = (char*)malloc (dataSize);
	memcpy(Buffer.BufferContent, data, dataSize);


    pmessage = MXPutMessage (pclient->pPointOfSale->pCom, "APDU", "C-APDU");
    MXSetValue(pmessage, "Cla",		1,	&cla) ;
    MXSetValue(pmessage, "Ins",		1,	&ins) ;
    MXSetValue(pmessage, "P1",		1,	&p1) ;
    MXSetValue(pmessage, "P2",		1,	&p2) ;
    MXSetValue(pmessage, "Size",	1,	&dataSize) ;
    MXSetValue(pmessage, "Data",	1,	&Buffer) ;

	return pmessage;
}

int EMVSendACFirst (EMV* pemv, EMVClient* pclient,  unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2, unsigned char dataSize, const unsigned char* data)
{
	MXMessage* pmessage;
	BUFFERPARM Buffer;


	Buffer.BufferType    = 'B';
    Buffer.BufferSize    = dataSize;
    Buffer.BufferContent = (char*)malloc (dataSize);
	memcpy(Buffer.BufferContent, data, dataSize);


    pmessage = MXPutMessage (pclient->pPointOfSale->pCom, "APDU", "SendACFirst");
    MXSetValue(pmessage, "Cla",		1,	&cla) ;
    MXSetValue(pmessage, "Ins",		1,	&ins) ;
    MXSetValue(pmessage, "P1",		1,	&p1) ;
    MXSetValue(pmessage, "P2",		1,	&p2) ;
    MXSetValue(pmessage, "Size",	1,	&dataSize) ;
    MXSetValue(pmessage, "Data",	1,	&Buffer) ;

	return 1;
}

int EMVSendACSecond (EMV* pemv, EMVClient* pclient,  unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2, unsigned char dataSize, const unsigned char* data)
{
	MXMessage* pmessage;
	BUFFERPARM Buffer;


	Buffer.BufferType    = 'B';
    Buffer.BufferSize    = dataSize;
    Buffer.BufferContent = (char*)malloc (dataSize);
	memcpy(Buffer.BufferContent, data, dataSize);


    pmessage = MXPutMessage (pclient->pPointOfSale->pCom, "APDU", "SendACSecond");
    MXSetValue(pmessage, "Cla",		1,	&cla) ;
    MXSetValue(pmessage, "Ins",		1,	&ins) ;
    MXSetValue(pmessage, "P1",		1,	&p1) ;
    MXSetValue(pmessage, "P2",		1,	&p2) ;
    MXSetValue(pmessage, "Size",	1,	&dataSize) ;
    MXSetValue(pmessage, "Data",	1,	&Buffer) ;

	return 1;
}

int EMVSendCommand (EMV* pemv, EMVClient* pclient,  unsigned char p1, unsigned char p2)
{
	MXMessage* pmessage;
    pmessage = MXPutMessage (pclient->pPointOfSale->pCom, "APDU", "SendCommand");
    MXSetValue(pmessage, "P1",		1,	&p1) ;
    MXSetValue(pmessage, "P2",		1,	&p2) ;
	int outDataSize = 0;
	unsigned char outData[258];
	EMVTraceAPDU(pclient, 0x00, 0xCA, p1, p2, outData, outDataSize, 0);
	return 1;
}

int EMVSendVerify (EMV* pemv, EMVClient* pclient, BYTE enciphered)
{
	MXMessage* pmessage;
	
    pmessage = MXPutMessage (pclient->pPointOfSale->pCom, "APDU", "SendVerify");
    MXSetValue(pmessage, "Enciphered",		1,	&enciphered) ;
	return 1;
}

EMVClient* EMVGetClientFromCom (EMV* pemv, MXCom* pcom)
{
	List* Clients = pemv->Clients;
	while (Clients)
	{
		EMVClient* pclient = (EMVClient*)Clients->car;
		if (pclient->pPointOfSale->pCom == pcom) return pclient;
		Clients = Clients->cdr;
	}
	return NULL;
}

int EMVCardDetectionAndReset (EMV* pemv, EMVClient* pclient, char* satr) 
{
	pclient->Step = EMV_STEP_CARD_DETECTION_AND_RESET;
	EMVTraceStep (pclient);

	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, "Init Terminal %s\n", satr);
		s_printf(smessage, pclient, "%s", strace);
	}

	EMVSetClient(pemv, pclient);
	strcpy(pclient->ATR, satr);
	return EMV_OK;
}

int EMVCandidateListCreation (EMV* pemv, EMVClient* pclient) 
{
    char PaySelectString[20];


	pclient->Step = EMV_STEP_CANDIDATE_LIST_CREATION;
	EMVTraceStep (pclient);

	pclient->candidateApplicationCount = 0;
	pclient->indexApplicationSelected = 0;


	if (pemv->Settings.appSelectionUsePSE)
	{
		pclient->SubStep  = EMV_SUBSTEP_ATTEMP_DIRECTORY_LISTING_PSE;
		EMVTraceSubStep (pclient);
	
        if (pclient->pTransaction->POSEntryMode == EMV_POS_ENTRY_MODE_contactless)
            strcpy (PaySelectString, "2PAY.SYS.DDF01");
        else
            strcpy (PaySelectString, "1PAY.SYS.DDF01");



		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, ">>SELECT %s\n", PaySelectString);
			s_printf(smessage, pclient, "%s", strace);
		}
		pclient->RecordNo = 1;
		pemv->APDU (pemv, pclient, 0x00, INS_SELECT, 0x04, 0x00, strlen(PaySelectString), (unsigned char*)PaySelectString);
	}
	else
		return EMVBuildCandidateListUsingListOfAids (pemv, pclient);
	
	return EMV_OK;
}

int EMVApplicationSelection(EMV* pemv, EMVClient* pclient)
{

	BYTE idx;
	int resultSelectApplication;

	pclient->Step = EMV_STEP_APPLICATION_SELECTION;
	EMVTraceStep(pclient);

	pclient->SubStep = EMV_SUBSTEP_SELECT_APPLICATION_FILE;
	EMVTraceSubStep(pclient);

	resultSelectApplication = EMVApplicationPriority(pemv, pclient);

	if (resultSelectApplication == EMV_NEED_CONFIRM_APPLICATION)
	{
		MXMessage* pmessage;
		BYTE index = 0;
		BYTE count = 1;
		
		pmessage = MXPutMessage (pclient->pPointOfSale->pCom, "APDU", "SendAppliSelection");
		MXSetValue(pmessage, "Count",		1,	&count) ;
		MXSetValue(pmessage, "Priority",	1,	&EMVGetCandidate(pemv, pclient, index)->priority) ;
		MXSetValue(pmessage, "Index",		1,	&index) ;
		MXSetValue(pmessage, "Label",		1,	&EMVGetCandidate(pemv, pclient, index)->strApplicationLabel) ;
    
		return 1;
	}
	
	if (resultSelectApplication == EMV_NEED_SELECT_APPLICATION)
	{
		MXMessage* pmessage;
		BYTE count = EMVGetApplicationCandidatesCount (pemv, pclient);
		
		pmessage = MXPutMessage (pclient->pPointOfSale->pCom, "APDU", "SendAppliSelection");
		MXSetValue(pmessage, "Count",		1,	&count);
		for (idx = 0; idx < count; idx++)
		{
		
			MXSetValue(pmessage, "Priority",	idx + 1,	&EMVGetCandidate(pemv, pclient, idx)->priority) ;
			MXSetValue(pmessage, "Index",		idx + 1,	&idx) ;
			MXSetValue(pmessage, "Label",		idx + 1,	&EMVGetCandidate(pemv, pclient, idx)->strApplicationLabel) ;

		}
		while (idx < 5)
		{
			char zerobyte[1] = {0};
			MXSetValue(pmessage, "Priority",	idx + 1,	&zerobyte) ;
			MXSetValue(pmessage, "Index",		idx + 1,	&zerobyte) ;
			MXSetValue(pmessage, "Label",		idx + 1,	&zerobyte) ;
			idx++;
		}
		return 1;
	}
	
	if (resultSelectApplication == EMV_UNKNOWN_ERROR)
	{
		return EMVTransactionCompletion(pemv, pclient, 1);
	}

	if (resultSelectApplication == EMV_TERMINATED)
	{
		return EMVTransactionCompletion(pemv, pclient, 1);
	}

	return 1;
}

int EMVReadApplicationData (EMV* pemv, EMVClient* pclient) 
{
// Transaction flow. Read Application Data
// Result can be:
// EMV_OK - ok, you can process next step and use TLVGetTag to get some tags (for ex. PAN)
// EMV_TERMINATED, EMV_ERROR_TRANSMIT, EMV_UNKNOWN_ERROR

	unsigned char* aflValue;	
	int aflSize;
	unsigned char* aflCurrent;
	int aflIndex;


	pclient->Step = EMV_STEP_READ_APPLICATION_DATA;
	EMVTraceStep (pclient);

	pclient->SubStep  = 0;

	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, "Read application data\n");
		s_printf(smessage, pclient, "%s", strace);
	}

	aflValue = TLVGetTag(pclient->pTLV, TAG_AFL, &aflSize);
	if (!aflValue || (aflSize % 4) != 0)
		return EMV_UNKNOWN_ERROR;

	aflCurrent = aflValue;
	pclient->NumberOfRecordsToRead = 0;
	for (aflIndex = 0; aflIndex < aflSize; aflIndex += 4, aflCurrent += 4)
	{
		unsigned char record;
		unsigned char p2;


		p2 = (aflCurrent[0] & 0xF8) | 0x04;
		if (aflCurrent[1] > aflCurrent[2])
			return EMV_UNKNOWN_ERROR;
		
		pclient->RecordNumber = 0;
		for (record = aflCurrent[1]; record <= aflCurrent[2]; record++)
		{
			// READ RECORD
			pemv->APDU(pemv, pclient, 0x00, INS_READ_RECORD, record, p2, 0, (const BYTE*)"");
			pclient->NumberOfRecordsToRead++;
		}
	}

	return EMV_OK;
}

//========================
			int EMVCompleteEquivalentDataTags (EMV* pemv, EMVClient* pclient)
			{
				int tag1Size, tag2Size, tag3Size;
				BYTE* Track1Buffer  = NULL;
				BYTE* Track2Buffer  = NULL;
				BYTE* Track3Buffer  = NULL;

				pclient->SubStep = EMV_SUBSTEP_COMPLETE_EQUIVALENTDATA_TAGS;
				EMVTraceSubStep (pclient);

                if (pclient->pTransaction->POSEntryMode == EMV_POS_ENTRY_MODE_ICC_CVV_can_be_checked)
				{
					Track1Buffer = TLVGetTag(pclient->pTLV, TAG_TRACK1_EQUIVALENT_DATA, &tag1Size);
					Track2Buffer = TLVGetTag(pclient->pTLV, TAG_TRACK2_EQUIVALENT_DATA, &tag2Size);
					Track3Buffer = TLVGetTag(pclient->pTLV, TAG_TRACK3_EQUIVALENT_DATA, &tag3Size);
				}
				if (Track1Buffer != NULL)
				{
					EMVSetTag(pclient, TAG_TRACK1_DISCRETIONARY_DATA,	Track1Buffer + 52, 3);
				}
				if (Track2Buffer != NULL)
				{
					//Extract pan
					BYTE ServiceCode[2]={0x00,0x00};
					BYTE IIN[3];
					int j = 0;

					char* Output = (char*)malloc (2 * tag2Size);
					char* bOutput = Output;
					int length = CharArrayToHexaCharArray (Track2Buffer, 0, tag2Size, Output);
					CharArrayToBCDArray (Output, 6, 6, "n", IIN); // fill with 0 on the left
					
					EMVSetTag(pclient, TAG_ISSUER_IDENTIFICATION_NUMBER,	IIN , 3);
					EMVTraceTag (pclient, TAG_ISSUER_IDENTIFICATION_NUMBER, IIN, 3);
					
					while (*Output != 'D' && j < length) 
					{
						Output++;
						j++;
					}
					bOutput[j] = 0;
					
					EMVCardValidator (pemv, pclient, bOutput, j);
					EMVTraceIIN (pclient, bOutput);

					//Add seperator 1 Byte + Date 4 Bytes
					j += 5; 
					Output = Output + 5;
					CharArrayToBCDArray (Output, 3, 3, "n", ServiceCode); // fill with 0 on the left
					free (bOutput);
					
					EMVSetTag(pclient, TAG_SERVICE_CODE,	ServiceCode , 2);
					EMVTraceTag (pclient, TAG_SERVICE_CODE, ServiceCode, 2);
					
					pclient->EMV_SC = TLVGetTag (pclient->pTLV, TAG_SERVICE_CODE, &tag1Size);
					EMVTraceServiceCode (pclient);
				
				}
				if (Track3Buffer != NULL)
				{
				}

				return EMV_OK;
			}	

//========================
			int	EMVCheckMandatoryData (EMV* pemv, EMVClient* pclient) 
			{
				int tagSize;
			// Check for mandatory
				pclient->SubStep = EMV_SUBSTEP_CHECK_MANDATORY_DATA;
				EMVTraceSubStep (pclient);

				if (!TLVGetTag(pclient->pTLV, TAG_APPLICATION_EXPIRATION_DATE, &tagSize) || 
					!TLVGetTag(pclient->pTLV, TAG_PAN, &tagSize) || 
					!TLVGetTag(pclient->pTLV, TAG_CDOL_1, &tagSize) || 
					!TLVGetTag(pclient->pTLV, TAG_CDOL_2, &tagSize))
                {
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "%5s%s", "", "Mandatory Data is missing, EMV Should Terminate....\n");
						s_printf(smessage, pclient, "%s", strace);
					}
                }
                else
                {
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "%5s%s", "", "Mandatory Data is OK\n");
						s_printf(smessage, pclient, "%s", strace);
					}

                }


				return EMV_OK;
			}	
//========================

			int	EMVCheckMissingData (EMV* pemv, EMVClient* pclient) 
			{
				int tagSize;
			// Check for missing data
				pclient->SubStep = EMV_SUBSTEP_CHECK_MISSING_DATA;
				EMVTraceSubStep (pclient);

				pclient->EMV_CVM = TLVGetTag(pclient->pTLV, TAG_CVM_LIST, &pclient->EMV_CVMSize);

				if (CardHolder_verification_is_supported (pclient) && !pclient->EMV_CVM)
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "%5s%s", "", "CVM List Not present and AIP indicates that cardholder verification is supported\n");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
					EMVSetTVR (pemv, pclient, ICC_data_missing, 1);
				}

				if (SDA_supported (pclient) &&
					!TLVGetTag(pclient->pTLV, TAG_SIGNED_STATIC_APPLICATION_DATA, &tagSize))
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "%5s%s", "", "Not present and AIP indicates SDA supported\n");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}

					EMVSetTVR (pemv, pclient, ICC_data_missing, 1);
				}


				if ((SDA_supported (pclient) || DDA_supported (pclient) || CDA_supported (pclient)) &&
					!TLVGetTag(pclient->pTLV, TAG_CERTIFICATION_AUTHORITY_PUBLIC_KEY_INDEX_CARD, &tagSize) ||
					!TLVGetTag(pclient->pTLV, TAG_ISSUER_PUBLIC_KEY_CERTIFICATE, &tagSize) ||
					!TLVGetTag(pclient->pTLV, TAG_ISSUER_PUBLIC_KEY_EXPONENT, &tagSize) ||
					!TLVGetTag(pclient->pTLV, TAG_ISSUER_PUBLIC_KEY_REMAINDER, &tagSize))
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "%5s%s", "", "Issuer Public Key Not present and AIP indicates any form of offline data authentication is supported (SDA, DDA or CDA)\n");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
					EMVSetTVR (pemv, pclient, ICC_data_missing, 1);
				}

				if ((DDA_supported (pclient) || CDA_supported (pclient)) &&
					!TLVGetTag(pclient->pTLV, TAG_ICC_PUBLIC_KEY_CERTIFICATE, &tagSize) ||
					!TLVGetTag(pclient->pTLV, TAG_ICC_PUBLIC_KEY_REMAINDER, &tagSize) ||
					!TLVGetTag(pclient->pTLV, TAG_ICC_PUBLIC_KEY_EXPONENT, &tagSize))
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "%5s%s", "", "ICC Public Key Not present and AIP indicates DDA or CDA supported\n");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
					EMVSetTVR (pemv, pclient, ICC_data_missing, 1);
				}
				return EMV_OK;
			}
//========================


int EMVDataAuthentification (EMV* pemv, EMVClient* pclient) 
{
	int Authentification;

	pclient->Step = EMV_STEP_DATA_AUTHENTIFICATION;
	EMVTraceStep (pclient);

	if (EMVCheckTerminalType (pemv, pclient) == -1)
	{
		EMVProcessingRestrictions (pemv, pclient); 
		return EMV_OK;
	}

	Authentification = EMVCheckAuthentificationMechanism (pemv, pclient);

	if (Authentification == EMV_SUBSTEP_CDA)
		EMVCheckOfflineCDA (pemv, pclient);
	else
	if (Authentification == EMV_SUBSTEP_OFFLINE_DDA)
		EMVCheckOfflineDDA (pemv, pclient);
	else
	if (Authentification == EMV_SUBSTEP_OFFLINE_SDA)
		EMVCheckOfflineSDA (pemv, pclient);
	
	EMVSetTSI (pemv, pclient, Offline_data_authentication_was_performed, 1);


	EMVProcessingRestrictions (pemv, pclient); 
	return EMV_OK;
}
//========================
			int EMVCheckTerminalType (EMV* pemv, EMVClient* pclient)
			{
				
				pclient->SubStep = EMV_SUBSTEP_CHECK_TERMINAL_TYPE;
				EMVTraceSubStep (pclient);



				if (pemv->DebugEnabled)
				{
					char strace[1000];
					memset(strace, 0, 1000);
					sprintf(strace, "%5s%s", "", "Lets check Terminal Type if offline is supported\n");
					s_printf(smessage, pclient, "%s", strace);
				}
				if (EMVTerminalIsOnlineOnly (pclient))
				{
					EMVSetTVR(pemv, pclient, Offline_data_authentication_was_not_performed, 1);
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "%5s%s", "", "Terminal is Online only. Skip Offline Authentification\n");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
					return -1;
				}
				return EMV_OK;

			}


			int	EMVCheckAuthentificationMechanism (EMV* pemv, EMVClient* pclient)
			{

				pclient->SubStep = EMV_SUBSTEP_SELECTION_OFFLINE_AUTHENTIFICATION_MECHANISM;
				EMVTraceSubStep (pclient);
				
				if (EMVTerminalAndCardSupportCDA(pclient))
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "%5s%s", "", "Terminal And Card Support CDA Authentification, CDA is Selected\n");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
					return EMV_SUBSTEP_CDA;
				}
				else
				if (EMVTerminalAndCardSupportDDA(pclient))
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "%5s%s", "", "Terminal And Card Support DDA Authentification, DDA is Selected\n");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}

					return EMV_SUBSTEP_OFFLINE_DDA;
				}
				if (EMVTerminalAndCardSupportSDA(pclient))
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "%5s%s", "", "Terminal And Card Support SDA Authentification, SDA is Selected\n");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
					return EMV_SUBSTEP_OFFLINE_SDA;
				}
			
				return EMV_NO_MATCH_AUTHENTIFICATION;
			}
			int	EMVCheckOfflineSDA (EMV* pemv, EMVClient* pclient)
			{
				int KCTagSize = 0;
				int KRTagSize = 0;
				int KETagSize = 0;
				int SDATagSize = 0;
				int AKTagSize = 0;

				BYTE* BufferKeyCertificate;
				BYTE* BufferKeyRemainder;
				BYTE* BufferKeyExponent;
				BYTE* BufferSDA;
				BYTE* BufferAuthorityKeyIndex;


				pclient->SubStep = EMV_SUBSTEP_OFFLINE_SDA;
				EMVTraceSubStep (pclient);

				BufferKeyCertificate= TLVGetTag (pclient->pTLV, TAG_ISSUER_PUBLIC_KEY_CERTIFICATE,	&KCTagSize);
				BufferKeyRemainder	= TLVGetTag (pclient->pTLV,	TAG_ISSUER_PUBLIC_KEY_REMAINDER,	&KRTagSize);
				BufferKeyExponent	= TLVGetTag (pclient->pTLV, TAG_ISSUER_PUBLIC_KEY_EXPONENT,		&KETagSize);
				BufferSDA			= TLVGetTag (pclient->pTLV, TAG_SIGNED_STATIC_APPLICATION_DATA, &SDATagSize);
				BufferAuthorityKeyIndex	 = TLVGetTag (pclient->pTLV, TAG_CERTIFICATION_AUTHORITY_PUBLIC_KEY_INDEX_CARD, &AKTagSize);
				if (pemv->DebugEnabled)
				{
					printf ("\n%5s", "");
					EMVTraceTag (pclient, TAG_ISSUER_PUBLIC_KEY_CERTIFICATE,  BufferKeyCertificate, KCTagSize);
					printf ("%5s", "");
					EMVTraceTag (pclient, TAG_ISSUER_PUBLIC_KEY_REMAINDER,    BufferKeyRemainder, KRTagSize);
					printf ("%5s", "");
					EMVTraceTag (pclient, TAG_ISSUER_PUBLIC_KEY_EXPONENT,     BufferKeyExponent, KETagSize);
					printf ("%5s", "");
					EMVTraceTag (pclient, TAG_SIGNED_STATIC_APPLICATION_DATA, BufferSDA, SDATagSize);
					printf ("%5s", "");
					EMVTraceTag (pclient, TAG_CERTIFICATION_AUTHORITY_PUBLIC_KEY_INDEX_CARD, BufferAuthorityKeyIndex, AKTagSize);

				}		
				
				if (!BufferKeyCertificate || !BufferKeyExponent || !BufferSDA)
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "\n%5s\n", "Mandatory Fields for SDA is Missing.. ");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
					EMVSetTVR (pemv, pclient, ICC_data_missing, 1);
					EMVSetTVR (pemv, pclient, SDA_failed, 1);
					return EMV_TERMINATED;
				}

				EMVGetAuthorityPublicKeyFromKeyIndex (pemv, pclient, BufferAuthorityKeyIndex[0]);

				return EMV_OK;
			}
			int	EMVCheckOfflineDDA (EMV* pemv, EMVClient* pclient)
			{
				int KCTagSize = 0;
				int KRTagSize = 0;
				int KETagSize = 0;
				int AKTagSize = 0;
				int IccKCTagSize = 0;
				int IccKRTagSize = 0;
				int IccKETagSize = 0;
				int DDOLTagSize = 0;

				BYTE* BufferKeyCertificate;
				BYTE* BufferKeyRemainder;
				BYTE* BufferKeyExponent;
				BYTE* BufferAuthorityKeyIndex;
				BYTE* BufferIccKeyCertificate;
				BYTE* BufferIccKeyRemainder;
				BYTE* BufferIccKeyExponent;
				BYTE* BufferDDOL;

				pclient->SubStep = EMV_SUBSTEP_OFFLINE_DDA;
				EMVTraceSubStep (pclient);
				
				BufferKeyCertificate	= TLVGetTag (pclient->pTLV, TAG_ISSUER_PUBLIC_KEY_CERTIFICATE,	&KCTagSize);
				BufferKeyRemainder		= TLVGetTag (pclient->pTLV,	TAG_ISSUER_PUBLIC_KEY_REMAINDER,	&KRTagSize);
				BufferKeyExponent		= TLVGetTag (pclient->pTLV, TAG_ISSUER_PUBLIC_KEY_EXPONENT,		&KETagSize);
				BufferAuthorityKeyIndex	 = TLVGetTag (pclient->pTLV, TAG_CERTIFICATION_AUTHORITY_PUBLIC_KEY_INDEX_CARD, &AKTagSize);
				BufferIccKeyCertificate	= TLVGetTag (pclient->pTLV, TAG_ICC_PUBLIC_KEY_CERTIFICATE,		&IccKCTagSize);
				BufferIccKeyRemainder	= TLVGetTag (pclient->pTLV,	TAG_ICC_PUBLIC_KEY_REMAINDER,		&IccKRTagSize);
				BufferIccKeyExponent	= TLVGetTag (pclient->pTLV, TAG_ICC_PUBLIC_KEY_EXPONENT,		&IccKETagSize);
				BufferDDOL				= TLVGetTag (pclient->pTLV, TAG_DYNAMIC_DATA_AUTHENTIFICATION_DATA_OBJECT_LIST,		&DDOLTagSize);
				if (pemv->DebugEnabled)
				{
					printf ("\n%5s", "");
					EMVTraceTag (pclient, TAG_ICC_PUBLIC_KEY_CERTIFICATE,  BufferIccKeyCertificate, IccKCTagSize);
					printf ("%5s", "");
					EMVTraceTag (pclient, TAG_ICC_PUBLIC_KEY_REMAINDER,    BufferIccKeyRemainder, IccKRTagSize);
					printf ("%5s", "");
					EMVTraceTag (pclient, TAG_ICC_PUBLIC_KEY_EXPONENT,     BufferIccKeyExponent, IccKETagSize);
					printf ("%5s", "");
					EMVTraceTag (pclient, TAG_DYNAMIC_DATA_AUTHENTIFICATION_DATA_OBJECT_LIST, BufferDDOL, DDOLTagSize);
					printf ("%5s", "");
					EMVTraceTag (pclient, TAG_CERTIFICATION_AUTHORITY_PUBLIC_KEY_INDEX_CARD, BufferAuthorityKeyIndex, AKTagSize);
				}					
				
				if (!BufferKeyCertificate || !BufferKeyExponent || !BufferIccKeyCertificate || !BufferIccKeyExponent)
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "\n%5s\n", "Mandatory Fields for DDA is Missing.. ");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
					EMVSetTVR (pemv, pclient, ICC_data_missing, 1);
					EMVSetTVR (pemv, pclient, DDA_failed, 1);
					return EMV_TERMINATED;
				}
				if (!BufferDDOL)
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "\n%5s\n", "DDOL is Missing, We check the default DDOL personalized in the Terminal");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
				}

				EMVGetAuthorityPublicKeyFromKeyIndex (pemv, pclient, BufferAuthorityKeyIndex[0]);
				return EMV_OK;
			}


			int	EMVCheckOfflineCDA (EMV* pemv, EMVClient* pclient)
			{
				pclient->SubStep = EMV_SUBSTEP_CDA;
				EMVTraceSubStep (pclient);
				return EMV_OK;
			}

//========================

int EMVProcessingRestrictions (EMV* pemv, EMVClient* pclient) 
{
	pclient->Step = EMV_STEP_PROCESSING_RESTRICTIONS;
	EMVTraceStep (pclient);

	EMVCheckApplicationVersionNumber (pemv, pclient);
	EMVCheckApplicationUsageControl (pemv, pclient);
	EMVCheckApplicationExpirationDates (pemv, pclient);

	EMVCardHolderVerification(pemv, pclient);

	return EMV_OK;
}
//========================
			int	EMVCheckApplicationVersionNumber (EMV* pemv, EMVClient* pclient)
			{
				int OutSize;
				BYTE* AppVersionCard;
				BYTE* AppVersionTerminal;

				pclient->SubStep = EMV_SUBSTEP_APPLICATION_VERSION_NUMBER;
				EMVTraceSubStep (pclient);

				AppVersionCard = TLVGetTag (pclient->pTLV, TAG_APPLICATION_VERSION_NUMBER_CARD, &OutSize);
				if (AppVersionCard == NULL)
				{
					return EMV_OK;
				}
				AppVersionTerminal = pclient->pApplication->AID[pclient->IndexApplicationSelected].ApplicationVersionNumber; //TLVGetTag (pclient->pTLV, TAG_APPLICATION_VERSION_NUMBER_TERMINAL, &OutSize);
				if (AppVersionTerminal == NULL)
				{
					return EMV_OK;
				}
				if (pemv->DebugEnabled)
				{
					EMVTraceHexaBuffer (pclient->pEMV, "\n       -- Card Application Version Number: ", AppVersionCard, OutSize, "\n");
					EMVTraceHexaBuffer (pclient->pEMV, "       -- Terminal Application Version Number: ", AppVersionTerminal, OutSize, "\n");
				}
				if (memcmp (AppVersionCard, AppVersionTerminal, OutSize) != 0)
					EMVSetTVR (pemv, pclient, ICC_and_terminal_have_different_application_versions, 1);

				return EMV_OK;

			}
			int	EMVCheckApplicationUsageControl (EMV* pemv, EMVClient* pclient)
			{
				int OutSize;
				BYTE* ICC;
				BYTE* TCC;
				pclient->SubStep = EMV_SUBSTEP_APPLICATION_USAGE_CONTROL;
				EMVTraceSubStep (pclient);
				EMVTraceAUC (pclient);

				//If the transaction is being conducted at an ATM, 
				//the ‘Valid at ATMs’ bit must be on in Application Usage Control. 				

				if (EMVTerminalIsATM && !Valid_at_ATMs(pclient))				
					EMVSetTVR (pemv, pclient, Requested_service_not_allowed_for_card_product, 1);
				
				//If the transaction is not being conducted at an ATM, 
				//the ‘Valid at terminals other than ATMs’ bit must be on in Application Usage Control. 

				if (!EMVTerminalIsATM && !Valid_at_terminals_other_than_ATMs(pclient))
					EMVSetTVR (pemv, pclient, Requested_service_not_allowed_for_card_product, 1);

				//If the Application Usage Control and Issuer Country Code are both present in the ICC, 
				//the terminal shall make the checks described in Table 32
				ICC = TLVGetTag (pclient->pTLV, TAG_ISSUER_COUNTRY_CODE, &OutSize);
				TCC = pclient->pTerminal->TerminalCountryCode;

				if (!ICC) return EMV_OK;


				if (pclient->pTransaction->TransactionType == EMV_TRANSACTION_TYPE_CASH_ADVANCE)
				{
					if (memcmp (TCC, ICC, 2) == 0)
					{
						if (!Valid_for_domestic_cash_transactions(pclient))
							EMVSetTVR (pemv, pclient, Requested_service_not_allowed_for_card_product, 1);
					}
					else
					{
						if (!Valid_for_international_cash_transactions(pclient))
							EMVSetTVR (pemv, pclient, Requested_service_not_allowed_for_card_product, 1);
					}
				}
				else
				if (pclient->pTransaction->TransactionType == EMV_TRANSACTION_TYPE_PURCHASE)
				{
					if (memcmp (TCC, ICC, 2) == 0)
					{
						if (!Valid_for_domestic_goods(pclient) && !Valid_for_domestic_services(pclient))
							EMVSetTVR (pemv, pclient, Requested_service_not_allowed_for_card_product, 1);
					}
					else
					{
						if (!Valid_for_international_goods(pclient) && !Valid_for_international_services(pclient))
							EMVSetTVR (pemv, pclient, Requested_service_not_allowed_for_card_product, 1);
					}
				}
				else
				if (pclient->pTransaction->TransactionType == EMV_TRANSACTION_TYPE_PURCHASE_CASHBACK)
				{
					if (memcmp (TCC, ICC, 2) == 0)
					{
						if (!Domestic_cashback_allowed(pclient))
							EMVSetTVR (pemv, pclient, Requested_service_not_allowed_for_card_product, 1);
					}
					else
					{
						if (!International_cashback_allowed(pclient))
							EMVSetTVR (pemv, pclient, Requested_service_not_allowed_for_card_product, 1);
					}
				}
				return EMV_OK;

			}

			int	EMVCheckApplicationExpirationDates (EMV* pemv, EMVClient* pclient)
			{
				int OutSize;
				char strdate[7];
				char strtime[7];
				BYTE dcbstrdate[3];
				BYTE* ExpirationDate;
				BYTE* EffectiveDate;


				pclient->SubStep = EMV_SUBSTEP_APPLICATION_EFFECTIVE_EXPIRATION_DATES;
				EMVTraceSubStep (pclient);

				DateGetLocalDateHour (strdate, strtime);
				CharArrayToBCDArray (strdate, 6, 6, "n", dcbstrdate);
				if (pemv->DebugEnabled)
				{
					EMVTraceHexaBuffer(pclient->pEMV, "\n       -- Current Date: ", dcbstrdate, 3, "\n");
				}


				ExpirationDate = TLVGetTag (pclient->pTLV, TAG_APPLICATION_EXPIRATION_DATE, &OutSize);
				if (ExpirationDate != NULL)
				{
					if (pemv->DebugEnabled)
					{
						EMVTraceHexaBuffer(pclient->pEMV, "       -- Expiration Date: ", ExpirationDate, OutSize, "\n");
					}

					if (strncmp ((char*)ExpirationDate, (char*)dcbstrdate, OutSize) < 0)
						EMVSetTVR (pemv, pclient, Expired_application, 1);
				}

				EffectiveDate = TLVGetTag (pclient->pTLV, TAG_APPLICATION_EFFECTIVE_DATE, &OutSize);
				if (EffectiveDate != NULL)
				{
					if (pemv->DebugEnabled)
					{
						EMVTraceHexaBuffer(pclient->pEMV, "       -- Effective Date: ", EffectiveDate, OutSize, "\n");
					}
					if (strncmp ((char*)EffectiveDate, (char*)dcbstrdate, OutSize) > 0)
						EMVSetTVR (pemv, pclient, Application_not_yet_effective, 1);
				}
				else
					EMVSetTVR (pemv, pclient, Application_not_yet_effective, 1);

				return EMV_OK;
			}

//=================================


int EMVCardHolderVerification (EMV* pemv, EMVClient* pclient) 
{
	BYTE* pCVMList;		
	int ListSize;
	DWORD X;                       
    DWORD Y;   

	pclient->Step = EMV_STEP_CARD_HOLDER_VERIFICATION;
	EMVTraceStep (pclient);
	
	if (!CardHolder_verification_is_supported (pclient))
	{
//		EMVTraceAIP (pclient);
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "%5s%s", "", "Card Holder Verification is supported is not set in AIP. Skip Card Holder Verification and Set no CVM performed in CVM result: CVMR to 3F0000\n");
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
		}
		EMVSetTag(pclient, TAG_CVM_RESULTS, (BYTE*)"\x3F\x00\x00" , 3);
		EMVTerminalRiskManagement (pemv, pclient);
		return EMV_OK;
	}	

	if (pclient->EMV_CVM == NULL)
	{
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "%5s%s", "", "CVM List is absent, Skip Card Holder Verification and Set no CVM performed in CVM result: CVMR to 3F0000\n");
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
		}
		EMVSetTag(pclient, TAG_CVM_RESULTS, (BYTE*)"\x3F\x00\x00" , 3);

		EMVTerminalRiskManagement (pemv, pclient);
		return EMV_OK;
		
	}



	pCVMList = pclient->EMV_CVM + 8;		
	ListSize = (pclient->EMV_CVMSize - 8) / 2;
	sscanf((const char*)pclient->EMV_CVM, "%X", &X);
	sscanf((const char*)pclient->EMV_CVM + sizeof (DWORD), "%X", &Y);
	
	EMVTraceCVM(pclient);
    if (pemv->DebugEnabled)
    {
        EMVTraceTag (pclient, TAG_CVM_LIST,  pclient->EMV_CVM,  pclient->EMV_CVMSize);
		printf("%5s%s(%lu,%lu)\n", "", "CVM List (X,Y) :", X, Y);    
    }

	int result = EMVCheckCVMRule(pemv, pclient, X, Y, pCVMList, ListSize);
	if (result == EMV_OK)
	{
		//break;
	}
	else
	if (result == EMV_NOT_SATISFIED)
	{
		//break;
	}
	result = EMVGetLastOnlineATC(pemv, pclient);
	result = EMVGetATC(pemv, pclient);
	result = EMVGetPTC (pemv, pclient);   //Pin Try Counter
	
    EMVSetTSI (pemv, pclient, Cardholder_verification_was_performed, 1);		
	
	EMVTerminalRiskManagement (pemv, pclient);
	return EMV_OK;
}

//========================

			int EMVCheckCVMRule (EMV* pemv, EMVClient* pclient, DWORD X, DWORD Y, BYTE* pCVMList, int ListSize)
			{
				int NotSuccessful = 0;
				int result;
				pclient->SubStep = EMV_SUBSTEP_CHECK_CVM_RULES;
				EMVTraceSubStep (pclient);

				for (int i = 0; i < ListSize * 2; i = i + 2)
				{
					printf("\n%02X%02X : ", pCVMList[i] & 0xFF, pCVMList[i + 1] & 0xFF);
					switch (pCVMList[i] & 0xFF & 0x3F) {
					case Fail_CVM_processing:
						printf("%-s\n", "Fail CVM processing");
						break;
					case Offline_plaintext_PIN:

						printf("%-s\n", "Offline plaintext PIN");
						break;
					case Online_PIN_(always_enciphered):
						printf("%-s\n", "Online PIN (always enciphered)");
						//   if (EMVTerminalIs(pclient, Enciphered_PIN_for_online_verification))
						//           NotSuccessful = 1;
						break;
					case Offline_plaintext_PIN_and_paper_based_Signature:
						printf("%-s\n", "Offline plaintext PIN and paper based Signature");
						break;
					case Offline_enciphered_PIN:
						printf("%-s\n", "Offline enciphered PIN");
						//   if (EMVTerminalIs(pclient, Enciphered_PIN_for_online_verification))
						   //EMVSendVerify (pclient);
						break;
					case Offline_enciphered_PIN_and_paper_based_Signature:
						printf("%-s\n", "Offline enciphered PIN and paper based Signature");
						break;
					case Paper_based_Signature_only:
						printf("%-s\n", "Paper based Signature only");
						break;
					case Approve_CVM_processing:
						printf("%-s\n", "Approve CVM processing");
						break;
					}

					switch (pCVMList[i + 1] & 0xFF & 0x0F) {
					case Always_try_to_apply_this_rule:
						printf("       %s\n", "Always try to apply this rule");
						break;
					case Only_try_to_apply_this_rule_where_this_is_an_unattended_cash_transaction:
						printf("       %s\n", "Only try to apply this rule where this is an unattended cash transaction");
						break;
					case If_not_unattended_cash_and_not_manual_cash_and_not_purchase_with_cashback:
						printf("       %s\n", "If not unattended cash and not manual cash and not purchase with cashback");
						break;
					case Always_try_to_apply_this_rule_where_the_CVM_code_is_supported:
						printf("       %s\n", "Always try to apply this rule where the CVM code is supported");
						break;
					case If_this_is_a_manual_cash_transaction_apply_this_rule:
						printf("       %s\n", "If this is a manual cash transaction apply this rule");
						break;
					case If_this_is_a_purchase_with_cashback_apply_this_rule:
						printf("       %s\n", "If this is a purchase with cashback apply this rule");
						break;
					case If_transaction_is_in_the_application_currency_and_is_under_X_value:
						printf("       %s\n", "If transaction is in the application currency and is under X value");
						break;
					case If_transaction_is_in_the_application_currency_and_is_over_X_value:
						printf("       %s\n", "If transaction is in the application currency and is over X value");
						break;
					case If_transaction_is_in_the_application_currency_and_is_under_Y_value:
						printf("       %s\n", "If transaction is in the application currency and is under Y value");
						break;
					case If_transaction_is_in_the_application_currency_and_is_over_Y_value:
						printf("       %s\n", "If transaction is in the application currency and is over Y value");

						break;


					}
					printf("%s : %s\n", "Rule", (pCVMList[i] & 0xFF & 0x40) ? "Move to next rule if verification is unsuccessful." : "Fail cardholder verification if verification is unsuccessful.");


					if (!NotSuccessful) result =  EMV_OK;
					else
					{
						if (pCVMList[i] & 0xFF & 0x40)
							result = EMV_NEXT_RULE;
						else
							result = EMV_NOT_SATISFIED;
					}
				}
				return result;
			}

	
//========================

int EMVTerminalRiskManagement (EMV* pemv, EMVClient* pclient) 
{
	pclient->Step = EMV_STEP_TERMINAL_RISK_MANAGEMENT;
	EMVTraceStep (pclient);


	EMVCheckExceptionListCards (pemv, pclient);
	EMVCheckCBRegisteredBin (pemv, pclient);

	if (!Terminal_risk_management_is_to_be_performed (pclient))
	{
		EMVTraceAIP (pclient);
		printf("%5s%s", "", "Terminal Risk Management is to be performed is not set in AIP. Skip Terminal Risk Management process\n");
		EMVTerminalActionAnalysis (pemv, pclient);
		return EMV_OK;
	}	
	EMVCheckFloorLimit (pemv, pclient);
	EMVCheckRandomTransactionSelection (pemv, pclient);
	EMVCheckVelocity (pemv, pclient);

	EMVSetTSI (pemv, pclient, Terminal_risk_management_was_performed, 1);	


	return EMV_OK;
}

//========================

			int EMVCheckExceptionListCards (EMV* pemv, EMVClient* pclient)  //TAG DF1D
			{
				int tagSize;
				BYTE* PanBuffer;
				EMVExceptionCard* pexceptionCard;

				pclient->SubStep = EMV_SUBSTEP_EXCEPTION_LIST_CARDS_CHECKING;
				EMVTraceSubStep (pclient);

				PanBuffer = TLVGetTag(pclient->pTLV, TAG_PAN, &tagSize);
				pexceptionCard = EMVGetExceptionPanFromPan (pemv, pclient, PanBuffer, tagSize);

				if (pexceptionCard)
				{
					
					if (pemv->DebugEnabled)
					{
						EMVTraceHexaBuffer(pclient->pEMV, "\n       -- PAN is in Black List : ", PanBuffer, tagSize, "\n\n");
					}
					EMVTraceCardStatus (pexceptionCard->Status);
					if (pexceptionCard->Status == Card_forbidden || pexceptionCard->Status == Card_refused)
					{
						EMVSetTVR (pemv, pclient, Card_appears_on_terminal_exception_file, 1);
					}
					else
					if (pexceptionCard->Status == Card_watched)
					{
						EMVSetTVR (pemv, pclient, Transaction_exceeds_floor_limit, 1);
					}
				}
				else
					if (pemv->DebugEnabled)
					{
						EMVTraceHexaBuffer(pclient->pEMV, "\n       -- PAN is not in Black List : ", PanBuffer, tagSize, "\n\n");
					}


				return EMV_OK;
			}

			int EMVCheckCBRegisteredBin (EMV* pemv, EMVClient* pclient)      ////TAG DF1E
			{
				int tagSize;
				BYTE* PanBuffer;
				EMVRangeBin* pRangeBin;

				pclient->SubStep = EMV_SUBSTEP_CB_REGISTERED_BIN_CHECKING;
				EMVTraceSubStep (pclient);

				PanBuffer = TLVGetTag(pclient->pTLV, TAG_PAN, &tagSize);
				pRangeBin = EMVGetRangeBinFromPan (pemv, pclient, PanBuffer, tagSize);

				if (pRangeBin)
				{

					if (pemv->DebugEnabled)
					{
						EMVTraceHexaBuffer(pclient->pEMV, "\n       -- PAN is in Range Bin : ", PanBuffer, tagSize, "\n\n");
					}
					EMVTraceBinStatus (pRangeBin->Status);
					if (pRangeBin->Status == Bin_forbidden || pRangeBin->Status == Bin_refused)
					{
						EMVSetTVR (pemv, pclient, Card_appears_on_terminal_exception_file, 1);
					}
					else
					if (pRangeBin->Status == Bin_watched)
					{
						EMVSetTVR (pemv, pclient, Transaction_exceeds_floor_limit, 1);

					}

				}
				else
                {
                    EMVSetTVR (pemv, pclient, Transaction_exceeds_floor_limit, 1);
					if (pemv->DebugEnabled)
					{
						EMVTraceHexaBuffer(pclient->pEMV, "\n       -- PAN is NOT in Range Bin : ", PanBuffer, tagSize, "\n\n");
					}
                }
				return EMV_OK;

			}
			int	EMVCheckFloorLimit (EMV* pemv, EMVClient* pclient)
			{
				pclient->SubStep = EMV_SUBSTEP_FLOOR_LIMIT_CHECKING;
				EMVTraceSubStep (pclient);
				return EMV_OK;
			}

			int	EMVCheckRandomTransactionSelection (EMV* pemv, EMVClient* pclient)
			{
				pclient->SubStep = EMV_SUBSTEP_RANDOM_TRANSACTION_SELECTION;
				EMVTraceSubStep (pclient);
				return EMV_OK;
			}

			int	EMVCheckVelocity (EMV* pemv, EMVClient* pclient)
			{
				int TagSize;
				unsigned char* TagBufUCOL;
				unsigned char* TagBufLCOL;

				pclient->SubStep = EMV_SUBSTEP_VELOCITY_CHECKING;
				EMVTraceSubStep (pclient);

				TagBufLCOL = TLVGetTag(pclient->pTLV, TAG_LCOL, &TagSize);
				TagBufUCOL = TLVGetTag(pclient->pTLV, TAG_UCOL, &TagSize);

				if (pemv->DebugEnabled)
				{
					printf ("\n%5s", "");
					EMVTraceTag (pclient, TAG_UCOL, TagBufUCOL, TagSize);
					printf ("%5s", "");
					EMVTraceTag (pclient, TAG_LCOL, TagBufLCOL, TagSize);
					printf ("\n");
				}			
				
				if (!TagBufLCOL || !TagBufUCOL)
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "\n%5sNo Lower/Upper Consecutive Offline Limit data objects on the card : Skip velocity checking\n", "");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
					return EMVTerminalActionAnalysis (pemv, pclient);
				}
				else
				{
					EMVGetATC (pemv, pclient);
					EMVGetLastOnlineATC (pemv, pclient);
				}
				return EMV_OK;
			}

//========================


int EMVTerminalActionAnalysis (EMV* pemv, EMVClient* pclient) 
{
	pclient->Step = EMV_STEP_TERMINAL_ACTION_ANALYSIS;
	EMVTraceStep (pclient);
	
	EMVCheckActionIfDenial (pemv, pclient);
	
	if (pclient->Cryptogram != EMV_CRYPTO_TYPE_AAC)	
		EMVCheckActionIfOnLine (pemv, pclient);
	
	if (pclient->Cryptogram != EMV_CRYPTO_TYPE_AAC && pclient->Cryptogram != EMV_CRYPTO_TYPE_ARQC) 
		EMVCheckActionIfApproved (pemv, pclient);
	


	EMVCardActionAnalysis (pemv, pclient);
	
	return EMV_OK;
}
//========================

			int	EMVCheckActionIfDenial (EMV* pemv, EMVClient* pclient)
			{
				int outSize;
				int i;
				BOOL DenialAction = FALSE;
				BYTE* EMV_TVR_TACDENIAL = (BYTE*) TLVGetTag(pclient->pTLV, TAG_TERMINAL_ACTION_CODE_DENIAL, &outSize);
				BYTE* EMV_TVR_IACDENIAL = (BYTE*) TLVGetTag(pclient->pTLV, TAG_ISSUER_ACTION_CODE_DENIAL, &outSize);
				BYTE* EMV_TVR = (BYTE*) TLVGetTag(pclient->pTLV, TAG_TVR, &outSize);
				BYTE EMV_RESULT[5];
				

				pclient->SubStep = EMV_SUBSTEP_ACTION_IF_DENIAL;
				EMVTraceSubStep (pclient);

				EMVTraceTVROnlyYes (pclient, (EMV_BITS*)EMV_TVR_IACDENIAL, "IAC_Denial");
				EMVTraceTVROnlyYes (pclient, (EMV_BITS*)EMV_TVR_TACDENIAL, "TAC_Denial");
				EMVTraceTVROnlyYes (pclient, (EMV_BITS*)EMV_TVR, "TVR");

				for (i = 0; i < 5; i++)
				{			
					EMV_RESULT[i] = (EMV_TVR_TACDENIAL[i] | EMV_TVR_IACDENIAL[i]) & EMV_TVR[i];
					if (EMV_RESULT[i])
					{
						DenialAction = TRUE;
					}
				}
				if (DenialAction)
				{
					pclient->Cryptogram = EMV_CRYPTO_TYPE_AAC;
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "***ACTION RESULT : bits matches with TVR for a denial\n --REASON(S) :");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}

					EMVTraceTVROnlyYes (pclient, (EMV_BITS*)EMV_RESULT, "");
					EMVTraceCryptogramType (pclient);					
					return EMV_TERMINATED;
				}
				if (pemv->DebugEnabled)
				{
					char strace[1000];
					memset(strace, 0, 1000);
					sprintf(strace, "***ACTION RESULT : No bits matches with TVR for a denial, let's check Online action\n\n");
					s_printf(smessage, pclient, "%s", strace);
					Send_Info(EMVRooterCom, pclient, "INFO", strace);
				}
				return EMV_OK;

			}

			int	EMVCheckActionIfOnLine (EMV* pemv, EMVClient* pclient)
			{
				int outSize;
				int i;
				BOOL OnlineAction = FALSE;
				BYTE* EMV_TVR_TACONLINE = (BYTE*) TLVGetTag(pclient->pTLV, TAG_TERMINAL_ACTION_CODE_ONLINE, &outSize);
				BYTE* EMV_TVR_IACONLINE = (BYTE*) TLVGetTag(pclient->pTLV, TAG_ISSUER_ACTION_CODE_ONLINE, &outSize);
				BYTE* EMV_TVR = (BYTE*) TLVGetTag(pclient->pTLV, TAG_TVR, &outSize);
				BYTE EMV_RESULT[5];
				

				pclient->SubStep = EMV_SUBSTEP_ACTION_IF_ONLINE;
				EMVTraceSubStep (pclient);


				EMVTraceTVROnlyYes (pclient, (EMV_BITS*)EMV_TVR_IACONLINE, "IAC_Online");
				EMVTraceTVROnlyYes (pclient, (EMV_BITS*)EMV_TVR_TACONLINE, "TAC_Online");
				EMVTraceTVROnlyYes (pclient, (EMV_BITS*)EMV_TVR, "TVR");

				for (i = 0; i < 5; i++)
				{
					EMV_RESULT[i] = (EMV_TVR_TACONLINE[i] | EMV_TVR_IACONLINE[i]) & EMV_TVR[i];
					if (EMV_RESULT[i])
					{
						OnlineAction = TRUE;
					}
				}
				if (OnlineAction)
				{
					pclient->Cryptogram = EMV_CRYPTO_TYPE_ARQC;

					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "***ACTION RESULT : bits matches with TVR for online\n --REASON(S) :");
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}

					EMVTraceTVROnlyYes (pclient, (EMV_BITS*)EMV_RESULT, "");
					EMVTraceCryptogramType (pclient);
					return EMV_TERMINATED;
				}
				if (pemv->DebugEnabled)
				{
					char strace[1000];
					memset(strace, 0, 1000);
					sprintf(strace, "***ACTION RESULT : No bits matches with TVR for online. Let's check if we can approve\n\n");
					s_printf(smessage, pclient, "%s", strace);
					Send_Info(EMVRooterCom, pclient, "INFO", strace);
				}
				return EMV_OK;
			}
			
			int	EMVCheckActionIfApproved (EMV* pemv, EMVClient* pclient)
			{
				pclient->SubStep = EMV_SUBSTEP_ACTION_IF_APPROVED;
				EMVTraceSubStep (pclient);

				pclient->Cryptogram = EMV_CRYPTO_TYPE_ARQC;
				EMVTraceCryptogramType (pclient);
				return EMV_OK;

			}
//========================

int EMVCardActionAnalysis (EMV* pemv, EMVClient* pclient) 
{
	pclient->Step = EMV_STEP_CARD_ACTION_ANALYSIS;
	EMVTraceStep (pclient);

	EMVGenerateACFirst (pemv, pclient);

	EMVSetTSI (pemv, pclient, Card_risk_management_was_performed, 1);	

	return EMV_OK;
}
//========================
			int	EMVGenerateACFirst (EMV* pemv, EMVClient* pclient)
			{
				unsigned char* pdolTagValue;
				int pdolTagSize;
				unsigned char dolComposed[256];
				int dolComposedSize = 0;
				int lcSize = 0;

				pclient->SubStep = EMV_SUBSTEP_GENERATE_AC_FIRST;
				EMVTraceSubStep (pclient);
				


				EMVTraceDOL (pclient, TAG_CDOL_1);
				
				if (pemv->DebugEnabled)
				{
					char strace[1000];
					memset(strace, 0, 1000);
					sprintf(strace, "%5s%s", "", "Make data from DOL list. If no data in buffer - fill zeros\n");
					s_printf(smessage, pclient, "%s", strace);
					Send_Info(EMVRooterCom, pclient, "INFO", strace);
				}
				pdolTagValue = TLVGetTag(pclient->pTLV, TAG_CDOL_1, &pdolTagSize);
				if (pdolTagValue)
				{
					dolComposedSize = TLVDol(pclient->pTLV, pdolTagValue, pdolTagSize, dolComposed);
					if (dolComposedSize > 0)
					{
						if (pemv->DebugEnabled)
						{
							char strace[1000];
							memset(strace, 0, 1000);
							sprintf(strace, ">>GENERATE_AC : Generate AC First\n");
							s_printf(smessage, pclient, "%s", strace);
							Send_Info(EMVRooterCom, pclient, "INFO", strace);
						}

						//now we send a Get Challenge command to the card. The card will return an 8-byte unpredictable number.				
						EMVSendACFirst (pemv, pclient, 0x80, INS_GENERATE_AC, pclient->Cryptogram, 0x00, dolComposedSize, dolComposed);
					}
				}
//				EMVGetICCDynamicNumber (pemv, pclient);

				return EMV_OK;
			}

//========================

int EMVOnlineOfflineDecision (EMV* pemv, EMVClient* pclient) 
{	
    
	pclient->Step = EMV_STEP_ONLINE_OFFLINE_DECISION;
	EMVTraceStep (pclient);
	
	EMVTraceCID (pclient);

    if (EMVCardReturnAAC (pclient)) 
    {
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "%5s%s", "", "Card Response on GENERATE AC 1 : Decline Transaction\n");
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
		}
		EMVTransactionCompletion (pemv, pclient, 1);
		return EMV_OK;
    }

    if (EMVCardReturnARQC (pclient)) 
    {
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "%5s%s", "", "Card Response on GENERATE AC 1 : Online Transaction\n");
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
		}
        EMVOnlineProcessing (pemv, pclient);
		return EMV_OK;
    }

    if (EMVCardReturnTC (pclient)) 
    {
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "%5s%s", "", "Card Response on GENERATE AC 1 : Accept Transaction\n");
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
		}
		EMVTransactionCompletion(pemv, pclient, 0);
		return EMV_OK;
    }

	return EMV_OK;
}


int EMVOnlineProcessing (EMV* pemv, EMVClient* pclient) 
{
	
	pclient->Step = EMV_STEP_ONLINE_PROCESSING;
	EMVTraceStep (pclient);
	
	
	pclient->pAutorisationMessage = CB2AInitMessage (pemv->pCB2A, "0100");

// A DEPLACER	
	EMVSetTag(pclient, TAG_AUTHORISATION_CODE,					    Online_approved,2);
	

	EMVConnectToAcquirer (pemv, pclient);
	
	if (EMVSendMessageToAcquirer (pemv, pclient, pclient->pAutorisationMessage) < 0)
		EMVScriptProcessing (pemv, pclient);

	return EMV_OK;
}

//========================

			EMVAcquirerConnection* EMVConnectToAcquirer (EMV* pemv, EMVClient* pclient)
			{
				EMVApplication* pApplication = pclient->pApplication;
				List*	AcquirersConnectionList = pemv->pAcquirer->Connections;
				
				
				pclient->SubStep = EMV_SUBSTEP_CONNECT_ACQUIRER;
				EMVTraceSubStep (pclient);
				

				while (AcquirersConnectionList)
				{
					EMVAcquirerConnection* pAcquirerConnection = (EMVAcquirerConnection*)AcquirersConnectionList->car;
					MXCom* pcom = MXOpenTCP (pemv->pMX, pAcquirerConnection->Host, pAcquirerConnection->Port, IPPROTO_CB2A, NULL, NULL, TRUE);
					pAcquirerConnection->pCom = pcom;
					
					if (pcom) 
					{
						pemv->pAcquirer->pConnection = pAcquirerConnection;
						MXAddComCallBack (pemv->pMX, pcom, "CB2A", "Reply", MXONRECV, OnRecvReply, pclient);
						return pAcquirerConnection;
					}
					AcquirersConnectionList = AcquirersConnectionList->cdr;
				}
				return NULL;
			}


			int EMVSendMessageToAcquirer (EMV* pemv, EMVClient* pclient, CB2AMessage* pcb2amessage)
			{
				char strdate[7];
				char strtime[7];
				BYTE dcbstrdatetime[5];

				MXMessage* pmessage = NULL;
				BUFFERPARM OutBuffer;
				int totalsize;

				pclient->SubStep = EMV_SUBSTEP_PREPARE_AND_SEND_TO_ACQUIRER;
				EMVTraceSubStep (pclient);

				DateGetLocalDateHour (strdate, strtime);	
				CharArrayToBCDArray (strdate+2, 4, 4, "n", dcbstrdatetime);
				CharArrayToBCDArray (strtime, 6, 6, "n",   &dcbstrdatetime[2]);
				EMVSetTag(pclient, TAG_TRANSMISSION_DATETIME, dcbstrdatetime, 5);

				if (pemv->pAcquirer == NULL || pemv->pAcquirer->pConnection == NULL)
				{
					
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "Can't connect to any Acquirer\n");
						s_printf(smessage, pclient, "%s", strace);
					}
					return -1;
				}
				totalsize = EMVFillMessageFields (pemv, pclient->pAutorisationMessage, pclient->pTLV);
				EMVTraceCB2AMessage (pclient, pclient->pAutorisationMessage);
				return -1;
				pmessage = 	MXPutMessage (pemv->pAcquirer->pConnection->pCom, "CB2A", "Stream");
				MXSetValue(pmessage, "Identifier",		1,	&pcb2amessage->Identifier[0]) ;
				MXSetValue(pmessage, "Identifier",		2,	&pcb2amessage->Identifier[1]) ;
				
				
				OutBuffer.BufferType    = 'B';
				OutBuffer.BufferSize    = 10;
				OutBuffer.BufferContent = (char*)malloc (10);
				memset ( OutBuffer.BufferContent, 0, 10);

				MXSetValue (pmessage, "Buffer",  1, &OutBuffer);

				free (OutBuffer.BufferContent);



				return 1;
			}

//========================

int EMVScriptProcessing(EMV* pemv, EMVClient* pclient) 
{
	pclient->Step = EMV_STEP_SCRIPT_PROCESSING;
	EMVTraceStep (pclient);

	EMVCheckIssuerScriptsTemplate (pemv, pclient);
	EMVPostIssuanceCommands (pemv, pclient);
	
	EMVSetTSI (pemv, pclient, Script_processing_was_performed, 1);		


	EMVTransactionCompletion (pemv, pclient, 0);

	return EMV_OK;
}
//========================

			int	EMVCheckIssuerScriptsTemplate (EMV* pemv, EMVClient* pclient)
			{

				pclient->SubStep = EMV_SUBSTEP_CHECK_ISSUER_SCRIPTS_TEMPLATE;
				EMVTraceSubStep (pclient);
				return EMV_OK;
			}
			int	EMVPostIssuanceCommands (EMV* pemv, EMVClient* pclient)
			{
				pclient->SubStep = EMV_SUBSTEP_POST_ISSUANCE_COMMANDS;
				EMVTraceSubStep (pclient);
				return EMV_OK;
			}
//========================


int EMVTransactionCompletion(EMV* pemv, EMVClient* pclient, int forceterminate) 
{
	pclient->Step = EMV_STEP_TRANSACTION_COMPLETION;
	EMVTraceStep (pclient);
	
	if (forceterminate == 1)
	{
		EMVTerminateTransaction(pemv, pclient);
		return EMV_OK;
	}

    if (pclient->pTransaction->POSEntryMode == EMV_POS_ENTRY_MODE_ICC_CVV_can_be_checked)
	    EMVGenerateACSecond (pemv, pclient);	
    else
        EMVTerminateTransaction (pemv, pclient);
	
	return EMV_OK;
}
//========================
			int	EMVGenerateACSecond (EMV* pemv, EMVClient* pclient)
			{
				unsigned char* pdolTagValue;
				int pdolTagSize;
				unsigned char dolComposed[256];
				int dolComposedSize = 0;
				int lcSize = 0;

				pclient->SubStep = EMV_SUBSTEP_GENERATE_AC_SECOND;
				EMVTraceSubStep (pclient);
				


				EMVTraceDOL (pclient, TAG_CDOL_2);
				
				if (pemv->DebugEnabled)
				{
					char strace[1000];
					memset(strace, 0, 1000);
					sprintf(strace, "%5s%s", "", "Make data from DOL list. If no data in buffer - fill zeros\n");
					s_printf(smessage, pclient, "%s", strace);
					Send_Info(EMVRooterCom, pclient, "INFO", strace);
				}

				pdolTagValue = TLVGetTag(pclient->pTLV, TAG_CDOL_2, &pdolTagSize);
				if (pdolTagValue)
				{
					dolComposedSize = TLVDol(pclient->pTLV, pdolTagValue, pdolTagSize, dolComposed);
					if (dolComposedSize > 0)
					{
						if (pemv->DebugEnabled)
						{
							char strace[1000];
							memset(strace, 0, 1000);
							sprintf(strace, ">>GENERATE_AC : Generate AC Second\n");
							s_printf(smessage, pclient, "%s", strace);
							Send_Info(EMVRooterCom, pclient, "INFO", strace);
						}

						//now we send a Get Challenge command to the card. The card will return an 8-byte unpredictable number.				
						EMVSendACSecond (pemv, pclient, 0x80, INS_GENERATE_AC, pclient->Cryptogram, 0x00, dolComposedSize, dolComposed);
					}
				}
//				EMVGetICCDynamicNumber (pemv, pclient);

				return EMV_OK;
			}

			int EMVTerminateTransaction (EMV* pemv, EMVClient* pclient)
			{
				pclient->SubStep = EMV_SUBSTEP_TERMINATE_TRANSACTION;
				EMVTraceSubStep (pclient);

			//	EMVTraceTLV(pclient);
			    EMVEndClient (pemv, pclient);

				return EMV_OK;
			}

			


EMVPointOfSale* EMVInitPointOfSale (EMV* pemv, EMVClient* pclient)
{
	EMVPointOfSale* ppointofsale = (EMVPointOfSale*)malloc(sizeof(EMVPointOfSale));
	memset (ppointofsale, 0, sizeof (EMVPointOfSale));
	return ppointofsale;
}


EMVTransaction* EMVInitTransaction (EMV* pemv, BYTE type, char* currency, char* amount, BYTE Media)
{
	char strdate[7];
	char strtime[7];
	char strdecimal[30];
	char strnumeric[30];
	char stramount[60];
	int i;
	int k = 0;
	int j = 0;
	int decimal = 0;


	EMVTransaction* ptransaction = (EMVTransaction*)malloc(sizeof(EMVTransaction));

	memset (ptransaction, 0, sizeof (EMVTransaction));
	ptransaction->TransactionCurrencyExponent = 2;
	ptransaction->TransactionType = type;


    ptransaction->POSEntryMode = Media;

	memset (strdecimal, 0, sizeof (strdecimal));
	memset (strnumeric, 0, sizeof (strnumeric));
	memset (stramount, 0, sizeof (stramount));
	for (i = 0; i < (int)strlen(amount); i++)
	{
		if (amount[i] == '.' || amount[i] == ',')	decimal = 1;
		else
		{
			if (decimal)
			{
				if (k < ptransaction->TransactionCurrencyExponent) {strdecimal[k] = amount[i]; k++;}
			}
			else {strnumeric[j] = amount[i]; j++;} 
		}
	}
	CharArrayToBCDArray (strnumeric, strlen (strnumeric), 12 - ptransaction->TransactionCurrencyExponent, "n", ptransaction->AmountAuthorized);
	CharArrayToBCDArray (strdecimal, strlen (strdecimal), ptransaction->TransactionCurrencyExponent, "n", ptransaction->AmountAuthorized + 5);

	CharArrayToBCDArray (currency, strlen (currency), 4, "n", ptransaction->TransactionCurrencyCode);
	ptransaction->TransactionType = type;
	memcpy (ptransaction->AmountAuthorisation,  ptransaction->AmountAuthorized, 6);


	DateGetLocalDateHour (strdate, strtime);
	CharArrayToBCDArray (strdate, 6, 6, "n", ptransaction->TransactionDate);
	CharArrayToBCDArray (strtime, 6, 6, "n", ptransaction->TransactionTime);



	return ptransaction;
}


void EMVSetClient(EMV* pemv, EMVClient* pclient)
{
	int outSize;

	TLVClearBuffer(pclient->pTLV);

	// Add default value

	EMVSetTag(pclient, TAG_APPLICATION_CURRENCY_CODE, pemv->ApplicationCurrencyCode, 2);
	EMVSetTag(pclient, TAG_APPLICATION_CURRENCY_EXPONENT, &pemv->ApplicationCurrencyExponent, 1);


	EMVSetTag(pclient, TAG_TVR, (BYTE*)"\x00\x00\x00\x00\x00", 5);
	EMVSetTag(pclient, TAG_TSI, (BYTE*)"\x00\x00", 2);
	EMVSetTag(pclient, TAG_AIP, (BYTE*)"\x00\x00", 2);
	EMVSetTag(pclient, TAG_AUC, (BYTE*)"\x00\x00", 2);
	EMVSetTag(pclient, TAG_CTQ, (BYTE*)"\x00\x00", 2);
	EMVSetTag(pclient, TAG_CVM_RESULTS, (BYTE*)"\x00\x00\x00", 3);

	EMVSetTerminal(pemv, pclient, (EMVTerminal*)pemv->Terminals->car);
	EMVSetPointOfSale(pemv, pclient, pclient->pPointOfSale);
	EMVSetAcceptor(pemv, pclient, pemv->pAcceptor);
	EMVSetAcquirer(pemv, pclient, pemv->pAcquirer);


	pclient->EMV_TVR = (EMV_BITS*)TLVGetTag(pclient->pTLV, TAG_TVR, &outSize);
	pclient->EMV_TSI = (EMV_BITS*)TLVGetTag(pclient->pTLV, TAG_TSI, &outSize);
	pclient->EMV_AIP = (EMV_BITS*)TLVGetTag(pclient->pTLV, TAG_AIP, &outSize);
	pclient->EMV_AUC = (EMV_BITS*)TLVGetTag(pclient->pTLV, TAG_AUC, &outSize);
	pclient->EMV_CVMR = (EMV_BITS*)TLVGetTag(pclient->pTLV, TAG_CVM_RESULTS, &outSize);

}

EMVClient* EMVInitClient (EMV* pemv)
{

	EMVClient* pclient = (EMVClient*)malloc(sizeof(EMVClient));
	pclient->pEMV			= pemv;


	pclient->NumberOfRecordsToRead	= 0;
	pclient->RecordNo		= 0;
	pclient->RecordNumber	= 0;
	pclient->pTLV			= (TLV*)malloc(sizeof(TLV));
	pclient->Cryptogram		= 0xFF;
	pclient->EMV_ATC		= -1;
	pclient->EMV_LATC		= -1;
	pclient->pPointOfSale	= NULL;
	pclient->pTransaction	= NULL;
	pclient->DifferentCurrency	= 0;

	
	pclient->IndexApplicationSelected = -1;

	pclient->pAutorisationMessage = NULL;
	pclient->pRedressementMessage = NULL;
	pclient->pPointOfSale = EMVInitPointOfSale(pemv, pclient);
	TLVInitBuffer(pclient->pTLV);

	strcpy(pclient->UserID, "-1");
	ListNewr(&pemv->Clients, pclient);


	return pclient;
}



void EMVEndClient (EMV* pemv, EMVClient* pclient)
{
	if (!ListMember(pclient, pemv->Clients))
	{
		return; // already removed;
	}
	Send_End(EMVRooterCom, pclient);

	CB2AEndMessage (pclient->pAutorisationMessage);
	CB2AEndMessage (pclient->pRedressementMessage);
	
	ListRemove (&pemv->Clients, pclient);
	TLVEndBuffer (pclient->pTLV);
    

	if (pclient->pTransaction) free (pclient->pTransaction);

	if (pclient->pTLV) free (pclient->pTLV);

	if (pclient->pPointOfSale->pCom) {
		MXFlushCom(pemv->pMX, pclient->pPointOfSale->pCom);
		MXCloseCom(pemv->pMX, pclient->pPointOfSale->pCom);
		free(pclient->pPointOfSale);
	}
	free (pclient);
}


EMVAcquirer* EMVInitAcquirer (EMV* pemv)
{
	EMVAcquirer* pAcquirer = (EMVAcquirer*)malloc (sizeof (EMVAcquirer));
	pAcquirer->pConnection = NULL;
	pAcquirer->Connections = NULL;

	return pAcquirer;
}


void EMVEndAcquirer (EMV* pemv, EMVAcquirer* pacquirer)
{
	while (pacquirer->Connections)
	{
		EMVAcquirerConnection* pAcquirerConnection = (EMVAcquirerConnection*)pacquirer->Connections->car;
		ListRemove (&pacquirer->Connections, pAcquirerConnection);
		free (pAcquirerConnection);
	}
	free (pacquirer);
}



EMVAcceptor* EMVInitAcceptor (EMV* pemv)
{
	EMVAcceptor* pAcceptor = (EMVAcceptor*)malloc (sizeof (EMVAcceptor));
	memset (pAcceptor, 0, sizeof (EMVAcceptor));

	return pAcceptor;
}


void EMVEndAcceptor (EMV* pemv, EMVAcceptor* pacceptor)
{
	free (pacceptor);
}



EMVAcquirerConnection* EMVInitHost (EMV* pemv, EMVAcquirer* pAcquirer, char* host, int port)
{
	EMVAcquirerConnection* pAcquirerConnnection;
	if (!pAcquirer) return NULL;

	pAcquirerConnnection = (EMVAcquirerConnection*)malloc(sizeof(EMVAcquirerConnection));
	pAcquirerConnnection->Port			= port;
	strcpy (pAcquirerConnnection->Host, host);	
	pAcquirerConnnection->Error		= 0;
	pAcquirerConnnection->Output		= NULL;
	pAcquirerConnnection->Input		= NULL;
	

	ListNewr (&pAcquirer->Connections, pAcquirerConnnection);
	return pAcquirerConnnection;
}



EMVApplication* EMVInitApplication (EMV* pemv)
{
	EMVApplication* pApplication = (EMVApplication*)malloc (sizeof(EMVApplication));
	memset (pApplication, 0, sizeof (EMVApplication));

	return pApplication;
}

void EMVEndApplication (EMV* pemv, EMVApplication* papplication)
{

	ListRemove (&pemv->Applications, papplication);
	free (papplication);
}

EMVTerminal* EMVInitTerminal (EMV* pemv)
{
	EMVTerminal* pTerminal = (EMVTerminal*)malloc (sizeof(EMVTerminal));
	memset (pTerminal, 0, sizeof (EMVTerminal));
	return pTerminal;
}

void EMVEndTerminal(EMV* pemv, EMVTerminal* pTerminal)
{
	ListRemove(&pemv->Terminals, pTerminal);
	free(pTerminal);
}

int EMVBuildCandidateListUsingPSE(EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize)
{
	char sfiExists = 0;
	int parseShift_1;
	unsigned short parseTag_1;
	unsigned char* parseData_1;
	int parseSize_1;


	memset(&standartCandidate, 0, sizeof(EMVSelectApplicationInfo));

	// Parse 6F (FCI Template)
	parseShift_1 = TLVParse(outData, outSize - 2, &parseTag_1, &parseData_1, &parseSize_1);
	if (!parseShift_1)
		return EMV_UNKNOWN_ERROR;


	if (parseTag_1 != TAG_FCI_TEMPLATE)
		return EMV_UNKNOWN_ERROR;

	// ICC :            Parse 84 (DF Name),         A5 (FCI Proprietary Template)
	// CONTACTLESS :    Parse 84 (DF Name),         A5 (FCI Proprietary Template),           BF0C - File Control Information (FCI) Issuer Discretionary Data

	while (1)
	{
		int parseShift_2;
		unsigned short parseTag_2;
		unsigned char* parseData_2;
		int parseSize_2;

		parseShift_2 = TLVParse(parseData_1, parseSize_1, &parseTag_2, &parseData_2, &parseSize_2);
		if (!parseShift_2)
			break;


		if (parseTag_2 == TAG_FCI_PROPRIETARY_TEMPLATE || parseTag_2 == TAG_DF_NAME)
		{
			// Parse 88 (SFI of the Directory Elementary File)
			// And save others like 5F2D (Language Preference)
			while (1)
			{
				int parseShift_3;
				unsigned short parseTag_3;
				unsigned char* parseData_3;
				int parseSize_3;

				parseShift_3 = TLVParse(parseData_2, parseSize_2, &parseTag_3, &parseData_3, &parseSize_3);
				if (!parseShift_3)
					break;



				if (parseTag_3 == TAG_FCI_ISSUER_DISCRETIONARY_DATA)
				{
					int parseShift_4;
					unsigned short parseTag_4;
					unsigned char* parseData_4;
					int parseSize_4;
					EMVSelectApplicationInfo currentApplicationInfo;

					parseShift_4 = TLVParse(parseData_3, parseSize_3, &parseTag_4, &parseData_4, &parseSize_4);
					if (!parseShift_4)
						break;

					// Tag must be only 61
					if (parseTag_4 != TAG_APPLICATION_TEMPLATE)
						break;

					// Parse applications info, 4F (ADF Name), 50 (Application Label), etc
					memcpy(&currentApplicationInfo, &standartCandidate, sizeof(EMVSelectApplicationInfo));

					while (1)
					{
						int parseShift_5;
						unsigned short parseTag_5;
						unsigned char* parseData_5;
						int parseSize_5;



						parseShift_5 = TLVParse(parseData_4, parseSize_4, &parseTag_5, &parseData_5, &parseSize_5);
						if (!parseShift_5)
							break;
						EMVSetTag(pclient, parseTag_5, parseData_5, parseSize_5);

						if (parseTag_5 == TAG_ADF_NAME)
						{
							if (parseSize_5 <= 16)
							{
								currentApplicationInfo.DFNameLength = parseSize_5;
								memcpy(currentApplicationInfo.DFName, parseData_5, parseSize_5);
							}
						}
						if (parseTag_5 == TAG_APPLICATION_LABEL)
						{
							if (parseSize_5 <= 16)
								memcpy(currentApplicationInfo.strApplicationLabel, parseData_5, parseSize_5);
						}

						// Tag 9F12 (Application Preferred Name)
						if (parseTag_5 == TAG_APPLICATION_PREFERRED_NAME)
						{
							if (parseSize_5 <= 16)
								memcpy(currentApplicationInfo.strApplicationPreferredName, parseData_5, parseSize_5);
						}

						// Tag 87 (Application Priority Indicator)
						if (parseTag_5 == TAG_APPLICATION_PRIORITY_INDICATOR)
						{
							if (parseSize_5 == 1)
							{
								if (*parseData_5 & 0x80)
									currentApplicationInfo.needCardholderConfirm = 1;
								currentApplicationInfo.priority = *parseData_5 & 0x0F;
							}
						}



						parseData_4 += parseShift_5;
						parseSize_4 -= parseShift_5;
					}
					// Check currentApplicationInfo is candidate and then add to list
					if (pclient->candidateApplicationCount < MAX_CANDIDATE_APPLICATIONS && currentApplicationInfo.DFNameLength > 0
						&& EMVCheckCandidateInApplicationList(pemv, &currentApplicationInfo))
					{
						if (pemv->DebugEnabled)
						{
							char strace[1000];
							memset(strace, 0, 1000);
							sprintf(strace, "Add candidate from PSE: %s\n", currentApplicationInfo.strApplicationLabel);
							s_printf(smessage, pclient, "%s", strace);
							Send_Info(EMVRooterCom, pclient, "RESULT", strace);
						}
						memcpy(pclient->candidateApplications + pclient->candidateApplicationCount, &currentApplicationInfo, sizeof(EMVSelectApplicationInfo));
						pclient->candidateApplicationCount++;
					}
					return EMVApplicationSelection(pemv, pclient);
				}

				// Check SFI tag
				if (parseTag_3 == TAG_SFI)
				{
					if (parseSize_3 != 1)
						return EMV_UNKNOWN_ERROR;
					sfiExists = 1;
					pclient->sfiOfPSE = *parseData_3;
				}

				// Tag 5F2D (Language Preference)
				if (parseTag_3 == TAG_LANGUAGE_PREFERENCE)
				{
					if (parseSize_3 <= 8)
						memcpy(standartCandidate.strLanguagePreference, parseData_3, parseSize_3);
				}

				// Tag 9F11 (Issuer Code Table Index)
				if (parseTag_3 == TAG_ISSUER_CODE_TABLE_INDEX)
				{
					if (parseSize_3 == 1)
						standartCandidate.issuerCodeTableIndex = *parseData_3;
				}

				// Next
				parseData_2 += parseShift_3;
				parseSize_2 -= parseShift_3;
			}
		}

		// Next
		parseData_1 += parseShift_2;
		parseSize_1 -= parseShift_2;
	}

	// SFI must exist
	if (pclient->pTransaction->POSEntryMode == EMV_POS_ENTRY_MODE_ICC_CVV_can_be_checked)
	{
		if (!sfiExists)
			return EMV_UNKNOWN_ERROR;

		// Read record, start from record 1

		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, ">>READ RECORD with SFI: %02X\n", pclient->sfiOfPSE & 0xFF);
			s_printf(smessage, pclient, "%s", strace);
		}

		pclient->sfiOfPSE <<= 3;
		pclient->sfiOfPSE |= 4;

		pclient->SubStep = EMV_SUBSTEP_READ_RECORD_SFI;
		EMVTraceSubStep(pclient);

		pemv->APDU(pemv, pclient, 0x00, INS_READ_RECORD, pclient->RecordNo++, pclient->sfiOfPSE, 0, (unsigned char*)"");
	}
	return EMV_OK;
}

int EMVBuildCandidateListUsingListOfAids (EMV* pemv, EMVClient* pclient)
{
	EMVApplication* pApplicationSelected = NULL;
	int selectionIndicator = 0;
	List* ApplicationList = pemv->Applications;
	

                                                                
    
    pclient->SubStep  = EMV_SUBSTEP_BUILD_CANDIDATE_LIST_WITH_SELECT;
	EMVTraceSubStep (pclient);

	while (ApplicationList)
	{
		pApplicationSelected = (EMVApplication*)ApplicationList->car;
		if (pApplicationSelected->AIDCount != 0)
		{
			pclient->indexApplicationSelected		= selectionIndicator;
			pclient->indexApplicationAidSelected	= 0;
			break;
		}
		selectionIndicator++;
		ApplicationList = ApplicationList->cdr;
	}

	if (pclient->indexApplicationSelected == -1)
	{
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "NO APPLICATION MATCHES !!! \n");
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
		}
		return EMV_OK;
	}
	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, ">>SELECT AID[%d][%d]\n", pclient->indexApplicationSelected, pclient->indexApplicationAidSelected);
		s_printf(smessage, pclient, "%s", strace);
		Send_Info(EMVRooterCom, pclient, "INFO", strace);
	}

	pemv->APDU (pemv, pclient, 0x00, INS_SELECT, 0x04, 0, pApplicationSelected->AID[pclient->indexApplicationAidSelected].Length, pApplicationSelected->AID[pclient->indexApplicationAidSelected].AID);
	return EMV_OK;
}


 

// Process data from SELECT ADF response
// Return EMV_OK or error


int EMVParseSelectADF (EMV* pemv, EMVSelectApplicationInfo* appInfo, unsigned char* rApdu, int rApduSize)
{
	int parseShift_1;
	unsigned short parseTag_1;
	unsigned char* parseData_1;
	int parseSize_1;

	if (appInfo)
		memset(appInfo, 0, sizeof(EMVSelectApplicationInfo));

	// Parse 6F (FCI Template)
	parseShift_1 = TLVParse(rApdu, rApduSize - 2, &parseTag_1, &parseData_1, &parseSize_1);
	if (!parseShift_1)
		return EMV_UNKNOWN_ERROR;
	if (parseTag_1 != 0x6F)
		return EMV_UNKNOWN_ERROR;

	// Parse 84 (DF Name), A5 (FCI Proprietary Template)
	while (1)
	{
		int parseShift_2;
		unsigned short parseTag_2;
		unsigned char* parseData_2;
		int parseSize_2;

		parseShift_2 = TLVParse(parseData_1, parseSize_1, &parseTag_2, &parseData_2, &parseSize_2);
		if (!parseShift_2)
			break;

		// Tag 84 (DF Name)
		if (parseTag_2 == TAG_DF_NAME)
		{
			if (parseSize_2 <= 16)
			{
				if (appInfo)
				{
					memcpy(appInfo->DFName, parseData_2, parseSize_2);
					appInfo->DFNameLength = parseSize_2;
				}
			}
		}

		if (parseTag_2 == TAG_FCI_PROPRIETARY_TEMPLATE)
		{
			// Parse all like 50 (Application Label) etc
			while (1)
			{
				int parseShift_3;
				unsigned short parseTag_3;
				unsigned char* parseData_3;
				int parseSize_3;

				parseShift_3 = TLVParse(parseData_2, parseSize_2, &parseTag_3, &parseData_3, &parseSize_3);
				if (!parseShift_3)
					break;

				// Tag 50 (Application Label)
				if (parseTag_3 == TAG_APPLICATION_LABEL)
				{
					if (parseSize_3 <= 16)
					{
						if (appInfo)
							memcpy(appInfo->strApplicationLabel, parseData_3, parseSize_3);
					}
				}			

				// Tag 87 (Application Priority Indicator)
				if (parseTag_3 == TAG_APPLICATION_PRIORITY_INDICATOR)
				{
					if (parseSize_3 == 1)
					{
						if (appInfo)
						{
							if (*parseData_3 & 0x80)
								appInfo->needCardholderConfirm = 1;
							appInfo->priority = *parseData_3 & 0x0F;
						}
					}
				}

				// Tag 9F38 (PDOL)
				if (parseTag_3 == TAG_PDOL)
				{
				}

				// Tag 5F2D (Language Preference)
				if (parseTag_3 == TAG_LANGUAGE_PREFERENCE)
				{
					if (parseSize_3 <= 8)
					{
						if (appInfo)
							memcpy(appInfo->strLanguagePreference, parseData_3, parseSize_3);
					}
				}

				// Tag 9F11 (Issuer Code Table Index)
				if (parseTag_3 == TAG_ISSUER_CODE_TABLE_INDEX)
				{
					if (parseSize_3 == 1)
					{
						if (appInfo)
							appInfo->issuerCodeTableIndex = *parseData_3;
					}
				}

				// Tag 9F12 (Application Preferred Name)
				if (parseTag_3 == TAG_APPLICATION_PREFERRED_NAME)
				{
					if (parseSize_3 <= 16)
					{
						if (appInfo)
							memcpy(appInfo->strApplicationPreferredName, parseData_3, parseSize_3);
					}
				}

				// Next
				parseData_2 += parseShift_3;
				parseSize_2 -= parseShift_3;
			}
		}

		// Next
		parseData_1 += parseShift_2;
		parseSize_1 -= parseShift_2;
	}

	return EMV_OK;
}


// Get application candidates count (for select)
 int EMVGetApplicationCandidatesCount(EMV* pemv, EMVClient* pclient)
{
	return pclient->candidateApplicationCount;
}

 
// Get candidate using index
 EMVSelectApplicationInfo* EMVGetCandidate(EMV* pemv, EMVClient* pclient, int indexApplication)
{
	// Input parameter wrong
	if (indexApplication < 0 || indexApplication >= pclient->candidateApplicationCount)
		return &pclient->candidateApplications[0];

	return &pclient->candidateApplications[indexApplication];
}

// Transaction flow. Final Selection
// User select application manually or confirm selection application
// indexApplication - index can be from 0 to EMVGetApplicationCandidatesCount() - 1
// For confirm it always = 0
// Result can be:
// EMV_OK - ok, application was selected, call EMVGetProcessingOption to process next step
// EMV_ERROR_TRANSMIT, EMV_UNKNOWN_ERROR
 
 
int EMVSelectApplication (EMV* pemv, EMVClient* pclient, int indexApplication)
{
	// Input parameter wrong
	if (indexApplication < 0 || indexApplication >= pclient->candidateApplicationCount)
	{
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "Wrong selection... Terminated\n");
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
		}
        EMVEndClient (pemv, pclient);
		return EMV_UNKNOWN_ERROR;
	}

	if (pemv->DebugEnabled)
	{
		char strace[1000] = { 0 };
		char applidfname[50] = { 0 };

		CharToHexaChar(pclient->candidateApplications[indexApplication].DFName, &applidfname, pclient->candidateApplications[indexApplication].DFNameLength * 2);
		sprintf(strace, ">>SELECT application index: %d, %s\n", indexApplication, applidfname);
		s_printf(smessage, pclient, "%s", strace);
		Send_Info(EMVRooterCom, pclient, "INFO", strace);
	}

	pclient->IndexApplicationSelected = indexApplication;

	pemv->APDU(pemv, pclient, 0x00, INS_SELECT, 0x04, 0x00, pclient->candidateApplications[indexApplication].DFNameLength,	pclient->candidateApplications[indexApplication].DFName);
	
	return EMV_OK;
}


// Transaction flow. Get processing option
// Result can be:
// EMV_OK - ok, you can process next step
// EMV_NOT_SATISFIED, EMV_ERROR_TRANSMIT, EMV_UNKNOWN_ERROR


 int EMVGetProcessingOption (EMV* pemv, EMVClient* pclient, unsigned char *lcpdolData, int lcSize)
{
	pclient->SubStep = EMV_SUBSTEP_GET_PROCESSING_OPTIONS;
	EMVTraceSubStep(pclient);


	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, ">>GPO : Get Processing Options\n");
		s_printf(smessage, pclient, "%s", strace);
		//Send_Info(EMVRooterCom, pclient, "INFO", strace);
	}
	unsigned char lcData[256] = { 0 };
	memcpy(&lcData, lcpdolData, lcSize);

	pemv->APDU(pemv, pclient, 0x80, INS_GET_PROCESSING_OPTIONS, 0x00, 0x00, lcSize, lcData);
	return EMV_OK;
}

 //To obtain the ICC Dynamic Number we send a Get Challenge command to the card. The card will return an 8-byte unpredictable number.

 
int EMVGetICCDynamicNumber (EMV* pemv, EMVClient* pclient)
{
	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, ">>GET_CHALLENGE : Get ICC Dynamic Number\n");
		s_printf(smessage, pclient, "%s", strace);
		Send_Info(EMVRooterCom, pclient, "INFO", strace);
	}
	pemv->APDU(pemv, pclient, 0x00, INS_GET_CHALLENGE, 0x00, 0x00, 0, (unsigned char*)"");
	return EMV_OK;
}

int EMVGetATC (EMV* pemv, EMVClient* pclient)   // Tag 9F36 Application Transaction Counter
{
	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, ">>GET_COMMAND : Get Application Transaction Counter (ATC)\n");
		s_printf(smessage, pclient, "%s", strace);
		Send_Info(EMVRooterCom, pclient, "INFO", strace);
	}
	EMVSendCommand (pemv, pclient, 0x9F, 0x36);
	return EMV_OK;
}


int EMVGetLastOnlineATC (EMV* pemv, EMVClient* pclient)  //tag 9F13 ATC value of the last transaction that went online
{
	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, ">>GET_COMMAND : Get Last Online Application Transaction Counter (ATC) Register\n");
		s_printf(smessage, pclient, "%s", strace);
		Send_Info(EMVRooterCom, pclient, "INFO", strace);
	}
	EMVSendCommand (pemv, pclient, 0x9F, 0x13);
	return EMV_OK;
}

int EMVGetPTC (EMV* pemv, EMVClient* pclient) // tag 9F17 Number of PIN tries remaining
{
	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, ">>GET_COMMAND : Get Current PIN Try Counter (PTC)\n");
		s_printf(smessage, pclient, "%s", strace);
		Send_Info(EMVRooterCom, pclient, "INFO", strace);
	}
	EMVSendCommand (pemv, pclient, 0x9F, 0x17);
	return EMV_OK;
}

  void EMVSetDebugEnabled (EMV* pemv, char enabled)
{
	pemv->DebugEnabled = enabled;
}


void EMVDestroySettings(EMV* pemv)
{

	while (pemv->Settings.appTags)
	{
		EMVTag* ptag = (EMVTag*)pemv->Settings.appTags->car;
		ListRemove (&pemv->Settings.appTags, ptag);
		free (ptag);
	}
	while (pemv->Settings.appFiles)
	{
		EMVFile* pfile = (EMVFile*)pemv->Settings.appFiles->car;
		ListRemove (&pemv->Settings.appFiles, pfile);
		free (pfile);
	}
}

void EMVSetLibrarySettings(EMV* pemv, EMVSettings* settings)
{
	memcpy(&pemv->Settings, settings, sizeof(EMVSettings));
}






void EMVSetFunctionAPDU(EMV* pemv, MXMessage* (*APDUFunction)(EMV* pemv, EMVClient* pclient, unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2,
								  unsigned char dataSize, const unsigned char* data))
{

	pemv->APDU = APDUFunction;
}


int		EMVAPDU (EMV* pemv, unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2,
									unsigned char dataSize, const unsigned char* data,
									int* outDataSize, unsigned char* outData)
{
	return 1;
}


//RECEPTION PROCESS

int EMVOnRecvPinTryCounter (EMV* pemv, EMVClient* pclient, BYTE p1, BYTE p2, BYTE* outData, int outSize)
{

	if (outSize < 2 || outData[outSize - 2] != 0x90 || outData[outSize - 1] != 0x00)
	{
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "Error in Response to Get Command Pin try Counter\n");
			s_printf(smessage, pclient, "%s", strace);
		}
	}
	else
		pclient->EMV_PTC = outData[0];

	return 1;
}


int EMVOnRecvATC (EMV* pemv, EMVClient* pclient, BYTE p1, BYTE p2, BYTE* outData, int outSize)
{

	if (outSize < 2 || outData[outSize - 2] != 0x90 || outData[outSize - 1] != 0x00)
	{
		if (pemv->DebugEnabled)
		{
			printf("Error in Response to Get Command ATC\n");
		}
		EMVSetTVR (pemv, pclient, Lower_consecutive_offline_limit_exceeded, 1);
		EMVSetTVR (pemv, pclient, Upper_consecutive_offline_limit_exceeded, 1);
		EMVSetTVR (pemv, pclient, New_Card, 0);
	}
	else
		pclient->EMV_ATC = outData[0];

	return 1;
}


int EMVOnRecvLastOnlineATC (EMV* pemv, EMVClient* pclient, BYTE p1, BYTE p2, BYTE* outData, int outSize)
{

	if (outSize < 2  || outData[outSize - 2] != 0x90 || outData[outSize - 1] != 0x00)
	{
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "Error in Response to Get Command LATC\n");
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
		}
		EMVSetTVR (pemv, pclient, Lower_consecutive_offline_limit_exceeded, 1);
		EMVSetTVR (pemv, pclient, Upper_consecutive_offline_limit_exceeded, 1);
		EMVSetTVR (pemv, pclient, New_Card, 0);
	}
	else
		pclient->EMV_LATC = outData[0];

	if (pclient->EMV_LATC >= 0 && pclient->EMV_ATC >= 0)
	{
		int TagSize;
		unsigned char* TagBufUCOL;
		unsigned char* TagBufLCOL;
		int consecutiveofflinetrs = pclient->EMV_ATC - pclient->EMV_LATC;


		TagBufLCOL = TLVGetTag(pclient->pTLV, TAG_LCOL, &TagSize);
		TagBufUCOL = TLVGetTag(pclient->pTLV, TAG_UCOL, &TagSize);
		
		if (consecutiveofflinetrs > (int)TagBufLCOL[0])
			EMVSetTVR (pemv, pclient, Lower_consecutive_offline_limit_exceeded, 1);
		
		if (consecutiveofflinetrs > (int)TagBufUCOL[0])
			EMVSetTVR (pemv, pclient, Upper_consecutive_offline_limit_exceeded, 1);

		if (pclient->EMV_LATC == 0)
			EMVSetTVR (pemv, pclient, New_Card, 1);
	}

	EMVTerminalActionAnalysis (pemv, pclient);
	return 1;
}


int EMVOnRecvACFirst (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize)
{
	unsigned char* TagBufAC;
	unsigned char* TagBufCID;
	unsigned char* TagBufATC;
	int TagACSize;
	int TagCIDSize;
	int TagATCSize;

	int parseShift_1;
	unsigned short parseTag_1;
	unsigned char* parseData_1;
	int parseSize_1;


// TNA 09
	if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x81)
		return EMV_NOT_SUPPORTED;
	
	if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x82)
	{
		if (pemv->DebugEnabled)
		{
			printf("File not found\n");
		}
	}

	parseShift_1 = TLVParse(outData, outSize - 2, &parseTag_1, &parseData_1, &parseSize_1);

	if (!parseShift_1 || (parseTag_1 != TAG_RESPONSE_FORMAT_2 && parseTag_1 != TAG_RESPONSE_FORMAT_1))
		return EMV_TERMINATED;

	if (parseTag_1 == TAG_RESPONSE_FORMAT_2)  //77
	{
		while (1)
		{
			int parseShift_2;
			unsigned short parseTag_2;
			unsigned char* parseData_2;
			int parseSize_2;

			parseShift_2 = TLVParse(parseData_1, parseSize_1, &parseTag_2, &parseData_2, &parseSize_2);
			if (!parseShift_2)
				break;
			EMVSetTag(pclient, parseTag_2, parseData_2, parseSize_2);
			// Next
			parseData_1 += parseShift_2;
			parseSize_1 -= parseShift_2;
		}
		TagBufAC	= TLVGetTag(pclient->pTLV, TAG_AC, &TagACSize);
		TagBufCID	= TLVGetTag(pclient->pTLV, TAG_CID, &TagCIDSize);
		TagBufATC	= TLVGetTag(pclient->pTLV, TAG_ATC, &TagATCSize);

	// Check for mandatory
		if (pemv->DebugEnabled)
		{
			printf("Received AC First Response\n");
			printf ("\n%5s", "");
			EMVTraceTag (pclient, TAG_AC, TagBufAC, TagACSize);
			printf ("%5s", "");
			EMVTraceTag (pclient, TAG_CID, TagBufCID, TagCIDSize);
			printf ("%5s", "");
			EMVTraceTag (pclient, TAG_ATC, TagBufATC, TagATCSize);
		}			

		if (!TagBufAC || !TagBufCID || !TagBufATC)
		{
			if (pemv->DebugEnabled)
			{
				char strace[1000];
				memset(strace, 0, 1000);
				sprintf(strace, "\n%5s\n", "Mandatory Fields Missing.. EMV Terminated");
				s_printf(smessage, pclient, "%s", strace);
				Send_Info(EMVRooterCom, pclient, "INFO", strace);
			}
			return EMV_TERMINATED;
		}

	}
	else
	if (parseTag_1 == TAG_RESPONSE_FORMAT_1) //80
	{
		//The data object is a primitive data object. The value field consists of a concatenation of data objects without tag and length bytes.
		//The data objects are:

		//Cryptogram Information Data (CID)
		//Application Transaction Counter (ATC)
		//Application Cryptogram (AC)
		//Issuer Application Data (IAD) (optional data object)
		int remainsize = 0;

		EMVSetTag(pclient, TAG_CID, outData + 2, 1);
		EMVTraceTag(pclient, TAG_CID, outData + 2, 1);

		EMVSetTag(pclient, TAG_ATC, outData + 3, 2);
		EMVTraceTag (pclient, TAG_ATC, outData + 3, 2);

		EMVSetTag(pclient, TAG_AC, outData + 5, 8);
		EMVTraceTag (pclient, TAG_AC, outData + 5, 8);

		remainsize =  outSize - 2 - (11 + 1 + 1);  // + tag + length	
		
		//IAD is Mandatory ????	
		EMVSetTag(pclient, TAG_IAD, outData + 13, remainsize);
		EMVTraceTag (pclient, TAG_IAD, outData + 13, remainsize);

	}
    pclient->EMV_CID = (EMV_BITS*) TLVGetTag(pclient->pTLV, TAG_CID, &parseSize_1);

	EMVOnlineOfflineDecision (pemv, pclient);
	return EMV_OK;
}

int EMVOnRecvACSecond (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize)
{
	unsigned char* TagBufAC;
	unsigned char* TagBufCID;
	unsigned char* TagBufATC;
	int TagSize;
	int parseShift_1;
	unsigned short parseTag_1;
	unsigned char* parseData_1;
	int parseSize_1;


	if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x81)
		return EMV_NOT_SUPPORTED;
	
	if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x82)
	{
		if (pemv->DebugEnabled)
		{
			printf("File not found\n");
		}
	}

	parseShift_1 = TLVParse(outData, outSize - 2, &parseTag_1, &parseData_1, &parseSize_1);

	if (!parseShift_1 || (parseTag_1 != TAG_RESPONSE_FORMAT_2 && parseTag_1 != TAG_RESPONSE_FORMAT_1))
	{
		printf("\n%5s\n", "Mandatory Fields Missing.. EMV Terminated");
	}

	if (parseTag_1 == TAG_RESPONSE_FORMAT_2)  //77
	{
		while (1)
		{
			int parseShift_2;
			unsigned short parseTag_2;
			unsigned char* parseData_2;
			int parseSize_2;

			parseShift_2 = TLVParse(parseData_1, parseSize_1, &parseTag_2, &parseData_2, &parseSize_2);
			if (!parseShift_2)
				break;
			EMVSetTag(pclient, parseTag_2, parseData_2, parseSize_2);
			// Next
			parseData_1 += parseShift_2;
			parseSize_1 -= parseShift_2;
		}
		TagBufAC	= TLVGetTag(pclient->pTLV, TAG_AC, &TagSize);
		TagBufCID	= TLVGetTag(pclient->pTLV, TAG_CID, &TagSize);
		TagBufATC	= TLVGetTag(pclient->pTLV, TAG_ATC, &TagSize);

	// Check for mandatory
		if (pemv->DebugEnabled)
		{
			printf("Received AC Second Response\n");
			printf ("\n%5s", "");
			EMVTraceTag (pclient, TAG_AC, TagBufAC, TagSize);
			printf ("%5s", "");
			EMVTraceTag (pclient, TAG_CID, TagBufCID, TagSize);
			printf ("%5s", "");
			EMVTraceTag (pclient, TAG_ATC, TagBufATC, TagSize);
		}			

		if (!TagBufAC || !TagBufCID || !TagBufATC)
		{
			if (pemv->DebugEnabled)
			{
				char strace[1000];
				memset(strace, 0, 1000);
				sprintf(strace, "\n%5s\n", "Mandatory Fields Missing.. EMV Terminated");
				s_printf(smessage, pclient, "%s", strace);
				Send_Info(EMVRooterCom, pclient, "INFO", strace);
			}
		}

	}
	else
	if (parseTag_1 == TAG_RESPONSE_FORMAT_1) //80
	{
		//The data object is a primitive data object. The value field consists of a concatenation of data objects without tag and length bytes.
		//The data objects are:

		//Cryptogram Information Data (CID)     9F27
		//Application Transaction Counter (ATC)
		//Application Cryptogram (AC)           9F26
		//Issuer Application Data (IAD) (optional data object)
		int remainsize = 0;

		EMVSetTag(pclient, TAG_CID, outData + 2, 1);
		EMVTraceTag(pclient, TAG_CID, outData + 2, 1);

		EMVSetTag(pclient, TAG_ATC, outData + 3, 2);
		EMVTraceTag(pclient, TAG_ATC, outData + 3, 2);

		EMVSetTag(pclient, TAG_AC, outData + 5, 8);
		EMVTraceTag(pclient, TAG_AC, outData + 5, 8);

		remainsize = outSize - 2 - (11 + 1 + 1);  // + tag + length	

		//IAD is Mandatory ????	
		EMVSetTag(pclient, TAG_IAD, outData + 13, remainsize);
		EMVTraceTag(pclient, TAG_IAD, outData + 13, remainsize);

	}


	return EMV_OK;
}
int	EMVOnRecvVerify (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize)
{

	EMVSetTSI (pemv, pclient, Cardholder_verification_was_performed, 1);		
	
	EMVTerminalRiskManagement (pemv, pclient);

	return EMV_OK;
}

int	EMVOnRecvCardDetectionAndReset (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize)
{
	EMVCardDetectionAndReset (pemv, pclient, (char*)outData);
	EMVCandidateListCreation (pemv, pclient);
	return EMV_OK;
}


int EMVOnRecvCandidateListCreation (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize)
{

	if (pclient->SubStep == EMV_SUBSTEP_ATTEMP_DIRECTORY_LISTING_PSE)
	{
		if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x81)
			return EMV_NOT_SUPPORTED;
	
		if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x82)
		{
			if (pemv->DebugEnabled)
			{
				printf("File not found\n");
			}
		}
		// Only 90 00 is OK otherwise use list of AID
		if (outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00)
		{
			return EMVBuildCandidateListUsingPSE(pemv, pclient, outData, outSize);
		}
		else
		{
			return EMVBuildCandidateListUsingListOfAids(pemv, pclient);
		}
	}
	else
	if (pclient->SubStep == EMV_SUBSTEP_READ_RECORD_SFI)
	{
		int parseShift_4;
		unsigned short parseTag_4;
		unsigned char* parseData_4;
		int parseSize_4;


		if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x81)
			return EMV_NOT_SUPPORTED;

		// 6A 83 is the end
		if (outData[outSize - 2] != 0x90 || outData[outSize - 1] != 0x00)
		{
			printf("No More to Read\n");
			EMVTraceCandidates(pemv, pclient);
			if (pclient->candidateApplicationCount != 0)  // If no candidates found using PSE, build candidates using list of AIDs
			{
				return EMVApplicationSelection(pemv, pclient);
			}
			else
			{
				return EMVBuildCandidateListUsingListOfAids(pemv, pclient);
			}
		}

		// Parse 70
		parseShift_4 = TLVParse(outData, outSize - 2, &parseTag_4, &parseData_4, &parseSize_4);
		if (!parseShift_4)
			return EMV_UNKNOWN_ERROR;
		if (parseTag_4 != 0x70)
			return EMV_UNKNOWN_ERROR;

		pemv->APDU (pemv, pclient,0x00, INS_READ_RECORD, pclient->RecordNo++, pclient->sfiOfPSE, 0, (unsigned char*)"");

		// Parse every tag 61
		while (1)
		{
			int parseShift_5;
			unsigned short parseTag_5;
			unsigned char* parseData_5;
			int parseSize_5;
			EMVSelectApplicationInfo currentApplicationInfo;


			parseShift_5 = TLVParse(parseData_4, parseSize_4, &parseTag_5, &parseData_5, &parseSize_5);
			if (!parseShift_5)
				break;

			// Tag must be only 61
			if (parseTag_5 != TAG_APPLICATION_TEMPLATE)
				break;

			// Parse applications info, 4F (ADF Name), 50 (Application Label), etc
			memcpy(&currentApplicationInfo, &standartCandidate, sizeof(EMVSelectApplicationInfo));
			while (1)
			{
				int parseShift_6;
				unsigned short parseTag_6;
				unsigned char* parseData_6;
				int parseSize_6;

				parseShift_6 = TLVParse(parseData_5, parseSize_5, &parseTag_6, &parseData_6, &parseSize_6);
				if (!parseShift_6)
					break;

				// Tag 4F (ADF Name)
				if (parseTag_6 == TAG_ADF_NAME)
				{
					if (parseSize_6 <= 16)
					{
						currentApplicationInfo.DFNameLength = parseSize_6;
						memcpy(currentApplicationInfo.DFName, parseData_6, parseSize_6);
						
					}
				}

				// Tag 50 (Application Label)
				if (parseTag_6 == TAG_APPLICATION_LABEL)
				{
					if (parseSize_6 <= 16)
						memcpy(currentApplicationInfo.strApplicationLabel, parseData_6, parseSize_6);
				}

				// Tag 9F12 (Application Preferred Name)
				if (parseTag_6 == TAG_APPLICATION_PREFERRED_NAME)
				{
					if (parseSize_6 <= 16)
						memcpy(currentApplicationInfo.strApplicationPreferredName, parseData_6, parseSize_6);
				}

				// Tag 87 (Application Priority Indicator)
				if (parseTag_6 == TAG_APPLICATION_PRIORITY_INDICATOR)
				{
					if (parseSize_6 == 1)
					{
						if (*parseData_6 & 0x80)
							currentApplicationInfo.needCardholderConfirm = 1;
						currentApplicationInfo.priority = *parseData_6 & 0x0F;
					}
				}

				// Next
				parseData_5 += parseShift_6;
				parseSize_5 -= parseShift_6;
			}

			// Check currentApplicationInfo is candidate and then add to list
			if (pclient->candidateApplicationCount < MAX_CANDIDATE_APPLICATIONS && currentApplicationInfo.DFNameLength > 0
				&& EMVCheckCandidateInApplicationList(pemv, &currentApplicationInfo))
			{
				if (pemv->DebugEnabled)
				{
					char strace[1000];
					memset(strace, 0, 1000);
					sprintf(strace, "Add candidate from PSE: %s\n", currentApplicationInfo.strApplicationLabel);
					s_printf(smessage, pclient, "%s", strace);
					Send_Info(EMVRooterCom, pclient, "INFO", strace);
				}

				memcpy(pclient->candidateApplications + pclient->candidateApplicationCount, &currentApplicationInfo, sizeof(EMVSelectApplicationInfo));
				pclient->candidateApplicationCount++;
			}

			// Next
			parseData_4 += parseShift_5;
			parseSize_4 -= parseShift_5;
		}
	}
	else
	if (pclient->SubStep == EMV_SUBSTEP_BUILD_CANDIDATE_LIST_WITH_SELECT)
	{

		int selectAdfParse;
		unsigned char selectionIndicator = 0;
		EMVSelectApplicationInfo currentApplicationInfo;
		EMVApplication* pApplicationSelected = (EMVApplication*)ListGetElt (pemv->Applications, pclient->indexApplicationSelected);


		if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x81)
			return EMV_NOT_SUPPORTED;

			// Skip status codes except 90 00 or 62 83 (blocked)
		if ((outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00) ||
		   (outData[outSize - 2] == 0x62 && outData[outSize - 1] == 0x83))
		{
/*
		if (pclient->selectionIndicator == 0)
		{
			// Skip status codes except 90 00 or 62 83 (blocked)
			if (!(outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00)
				&& !(outData[outSize - 2] == 0x62 && outData[outSize - 1] == 0x83))
				return 0;
		} 
		else
		{
			// Skip status codes except 90 00, 62 xx, 63 xx
			if (!(outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00)
				&& !(outData[outSize - 2] == 0x62) && !(outData[outSize - 2] == 0x63))
				return 0;
		}
*/
			selectAdfParse = EMVParseSelectADF(pemv, &currentApplicationInfo, outData, outSize);
			if (selectAdfParse != EMV_OK)
				return selectAdfParse;

			// DF name must exists
			if (currentApplicationInfo.DFNameLength == 0)
				return 0;

			// Detect match exact
			if (pApplicationSelected->AID[pclient->indexApplicationAidSelected].Length == currentApplicationInfo.DFNameLength
				&& memcmp(pApplicationSelected->AID[pclient->indexApplicationAidSelected].AID, currentApplicationInfo.DFName, currentApplicationInfo.DFNameLength) == 0)
			{
				// Check currentApplicationInfo is candidate and then add to list
				if (candidateApplicationCount < MAX_CANDIDATE_APPLICATIONS && outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00)
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "Add candidate from list AIDs, match exact: %s\n", currentApplicationInfo.strApplicationLabel);
						s_printf(smessage, pclient, "%s", strace);
						Send_Info(EMVRooterCom, pclient, "INFO", strace);
					}
					currentApplicationInfo.indexRID = pclient->indexApplicationSelected;
					currentApplicationInfo.indexAID = pclient->indexApplicationAidSelected;
					memcpy(pclient->candidateApplications + pclient->candidateApplicationCount, &currentApplicationInfo, sizeof(EMVSelectApplicationInfo));
					pclient->candidateApplicationCount++;
				}
			}
	
			// Partial selection
			if (pemv->Settings.appSelectionPartial && pApplicationSelected->AID[pclient->indexApplicationAidSelected].applicationSelectionIndicator
				&& pApplicationSelected->AID[pclient->indexApplicationAidSelected].Length < currentApplicationInfo.DFNameLength
				&& memcmp(pApplicationSelected->AID[pclient->indexApplicationAidSelected].AID, currentApplicationInfo.DFName, pApplicationSelected->AID[pclient->indexApplicationAidSelected].Length) == 0)
			{
				// Check currentApplicationInfo is candidate and then add to list
				if (pclient->candidateApplicationCount < MAX_CANDIDATE_APPLICATIONS && outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00)
				{
					if (pemv->DebugEnabled)
					{
						char strace[1000];
						memset(strace, 0, 1000);
						sprintf(strace, "Add candidate from list AIDs, partial: %s\n", currentApplicationInfo.strApplicationLabel);
						s_printf(smessage, pclient, "%s", strace);
					}
					currentApplicationInfo.indexRID = pclient->indexApplicationSelected;
					currentApplicationInfo.indexAID = pclient->indexApplicationAidSelected;
					memcpy(pclient->candidateApplications + pclient->candidateApplicationCount, &currentApplicationInfo, sizeof(EMVSelectApplicationInfo));
					pclient->candidateApplicationCount++;
				}
			}
	
		}
		//Next AID for the same application or next application

		if (pclient->indexApplicationAidSelected == pApplicationSelected->AIDCount - 1)
		{
			pApplicationSelected = (EMVApplication*)ListGetElt (pemv->Applications,  pclient->indexApplicationSelected + 1);
			pclient->indexApplicationAidSelected = 0;		
			if (!pApplicationSelected) 
			{
				if (pemv->DebugEnabled) 
				{
					char strace[1000];
					memset(strace, 0, 1000);
					sprintf(strace, "No More Terminal AID To Select\n");
					s_printf(smessage, pclient, "%s", strace);
					Send_Info(EMVRooterCom, pclient, "INFO", strace);
				}
				EMVTraceCandidates (pemv, pclient);
				return EMVApplicationSelection (pemv, pclient);
			}
			else
				pclient->indexApplicationSelected++;
		}
		else
			pclient->indexApplicationAidSelected++;

		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "SELECT AID[%d][%d]\n", pclient->indexApplicationSelected, pclient->indexApplicationAidSelected);
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
		}

		pemv->APDU (pemv, pclient, 0x00, INS_SELECT, 0x04, selectionIndicator, pApplicationSelected->AID[pclient->indexApplicationAidSelected].Length, pApplicationSelected->AID[pclient->indexApplicationAidSelected].AID);
	}
	return EMV_OK;
}

int EMVOnRecvApplicationSelection (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize)
{
	EMVApplication* pApplication;
	int indexRID;
	if (pclient->SubStep == EMV_SUBSTEP_SELECT_APPLICATION_FILE)
	{
	// Check if any error
		if (outData[outSize - 2] != 0x90 || outData[outSize - 1] != 0x00)
		{
			// Remove candidate from list
			if (pemv->DebugEnabled)
			{
				char strace[1000];
				memset(strace, 0, 1000);
				sprintf(strace, "Remove candidate from list\n");
				s_printf(smessage, pclient, "%s", strace);
			}
			memmove(pclient->candidateApplications + pclient->IndexApplicationSelected,
					pclient->candidateApplications + (pclient->IndexApplicationSelected + 1),
					(pclient->candidateApplicationCount - pclient->IndexApplicationSelected - 1) * sizeof(EMVSelectApplicationInfo));
			pclient->candidateApplicationCount--;

			EMVTransactionCompletion(pemv, pclient, 1);
    		return EMV_UNKNOWN_ERROR;
		}

		// Store AID
		EMVSetTag(pclient, TAG_TERMINAL_AID, pclient->candidateApplications[pclient->IndexApplicationSelected].DFName, pclient->candidateApplications[pclient->IndexApplicationSelected].DFNameLength);

		// Extract tag to global buffer
		do
		{
			int parseShift_1;
			unsigned short parseTag_1;
			unsigned char* parseData_1;
			int parseSize_1;

			// Parse 6F (FCI Template)
			parseShift_1 = TLVParse(outData, outSize - 2, &parseTag_1, &parseData_1, &parseSize_1);
			if (!parseShift_1)
				break;
			if (parseTag_1 != TAG_FCI_TEMPLATE)
				break;

			// Parse 84 (DF Name), A5 (FCI Proprietary Template)
			while (1)
			{
				int parseShift_2;
				unsigned short parseTag_2;
				unsigned char* parseData_2;
				int parseSize_2;

				parseShift_2 = TLVParse(parseData_1, parseSize_1, &parseTag_2, &parseData_2, &parseSize_2);
				if (!parseShift_2)
					break;

				if (parseTag_2 == TAG_FCI_PROPRIETARY_TEMPLATE)
				{
					// Parse all like 50 (Application Label) etc
					while (1)
					{
						int parseShift_3;
						unsigned short parseTag_3;
						unsigned char* parseData_3;
						int parseSize_3;

						parseShift_3 = TLVParse(parseData_2, parseSize_2, &parseTag_3, &parseData_3, &parseSize_3);
						if (!parseShift_3)
							break;

						if (pemv->DebugEnabled)
						{
							EMVTraceTag(pclient, parseTag_3, parseData_3, parseSize_3);
						}

						EMVSetTag(pclient, parseTag_3, parseData_3, parseSize_3);

						// Next
						parseData_2 += parseShift_3;
						parseSize_2 -= parseShift_3;
					}
				} 
				else
				{
					if (pemv->DebugEnabled)
					{
						EMVTraceTag(pclient, parseTag_2, parseData_2, parseSize_2);
					}

					EMVSetTag(pclient, parseTag_2, parseData_2, parseSize_2);
				}

				// Next
				parseData_1 += parseShift_2;
				parseSize_1 -= parseShift_2;
			}
		} while (0);


		unsigned char lcData[256];
		int lcSize = 0;
		unsigned char* pdolTagValue;
		int pdolTagSize;
		unsigned char dolComposed[256];
		int dolComposedSize = 0;

		pdolTagValue = TLVGetTag(pclient->pTLV, TAG_PDOL, &pdolTagSize);
		if (pdolTagValue)
		{
			dolComposedSize = TLVDol(pclient->pTLV, pdolTagValue, pdolTagSize, dolComposed);
			if (dolComposedSize > 0)
			{
				lcSize = TLVMake(dolComposed, dolComposedSize, TAG_COMMAND_TEMPLATE, lcData);
			}
			EMVTraceDOL(pclient, TAG_PDOL);
		}
		else
		{
			if (pemv->DebugEnabled)
			{
				char strace[1000];
				memset(strace, 0, 1000);
				sprintf(strace, "PDOL is absent\n");
				s_printf(smessage, pclient, "%s", strace);
				Send_Info(EMVRooterCom, pclient, "INFO", strace);
			}
			memset(lcData, 0, 20);
			lcData[0] = 0x83;
			lcData[1] = 0x00;

			lcSize = 2;
			//	var pdol = new ByteString("83 0B 00 00 00 00 00 00 00 00 00 00 00", HEX);
		}


		indexRID = pclient->candidateApplications[pclient->IndexApplicationSelected].indexRID;
		pApplication = (EMVApplication*)ListGetElt (pemv->Applications, indexRID);
		pApplication->IndexAIDSelected = pclient->candidateApplications[pclient->IndexApplicationSelected].indexAID;
		EMVSetApplication (pemv, pclient, pApplication);

		// Application selected ok, get processing option
		EMVGetProcessingOption(pemv, pclient, &lcData, lcSize);
	}
	else
	if (pclient->SubStep == EMV_SUBSTEP_GET_PROCESSING_OPTIONS)
	{
		int processingOptionResult = EMV_UNKNOWN_ERROR;
		do
		{
			int parseShift_1;
			unsigned short parseTag_1;
			unsigned char* parseData_1;
			int parseSize_1;

			if (outData[outSize - 2] == 0x69 && outData[outSize - 1] == 0x85)
			{
				processingOptionResult = EMV_NOT_SATISFIED;
				break;
			}
			if (outData[outSize - 2] != 0x90 || outData[outSize - 1] != 0x00)
			{
				processingOptionResult = EMV_UNKNOWN_ERROR;
				break;
			}		

			// Parse 6F (FCI Template)
			parseShift_1 = TLVParse(outData, outSize - 2, &parseTag_1, &parseData_1, &parseSize_1);
			if (!parseShift_1)
			{
				processingOptionResult = EMV_UNKNOWN_ERROR;
				break;
			}

			// Format 1
			if (parseTag_1 == TAG_RESPONSE_FORMAT_1)
			{
				if (parseSize_1 < 6 || (parseSize_1 - 2) % 4 != 0)
				{   
					processingOptionResult = EMV_UNKNOWN_ERROR;
					break;
				}
				// [2 bytes AIP][N bytes AFL]

				EMVSetTag(pclient, TAG_AIP, parseData_1, 2);
				EMVTraceAIP(pclient);

				EMVSetTag(pclient, TAG_AFL, parseData_1 + 2, parseSize_1 - 2);

				if (pemv->DebugEnabled)
				{
					EMVTraceHexaBuffer (pclient->pEMV, "AIP: ", parseData_1, 2, "\n");
					EMVTraceHexaBuffer (pclient->pEMV, "AFL: ", parseData_1 + 2, parseSize_1 - 2, "\n");
				}
				processingOptionResult = EMV_OK;
				break;
			}

			// Format 2
			if (parseTag_1 == TAG_RESPONSE_FORMAT_2)
			{
				int tagSize;
				unsigned char* tagValue;
				char aipExist;
				aipExist = 0;

				// Parse AIP, AFL
				while (1)
				{
					int parseShift_2;
					unsigned short parseTag_2;
					unsigned char* parseData_2;
					int parseSize_2;

					parseShift_2 = TLVParse(parseData_1, parseSize_1, &parseTag_2, &parseData_2, &parseSize_2);
					if (!parseShift_2)
						break;

					if (parseTag_2 == TAG_AIP)
					{
						if (parseSize_2 != 2)
							break;
						aipExist = 1;
					
					}

					EMVSetTag(pclient, parseTag_2, parseData_2, parseSize_2);

					// Next
					parseData_1 += parseShift_2;
					parseSize_1 -= parseShift_2;
				}

				tagValue = TLVGetTag(pclient->pTLV, TAG_AIP, &tagSize);
				if (!aipExist)
				{
					processingOptionResult = EMV_UNKNOWN_ERROR;
					break;
				}
				EMVTraceAIP(pclient);
				if (pemv->DebugEnabled)
				{
					EMVTraceHexaBuffer(pclient->pEMV, "AIP: ", tagValue, tagSize, "\n");
				}

				tagValue = TLVGetTag(pclient->pTLV,  TAG_AFL, &tagSize);
                if (pclient->pTransaction->POSEntryMode == EMV_POS_ENTRY_MODE_ICC_CVV_can_be_checked)
                {
                    if (!tagValue || tagSize % 4 != 0)
				    {
					    processingOptionResult = EMV_UNKNOWN_ERROR;
					    break;
				    }
					if (pemv->DebugEnabled)
					{
						EMVTraceHexaBuffer(pclient->pEMV, "AFL: ", tagValue, tagSize, "\n");
					}
                }
				processingOptionResult = EMV_OK;
				break;
			}

			// Tag unknown
			processingOptionResult = EMV_UNKNOWN_ERROR;
			break;
		} while (0);
  
		if (processingOptionResult != EMV_OK)
		{
			// Remove candidate from list
			if (pemv->DebugEnabled)
			{
				char strace[1000];
				memset(strace, 0, 1000);
				sprintf(strace, "Remove candidate from list\n");
				s_printf(smessage, pclient, "%s", strace);
			}

			memmove(pclient->candidateApplications + pclient->IndexApplicationSelected,
					pclient->candidateApplications + (pclient->IndexApplicationSelected + 1),
					(pclient->candidateApplicationCount - pclient->IndexApplicationSelected - 1) * sizeof(EMVSelectApplicationInfo));
			pclient->candidateApplicationCount--;
			EMVTransactionCompletion(pemv, pclient, 1);

		}	
		else
        {
            if (pclient->pTransaction->POSEntryMode == EMV_POS_ENTRY_MODE_ICC_CVV_can_be_checked)
			    EMVReadApplicationData(pemv, pclient);
            else
            {
                EMVTransactionCompletion (pemv, pclient, 1);
            }


        }
		
		return processingOptionResult;
	}
	return EMV_OK;
}


int EMVOnRecvReadApplicationData (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize)
{
	int parseShift_1;
	unsigned short parseTag_1;
	unsigned char* parseData_1;
	int parseSize_1;



	if (outData[outSize - 2] != 0x90 || outData[outSize - 1] != 0x00) {
		return EMV_TERMINATED;
	}

	// Parse 70
	parseShift_1 = TLVParse(outData, outSize - 2, &parseTag_1, &parseData_1, &parseSize_1);
	if (!parseShift_1) {
		return EMV_UNKNOWN_ERROR;
	}
	if (parseTag_1 != TAG_READ_RECORD_RESPONSE_TEMPLATE) {
		return EMV_UNKNOWN_ERROR;
	}

	// Parse data in records
	while (1)
	{
		int parseShift_2;
		unsigned short parseTag_2;
		unsigned char* parseData_2;
		int parseSize_2;

		parseShift_2 = TLVParse(parseData_1, parseSize_1, &parseTag_2, &parseData_2, &parseSize_2);
		if (!parseShift_2)
			break;


		EMVSetTag(pclient, parseTag_2, parseData_2, parseSize_2);

		// Next
		parseData_1 += parseShift_2;
		parseSize_1 -= parseShift_2;
	}
	pclient->RecordNumber++;

	if (pclient->NumberOfRecordsToRead == pclient->RecordNumber)
	{

		BYTE* EMV_TVR_IACONLINE = (BYTE*)TLVGetTag(pclient->pTLV, TAG_ISSUER_ACTION_CODE_ONLINE, &outSize);
		BYTE* EMV_TVR_IACDENIAL = (BYTE*)TLVGetTag(pclient->pTLV, TAG_ISSUER_ACTION_CODE_DENIAL, &outSize);
		BYTE* EMV_TVR_IACDFAULT = (BYTE*)TLVGetTag(pclient->pTLV, TAG_ISSUER_ACTION_CODE_DEFAULT, &outSize);
		EMVTraceTVR(pclient, (EMV_BITS*)EMV_TVR_IACDENIAL, "IAC_Denial");
		EMVTraceTVR(pclient, (EMV_BITS*)EMV_TVR_IACONLINE, "IAC_Online");
		EMVTraceTVR(pclient, (EMV_BITS*)EMV_TVR_IACDFAULT, "IAC_Default");

		EMVCompleteEquivalentDataTags (pemv, pclient);
		EMVCheckMandatoryData (pemv, pclient);
		EMVCheckMissingData (pemv, pclient);
		EMVDataAuthentification (pemv, pclient);
	}
	return EMV_OK;
}


int EMVBuildCandidateList (EMV* pemv)
{
	unsigned short endianNumber;
	char isBigEndian;

	// Detect endiannes
	endianNumber = 1; /* 0x0001 */
	isBigEndian = *((unsigned char *) &endianNumber) == 0 ? 1 : 0;

	if (isBigEndian)
	{
		#ifndef BIG_ENDIAN
			printf("Your system is big endian model, please compile with define BIG_ENDIAN\n");
			return EMV_UNKNOWN_ERROR;
		#endif
	}


	candidateApplicationCount = 0;
	indexApplicationSelected = 0;

	// Try to use PSE method
	// SELECT ‘1PAY.SYS.DDF01’
	if (pemv->Settings.appSelectionUsePSE)
	{
		int outSize;
		unsigned char outData[256];
		if (pemv->DebugEnabled)
		{
			printf(">>SELECT 1PAY.SYS.DDF01\n");
		}

		if (!EMVAPDU(pemv, 0x00, INS_SELECT, 0x04, 0x00, 14, (const unsigned char*)"1PAY.SYS.DDF01", &outSize, outData)) {
			return EMV_ERROR_TRANSMIT;
		}
		
		if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x81) {
			return EMV_NOT_SUPPORTED;
		}
		
		if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x82)
		{
			if (pemv->DebugEnabled)
			{
				printf("PSE not found\n");
			}
		}

		// Only 90 00 is OK otherwise use list of AID
		if (outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00)
		{
			unsigned char sfiOfPSE;
			char sfiExists;
			unsigned char recordNo;
			EMVSelectApplicationInfo standartCandidate;

			int parseShift_1;
			unsigned short parseTag_1;
			unsigned char* parseData_1;
			int parseSize_1;

			sfiExists = 0;
			memset(&standartCandidate, 0, sizeof(EMVSelectApplicationInfo));

			// Parse 6F (FCI Template)
			parseShift_1 = TLVParse(outData, outSize - 2, &parseTag_1, &parseData_1, &parseSize_1);
			if (!parseShift_1) {
				return EMV_UNKNOWN_ERROR;
			}
			if (parseTag_1 != TAG_FCI_TEMPLATE) {
				return EMV_UNKNOWN_ERROR;
			}

			// Parse 84 (DF Name), A5 (FCI Proprietary Template)
			while (1)
			{
				int parseShift_2;
				unsigned short parseTag_2;
				unsigned char* parseData_2;
				int parseSize_2;

				parseShift_2 = TLVParse(parseData_1, parseSize_1, &parseTag_2, &parseData_2, &parseSize_2);
				if (!parseShift_2)
					break;

				if (parseTag_2 == TAG_FCI_PROPRIETARY_TEMPLATE)
				{
					// Parse 88 (SFI of the Directory Elementary File)
					// And save others like 5F2D (Language Preference)
					while (1)
					{
						int parseShift_3;
						unsigned short parseTag_3;
						unsigned char* parseData_3;
						int parseSize_3;

						parseShift_3 = TLVParse(parseData_2, parseSize_2, &parseTag_3, &parseData_3, &parseSize_3);
						if (!parseShift_3)
							break;

						// Check SFI tag
						if (parseTag_3 == TAG_SFI)
						{
							if (parseSize_3 != 1) {
								return EMV_UNKNOWN_ERROR;
							}
							sfiExists = 1;
							sfiOfPSE = *parseData_3;
						}

						// Tag 5F2D (Language Preference)
						if (parseTag_3 == TAG_LANGUAGE_PREFERENCE)
						{
							if (parseSize_3 <= 8)
								memcpy(standartCandidate.strLanguagePreference, parseData_3, parseSize_3);
						}

						// Tag 9F11 (Issuer Code Table Index)
						if (parseTag_3 == TAG_ISSUER_CODE_TABLE_INDEX)
						{
							if (parseSize_3 == 1)
								standartCandidate.issuerCodeTableIndex = *parseData_3;
						}

						// Next
						parseData_2 += parseShift_3;
						parseSize_2 -= parseShift_3;
					}
				}

				// Next
				parseData_1 += parseShift_2;
				parseSize_1 -= parseShift_2;
			}

			// SFI must exist
			if (!sfiExists) {
				return EMV_UNKNOWN_ERROR;
			}

			// Read record, start from record 1
			recordNo = 1;
			if (pemv->DebugEnabled)
			{
				printf(">>READ RECORD with SFI: %02X\n", sfiOfPSE & 0xFF);
			}

			sfiOfPSE <<= 3;
			sfiOfPSE |= 4;
			while (1)
			{
				int parseShift_4;
				unsigned short parseTag_4;
				unsigned char* parseData_4;
				int parseSize_4;

				if (!EMVAPDU(pemv, 0x00, INS_READ_RECORD, recordNo, sfiOfPSE, 0, (const unsigned char*)"", &outSize, outData)) {
					return EMV_ERROR_TRANSMIT;
				}

				if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x81) {
					return EMV_NOT_SUPPORTED;
				}

				// 6A 83 is the end
				if (outData[outSize - 2] != 0x90 || outData[outSize - 1] != 0x00)
					break;

				// Parse 70
				parseShift_4 = TLVParse(outData, outSize - 2, &parseTag_4, &parseData_4, &parseSize_4);
				
				if (!parseShift_4) {
					return EMV_UNKNOWN_ERROR;
				}
				
				if (parseTag_4 != 0x70) {
					return EMV_UNKNOWN_ERROR;
				}

				// Parse every tag 61
				while (1)
				{
					int parseShift_5;
					unsigned short parseTag_5;
					unsigned char* parseData_5;
					int parseSize_5;
					EMVSelectApplicationInfo currentApplicationInfo;

					parseShift_5 = TLVParse(parseData_4, parseSize_4, &parseTag_5, &parseData_5, &parseSize_5);
					if (!parseShift_5)
						break;

					// Tag must be only 61
					if (parseTag_5 != TAG_APPLICATION_TEMPLATE)
						break;

					// Parse applications info, 4F (ADF Name), 50 (Application Label), etc
					memcpy(&currentApplicationInfo, &standartCandidate, sizeof(EMVSelectApplicationInfo));
					while (1)
					{
						int parseShift_6;
						unsigned short parseTag_6;
						unsigned char* parseData_6;
						int parseSize_6;

						parseShift_6 = TLVParse(parseData_5, parseSize_5, &parseTag_6, &parseData_6, &parseSize_6);
						if (!parseShift_6)
							break;

						// Tag 4F (ADF Name)
						if (parseTag_6 == TAG_ADF_NAME)
						{
							if (parseSize_6 <= 16)
							{
								currentApplicationInfo.DFNameLength = parseSize_6;
								memcpy(currentApplicationInfo.DFName, parseData_6, parseSize_6);
							}
						}

						// Tag 50 (Application Label)
						if (parseTag_6 == TAG_APPLICATION_LABEL)
						{
							if (parseSize_6 <= 16)
								memcpy(currentApplicationInfo.strApplicationLabel, parseData_6, parseSize_6);
						}

						// Tag 9F12 (Application Preferred Name)
						if (parseTag_6 == TAG_APPLICATION_PREFERRED_NAME)
						{
							if (parseSize_6 <= 16)
								memcpy(currentApplicationInfo.strApplicationPreferredName, parseData_6, parseSize_6);
						}

						// Tag 87 (Application Priority Indicator)
						if (parseTag_6 == TAG_APPLICATION_PRIORITY_INDICATOR)
						{
							if (parseSize_6 == 1)
							{
								if (*parseData_6 & 0x80)
									currentApplicationInfo.needCardholderConfirm = 1;
								currentApplicationInfo.priority = *parseData_6 & 0x0F;
							}
						}

						// Next
						parseData_5 += parseShift_6;
						parseSize_5 -= parseShift_6;
					}

					// Check currentApplicationInfo is candidate and then add to list
					if (candidateApplicationCount < MAX_CANDIDATE_APPLICATIONS && currentApplicationInfo.DFNameLength > 0
						&& EMVCheckCandidateInApplicationList(pemv, &currentApplicationInfo))
					{
						if (pemv->DebugEnabled)
						{
							printf("Add candidate from PSE: %s\n", currentApplicationInfo.strApplicationLabel);
						}
						memcpy(candidateApplications + candidateApplicationCount, &currentApplicationInfo, sizeof(EMVSelectApplicationInfo));
						candidateApplicationCount++;
					}

					// Next
					parseData_4 += parseShift_5;
					parseSize_4 -= parseShift_5;
				}

				// Next record number
				recordNo++;
			}
		}
	}

	// If no candidates found using PSE, build candidates using list of AIDs
	if (candidateApplicationCount == 0)
	{
		int i = 0;
		List* applications = pemv->Applications;
		while (applications)
		{
			EMVApplication* application = (EMVApplication *)applications->car;
			int j;
			for (j = 0; j < application->AIDCount; j++)
			{
				unsigned char selectionIndicator = 0;

				// For repeat select for 1 AID
				while (1)
				{
					int outSize;
					unsigned char outData[256];
					int selectAdfParse;
					EMVSelectApplicationInfo currentApplicationInfo;

					// SELECT AID in terminal list
					if (pemv->DebugEnabled)
					{
						printf(">>SELECT AID[%d][%d]\n", i, j);
					}

					if (!EMVAPDU(pemv, 0x00, INS_SELECT, 0x04, selectionIndicator, application->AID[j].Length, application->AID[j].AID, &outSize, outData)) {
						return EMV_ERROR_TRANSMIT;
					}

					if (outData[outSize - 2] == 0x6A && outData[outSize - 1] == 0x81) {
						return EMV_NOT_SUPPORTED;
					}

					if (selectionIndicator == 0)
					{
						// Skip status codes except 90 00 or 62 83 (blocked)
						if (!(outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00)
							&& !(outData[outSize - 2] == 0x62 && outData[outSize - 1] == 0x83))
							break;
					} else
					{
						// Skip status codes except 90 00, 62 xx, 63 xx
						if (!(outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00)
							&& !(outData[outSize - 2] == 0x62) && !(outData[outSize - 2] == 0x63))
							break;
					}

					selectAdfParse = EMVParseSelectADF(pemv, &currentApplicationInfo, outData, outSize);
					if (selectAdfParse != EMV_OK)
						return selectAdfParse;

					// DF name must exists
					if (currentApplicationInfo.DFNameLength == 0)
						break;

					// Detect match exact
					if (application->AID[j].Length == currentApplicationInfo.DFNameLength
						&& memcmp(application->AID[j].AID, currentApplicationInfo.DFName, currentApplicationInfo.DFNameLength) == 0)
					{
						// Check currentApplicationInfo is candidate and then add to list
						if (candidateApplicationCount < MAX_CANDIDATE_APPLICATIONS && outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00)
						{
							if (pemv->DebugEnabled)
							{
								printf("Add candidate from list AIDs, match exact: %s\n", currentApplicationInfo.strApplicationLabel);
							}
							currentApplicationInfo.indexRID = i;
							currentApplicationInfo.indexAID = j;
							memcpy(candidateApplications + candidateApplicationCount, &currentApplicationInfo, sizeof(EMVSelectApplicationInfo));
							candidateApplicationCount++;
						}
					}

					// Partial selection
					if (pemv->Settings.appSelectionPartial && application->AID[j].applicationSelectionIndicator
						&& application->AID[j].Length < currentApplicationInfo.DFNameLength
						&& memcmp(application->AID[j].AID, currentApplicationInfo.DFName, application->AID[j].Length) == 0)
					{
						// Check currentApplicationInfo is candidate and then add to list
						if (candidateApplicationCount < MAX_CANDIDATE_APPLICATIONS && outData[outSize - 2] == 0x90 && outData[outSize - 1] == 0x00)
						{
							if (pemv->DebugEnabled)
							{
								printf("Add candidate from list AIDs, partial: %s\n", currentApplicationInfo.strApplicationLabel);
							}

							currentApplicationInfo.indexRID = i;
							currentApplicationInfo.indexAID = j;
							memcpy(candidateApplications + candidateApplicationCount, &currentApplicationInfo, sizeof(EMVSelectApplicationInfo));
							candidateApplicationCount++;
						}

						// Next selection with current aid
						selectionIndicator = 2;
						continue;
					}

					// Always break
					break;
				}				
			}
			applications = applications->cdr;
			i++;
		}
	}

	return EMV_OK;
}

char EMVCheckCandidateInApplicationList (EMV* pemv, EMVSelectApplicationInfo* candidate)
{
	int i = 0;
	List* applications = pemv->Applications;
	
	while (applications)
	{
		EMVApplication* application = (EMVApplication *)applications->car;
		int j;
		for (j = 0; j < application->AIDCount; j++)
		{
			int smallSize;
			// Detect smaller size AID (from terminal) or DF name (from ICC)

			smallSize = application->AID[j].Length;
			if (smallSize > candidate->DFNameLength)
				smallSize = candidate->DFNameLength;

			if (memcmp(application->AID[j].AID, candidate->DFName, smallSize) == 0)
			{
				// Check exact match
				if (application->AID[j].Length == candidate->DFNameLength)
				{
					candidate->indexRID = i;
					candidate->indexAID = j;
					return 1;
				}
				// Check ASI
				if (pemv->Settings.appSelectionPartial && application->AID[j].applicationSelectionIndicator
					&& application->AID[j].Length < candidate->DFNameLength)
				{
					candidate->indexRID = i;
					candidate->indexAID = j;
					return 1;
				}
			}
		}
		applications = applications->cdr;
		i++;
	}
	return 0;
}

// Transaction flow. Final Selection
// Result can be:
// EMV_OK - ok, application was selected, call EMVGetProcessingOption to process next step
// EMV_NEED_CONFIRM_APPLICATION - cardholder must confirm application before it will be selected, call EMVGetCandidate(0)
// EMV_NEED_SELECT_APPLICATION - cardholder must selection application from application list, call EMVGetApplicationCandidatesCount() and EMVGetCandidate(index)
// EMV_TERMINATED, EMV_ERROR_TRANSMIT, EMV_UNKNOWN_ERROR
// No candidates

int EMVApplicationPriority (EMV* pemv, EMVClient* pclient) 
{
	if (pclient->candidateApplicationCount <= 0) 
    {
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "No Application is Matching\n");
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
		}

		return EMV_TERMINATED;
    }
	// Only one supported application
	if (pclient->candidateApplicationCount == 1)
	{
		if (pclient->candidateApplications[0].needCardholderConfirm)
		{
			if (pemv->DebugEnabled)
			{
				char strace[1000];
				memset(strace, 0, 1000);
				sprintf(strace, "Application need to be confirmed\n");
				s_printf(smessage, pclient, "%s", strace);
			}
			if (pemv->Settings.appSelectionSupportConfirm)
				return EMV_NEED_CONFIRM_APPLICATION;
			else {
				return EMV_TERMINATED;
			}
		} else
		{
			if (pemv->DebugEnabled)
			{
				char strace[1000];
				memset(strace, 0, 1000);
				sprintf(strace, "Select one application automatically\n");
				s_printf(smessage, pclient, "%s", strace);
			}
			return EMVSelectApplication(pemv, pclient, 0);
		}
	}

	// Multi application
	if (pemv->Settings.appSelectionSupport)
	{
		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "User must select application\n");
			s_printf(smessage, pclient, "%s", strace);
			Send_Info(EMVRooterCom, pclient, "INFO", strace);
			return EMV_NEED_SELECT_APPLICATION;
		}

	}

	// Application selection doesn't supported, select auto
	if (pemv->DebugEnabled)
	{
		char strace[1000];
		memset(strace, 0, 1000);
		sprintf(strace, "Select multi applications automatically\n");
		s_printf(smessage, pclient, "%s", strace);
		Send_Info(EMVRooterCom, pclient, "INFO", strace);
	}
	while (1)
	{
		int idx;
		int highestPriority, indexFound;
		int oldApplicationCount;
		int resultSelect;
		highestPriority = 16;
		indexFound = -1;
		oldApplicationCount = pclient->candidateApplicationCount;

		for (idx = 0; idx < pclient->candidateApplicationCount; idx++)
		{
			if (!pclient->candidateApplications[idx].needCardholderConfirm && pclient->candidateApplications[idx].priority < highestPriority)
			{
				// Skip priority is empty
				if (pclient->candidateApplications[idx].priority == 0 && highestPriority != 16)
					continue;
				highestPriority = pclient->candidateApplications[idx].priority;
				indexFound = idx;
			}
		}

		if (indexFound == -1) {
			return EMV_TERMINATED;
		}

		if (pemv->DebugEnabled)
		{
			char strace[1000];
			memset(strace, 0, 1000);
			sprintf(strace, "The highest priority is: %d\n", highestPriority);
			s_printf(smessage, pclient, "%s", strace);
		}

		resultSelect = EMVSelectApplication(pemv, pclient, indexFound);
		if (resultSelect == EMV_OK)
			return resultSelect;
		else if (pclient->candidateApplicationCount < oldApplicationCount)
			continue;
		else
			return resultSelect;
	}

	return EMV_UNKNOWN_ERROR;
}


