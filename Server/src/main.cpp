#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <fstream>
#include <sstream>
#include "EMV.h"
#include "cb2a.h"


char   Directory[300];
int    EMVServerPort		= 2000;

char   EMVRooter[300]		= "217.112.89.92";
int	   EMVRooterPort		= 3008;

char   LoginServer[300]	    = "www.jurextrade.com";

MXCom* EMVRooterCom			= NULL;

char   smessage[300];

using namespace std;

void Send_Start(MXCom* pcom, EMVClient* pclient) {
	MXMessage* pmessage;
	BUFFERPARM	Buffer;
	char		message[100];

	sprintf(message, "%s%s*", "*START^", pclient->UserID);
	
	Buffer.BufferType	 = 'T';
	Buffer.BufferSize	 = strlen(message);
	Buffer.BufferContent = (char*)&message; 

	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

}

void Send_End(MXCom* pcom, EMVClient* pclient) {
	MXMessage* pmessage;
	BUFFERPARM	Buffer;
	char		message[100];

	sprintf(message, "%s%s*", "*END^", pclient->UserID);

	Buffer.BufferType	 = 'T';
	Buffer.BufferSize	 = strlen(message);
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

	Buffer.BufferType	 = 'B';
	Buffer.BufferSize	 = totalsize;
	Buffer.BufferContent = tag_buffer;

	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	free(temp_buffer);
	free(tag_buffer);
}


void Send_TLV (MXCom* pcom, EMVClient* pclient, BYTE* tlvbuffer, int tlvsize) 
{
	int userid_size = strlen(pclient->UserID) + 1; // add '^'
	int header_size = strlen("*TLV^") + userid_size;
	char header[20];
	int totalsize;
	
	char* adpu_buffer;

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

	Buffer.BufferType		= 'B';
	Buffer.BufferSize		= totalsize;
	Buffer.BufferContent	= adpu_buffer;

	pmessage = MXPutMessage(pcom, "TCP", "Stream");
	MXSetValue(pmessage, "Buffer", 1, &Buffer);

	free(adpu_buffer);
}


void Send_APDU (MXCom* pcom, EMVClient* pclient, BYTE cla, BYTE ins, BYTE p1, BYTE p2, int datasize, char* data, int way)
{
	if (!pcom) {
		return;
	}
	if (strcmp(pclient->UserID, "-1") == 0) {
		printf("Client : Not Identified by EMV Router \n");
		return;
	}
	
	int totalsize;
	char* adpu_buffer;
	char header[20];

	if (way) {		// R-ADPU

		int userid_size = strlen(pclient->UserID) + 1; // add '^'
		int header_size = strlen("*R-ADPU^") + userid_size;

		totalsize = header_size + (2 * datasize) + 1;

		sprintf(header, "%s%s^", "*R-APDU^", pclient->UserID);

		adpu_buffer = (char*)malloc(totalsize);
	
		for (int i = 0, j = 0; i < datasize; ++i, j += 2)
			sprintf(adpu_buffer + header_size + j, "%02X", data[i] & 0xff);

		memcpy(adpu_buffer, header, header_size);
		adpu_buffer[header_size + (2 * datasize)] = '*';

	} 
	else {
		int userid_size = strlen(pclient->UserID) + 1; // add '^'
		int header_size = strlen("*R-ADPU^") + userid_size;
		int apduheader_size = 10;

		totalsize = header_size + apduheader_size + (2 * datasize) + 1;

		sprintf(header, "%s%s^", "*C-APDU^", pclient->UserID);


		adpu_buffer = (char*)malloc(totalsize);

		memcpy(adpu_buffer, header, header_size);

		int j = 0;
		sprintf(adpu_buffer + header_size + j, "%02X", cla & 0xFF);		 j += 2;
		sprintf(adpu_buffer + header_size + j, "%02X", ins & 0xFF);		 j += 2;
		sprintf(adpu_buffer + header_size + j, "%02X", p1  & 0xFF);		 j += 2;
		sprintf(adpu_buffer + header_size + j, "%02X", p2  & 0xFF);		 j += 2;
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

	Buffer.BufferType    = 'T';
	Buffer.BufferSize    = strlen(message);
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

	Send_Trace(EMVRooterCom, pclient, message);
}

MXCom* Connect_RouterServer(EMV* pemv) {

	MXCom* pCom = MXOpenTCP(pemv->pMX, EMVRooter, EMVRooterPort, IPPROTO_TCP, NULL, NULL, TRUE);
	if (!pCom) {
		printf("Can not connect to EMV Router Server\n");
		return NULL;
	}
	else
	{
		pemv->pRouterCom = pCom;
		printf("Connected to EMV Router Server Ok\n");
	}
	return pCom;
}

int OnConnect(MXCom* pcom, void* applicationfield)
{
	EMV* pemv = (EMV*)applicationfield;
	EMVClient* pclient;

	int Port = MXGetPort(pcom);
	if (Port == -1) return 0;

	if (Port != EMVServerPort)
	{
		printf("Open Connection With Acquirer on port : %d\n\n", Port);

		return 1;
	}
	pclient = EMVInitClient(pemv);

	//DOUDOU	
	/*
		strcpy (pPointOfSale->SIRET,							"00000001999301");
		strcpy (pPointOfSale->PointAcceptationIdentification,	"98765432");
		strcpy (pPointOfSale->PointAcceptationLogicalNumber,	"001");
		strcpy (pPointOfSale->SystemAcceptationIdentification,	"TST5A742");
		strcpy (pPointOfSale->SystemAcceptationLogicalNumber,	"001");
		strcpy (pPointOfSale->SystemAcceptationLogicalNumber,	"001");
		strcpy(pPointOfSale->MerchantContractNumber,			"1999301");

		memcpy(pPointOfSale->MerchantCategoryCode, "\x89\x99", 8);
		strcpy(pPointOfSale->MerchantIdentifier, "1999301        ");
		strcpy(pPointOfSale->MerchantNameAndLocation, "Gabriel Jureidini 20 rue des belles feuilles paris 750116");

	*/


	pclient->pPointOfSale->pCom = pcom;

	printf("\nOpen Connection With Point Of Sale \n");
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "UserInfo",			MXONRECV, OnRecvUserInfo,		pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "C-APDU",				MXONSEND, OnSendAPDU,			pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "R-APDU",				MXONRECV, OnRecvAPDU,			pclient);

	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendACFirst",		MXONSEND, OnSendACFirst,		pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "RecvACFirst",		MXONRECV, OnRecvACFirst,		pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendACSecond",		MXONSEND, OnSendACSecond,		pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "RecvACSecond",		MXONRECV, OnRecvACSecond,		pclient);

	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendATR",			MXONRECV, OnRecvATR,			pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendError",			MXONRECV, OnRecvError,			pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendTransaction",	MXONRECV, OnRecvTransaction,	pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendACFirst",		MXONRECV, OnRecvACFirst,		pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "RecvAppliSelection", MXONRECV, OnRecvAppliSelection, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "RecvCommand",		MXONRECV, OnRecvCommand,		pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "RecvVerify",			MXONRECV, OnRecvVerify,			pclient);

	return 1;
}

int OnClose(MXCom* pcom, void* applicationfield)
{
	EMV* pemv = (EMV*)applicationfield;
	EMVClient* pclient = EMVGetClientFromCom(pemv, pcom);

	int Port = MXGetPort(pcom);

	if (Port == -1) return 0;

	if (Port != EMVServerPort)
	{
		printf("Close Connection With Acquirer \n");
		return 1;
	}
	printf("Close Connection With Point Of Sale  \n");
	EMVEndClient(pemv, pclient);

	return 1;

}


int EMVReadFile (EMV* pemv)
{
	char filename[200]; 
	sprintf (filename, "%s\\FILES\\%s", Directory, "FILES.csv"); 

	ifstream inFile;
    inFile.open(filename);
	if(inFile.fail()) 
	{ 
		char error[250];
		sprintf (error, "%s %s", "Can't open File " , filename);
		return -1; 
	}

    string line;
    int linenum = 0;
	int k = 0;
	int p = 0;



    while (getline (inFile, line))
    { 
        linenum++;
		EMVFile* pemvfile = (EMVFile*)malloc (sizeof(EMVFile)); 
		memset (pemvfile, 0, sizeof (EMVFile));
	    istringstream linestream(line);
		string item;
		int itemnum = 0;
		if (linenum == 1) continue;
		while (getline (linestream, item, ';'))
		{
			if (itemnum == 0)
			{
 				pemvfile->Index = atoi ((char*)item.c_str());
			}
			else
			if (itemnum == 1)
			    strcpy(pemvfile->Name , item.c_str());
			else
			if (itemnum == 2)
			{
			   strcpy(pemvfile->Format, item.c_str());
				
			}
			else
			if (itemnum == 3)
			{
			   strcpy(pemvfile->Description, item.c_str());
				
			}
			itemnum++;
	    }
		EMVAddFile (pemv, pemvfile);
		k++;
	}
    
	inFile.close ();

//Generate dialog 
	sprintf (filename, "%s\\MX\\%s", Directory, "dialogfiles.mx"); 
	EMVGenerateMXFile (pemv, "LOAD", 8, filename);
	MXLoadResourceFile (pemv->pMX, filename);


    return (0);
}



int EMVReadApduErrorFile(EMV* pemv)
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
		EMVAddApduError(pemv, pemverror);
		k++;
	}

	inFile.close();
	return (0);
}

int EMVReadTagFile (EMV* pemv)
{
	char filename[200]; 
	sprintf (filename, "%s\\FILES\\%s", Directory, "TAGS.csv"); 

	ifstream inFile;
    inFile.open(filename);
	if(inFile.fail()) 
	{ 
		char error[250];
		sprintf (error, "%s %s", "Can't open File " , filename);
		return -1; 
	}

    string line;
    int linenum = 0;
	int k = 0;
	int p = 0;



    while (getline (inFile, line))
    { 
        linenum++;
		EMVTag* pemvtag = (EMVTag*)malloc (sizeof(EMVTag)); 
	    istringstream linestream(line);
		string item;
		int itemnum = 0;
		if (linenum == 1) continue;
		while (getline (linestream, item, ';'))
		{
			if (itemnum == 1)
			{
 				pemvtag->Tag = HexStrToDecimal ((char*)item.c_str());
				strcpy(pemvtag->strTag , item.c_str());
			}
			else
			if (itemnum == 2)
			    strcpy(pemvtag->Name , item.c_str());
			else
			if (itemnum == 3)
			{
			   strcpy(pemvtag->Format , item.c_str());
			   EMVGetFormatProperties (pemvtag->Format, &pemvtag->DataType, &pemvtag->IsVariable, &pemvtag->Size);
				
			}
			else
			if (itemnum == 6)
			    strcpy(pemvtag->Origin , item.c_str());
			itemnum++;
	    }
		EMVAddTag (pemv, pemvtag);
		k++;
	}
    
	inFile.close ();
    return (0);
}

int CB2AReadTagFile (CB2A* pcb2a)
{
	char filename[200]; 
	sprintf (filename, "%s\\FILES\\%s", Directory, "CB2A.csv"); 

	ifstream inFile;
    inFile.open(filename);
	if(inFile.fail()) 
	{ 
		char error[250];
		sprintf (error, "%s %s", "Can't open File " , filename);
		return -1; 
	} 

    string line;
    int linenum = 0;
	int k = 0;
	int p = 0;
	CB2AField* pcb2afield = NULL;
	CB2ATag* pcb2atag = NULL;
	int readfield = 0;


    while (getline (inFile, line))
    { 
        linenum++;
		
	    istringstream linestream(line);
		string item;
		int itemnum = 0;
		if (linenum == 1) continue;
		while (getline (linestream, item, ';'))
		{
			if (itemnum == 0)
			{
				if (pcb2afield == NULL || strcmp ((char*)item.c_str(), pcb2afield->strField) != 0)
				{
					pcb2afield = (CB2AField*)malloc (sizeof(CB2AField)); 
					memset (pcb2afield, 0, sizeof (CB2AField));
					pcb2afield->To = -1;
					pcb2afield->From = -1;
                    
					pcb2afield->EMVTag = 0;
					pcb2afield->Value  = NULL;
					pcb2afield->Tags   = NULL;
					strcpy (pcb2afield->strField, (char*)item.c_str());
					pcb2afield->Field = atoi (pcb2afield->strField);
					CB2AAddField (pcb2a, pcb2afield);
					readfield = 1;
				}
			}
			else
			if (itemnum == 1)
			{
				if (strcmp ((char*)item.c_str(), "") != 0)
				{
					pcb2atag = (CB2ATag*)malloc (sizeof(CB2ATag)); 
					memset (pcb2atag, 0, sizeof (CB2ATag));
					pcb2atag->To = -1;
					pcb2atag->From = -1;

					pcb2atag->EMVTag = 0;
					pcb2atag->Value = NULL;
					pcb2atag->Tag = HexStrToDecimal((char*)item.c_str());

					strcpy(pcb2atag->strTag, item.c_str());
					pcb2atag->pField = pcb2afield;

					CB2AAddTag(pcb2a, pcb2afield, pcb2atag);
					readfield = 0;
				}
			}
			else
			if (itemnum == 2)
			{
				if (readfield)
					strcpy(pcb2afield->Name, item.c_str());
				else
					strcpy(pcb2atag->Name, item.c_str());
			}
			else
			if (itemnum == 3)
			{
				if (readfield)
				{
					strcpy(pcb2afield->Format, item.c_str());
					EMVGetFormatProperties(pcb2afield->Format, &pcb2afield->DataType, &pcb2afield->IsVariable, &pcb2afield->Size);
				}
				else
				{
					strcpy(pcb2atag->Format, item.c_str());
					EMVGetFormatProperties(pcb2atag->Format, &pcb2atag->DataType, &pcb2atag->IsVariable, &pcb2atag->Size);
				}
			}
			else
			if (itemnum == 5)
			{
				if (readfield)
					strcpy(pcb2afield->Requirements, item.c_str());
				else
					strcpy(pcb2atag->Requirements, item.c_str());
			}
			else
			if (itemnum == 19)      //From  
			{
				if (strcmp((char*)item.c_str(), "") != 0)
					if (readfield)
						pcb2afield->From = atoi(item.c_str());
					else
						pcb2atag->From = atoi(item.c_str());
			}
			else
			if (itemnum == 20)      //to
			{
				if (strcmp((char*)item.c_str(), "") != 0)
					if (readfield)
						pcb2afield->To = atoi(item.c_str());
					else
						pcb2atag->To = atoi(item.c_str());

			}
			else
			if (itemnum == 22)
			{
				if (readfield)
					pcb2afield->EMVTag = HexStrToDecimal((char*)item.c_str());
				else
					pcb2atag->EMVTag = HexStrToDecimal((char*)item.c_str());
			}
			else
			if (itemnum == 8)
			{
			}
			itemnum++;
		}

		k++;
	}
	inFile.close();

	return (0);
}

int main(int argc, char** argv)
{
	EMV*	pemv;
	MX		mx;
	char	filename[200];

	if (argc >= 2) {
		printf("The argument supplied for EMV Rooter is %s\n", argv[1]);
		strcpy(EMVRooter, argv[1]);
	}
	if (argc >= 3) {
		printf("The argument supplied for Login Server is %s\n", argv[2]);
		strcpy(LoginServer, argv[2]);
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
	
	sprintf (filename, "%s\\MX\\%s", Directory, "apdu.mx"); 
	
	MXInit (&mx, MXSERVER, NULL, NULL, EMVServerPort,  filename);	

	pemv	= EMVInit (&mx);
	

	MXAddGeneralConnectCallBack (&mx, MXONCONNECT, OnConnect, pemv);
	MXAddGeneralConnectCallBack (&mx, MXONCLOSE,   OnClose,   pemv);

	if (Connect_RouterServer(pemv) == NULL)
	{
		EMVEnd(pemv);
		MXEnd(&mx);
		return 0;
	}
	EMVRooterCom = pemv->pRouterCom;

	MXDispatchEvents (&mx, NULL);
	

	EMVEnd(pemv);
    MXEnd (&mx);
	
	return 0;
}
/*	int l;
	char hour[6];
	char date[6];
	BYTE Decimal[50];
	BYTE Ascii[50];
CB2AMessage* pmessage = CB2AInitMessage (pcb2a, "0100");

	srand((unsigned int) time(NULL));
	HexaCharToChar ((BYTE *)"0000000000000000", Decimal, 16);
	NumericDecimalCharToHexaChar (( char *)Decimal, "ab0c" , 50);
	CharToHexaChar ((BYTE *)"111", Ascii, 3);

	l = CharArrayToBCDArray ("C12345", 6, 12, "x+n", Ascii);
	l = CharArrayToCharArray ("12345", 5, 12, "n", Ascii);
	l = CharArrayToCharArray ("AGENCE2", 7, 12, "an", Ascii);
	l = CharArrayToCharArray ("C12345 ", 6, 12, "x+n", Ascii);

	l = CharArrayToHexaArray ("3CDE1245EF7684172048CBFF ", 24, 12, "anb", Ascii);
	DateGetGMTDateHour (hour, date);
	DateGetLocalDateHour (hour, date);
*/

