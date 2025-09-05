#include "EMV.h"
#include <sys/stat.h>

int tradeprocedure(void* pmessage, BYTE type, char* stream, int size) {
	printf("%.*s", size, stream);
	return 1;
}

int OnCloseHttpServer(MXCom* pcom, void* app) {
	EMVClient* pclient = (EMVClient*)app;

//	printf("disconnected from Http Server\n");
	return 0;
}

int EMVDownloadFile(EMVClient* pclient, char* filename)
{

	MXCom* pcom = MXOpenTCP(pclient->pEMV->pMX, LoginServer, 80, IPPROTO_HTTP, NULL, NULL, TRUE);


	if (!pcom) {
		printf("Can not connect to HTTP Server\n");
		return -1;
	}
	else
	{
	//	MXSetTraceProcedure(pcom, tradeprocedure);
		MXAddEndingProcedure(pcom, OnCloseHttpServer, pclient);
		printf("Connected to HTTP Server Ok\n");
	}

	MXMessage* psend_message;
	MXMessage* precv_message;

	char url[500] = { 0 };
	sprintf(url, "GET /members/%s/EMV/%s/Files/%s HTTP/1.1", pclient->UserID, pclient->pEMV->ProjectName, filename);

	psend_message = MXCreateMessage(pcom->MX, HTTP_SYS, "Request");
	MXSetValue(psend_message, "Request-Line", 1, url);
	MXSetValue(psend_message, "Host", 1, LoginServer);

	MXSetValue(psend_message, "Accept", 1, "image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, application/msword, application/vnd.ms-powerpoint, */*");
	MXSetValue(psend_message, "Accept-Language", 1, "fr");
	MXSetValue(psend_message, "Accept-Encoding", 1, "gzip, deflate");
	MXSetValue(psend_message, "User-Agent", 1, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT)");
	MXSetValue(psend_message, "Proxy-Connection", 1, "Keep-Alive");
	MXSetValue(psend_message, "Pragma", 1, "no-cache");


	MXSend(pcom->MX, pcom, psend_message);
	MXFreeMessage(pcom->MX, psend_message);


	//receive

	precv_message = MXRecv(pcom->MX, pcom);
	if (!precv_message)                                    // connection closed
	{
		return -1;
	}

	STRING Response = (STRING)MXGetValue(precv_message, "Status-Line", 1);
	BUFFERPARM* buffer = MXGetValue(precv_message, "Content", 1);

	char recvfilename[500] = { 0 };

	sprintf(recvfilename, "%s\\Projects\\%s\\Files\\%s", Directory, pclient->pEMV->ProjectName, filename);

	FILE* f = fopen(recvfilename, "wb");

	fwrite(buffer->BufferContent, 1, buffer->BufferSize, f);

	fclose(f);

	free(buffer->BufferContent);
	free(buffer);
	MXFreeMessage(pcom->MX, precv_message);

	return 1;
}


int EMVDownloadProject(EMVClient* pclient) {

	EMVDownloadFile(pclient, "emv_acceptor.conf");
	EMVDownloadFile(pclient, "emv_rangebins.conf");
	EMVDownloadFile(pclient, "emv_exceptioncards.conf");
	EMVDownloadFile(pclient, "emv_authoritypublickeys.conf");
	EMVDownloadFile(pclient, "emv_acceptor.conf");
	EMVDownloadFile(pclient, "emv_applications.conf");
	EMVDownloadFile(pclient, "emv_tacs.conf");
	EMVDownloadFile(pclient, "emv_currencies.conf");
	EMVDownloadFile(pclient, "emvsolution.json");

//	MXCloseCom(pclient->pEMV->pMX, pcom);
	return 1;
}


int EMVLoadProject(EMVClient* pclient, char* projectname, boolean shouldreload)
{

	char dirname[200];
	struct stat info;
	int returnvalue = 0;

	EMV* pemv = (EMV*)pclient->pEMV;


	sprintf(dirname, "%s\\Projects", Directory);

	if (stat(dirname, &info) != 0 || !(info.st_mode & S_IFDIR))
	{

		_mkdir(dirname);
		printf("Folder %s missing ... created \n", dirname);
		returnvalue = -1;
	}

	sprintf(dirname, "%s\\Projects\\%s", Directory, projectname);

	if (stat(dirname, &info) != 0 || !(info.st_mode & S_IFDIR))
	{
		_mkdir(dirname);
		printf("Could not find %s Folder\n", projectname);
		returnvalue = -1;
	}
	sprintf(dirname, "%s\\Projects\\%s\\Files", Directory, projectname);

	if (stat(dirname, &info) != 0 || !(info.st_mode & S_IFDIR))
	{
		_mkdir(dirname);
		printf("Could not find Files Folder\n");
		returnvalue = -1;
	}

	if (returnvalue != 0)
	{
		shouldreload = TRUE;
	}

	strcpy(pemv->ProjectName, projectname);


	if (shouldreload && EMVDownloadProject(pclient) < 0)
	{
		return -1;
	}

	pemv->ApplicationsCount = 0;
	pemv->Applications = 0;

	pemv->Terminals = 0;
	pemv->TerminalsCount = 0;

	memset(&pemv->Settings, 0, sizeof(EMVSettings));
	pemv->Settings.appSelectionUsePSE = 1;
	pemv->Settings.appSelectionSupportConfirm = 1;
	pemv->Settings.appSelectionPartial = 1;
	pemv->Settings.appSelectionSupport = 1;


	if (EMVLoadAcceptor(pemv) < 0) 				//SIT_D753.wp
	{
		printf("Missing File %s\n", "emv_acceptor.conf");
	}
	else
	{
		EMVTraceAcceptor(pemv);
		EMVTraceAcquirer(pemv);
		EMVInitHost(pemv, pemv->pAcquirer, "127.0.0.1", 8000);
	}

	if (EMVLoadRangeBins(pemv) < 0) 			    //APB_D236.wp
	{
		printf("Missing File %s\n", "emv_rangebins.conf");
	}

	if (EMVLoadExceptionCards(pemv) < 0) 		//APL_D253.wp
	{
		printf("Missing File %s\n", "emv_exceptioncards.conf");
	}

	if (EMVLoadAuthorityPublicKeys(pemv) < 0) 	//EPK_D782.wp
	{
		printf("Missing File %s\n", "emv_authoritypublickeys.conf");
	}

	if (EMVLoadTerminals(pemv) < 0)
	{
		printf("Missing File %s\n", "emv_acceptor.conf");
	}

	if (EMVLoadApplications(pemv) < 0) 			//EPV_D787.wp
	{
		printf("Missing File %s\n", "emv_applications.conf");
	}
	else
	{
		EMVTraceApplications(pemv);
	}

	if (EMVLoadTacs(pemv) < 0) 					//EPT_D778.wp
	{
		printf("Missing File %s\n", "emv_tacs.conf");
	}

	if (EMVLoadCurrencies(pemv) < 0) 			//MON_D747.wp
	{
		printf("Missing File %s\n", "emv_currencies.conf");
	}
	return 0;
}

int EMVLoadAcceptor (EMV* pemv)  //TAG DF04  DF20-DF21-DF22-DF23 File index 4
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	pemv->pAcceptor = NULL;

	sprintf (filename, "%s\\Projects\\%s\\Files\\%s", Directory, pemv->ProjectName, "emv_acceptor.conf");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return -1; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "SIT_D753"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "SIT_D753", MXONRECV, OnLoadAcceptor, pemv);
	//Only one line
	pmessage = MXRecv (pemv->pMX, pFileCom);
	MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	return 0;
}

int EMVLoadApplications (EMV* pemv)    //DF17
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	

	sprintf (filename, "%s\\Projects\\%s\\Files\\%s", Directory, pemv->ProjectName, "emv_applications.conf");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return -1; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "EPV_D787"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "EPV_D787", MXONRECV, OnLoadApplications, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	return 0;
	
}
int EMVLoadTacs (EMV* pemv)    //DF18
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	sprintf (filename, "%s\\Projects\\%s\\Files\\%s", Directory, pemv->ProjectName, "emv_tacs.conf");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return -1; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "EPT_D778"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "EPT_D778", MXONRECV, OnLoadTacs, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	return 0;
	
}

int EMVLoadCurrencies (EMV* pemv)    
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	


	sprintf (filename, "%s\\Projects\\%s\\Files\\%s", Directory, pemv->ProjectName, "emv_currencies.conf");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return -1; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "MON_D747"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "MON_D747", MXONRECV, OnLoadCurrencies, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	return 0;
	
}

int EMVLoadExceptionCards (EMV* pemv)
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	pemv->ExceptionCardList = NULL;	

	sprintf (filename, "%s\\Projects\\%s\\Files\\%s", Directory, pemv->ProjectName, "emv_exceptioncards.conf");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return -1; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "APL_D253"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "APL_D253", MXONRECV, OnLoadExceptionCards, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	return 0;
}

int EMVLoadRangeBins (EMV* pemv)
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	pemv->RangBinList = NULL;

	sprintf (filename, "%s\\Projects\\%s\\Files\\%s", Directory, pemv->ProjectName, "emv_rangebins.conf");

	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return -1; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "APB_D236"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "APB_D236", MXONRECV, OnLoadRangeBins, pemv);
	
	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	return 0;
}


int EMVLoadAuthorityPublicKeys (EMV* pemv)
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	
	sprintf (filename, "%s\\Projects\\%s\\Files\\%s", Directory, pemv->ProjectName, "emv_authoritypublickeys.conf");

	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return -1; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "EPK_D782"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "EPK_D782", MXONRECV, OnLoadAuthorityPublicKeys, pemv);
	
	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom);
	return 0;
}

int EMVLoadTerminals(EMV* pemv)
{
	EMVTerminal* pTerminal = EMVInitTerminal(pemv);
	EMVTerminal Terminal = { "12345678", {0x02, 0x50}, {0xC1, 0x00, 0xF0, 0xA0, 0x01}, {0x60, 0xB8, 0xC8}, 0x22, {0x26, 0x40, 0x40, 0x00} };
	*pTerminal = Terminal;
	EMVAddTerminal(pemv, pTerminal);
	return 0;
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


void EMVAddFile(EMV* pemv, EMVFile* pfile)
{
	ListNewr(&pemv->Settings.appFiles, pfile);
}


int EMVGenerateMXFile(EMV* pemv, char* classname, int classindex, char* dialogfilename)
{
	FILE* fileid;
	List* FileList = pemv->Settings.appFiles;



	fileid = fopen(dialogfilename, "w");
	if (!fileid)
		return -1;


	fprintf(fileid, "%s %d *%s*\n", "DIALOGCLASS", classindex, classname);
	fprintf(fileid, "%s\n", "BEGIN");
	while (FileList)
	{
		EMVFile* pemvfile = (EMVFile*)FileList->car;
		if (strcmp(pemvfile->Format, "") != 0)
			fprintf(fileid, "%5s %s (%s)\n", "MESSAGECLASS", pemvfile->Name, pemvfile->Format);
		FileList = FileList->cdr;
	}
	fprintf(fileid, "%s\n", "END");
	fclose(fileid);
	return 1;
}
