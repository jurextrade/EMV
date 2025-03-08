#include "EMV.h"
#include "Actys.h"

// ----------------------------------------------------------------------- CONNECTION AND COMMUNICATION WITH EMV ROUTER -------------------------------------------------------

MXCom* Connect_RouterServer(EMV* pemv) {

	MXCom* pCom = MXOpenTCP(pemv->pMX, EMVRooter, EMVRooterPort, IPPROTO_TCP, NULL, NULL, TRUE);
	if (!pCom) {
		printf("Can not connect to EMV Router Server\n");
		pemv->pRouterCom = NULL;
		EMVRooterCom = NULL;
		return NULL;
	}
	else
	{
		pemv->pRouterCom = pCom;
		printf("Connected to EMV Router Server Ok\n");
		EMVRooterCom = pCom;
	}
	return pCom;
}

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

void Send_Start(MXCom* pcom, EMVClient* pclient) {
	MXMessage* pmessage;
	BUFFERPARM	Buffer;
	char		message[100];

	if (!pcom) {
		return;
	}

	sprintf(message, "%s%s*", "*START^", pclient->UserID);

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)&message;

	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

}

void Send_End(MXCom* pcom, EMVClient* pclient) {
	MXMessage* pmessage;
	BUFFERPARM	Buffer;
	char		message[100];

	if (!pcom) {
		return;
	}

	sprintf(message, "%s%s*", "*END^", pclient->UserID);

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)&message;

	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);
}


void Send_Tag(MXCom* pcom, EMVClient* pclient, unsigned short tag, BYTE* data, int size)
{
	int userid_size = strlen(pclient->UserID) + 1; // add '^'
	int header_size = strlen("*TAG^") + userid_size;
	char header[20];
	int totalsize;
	char* tag_buffer;
	BYTE* temp_buffer;

	if (!pcom) {
		return;
	}

	totalsize = (3 * size) + 100;
	temp_buffer = (BYTE*)malloc(totalsize);

	int tag_size = TLVMake(data, size, tag, temp_buffer);
	totalsize = header_size + (2 * tag_size) + 1;

	sprintf(header, "%s%s^", "*TAG^", pclient->UserID);

	tag_buffer = (char*)malloc(totalsize);

	memset(tag_buffer, 0, totalsize);
	memcpy(tag_buffer, header, header_size);

	for (int i = 0, j = 0; i < tag_size; ++i, j += 2)
	{
		sprintf(tag_buffer + header_size + j, "%02X", temp_buffer[i] & 0xff);
	}

	tag_buffer[totalsize - 1] = '*';

	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'B';
	Buffer.BufferSize = totalsize;
	Buffer.BufferContent = tag_buffer;

	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	free(temp_buffer);
	free(tag_buffer);
}


void Send_TLV(MXCom* pcom, EMVClient* pclient, BYTE* tlvbuffer, int tlvsize)
{
	int userid_size = strlen(pclient->UserID) + 1; // add '^'
	int header_size = strlen("*TLV^") + userid_size;
	char header[20];
	int totalsize;

	char* adpu_buffer;

	if (!pcom) {
		return;
	}

	totalsize = header_size + (2 * tlvsize) + 1;

	sprintf(header, "%s%s^", "*TLV^", pclient->UserID);

	adpu_buffer = (char*)malloc(totalsize);

	for (int i = 0, j = 0; i < tlvsize; ++i, j += 2)
	{
		sprintf(adpu_buffer + header_size + j, "%02X", tlvbuffer[i] & 0xff);
	}

	memcpy(adpu_buffer, header, header_size);
	adpu_buffer[header_size + (2 * tlvsize)] = '*';

	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'B';
	Buffer.BufferSize = totalsize;
	Buffer.BufferContent = adpu_buffer;

	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	free(adpu_buffer);
}


void Send_APDU(MXCom* pcom, EMVClient* pclient, BYTE cla, BYTE ins, BYTE p1, BYTE p2, int datasize, BYTE* data, int way)
{
	if (!pcom) {
		return;
	}
	if (strcmp(pclient->UserID, "-1") == 0) {
		printf("Client : Not Identified by EMV Router \n");
		return;
	}
	char header[30];
	if (way) {		// R-ADPU
		sprintf(header, "%s%s^", "*R-APDU^", pclient->UserID);
	}
	else {
		sprintf(header, "%s%s^", "*C-APDU^", pclient->UserID);
	}


	int userid_size = strlen(pclient->UserID) + 1; // add '^'
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

	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	free(adpu_buffer);

}

void Send_Trace(MXCom* pcom, EMVClient* pclient, char* message)
{
	if (!pcom) {
		return;
	}
	int size = strlen(message);

	int userid_size = strlen(pclient->UserID) + 1; // add '^'
	int header_size = strlen("*TRACE^") + userid_size;

	char header[20];

	sprintf(header, "%s%s^", "*TRACE^", pclient->UserID);

	memcpy(message + header_size, message, size);
	memcpy(message, header, header_size);

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

void Send_Info(MXCom* pcom, EMVClient* pclient, char* command, char* message)
{
	if (!pcom) {
		return;
	}
	int size = strlen(message);

	int userid_size = strlen(pclient->UserID) + 1; // add '^'
	int header_size = strlen(command) + userid_size + 2;
	char header[20];

	sprintf(header, "*%s^%s^", command, pclient->UserID);

	memcpy(message + header_size, message, size);
	memcpy(message, header, header_size);

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

void Send_Command(MXCom* pcom, EMVClient* pclient, char* command, long long par)
{
	if (!pcom) {
		return;
	}

	int userid_size = strlen(pclient->UserID) + 1; // add '^'
	int header_size = strlen(command) + userid_size + 2;

	char header[20];

	sprintf(header, "*%s^%s^", command, pclient->UserID);

	char		message[100];
	sprintf(message, "%s%I64d*", header, par);

	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)message;


	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

}

void Send_Step(MXCom* pcom, EMVClient* pclient, int step)
{
	if (!pcom) {
		return;
	}

	int userid_size = strlen(pclient->UserID) + 1; // add '^'
	int header_size = strlen("*STEP^") + userid_size;

	char header[20];

	sprintf(header, "%s%s^", "*STEP^", pclient->UserID);

	char		message[100];
	sprintf(message, "%s%d*", header, step);

	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)message;


	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

}

void Send_SetTSI(MXCom* pcom, EMVClient* pclient, unsigned short b)
{
	if (!pcom) {
		return;
	}

	int userid_size = strlen(pclient->UserID) + 1; // add '^'
	int header_size = strlen("*SETTSI^") + userid_size;

	char header[20];

	sprintf(header, "%s%s^", "*SETTSI^", pclient->UserID);

	char		message[100];
	sprintf(message, "%s%d*", header, b);

	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)message;


	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

}

void Send_SetTVR(MXCom* pcom, EMVClient* pclient, unsigned short b)
{
	if (!pcom) {
		return;
	}

	int userid_size = strlen(pclient->UserID) + 1; // add '^'
	int header_size = strlen("*SETTVR^") + userid_size;

	char header[20];

	sprintf(header, "%s%s^", "*SETTVR^", pclient->UserID);

	char		message[100];
	sprintf(message, "%s%d*", header, b);

	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)message;


	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

}

void Send_AUC(MXCom* pcom, EMVClient* pclient, unsigned short b)
{
	if (!pcom) {
		return;
	}

	int userid_size = strlen(pclient->UserID) + 1; // add '^'
	int header_size = strlen("*AUC^") + userid_size;

	char header[20];

	sprintf(header, "%s%s^", "*AUC^", pclient->UserID);

	char		message[100];
	sprintf(message, "%s%d*", header, b);

	MXMessage* pmessage;
	BUFFERPARM	Buffer;

	Buffer.BufferType = 'T';
	Buffer.BufferSize = strlen(message);
	Buffer.BufferContent = (char*)message;


	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

}



void s_printf(char* message, EMVClient* pclient, char* format, char* string)
{
	sprintf(message, format, string);
	printf(message);

	if (EMVRooter) {
		Send_Trace(EMVRooterCom, pclient, message);
	}
}

