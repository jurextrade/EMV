#define _CRT_SECURE_NO_WARNINGS

#include "Card.h"



// ----------------------------------------------------------------------- CONNECTION AND COMMUNICATION WITH EMV ROUTER -------------------------------------------------------

MXCom* Connect_RouterServer(CC* pcc) {
	MX* pmx = pcc->pMX;
	MXCom* pCom = MXOpenTCP(pmx, EMVRooter, EMVRooterPort, IPPROTO_TCP, NULL, NULL, TRUE);
	if (!pCom) {
		printf("Can not connect to EMV Router Server\n");
		pcc->pRouterCom = NULL;
		EMVRooterCom = NULL;
		return NULL;
	}
	else
	{
		pcc->pRouterCom = pCom;
		printf("Connected to EMV Router Server Ok\n");
		EMVRooterCom = pCom;
	}

	MXAddComCallBack(pmx, pCom, "TCP", "Stream", MXONRECV, OnRecvRouter, pcc);
	MXAddComCallBack(pmx, pCom, "TCP", "Stream", MXONSEND, OnSendRouter, pcc);
	return pCom;
}


int OnSendRouter(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Buffer", 1);
	BYTE* outData = (BYTE*)Buffer->BufferContent;
	DWORD size = Buffer->BufferSize;

	//printf("Send torouter : %.*s\n", size, outData);

	free(Buffer->BufferContent);
	free(Buffer);
	return 1;
}


int OnRecvRouter(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	BUFFERPARM* pBuffer = (BUFFERPARM*)MXGetValue(pmessage, "Buffer", 1);
	char* outData = (char*)pBuffer->BufferContent;
	DWORD size = pBuffer->BufferSize;
	CC* pcc = (CC*)applicationfield;

	char UserId[20] = { 0 };
	char Command[20] = { 0 };
	char WebStatut[20] = { 0 };
	char message[300] = { 0 };

	memset(message, 0, 300);
	strncpy(message, outData, size);
	sscanf(message, "%[0-9]*%[a-zA-Z]*%[a-zA-Z ]*", UserId, Command, WebStatut);

	if (strcmp(Command, "WEBCLIENT") == 0) {
		if (strcmp(WebStatut, "OK") == 0) {
			printf("You are connected on the web please follow the transactions on the web\n");
			pcc->pCardContext->WebUser = 1;
		}
	}
	return 1;
}

int Send_Login(CC* pcc)
{
	MXMessage* pmessage;
	BUFFERPARM	Buffer;
	char message[300] = { 0 };
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

	BUFFERPARM* pBuffer = (BUFFERPARM*)MXGetValue(pmessage, "Buffer", 1);
	char* outData = (char*)pBuffer->BufferContent;
	DWORD size = pBuffer->BufferSize;

	printf("Receive from router : %.*s\n", size, outData);

	char UserId[20] = { 0 };
	char Command[20] = { 0 };
	char WebStatut[20] = { 0 };

	memset(message, 0, 300);
	strncpy(message, outData, size);

	sscanf(message, "%[0-9]*%[a-zA-Z]*%[a-zA-Z ]*", UserId, Command, WebStatut);
	if (strcmp(Command, "LOGIN") == 0) {
		if (strcmp(UserId, "0") == 0)
		{
			printf("Indentification Failed\n");
			free(pBuffer->BufferContent);
			free(pBuffer);
			MXFreeMessage(pcc->pRouterCom->MX, pmessage);
			return 0;

		}
		else
		{
			printf("Indentification Succeeded\n");
			strcpy(CardConnector->UserID, UserId);
			free(pBuffer->BufferContent);
			free(pBuffer);
			MXFreeMessage(pcc->pRouterCom->MX, pmessage);

		}
		if (strcmp(WebStatut, "KO") == 0) {
			printf("Login to Jurextrade Site to follow the transaction on the web, this transaction will be treated on your cmd console only\n");
			pcc->pCardContext->WebUser = 0;
		}
		else
		{
			printf("You are connected please follow the transactions on the web\n");
			pcc->pCardContext->WebUser = 1;
		}
		return 1;
	}
	return 0;
}

void Send_Start(MXCom* pcom) {

	BUFFERPARM	Buffer;
	char	message[100];

	if (!pcom) {
		return;
	}

	sprintf(message, "%s%s*", "*START^", CardConnector->UserID);

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)&message;

	MXMessage* pmessage = pmessage = MXCreateMessage(pcom->MX, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);
	MXSend(pcom->MX, pcom, pmessage);
}

void Send_Plug(MXCom* pcom, char* message)
{
	if (!pcom) {
		return;
	}
	int size = strlen(message);

	int userid_size = strlen(CardConnector->UserID) + 1; // add '^'
	int header_size = strlen("*PLUG^") + userid_size;

	char header[30];

	sprintf(header, "%s%s^", "*PLUG^", CardConnector->UserID);

	memcpy(message + header_size, message, size);
	memcpy(message, header, header_size);

	message[size + header_size] = '*';

	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)message;


	//	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	//	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	pmessage = MXCreateMessage(pcom->MX, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);
	MXSend(pcom->MX, pcom, pmessage);
	MXFreeMessage(pcom->MX, pmessage);

}

void Send_Select(MXCom* pcom, char* message)
{
	if (!pcom) {
		return;
	}
	int size = strlen(message);

	int userid_size = strlen(CardConnector->UserID) + 1; // add '^'
	int header_size = strlen("*SELECT^") + userid_size;

	char header[30];

	sprintf(header, "%s%s^", "*SELECT^", CardConnector->UserID);

	memcpy(message + header_size, message, size);
	memcpy(message, header, header_size);

	message[size + header_size] = '*';

	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)message;


	//	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	//	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	pmessage = MXCreateMessage(pcom->MX, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);
	MXSend(pcom->MX, pcom, pmessage);
	MXFreeMessage(pcom->MX, pmessage);

}

void Send_Trace(MXCom* pcom, char* message)
{
	if (!pcom) {
		return;
	}

	int size = strlen(message);

	int userid_size = strlen(CardConnector->UserID) + 1; // add '^'
	int header_size = strlen("*TRACE^") + userid_size;

	char header[30];

	sprintf(header, "%s%s^", "*TRACE^", CardConnector->UserID);

	memcpy(message + header_size, message, size);
	memcpy(message, header, header_size);

	message[size + header_size] = '*';
	message[size + header_size + 1] = 0;


	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)message;


	//	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	//	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	pmessage = MXCreateMessage(pcom->MX, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);
	MXSend(pcom->MX, pcom, pmessage);
	MXFreeMessage(pcom->MX, pmessage);

}

void Send_APDU(MXCom* pcom, BYTE cla, BYTE ins, BYTE p1, BYTE p2, int datasize, unsigned char* data, int way)
{
	if (!pcom) {
		return;
	}

	char header[30];
	if (way) {		// R-ADPU
		sprintf(header, "%s%s^", "*R-APDU^", CardConnector->UserID);
	}
	else {
		sprintf(header, "%s%s^", "*C-APDU^", CardConnector->UserID);
	}

	int userid_size = strlen(CardConnector->UserID) + 1; // add '^'
	int header_size = strlen("*C-ADPU^") + userid_size;
	int apduheader_size = 10;

	int totalsize = header_size + apduheader_size + (2 * datasize) + 1;

	char* adpu_buffer = (char*)malloc(totalsize);
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

	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'B';
	Buffer.BufferSize = totalsize;
	Buffer.BufferContent = adpu_buffer;

	pmessage = MXCreateMessage(pcom->MX, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);
	MXSend(pcom->MX, pcom, pmessage);
	MXFreeMessage(pcom->MX, pmessage);
	free(adpu_buffer);
}

void s_printf(char* message, char* format, char* string)
{

	sprintf(message, format, string);
	printf(message);
	Send_Trace(EMVRooterCom, message);
}