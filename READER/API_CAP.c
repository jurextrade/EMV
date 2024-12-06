/**************************************************************************/
/*                   Copyright 2000 WYNID TECHNOLOGIES                    */
/**************************************************************************/
/*
* MODULE NAME : API_CAP
*
* FILE NAME : API_CAP.c
*
* PRIVATE :
*
* DESCRIPTION : Permet au application d'acceder au driver CAP
*
***************************************************************************/

/**************************************************************************/
/*                     Historique des modifications                       */
/**************************************************************************/
/*
*		Version 0.01 :
*     	Creation, premier version fonctionnelle.
*
***************************************************************************/


/**************************************************************************/
/*                              Includes                                  */
/**************************************************************************/

/* Module External includes  */
#include "Id_Taches.h"
/* Module Internal includes  */
#include "API_CAP.h"
#include "utils.h"

/**************************************************************************/
/*                               Defines                                  */
/**************************************************************************/

/**************************************************************************/
/*                               Macros                                   */
/**************************************************************************/

/**************************************************************************/
/*                              Typedefs                                  */
/**************************************************************************/

/**************************************************************************/
/*                  Global Variables (GLOBAL / IMPORT)                    */
/**************************************************************************/

/**************************************************************************/
/*                 Local Variables to the Module (MODULE)                 */
/**************************************************************************/

/**************************************************************************/
/*                  Local Functions Declarations (LOCAL)                  */
/**************************************************************************/

/**************************************************************************/
/*                   Local Variables to the File (LOCAL)                  */
/**************************************************************************/

/**************************************************************************/
/*                  Functions Definitions (LOCAL / GLOBAL)                */
/**************************************************************************/

/***************************************************************************
* Function Name  : CAM_RetraitCarte
*
* Description    :
*
* Side effects   :
*
* Comment        :
*
***************************************************************************/
unsigned char CAM_RetraitCarte (OS_HANDLE *pP_Handle)
{
unsigned char * pL_Reponse;
unsigned char * cpl_ActionAddress;
unsigned char   cL_ResultOS ;
unsigned char   cL_CapStatus ;
unsigned char   cL_CamCommand = CAM_ACTION_WAIT_REMOVAL_INTERRUPT ;
unsigned int    ul_ActionStatus;
unsigned char * cpl_NoWarning;
unsigned char   uc_Erreur=FALSE;

	cpl_ActionAddress = &cL_CamCommand;
	cL_ResultOS       = SendMail(ID_CAM,
        	                       0,
                	              pP_Handle->cId,
                        	     0,
                             	  0,
                        	     KEEP_MEMORY_BEFORE_SENDING,
                        	     0,
                        	     1,
                        	     &cpl_ActionAddress,
                        	     pP_Handle);

	// ICI RECUPERATION MESSAGE BOITE A LETTRE
	pL_Reponse=OSMboxPend(pP_Handle->pMBox,0,&cL_ResultOS); // Attente Bloquante

   if ((cL_ResultOS == OS_NO_ERR)
    && (pL_Reponse != NULL))
   {
      uc_Erreur = TRUE;
   }

   OS_Free(pL_Reponse);
   pL_Reponse = NULL;

   return( uc_Erreur );
}

/***************************************************************************
* Function Name  : CAM_InsertionCarte
*
* Description    :
*
* Side effects   :
*
* Comment        :
*
***************************************************************************/
unsigned char CAM_InsertionCarte (OS_HANDLE *pP_Handle)
{
unsigned char * pL_Reponse;
unsigned char * cpl_ActionAddress;
unsigned char   cL_ResultOS ;
unsigned char   cL_CapStatus ;
unsigned char   cL_CamCommand = CAM_ACTION_WAIT_INSERTION_INTERRUPT ;
unsigned int    ul_ActionStatus;
unsigned char * cpl_NoWarning;
unsigned char   uc_Erreur=FALSE;

	cpl_ActionAddress = &cL_CamCommand;
	cL_ResultOS       = SendMail(ID_CAM,
        	                       0,
                	              pP_Handle->cId,
                        	     0,
                             	  0,
                        	     KEEP_MEMORY_BEFORE_SENDING,
                        	     0,
                        	     1,
                        	     &cpl_ActionAddress,
                        	     pP_Handle);

	// ICI RECUPERATION MESSAGE BOITE A LETTRE
	pL_Reponse=OSMboxPend(pP_Handle->pMBox,0,&cL_ResultOS); // Attente Bloquante

   if ((cL_ResultOS == OS_NO_ERR)
    && (pL_Reponse != NULL))
   {
      uc_Erreur = TRUE;
   }

   OS_Free(pL_Reponse);
   pL_Reponse = NULL;

   return( uc_Erreur );
}

/***************************************************************************
* Function Name  : CAP_AttenteCarte
*
* Description    :
*
* Side effects   :
*
* Comment        :
*
***************************************************************************/
unsigned char CAP_AttenteCarte (OS_HANDLE *pP_Handle,
				unsigned int iTimeOut,
				unsigned char *pP_Donnees )
{
unsigned char * pL_Reponse;
unsigned char * cpl_ActionAddress;
unsigned char   cL_ResultOS ;
unsigned char   cL_CapStatus ;
unsigned char   cL_CamCommand = ISO2_READING_COMMAND ;
unsigned int    ul_ActionStatus;
unsigned char * cpl_NoWarning;
unsigned char   uc_Erreur=FALSE;

	cpl_ActionAddress = &cL_CamCommand;
	cL_ResultOS       = SendMail(TASK_ISO2_ID,
        	                     0,
                	             pP_Handle->cId,
                        	     0,
                             	     0,
                        	     KEEP_MEMORY_BEFORE_SENDING,
                        	     0,
                        	     1,
                        	     &cpl_ActionAddress,
                        	     pP_Handle);

	// ICI RECUPERATION MESSAGE BOITE A LETTRE
	pL_Reponse=OSMboxPend(pP_Handle->pMBox,iTimeOut,&cL_ResultOS); // Attente Bloquante

   if ((cL_ResultOS == OS_NO_ERR)
   && (pL_Reponse != NULL))
   {
       /* +++++> Pistes ISO lues a priori ++++++++++++++++++++++++++++++ */
       if ( TLVToInt(&pL_Reponse[MS_DebutMessage+2])==ISO_STATUS_READ )
       {
          UTS_memcpy(pP_Donnees,&pL_Reponse[MS_DebutMessage+12],CAP_LONGUEUR_ISO2_MAX);

          uc_Erreur = TRUE;
       }
       else
       {
          uc_Erreur = FALSE;
       }
   }

   OS_Free(pL_Reponse);
   pL_Reponse = NULL;

   return( uc_Erreur );
}
