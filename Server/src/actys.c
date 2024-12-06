
#include "actys.h"

 unsigned char aucL_Tampon[8] = "\x70\x10\xFF\x02\x50\x00\x00";



void ACTYS_SelectFile (EMV* pemv, EMVClient* pclient, BYTE  ucP_TSelect,  unsigned int   ip_LgFileName, BYTE* ucP_FileName) {
    MXMessage* pmessage = pemv->APDU (pemv, pclient, 0x00, INS_SELECT, ucP_TSelect, 0x00, ip_LgFileName, ucP_FileName);
    
    MXSend (pemv->pMX, pclient->pPointOfSale->pCom, pmessage);
    MXFreeMessage (pemv->pMX, pmessage);

    pmessage = MXRecv (pemv->pMX, pclient->pPointOfSale->pCom);
    if (pmessage)
     MXFreeMessage (pemv->pMX, pmessage);

}

void ACTYS_ReadBinary (EMV* pemv, EMVClient* pclient, int TypeFichier,  BYTE Offset) {
    MXMessage* pmessage = pemv->APDU (pemv, pclient, 0x00, 0xB0, 0x80 + TypeFichier, Offset, 0, "");
    MXSend (pemv->pMX, pclient->pPointOfSale->pCom, pmessage);
    MXFreeMessage (pemv->pMX, pmessage);

    pmessage = MXRecv (pemv->pMX, pclient->pPointOfSale->pCom);
    if (pmessage)
     MXFreeMessage (pemv->pMX, pmessage);

    if (TypeFichier == 1) {
	
	}



}


void ACTYS_Read_File (EMV* pemv, EMVClient* pclient, int  TypeFichier) {
    BYTE auc_AdresseFichier[3];
    BYTE uc_NbReadLoop;
    unsigned int  ui_LgCFile;
    BYTE Offset;
    BYTE ToRead;
    int i;
	BYTE *   puc_BufferCFile = NULL;

    if (TypeFichier == 1)    ui_LgCFile = LG_C1_FILE;
    if (TypeFichier == 2)    ui_LgCFile = LG_C2_FILE;
    if (TypeFichier == 3)    ui_LgCFile = LG_C3_FILE;


	puc_BufferCFile   = (BYTE*) malloc(ui_LgCFile);


    // Selection du fichier EF-Data

    auc_AdresseFichier[0]= 0x80 + TypeFichier;

    // Nb de lecture de 252 octets a faire
    uc_NbReadLoop = ui_LgCFile / 0xFC + 1;

    /* *****> Lecture C File *********** */
    for (i = 0; i < uc_NbReadLoop; i++)    {

       Offset = 0x3F * i;  // Offset: adressage de mot (4 octets)

       // dernier passage ?
       if (i == (uc_NbReadLoop - 1))
          ToRead = ui_LgCFile % 0xFC;
       else
          ToRead = 0xFC;

       ACTYS_ReadBinary(pemv, pclient,  TypeFichier, Offset);


    }



}

int OnRecvAPDU1 (MXMessage*  pmessage, MXCom* pcom, void* applicationfield)
{
	EMVClient* pclient = (EMVClient*)applicationfield;
	EMV* pemv = pclient->pEMV;
	BYTE Cla					= *(BYTE*)MXGetValue(pmessage, "Cla", 1) ;
	BYTE Ins					= *(BYTE*)MXGetValue(pmessage, "Ins", 1) ;
	BUFFERPARM* Buffer			= (BUFFERPARM *)MXGetValue (pmessage, "Data", 1);
	
	int			outSize			= *(LONG*)MXGetValue (pmessage, "Size", 1);
	BYTE*		outData			= (BYTE*)Buffer->BufferContent;
	
	EMVSetTag(pclient, TAG_TRACK1_EQUIVALENT_DATA, outData, outSize-2);
	
	free (Buffer->BufferContent);
	free (Buffer);
    MXCloseCom (pemv->pMX, pcom);
	return 1;
}

void CallACTYSS (EMV* pemv, EMVClient* pclient) {
    
    BYTE auc_AdresseFichier[3];

    // Build

            ACTYS_SelectFile(pemv, pclient, kSelectAID, 7, aucL_Tampon);

			

			
			ACTYS_Read_File (pemv, pclient, 1);
  			ACTYS_Read_File (pemv, pclient, 2);
			ACTYS_Read_File (pemv, pclient, 3);
				
			auc_AdresseFichier[0] = 0x3F;
            auc_AdresseFichier[1] = 0x00;
            // Authentification
            ACTYS_SelectFile(pemv, pclient, kSelectMasterFile, 2, auc_AdresseFichier);

            /* Selection du DF System */
            auc_AdresseFichier[0] = 0x01;
            auc_AdresseFichier[1] = 0x00;
            ACTYS_SelectFile(pemv, pclient, kSelectDirectoryFile, 2, auc_AdresseFichier);

            /* Lecture du numero de serie de la carte CSN */
            auc_AdresseFichier[0] = 0x81; /* Selection du fichier de SFI 1 */
            auc_AdresseFichier[1] = 0x00; /* Offset 0 */
            auc_AdresseFichier[2] = 0x08; /* Lecture des 8 premiers octets */

        //    ACTYS_ReadBinary( pemv, pclient, auc_AdresseFichier, &pL_ActysInfoCarte->aucCSN[1], &i);
         //   pL_ActysInfoCarte->aucCSN[0] = i;
            /* <***** Recup Numero CSN ********************* */

            // Select Master File
            auc_AdresseFichier[0]=0x3F;
            auc_AdresseFichier[1]=0x00;
            ACTYS_SelectFile(pemv, pclient, kSelectMasterFile,  2,  auc_AdresseFichier);

            /* Selection du ADF System */
            auc_AdresseFichier[0]=0x03;
            auc_AdresseFichier[1]=0x00;
            ACTYS_SelectFile(pemv, pclient, kSelectDirectoryFile, 2,  auc_AdresseFichier);

            // Select Elementary File
            auc_AdresseFichier[0]=0x03;
            auc_AdresseFichier[1]=0x07;
            ACTYS_SelectFile(pemv, pclient, kSelectFile, 2,  auc_AdresseFichier);

			ACTYS_Read_File (pemv, pclient, 1);
  			ACTYS_Read_File (pemv, pclient, 2);
			ACTYS_Read_File (pemv, pclient, 3);


		MXAddComCallBack (pemv->pMX, pclient->pPointOfSale->pCom,  "APDU", "R-APDU", MXONRECV, OnRecvAPDU1, pclient);
		ACTYS_Read_File (pemv, pclient, 1);
				/* <***** Deplacement jusqu'a EFkey *********** */

            // Generation d'un nb aleatoire


            /* *****> Demande d'authentification *********** 
            if(eL_ActysStatus == kACTYS_STATUS_OK)
            {
              eL_ActysStatus = ACTYS_SelectFileKey(pemv, pclient, pL_ActysInfoCarte->puc_Trnd, &pL_DataOut);
            }

     
            /* <***** Demande d'authentification *********** */


}