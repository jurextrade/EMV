#include "EMV.h"

void EMVAddFile (EMV* pemv, EMVFile* pfile)
{
	ListNewr (&pemv->Settings.appFiles, pfile);
}


int EMVGenerateMXFile (EMV* pemv, char* classname, int classindex, char* dialogfilename)
{
	FILE *  fileid;
	List*	FileList = pemv->Settings.appFiles;

	

    fileid = fopen(dialogfilename, "w");
    if (!fileid)
        return -1;


	fprintf (fileid, "%s %d *%s*\n", "DIALOGCLASS", classindex, classname);
	fprintf (fileid, "%s\n", "BEGIN");
	while (FileList)
	{
		EMVFile* pemvfile = (EMVFile*)FileList->car;
		if (strcmp (pemvfile->Format, "") != 0) 
			fprintf (fileid, "%5s %s (%s)\n", "MESSAGECLASS", pemvfile->Name, pemvfile->Format);
		FileList = FileList->cdr;
	}
	fprintf (fileid, "%s\n", "END");
	fclose (fileid);
	return 1;
}

void EMVLoadAcceptor (EMV* pemv)  //TAG DF04  DF20-DF21-DF22-DF23 File index 4
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	pemv->pAcceptor = NULL;

	sprintf (filename, "%s\\WPAF\\%s", Directory, "SIT_D753.wp");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "SIT_D753"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "SIT_D753", MXONRECV, OnLoadAcceptor, pemv);
	//Only one line
	pmessage = MXRecv (pemv->pMX, pFileCom);
	MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
}

void EMVLoadApplications (EMV* pemv)    //DF17
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	

	sprintf (filename, "%s\\WPAF\\%s", Directory, "EPV_D787.wp");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "EPV_D787"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "EPV_D787", MXONRECV, OnLoadApplications, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	
}
void EMVLoadTacs (EMV* pemv)    //DF18
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	


	sprintf (filename, "%s\\WPAF\\%s", Directory, "EPT_D778.wp");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "EPT_D778"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "EPT_D778", MXONRECV, OnLoadTacs, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	
}

void EMVLoadCurrencies (EMV* pemv)    
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	


	sprintf (filename, "%s\\WPAF\\%s", Directory, "MON_D747.wp");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "MON_D747"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "MON_D747", MXONRECV, OnLoadCurrencies, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	
}

void EMVLoadExceptionCards (EMV* pemv)
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	pemv->ExceptionCardList = NULL;	

	sprintf (filename, "%s\\WPAF\\%s", Directory, "APL_D253.wp");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "APL_D253"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "APL_D253", MXONRECV, OnLoadExceptionCards, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
}

void EMVLoadRangeBins (EMV* pemv)
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	pemv->RangBinList = NULL;

	sprintf (filename, "%s\\WPAF\\%s", Directory, "APB_D236.wp");

	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "APB_D236"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "APB_D236", MXONRECV, OnLoadRangeBins, pemv);
	
	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
}


void EMVLoadAuthorityPublicKeys (EMV* pemv)
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	
	sprintf (filename, "%s\\WPAF\\%s", Directory, "EPK_D782.wp");

	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "EPK_D782"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "EPK_D782", MXONRECV, OnLoadAuthorityPublicKeys, pemv);
	
	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
}

/////////////////////////////////////////////////////////////////////////////////////////////

int OnLoadAcceptor(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVAcceptor* pAcceptor;
	EMVAcquirer* pAcquirer;
	EMV* pemv = (EMV*)applicationfield;

	if (*(BYTE*)MXGetValue(pmessage, "ParOpen", 1) != '[') return 1;

	pAcceptor = EMVInitAcceptor(pemv);
	pAcquirer = EMVInitAcquirer(pemv);



	strncpy(pAcceptor->SIRET, (char*)MXGetValue(pmessage, "SIRET", 1), 14);
	strncpy(pAcceptor->SystemAcceptationIdentification, (char*)MXGetValue(pmessage, "SystemAcceptationIdentification", 1), 8);
	strncpy(pAcceptor->MerchantIdentifier, (char*)MXGetValue(pmessage, "MerchantIdentifier", 1), 15);
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "MerchantCategoryCode", 1), 4, 2, "b", pAcceptor->MerchantCategoryCode);
	strncpy(pAcceptor->MerchantContractNumber, (char*)MXGetValue(pmessage, "MerchantContractNumber", 1), 7);
	strncpy(pAcceptor->TerminalIdentification, (char*)MXGetValue(pmessage, "TerminalIdentification", 1), 8);

	strncpy(pAcceptor->MerchantNameAndLocation, (char*)MXGetValue(pmessage, "MerchantNameAndLocation", 1), 40);  // variable should end with 0

	CharArrayToBCDArray((char*)MXGetValue(pmessage, "AcquirerIdentifier", 1), 11, 11, "n", pAcquirer->AcquirerIdentifier);
	pemv->pAcceptor = pAcceptor;
	pemv->pAcquirer = pAcquirer;

	return 1;
}

int OnLoadApplications(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	BYTE	AID[16];
	BOOL    found = FALSE;
	int		AIDLen = 0;

	char	sPix[23];
	char	sRid[11];
	char	sAid[33];
	BYTE	sPriority[2];
	int		j;
	int		i = 0;


	EMVApplication* pApplication;
	EMV* pemv = (EMV*)applicationfield;

	if (*(BYTE*)MXGetValue(pmessage, "ParOpen", 1) != '[') return 1;

	memset(sRid, 0, 11);
	memset(sPix, 0, 23);
	memset(sAid, 0, 33);
	memset(sPriority, 0, 2);

	strncpy(sRid, (char*)MXGetValue(pmessage, "RID", 1), 10);
	strncpy(sPix, (char*)MXGetValue(pmessage, "PIX", 1), 21);

	strcat(sAid, sRid);
	strcat(sAid, sPix);

	while (i < 32 && sAid[i] != ' ') i++;
	AIDLen = (i + 1) / 2;

	CharArrayToHexaArray(sAid, i, AIDLen, "b", AID);

	pApplication = EMVGetApplicationFromAID(pemv, AID);
	if (!pApplication)
	{
		pApplication = EMVInitApplication(pemv);
		memcpy(pApplication->RID, AID, 5);
		EMVAddApplication(pemv, pApplication);
	}


	for (j = 0; j < pApplication->AIDCount; j++)
		if (pApplication->AID[j].Length == AIDLen && memcmp(pApplication->AID[j].AID, AID, AIDLen) == 0)
			found = TRUE;
	if (!found)
	{
		memcpy(pApplication->AID[j].AID, AID, AIDLen);
		pApplication->AID[j].Length = AIDLen;
		pApplication->AID[j].applicationSelectionIndicator = 1;
		j = pApplication->AIDCount;
		pApplication->AIDCount++;
	}

	CharArrayToHexaArray((char*)MXGetValue(pmessage, "ApplicationVersionNumber", 1), 4, 2, "b", pApplication->AID[j].ApplicationVersionNumber);
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "Priority", 1), 2, 1, "b", sPriority);
	pApplication->AID[j].Priority = sPriority[0];
	pApplication->AID[j].ForceTransaction = *(BYTE*)MXGetValue(pmessage, "ForceTransaction", 1);

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

int OnLoadTacs(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
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

	if (*(BYTE*)MXGetValue(pmessage, "ParOpen", 1) != '[') return 1;


	memset(sRid, 0, 11);
	memset(sPix, 0, 23);
	memset(sAid, 0, 33);



	strncpy(sRid, (char*)MXGetValue(pmessage, "RID", 1), 10);
	strncpy(sPix, (char*)MXGetValue(pmessage, "PIX", 1), 21);

	strcat(sAid, sRid);
	strcat(sAid, sPix);
	while (i < 32 && sAid[i] != ' ') i++;
	AIDLen = (i + 1) / 2;

	CharArrayToHexaArray(sAid, i, AIDLen, "b", AID);

	pApplication = EMVGetApplicationFromAID(pemv, AID);
	if (!pApplication)
	{
		pApplication = EMVInitApplication(pemv);
		memcpy(pApplication->RID, AID, 5);
		EMVAddApplication(pemv, pApplication);
	}


	for (j = 0; j < pApplication->AIDCount; j++)
		if (pApplication->AID[j].Length == AIDLen && memcmp(pApplication->AID[j].AID, AID, AIDLen) == 0)
		{
			found = TRUE;
			break;
		}
	if (!found)
	{
		memcpy(pApplication->AID[j].AID, AID, AIDLen);
		pApplication->AID[j].Length = AIDLen;
		j = pApplication->AIDCount;
		pApplication->AIDCount++;
	}

	CharArrayToHexaArray((char*)MXGetValue(pmessage, "TerminalActionCodeDenial", 1), 10, 5, "b", pApplication->AID[j].TerminalActionCodeDenial);
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "TerminalActionCodeOnline", 1), 10, 5, "b", pApplication->AID[j].TerminalActionCodeOnline);
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "TerminalActionCodeDefault", 1), 10, 5, "b", pApplication->AID[j].TerminalActionCodeDefault);
	return 1;
}

int OnLoadExceptionCards(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVExceptionCard* pExceptionCard;
	EMV* pemv = (EMV*)applicationfield;

	if (*(BYTE*)MXGetValue(pmessage, "ParOpen", 1) != '[') return 1;

	pExceptionCard = (EMVExceptionCard*)malloc(sizeof(EMVExceptionCard));


	CharArrayToHexaArray((char*)MXGetValue(pmessage, "Pan", 1), 20, 10, "b", pExceptionCard->Pan);
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "Status", 1), 1, 1, "b", &pExceptionCard->Status);

	ListInsert(&pemv->ExceptionCardList, pExceptionCard);

	return 1;
}

int OnLoadCurrencies(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	BYTE FirstChar;

	EMV* pemv = (EMV*)applicationfield;
	FirstChar = *(BYTE*)MXGetValue(pmessage, "ParOpen", 1);

	if (*(BYTE*)MXGetValue(pmessage, "ParOpen", 1) != '[') return 1;

	CharArrayToHexaArray((char*)MXGetValue(pmessage, "CurrencyExponent", 1), 1, 1, "b", &pemv->ApplicationCurrencyExponent);
	CharArrayToBCDArray((char*)MXGetValue(pmessage, "CurrencyCode", 1), 3, 3, "n", pemv->ApplicationCurrencyCode);

	return 1;
}

int OnLoadRangeBins(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	int i = 0;
	EMVRangeBin* pRangeBin;
	EMV* pemv = (EMV*)applicationfield;


	if (*(BYTE*)MXGetValue(pmessage, "ParOpen", 1) != '[') return 1;

	pRangeBin = (EMVRangeBin*)malloc(sizeof(EMVRangeBin));

	CharArrayToHexaArray((char*)MXGetValue(pmessage, "FromBin", 1), 19, 10, "b", pRangeBin->FromBin);
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "ToBin", 1), 19, 10, "b", pRangeBin->ToBin);
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "Status", 1), 2, 1, "b", &pRangeBin->Status);
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "TreatmentCode", 1), 2, 1, "b", &pRangeBin->TreatmentCode);

	ListInsert(&pemv->RangBinList, pRangeBin);

	return 1;
}

int OnLoadAuthorityPublicKeys(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	char	sRid[11];
	BYTE	RID[5];
	EMVAuthorityPublicKey* pAuthorityPublicKey;
	EMV* pemv = (EMV*)applicationfield;
	EMVApplication* pApplication;

	if (*(BYTE*)MXGetValue(pmessage, "ParOpen", 1) != '[') return 1;

	pAuthorityPublicKey = (EMVAuthorityPublicKey*)malloc(sizeof(EMVAuthorityPublicKey));

	memset(sRid, 0, 11);

	strncpy(sRid, (char*)MXGetValue(pmessage, "RID", 1), 10);
	CharArrayToHexaArray(sRid, 10, 5, "b", RID);

	pApplication = EMVGetApplicationFromRID(pemv, RID);
	if (!pApplication)
	{
		pApplication = EMVInitApplication(pemv);
		memcpy(pApplication->RID, RID, 5);
		EMVAddApplication(pemv, pApplication);
	}


	CharArrayToHexaArray((char*)MXGetValue(pmessage, "CertificationAuthorityPublicKeyIndex", 1) + 4, 2, 1, "b", &pAuthorityPublicKey->CertificationAuthorityPublicKeyIndex);
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "IssuerPublicKeyExponent", 1), 6, 3, "b", pAuthorityPublicKey->IssuerPublicKeyExponent);
	pAuthorityPublicKey->IssuerPublicKeyModulusSize = atoi((char*)MXGetValue(pmessage, "IssuerPublicKeyModulusSize", 1));
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "IssuerPublicKeyModulus", 1), 512, 256, "b", pAuthorityPublicKey->IssuerPublicKeyModulus);
	CharArrayToHexaArray((char*)MXGetValue(pmessage, "IssuerPublicKeyRemainder", 1), 256, 128, "b", pAuthorityPublicKey->IssuerPublicKeyRemainder);
	ListInsert(&pApplication->AuthorityPublicKeys, pAuthorityPublicKey);

	return 1;

}