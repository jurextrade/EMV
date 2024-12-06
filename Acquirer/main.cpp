#define _CRT_SECURE_NO_WARNINGS
#include "mx.h"


int OnConnect (MXCom* pcom, void* applicationfield)
{
	printf ("Open Connection With Server \n");
	return 1;
}

int OnClose (MXCom* pcom, void* applicationfield)
{
	printf ("Close Connection With Server \n");
	return 1;

}

int OnRecvStream (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{

	MXMessage* Outpmessage;
	BYTE		Identifier[2];


	Identifier[0] = *(BYTE*)MXGetValue (pmessage, "Identifier", 1);
	Identifier[1] = *(BYTE*)MXGetValue (pmessage, "Identifier", 2);

	BUFFERPARM* Buffer = (BUFFERPARM *)MXGetValue (pmessage, "Buffer", 1);
	Identifier[1] = 10;
	printf ("Receiving Autorisation Command from Acquirer \n");

	Outpmessage = MXPutMessage (pcom, "CB2A", "Reply");
	MXSetValue(Outpmessage, "Identifier",		1,	&Identifier[0]) ;
	MXSetValue(Outpmessage, "Identifier",		2,	&Identifier[1]) ;
	MXSetValue (Outpmessage, "Buffer",  1, Buffer);
	
 //   free (Buffer->BufferContent);
  //  free (Buffer);
	return 1;
}



int main(int argc, char **argv)
{
	
	MX mx;
	MXInit (&mx, MXSERVER, NULL, NULL, 0, NULL);
	MXAddPort (&mx, 8000, IPPROTO_CB2A);
	MXAddGeneralConnectCallBack (&mx, MXONCONNECT, OnConnect, NULL);
	MXAddGeneralConnectCallBack (&mx, MXONCLOSE,   OnClose,   NULL);
	MXAddGeneralCallBack (&mx, "CB2A", "Stream", MXONRECV, OnRecvStream, NULL);
	MXDispatchEvents (&mx, NULL);
	
	return 0;
}
