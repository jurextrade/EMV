#include "EMV.h"





void EMVAddFile (EMV* pemv, EMVFile* pfile)
{
	ListNewr (&pemv->Settings.appFiles, pfile);
}


int EMVGenerateMXFile (EMV* pemv, char* classname, int classindex, char* dialogfilename)
{
	FILE *  fileid;
	List*	FileList = pemv->Settings.appFiles;

	

    fileid = fopen(dialogfilename, "w");
    if (!fileid)
        return -1;


	fprintf (fileid, "%s %d *%s*\n", "DIALOGCLASS", classindex, classname);
	fprintf (fileid, "%s\n", "BEGIN");
	while (FileList)
	{
		EMVFile* pemvfile = (EMVFile*)FileList->car;
		if (strcmp (pemvfile->Format, "") != 0) 
			fprintf (fileid, "%5s %s (%s)\n", "MESSAGECLASS", pemvfile->Name, pemvfile->Format);
		FileList = FileList->cdr;
	}
	fprintf (fileid, "%s\n", "END");
	fclose (fileid);
	return 1;
}

void EMVLoadAcceptor (EMV* pemv)  //TAG DF04  DF20-DF21-DF22-DF23 File index 4
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	pemv->pAcceptor = NULL;

	sprintf (filename, "%s\\WPAF\\%s", Directory, "SIT_D753.wp");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "SIT_D753"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "SIT_D753", MXONRECV, OnLoadAcceptor, pemv);
	//Only one line
	pmessage = MXRecv (pemv->pMX, pFileCom);
	MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
}

void EMVLoadApplications (EMV* pemv)    //DF17
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	

	sprintf (filename, "%s\\WPAF\\%s", Directory, "EPV_D787.wp");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "EPV_D787"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "EPV_D787", MXONRECV, OnLoadApplications, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	
}
void EMVLoadTacs (EMV* pemv)    //DF18
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	


	sprintf (filename, "%s\\WPAF\\%s", Directory, "EPT_D778.wp");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "EPT_D778"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "EPT_D778", MXONRECV, OnLoadTacs, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	
}

void EMVLoadCurrencies (EMV* pemv)    
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	


	sprintf (filename, "%s\\WPAF\\%s", Directory, "MON_D747.wp");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "MON_D747"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "MON_D747", MXONRECV, OnLoadCurrencies, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
	
}

void EMVLoadExceptionCards (EMV* pemv)
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	pemv->ExceptionCardList = NULL;	

	sprintf (filename, "%s\\WPAF\\%s", Directory, "APL_D253.wp");
	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "APL_D253"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "APL_D253", MXONRECV, OnLoadExceptionCards, pemv);

	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
}

void EMVLoadRangeBins (EMV* pemv)
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	pemv->RangBinList = NULL;

	sprintf (filename, "%s\\WPAF\\%s", Directory, "APB_D236.wp");

	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "APB_D236"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "APB_D236", MXONRECV, OnLoadRangeBins, pemv);
	
	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
}


void EMVLoadAuthorityPublicKeys (EMV* pemv)
{
	MXCom*				pFileCom;
	MXMessage*			pmessage;
	char				filename[200]; 
	
	
	sprintf (filename, "%s\\WPAF\\%s", Directory, "EPK_D782.wp");

	
	pFileCom = MXOpenFile (pemv->pMX, filename, "r", IOPROTO_FIOP, 'D', 63);
	if (!pFileCom) return; 

	MXSetIOMessageClass(pFileCom, MXGetMessageClassFromName (MXGetDialogClassFromName (pemv->pMX, "LOAD"), "EPK_D782"));
	MXAddComCallBack (pemv->pMX, pFileCom,  "LOAD", "EPK_D782", MXONRECV, OnLoadAuthorityPublicKeys, pemv);
	
	while (pmessage = MXRecv (pemv->pMX, pFileCom))
		MXFreeMessage (pemv->pMX, pmessage);
	
	MXCloseCom (pemv->pMX, pFileCom); 
}