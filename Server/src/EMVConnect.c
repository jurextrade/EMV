#include "EMV.h"
#include "Actys.h"


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


int OnConnect(MXCom* pcom, void* applicationfield)
{
	EMV* pemv = (EMV*)applicationfield;

	EMVClient* pclient;

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
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "UserInfo", MXONRECV, OnRecvUserInfo, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "C-APDU", MXONSEND, OnSendAPDU, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "R-APDU", MXONRECV, OnRecvAPDU, pclient);

	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendACFirst", MXONSEND, OnSendACFirst, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "RecvACFirst", MXONRECV, OnRecvACFirst, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendACSecond", MXONSEND, OnSendACSecond, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "RecvACSecond", MXONRECV, OnRecvACSecond, pclient);

	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendATR", MXONRECV, OnRecvATR, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendError", MXONRECV, OnRecvError, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendTransaction", MXONRECV, OnRecvTransaction, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendACFirst", MXONRECV, OnRecvACFirst, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "RecvAppliSelection", MXONRECV, OnRecvAppliSelection, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "RecvCommand", MXONRECV, OnRecvCommand, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "SendCommand", MXONSEND, OnSendCommand, pclient);
	MXAddComCallBack(pemv->pMX, pcom, "APDU", "RecvVerify", MXONRECV, OnRecvVerify, pclient);

	return 1;
}

int OnClose(MXCom* pcom, void* applicationfield)
{
	EMV* pemv = (EMV*)applicationfield;


	if (pcom == pemv->pRouterCom) {
		printf("Close Connection With Rooter Server \n");
		pemv->pRouterCom = NULL;
		EMVRooterCom = NULL;
		printf("Something wrong we connect again\n");
		Connect_RouterServer(pemv);
	}
	else
	{
		EMVClient* pclient = EMVGetClientFromCom(pemv, pcom);
		printf("Close Connection With Point Of Sale  \n");

		EMVEndClient(pemv, pclient);
	}

	return 1;

}

int OnRecvUserInfo(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	printf("Receive Userinfo from point of sale\n");

	STRING UserName = (STRING)MXGetValue(pmessage, "UserName", 1);
	STRING UserPassword = (STRING)MXGetValue(pmessage, "UserPassword", 1);

	strcpy(pclient->UserName, UserName);
	strcpy(pclient->UserPassword, UserPassword);

	if (pemv->pRouterCom == NULL) {
		if (Connect_RouterServer(pemv) == NULL) {   // abort client something wrong with router
			EMVEndClient(pemv, pclient);
			return 1;
		}
	}

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

	//	if (strATR[0] == 0)
	//		CallACTYSS(pemv, pclient);
	//	else
	EMVOnRecvCardDetectionAndReset(pemv, pclient, (BYTE*)strATR, strlen(strATR));

	return 1;
}

int OnSendCommand(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	BYTE P1 = *(BYTE*)MXGetValue(pmessage, "P1", 1);
	BYTE P2 = *(BYTE*)MXGetValue(pmessage, "P2", 1);
	return 1;
}


int OnRecvCommand(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	BYTE P1 = *(BYTE*)MXGetValue(pmessage, "P1", 1);
	BYTE P2 = *(BYTE*)MXGetValue(pmessage, "P2", 1);

	BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);

	int			outSize = *(LONG*)MXGetValue(pmessage, "Size", 1);
	BYTE* outData = (BYTE*)Buffer->BufferContent;

	if (pemv->DebugEnabled)
	{
		EMVTraceAPDU(pclient, 0x80, 0xCA, P1, P2, outData, outSize, 1);
	}

	if (P2 == 0x36) {
		EMVOnRecvATC(pemv, pclient, P1, P2, outData, outSize);
	}
	else
		if (P2 == 0x13)
		{
			EMVOnRecvLastOnlineATC(pemv, pclient, P1, P2, outData, outSize);
		}
		else
			if (P2 == 0x17) {
				EMVOnRecvPinTryCounter(pemv, pclient, P1, P2, outData, outSize);
			}

	free(Buffer->BufferContent);
	free(Buffer);
	return 1;
}

int OnSendACFirst(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	if (pemv->DebugEnabled)
	{
		BYTE Cla = *(BYTE*)MXGetValue(pmessage, "Cla", 1);
		BYTE Ins = *(BYTE*)MXGetValue(pmessage, "Ins", 1);
		BYTE P1 = *(BYTE*)MXGetValue(pmessage, "P1", 1);
		BYTE P2 = *(BYTE*)MXGetValue(pmessage, "P2", 1);
		BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);
		BYTE		outSize = *(BYTE*)MXGetValue(pmessage, "Size", 1);
		BYTE* outData = (BYTE*)Buffer->BufferContent;

		pclient = EMVGetClientFromCom(pemv, pcom);
		EMVTraceAPDU(pclient, Cla, Ins, P1, P2, outData, outSize, 0);
		free(Buffer->BufferContent);
		free(Buffer);
	}
	return 1;
}

int OnRecvACFirst(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	BYTE Cla = *(BYTE*)MXGetValue(pmessage, "Cla", 1);
	BYTE Ins = *(BYTE*)MXGetValue(pmessage, "Ins", 1);
	BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);

	int			outSize = *(LONG*)MXGetValue(pmessage, "Size", 1);
	BYTE* outData = (BYTE*)Buffer->BufferContent;


	if (pemv->DebugEnabled)
	{
		EMVTraceAPDU(pclient, 0, 0, 0, 0, outData, outSize, 1);
	}

	EMVOnRecvACFirst(pemv, pclient, outData, outSize);
	free(Buffer->BufferContent);
	free(Buffer);
	return 1;

}

int OnSendACSecond(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	if (pemv->DebugEnabled)
	{
		BYTE Cla = *(BYTE*)MXGetValue(pmessage, "Cla", 1);
		BYTE Ins = *(BYTE*)MXGetValue(pmessage, "Ins", 1);
		BYTE P1 = *(BYTE*)MXGetValue(pmessage, "P1", 1);
		BYTE P2 = *(BYTE*)MXGetValue(pmessage, "P2", 1);
		BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);
		BYTE		outSize = *(BYTE*)MXGetValue(pmessage, "Size", 1);
		BYTE* outData = (BYTE*)Buffer->BufferContent;

		pclient = EMVGetClientFromCom(pemv, pcom);
		EMVTraceAPDU(pclient, Cla, Ins, P1, P2, outData, outSize, 0);
		free(Buffer->BufferContent);
		free(Buffer);
	}
	return 1;
}


int OnRecvACSecond(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	BYTE Cla = *(BYTE*)MXGetValue(pmessage, "Cla", 1);
	BYTE Ins = *(BYTE*)MXGetValue(pmessage, "Ins", 1);
	BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);

	int			outSize = *(LONG*)MXGetValue(pmessage, "Size", 1);
	BYTE* outData = (BYTE*)Buffer->BufferContent;


	if (pemv->DebugEnabled)
		EMVTraceAPDU(pclient, 0, 0, 0, 0, outData, outSize, 1);

	EMVOnRecvACSecond(pemv, pclient, outData, outSize);
	free(Buffer->BufferContent);
	free(Buffer);
	EMVTerminateTransaction(pemv, pclient);
	return 1;

}


int OnSendAPDU(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	if (pemv->DebugEnabled)
	{
		BYTE Cla = *(BYTE*)MXGetValue(pmessage, "Cla", 1);
		BYTE Ins = *(BYTE*)MXGetValue(pmessage, "Ins", 1);
		BYTE P1 = *(BYTE*)MXGetValue(pmessage, "P1", 1);
		BYTE P2 = *(BYTE*)MXGetValue(pmessage, "P2", 1);
		BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);
		BYTE		outSize = *(BYTE*)MXGetValue(pmessage, "Size", 1);
		BYTE* outData = (BYTE*)Buffer->BufferContent;

		EMVTraceAPDU(pclient, Cla, Ins, P1, P2, outData, outSize, 0);
		free(Buffer->BufferContent);
		free(Buffer);
	}
	return 1;

}


int OnRecvAPDU(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	BYTE Cla = *(BYTE*)MXGetValue(pmessage, "Cla", 1);
	BYTE Ins = *(BYTE*)MXGetValue(pmessage, "Ins", 1);
	WORD outSize = *(WORD*)MXGetValue(pmessage, "Size", 1);
	BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);


	BYTE* outData = (BYTE*)Buffer->BufferContent;

	if (pemv->DebugEnabled)
		EMVTraceAPDU(pclient, 0, 0, 0, 0, outData, outSize, 1);

	if (pclient->Step == EMV_STEP_READ_APPLICATION_DATA)
	{
		EMVOnRecvReadApplicationData(pemv, pclient, outData, outSize);
	}
	else
		if (pclient->Step == EMV_STEP_APPLICATION_SELECTION)
		{
			EMVOnRecvApplicationSelection(pemv, pclient, outData, outSize);
		}
		else
			if (pclient->Step == EMV_STEP_CANDIDATE_LIST_CREATION)
			{
				EMVOnRecvCandidateListCreation(pemv, pclient, outData, outSize);
			}
	free(Buffer->BufferContent);
	free(Buffer);
	return 1;
}



int OnRecvVerify(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;

	BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Data", 1);
	BYTE		outSize = (BYTE)Buffer->BufferSize;
	BYTE* outData = (BYTE*)Buffer->BufferContent;

	if (pemv->DebugEnabled)
		EMVTraceAPDU(pclient, 0, 0, 0, 0, outData, outSize, 1);

	EMVOnRecvVerify(pemv, pclient, outData, outSize);

	return 1;
}


int OnRecvError(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;
	STRING strError = (STRING)MXGetValue(pmessage, "Error", 1);
	printf("Error from client %s\n", strError);
	return 1;
}

//CB2A

int OnRecvReply(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;
	BYTE* Identifier = (BYTE*)MXGetValue(pmessage, "Identifier", 1);
	BUFFERPARM* Buffer = (BUFFERPARM*)MXGetValue(pmessage, "Buffer", 1);

	if (pemv->DebugEnabled)
	{
		printf("Receiving Response From Acquirer\n");
	}
	EMVScriptProcessing(pemv, pclient);

	free(Buffer->BufferContent);
	free(Buffer);

	return 1;
}

int OnRecvAppliSelection(MXMessage* pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;
	CHAR		Index = *(CHAR*)MXGetValue(pmessage, "Index", 1);

	EMVSelectApplication(pemv, pclient, (int)Index);
	return 1;
}

