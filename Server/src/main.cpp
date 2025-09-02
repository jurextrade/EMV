#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include "EMV.h"
#include "cb2a.h"



using namespace std;

// -----------------------------------------------------------------------MAIN PROCEDURE  -------------------------------------------------------

int EMVReadFile (EMV* pemv)
{
	char filename[200]; 
	sprintf (filename, "%s\\Files\\%s", Directory, "FILES.csv"); 

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
		EMVAddApduError(pemv, pemverror);
		k++;
	}

	inFile.close();
	return (0);
}

int EMVReadTagFile (EMV* pemv)
{
	char filename[200]; 
	sprintf (filename, "%s\\Files\\%s", Directory, "TAGS.csv"); 

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
	sprintf (filename, "%s\\Files\\%s", Directory, "CB2A.csv"); 

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
	
	//char	filename[200];
	//sprintf (filename, "%s\\MX\\%s", Directory, "apdu.mx"); 
	
	MXInit (&mx, MXSERVER, NULL, NULL, EMVServerPort,  NULL);	
	MXAddAPDUCommands(&mx);

	pemv = EMVInit(&mx);
	
	if (EMVLoadProject(pemv, DefaultProjectName) < 0)     //load local demo project 
	{
		printf("Could not find Project %s\n", DefaultProjectName);
	}

	Connect_RouterServer(pemv);

	MXAddGeneralConnectCallBack (&mx, MXONCONNECT, OnConnect, pemv); //connections from clients only
	MXAddGeneralConnectCallBack (&mx, MXONCLOSE,   OnClose,   pemv);
	


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
	CharToHexaChar ((BYTE *)"111", Ascii, 6);

	l = CharArrayToBCDArray ("C12345", 6, 12, "x+n", Ascii);
	l = CharArrayToCharArray ("12345", 5, 12, "n", Ascii);
	l = CharArrayToCharArray ("AGENCE2", 7, 12, "an", Ascii);
	l = CharArrayToCharArray ("C12345 ", 6, 12, "x+n", Ascii);

	l = CharArrayToHexaArray ("3CDE1245EF7684172048CBFF ", 24, 12, "anb", Ascii);
	DateGetGMTDateHour (hour, date);
	DateGetLocalDateHour (hour, date);
*/

