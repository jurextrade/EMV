#define _CRT_SECURE_NO_WARNINGS

#include "Card.h"

int MXAddAPDUCommands(MX* pmx)
{
	MXDialogClass* pclass;

	pclass = MXCreateDialogClass(pmx, "APDU", 7);

	MXCreateMessageClass(pmx, pclass, "UserInfo", 1, 2, 
		"STRING", 1, "UserName",
		"STRING", 1, "UserPassword");

	MXCreateMessageClass(pmx, pclass, "SendTransaction", 2, 4, 
		"CHAR", 1, "Type",
		"STRING", 1, "Currency",
		"STRING", 1, "Amount",
		"BYTE", 1, "Media");

	/* active connection */

	MXCreateMessageClass(pmx, pclass, "SendATR", 3, 1, 
		"STRING", 1, "Atr");

	MXCreateMessageClass(pmx, pclass, "SendError", 4, 1, 
		"STRING", 1, "Error");

	MXCreateMessageClass(pmx, pclass, "C-APDU", 5, 6, 
		"BYTE", 1, "Cla",
		"BYTE", 1, "Ins",
		"BYTE", 1, "P1",
		"CHAR", 1, "P2",
		"STRING", 1, "Size",
		"BUFFER", 1, "Data");

	MXCreateMessageClass(pmx, pclass, "R-APDU", 6, 4, 
		"BYTE", 1, "Cla",
		"BYTE", 1, "Ins",
		"WORD", 1, "Size",
		"BUFFER", 1, "Data");

	MXCreateMessageClass(pmx, pclass, "SendACFirst", 7, 6,
		"BYTE", 1, "Cla",
		"BYTE", 1, "Ins",
		"BYTE", 1, "P1",
		"CHAR", 1, "P2",
		"STRING", 1, "Size",
		"BUFFER", 1, "Data");

	MXCreateMessageClass(pmx, pclass, "RecvACFirst", 8, 4,
		"BYTE", 1, "Cla",
		"BYTE", 1, "Ins",
		"LONG", 1, "Size",
		"BUFFER", 1, "Data");

	MXCreateMessageClass(pmx, pclass, "SendACSecond", 9, 6,
		"BYTE", 1, "Cla",
		"BYTE", 1, "Ins",
		"BYTE", 1, "P1",
		"CHAR", 1, "P2",
		"STRING", 1, "Size",
		"BUFFER", 1, "Data");

	MXCreateMessageClass(pmx, pclass, "RecvACSecond", 10, 4,
		"BYTE", 1, "Cla",
		"BYTE", 1, "Ins",
		"LONG", 1, "Size",
		"BUFFER", 1, "Data");

	MXCreateMessageClass(pmx, pclass, "SendAppliSelection", 11, 4,
		"BYTE", 1, "Count",
		"BYTE", 5, "Priority",
		"BYTE", 5, "Index",
		"STRING", 5, "Label");

	MXCreateMessageClass(pmx, pclass, "RecvAppliSelection", 12, 1,
		"CHAR", 1, "Index");

	MXCreateMessageClass(pmx, pclass, "SendCommand", 13, 2,
		"BYTE", 1, "P1",
		"BYTE", 1, "P2");

	MXCreateMessageClass(pmx, pclass, "RecvCommand", 14, 4,
		"BYTE", 1, "P1",
		"BYTE", 1, "P2",
		"LONG", 1, "Size",
		"BUFFER", 1, "Data");

	MXCreateMessageClass(pmx, pclass, "SendVerify", 15, 1,
		"BYTE", 1, "Enciphered");

	MXCreateMessageClass(pmx, pclass, "RecvVerify", 16, 1,
		"BUFFER", 1, "Data");

	MXCreateMessageClass(pmx, pclass, "Abort", 17, 1,
		"STRING", 1, "Reason");

	return 1;
}


// ----------------------------------------------------------------------- CONNECTION AND COMMUNICATION WITH EMV SERVER -------------------------------------------------------

MXCom* Connect_EMVServer(CC* pcc, CARD* pCard) {

	MX* pmx = pcc->pMX;

	MXCom* pCom = MXOpenTCP(pcc->pMX, EMVServer, EMVServerPort, IPPROTO_DG, NULL, NULL, TRUE);

	if (!pCom)
	{
		s_printf(smessage, "%s", "Can not connect to EMV Server \n");
		return NULL;
	}
	s_printf(smessage, "%s", "Open Connection With EMV Server \n");

	MXAddComCallBack(pmx, pCom, "APDU", "C-APDU",		MXONRECV, OnRecvAPDU, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "R-APDU",		MXONSEND, OnSendAPDU, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendACFirst",	MXONRECV, OnRecvACFirst, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "RecvACFirst",	MXONSEND, OnSendACFirst, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendACSecond", MXONRECV, OnRecvACSecond, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "RecvACSecond", MXONSEND, OnSendACSecond, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "RecvVerify",	MXONSEND, OnSendVerify, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendVerify",	MXONRECV, OnRecvVerify, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendATR",			MXONSEND, OnSendATR, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendTransaction",  MXONSEND, OnSendTransaction, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendAppliSelection", MXONRECV, OnRecvSendAppliSelection, pCard);
	MXAddComCallBack(pmx, pCom, "APDU", "SendCommand",        MXONRECV, OnRecvSendCommand, pCard);
	pCard->pCom = pCom;
	return pCom;
}

int OnSendAPDU(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending APDU Answer to Server \n");
	return 1;

}

int OnSendACFirst(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending Generate AC First Answer to Server \n");
	return 1;

}

int OnSendACSecond(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending Generate AC Second Answer to Server \n");
	return 1;

}

int OnSendVerify(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending Verify Answer to Server \n");
	return 1;

}


int OnSendTransaction(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending Transaction  to Server \n");
	return 1;

}

int OnSendATR(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	s_printf(smessage, "%s", "Sending ATR  to Server \n");
	return 1;

}

int OnRecvSendCommand(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{

	int outDataSize;
	unsigned char outData[258];
	char strError[250];
	BUFFERPARM OutBuffer;
	MXMessage* Outpmessage;
	CARD* pCard = (CARD*)applicationfield;

	BYTE	p1 = *(BYTE*)MXGetValue(pmessage, "P1", 1);
	BYTE	p2 = *(BYTE*)MXGetValue(pmessage, "P2", 1);

	s_printf(smessage, "%s", "Receiving APDU Command from Server \n");

	Send_APDU(EMVRooterCom, 0x80, 0xCA, p1, p2, 0, outData, 0);

	if (!CardAPDU(pCard, 0x80, 0xCA, p1, p2, 0, (unsigned char*)"", &outDataSize, outData))
	{
		sprintf(strError, "%s", "APDU failed, transmission error\n");

		Outpmessage = MXPutMessage(pcom, "APDU", "SendError");
		MXSetValue(Outpmessage, "Error", 1, strError);

		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	// Response data must at least have SW1 SW2
	if (outDataSize < 2)
	{
		sprintf(strError, "%s", "APDU failed, wrong size\n");

		Outpmessage = MXPutMessage(pcom, "APDU", "SendError");
		MXSetValue(Outpmessage, "Error", 1, strError);

		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	s_printf(smessage, "%s", "Response APDU From Card \n");

	OutBuffer.BufferType = 'B';
	OutBuffer.BufferSize = outDataSize;
	OutBuffer.BufferContent = (char*)&outData;

	Outpmessage = MXPutMessage(pcom, "APDU", "RecvCommand");
	MXSetValue(Outpmessage, "Size", 1, &outDataSize);
	MXSetValue(Outpmessage, "Data", 1, &OutBuffer);
	MXSetValue(Outpmessage, "P1", 1, &p1);
	MXSetValue(Outpmessage, "P2", 1, &p2);

	Send_APDU(EMVRooterCom, 0x80, 0xCA, p1, p2, outDataSize, outData, 1);

	return 1;
}

int OnRecvAPDU(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	int outDataSize;
	unsigned char outData[2000];
	char strError[250];

	MXMessage* Outpmessage;
	BUFFERPARM OutBuffer;
	CARD* pCard = (CARD*)applicationfield;


	BYTE	Cla = *(BYTE*)MXGetValue(pmessage, "Cla", 1);
	BYTE	Ins = *(BYTE*)MXGetValue(pmessage, "Ins", 1);
	BYTE	P1 = *(BYTE*)MXGetValue(pmessage, "P1", 1);
	BYTE	P2 = *(BYTE*)MXGetValue(pmessage, "P2", 1);
	BYTE	Size = *(BYTE*)MXGetValue(pmessage, "Size", 1);
	BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);

	s_printf(smessage, "%s", "Receiving APDU Command from Terminal \n");
	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, 0);

	if (!CardAPDU(pCard, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, &outDataSize, (unsigned char*)&outData))
	{
		sprintf(strError, "%s", "APDU failed, transmission error\n");

		Outpmessage = MXPutMessage(pcom, "APDU", "SendError");
		MXSetValue(Outpmessage, "Error", 1, strError);

		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	// Response data must at least have SW1 SW2
	if (outDataSize < 2)
	{
		sprintf(strError, "%s", "APDU failed, wrong size\n");

		Outpmessage = MXPutMessage(pcom, "APDU", "SendError");
		MXSetValue(Outpmessage, "Error", 1, strError);

		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		return 1;
	}

	s_printf(smessage, "%s", "Response APDU From Card \n");

	OutBuffer.BufferType = 'B';
	OutBuffer.BufferSize = outDataSize;
	OutBuffer.BufferContent = (char*)&outData;

	Outpmessage = MXPutMessage(pcom, "APDU", "R-APDU");
	MXSetValue(Outpmessage, "Cla", 1, &Cla);
	MXSetValue(Outpmessage, "Ins", 1, &Ins);
	MXSetValue(Outpmessage, "Size", 1, &outDataSize);
	MXSetValue(Outpmessage, "Data", 1, &OutBuffer);

	free(Buffer->BufferContent);
	free(Buffer);

	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, outDataSize, outData, 1);

	return 1;
}


int OnRecvACFirst(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{

	int outDataSize;
	unsigned char outData[258];
	char strError[250];

	MXMessage* Outpmessage;
	BUFFERPARM OutBuffer;
	CARD* pCard = (CARD*)applicationfield;


	BYTE	Cla = *(BYTE*)MXGetValue(pmessage, "Cla", 1);
	BYTE	Ins = *(BYTE*)MXGetValue(pmessage, "Ins", 1);
	BYTE	P1 = *(BYTE*)MXGetValue(pmessage, "P1", 1);
	BYTE	P2 = *(BYTE*)MXGetValue(pmessage, "P2", 1);
	BYTE	Size = *(BYTE*)MXGetValue(pmessage, "Size", 1);
	BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);

	s_printf(smessage, "%s", "Receiving GENERATE AC FIRST Command from Server \n");

	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, 0);

	if (!CardAPDU(pCard, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, &outDataSize, outData))
	{
		sprintf(strError, "%s", "APDU failed, transmission error\n");

		Outpmessage = MXPutMessage(pcom, "APDU", "SendError");
		MXSetValue(Outpmessage, "Error", 1, strError);

		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	// Response data must at least have SW1 SW2
	if (outDataSize < 2)
	{
		sprintf(strError, "%s", "APDU failed, wrong size\n");

		Outpmessage = MXPutMessage(pcom, "APDU", "SendError");
		MXSetValue(Outpmessage, "Error", 1, strError);

		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	s_printf(smessage, "%s", "Response APDU From Card \n");


	OutBuffer.BufferType = 'B';
	OutBuffer.BufferSize = outDataSize;
	OutBuffer.BufferContent = (char*)&outData;

	Outpmessage = MXPutMessage(pcom, "APDU", "RecvACFirst");
	MXSetValue(Outpmessage, "Cla", 1, &Cla);
	MXSetValue(Outpmessage, "Ins", 1, &Ins);
	MXSetValue(Outpmessage, "Size", 1, &outDataSize);
	MXSetValue(Outpmessage, "Data", 1, &OutBuffer);

	free(Buffer->BufferContent);
	free(Buffer);

	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, outDataSize, outData, 1);

	return 1;
}

int OnRecvACSecond(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{

	int outDataSize;
	unsigned char outData[258];
	char strError[250];

	MXMessage* Outpmessage;
	BUFFERPARM OutBuffer;
	CARD* pCard = (CARD*)applicationfield;


	BYTE	Cla = *(BYTE*)MXGetValue(pmessage, "Cla", 1);
	BYTE	Ins = *(BYTE*)MXGetValue(pmessage, "Ins", 1);
	BYTE	P1 = *(BYTE*)MXGetValue(pmessage, "P1", 1);
	BYTE	P2 = *(BYTE*)MXGetValue(pmessage, "P2", 1);
	BYTE	Size = *(BYTE*)MXGetValue(pmessage, "Size", 1);
	BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);

	printf("Receiving GENERATE AC SECOND Command from Server \n");

	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, 0);

	if (!CardAPDU(pCard, Cla, Ins, P1, P2, Size, (unsigned char*)Buffer->BufferContent, &outDataSize, outData))
	{
		sprintf(strError, "%s", "APDU failed, transmission error\n");

		Outpmessage = MXPutMessage(pcom, "APDU", "SendError");
		MXSetValue(Outpmessage, "Error", 1, strError);

		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	// Response data must at least have SW1 SW2
	if (outDataSize < 2)
	{
		sprintf(strError, "%s", "APDU failed, wrong size\n");

		Outpmessage = MXPutMessage(pcom, "APDU", "SendError");
		MXSetValue(Outpmessage, "Error", 1, strError);

		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		return 1;
	}


	OutBuffer.BufferType = 'B';
	OutBuffer.BufferSize = outDataSize;
	OutBuffer.BufferContent = (char*)&outData;

	Outpmessage = MXPutMessage(pcom, "APDU", "RecvACSecond");
	MXSetValue(Outpmessage, "Cla", 1, &Cla);
	MXSetValue(Outpmessage, "Ins", 1, &Ins);
	MXSetValue(Outpmessage, "Size", 1, &outDataSize);
	MXSetValue(Outpmessage, "Data", 1, &OutBuffer);

	free(Buffer->BufferContent);
	free(Buffer);

	Send_APDU(EMVRooterCom, Cla, Ins, P1, P2, outDataSize, outData, 1);

	return 1;
}

int OnRecvVerify(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	BUFFERPARM OutBuffer;
	int outDataSize;
	unsigned char outData[258];
	char strError[250];
	char c;
	unsigned char PinCode[10];
	MXMessage* Outpmessage;
	CARD* pCard = (CARD*)applicationfield;
	BYTE	Enciphered = *(BYTE*)MXGetValue(pmessage, "Enciphered", 1);
	int i = 0;

	memset(PinCode, 0xFF, 10);

	s_printf(smessage, "%s", "Receiving Verify Command from Server \n");

	fseek(stdin, 0, SEEK_END);
	printf(smessage, "Enter Pin Code : ");
	while ((c = getchar()) != '\n')
	{
		PinCode[i] = c;
		i++;
	}

	if (!CardAPDU(pCard, 0x00, 0x20, 0x00, 0x08, 0x08, PinCode, &outDataSize, outData))
	{
		sprintf(strError, "%s", "APDU failed, Verify\n");

		Outpmessage = MXPutMessage(pcom, "APDU", "SendError");
		MXSetValue(Outpmessage, "Error", 1, strError);

		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		return 1;
	}
	// Response data must at least have SW1 SW2
	if (outDataSize < 2)
	{
		sprintf(strError, "%s", "APDU Verify failed, wrong size\n");

		Outpmessage = MXPutMessage(pcom, "APDU", "SendError");
		MXSetValue(Outpmessage, "Error", 1, strError);

		MXCloseCom(pCard->pCom->MX, pCard->pCom);
		return 1;
	}

	OutBuffer.BufferType = 'B';
	OutBuffer.BufferSize = outDataSize;
	OutBuffer.BufferContent = (char*)&outData;

	Outpmessage = MXPutMessage(pcom, "APDU", "RecvVerify");
	MXSetValue(Outpmessage, "Size", 1, &outDataSize);
	MXSetValue(Outpmessage, "Data", 1, &OutBuffer);

	return 1;
}

int OnRecvSendAppliSelection(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	int idx;
	char charindex;


	CARD* pCard = (CARD*)applicationfield;

	BYTE	count = *(BYTE*)MXGetValue(pmessage, "Count", 1);
	STRING  label = (STRING)MXGetValue(pmessage, "Label", 1);

	if (count == 1)
	{
		s_printf(smessage, "Confirm select app %s (y/n): ", label);
		charindex = getchar();
		if (charindex != 'y')
			charindex = -1;
		else
			charindex = 0;
	}
	else
	{
		s_printf(smessage, "%s", "Select application from list:\n");
		char   message[300] = { 0 };

		if (CardConnector->pCardContext->WebUser == 0) {
			// OFFLINE METHOD BLOCKING
			for (idx = 0; idx < count; idx++)
			{
				BYTE	priority = *(BYTE*)MXGetValue(pmessage, "Priority", idx + 1);
				label = (STRING)MXGetValue(pmessage, "Label", idx + 1);
				sprintf(message, "%d (priority %d) - %s", idx, priority, label);
				s_printf(smessage, "%s\n", message);
			}
			s_printf(smessage, "%s", "Index: ");
			fseek(stdin, 0, SEEK_END);
			charindex = getchar() - '0';
			MXMessage* Outpmessage = MXPutMessage(pcom, "APDU", "RecvAppliSelection");
			MXSetValue(Outpmessage, "Index", 1, &charindex);
		}
		else
		{

			for (idx = 0; idx < count; idx++)
			{
				BYTE	priority = *(BYTE*)MXGetValue(pmessage, "Priority", idx + 1);
				label = (STRING)MXGetValue(pmessage, "Label", idx + 1);
				sprintf(message + strlen(message), "%d (priority %d) - %s^", idx, priority, label);
			}
			message[strlen(message) - 1] = 0; // remove the last ^

			Send_Select(EMVRooterCom, message);



			// ONLINE METHOD BLOCKING
			if (EMVRooterCom == 0) {   // stop router while receiving
				MXCloseCom(pCard->pCom->MX, pCard->pCom);
				return 1;
			}
			pmessage = MXRecv(pCard->pCom->MX, EMVRooterCom);

			BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Buffer", 1);
			char* outData = (char*)Buffer->BufferContent;
			DWORD size = Buffer->BufferSize;
			CARD* pCard = (CARD*)applicationfield;
			printf("Receive from router : %.*s\n", size, outData);

			memset(message, 0, 300);

			strncpy(message, outData, size);

			free(Buffer->BufferContent);
			free(Buffer);
			MXFreeMessage(EMVRooterCom->MX, pmessage);


			char Command[20] = { 0 };
			char Parameter[20] = { 0 };
			char UserId[20] = { 0 };



			sscanf(message, "%[0-9]*%[a-zA-Z]*%[0-9]*", UserId, Command, Parameter);

			if (strcmp(Command, "ABORT") == 0) {
				MXCloseCom(pCard->pCom->MX, pCard->pCom);
				return 1;
			}
			else
			{
				if (strcmp(Command, "SELECT") == 0) {
					char charindex;
					charindex = Parameter[0] - '0';

					MXMessage* Outpmessage = MXPutMessage(pcom, "APDU", "RecvAppliSelection");
					MXSetValue(Outpmessage, "Index", 1, &charindex);
					return 1;
				}
			}
		}
	}
	return 1;
}

int SendUserInfo(MXCom* pcom, char* username, char* password) 
{

	printf("Send Userinfo to EMV Server\n");

	MXMessage* pmessage = MXCreateMessage(pcom->MX, "APDU", "UserInfo");

	MXSetValue(pmessage, "UserName", 1, username);
	MXSetValue(pmessage, "UserPassword", 1, password);
	
	MXSend(pcom->MX, pcom, pmessage);
	
	MXFreeMessage(pcom->MX, pmessage);

	return 0;
}

int SendTransaction(MXCom* pcom, char type, char* currency, char* amount, BYTE mediatype)
{

	printf("Send Transaction to EMV Server\n");

	MXMessage* pmessage = MXCreateMessage(pcom->MX, "APDU", "SendTransaction");
	
	MXSetValue(pmessage, "Type", 1, &type);
	MXSetValue(pmessage, "Currency", 1, currency);
	MXSetValue(pmessage, "Amount", 1, amount);
	MXSetValue(pmessage, "Media", 1, &mediatype);
	

	MXSend(pcom->MX, pcom, pmessage);

	MXFreeMessage(pcom->MX, pmessage);
	
	return 0;
}

int SendATR(MXCom* pcom, unsigned char* atr)
{

	printf("Send ATR to EMV Server\n");

	MXMessage* pmessage = MXCreateMessage(pcom->MX, "APDU", "SendATR");

	MXSetValue(pmessage, "Atr", 1, atr);

	MXSend(pcom->MX, pcom, pmessage);

	MXFreeMessage(pcom->MX, pmessage);

	return 0;
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
		s_printf(smessage, "%s", "Close Connection With EMV Server \n");
		pCardContext->ShouldRelase = 1;
		pCardContext->pCurrentState->dwCurrentState = 0;

	}
	else
	{
		printf("Close Connection With Rooter Server \n");
		pcc->pRouterCom = NULL;

		printf("Something wrong we connect again\n");
		Connect_RouterServer(pcc);
	}
	return 1;
}

int CardApplicationProcedure(MX* pmx, void* applicationfield)
{
	CC* pcc = (CC*)applicationfield;

	CardContext* pCardContext = pcc->pCardContext;
	char* szReaderList = NULL;

	DWORD	dwReaderListSize = 0,
		dwNewState,
		dwOldState;

	HRESULT	hr;
	BOOL	fEvent = FALSE;

	LONG    lReturn;


	if (pCardContext->ReadersCount == 0) {
		lReturn = Readers_Init(pCardContext);
		if (lReturn != 0L)
		{
			return ReaderPlugging(pCardContext, lReturn);
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

		DWORD dwStateMask = ~(SCARD_STATE_UNAWARE |
			SCARD_STATE_IGNORE |
			SCARD_STATE_UNAVAILABLE |
			SCARD_STATE_ATRMATCH |
			SCARD_STATE_EXCLUSIVE |
			SCARD_STATE_INUSE |
			SCARD_STATE_MUTE |
			SCARD_STATE_UNPOWERED);


		dwNewState = pCardContext->pCurrentState->dwEventState & dwStateMask;
		dwOldState = pCardContext->pCurrentState->dwCurrentState & dwStateMask;

		if (dwNewState != dwOldState)
		{
			pCardContext->pCurrentState->dwCurrentState = pCardContext->pCurrentState->dwEventState;
			if ((pCardContext->pCurrentState->dwEventState & SCARD_STATE_EMPTY) == SCARD_STATE_EMPTY)
			{
				// Card Removed
				pCardContext->ShouldRelase = 0;

				if (pcc->pRouterCom == NULL) {
					Connect_RouterServer(pcc);
				}
				s_printf(smessage, "%s\n", "Insert Card");
				return 1;
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
				return 1;
				//	pCard->TimeOut = INFINITE;
				//	pCardContext->pCurrentState->dwCurrentState = pCardContext->pCurrentState->dwEventState;
			}
		}
	}
	return 1;  // 0 means block !
}
