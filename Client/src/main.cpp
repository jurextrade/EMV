#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <fstream>
#include <sstream>
#include "Card.h"


char   Directory[300];
char   EMVServer[300]		= "217.112.89.92";
int	   EMVServerPort		= 2000;

char   EMVRooter[300]		= "217.112.89.92";
int	   EMVRooterPort		= 3007;

char   LoginServer[300]	    = "www.jurextrade.com";

MXCom* EMVRooterCom				= NULL;

char   smessage[300];

using namespace std;


void Send_Start(MXCom* pcom) {
	MXMessage* pmessage;
	BUFFERPARM	Buffer;
	char		message[100];

	sprintf(message, "%s*", "*START");

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)&message;

	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);
}


int Send_Login(CC* pcc)
{
	MXMessage*  pmessage;
	BUFFERPARM	Buffer;
	char		message[100];
	int			lReturn = 0;


	printf("Login: ");
	lReturn = scanf("%s", pcc->UserName);

	printf("PassWord: ");
	scanf("%s", pcc->UserPassword);

	printf("Sending Login Indentification\n");
	sprintf(message, "*LOGIN^%s^%s^%s*", LoginServer, pcc->UserName, pcc->UserPassword);

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)&message; // (char*)malloc(strlen(message));
	//memcpy(Buffer.BufferContent, message, dataSize);

	pmessage = MXCreateMessage(pcc->pRouterCom->MX, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	MXSend(pcc->pRouterCom->MX, pcc->pRouterCom, pmessage);

	pmessage = MXRecv(pcc->pRouterCom->MX, pcc->pRouterCom);
	if (!pmessage) {

		printf("Problem with connection\n");
		lReturn = 0;
		return lReturn;
	}
	BUFFERPARM* RecvBuffer = (BUFFERPARM*)MXGetValue(pmessage, "Buffer", 1);
	if (strcmp(RecvBuffer->BufferContent, "1") == 0)
	{
		printf("Indentification Succeeded\n");
		lReturn = 1;
	}
	else
	{
		printf("Indentification Failed\n");
		lReturn = 0;
	}
	free(RecvBuffer->BufferContent);
	free(RecvBuffer);
	return lReturn;
}

void Send_APDU(MXCom* pcom, BYTE cla, BYTE ins, BYTE p1, BYTE p2, int datasize, unsigned char* data, int way)
{
	if (!pcom) {
		return;
	}

	int totalsize;
	char* adpu_buffer;
	char header[20];

	if (way) {		// R-ADPU


		int header_size = strlen("*R-ADPU^");

		totalsize = header_size + (2 * datasize) + 1;

		sprintf(header, "%s", "*R-APDU^");

		adpu_buffer = (char*)malloc(totalsize);

		for (int i = 0, j = 0; i < datasize; ++i, j += 2)
			sprintf(adpu_buffer + header_size + j, "%02X", data[i] & 0xff);

		memcpy(adpu_buffer, header, header_size);
		adpu_buffer[header_size + (2 * datasize)] = '*';

	}
	else {

		int header_size = strlen("*R-ADPU^");
		int apduheader_size = 10;

		totalsize = header_size + apduheader_size + (2 * datasize) + 1;

		sprintf(header, "%s", "*C-APDU^");


		adpu_buffer = (char*)malloc(totalsize);

		memcpy(adpu_buffer, header, header_size);

		int j = 0;
		sprintf(adpu_buffer + header_size + j, "%02X", cla & 0xFF);		 j += 2;
		sprintf(adpu_buffer + header_size + j, "%02X", ins & 0xFF);		 j += 2;
		sprintf(adpu_buffer + header_size + j, "%02X", p1 & 0xFF);		 j += 2;
		sprintf(adpu_buffer + header_size + j, "%02X", p2 & 0xFF);		 j += 2;
		sprintf(adpu_buffer + header_size + j, "%02X", datasize & 0xFF); j += 2;
		for (int i = 0; i < datasize; ++i, j += 2)
			sprintf(adpu_buffer + header_size + j, "%02X", data[i] & 0xff);

		adpu_buffer[header_size + j] = '*';

	}
	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'B';
	Buffer.BufferSize = totalsize;
	Buffer.BufferContent = adpu_buffer;

	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	free(adpu_buffer);

}

void Send_Trace(MXCom* pcom, char* message)
{
	if (!pcom) {
		return;
	}
	int size = strlen(message);
	int header_size = strlen("*TRACE^");

	memcpy(message + header_size, message, size);
	memcpy(message, "*TRACE^", header_size);

	message[size + header_size] = '*';
	message[size + header_size + 1] = 0;



	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)message;


	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

}

void s_printf(char* message, char* format, char* string) 
{
	sprintf(message, format, string);
	printf(message);

	Send_Trace(EMVRooterCom, message);
}

MXCom* Connect_EMVServer(CC* pcc, CARD* pCard) {

	MX* pmx = pcc->pMX;

	MXCom* pCom = MXOpenTCP(pcc->pMX, EMVServer, EMVServerPort, IPPROTO_DG, NULL, NULL, TRUE);

	if (!pCom)
	{
		s_printf(smessage, "%s", "Can not connect to EMV Server \n");
		return NULL;
	}
	s_printf(smessage, "%s", "Open Connection With EMV Server \n");

	MXAddComCallBack(pmx, pCom, "APDU", "C-APDU", MXONRECV, OnRecvAPDU, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "R-APDU", MXONSEND, OnSendAPDU, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendACFirst", MXONRECV, OnRecvACFirst, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "RecvACFirst", MXONSEND, OnSendACFirst, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendACSecond", MXONRECV, OnRecvACSecond, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "RecvACSecond", MXONSEND, OnSendACSecond, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "RecvVerify", MXONSEND, OnSendVerify, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendVerify", MXONRECV, OnRecvVerify, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendATR", MXONSEND, OnSendATR, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendTransaction", MXONSEND, OnSendTransaction, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendAppliSelection", MXONRECV, OnRecvSendAppliSelection, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendCommand", MXONRECV, OnRecvSendCommand, pCard);
	pCard->pCom = pCom;
	return pCom;
}

MXCom* Connect_RouterServer(CC* pcc) {

	MXCom* pCom = MXOpenTCP(pcc->pMX, EMVRooter, EMVRooterPort, IPPROTO_TCP, NULL, NULL, TRUE);
	if (!pCom) {
		printf("Can not connect to EMV Router Server\n");
		return NULL;
	}
	else
	{
		pcc->pRouterCom = pCom;
		printf("Connected to EMV Router Server Ok\n");
	}
	return pCom;
}

int OnConnect(MXCom* pcom, void* applicationfield)
{
	CC* pcc = (CC*)applicationfield;
	CardContext* pCardContext = pcc->pCardContext;
	return 1;
}

int OnClose(MXCom* pcom, void* applicationfield)
{
	CC* pcc = (CC*)applicationfield;
	CardContext* pCardContext = pcc->pCardContext;
	if (pcom != pcc->pRouterCom)
	{
		s_printf (smessage, "%s", "Close Connection With EMV Server \n");
		pCardContext->ShouldRelase = 1;
		pCardContext->pCurrentState->dwCurrentState = 0;

	}
	else
	{
		printf("Close Connection With Rooter Server \n");
		pcc->pRouterCom = NULL;
	}
	return 1;
}

int CardApplicationProcedure(MX* pmx, void* applicationfield)
{
	CC* pcc = (CC*)applicationfield;

	CardContext* pCardContext = pcc->pCardContext;
	char* szReaderList		  = NULL;

	DWORD	dwReaderListSize = 0,
			dwNewState,
			dwOldState;

	HRESULT	hr;
	BOOL	fEvent = FALSE;

	LONG    lReturn;
	

	if (pCardContext->ReadersCount == 0) {
		lReturn = Readers_Init(pCardContext);
		if (lReturn == -1)
		{
			getchar();
			return 0;
		}
	}

	hr = SCardGetStatusChange(pCardContext->hContext, pCardContext->TimeOut, pCardContext->ReadersStates, pCardContext->ReadersCount);
	if (hr != SCARD_S_SUCCESS)
	{
		if (hr != SCARD_E_TIMEOUT)
		{
			s_printf(smessage, "%s\n", CardStrError(hr));
			CardContext_End(pCardContext);													// init context
			pcc->pCardContext = CardContext_Init();													// init context
			return 1;
		}
	}

	if (pCardContext->pCurrentState->dwCurrentState != pCardContext->pCurrentState->dwEventState)   // state of reader not the same
	{
		
		DWORD dwStateMask = ~(	SCARD_STATE_UNAWARE | 
								SCARD_STATE_IGNORE | 
								SCARD_STATE_UNAVAILABLE | 
								SCARD_STATE_ATRMATCH |
								SCARD_STATE_EXCLUSIVE | 
								SCARD_STATE_INUSE |
								SCARD_STATE_MUTE |
								SCARD_STATE_UNPOWERED);


		dwNewState = pCardContext->pCurrentState->dwEventState   &dwStateMask;
		dwOldState = pCardContext->pCurrentState->dwCurrentState &dwStateMask;

		if (dwNewState != dwOldState)
		{
			pCardContext->pCurrentState->dwCurrentState = pCardContext->pCurrentState->dwEventState;
			if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_EMPTY) == SCARD_STATE_EMPTY)
			{
				// Card Removed
				pCardContext->ShouldRelase = 0;

				if (EMVRooterCom == NULL) {
					EMVRooterCom = Connect_RouterServer(pcc);
				}
				s_printf(smessage, "%s\n", "Insert Card");

				//	OnCardDisconnected(pCard);
				//	pCard->TimeOut = INFINITE;
			
			}
			if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_PRESENT) == SCARD_STATE_PRESENT)
			{
				if (pCardContext->ShouldRelase)
				{
					s_printf(smessage, "%s\n", "Transaction Completed please Remove Card");
				} 
				else
				if (OnCardConnected(pcc) != 0) {
					s_printf(smessage, "%s\n", "System Problem Remove Card");
				}
				//	pCard->TimeOut = INFINITE;
				//	pCardContext->pCurrentState->dwCurrentState = pCardContext->pCurrentState->dwEventState;
			}
		}
	}
	return 0;  // 0 means block !
}


int EMVReadApduErrorFile(CardContext* pCardContext)
{
	char filename[200];
	sprintf(filename, "%s\\FILES\\%s", Directory, "SW1SW2.csv");

	ifstream inFile;
	inFile.open(filename);
	if (inFile.fail())
	{
		char error[250];
		sprintf(error, "%s %s", "Can't open File ", filename);
		return -1;
	}

	string line;
	int linenum = 0;
	int k = 0;
	int p = 0;



	while (getline(inFile, line))
	{
		linenum++;
		EMVApduError* pemverror = (EMVApduError*)malloc(sizeof(EMVApduError));
		istringstream linestream(line);
		string item;
		int itemnum = 0;
		if (linenum == 1) continue;
		while (getline(linestream, item, ';'))
		{
			if (itemnum == 0)
				pemverror->SW1 = HexStrToDecimal((char*)item.c_str());
			else
				if (itemnum == 1)
					pemverror->SW2 = HexStrToDecimal((char*)item.c_str());
				else
					if (itemnum == 2)
						strcpy(pemverror->Type, item.c_str());
					else
						if (itemnum == 3)
							strcpy(pemverror->Description, item.c_str());
			itemnum++;
		}
		EMVAddApduError(pCardContext, pemverror);
		k++;
	}

	inFile.close();
	return (0);
}


int main(int argc, char** argv)    // arg : EMVServer EmvRooter
{
	MX			mx;
	char filename[200];
	int	 servertype = 0; // local
	CC* pcc			= NULL;

	if (argc >= 2) {
		printf("The argument supplied for EMV Rooter is %s\n", argv[1]);
		strcpy(EMVRooter, argv[1]); 
	}

	if (argc >= 3) {
		printf("The argument supplied for EMV Server is %s\n", argv[2]);
		strcpy(EMVServer, argv[2]);
	}
	if (argc >= 4) {
		printf("The argument supplied for Login Server is %s\n", argv[3]);
		strcpy(LoginServer, argv[3]);
	}


	if (_getcwd(Directory, sizeof(Directory)) != NULL) {
		printf("Current working dir: %s\n", Directory);
	}
	else {
		perror("getcwd() error");
		return 1;
	}

	char* lastslash = strrchr(Directory, '\\');

	if (strcmp(lastslash, "\\Debug") == 0 || strcmp(lastslash, "\\Release") == 0)
	{
		if (lastslash)           // if found
			*(lastslash) = 0;
	}
	


	strcpy(currency, EurCode);
	sprintf(filename, "%s\\MX\\%s", Directory, "apdu.mx");

	MXInit(&mx, MXCLIENT, NULL, NULL, -1, filename);
	
	pcc = CCInit(&mx);
	
	if (pcc == NULL) {
		MXEnd(&mx);
		return 0;

	}
	
	MXAddGeneralConnectCallBack(&mx, MXONCONNECT,	OnConnect, pcc);
	MXAddGeneralConnectCallBack(&mx, MXONCLOSE,		OnClose,   pcc);
	MXAddApplicationProcedure(&mx, CardApplicationProcedure,   pcc);
	
	if (Connect_RouterServer(pcc) == NULL)
	{
		CCEnd(pcc);
		MXEnd(&mx);
		return 0;
	}
	EMVRooterCom = pcc->pRouterCom;
	int		lReturnLogin = 0;
	while ((lReturnLogin = Send_Login(pcc)) != 1)
	{
	}

	while (1)
	{
		MXDispatchEvents(&mx, 0);
	}

	CCEnd(pcc);
	MXEnd(&mx);
	return 0;
}


/*		
		while (TRUE)
		{
			for (i = 0; i < CurrentCard->ReaderCount; i++)
			{
				CurrentCard->ListStates[i].dwCurrentState = SCARD_STATE_UNAWARE;
			}

			fseek(stdin,0,SEEK_END);
			printf("Select Transaction \n");
			printf("Debit : 0\n");
			printf ("Credit : 1\n>");
			transactiontype = getchar ();
			if (transactiontype != '1' && transactiontype != '0')
				continue;
			else
			{
				if (transactiontype == '0')
					transactiontype = EMV_TRANSACTION_TYPE_PURCHASE;
				else
				if (transactiontype == '1')
					transactiontype = EMV_TRANSACTION_TYPE_REFUND;
				break;
			}

		}
		fseek(stdin,0,SEEK_END);
		while (TRUE)
		{
			int i = 0;
			printf("Select Amount (EUR) \n>");
			gets_s(amount);
			for (i = 0; i < (int)strlen (amount); i++)
				if (!isdigit (amount[i]) && amount[i] != '.')
					continue;
			break;
		}
*/