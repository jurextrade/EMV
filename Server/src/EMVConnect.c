#include "EMV.h"
#include "Actys.h"



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

