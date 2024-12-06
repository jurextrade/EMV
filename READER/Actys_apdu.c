#include "API_CAM.h"
#include "OS.h"
#include "TLV.h"
#include "utils.h"
#include "API_DISPLAY.h"
#include "API_FLASH.h"
#include <string.h>   /* Pour le memset */

#include "Total.h"
#include "key.h"
#include "des.h"


/******************************************************************************/
/*                             Commmandes CAM                                 */
/******************************************************************************/

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_SelectFile --> Demande la realisation par la carte à
**                                    mémoire de la commande SELECT DIRECTORY FILE
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_CamConf    --> Pointeur sur la structure de dialogue CAM
**           pL_FileName   --> Nom du fichier a selectionner dans la CAM
**           iL_LgFileName --> Longueur du nom du fichier à selectionner
**
** RETOUR  : kACTYS_STATUS_ERREUR_CAM        --> Incident CAM
**           kACTYS_STATUS_ERREUR_SW1        --> Commande CAM echouee
**           kACTYS_STATUS_OK                --> Commande reussie
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_SelectFile(OS_HANDLE*     pP_Handle,
                              CAM_CONF*      pL_CamConf,
                              unsigned char  ucP_TSelect,
                              unsigned int   ip_LgFileName,
                              unsigned char* ucP_FileName)
{
  CAM_STATUS     eL_CamStatus;        /* --> Etat des traitements CAM         */

  /* =====> CONSTRUCTION DE L'ORDRE CAM ===================================== */
  pL_CamConf->xDialogue.cOrdreType   = kCAM_ORDRE_IN;
  pL_CamConf->xDialogue.iLenFromCard = kCAM_LG_MAX_RECEPTION;

  /* Copie des octets fixes de la commande */
  pL_CamConf->xDialogue.pBufferToCard[0] = kACTYS_CLA_INTER_INDUSTRY; /* 0x00 */
  pL_CamConf->xDialogue.pBufferToCard[1] = kACTYS_INS_SELECT;         /* 0xA4 */

  pL_CamConf->xDialogue.pBufferToCard[2] = ucP_TSelect;

  pL_CamConf->xDialogue.pBufferToCard[3] = kACTYS_P2_SELECT;	      /* 0x00 */

  pL_CamConf->xDialogue.pBufferToCard[4] = ip_LgFileName;


  /* Copie du Nom de fichier à sélectionner */
  UTS_memcpy(&pL_CamConf->xDialogue.pBufferToCard[5],
             ucP_FileName,
             ip_LgFileName);

  /* Copie de Le */
  pL_CamConf->xDialogue.pBufferToCard[5 + ip_LgFileName] = kACTYS_Le_SELECT;		  /* 0x00 */

  /* Copie de la longueur total de la commande */
  pL_CamConf->xDialogue.iLenToCard = 6 + ip_LgFileName;


  /* <===== CONSTRUCTION DE L'ORDRE CAM ===================================== */

  /* =====> REALISATION DE LA COMMANDE CAM ================================== */


   eL_CamStatus = CAM_SetCommand(pP_Handle, &pL_CamConf, kCAM_COMMAND_SEND);

   if ((eL_CamStatus != kCAM_STATUS_OK)
    || (pL_CamConf->iState & kCAM_STATE_ERROR)
    || !(pL_CamConf->iState & kCAM_STATE_POWERED)
    || (pL_CamConf->iState & kCAM_STATE_REMOVED))
   {
     /* -----> Incident CAM ------------------------------------------------ */
     return(kACTYS_STATUS_ERREUR_CAM);
     /* <----- Incident CAM ------------------------------------------------ */
   }
   /* <===== REALISATION DE LA COMMANDE CAM ================================ */
   if ((pL_CamConf->cSW1 == kACTYS_WS1_OK        /* 90 */
     && pL_CamConf->cSW2 == kACTYS_WS2_OK)       /* 00 */
     || pL_CamConf->cSW1 == kACTYS_WS1_WARNING ) /* 61 */
   {
     /* -----> Commande SELECT réussie ------------------------------------- */
     return(kACTYS_STATUS_OK);
     /* <----- Commande SELECT réussie ------------------------------------- */
   }
     return(kACTYS_STATUS_ERREUR_SW1);

}


/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_ReadBinary	   --> Demande la réalisation par la carte a
**                                        mémoire de la commande ReadBinary
**
** APPEL   : pP_Handle             --> Handle de la tache ACTYS
**           pL_CamConf            --> Pointeur sur la structure de dialogue CAM
**           pL_FichierSelectionne --> infos de l'enregistrement à lire dans la CAM
**                                     record Number+Refer control Byte+record length
**           pL_DataOut            --> Donnees recues
**           pui_LgDataOut         --> Lg Donnees recues
**
** RETOUR  : kACTYS_STATUS_ERREUR_CAM        --> Incident CAM
**           kACTYS_STATUS_ERREUR_SW1        --> Commande CAM échouée
**           kACTYS_STATUS_OK                --> Commande réussie
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_ReadBinary(OS_HANDLE     * pP_Handle,
                              CAM_CONF      * pL_CamConf,
                              unsigned char * pL_FichierSelectionne,
			                     unsigned char * pL_DataOut,
			                     unsigned int  * pui_LgDataOut)

{
  CAM_STATUS     eL_CamStatus ; /* Status de la commande CAM Read record */

  /* Ordre READ BINARY */
  pL_CamConf->xDialogue.cOrdreType = kCAM_ORDRE_OUT;/* A ne pas oublier si T=0 */
  /* Longueur max en reception */
  pL_CamConf->xDialogue.iLenFromCard = kCAM_LG_MAX_RECEPTION ;

  /* construction de la commande */
  pL_CamConf->xDialogue.pBufferToCard[0] = kACTYS_CLA_INTER_INDUSTRY; /* 0x00 */
  pL_CamConf->xDialogue.pBufferToCard[1] = kACTYS_INS_READ_BINARY;    /* 0xB0 */

 /* Copie des données de l'enregistrement à lire*/
  UTS_memcpy(&pL_CamConf->xDialogue.pBufferToCard[2],
             pL_FichierSelectionne,
             kACTYS_READ_BINARY_LEN);

  /* Longueur de la commande */
  pL_CamConf->xDialogue.iLenToCard = 5;


  /* Appel de la commande kCAM_COMMAND_SEND */
  eL_CamStatus = CAM_SetCommand (pP_Handle, &pL_CamConf, kCAM_COMMAND_SEND) ;

  /* -------------Test: pas de problèmes lors de la commande ---------------*/
  if ((eL_CamStatus != kCAM_STATUS_OK)
   || (pL_CamConf->iState & kCAM_STATE_ERROR)
   || !(pL_CamConf->iState & kCAM_STATE_POWERED)
   || (pL_CamConf->iState & kCAM_STATE_REMOVED))
  {
    /* -----> Incident CAM ------------------------------------------------ */
    return(kACTYS_STATUS_ERREUR_CAM);
    /* <----- Incident CAM ------------------------------------------------ */
  }

  /* ---------------Aucun problème Statut-----------------------------------*/
  if ((pL_CamConf->cSW1 == kACTYS_WS1_OK        /* 90 */
    && pL_CamConf->cSW2 == kACTYS_WS2_OK)       /* 00 */
    || pL_CamConf->cSW1 == kACTYS_WS1_WARNING ) /* 61 */
   {
    /* --------------Test: aucune donnée reçue -----------------------------*/

    if (pL_CamConf->xDialogue.iLenFromCard == 0)
    {
      *pui_LgDataOut = 0;
      return(kACTYS_STATUS_ERREUR_CAM);
    }
    /* Copie des données dans le pointeur retour */
    UTS_memcpy(pL_DataOut,
               pL_CamConf->xDialogue.pBufferFromCard,
               pL_CamConf->xDialogue.iLenFromCard);
    *pui_LgDataOut = pL_CamConf->xDialogue.iLenFromCard;
    return(kACTYS_STATUS_OK);
  }
  /* -------------- Erreur SW1 ---------------------------------------------*/
  *pui_LgDataOut = 0;
  return ( kACTYS_STATUS_ERREUR_SW1 ) ;

}


/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_UpdateBinary --> Demande la realisation par la carte
**                                          d'une mise à jour d'un Record
**
** APPEL   : pP_Handle             --> Handle de la tache ACTYS
**           pL_CamConf            --> Pointeur sur la structure de dialogue CAM
**           pL_FichierSelectionne --> infos de l'enregistrement à liredans la CAM
**                                     record Number+Refer control Byte+record length
**           pL_DataIn             --> Donnees envoyées
**
** RETOUR  : kACTYS_STATUS_ERREUR_CAM        --> Incident CAM
**           kACTYS_STATUS_ERREUR_SW1        --> Commande CAM echouee
**           kACTYS_STATUS_OK                --> Commande reussie
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_UpdateBinary(OS_HANDLE *pP_Handle,
                                CAM_CONF  *pL_CamConf,
                                unsigned char *pL_FichierSelectionne,
                                unsigned char *pL_DataIn,
                                unsigned char *puc_TKgr)

{
  CAM_STATUS     eL_CamStatus ; /* Status de la commande CAM Update record    */
  unsigned char * puc_RES1;
  unsigned char * InitVector;
  unsigned int    ui_Lg;

  pL_CamConf->xDialogue.cOrdreType = kCAM_ORDRE_IN;/* A ne pas oublier si T=0 */
  /* Longueur max en reception */
  pL_CamConf->xDialogue.iLenFromCard = kCAM_LG_MAX_RECEPTION ;

  /* construction de la commande */
  pL_CamConf->xDialogue.pBufferToCard[0] = kACTYS_CLA_INTER_INDUSTRY_SECURED ;/* 0x04 */
  pL_CamConf->xDialogue.pBufferToCard[1] = kACTYS_INS_UPDATE_BINARY ; /* 0xD6 */

 /* Copie de l'enregistrement à mettre à jour */
  UTS_memcpy(&pL_CamConf->xDialogue.pBufferToCard[2],
             pL_FichierSelectionne,
             kACTYS_READ_BINARY_LEN);

  /* Copie des données de l'enregistrement à mettre à jour */
  UTS_memcpy(&pL_CamConf->xDialogue.pBufferToCard[2+kACTYS_READ_BINARY_LEN],
             pL_DataIn,
             pL_FichierSelectionne[kACTYS_READ_BINARY_LEN - 1]);

  //Calcul du Mac sur les donnees a ecrire.
  InitVector = OS_Malloc(8);
  memset(InitVector,0,8);
  ui_Lg = 5+pL_FichierSelectionne[2]-3;

  puc_RES1 = CHIF_MAC8 (pL_CamConf->xDialogue.pBufferToCard, ui_Lg, puc_TKgr ,InitVector);

  UTS_memcpy(&pL_CamConf->xDialogue.pBufferToCard[ui_Lg],&puc_RES1[5],3);

  OS_Free(puc_RES1);
  OS_Free(InitVector);

  /* Longueur de la commande */
  pL_CamConf->xDialogue.iLenToCard =  pL_FichierSelectionne[kACTYS_READ_BINARY_LEN - 1] + kACTYS_READ_BINARY_LEN + 2;

  /* Appel de la commande kCAM_COMMAND_SEND */
  eL_CamStatus = CAM_SetCommand (pP_Handle, &pL_CamConf, kCAM_COMMAND_SEND) ;


  /* -------------Test: pas de problèmes lors de la commande ---------------*/
  if ((eL_CamStatus != kCAM_STATUS_OK)
   || (pL_CamConf->iState & kCAM_STATE_ERROR)
   || !(pL_CamConf->iState & kCAM_STATE_POWERED)
   || (pL_CamConf->iState & kCAM_STATE_REMOVED))
  {
    /* -----> Incident CAM ------------------------------------------------ */
    return(kACTYS_STATUS_ERREUR_CAM);
    /* <----- Incident CAM ------------------------------------------------ */
  }

  /* ---------------Aucun problème Statut-----------------------------------*/
  if ((pL_CamConf->cSW1 == kACTYS_WS1_OK        /* 90 */
    && pL_CamConf->cSW2 == kACTYS_WS2_OK)       /* 00 */
    || pL_CamConf->cSW1 == kACTYS_WS1_WARNING ) /* 61 */

  {
    return(kACTYS_STATUS_OK);
  }
  else
  {
    /* -------------- Erreur SW1 ---------------------------------------------*/
    return ( kACTYS_STATUS_ERREUR_SW1 ) ;
  }
}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_GetInfo --> Demande la realisation par la carte a
**                                          memoire de la commande GetInfo
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_CamConf    --> Pointeur sur la structure de dialogue CAM
**
** RETOUR  : kACTYS_STATUS_ERREUR_CAM        --> Incident CAM
**           kACTYS_STATUS_ERREUR_SW1        --> Commande CAM echouee
**           kACTYS_STATUS_OK                --> Commande realisee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_GetInfo(OS_HANDLE *pP_Handle,
                           CAM_CONF *pL_CamConf,
                           xACTYS_CARTE*  pL_Carte)
{
  CAM_STATUS    eL_CamStatus ;  /* Status de la commande CAM GetChallenge */

  /* Ordre GetChallenge */
  pL_CamConf->xDialogue.cOrdreType = kCAM_ORDRE_OUT ;
  /* Longueur max en reception */
  pL_CamConf->xDialogue.iLenFromCard = kCAM_LG_MAX_RECEPTION ;

  /* envoi de la commande */
  pL_CamConf->xDialogue.pBufferToCard[0] = kACTYS_CLA_80;	 /* 0x80 */
  pL_CamConf->xDialogue.pBufferToCard[1] = kACTYS_INS_GET_INFO ; /* 0xC0 */
  pL_CamConf->xDialogue.pBufferToCard[2] = kACTYS_P1_GET_INFO;   /* 0x02 */
  pL_CamConf->xDialogue.pBufferToCard[3] = kACTYS_P2_GET_INFO;   /* 0x05 */
  pL_CamConf->xDialogue.pBufferToCard[4] = kACTYS_Le_GET_INFO ;  /* 0x08 */

  /* Longueur de la commande */
  pL_CamConf->xDialogue.iLenToCard = 5 ;

  /* appel de la commande kCAM_COMMAND_SEND */
  eL_CamStatus = CAM_SetCommand (pP_Handle, &pL_CamConf, kCAM_COMMAND_SEND) ;

  /* -------------Test: pas de problèmes lors de la commande -----------------*/
  if ((eL_CamStatus != kCAM_STATUS_OK)
   || (pL_CamConf->iState & kCAM_STATE_ERROR)
   || !(pL_CamConf->iState & kCAM_STATE_POWERED)
   || (pL_CamConf->iState & kCAM_STATE_REMOVED))
  {
    /* -----> Incident CAM -------------------------------------------------- */
    return(kACTYS_STATUS_ERREUR_CAM);
    /* <----- Incident CAM -------------------------------------------------- */
  }

  /* ---------------Aucun problème Statut-------------------------------------*/
  if ((pL_CamConf->cSW1 == kACTYS_WS1_OK        /* 90 */
    && pL_CamConf->cSW2 == kACTYS_WS2_OK)       /* 00 */
    || pL_CamConf->cSW1 == kACTYS_WS1_WARNING ) /* 61 */

  {
    /* --------------Test: aucune donnée reçue -------------------------------*/
    if (pL_CamConf->xDialogue.iLenFromCard == 0)
    {
      return(kACTYS_STATUS_ERREUR_CAM);
    }
      pL_Carte->aucInfo[0] = pL_CamConf->xDialogue.iLenFromCard;
      /* Copie des données dans le pointeur retour */
      UTS_memcpy(&pL_Carte->aucInfo[1],
                 pL_CamConf->xDialogue.pBufferFromCard,
                 pL_Carte->aucInfo[0]);


    return(kACTYS_STATUS_OK);
  }
  /* -------------- Erreur SW1 -----------------------------------------------*/
  else
  {
    return ( kACTYS_STATUS_ERREUR_SW1 ) ;
  }

}


/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_SelectFileKey --> Demande la realisation par la carte à
**                                    mémoire de la commande SELECT FILE KEY
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_CamConf    --> Pointeur sur la structure de dialogue CAM
**           pL_FileName   --> Nom du fichier a selectionner dans la CAM
**           iL_LgFileName --> Longueur du nom du fichier à selectionner
**
** RETOUR  : kACTYS_STATUS_ERREUR_CAM        --> Incident CAM
**           kACTYS_STATUS_ERREUR_SW1        --> Commande CAM echouee
**           kACTYS_STATUS_OK                --> Commande reussie
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_SelectFileKey(OS_HANDLE*      pP_Handle,
                                 CAM_CONF*       pL_CamConf,
                                 unsigned char*  pL_Trnd,
                                 unsigned char** pL_DataOut)
{
  CAM_STATUS eL_CamStatus; /* --> Etat des traitements CAM                    */

  /* =====> CONSTRUCTION DE L'ORDRE CAM ===================================== */
  pL_CamConf->xDialogue.cOrdreType   = kCAM_ORDRE_IN;
  pL_CamConf->xDialogue.iLenFromCard = kCAM_LG_MAX_RECEPTION;

  /* Copie des octets fixes de la commande */
  pL_CamConf->xDialogue.pBufferToCard[0] = kACTYS_CLA_80;	  /* 0x80 */
  pL_CamConf->xDialogue.pBufferToCard[1] = kACTYS_INS_FILEKEY;    /* 0x28 */
  pL_CamConf->xDialogue.pBufferToCard[2] = kACTYS_P1_FILEKEY;     /* 0x00 */
  pL_CamConf->xDialogue.pBufferToCard[3] = kACTYS_P2_FILEKEY;	  /* 0x27 */
  pL_CamConf->xDialogue.pBufferToCard[4] = kACTYS_Lc_FILEKEY;	  /* 0x08 */


  /* Copie du Nom de fichier à sélectionner */
    UTS_memcpy(&pL_CamConf->xDialogue.pBufferToCard[5],
                pL_Trnd,
                kACTYS_FILEKEY_LEN);

  /* Copie de Le */
  //pL_CamConf->xDialogue.pBufferToCard[7 ] = kACTYS_Le_FILEKEY;		  /* 0x0C */

  /* Copie de la longueur total de la commande */
  pL_CamConf->xDialogue.iLenToCard = 6 + kACTYS_FILEKEY_LEN;


  /* <===== CONSTRUCTION DE L'ORDRE CAM ===================================== */

  /* =====> REALISATION DE LA COMMANDE CAM ================================== */


   eL_CamStatus = CAM_SetCommand(pP_Handle, &pL_CamConf, kCAM_COMMAND_SEND);

   if ((eL_CamStatus != kCAM_STATUS_OK)
    || (pL_CamConf->iState & kCAM_STATE_ERROR)
    || !(pL_CamConf->iState & kCAM_STATE_POWERED)
    || (pL_CamConf->iState & kCAM_STATE_REMOVED))
   {
     /* -----> Incident CAM ------------------------------------------------ */
     return(kACTYS_STATUS_ERREUR_CAM);
     /* <----- Incident CAM ------------------------------------------------ */
   }
   /* <===== REALISATION DE LA COMMANDE CAM ================================ */
   if ((pL_CamConf->cSW1 == kACTYS_WS1_OK        /* 90 */
     && pL_CamConf->cSW2 == kACTYS_WS2_OK)       /* 00 */
     || pL_CamConf->cSW1 == kACTYS_WS1_WARNING ) /* 61 */
   {

     /* Copie des données dans le pointeur retour */
     *pL_DataOut = OS_Malloc(pL_CamConf->xDialogue.iLenFromCard+1);
     *pL_DataOut[0] = pL_CamConf->xDialogue.iLenFromCard;
     UTS_memcpy(*pL_DataOut + 1,
                pL_CamConf->xDialogue.pBufferFromCard,
                pL_CamConf->xDialogue.iLenFromCard);

     /* -----> Commande SELECT réussie ------------------------------------- */
     return(kACTYS_STATUS_OK);
     /* <----- Commande SELECT réussie ------------------------------------- */
   }
     return(kACTYS_STATUS_ERREUR_SW1);

}


/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_Verify --> Demande la presentation du code PIN
**
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_CamConf    --> Pointeur sur la structure de dialogue CAM
**           uc_Scn        --> Numéro du code secret à sélectionner
**           puc_RES2      --> Pointeur sur la donnée RES2 issue du PVV
**
** RETOUR  : kACTYS_STATUS_ERREUR_CAM        --> Incident CAM
**           kACTYS_STATUS_ERREUR_SW1        --> Commande CAM echouee
**           kACTYS_STATUS_ERREUR_PRESENTATION_CODE_CONF --> Application bloquee
**           kACTYS_STATUS_OK                --> Commande realisee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_Verify(OS_HANDLE*     pP_Handle,
                          CAM_CONF*      pL_CamConf,
                          unsigned char  uc_Scn,
                          unsigned char* puc_RES2,
                          xACTYS_CARTE*  pL_Carte)
{
  CAM_STATUS     eL_CamStatus;    /* --> Etat des traitements locaux          */

  /* =====> CONSTRUCTION DE L'ORDRE CAM ===================================== */
  /* *****> Mise en place des octets de controle CLA, INS, P1, P2, Lc, ****** */
  (pL_CamConf->xDialogue).cOrdreType   = kCAM_ORDRE_IN;
  (pL_CamConf->xDialogue).iLenFromCard = kCAM_LG_MAX_RECEPTION;

  (pL_CamConf->xDialogue).pBufferToCard[0] = kACTYS_CLA_INTER_INDUSTRY_SECURED;
  (pL_CamConf->xDialogue).pBufferToCard[1] = kACTYS_INS_VERIFY;
  (pL_CamConf->xDialogue).pBufferToCard[2] = kACTYS_P1_VERIFY;

  /* Copie du Numéro du code secret à sélectionner */
  UTS_memcpy(&pL_CamConf->xDialogue.pBufferToCard[3],
             &uc_Scn,
              1);

  (pL_CamConf->xDialogue).pBufferToCard[4] = kACTYS_Lc_VERIFY;


  /* Copie du Nom de fichier à sélectionner */
  UTS_memcpy(&pL_CamConf->xDialogue.pBufferToCard[5],
              puc_RES2,
              kACTYS_Lc_VERIFY);


  (pL_CamConf->xDialogue).iLenToCard = 5 + kACTYS_Lc_VERIFY;
  /* <===== CONSTRUCTION DE L'ORDRE CAM ===================================== */


  /* =====> REALISATION DE LA COMMANDE CAM ================================== */
  eL_CamStatus = CAM_SetCommand(pP_Handle, &pL_CamConf, kCAM_COMMAND_SEND);
  if ((eL_CamStatus != kCAM_STATUS_OK)
   || ((pL_CamConf->iState & kCAM_STATE_ERROR) != 0)
   || ((pL_CamConf->iState & kCAM_STATE_POWERED) == 0)
   || ((pL_CamConf->iState & kCAM_STATE_REMOVED) != 0))
  {
    /* -----> Incident CAM -------------------------------------------------- */
    return(kACTYS_STATUS_ERREUR_CAM);
    /* <----- Incident CAM -------------------------------------------------- */
  }
  /* <===== REALISATION DE LA COMMANDE CAM ================================== */


  /* =====> ANALYSE DE L'ETAT DE LA REALISATION DE LA COMMANDE CAM ========== */
  if (((pL_CamConf->cSW1 == kACTYS_WS1_OK) &&
       (pL_CamConf->cSW2 == kACTYS_WS2_OK))
   || (pL_CamConf->cSW1 == kACTYS_WS1_WARNING))
  {
    /* +++++> Commande realisee +++++++++++++++++++++++++++++++++++++++++++++ */
    (pL_Carte->xToServer).ui_PinTryCounter = 3;
    return(kACTYS_STATUS_OK);
    /* <+++++ Commande realisee +++++++++++++++++++++++++++++++++++++++++++++ */
  }
  else if ((pL_CamConf->cSW1 == kACTYS_WS1_WRONG_PIN)
   &&      (((pL_CamConf->cSW2 & kACTYS_WS2_WRONG_PIN) & 0xF0) != 0))
  {
    /* -----> Code errone --------------------------------------------------- */
//    (pL_Carte->xToServer).ui_PinTryCounter
//    = ((pL_CamConf->cSW2 & kACTYS_WS2_WRONG_PIN) & 0x0F);
    return(kACTYS_STATUS_ERREUR_PRESENTATION_CODE_CONF);
    /* <----- Code errone --------------------------------------------------- */
  }
  else if ((pL_CamConf->cSW1 == kACTYS_WS1_PIN_BLOCKED)
   &&      (pL_CamConf->cSW2 == kACTYS_WS2_PIN_BLOCKED))
  {
    /* -----> Application bloquee ------------------------------------------- */
    (pL_Carte->xToServer).ui_PinTryCounter = 0;
    /* Pour ne pas afficher Code Faux */
    return(kACTYS_STATUS_NB_SAISIE_NULLE);
    /* <----- Application bloquee ------------------------------------------- */
  }

  return(kACTYS_STATUS_ERREUR_SW1);
  /* <===== ANALYSE DE L'ETAT DE LA REALISATION DE LA COMMANDE CAM ========== */
}
