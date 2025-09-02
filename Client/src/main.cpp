#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <fstream>
#include <sstream>
#include "Card.h"

using namespace std;


// -----------------------------------------------------------------------MAIN MX PROCEDURE  -------------------------------------------------------


int main(int argc, char** argv)    // arg : EMVServer EmvRooter
{
	MX			mx;

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

//	char filename[200];
//	sprintf(filename, "%s\\MX\\%s", Directory, "apdu.mx");

	MXInit(&mx, MXCLIENT, NULL, NULL, -1, NULL);
	MXAddAPDUCommands(&mx);

	pcc = CCInit(&mx);
	
	if (pcc == NULL) {
		MXEnd(&mx);
		return 0;

	}
	CardConnector = pcc;
	MXAddGeneralConnectCallBack(&mx, MXONCONNECT,	OnConnect, pcc);
	MXAddGeneralConnectCallBack(&mx, MXONCLOSE,		OnClose,   pcc);
	MXAddApplicationProcedure(&mx, CardApplicationProcedure,   pcc);
	

	while (Connect_RouterServer(pcc) == NULL)
	{
		char input[300] = { 0 };
		printf("Please Launch Router Server and type a char\n>");
		scanf("%s", input);
//		getchar();
	}

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


int EMVReadApduErrorFile(CardContext* pCardContext)
{
	char filename[200];
	sprintf(filename, "%s\\Files\\%s", Directory, "SW1SW2.csv");

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
