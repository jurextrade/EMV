/*==============================================================================
================================================================================
=====                        MODULE ACTYS ACTYS.C                          =====
================================================================================
================================================================================
===== PROJET     : ACTYS                                                   =====
=====              ***** APPLICATION *****                                 =====
=====                                                                      =====
===== EDITEUR    : WYNID Technologies - 2003                               =====
================================================================================
===== EVOLUTIONS : | 1.00 | 01/10/2004 | Creation                   =====
=====----------------------------------------------------------------------=====
=====              | 1.01 | 04/01/2005 | Ajout controle parametrage du     =====
=====           |      |        | Fichier C2                =====
====-----------------------------------------------------------------------=====
=====              | 1.02 | 18/03/2005 | Ajout renvoi code chauff au       =====
=====           |      |        | serveur                    =====
================================================================================

================================================================================
===== LISTE DES POINTS D'ENTREE DU MODULE:                                 =====
===== ------------------------------------                                 =====
===== void ACTYS_Tache(void)                                               =====
==============================================================================*/


/*##############################################################################
##### INCLUDE                          #########################################
##############################################################################*/
#include "OS.h"
#include "sfr.h"
#include "API_CAM.h"
#include "API_FLASH.h"
#include "API_DISPLAY.h"
#include "utils.h"
#include "TLV.h"

#include "Total.h"
#include "pvv.h"
#include "des.h"
#include "DECRYPT_KEY.h"

//DEBUG: lecture piste
#include "API_CAP.h"
#include "XS0CONV.h"
#include "Key.h"

#include <string.h>
/*############################################################################*/


/*##############################################################################
####################### DEFINITIONS PRIVEES ####################################
##############################################################################*/
/*------------------------------------------------------------------------------
              INSCRIPTION EN MEMOIRE FLASH DE LA VERSION ACTYS
------------------------------------------------------------------------------*/
#pragma CONST_SEG LabelEdit
const char txt0[] = {"\x030\x000 (c) WYNID TECHNOLOGIES 2005 $"};
#if BICA
  const char txt1[] = {"\x0FF\x030\x001\x01C BICA   (V3.03) $"};
#else
  const char txt1[] = {"\x0FF\x030\x001\x01C TOTAL  (V3.01) $"};
#endif
#pragma CONST_SEG LabelDonnees
const unsigned char txt2[] = {0x31,
                              0,
                              2,
                              kACTYS_VER_EDITION,
                              kACTYS_VER_REVISION,
                              0x31, 0x01, 1, ' ',
                              0x70, 0x00, 3, 0, 0, 0,
                              0x70, 0x01, 2, 0x1F, 0xFF,
                              0x31, 0x10, 1, 2,
                              0x70, 0x10, 0x02, 0x01, 0x00, 0xFF, 0xFF,
                              0x70, 0x90, 0x02, 0x00, 0x00};

#pragma CONST_SEG Validation
const char txt3[] = {0xFF, 0x31, 0x12, 0x04, 0xFF, 0xFF, 0xFF, 0xFF};

#pragma CONST_SEG AppliCode
const char txt4[] = {"\x060\x001"};


#pragma CONST_SEG DEFAULT

// crypto test MKGR
const unsigned char cauc_CRYPTO_MKGR_TEST[] = "\x61\x6C\x45\x03\x78\xE4\x75\x5E";
// crypto pour carte reelle MKGR
const unsigned char cauc_CRYPTO_MKGR[]      = "\x58\xDC\x48\x9F\xD1\xCD\xDE\x30";

const unsigned char cauc_CRYPTO_MUTA_TEST[] = "\x01\x63\xEF\x9C\x10\xF4\xCA\xBE";
// crypto pour carte reelle MKGR
const unsigned char cauc_CRYPTO_MUTA[]      = "\xD6\xF2\x2C\xE1\x74\xFA\x66\x1A";

//DEBUG
#if DEBUG
/*
const unsigned char pL_BufDebug[146]={  0x00,0x90,

                          0x00,0x3c,
                          0x00,0x00,
                          0x00,0x10,0x00,0x06,0x55,0x96,0x20,0x04,0x12,0x14,
                          0x11,0x31,0x00,0x00,0x00,0x06,0x88,0x76,0x02,0x01,
                          0x20,0x04,0x12,0x14,0x00,0x00,0x00,0x00,0x01,0x04,
                          0xa4,0xd0,0x00,0x00,0x06,0x55,0x96,0x00,0x00,0x00,
                          0x68,0x87,0x60,0x00,0x00,0x65,0x59,0x60,0x00,0x00,
                          0x65,0x59,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                          0x00,0x04,
                          0x00,0x3c,
                          0x00,0x00,0x00,0x00,

                          0x00,0x3c,
                          0x00,0x40,
                          0x4d,0x41,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                          0x00,0x04,
                          0x00,0x7C,
                          0x00,0x00,0x00,0x00};




const unsigned char pL_BufDebug[146]={  0x00,0x90,

                          0x00,0x3C,
                          0x00,0x00,
                          0x00,0x20,0x37,0x03,0x53,0x11,0x20,0x05,0x01,0x05,
                          0x16,0x43,0x00,0x00,0x37,0x03,0x53,0x11,0x45,0x04,
                          0x20,0x05,0x01,0x05,0x00,0x00,0x00,0x00,0x02,0x04,
                          0xA4,0xD0,0x00,0x37,0x03,0x53,0x11,0x00,0x03,0x70,
                          0x35,0x31,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                          0x00,0x3C,
                          0x00,0x3C,
                          0x00,0x00,0x00,0x00,0x00,0x07,0x86,0x27,0x4D,0x41,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                          0x00,0x0C,
                          0x00,0x78,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
*/

const unsigned char pL_BufDebug[145]={  0x90,

                          0x00,0x3c,
                          0x00,0x00,
                          0x00,0x10,0x37,0x02,0x80,0x23,0x20,0x05,0x01,0x06,
                          0x18,0x06,0x00,0x00,0x37,0x02,0x80,0x23,0x45,0x04,
                          0x20,0x05,0x01,0x06,0x00,0x00,0x00,0x00,0x01,0x04,
                          0xa4,0xd0,0x00,0x37,0x02,0x80,0x23,0x00,0x03,0x70,
                          0x28,0x02,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                          0x00,0x3c,
                          0x00,0x3c,
                          0x00,0x00,0x00,0x00,0x00,0x05,0x06,0x27,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                          0x00,0x0c,
                          0x00,0x78,
                          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

#endif
/*##############################################################################
######################### FONCTIONS PRIVEES ####################################
##############################################################################*/

/******************************************************************************/
/*                         Fonctions   PME                                    */
/******************************************************************************/

/*******************************************************************************
********************************************************************************
** FONCTION: DeAllocateAnswerArea --> Libere une zone de stockage de reponse
**                                    correspondant a une etiquette donnee
**                                    et initialise le mapping memoire sur la
**                                    zone allouee
**
** APPEL   : spx_MemoryMapping --> Adresse du mapping memoire
**           lx_AreaTag        --> Etiquette a associer a la zone
**           cppx_AreaAddress  --> Adresse de stockage de l'adresse de zone
**
** RETOUR  :
********************************************************************************
*******************************************************************************/
void DeAllocateAnswerArea(MAPMEM*         spx_MemoryMapping,
                          unsigned long   lx_AreaTag,
                          unsigned char** cppx_AreaAddress)
{
  MAPMEM* pl_MemoryMapped;

  if ((spx_MemoryMapping == NULL) || (cppx_AreaAddress == NULL))
  {
    /* -----> Contexte incoherent ------------------------------------------- */
    return;
    /* <----- Contexte incoherent ------------------------------------------- */
  }

  OS_Free(*cppx_AreaAddress);
  *cppx_AreaAddress = NULL;

  pl_MemoryMapped = GetMapMem(spx_MemoryMapping, (lx_AreaTag * 0x100));
  if (pl_MemoryMapped == NULL)
  {
    /* -----> Contexte incoherent ------------------------------------------- */
    return;
    /* <----- Contexte incoherent ------------------------------------------- */
  }

  pl_MemoryMapped->pAd   = NULL;
}
/*************************   DeAllocateAnswerArea   ***************************/

/*******************************************************************************
********************************************************************************
** FONCTION: AllocateAnswerArea --> Alloue une zone de stockage de reponse
**                                  correspondant a une etiquette donnee
**                                  et etablit le mapping memoire sur la
**                                  zone allouee
**
** APPEL   : spx_MemoryMapping --> Adresse du mapping memoire
**           lx_AreaTag        --> Etiquette a associer a la zone
**           cx_AreaSize       --> Taille de la zone a allouer
**           cppx_AreaAddress  --> Adresse de stockage de l'adresse de zone
**
** RETOUR  : kMONEO_STATUS_OUT_OF_MEMORY --> Capacite memoire atteinte
**           kMONEO_STATUS_OK            --> Operation effectuee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS AllocateAnswerArea(MAPMEM*         spx_MemoryMapping,
                                unsigned long   lx_AreaTag,
                                unsigned char   cx_AreaSize,
                                unsigned char** cppx_AreaAddress)
{
  MAPMEM* pl_MemoryMapped;

  if ((spx_MemoryMapping == NULL) || (cppx_AreaAddress == NULL))
  {
    /* -----> Contexte incoherent ------------------------------------------- */
    return(kACTYS_STATUS_OUT_OF_MEMORY);
    /* <----- Contexte incoherent ------------------------------------------- */
  }

  *cppx_AreaAddress = OS_Malloc(cx_AreaSize + 1);
  if (*cppx_AreaAddress == NULL)
  {
    /* -----> Capacite memoire atteinte ------------------------------------- */
    return(kACTYS_STATUS_OUT_OF_MEMORY);
    /* <----- Capacite memoire atteinte ------------------------------------- */
  }

  pl_MemoryMapped = GetMapMem(spx_MemoryMapping, (lx_AreaTag * 0x100));
  if (pl_MemoryMapped == NULL)
  {
    /* -----> Contexte incoherent ------------------------------------------- */
    return(kACTYS_STATUS_OUT_OF_MEMORY);
    /* <----- Contexte incoherent ------------------------------------------- */
  }

  pl_MemoryMapped->pAd   = *cppx_AreaAddress;
  (*cppx_AreaAddress)[0] = cx_AreaSize;

  return(kACTYS_STATUS_OK);
}
/**************************   AllocateAnswerArea   ****************************/

/*******************************************************************************
********************************************************************************
** FONCTION: GetExternalAllocatedArea --> Obtient une zone de stockage reponse
**                                        correspondant a une etiquette donnee
**                                        dont le mapping memoire sur la
**                                        zone allouee a ete initialise par une
**                                        fonction externe
**
** APPEL   : spx_MemoryMapping --> Adresse du mapping memoire
**           lx_AreaTag        --> Etiquette a associer a la zone
**           cppx_AreaAddress  --> Adresse de stockage de l'adresse de zone
**
** RETOUR  : kMONEO_STATUS_OUT_OF_MEMORY --> Capacite memoire atteinte
**           kMONEO_STATUS_OK            --> Operation effectuee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS GetExternalAllocatedArea(MAPMEM       *   spx_MemoryMapping,
                                       unsigned long    lx_AreaTag,
                                       unsigned char ** cppx_AreaAddress)
{
  MAPMEM* pl_MemoryMapped;

  if ((spx_MemoryMapping == NULL) || (cppx_AreaAddress == NULL))
  {
    /* -----> Contexte incoherent ------------------------------------------- */
    return(kACTYS_STATUS_OUT_OF_MEMORY);
    /* <----- Contexte incoherent ------------------------------------------- */
  }

  pl_MemoryMapped = GetMapMem(spx_MemoryMapping, (lx_AreaTag * 0x100));
  if ((pl_MemoryMapped == NULL) || (pl_MemoryMapped->pAd == NULL))
  {
    /* -----> Contexte incoherent ------------------------------------------- */
    return(kACTYS_STATUS_OUT_OF_MEMORY);
    /* <----- Contexte incoherent ------------------------------------------- */
  }

  *cppx_AreaAddress = pl_MemoryMapped->pAd;
  return(kACTYS_STATUS_OK);
}
/***********************   GetExternalAllocatedArea   *************************/


/***************************************************************************
* Function Name  : ACTYS_AfficheMessage
*
* Description    :
*
* Side effects   :
*
* Comment        :
*
***************************************************************************/
ACTYS_STATUS ACTYS_AfficheMessage(   OS_HANDLE    *pP_Handle,
                                   unsigned char cL_Efface,
                                   unsigned char cL_NumeroDeLigne,
                                   unsigned int  iP_Langue,
                                   unsigned char cP_NumeroMessage)
{
HANDLE_FLASH  * pL_HandleFlash ;
TLV           * pL_ResultatTLVLangues ;
TLV           * pL_ResultatTLVMessage ;
unsigned char * pL_DonneesDeLaFLASH ;
unsigned long   lL_Langue ;
unsigned char   cL_FlashResult ;

   pL_HandleFlash = FLASH_OpenFlash( pP_Handle, &cL_FlashResult ) ;
   if ( cL_FlashResult != OS_NO_ERR )
   {
     return( kACTYS_STATUS_ERREUR_DONNEES_TELECHARGE ) ;
   }

   pL_DonneesDeLaFLASH = FLASH_SearchTLV (pP_Handle, pL_HandleFlash, kACTYS_TAG_APPLI_TOTAL ,kACTYS_TAG_ListeMessages , &cL_FlashResult);

   if ( cL_FlashResult != OS_NO_ERR || pL_DonneesDeLaFLASH == NULL )
   {
      OS_Free( pL_DonneesDeLaFLASH ) ;
      FLASH_Close( pL_HandleFlash ) ;
      return( kACTYS_STATUS_ERREUR_DONNEES_TELECHARGE ) ;
   }

   lL_Langue = kACTYS_TAG_LANGUE
                            + (((unsigned long)(iP_Langue & 0x0F00)) << 8)
                            + (((unsigned long)(iP_Langue & 0x00F0)) << 4)
                            + (iP_Langue & 0x000F) ;
   pL_ResultatTLVLangues = ChercheTag(pL_DonneesDeLaFLASH,
                                      pL_HandleFlash->iLongueurFichier,
                                      lL_Langue,
                                      NULL);
   if ( pL_ResultatTLVLangues == NULL )
   {
      OS_Free( pL_DonneesDeLaFLASH ) ;
      FLASH_Close( pL_HandleFlash ) ;
      return( kACTYS_STATUS_ERREUR_DONNEES_TELECHARGE ) ;
   }
   pL_ResultatTLVMessage = ChercheTag ( pL_ResultatTLVLangues->pData,
                                        pL_ResultatTLVLangues->iLongueur,
                                        kACTYS_TAG_MESSAGE_0 + cP_NumeroMessage,
                                        NULL ) ;
   if ( pL_ResultatTLVMessage == NULL )
   {
      TLV_LibereTLV(pL_ResultatTLVLangues, 'r');
      OS_Free( pL_DonneesDeLaFLASH ) ;
      FLASH_Close( pL_HandleFlash ) ;
      return( kACTYS_STATUS_ERREUR_DONNEES_TELECHARGE ) ;
   }

   ACTYS_Affiche(pP_Handle, cL_Efface, cL_NumeroDeLigne, pL_ResultatTLVMessage->pData, 16) ;

   TLV_LibereTLV(pL_ResultatTLVLangues, 'r');
   TLV_LibereTLV(pL_ResultatTLVMessage, 'r');
   OS_Free( pL_DonneesDeLaFLASH ) ;
   FLASH_Close(pL_HandleFlash) ;
   return ( kACTYS_STATUS_OK ) ;
}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_Saisie_Info   --> Permet la saisie de la Ref. Client
**                                            et du message client
**
** APPEL   : pP_Handle            --> Handle de la tache
**           uc_LgSaisieMin       --> Nb touches saisies min
**           uc_LgSaisieMax       --> Nb touches saisies max
**           puc_BufferInfoSaisie   --> buffer contenant les touches saisies
**                                  --> puc_BufferInfoSaisie[0] = Nb de touches saisies
**         pL_Carte              --> Info globale a l'appli
**
** RETOUR  : kACTYS_STATUS_OK
**           kACTYS_STATUS_TIME_OUT_SAISIE    --> Delai expire
**           kACTYS_STATUS_ABANDON            --> Saisie abandonnee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_Saisie_Info(OS_HANDLE      * pP_Handle,
                               unsigned char    uc_LgSaisieMin,
                               unsigned char    uc_LgSaisieMax,
                               unsigned char  * puc_BufferInfoSaisie,
                               unsigned char    uc_flag_puce,
                               xACTYS_CARTE   * pL_Carte)
{
unsigned char   cL_Buffer[5];
unsigned char   tL_Resultat[15];
unsigned char   tL_ParaSaisie[5] = {1,1,0,6,6};
unsigned char   cL_NoWarning;
unsigned int   ui_MasqueArrachage;
ACTYS_STATUS    e_Status;

  tL_ParaSaisie[3]=uc_LgSaisieMin;
  tL_ParaSaisie[4]=uc_LgSaisieMax;

  /* *****> Positionnement de l'affichage de la zone de saisie ************** */
  cL_Buffer[0] = ESC;
  cL_Buffer[1] = 'P';
  cL_Buffer[2] = 2;
  cL_Buffer[3] = 1;
  cL_Buffer[4] = FC;
  /* <***** Positionnement de l'affichage de la zone de saisie ************** */

  //ACTYS
  if(uc_flag_puce == 1)
    ui_MasqueArrachage = OS_PEI_CARTE_RETIRAIT;
  //PISTE
  else
    ui_MasqueArrachage = 0;

  /* *****> Saisie ********************************************************** */
  cL_NoWarning = API_DISPLAY_WinSaisie(  0,
                            NULL,
                            cL_Buffer,
                            0,
                            0,
                            tL_ParaSaisie,
                            kACTYS_TEMPO_SAISIE_1ERE_TOUCHE,
                            kACTYS_TEMPO_SAISIE,
                            ui_MasqueArrachage,
                            tL_Resultat,
                            pP_Handle);

    if (tL_Resultat[1] == 0xFF)
    {
      /* -----> Abandon suite a arrachage carte ------------------------------- */
    e_Status = ACTYS_AfficheMessage(pP_Handle,
                                     1,
                                     2,
                                     pL_Carte->xFromServer.iCodeLangue,
                                     kACTYS_MESS_CARTE_ARRACHEE);

      TEMPO_XMS(pP_Handle, 1500);

      if( e_Status == kACTYS_STATUS_OK )
        return(kACTYS_STATUS_CARTE_ARRACHEE);
      else
        return(e_Status);
      /* <----- Abandon suite a arrachage carte ------------------------------- */
    }

  if ((tL_Resultat[1] == 'A') || (tL_Resultat[0] == 0) || (tL_Resultat[1] == 1) || (tL_Resultat[1] == 2))
  {
    /* -----> Abandon de la saisie ------------------------------------------ */
    e_Status = ACTYS_AfficheMessage(pP_Handle,
                                     1,
                                     2,
                                     pL_Carte->xFromServer.iCodeLangue,
                                     kACTYS_MESS_ABANDON);

      TEMPO_XMS(pP_Handle, 1500);

      if( e_Status == kACTYS_STATUS_OK )
      {
        if ( (tL_Resultat[1] == 1) || (tL_Resultat[1] == 2) )
      {     
        /* -----> Expiration du delai avant saisie de la premiere touche ------ */
        return(kACTYS_STATUS_TIME_OUT_SAISIE);
        /* <----- Expiration du delai avant saisie de la premiere touche ------ */
      }
      else
        return(kACTYS_STATUS_ABANDON);
      /* <----- Abandon de la saisie ------------------------------------------ */
    }
    else
      return(e_Status);
  }
  else
  {
    puc_BufferInfoSaisie[0] = tL_Resultat[0]-1;

    UTS_memcpy(&puc_BufferInfoSaisie[1],&tL_Resultat[2],tL_Resultat[0]-1);
    return(kACTYS_STATUS_OK);
  }
}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_Saisie_Ref_Client   --> Permet la saisie de la Ref. Client
**                                         et du message client
**
** APPEL   : pP_Handle        --> Handle de la tache
**           pL_Carte         --> Info globale a l'appli
**
** RETOUR  :   kACTYS_STATUS_OK
**             kACTYS_STATUS_TIME_OUT_SAISIE    --> Delai expire
**             kACTYS_STATUS_ABANDON            --> Saisie abandonnee
**          kACTYS_STATUS_ERROR_REF_CLIENT   --> erreur reference client
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_Saisie_Ref_Client(unsigned char    uc_flag_puce,
                         OS_HANDLE      * pP_Handle,
                                     xACTYS_CARTE   * pL_Carte)
{
ACTYS_STATUS    e_Status;
unsigned char   uc_NbEssai=0;
unsigned char * puc_Buffer;
unsigned char   uc_TypeMessage;
ACTYS_STATUS    e_Status2;

  memset(&pL_Carte->xToServer.aucRefClient[1],0,LG_REF_CLIENT);

  puc_Buffer = OS_Malloc(LG_REF_CLIENT+1);
  if( puc_Buffer == NULL)
     return(kACTYS_STATUS_OUT_OF_MEMORY);

  /* *****> Affichage Info Ref. Client: ************** */
  do
  {
    uc_NbEssai++;

    e_Status2 = ACTYS_AfficheMessage(pP_Handle,
                                     1,
                                     1,
                                     pL_Carte->xFromServer.iCodeLangue,
                                     kACTYS_MESS_REF_CLIENT);

    if( e_Status2 != kACTYS_STATUS_OK )
    {
      OS_Free(puc_Buffer);
      return(e_Status2);
    }

      if(uc_NbEssai == 1)
        uc_TypeMessage = kACTYS_MESS_PREMIER_ESSAI;
      else if(uc_NbEssai == 2)
        uc_TypeMessage = kACTYS_MESS_DEUXIEME_ESSAI;
      else
        uc_TypeMessage = kACTYS_MESS_DERNIER_ESSAI;

    e_Status2 = ACTYS_AfficheMessage(pP_Handle,
                                     0,
                                     2,
                                     pL_Carte->xFromServer.iCodeLangue,
                                     uc_TypeMessage);
    if( e_Status2 != kACTYS_STATUS_OK )
    {
      OS_Free(puc_Buffer);
      return(e_Status2);
    }
    /* <***** Affichage Info Ref. Client: ************** */

    /* *****> Saisie *********************************** */
    e_Status = ACTYS_Saisie_Info(  pP_Handle,
                        LG_REF_CLIENT,
                        LG_REF_CLIENT,
                        puc_Buffer,
                        uc_flag_puce,
                        pL_Carte);
    /* <***** Saisie *********************************** */

     if( e_Status != kACTYS_STATUS_OK)
     {
      OS_Free(puc_Buffer);
      return(e_Status);
      }
    e_Status = bLuhnControl (&puc_Buffer[1], LG_REF_CLIENT);

    if( e_Status == kACTYS_STATUS_ERROR_REF_CLIENT )
    {
      e_Status2 = ACTYS_AfficheMessage(pP_Handle,
                                       1,
                                       1,
                                       pL_Carte->xFromServer.iCodeLangue,
                                       kACTYS_MESS_REF_CLIENT2);

       if( e_Status2 != kACTYS_STATUS_OK)
       {
        OS_Free(puc_Buffer);
        return(e_Status2);
        }

      e_Status2 = ACTYS_AfficheMessage(pP_Handle,
                                       0,
                                       2,
                                       pL_Carte->xFromServer.iCodeLangue,
                                       kACTYS_MESS_REF_FAUSSE);

       if( e_Status2 != kACTYS_STATUS_OK)
       {
        OS_Free(puc_Buffer);
        return(e_Status2);
        }

        /* Attente de 1,5 seconde pour voir l'affichage du code faux */
        TEMPO_XMS(pP_Handle, 1500);
    }
  }
  while( (uc_NbEssai<3) && (e_Status != kACTYS_STATUS_OK));
  /* <***** Affichage Info Ref. Client: ************** */


  if(e_Status == kACTYS_STATUS_OK)
    UTS_memcpy(&pL_Carte->xToServer.aucRefClient[1],&puc_Buffer[1],LG_REF_CLIENT);

  OS_Free(puc_Buffer);
  return(e_Status);


}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_Build   --> Permet de vérifier si la carte
**                             possède l'application Actys
**                             ou non et de la selectionner
**                             si elle est présente
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_CamConf    --> Pointeur sur la structure de dialogue CAM
**
** RETOUR  : kACTYS_STATUS_ERREUR_CAM        --> Incident CAM
**           kACTYS_STATUS_ERREUR_SW1        --> Commande CAM echouee
**           kACTYS_STATUS_OK                --> Commande reussie
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_Build(OS_HANDLE*     pP_Handle,
                         CAM_CONF*      pL_CamConf)
{
  /* status d'erreur */
  ACTYS_STATUS  eL_ActysStatus;
  unsigned char aucL_Tampon[7] = kACTYS_AID_ACTYS ; /* identifiant AID ACTYS*/

  /* Sélection de l'AID de paiement ACTYS */
  eL_ActysStatus =  ACTYS_SelectFile(pP_Handle,
                                     pL_CamConf,
                            kSelectAID,
                            7,
                                     aucL_Tampon);

   return (eL_ActysStatus);
}

/******************************************************************************/
/*                          Fonctions utiles diverses                         */
/******************************************************************************/

/*******************************************************************************
********************************************************************************
** FONCTION: AVIA_CheckPINSaisie --> Permet de saisir les codes pin + controle PVV
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  : kACTYS_STATUS_OK
**           kACTYS_STATUS_TIME_OUT_SAISIE    --> Delai expire
**           kACTYS_STATUS_ABANDON            --> Saisie abandonnee
**           kACTYS_STATUS_OUT_OF_MEMORY      --> Capacite memoire depassee
**           kACTYS_STATUS_ERROR_PVV          --> Code errone
********************************************************************************
*******************************************************************************/
ACTYS_STATUS AVIA_CheckPINSaisie (OS_HANDLE    * pP_Handle,
                                 xACTYS_CARTE * pL_Carte)
{
unsigned char * puc_PIN;
ACTYS_STATUS    eL_ActysStatus=kACTYS_STATUS_OK; /* --> Etat final de realisation                     */

    /* =====> REALISATION DE LA SAISIE DES CODES ========================== */
    puc_PIN = (unsigned char*) OS_Malloc(4);
    if (puc_PIN == NULL)
      return(kACTYS_STATUS_OUT_OF_MEMORY);

    // Saisie du code Pin
    eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, puc_PIN,'P',NULL);

    /* <===== REALISATION DE LA SAISIE DES CODES ============================ */

    /* =====> REALISATION DE L'ALGO PVV ===================================== */
    if( eL_ActysStatus == kACTYS_STATUS_OK )
      eL_ActysStatus = PVV_CheckPVV_AVIA(&pL_Carte->puc_DataFromServer[1],
                                           puc_PIN);
    /* <===== REALISATION DE L'ALGO PVV ===================================== */

    ACTYS_Free(&puc_PIN);


    return(eL_ActysStatus);
}

/*******************************************************************************
********************************************************************************
** FONCTION: UTA_CheckPINSaisie --> Permet de saisir les codes pin + controle PVV
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  : kACTYS_STATUS_OK
**           kACTYS_STATUS_TIME_OUT_SAISIE    --> Delai expire
**           kACTYS_STATUS_ABANDON            --> Saisie abandonnee
**           kACTYS_STATUS_OUT_OF_MEMORY      --> Capacite memoire depassee
**           kACTYS_STATUS_ERROR_PVV          --> Code errone
********************************************************************************
*******************************************************************************/
ACTYS_STATUS UTA_CheckPINSaisie (OS_HANDLE    * pP_Handle,
                                 xACTYS_CARTE * pL_Carte)
{
unsigned char * puc_PIN;
ACTYS_STATUS    eL_ActysStatus; /* --> Etat final de realisation                     */

    /* =====> REALISATION DE LA SAISIE DES CODES ========================== */
    puc_PIN = (unsigned char*) OS_Malloc(4);
    if (puc_PIN == NULL)
       return(kACTYS_STATUS_OUT_OF_MEMORY);

    // Saisie du code Pin
    eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, puc_PIN,'P',NULL);

    /* <===== REALISATION DE LA SAISIE DES CODES ============================ */
    if( eL_ActysStatus == kACTYS_STATUS_OK )
    /* =====> REALISATION DE L'ALGO PVV ===================================== */
      eL_ActysStatus = PVV_CheckPVV_UTA(&pL_Carte->puc_DataFromServer[1],
                                       puc_PIN,
                                       pL_Carte->xFromServer.uc_FlagTest);

    ACTYS_Free(&puc_PIN);

    return(eL_ActysStatus);
}

/*******************************************************************************
********************************************************************************
** FONCTION: ARIS_CheckPINSaisie --> Permet de saisir les codes pin + controle PVV
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  : kACTYS_STATUS_OK
**           kACTYS_STATUS_TIME_OUT_SAISIE    --> Delai expire
**           kACTYS_STATUS_ABANDON            --> Saisie abandonnee
**           kACTYS_STATUS_OUT_OF_MEMORY      --> Capacite memoire depassee
**           kACTYS_STATUS_ERROR_PVV          --> Code errone
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ARIS_CheckPINSaisie (OS_HANDLE    * pP_Handle,
                                 xACTYS_CARTE * pL_Carte)
{
unsigned char * puc_PIN;
ACTYS_STATUS    eL_ActysStatus; /* --> Etat final de realisation                     */

    /* =====> REALISATION DE LA SAISIE DES CODES ========================== */
    puc_PIN = (unsigned char*) OS_Malloc(4);
    if (puc_PIN == NULL)
       return(kACTYS_STATUS_OUT_OF_MEMORY);

    // Saisie du code Pin
    eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, puc_PIN,'P',NULL);

    /* <===== REALISATION DE LA SAISIE DES CODES ============================ */
    if( eL_ActysStatus == kACTYS_STATUS_OK )
    /* =====> REALISATION DE L'ALGO PVV ===================================== */
      eL_ActysStatus = PVV_CheckPVV_ARIS(&pL_Carte->puc_DataFromServer[1],
                                       puc_PIN);

    ACTYS_Free(&puc_PIN);

    return(eL_ActysStatus);
}

/*******************************************************************************
********************************************************************************
** FONCTION: BICA_CheckPINSaisie --> Permet de saisir les codes pin + controle PVV
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  : kACTYS_STATUS_OK
**           kACTYS_STATUS_TIME_OUT_SAISIE    --> Delai expire
**           kACTYS_STATUS_ABANDON            --> Saisie abandonnee
**           kACTYS_STATUS_OUT_OF_MEMORY      --> Capacite memoire depassee
**           kACTYS_STATUS_ERROR_PVV          --> Code errone
********************************************************************************
*******************************************************************************/
ACTYS_STATUS BICA_CheckPINSaisie (OS_HANDLE    * pP_Handle,
                  xACTYS_CARTE * pL_Carte)
{
  unsigned char * puc_PIN;
  //unsigned char * puc_PINWith0;
  //int i;
  //int nbDigitInPIN;
  ACTYS_STATUS    eL_ActysStatus; /* --> Etat final de realisation                     */
  
    /* =====> REALISATION DE LA SAISIE DES CODES ========================== */
    puc_PIN = (unsigned char*) OS_Malloc(kBICA_PIN_LEN);
    if (puc_PIN == NULL)
    return(kACTYS_STATUS_OUT_OF_MEMORY);
  
    /*
    puc_PINWith0 = (unsigned char*) OS_Malloc(kBICA_PIN_LEN);
    if (puc_PINWith0 == NULL)
    return(kACTYS_STATUS_OUT_OF_MEMORY);
  */
    // Saisie du code Pin
    eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, puc_PIN,'B',NULL);
  
    /* <===== REALISATION DE LA SAISIE DES CODES ============================ */
    if( eL_ActysStatus == kACTYS_STATUS_OK ) {
    
    /* =====> REMPLISSAGE de 0  ============================================= */
    /*    memset(puc_PINWith0,'0',kBICA_PIN_LEN);
    nbDigitInPIN=0;
    for(i=0;i<kBICA_PIN_LEN;i++) {
    if(puc_PIN[i]!=kACTYS_PADDING_AFTER_PIN) {
        nbDigitInPIN++;
    }
    }
    for(i=0;i<nbDigitInPIN;i++) {
    puc_PINWith0[kBICA_PIN_LEN-nbDigitInPIN+i]=puc_PIN[i];
    }
    */
    
    /* =====> REALISATION DE L'ALGO PVV ===================================== */
    eL_ActysStatus = PVV_CheckPVV_BICA(pP_Handle,
      &pL_Carte->puc_DataFromServer[1],
      puc_PIN);
    }
  
    ACTYS_Free(&puc_PIN);
  
    return(eL_ActysStatus);
}


/*******************************************************************************
********************************************************************************
** FONCTION: LOMO_CheckPINSaisie --> Permet de saisir les codes pin + controle PVV
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  : kACTYS_STATUS_OK
**           kACTYS_STATUS_TIME_OUT_SAISIE    --> Delai expire
**           kACTYS_STATUS_ABANDON            --> Saisie abandonnee
**           kACTYS_STATUS_OUT_OF_MEMORY      --> Capacite memoire depassee
**           kACTYS_STATUS_ERROR_PVV          --> Code errone
********************************************************************************
*******************************************************************************/
ACTYS_STATUS LOMO_CheckPINSaisie (OS_HANDLE    * pP_Handle,
                  xACTYS_CARTE * pL_Carte)
{
  unsigned char * puc_PIN;
  //unsigned char * puc_PINWith0;
  //int i;
  //int nbDigitInPIN;
  ACTYS_STATUS    eL_ActysStatus; /* --> Etat final de realisation                     */
  
    /* =====> REALISATION DE LA SAISIE DES CODES ========================== */
    puc_PIN = (unsigned char*) OS_Malloc(kBICA_PIN_LEN);
    if (puc_PIN == NULL)
    return(kACTYS_STATUS_OUT_OF_MEMORY);
  
    /*
    puc_PINWith0 = (unsigned char*) OS_Malloc(kBICA_PIN_LEN);
    if (puc_PINWith0 == NULL)
    return(kACTYS_STATUS_OUT_OF_MEMORY);
  */
    // Saisie du code Pin
    eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, puc_PIN,'B',NULL);
  
    /* <===== REALISATION DE LA SAISIE DES CODES ============================ */
    if( eL_ActysStatus == kACTYS_STATUS_OK ) {
    
    /* =====> REMPLISSAGE de 0  ============================================= */
    /*    memset(puc_PINWith0,'0',kBICA_PIN_LEN);
    nbDigitInPIN=0;
    for(i=0;i<kBICA_PIN_LEN;i++) {
    if(puc_PIN[i]!=kACTYS_PADDING_AFTER_PIN) {
        nbDigitInPIN++;
    }
    }
    for(i=0;i<nbDigitInPIN;i++) {
    puc_PINWith0[kBICA_PIN_LEN-nbDigitInPIN+i]=puc_PIN[i];
    }
    */
    
    /* =====> REALISATION DE L'ALGO PVV ===================================== */
    eL_ActysStatus = PVV_CheckPVV_LOMO(&pL_Carte->puc_DataFromServer[1],puc_PIN);
    }
  
    ACTYS_Free(&puc_PIN);
  
    return(eL_ActysStatus);
}



/*******************************************************************************
********************************************************************************
** FONCTION: CSC_CheckPINSaisie --> Permet de saisir les codes pin + controle PVV
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  : kACTYS_STATUS_OK
**           kACTYS_STATUS_TIME_OUT_SAISIE    --> Delai expire
**           kACTYS_STATUS_ABANDON            --> Saisie abandonnee
**           kACTYS_STATUS_OUT_OF_MEMORY      --> Capacite memoire depassee
**           kACTYS_STATUS_ERROR_PVV          --> Code errone
********************************************************************************
*******************************************************************************/
ACTYS_STATUS CSC_CheckPINSaisie (OS_HANDLE    * pP_Handle,
                  xACTYS_CARTE * pL_Carte)
{
  unsigned char * puc_PIN;
  //unsigned char * puc_PINWith0;
  //int i;
  //int nbDigitInPIN;
  ACTYS_STATUS    eL_ActysStatus; /* --> Etat final de realisation                     */
  
    /* =====> REALISATION DE LA SAISIE DES CODES ========================== */
    puc_PIN = (unsigned char*) OS_Malloc(kBICA_PIN_LEN);
    if (puc_PIN == NULL)
    return(kACTYS_STATUS_OUT_OF_MEMORY);
  
    /*
    puc_PINWith0 = (unsigned char*) OS_Malloc(kBICA_PIN_LEN);
    if (puc_PINWith0 == NULL)
    return(kACTYS_STATUS_OUT_OF_MEMORY);
  */
    // Saisie du code Pin
    eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, puc_PIN,'B',NULL);
  
    /* <===== REALISATION DE LA SAISIE DES CODES ============================ */
    if( eL_ActysStatus == kACTYS_STATUS_OK ) {
    
    /* =====> REMPLISSAGE de 0  ============================================= */
    /*    memset(puc_PINWith0,'0',kBICA_PIN_LEN);
    nbDigitInPIN=0;
    for(i=0;i<kBICA_PIN_LEN;i++) {
    if(puc_PIN[i]!=kACTYS_PADDING_AFTER_PIN) {
        nbDigitInPIN++;
    }
    }
    for(i=0;i<nbDigitInPIN;i++) {
    puc_PINWith0[kBICA_PIN_LEN-nbDigitInPIN+i]=puc_PIN[i];
    }
    */
    
    /* =====> REALISATION DE L'ALGO PVV ===================================== */
    eL_ActysStatus = PVV_CheckPVV_CSC(&pL_Carte->puc_DataFromServer[1], puc_PIN);
    }
  
    ACTYS_Free(&puc_PIN);
  
    return(eL_ActysStatus);
}

/*******************************************************************************
********************************************************************************
** FONCTION: DKV_CheckPINSaisie --> Permet de saisir les codes pin + controle PVV
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  : kACTYS_STATUS_OK
**           kACTYS_STATUS_TIME_OUT_SAISIE    --> Delai expire
**           kACTYS_STATUS_ABANDON            --> Saisie abandonnee
**           kACTYS_STATUS_CARTE_ARRACHEE     --> Carte arrachee
**           kACTYS_STATUS_OUT_OF_MEMORY      --> Capacite memoire depassee
**           kACTYS_STATUS_ERROR_PVV          --> Code errone
********************************************************************************
*******************************************************************************/
ACTYS_STATUS DKV_CheckPINSaisie (OS_HANDLE    * pP_Handle,
                                 xACTYS_CARTE * pL_Carte)
{
unsigned char  auc_PIN[4];
unsigned char  auc_PIN_DRIVER[4];
ACTYS_STATUS   eL_ActysStatus; /* --> Etat final de realisation                     */

    /* =====> REALISATION DE LA SAISIE DES CODES ========================== */


    // Saisie du code Pin
    eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, auc_PIN,'P',NULL);

    if(pL_Carte->xFromServer.ui_TrackCardType == DKV_CLASSIC_CARD)
    {
      // Code chauffeur en dur, pas de saisie
      memset(auc_PIN_DRIVER,'0',kACTYS_PIN_LEN);
    }
    else if (pL_Carte->xFromServer.ui_TrackCardType == DKV_CLASSIC_CARD_AND_DRIVER_CODE)
    {
       // Saisie du code Chauffeur
       eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, auc_PIN_DRIVER,'C',NULL);

     // Transfert code chauff au serveur
     if(eL_ActysStatus == kACTYS_STATUS_OK)
       UTS_memcpy(&pL_Carte->xToServer.aucCodeChauf[1],auc_PIN_DRIVER,4);
    }
    /* <===== REALISATION DE LA SAISIE DES CODES ============================ */

    /* =====> REALISATION DE L'ALGO PVV ===================================== */
    if( eL_ActysStatus == kACTYS_STATUS_OK )
       eL_ActysStatus = PVV_CheckPVV_DKV(&pL_Carte->puc_DataFromServer[1],
                                         auc_PIN,
                                         auc_PIN_DRIVER,
                                         (unsigned char)pL_Carte->xFromServer.ui_TrackCardType);

    return(eL_ActysStatus);
}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_CheckPINSaisie --> Permet de saisir les codes pin + controle PVV
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  :   kACTYS_STATUS_OK
**             kACTYS_STATUS_OUT_OF_MEMORY                 --> Capacite memoire depassee
**             kACTYS_STATUS_TIME_OUT                      --> Delai expire
**             kACTYS_STATUS_ABANDON                    --> Saisie abandonnee
**             kACTYS_STATUS_CARTE_ARRACHEE                --> Carte arrachee
**             kACTYS_STATUS_ERROR_CARTE_BLOQUEE           --> Application bloquee
**          kACTYS_STATUS_ERROR_ISO2                    --> Erreur Piste ISO2
**             kACTYS_STATUS_ERROR_PVV                   --> Code errone
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_CheckPINSaisie (OS_HANDLE     * pP_Handle,
                                   xACTYS_CARTE  * pL_Carte,
                                   unsigned char * puc_RES2,
                                   CAM_CONF      **ppL_CamConf)
{
unsigned char* puc_PIN;
unsigned char* puc_PIN_DRIVER;
ACTYS_STATUS   eL_ActysStatus; /* --> Etat final de realisation */

    /* =====> REALISATION DE LA SAISIE DES CODES ============================ */

    puc_PIN = (unsigned char*) OS_Malloc(4);
    if (puc_PIN == NULL)
     {
       return(kACTYS_STATUS_OUT_OF_MEMORY);
     }

    puc_PIN_DRIVER = (unsigned char*) OS_Malloc(4);
     if (puc_PIN_DRIVER == NULL)
     {
       ACTYS_Free(&puc_PIN);
       return(kACTYS_STATUS_OUT_OF_MEMORY);
     }

    eL_ActysStatus = ACTYS_SaisieCodes (pP_Handle, pL_Carte, puc_PIN, puc_PIN_DRIVER,ppL_CamConf);

    if (eL_ActysStatus != kACTYS_STATUS_OK)
    {
      /* -----> Saisie abandonnee ------------------------------------------- */
      ACTYS_Free(&puc_PIN);
      ACTYS_Free(&puc_PIN_DRIVER);
      return(eL_ActysStatus);
    /* <----- Saisie abandonnee ------------------------------------------- */
    }
    /* <===== REALISATION DE LA SAISIE DES CODES ============================ */
  
    /* =====> REALISATION DE L'ALGO PVV ===================================== */
    if(pL_Carte->xFromServer.uc_FlagTest == 0) {
      eL_ActysStatus = PVV_CheckPVV_GR_EURO(pL_Carte->aucISO2,puc_PIN, puc_PIN_DRIVER, puc_RES2);
    }
    else {
      eL_ActysStatus = PVV_CheckPVV_GR_EURO_Test(pL_Carte->aucISO2,puc_PIN, puc_PIN_DRIVER, puc_RES2);
    }

    if (eL_ActysStatus != kACTYS_STATUS_OK)
    {
    ACTYS_Free(&puc_PIN);
      ACTYS_Free(&puc_PIN_DRIVER);
      return(eL_ActysStatus);
    }

    ACTYS_Free(&puc_PIN);
    ACTYS_Free(&puc_PIN_DRIVER);


    /* =====> REALISATION DE L'ALGO PVV ===================================== */

    return(eL_ActysStatus);
}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_VerifyPin --> Permet d'envoyer au PME la commande
**                                         de controle du code pin saisi
**
** APPEL   : pP_Handle     --> Handle de la tache MONEO
**           pL_CamConf    --> Pointeur sur la structure de dialogue CAM
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  : kACTYS_STATUS_OK
**           kACTYS_STATUS_TIME_OUT_SAISIE     --> Delai expire
**           kACTYS_STATUS_ABANDON             --> Saisie abandonnee
**           kACTYS_STATUS_CARTE_ARRACHEE      --> Carte arrachee
**           kACTYS_STATUS_OUT_OF_MEMORY       --> Capacite memoire depassee
**           kACTYS_STATUS_ERROR_CARTE_BLOQUEE --> Code errone
**           kACTYS_STATUS_ERROR_ISO2        --> Piste ISO2 erronee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_VerifyPin (OS_HANDLE*     pP_Handle,
                              CAM_CONF**     ppL_CamConf,
                              xACTYS_CARTE*  pL_Carte)

{
unsigned char* pL_NoWarning;   /* --> Suppression des warnings                      */
unsigned char* puc_RES2;
unsigned char* puc_SCV;
ACTYS_STATUS   eL_ActysStatus; /* --> Etat final de realisation                     */
ACTYS_STATUS   e_Status;
#if DEBUG
int i;
#endif
  // mise a zero du champ Reserve pour ACTYS
  pL_NoWarning = memset(&pL_Carte->aucISO2[33], '0', 3);

  puc_RES2 = (unsigned char*) OS_Malloc(8);
  if (puc_RES2 == NULL)
     return(kACTYS_STATUS_OUT_OF_MEMORY);

  do
  {
     /* =====> PRESENTATION DE(S) PIN(S)====================================== */
     eL_ActysStatus = ACTYS_CheckPINSaisie (pP_Handle,
                                            pL_Carte,
                                            puc_RES2,
                                            ppL_CamConf);
#if DEBUG
//DEBUG : trace resultat external authent
for(i=0;i<3;i++)
   API_UTILS_SendOnUART0(0xb0);
API_UTILS_SendOnUART0(eL_ActysStatus);
#endif


     if(   ( eL_ActysStatus == kACTYS_STATUS_TIME_OUT_SAISIE )
        || ( eL_ActysStatus == kACTYS_STATUS_ABANDON )
        || ( eL_ActysStatus == kACTYS_STATUS_CARTE_ARRACHEE )
        || ( eL_ActysStatus == kACTYS_STATUS_OUT_OF_MEMORY ) )
     {
        ACTYS_Free(&puc_RES2);
        return(eL_ActysStatus);
     }
     /* <===== PRESENTATION DE(S) PIN(S)====================================== */

     /* =====> PRESENTATION DE RES2 ========================================== */
     if(pL_Carte->uc_TypePIN != '9')
     {
        /* *****> Presentation du code a la carte ******************************* */
        puc_SCV = OS_Malloc(8);

       CHIF_DES8 (puc_RES2,
                   puc_SCV,
                   pL_Carte->auc_TKgr,
                   DES_DECRYPT);

        eL_ActysStatus = ACTYS_Verify(pP_Handle, *ppL_CamConf,0x01, puc_SCV, pL_Carte);

      OS_Free(puc_SCV);
        /* <***** Presentation du code a la carte ******************************* */

        if (eL_ActysStatus == kACTYS_STATUS_OK)
        {
        e_Status = ACTYS_AfficheMessage(pP_Handle,
                                           1,
                                         2,
                                         pL_Carte->xFromServer.iCodeLangue,
                                         kACTYS_MESS_CODE_BON);

          TEMPO_XMS(pP_Handle, 1500);

         if( e_Status != kACTYS_STATUS_OK)
         {
           ACTYS_Free(&puc_RES2);
          return(e_Status);
          }
        }
        else if(eL_ActysStatus == kACTYS_STATUS_ERREUR_PRESENTATION_CODE_CONF)
        {
            /* -----> Code errone ------------------------------------------------- */
        e_Status = ACTYS_AfficheMessage(pP_Handle,
                                         1,
                                        2,
                                        pL_Carte->xFromServer.iCodeLangue,
                                        kACTYS_MESS_CODE_FAUX);

          TEMPO_XMS(pP_Handle, 1500);

         if( e_Status != kACTYS_STATUS_OK)
         {
           ACTYS_Free(&puc_RES2);
           return(e_Status);
          }

           pL_Carte->xToServer.ui_PinTryCounter--;

           if( pL_Carte->xToServer.ui_PinTryCounter == 0)
           {
            e_Status = ACTYS_AfficheMessage(pP_Handle,
                                           1,
                                          2,
                                          pL_Carte->xFromServer.iCodeLangue,
                                          kACTYS_MESS_APPLI_BLOQUEE);

            TEMPO_XMS(pP_Handle, 1500);

             if( e_Status != kACTYS_STATUS_OK)
             {
             ACTYS_Free(&puc_RES2);
             return(e_Status);
              }

               return(kACTYS_STATUS_ERROR_CARTE_BLOQUEE);
           }
            /* <----- Code errone ------------------------------------------------- */
        }
        else if(eL_ActysStatus == kACTYS_STATUS_NB_SAISIE_NULLE)
        {
            /* -----> Carte bloquee ----------------------------------------------- */
        e_Status = ACTYS_AfficheMessage(pP_Handle,
                                         1,
                                        2,
                                        pL_Carte->xFromServer.iCodeLangue,
                                        kACTYS_MESS_APPLI_BLOQUEE);

          TEMPO_XMS(pP_Handle, 1500);

         if( e_Status != kACTYS_STATUS_OK)
         {
           ACTYS_Free(&puc_RES2);
           return(e_Status);
          }

           return(kACTYS_STATUS_ERROR_CARTE_BLOQUEE);
            /* <----- Carte bloquee ----------------------------------------------- */
        }
        else
        {
            /* -----> Erreur carte  ----------------------------------------------- */
           ACTYS_Free(&puc_RES2);
           return(eL_ActysStatus);
           /* <----- Erreur carte  ----------------------------------------------- */
        }
     }
     /* <===== PRESENTATION DU CODE ========================================== */
  }
  while(eL_ActysStatus != kACTYS_STATUS_OK);

  ACTYS_Free(&puc_RES2);

  return(eL_ActysStatus);
}
/*******************************   ACTYS_Verify   *****************************/

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_Write_C3   --> Permet d'écrire dans le fichier C3 et de gérer
**                                son fichier miroir C3bis.
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_CamConf    --> Pointeur sur la structure de dialogue CAM
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  :
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_Write_C3 (OS_HANDLE*        pP_Handle,
                             CAM_CONF*         pL_CamConf,
                             xACTYS_CARTE*     pL_Carte,
                             unsigned char*    puc_TKgr)

{
ACTYS_STATUS    eL_ActysStatus;
unsigned char * pucDataToWrite;

//MODIF JMA 28/04/05 : Flag deja active lors de la lecture, Fichier C3bis OK

    /*****************************************************************************/
    /******************************** Ecriture de C3 *****************************/
    /*****************************************************************************/
   eL_ActysStatus = ACTYS_Write_Chip_C3 ( pP_Handle,
                                          pL_CamConf,
                                         &pL_Carte->puc_DataFromServer[1],
                                         pL_Carte->puc_DataFromServer[0],
                                          puc_TKgr);

    if (eL_ActysStatus != kACTYS_STATUS_OK)
    {
      return (eL_ActysStatus);
    }

    // inutile ?
    //UTS_memcpy(pL_Carte->aucDataC3, &pL_Carte->puc_DataFromServer[1], LG_C3_FILE);

    /**************************************************************************************/
    /***************************** Mise à 0 du bit Miroir de C3bis*************************/
    /**************************************************************************************/
    pucDataToWrite = OS_Malloc(8);
    if( pucDataToWrite == NULL )
       return(kACTYS_STATUS_OUT_OF_MEMORY);

    pucDataToWrite[0] = 0x00;

    eL_ActysStatus = ACTYS_Write_Bloc (  pP_Handle,
                                       pL_CamConf,
                               0x84,
                               pucDataToWrite,
                               8,
                               8,
                               puc_TKgr);

    if (eL_ActysStatus != kACTYS_STATUS_OK)
    {
      OS_Free (pucDataToWrite);
      return (eL_ActysStatus);
    }

    pL_Carte->aucDataC3bis[0] = 0x00;

    OS_Free (pucDataToWrite);

    return (eL_ActysStatus);
}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_Check_Mirror --> Vérification du bit miroir C3bis et écriture
**                                  de C3bis dans C3 dans le cas où il est à 1.
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_CamConf    --> Pointeur sur la structure de dialogue CAM
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  :
********************************************************************************
*******************************************************************************/

ACTYS_STATUS ACTYS_Check_Mirror (OS_HANDLE*        pP_Handle,
                                 CAM_CONF*         pL_CamConf,
                                 xACTYS_CARTE*     pL_Carte,
                                 unsigned char*    puc_TKgr)

{

  ACTYS_STATUS     eL_ActysStatus = kACTYS_STATUS_OK;

  /*****************************************************************************/
  /******************************** Lecture du miroir **************************/
  /*****************************************************************************/

  // Fichier C3 invalide
  if (pL_Carte->aucDataC3bis[0] == 0x10)
  {
     /****************************************************************************/
     /********************** Copie de C3bis dans le fichier C3   *****************/
     /****************************************************************************/
     eL_ActysStatus = ACTYS_Write_Bloc (pP_Handle,
                                        pL_CamConf,
                                 0x83,
                                 &pL_Carte->aucDataC3bis[4],
                                 LG_C3_FILE,
                                 LG_BLOC_WRITE,
                                 puc_TKgr);

     if (eL_ActysStatus != kACTYS_STATUS_OK)
     {
        return (eL_ActysStatus);
     }

     UTS_memcpy(pL_Carte->aucDataC3, &pL_Carte->aucDataC3bis[4], LG_C3_FILE);

//MODIF JMA 04/02/05: Mise a jour buffer C3 envoye au serveur
     UTS_memcpy(pL_Carte->xC3FiletoServer.pC3FileToServer,&pL_Carte->aucDataC3bis[4], LG_C3_FILE);
     /****************************************************************************/
     /******************** Mise à zéro du miroir dans C3bis **********************/
     /****************************************************************************/
//MODIF JMA 28/04/05: Flag Miroir encore active: transaction en cours
  }
  //Fichier C3 Ok transfert dans C3bis et activation Flag Miroir
  else
  {
     /****************************************************************************/
     /********************** Copie de C3 dans le fichier C3bis   *****************/
     /****************************************************************************/
     // Initialisation du message à écrire.
     pL_Carte->aucDataC3bis[0]=0x10; //Activation miroir
     memset(&pL_Carte->aucDataC3bis[1],0,3);
     UTS_memcpy(&pL_Carte->aucDataC3bis[4], pL_Carte->aucDataC3, LG_C3_FILE);

     eL_ActysStatus = ACTYS_Write_Bloc(pP_Handle,
                                       pL_CamConf,
                                0x84,
                                pL_Carte->aucDataC3bis,
                                LG_C3b_FILE,
                                LG_BLOC_WRITE,
                                puc_TKgr);

     if (eL_ActysStatus != kACTYS_STATUS_OK)
     {
        return (eL_ActysStatus);
     }
  }


  return (eL_ActysStatus);

}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_Read_File --> Lecture du fichier Cx
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_CamConf    --> Pointeur sur la structure de dialogue CAM
**           pL_Carte      --> Pointeur sur la structure carte principale
**
** RETOUR  :
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_Read_File (OS_HANDLE*        pP_Handle,
                              CAM_CONF*         pL_CamConf,
                              unsigned char     uc_TypeCFile,
                              unsigned char *   puc_BufferCFile,
                              unsigned int  *   pui_LgCFileRead)
{
ACTYS_STATUS  eL_ActysStatus;
unsigned char auc_AdresseFichier[3];
unsigned char uc_NbReadLoop;
unsigned int  ui_LgCFile;
unsigned int  ui_LgRead;
int i;

  if(uc_TypeCFile == 1)
    ui_LgCFile = LG_C1_FILE;
  else if(uc_TypeCFile == 2)
    ui_LgCFile = LG_C2_FILE;
  else if(uc_TypeCFile == 3)
    ui_LgCFile = LG_C3_FILE;
  else
    ui_LgCFile = LG_C3b_FILE;

    // Selection du fichier EF-Data
    auc_AdresseFichier[0]=0x80+uc_TypeCFile;

    // Nb de lecture de 252 octets a faire
    uc_NbReadLoop = ui_LgCFile / 0xFC + 1;

    /* *****> Lecture C File *********** */
    for(i=0;i<uc_NbReadLoop;i++)
    {
       auc_AdresseFichier[1] = 0x3F*i;  // Offset: adressage de mot (4 octets)

       // dernier passage ?
       if(i == (uc_NbReadLoop - 1))
          auc_AdresseFichier[2] = ui_LgCFile % 0xFC;
       else
          auc_AdresseFichier[2] = 0xFC;

       eL_ActysStatus = ACTYS_ReadBinary(pP_Handle,
                                         pL_CamConf,
                                         auc_AdresseFichier,
                                         puc_BufferCFile+0xFC*i,
                                         &ui_LgRead);

       *pui_LgCFileRead += ui_LgRead;

       if( eL_ActysStatus != kACTYS_STATUS_OK )
          break;
    }

    return(eL_ActysStatus);
    /* <***** Lecture C File *********** */
}


/******************************************************************************/
/*                          Fonctions utiles diverses                         */
/******************************************************************************/

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_Write_Chip_C3   --> Ecriture d'une donnée par blocs dans la carte
**
** APPEL   : pP_Handle             --> Handle de la tache ACTYS
**           pL_CamConf            --> Pointeur sur la structure de dialogue CAM
**           pucDataToWrite        --> Buffer de blocs de donnees a ecrire
**           pucLgData             --> Lg totale du buffer de donnees
**           ucLgData              --> Longueur totale de la donnée à écrire
**           ucLgBloc              --> Longuer du bloc de donnée à écrire
**           puc_TKgr              --> Pointeur sur la clé MAC
**
** RETOUR  :
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_Write_Chip_C3 (OS_HANDLE*        pP_Handle,
                                  CAM_CONF*         pL_CamConf,
                                  unsigned char*    pucDataToWrite,
                                  unsigned char     ucLgData,
                                  unsigned char*    puc_TKgr)

{
unsigned char    auc_Parameters[3];
ACTYS_STATUS     eL_ActysStatus;
unsigned char    uc_NbLoop;
unsigned char    uc_IndexDataToWrite = 0;
int              i = 0;
unsigned int     iL_LgBloc = 0;
unsigned int     iL_OffsetBloc = 0;

   auc_Parameters[0]=0x83;

   //scan du fichier d'entree
   while  (uc_IndexDataToWrite < ucLgData)
   {
      //Recuperation de la longueur et de l'offset
      iL_LgBloc = TLVToInt (&pucDataToWrite[uc_IndexDataToWrite]);
      uc_IndexDataToWrite +=2; // lg bloc
      iL_OffsetBloc= TLVToInt (&pucDataToWrite[uc_IndexDataToWrite]);
      uc_IndexDataToWrite +=2; // offset bloc

     //Nb de boucle de lg LG_BLOC_WRITE
     if ((iL_LgBloc % LG_BLOC_WRITE) == 0)
     {
        uc_NbLoop = iL_LgBloc / LG_BLOC_WRITE;
     }
     else
     {
        uc_NbLoop = iL_LgBloc / LG_BLOC_WRITE + 1;
     }

      for(i=0;i<uc_NbLoop;i++)
      {
         auc_Parameters[1] = iL_OffsetBloc/4 + (LG_BLOC_WRITE/4)*i;  // Offset: adressage de mot (4 octets/adresse)

         // dernier passage ?
         if ((i == (uc_NbLoop - 1)) && (iL_LgBloc % LG_BLOC_WRITE) != 0)
         {
            auc_Parameters[2] = ((iL_LgBloc % LG_BLOC_WRITE) + 3);
         }
         else
         {
            auc_Parameters[2] = LG_BLOC_WRITE + 3;
         }

         eL_ActysStatus = ACTYS_UpdateBinary( pP_Handle,
                                              pL_CamConf,
                                              auc_Parameters,
                                             &pucDataToWrite[uc_IndexDataToWrite],
                                              puc_TKgr);

         if (eL_ActysStatus != kACTYS_STATUS_OK)
         {
            return(eL_ActysStatus);
         }
      }
      uc_IndexDataToWrite += iL_LgBloc;
  }

   return(eL_ActysStatus);

}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_Write_Bloc   --> Ecriture d'une donnée par blocs dans la carte
**
** APPEL   : pP_Handle     --> Handle de la tache ACTYS
**           pL_CamConf    --> Pointeur sur la structure de dialogue CAM
**           pL_Carte      --> Pointeur sur la structure carte principale
**           pucData       --> Pointeur sur la donnée à écrire
**           ucLgData      --> Longueur totale de la donnée à écrire
**           ucLgBloc      --> Longuer du bloc de donnée à écrire
**           puc_TKgr      --> Pointeur sur la clé MAC
**
** RETOUR  :
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_Write_Bloc (OS_HANDLE*        pP_Handle,
                               CAM_CONF*         pL_CamConf,
                               unsigned char     ucNumFic,
                               unsigned char*    pucDataToWrite,
                               int               ucLgData,
                               int               ucLgBloc,
                               unsigned char*    puc_TKgr)

{

  unsigned char *  pucData;
  unsigned char    auc_Parameters[3];
  unsigned char    uc_NbReadLoop;
  ACTYS_STATUS     eL_ActysStatus;
  unsigned int     iL_Index = 0;
  int              i;

   // Nb d'écriture par blocs à faire (sans compter les 3 octets de MAC)
   if ((ucLgData % ucLgBloc) == 0)
   {
      uc_NbReadLoop = ucLgData / ucLgBloc;
   }
   else
   {
      uc_NbReadLoop = ucLgData / ucLgBloc + 1;
   }

   pucData = OS_Malloc(ucLgBloc + 3);

   auc_Parameters[0]=ucNumFic;

   /* *****> Ecriture du fichier C3 bis *********** */
   for(i=0;i<uc_NbReadLoop;i++)
   {
       auc_Parameters[1] = (ucLgBloc / 4)*i;  // Offset: adressage de mot (4 octets)

       // dernier passage ?
       if ((i == (uc_NbReadLoop - 1)) && (ucLgData % ucLgBloc) != 0)
       {
          auc_Parameters[2] = ((ucLgData % ucLgBloc) + 3);
       }
       else
       {
          auc_Parameters[2] = ucLgBloc + 3;
       }

       UTS_memcpy(pucData, &pucDataToWrite[ucLgBloc * i], auc_Parameters[2] - 3);

       eL_ActysStatus = ACTYS_UpdateBinary(pP_Handle,
                                           pL_CamConf,
                                           auc_Parameters,
                                           pucData,
                                           puc_TKgr);

       if (eL_ActysStatus != kACTYS_STATUS_OK)
       {
        OS_Free (pucData);
        return(eL_ActysStatus);
       }
   }

   OS_Free (pucData);

   return(kACTYS_STATUS_OK);
}


/*******************************************************************************
** FONCTION: ACTYS_Free --> Libere une zone de donnees allouee
**
** APPEL   : pP_AdressePointeur --> Pointeur sur l'adresse de la zone a liberer
**
** RETOUR  :
**
** INFOS   : L'adresse de la zone liberee est mise a NULL
********************************************************************************
*******************************************************************************/
void ACTYS_Free(void **pP_AdressePointeur)
{
  OS_Free(*pP_AdressePointeur);
  *pP_AdressePointeur = NULL;
}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_ResetInfoCarte --> Fonction qui remet à jour
**                                    les données de la strucure principale
**
** APPEL   : pL_Carte      --> Pointeur sur la structure carte principale
**
********************************************************************************
*******************************************************************************/
void ACTYS_ResetInfoCarte ( xACTYS_CARTE*  pL_Carte )
{
  /* Pour éviter un Warning lors de la compilation */
  unsigned char *pL_PasDeWarning;

  /* On remplie de 0 toutes les structures pour ne pas avoir de surprises */
  pL_PasDeWarning = memset(pL_Carte, 0, sizeof(xACTYS_CARTE));

  /****************************************************************************/
  /****** Le premier élément de chaque tableau est la longueur des donnees ****/
  /******************** du tableau pour le MAP MEM ****************************/

  /* Card Serial Number */
  pL_Carte->aucCSN[0]
  =sizeof(pL_Carte->aucCSN)-1;

  /* Info Carte */
  pL_Carte->aucInfo[0]
  =sizeof(pL_Carte->aucInfo)-1;

  /* Reference Client */
  pL_Carte->xToServer.aucRefClient[0]
  =sizeof(pL_Carte->xToServer.aucRefClient)-1;

  pL_Carte->xToServer.aucCodeChauf[0]
  =sizeof(pL_Carte->xToServer.aucCodeChauf)-1;

}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_RecupMessageInfo --> Fonction qui renvoie les Tags d'infos
**
** APPEL   :
**
** RETOUR  : pL_Message --> Pointeur sur le tebleau de tag
********************************************************************************
*******************************************************************************/
unsigned char * ACTYS_RecupMessageInfo( void )
{
  /* initialisation des variables locales */
  unsigned char* pL_Message;
  unsigned char  tL_MessageTagPourInfo[36] = {0xE1, 34,
                                              0xC4, 0x02, 0x00, 0x01,
                                              0x5C, 28,
                                              0xDF, 0x82, 0x83, 0x00,
                                              0xDF, 0x85, 0x83, 0x02,
                                              0xDF, 0x85, 0x83, 0x08,
                                              0xDF, 0x85, 0x83, 0x0E,
                                              0xDF, 0x85, 0x83, 0x14,
                                              0xDF, 0x85, 0x83, 0x1A,
                                              0xDF, 0x85, 0x83, 0x20};

  /* Allocation de la mémoire pour le stockage des Tags infos */
  pL_Message = (unsigned char *) OS_Malloc(36);
  /* Copie des tags info */
  UTS_memcpy(pL_Message, tL_MessageTagPourInfo, 36);

  return(pL_Message);
}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_Affiche    --> Affiche un texte sur la visu
**
** APPEL   : pP_Handle        --> Handle de la tache
**           cL_Efface        --> Indicateur de requisition d'effacement de visu
**           cL_NumeroDeLigne --> Numero de ligne d'affichage du texte
**           pL_Texte         --> Pointeur sur le texte a afficher
**           cL_Longueur      --> Longueur du texte a afficher
**
** RETOUR  :
********************************************************************************
*******************************************************************************/
void ACTYS_Affiche(OS_HANDLE*     pP_Handle,
                   unsigned char  cL_Efface,
                   unsigned char  cL_NumeroDeLigne,
                   unsigned char* pL_Texte,
                   unsigned char  cL_Longueur)
{
  unsigned char* pL_MessageFinal; /* --> Pointeur sur message formate         */
  unsigned char  cL_Y;            /* --> Numero de ligne d'affichage          */
  unsigned int   iL_Index = 0;    /* --> Index de formatage du message        */

  /* =====> MISE EN PLACE DE LA SEQUENCE DE POSITIONNEMENT DE L'AFFICHAGE === */
  cL_Y = cL_NumeroDeLigne;

  pL_MessageFinal = OS_Malloc(cL_Longueur + 7);
  if (pL_MessageFinal == NULL)
  {
    /* -----> Espace memoire insuffisant ------------------------------------ */
    return;
    /* <----- Espace memoire insuffisant ------------------------------------ */
  }

  pL_MessageFinal[iL_Index++] = ESC;
  if (cL_Efface != 0)
  {
    /* +++++> Effacement de la visu requis ++++++++++++++++++++++++++++++++++ */
    pL_MessageFinal[iL_Index++] = 'H';
    /* <+++++ Effacement de la visu requis ++++++++++++++++++++++++++++++++++ */
  }
  else
  {
    /* +++++> Effacement non requis +++++++++++++++++++++++++++++++++++++++++ */
    iL_Index--;
    /* <+++++ Effacement non requis +++++++++++++++++++++++++++++++++++++++++ */
  }

  pL_MessageFinal[iL_Index++] = ESC;
  pL_MessageFinal[iL_Index++] = 'P';
  pL_MessageFinal[iL_Index++] = cL_Y;
  pL_MessageFinal[iL_Index++] = 1;

  UTS_memcpy(&pL_MessageFinal[iL_Index], pL_Texte, cL_Longueur);

  pL_MessageFinal[iL_Index + cL_Longueur] = FC;
  /* <===== MISE EN PLACE DE LA SEQUENCE DE POSITIONNEMENT DE L'AFFICHAGE === */

  /* =====> REALISATION DE L'AFFICHAGE ====================================== */
  API_DISPLAY_WindDial(0, NULL, pL_MessageFinal, 0, 0, pP_Handle);
  /* <===== REALISATION DE L'AFFICHAGE ====================================== */

  ACTYS_Free(&pL_MessageFinal);
}

/***************************************************************************
* Function Name  : ACTYS_GetVersion
*
* Description    :
*
* Side effects   :
*
* Comment        :
*
***************************************************************************/
unsigned char * ACTYS_GetVersion( OS_HANDLE *pP_Handle, ACTYS_STATUS * pP_ActysStatus, unsigned long lP_Type)
{
unsigned char  *pL_Version ;
HANDLE_FLASH   *pL_HandleFlash ;
TLV            *pL_ResultatTLVVersion = NULL ;
unsigned char  *pL_DonneesDeLaFLASH ;
unsigned char  cL_FlashResult ;

   pL_HandleFlash = FLASH_OpenFlash( pP_Handle, &cL_FlashResult ) ;
   if ( cL_FlashResult != OS_NO_ERR )
   {
           *pP_ActysStatus = kACTYS_STATUS_ERREUR_DONNEES_TELECHARGE ;
           return( NULL ) ;
   }

   pL_DonneesDeLaFLASH = FLASH_SearchTLV (pP_Handle, pL_HandleFlash, kACTYS_TAG_APPLI_TOTAL ,lP_Type , &cL_FlashResult);

   if ( cL_FlashResult != OS_NO_ERR || pL_DonneesDeLaFLASH == NULL )
   {
           OS_Free( pL_DonneesDeLaFLASH ) ;
           FLASH_Close( pL_HandleFlash ) ;
           *pP_ActysStatus = kACTYS_STATUS_ERREUR_DONNEES_TELECHARGE ;
           return( NULL ) ;
   }

   pL_ResultatTLVVersion = ChercheTag(pL_DonneesDeLaFLASH,
                                      pL_HandleFlash->iLongueurFichier,
                                      kACTYS_TAG_VERSION,
                                      NULL);

   if ( pL_ResultatTLVVersion == NULL )
   {
           OS_Free( pL_DonneesDeLaFLASH ) ;
           FLASH_Close( pL_HandleFlash ) ;
           *pP_ActysStatus = kACTYS_STATUS_ERREUR_DONNEES_TELECHARGE ;
           return( NULL ) ;
   }

   pL_Version = (unsigned char *)OS_Malloc(pL_ResultatTLVVersion->iLongueur);
   if ( pL_Version == NULL )
   {
           TLV_LibereTLV(pL_ResultatTLVVersion, 'r');
           OS_Free( pL_DonneesDeLaFLASH ) ;
           FLASH_Close( pL_HandleFlash ) ;
           *pP_ActysStatus = kACTYS_STATUS_OUT_OF_MEMORY ;
           return( NULL );
   }
   UTS_memcpy(pL_Version, pL_ResultatTLVVersion->pData, pL_ResultatTLVVersion->iLongueur);

   TLV_LibereTLV(pL_ResultatTLVVersion, 'r');
   OS_Free( pL_DonneesDeLaFLASH ) ;
   FLASH_Close(pL_HandleFlash) ;
   *pP_ActysStatus = kACTYS_STATUS_OK ;
   return (pL_Version) ;
}


/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_RefreshInfo --> Fonction qui remet à jour
**                                 les infos de l'appli
**
** APPEL   : pP_Handle         --> Handle de la tache MONEO
**           pL_MoneoInfo      --> Pointeur sur la structure d'info appli
**
** RETOUR  : kMONEO_STATUS_OK
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_RefreshInfo(OS_HANDLE *pP_Handle, xStructINFO *pL_ActysInfo )
{
/* Initialisation des variables locales */
unsigned char *pL_PasDeWarning ;/* Evite les Warning pendant la compilation */
unsigned char *pL_Version ;
ACTYS_STATUS   eL_ActysStatusMessages ;

   /* Initialisation des longueurs */
   pL_ActysInfo->tVerSpecifications[0]       = 4 ;
   pL_ActysInfo->tVerApplication[0]          = 4 ;
   pL_ActysInfo->tEditeurApplication[0]      = 4 ;
   pL_ActysInfo->tVerListeMessages[0]        = 0 ;
   pL_ActysInfo->tVerListeCles[0]            = 0 ;
   pL_ActysInfo->tVerListeAID[0]             = 0 ;

   /* Copie des valeurs dans la strucutres d'info */
   pL_ActysInfo->iId = kACTYS_ID ;
   pL_PasDeWarning = memcpy(&pL_ActysInfo->tVerSpecifications[1],
                            kACTYS_INFO_VER_SPECIFICATIONS,
                            4 );
   pL_PasDeWarning = memcpy(&pL_ActysInfo->tVerApplication[1],
                            kACTYS_INFO_VER_APPLICATION,
                            4 );
   pL_PasDeWarning = memcpy(&pL_ActysInfo->tEditeurApplication[1],
                            kACTYS_INFO_EDITEUR_APPLICATION,
                            4 );

   pL_Version = ACTYS_GetVersion( pP_Handle, &eL_ActysStatusMessages, kACTYS_TAG_ListeMessages) ;
   if ( eL_ActysStatusMessages == kACTYS_STATUS_OK )
   {
           pL_ActysInfo->tVerListeMessages[0] = 4 ;
           UTS_memcpy(&pL_ActysInfo->tVerListeMessages[1], pL_Version, 4 );
   }
   OS_Free(pL_Version);

   if ( eL_ActysStatusMessages != kACTYS_STATUS_OK )
   {
           return ( eL_ActysStatusMessages ) ;
   }

   /* statut OK */
   return(kACTYS_STATUS_OK) ;
}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_SaisieCodes --> Realise la saisie du code pin par le porteur
**
** APPEL   : IN  pP_Handle                 --> Handle de la tache
**               pL_Carte                  --> Pointeur sur la structure CAM
**               iL_CompteurPresentation   --> Compteur de présentation des codes
**               puc_ISO2                  --> ISO2 sur 19 octets
**
**           OUT puc_Pin                   --> Code confidentiel saisi
**               puc_Pin_Driver            --> Code chauffeur saisi
**
** RETOUR  :   kACTYS_STATUS_OK                            --> Code saisi
**             kACTYS_STATUS_OUT_OF_MEMORY                 --> Capacite memoire depassee
**             kACTYS_STATUS_TIME_OUT                      --> Delai expire
**             kACTYS_STATUS_ABANDON                    --> Saisie abandonnee
**             kACTYS_STATUS_CARTE_ARRACHEE                --> Carte arrachee
**             kACTYS_STATUS_ERROR_CARTE_BLOQUEE           --> Application bloquee
**          kACTYS_STATUS_ERROR_ISO2                    --> Erreur Type de PIN
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_SaisieCodes ( OS_HANDLE     * pP_Handle,
                                  xACTYS_CARTE  * pL_Carte,
                                  unsigned char * puc_Pin,
                                  unsigned char * puc_Pin_Driver,
                                  CAM_CONF      **ppL_CamConf)
{
  ACTYS_STATUS    eL_ActysStatus=kACTYS_STATUS_OK;
  
  // saisie PIN code
  if( (pL_Carte->uc_TypePIN >= '0' ) && ( pL_Carte->uc_TypePIN <= '3' ) )
  {
    eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, puc_Pin,'P',ppL_CamConf);
  }
  // saisie PIN code + PIN chauffeur
  else if ( pL_Carte->uc_TypePIN == '4' )
  {
    eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, puc_Pin,'P',ppL_CamConf);
    
    if (eL_ActysStatus != kACTYS_STATUS_OK)
    {
      return(eL_ActysStatus);
    }
    
    eL_ActysStatus = ACTYS_SaisieCode(pP_Handle, pL_Carte, puc_Pin_Driver,'C',ppL_CamConf);
    
    // Transfert code chauff au serveur
    if(eL_ActysStatus == kACTYS_STATUS_OK)
    {
      UTS_memcpy(&pL_Carte->xToServer.aucCodeChauf[1],puc_Pin_Driver,4);
    }
  }
  // pas de PIN demande
  else if ( pL_Carte->uc_TypePIN == '9' )
    return(kACTYS_STATUS_OK);
  // erreur type pin
  else
    return(kACTYS_STATUS_ERROR_ISO2);
  
  return(eL_ActysStatus);
}

/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_SaisieCode --> Realise la saisie du code pin par le porteur
**
** APPEL   : pP_Handle                --> Handle de la tache
**           pL_Carte                 --> Pointeur sur la structure CAM
**           iL_CompteurPresentation  --> Compteur de présentation de code
**           pL_Pin                   --> Pin saisi
**
** RETOUR  : kACTYS_STATUS_OK                            --> Code saisi
**           kACTYS_STATUS_TIME_OUT                      --> Delai expire
**           kACTYS_STATUS_ABANDON                 --> Saisie abandonnee
**           kACTYS_STATUS_CARTE_ARRACHEE                --> Carte arrachee
**           kACTYS_STATUS_ERROR_CARTE_BLOQUEE           --> Application bloquee
**           kACTYS_STATUS_OUT_OF_MEMORY                 --> Capacite memoire depassee
********************************************************************************
*******************************************************************************/
ACTYS_STATUS ACTYS_SaisieCode(OS_HANDLE     * pP_Handle,
                              xACTYS_CARTE  * pL_Carte,
                              unsigned char * pL_Pin,
                              unsigned char   ucType_Code,
                              CAM_CONF      **ppL_CamConf)
{
  unsigned char* pL_NoWarning;
  unsigned char  cL_NoWarning;
  unsigned char  cL_Buffer[5] ;
  unsigned char  tL_Resultat[15];
  unsigned char  tL_ParaSaisie[5] = {1,1,'X',4,4};
  int            iL_Compteur;
  unsigned int   iL_Masque_OS;
  ACTYS_STATUS   e_Status;
  unsigned char  uc_TypeMessage;
  int            iL_PINLength;
  //  int i;
  
  /* <===== CONTROLES PRELIMINAIRES ========================================= */
  
  /* =====> AFFICHAGE DU MESSAGE DE SAISIE ================================== */
  iL_PINLength=kACTYS_PIN_LEN;
  
  if (ucType_Code == 'P')
    uc_TypeMessage = kACTYS_MESS_CODE_PIN;
  else if (ucType_Code == 'C')
    uc_TypeMessage = kACTYS_MESS_CODE_CHAUFF;
  else if (ucType_Code == 'B') {
    uc_TypeMessage = kACTYS_MESS_CODE_PIN;
    tL_ParaSaisie[3] = 4; /* En fait 2 */
    tL_ParaSaisie[4] = 4; /* En fait 6 */
    iL_PINLength=kBICA_PIN_LEN;
  }
  
  e_Status = ACTYS_AfficheMessage(pP_Handle,
                                1,
                                1,
                                pL_Carte->xFromServer.iCodeLangue,
                                uc_TypeMessage);

  if( e_Status != kACTYS_STATUS_OK)
     return(e_Status);


  /* *****> Chargement du libelle du message a afficher ********************* */
  switch((pL_Carte->xToServer).ui_PinTryCounter)
  {
    case 0:
      /* ******************************************************************** */
      /* ***** APPLICATION BLOQUEE ****************************************** */
      /* ******************************************************************** */
     e_Status = ACTYS_AfficheMessage(  pP_Handle,
                                    1,
                                    2,
                                    pL_Carte->xFromServer.iCodeLangue,
                                    kACTYS_MESS_APPLI_BLOQUEE);

      TEMPO_XMS(pP_Handle, 1500);

      if( e_Status != kACTYS_STATUS_OK)
         return(e_Status);
      else
        return(kACTYS_STATUS_ERROR_CARTE_BLOQUEE);

    case 1:
      /* ******************************************************************** */
      /* ***** DERNIER ESSAI ************************************************ */
      /* ******************************************************************** */
      uc_TypeMessage = kACTYS_MESS_DERNIER_ESSAI;
      break;

    case 2:
      /* ******************************************************************** */
      /* ***** 2EME ESSAI *************************************************** */
      /* ******************************************************************** */
      uc_TypeMessage = kACTYS_MESS_DEUXIEME_ESSAI;
      break;

    case 3:
    default:
      /* ******************************************************************** */
      /* ***** 1ER ESSAI **************************************************** */
      /* ******************************************************************** */
      uc_TypeMessage = kACTYS_MESS_PREMIER_ESSAI;
      break;
  }
  /* *****> Chargement du libelle du message a afficher ********************* */

  /* *****> Affichage du nombre d'essais restants *************************** */

  e_Status = ACTYS_AfficheMessage(  pP_Handle,
                                  0,
                                  2,
                                  pL_Carte->xFromServer.iCodeLangue,
                                uc_TypeMessage);

  if( e_Status != kACTYS_STATUS_OK)
      return(e_Status);

  /* <***** Affichage du nombre d'essais restants *************************** */

  /* =====> AFFICHAGE DU MESSAGE DE SAISIE ================================== */

  if((pL_Carte->xFromServer.lListEtape & kACTYS_CHECK_PVV_TRACK_CARD) != 0)
    iL_Masque_OS = 0;
  else
    iL_Masque_OS = OS_PEI_CARTE_RETIRAIT;

  /* =====> REALISATION DE LA SAISIE ======================================== */

  /* *****> Positionnement de l'affichage de la zone de saisie ************** */
  cL_Buffer[0] = ESC;
  cL_Buffer[1] = 'P';
  cL_Buffer[2] = 2;
  cL_Buffer[3] = 1;
  cL_Buffer[4] = FC;
  /* <***** Positionnement de l'affichage de la zone de saisie ************** */

  /* *****> Saisie ********************************************************** */
  cL_NoWarning = API_DISPLAY_WinSaisie(0,
                                       NULL,
                                       cL_Buffer,
                                       0,
                                       0,
                                       tL_ParaSaisie,
                                       kACTYS_TEMPO_SAISIE_1ERE_TOUCHE,
                                       kACTYS_TEMPO_SAISIE,
                                       iL_Masque_OS,
    tL_Resultat,
    pP_Handle);
  
    /*
    API_UTILS_SendOnUART0( 'X');
    for (i=0;i<10;i++) {
    API_UTILS_SendOnUART0( tL_Resultat[i]);
    }
    API_UTILS_SendOnUART0( 'X');
  */
  
  
  if (tL_Resultat[1] == 0xFF)
  {
    /* -----> Abandon suite a arrachage carte ------------------------------- */
     e_Status = ACTYS_AfficheMessage(  pP_Handle,
                                    1,
                                    2,
                                    pL_Carte->xFromServer.iCodeLangue,
                                  kACTYS_MESS_CARTE_ARRACHEE);

     TEMPO_XMS(pP_Handle, 1500);

     if( e_Status != kACTYS_STATUS_OK)
        return(e_Status);
    else
      return(kACTYS_STATUS_CARTE_ARRACHEE);
    /* <----- Abandon suite a arrachage carte ------------------------------- */
  }

  if ((tL_Resultat[0] == 0) || (tL_Resultat[1] == 1) || (tL_Resultat[1] == 2))
  {
    /* -----> Abandon de la saisie ------------------------------------------ */
     e_Status = ACTYS_AfficheMessage(  pP_Handle,
                                    1,
                                    2,
                                    pL_Carte->xFromServer.iCodeLangue,
                                  kACTYS_MESS_ABANDON);

     TEMPO_XMS(pP_Handle, 1500);

     if( e_Status != kACTYS_STATUS_OK)
        return(e_Status);

    if ( (tL_Resultat[1] == 1) || (tL_Resultat[1] == 2) )
    {
      /* -----> Expiration du delai avant saisie de la premiere touche ------ */
      return(kACTYS_STATUS_TIME_OUT_SAISIE);
      /* <----- Expiration du delai avant saisie de la premiere touche ------ */
    }
    else
       return(kACTYS_STATUS_ABANDON);
    /* <----- Abandon de la saisie ------------------------------------------ */
  }
  /* <***** Saisie ********************************************************** */

  iL_Compteur = 0;
  /* *****> Analyse des touches saisies ************************************* */
  pL_NoWarning = memset (pL_Pin,
                        kACTYS_PADDING_AFTER_PIN,
                        iL_PINLength);

  while ((tL_Resultat[iL_Compteur + 2] != 'V')
   &&    (tL_Resultat[iL_Compteur + 2] != 'A')
   &&    (iL_Compteur < iL_PINLength ))
  {
    /* Le code PIN est en enregistré pour la présentation au PME */
    pL_Pin[iL_Compteur] = tL_Resultat[iL_Compteur + 2];
    iL_Compteur++;
  }

  /* On regarde si le porteur n'a pas validé le code porteur */
  if (tL_Resultat[iL_Compteur + 2] != 'V')
  {
    /* Abandon de la transaction */
     e_Status = ACTYS_AfficheMessage(  pP_Handle,
                                    1,
                                    2,
                                    pL_Carte->xFromServer.iCodeLangue,
                                  kACTYS_MESS_ABANDON);

   TEMPO_XMS(pP_Handle, 1500);

     if( e_Status != kACTYS_STATUS_OK)
        return(e_Status);
    else
      return(kACTYS_STATUS_ABANDON);
  }

  /* *****> Mise a jour de l'etat du coupleur CAM ********************* */
  if(ppL_CamConf!=NULL)
  {
      CAM_RefreshStat(ppL_CamConf);

      // Controle carte arrachee uniquement pour carte actys
      if (((((*ppL_CamConf)->iState & kCAM_STATE_REMOVED) != 0)
       || (((*ppL_CamConf)->iState & kCAM_STATE_ENDCOURSE) == 0))
        && ( iL_Masque_OS == OS_PEI_CARTE_RETIRAIT ))
      {
        /* -----> Abandon suite a arrachage carte ------------------------------- */
         e_Status = ACTYS_AfficheMessage(  pP_Handle,
                                      1,
                                      2,
                                      pL_Carte->xFromServer.iCodeLangue,
                                    kACTYS_MESS_CARTE_ARRACHEE);

         TEMPO_XMS(pP_Handle, 1500);

         if( e_Status != kACTYS_STATUS_OK)
            return(e_Status);
        else
          return(kACTYS_STATUS_CARTE_ARRACHEE);
        /* <----- Abandon suite a arrachage carte ------------------------------- */
      }
  }
  /* <***** Mise a jour de l'etat du coupleur CAM ********************* */


  /* Le porteur a validé la transaction */
  return(kACTYS_STATUS_OK);
}
/***************************   ACTYS_SaisieCode   *****************************/


/*******************************************************************************
********************************************************************************
** FONCTION: ACTYS_EtapeExecute --> Realise la saisie du code pin par le porteur
**
** APPEL   : pL_Carte                 --> Pointeur sur la structure carte
**           eL_ActysStatus          --> statut retourne par la fonctionnalite
**           ul_Etape                 --> Etape en cours
**
** RETOUR  : Rien
********************************************************************************
*******************************************************************************/
void ACTYS_EtapeExecute(xACTYS_CARTE * pL_Carte,
                        ACTYS_STATUS   eL_ActysStatus,
                        unsigned long  ul_Etape)
{
   if (eL_ActysStatus == kACTYS_STATUS_OK)
   {
      /* +++++> Commande realisee +++++++++++++++++++++++++++++++++++++ */
      (pL_Carte->xFromServer).lListEtape   &= (ul_Etape ^ 0xFFFFFFFF);
      (pL_Carte->xToServer).lEtapesExecute |= ul_Etape;
      /* <+++++ Commande realisee +++++++++++++++++++++++++++++++++++++ */
   }
   else
   {
      /* -----> Commande echouee -------------------------------------- */
      pL_Carte->xFromServer.lListEtape = 0;
      /* <----- Commande echouee -------------------------------------- */
   }
}


/*******************************************************************************
********************************************************************************
**
** FONCTION: ACTYS_Tache     --> Fonction représentant le point d'entrée
**
********************************************************************************
*******************************************************************************/
void TOTAL_Tache(void)
{
unsigned char  * pL_Message;           /* --> Pointeur sur message recu       */
unsigned char  * pL_MessageReponse;    /* --> Pointeur sur message de reponse */
unsigned char  * pL_ConvIntToChar;     /* --> Pointeur pour conversions       */
unsigned char    tL_AidActysTLV[18] = kACTYS_AID_ACTYS_TLV;
unsigned char    cL_ResultOS;          /* --> Etat de l'OS                    */
unsigned char    cl_IsContextToFree;
unsigned char    cl_IsCurrentApplication;

unsigned int     iL_CodeCde;           /* --> Type d'action a entreprendre    */
unsigned int   * pL_Tampon;            /* --> Pointeur sur adresse resultat   */
unsigned int     iL_LgMessageReponse;  /* --> Taille du message de reponse    */
unsigned int     iL_LongueurRecue;     /* --> Taille du message recu          */

OS_HANDLE        xL_ACTYSHandle;       /* --> Handle de la tache              */
MAPMEM         * pL_MapMem;            /* --> Pointeur de mapping memoire     */
MAPMEM         * pL_MapMemAntiWarning; /* --> Suppression de Warnings         */
CAM_CONF       * pL_CamConf;           /* --> Structure coupleur CAM          */
CAM_STATUS       eL_CamStatus;         /* --> Etat du coupleur CAM            */
ACTYS_STATUS     eL_ActysStatus;       /* --> Etat des traitements            */
ACTYS_STATUS    eL_Status;            /* --> Etat intermediaire              */
xACTYS_CARTE   * pL_ActysInfoCarte;    /* --> Structure ACTYS                 */
xStructINFO    * xL_ActysInfo;         /* --> Structure d'informations ACTYS  */
TLV            * pL_TLVMessageReponse; /* --> Pointeur sur TLV de reponse     */
unsigned char  * pL_DataOut;           /* --> Buffer Reponse CAM              */
unsigned char  * puc_Kgr;
unsigned char    auc_CRYPTO_Kgr[8];
unsigned char  * puc_MKgr;
unsigned char  * tucKeyLeft;
unsigned char  * tucKeyRight;
unsigned char  * pL_Crypto_LSB_C;   /* --> LSB Crypto Carte                */
unsigned char  * pL_Crypto_T;       /* --> Crypto Terminal                 */
unsigned char    pL_Crnd[8];           /* --> Nb aleatoire genere par la carte*/
unsigned short   us_erreur;
unsigned char    auc_AdresseFichier[3];
unsigned char    auc_unused[8];
volatile char    a,b,c,d,e;
int              i;
unsigned char    uc_flag_break=0;
unsigned char    uc_flag_puce=0;

#if DEBUG
unsigned char  * pL_Affichage;         /* --> Affichage message               */
unsigned char *pL;
unsigned char pL2[4];
unsigned char pL_AVIA_ISO2[40]={";9250001281234501705D05090000612BBBBBBFC"};
unsigned char pL_UTA_ISO2[40] ={"(70600012345612347D0012000836600000000F4"};
#endif

  /* =====> INITIALISATION DU CONTEXTE DE LA TACHE ========================== */
  OS_EXIT_CRITICAL();

  InitSysteme(&xL_ACTYSHandle);
  a = txt0[i];
  b = txt1[i];
  c = txt2[i];
  d = txt3[i];
  e = txt4[i];

  pL_MapMem                       = NULL;
  pL_ActysInfoCarte               = NULL;
  xL_ActysInfo                    = NULL;
  cl_IsContextToFree              = 0;
  cl_IsCurrentApplication         = 0;
  /* <===== INITIALISATION DU CONTEXTE DE LA TACHE ========================== */

  /* =====> TACHE =========================================================== */
  for(;;)
  {
    /* =====> MISE EN ATTENTE D'UN MESSAGE DU NOYAU ========================= */
    pL_Message = OSMboxPend(xL_ACTYSHandle.pMBox, 0, &cL_ResultOS);
    /* <===== MISE EN ATTENTE D'UN MESSAGE DU NOYAU ========================= */

    /* =====> ANALYSE DU MESSAGE RECU ======================================= */
    if (cL_ResultOS == OS_NO_ERR)
    {
      /* +++++> Message recu sans erreur ++++++++++++++++++++++++++++++++++++ */
      /* *****> Mise en place du contexte de l'application ****************** */
      if (pL_ActysInfoCarte == NULL)
      {
        /* +++++> Mise en place requise +++++++++++++++++++++++++++++++++++++ */
        if (pL_MapMem == NULL)
        {
          /* +++++> MApping memoire a etablir +++++++++++++++++++++++++++++++ */
          pL_MapMem = InitMapMem();
          /* <+++++ MApping memoire a etablir +++++++++++++++++++++++++++++++ */
        }

        /*=================== EN GUISE D'EXEMPLE===============================*/
        pL_ActysInfoCarte = (xACTYS_CARTE*) OS_Malloc(sizeof(xACTYS_CARTE));
        ACTYS_ResetInfoCarte(pL_ActysInfoCarte);

        pL_MapMemAntiWarning
        = AjouteMapMem(pL_MapMem,
                  0xDF8200,
                  (unsigned char*) &eL_ActysStatus,
                  sizeof(int));

        pL_MapMemAntiWarning
        = AjouteMapMem(pL_MapMem,
                  0xDFE401,
                  (unsigned char*) &(pL_ActysInfoCarte->xToServer),
                  sizeof(xStructTO_SERVER));

        pL_MapMemAntiWarning
        = AjouteMapMem(pL_MapMem,
                  0xDFE402,
                  (unsigned char*) &(pL_ActysInfoCarte->xFromServer),
                  sizeof(xStructFROM_SERVER));

        pL_MapMemAntiWarning
        = AjouteMapMem(pL_MapMem,
                  0xDFE504,
                  (unsigned char*) pL_ActysInfoCarte->puc_DataFromServer,
                  (LG_BUF_FROM_SERVER));

        pL_MapMemAntiWarning
        = AjouteMapMem(pL_MapMem,
                  0xDFE405,
                  (unsigned char*) &(pL_ActysInfoCarte->xC1FiletoServer),
                  sizeof(xStructC1FILE_TO_SERVER));

        pL_MapMemAntiWarning
        = AjouteMapMem(pL_MapMem,
                  0xDFE406,
                  (unsigned char*) &(pL_ActysInfoCarte->xC2FiletoServer),
                  sizeof(xStructC2FILE_TO_SERVER));

        pL_MapMemAntiWarning
        = AjouteMapMem(pL_MapMem,
                  0xDFE407,
                  (unsigned char*) &(pL_ActysInfoCarte->xC3FiletoServer),
                  sizeof(xStructC3FILE_TO_SERVER));

        /*===================================================================*/

        /* <+++++ Mise en place requise +++++++++++++++++++++++++++++++++++++ */
      }

      if (xL_ActysInfo == NULL)
      {
        /* +++++> Mise en place requise +++++++++++++++++++++++++++++++++++++ */
        if (pL_MapMem == NULL)
        {
          /* +++++> MApping memoire a etablir +++++++++++++++++++++++++++++++ */
          pL_MapMem = InitMapMem();
          /* <+++++ MApping memoire a etablir +++++++++++++++++++++++++++++++ */
        }

        xL_ActysInfo = (xStructINFO*) OS_Malloc(sizeof(xStructINFO));
        pL_Tampon    = memset(xL_ActysInfo, 0, sizeof(xStructINFO));

        pL_MapMemAntiWarning = AjouteMapMem(pL_MapMem,
                                0xDFE403,
                                (unsigned char*) xL_ActysInfo,
                                sizeof(xStructINFO));
        /* <+++++ Mise en place requise +++++++++++++++++++++++++++++++++++++ */
      }
      /* <***** Mise en place du contexte de l'application ****************** */

      eL_ActysStatus = kACTYS_STATUS_OK;

      /* =====> EXTRACTION DES ACTIONS REQUISES ============================= */
      if (pL_Message[MS_TAG]==1)
      {
        /* +++++> Message encode avec des TLV +++++++++++++++++++++++++++++++ */
        /* *****> Interpretation du message ********************************* */
        iL_LongueurRecue = (pL_Message[MS_Longueur]
                    + (pL_Message[MS_Longueur + 1] * 0x100));
        TLV_InterpreteCde(&pL_Message[MS_DebutMessage],
                     iL_LongueurRecue,
                     pL_MapMem,
                    &iL_CodeCde);
        /* <***** Interpretation du message ********************************* */

        /* *****> Analyse du type d'action a entreprendre ******************* */
        switch(iL_CodeCde)
        {
          case kACTYS_CODE_CDE_GET_INFO:
            /* ************************************************************** */
            /* ***** FOURNITURE DES INFORMATIONS DE L'APPLICATION *********** */
            /* ************************************************************** */
            pL_ActysInfoCarte->xFromServer.lListEtape = 0 ;
            eL_ActysStatus     = ACTYS_RefreshInfo(&xL_ACTYSHandle,xL_ActysInfo);
            cl_IsContextToFree = (cl_IsCurrentApplication == 0);
          break;

          case kACTYS_CODE_CDE_RESET_APPLI:
            /* ************************************************************** */
            /* ***** REDEMARRAGE DE L'APPLICATION (RUF) ********************* */
            /* ************************************************************** */
            pL_ActysInfoCarte->xFromServer.lListEtape = 0;
          break;

          case kACTYS_CODE_CDE_EXECUTION:
            /* ************************************************************** */
            /* ***** EXECUTION DES COMMANDES ******************************** */
            /* ************************************************************** */
          default:
          break;
        }
        /* <***** Analyse du type d'action a entreprendre ******************* */
        /* <+++++ Message encode avec des TLV +++++++++++++++++++++++++++++++ */
      }
      else
      {
        /* +++++> Message non encode Commande / Etapes / &CAM_CONF ++++++++++ */
        /* *****> Analyse du type d'action a entreprendre ******************* */
        iL_CodeCde = pL_Message[MS_DebutMessage] ;
        switch (iL_CodeCde)
        {
          case kACTYS_CODE_CDE_GET_INFO:
            /* ************************************************************** */
            /* ***** FOURNITURE DES INFORMATIONS DE L'APPLICATION *********** */
            /* ************************************************************** */
            pL_ActysInfoCarte->xFromServer.lListEtape = 0 ;
            eL_ActysStatus     = ACTYS_RefreshInfo(&xL_ACTYSHandle,xL_ActysInfo);
            cl_IsContextToFree = (cl_IsCurrentApplication == 0);
          break;

          case kACTYS_CODE_CDE_RESET_APPLI:
            /* ************************************************************** */
            /* ***** REDEMARRAGE DE L'APPLICATION (RUF) ********************* */
            /* ************************************************************** */
            pL_ActysInfoCarte->xFromServer.lListEtape = 0;
          break;

          case kACTYS_CODE_CDE_EXECUTION:
            /* ************************************************************** */
            /* ***** EXECUTION DES COMMANDES ******************************** */
            /* ************************************************************** */
          default:
            pL_ActysInfoCarte->xFromServer.lListEtape = 0 ;
            pL_ConvIntToChar        = (unsigned char*) &pL_ActysInfoCarte->xFromServer.lListEtape;
            *pL_ConvIntToChar       = pL_Message[MS_DebutMessage + 2];
            *(pL_ConvIntToChar + 1) = pL_Message[MS_DebutMessage + 1];

            pL_ConvIntToChar        = (unsigned char*) &pL_CamConf;
            *pL_ConvIntToChar       = pL_Message[MS_DebutMessage + 3];
            *(pL_ConvIntToChar + 1) = pL_Message[MS_DebutMessage + 4];
          break;
        }
        /* <***** Analyse du type d'action a entreprendre ******************* */
        /* <+++++ Message non encode Commande / Etapes / &CAM_CONF ++++++++++ */
      }
      /* <===== EXTRACTION DES ACTIONS REQUISES ============================= */

      /* =====> REALISATION DES ACTIONS REQUISES ============================ */
      pL_ActysInfoCarte->xToServer.lEtapesExecute = 0;

      /* *****> Mise a jour de l'etat du coupleur CAM *********************** */

      /* On regarde s'il y a une commande CAM pour tester l'arrachage */
      if (((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_BUILD) != 0)
       || ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_INTERNAL_AUTHENT) != 0)
       || ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_CHECK_BLOCAGE) != 0)
       || ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_READ_C_FILE) != 0)
       || ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_CHECK_MIRROR) != 0)
       || ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_EXTERNAL_AUTHENT) != 0)
       || ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_WRITE_C_FILE) != 0))
      {
        /* On va envoyer une comande CAM donc on peut tester l'arrachage */
        CAM_RefreshStat(&pL_CamConf);

        if (((pL_CamConf->iState & kCAM_STATE_REMOVED) != 0)
        || ((pL_CamConf->iState & kCAM_STATE_ENDCOURSE) == 0))
        {
          /* -----> Carte arrachee ------------------------------------------ */
          eL_ActysStatus = kACTYS_STATUS_CARTE_ARRACHEE;

          pL_ActysInfoCarte->xFromServer.lListEtape = 0;
          /* <----- Carte arrachee ------------------------------------------ */
        }
      }
      /* <***** Mise a jour de l'etat du coupleur CAM *********************** */

/*
//DEBUG: liste etape carte a piste
#if DEBUG
pL_ActysInfoCarte->xFromServer.lListEtape  = kACTYS_CHECK_PVV_TRACK_CARD;
pL_ActysInfoCarte->xFromServer.ui_TrackCardType = UTA_CARD;
//pL_ActysInfoCarte->xFromServer.ui_TrackCardType = DKV_SELECTION_CARD;
//pL_ActysInfoCarte->xFromServer.ui_TrackCardType = DKV_CLASSIC_CARD;
//pL_ActysInfoCarte->xFromServer.ui_TrackCardType = DKV_CLASSIC_CARD_AND_DRIVER_CODE;
//pL_ActysInfoCarte->xFromServer.ui_TrackCardType = GR_EUROTRAFIC_CARD;
//pL_ActysInfoCarte->xFromServer.ui_TrackCardType = AVIA_CARD;
UTS_memcpy(pL_ActysInfoCarte->puc_DataFromServer,pL_UTA_ISO2,LG_ISO2);
pL_ActysInfoCarte->xFromServer.ui_PinTryCounter=3;
pL_ActysInfoCarte->xFromServer.iCodeLangue=0x0250;
#endif
*/
      while (pL_ActysInfoCarte->xFromServer.lListEtape != 0)
      {
        /* ***************************************************************** */
        /* *************** BUILD DE L'APPLICATION ACTYS ******************** */
        /* ***************************************************************** */
        if ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_BUILD) != 0)
        {
          /* *****> Mise hors tension / Remise sous tension carte *********** */
          eL_CamStatus = CAM_SetCommand(&xL_ACTYSHandle,
          &pL_CamConf,
          kCAM_COMMAND_RESET);

          if (eL_CamStatus != kCAM_STATUS_OK)
          {
            /* -----> Incident carte ---------------------------------------- */
            eL_ActysStatus = kACTYS_STATUS_ERREUR_CAM;
            /* <----- Incident carte ---------------------------------------- */
          }
          /* <***** Mise hors tension / Remise sous tension carte *********** */

          if (eL_ActysStatus == kACTYS_STATUS_OK)
          {
            /* +++++> Carte sous tension ++++++++++++++++++++++++++++++++++++ */
            eL_ActysStatus = ACTYS_Build(&xL_ACTYSHandle,
                                pL_CamConf);
            /* <+++++ Carte sous tension ++++++++++++++++++++++++++++++++++++ */
          }

          pL_ActysInfoCarte->xFromServer.lListEtape    = 0;

//DEBUG: liste etape ACTYS
#if DEBUG
pL_ActysInfoCarte->xFromServer.lListEtape   = kACTYS_INTERNAL_AUTHENT;
pL_ActysInfoCarte->xFromServer.lListEtape  |= kACTYS_CHECK_BLOCAGE;
pL_ActysInfoCarte->xFromServer.lListEtape  |= kACTYS_READ_C_FILE;
pL_ActysInfoCarte->xFromServer.lListEtape  |= kACTYS_CHECK_MIRROR;

pL_ActysInfoCarte->xFromServer.lListEtape  |= kACTYS_EXTERNAL_AUTHENT;
pL_ActysInfoCarte->xFromServer.lListEtape  |= kACTYS_CUSTOMER_REFERENCE;
pL_ActysInfoCarte->xFromServer.lListEtape  |= kACTYS_WRITE_C_FILE;
pL_ActysInfoCarte->xFromServer.iCodeLangue=0x0250;
pL_ActysInfoCarte->xFromServer.uc_FlagTest=1;
//memset(pL_ActysInfoCarte->aucDataC3,0xbb,10);
//pL_ActysInfoCarte->puc_DataFromServer=OS_Malloc(500);
//memset(pL_ActysInfoCarte->puc_DataFromServer,0xaa,10);
#endif

          if (eL_ActysStatus == kACTYS_STATUS_OK)
          {
            /* -----> Application candidate --------------------------------- */
            pL_ActysInfoCarte->xToServer.lEtapesExecute |= kACTYS_BUILD;
            /* <----- Application candidate --------------------------------- */
          }

          cl_IsContextToFree                         = 1;
          cl_IsCurrentApplication                    = 0;
        }
        /* ****************************************************************** */
        /* *************** INTERNAL AUTHENTICATE         ******************** */
        /* ****************************************************************** */
        else 
        if ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_INTERNAL_AUTHENT) != 0)
        {
          //Carte a puce
          uc_flag_puce=1;

          cl_IsCurrentApplication = 1;

          /* *****> Mise hors tension / Remise sous tension carte *********** */
          eL_CamStatus = CAM_SetCommand(&xL_ACTYSHandle,
                              &pL_CamConf,
                               kCAM_COMMAND_RESET);

          if (eL_CamStatus != kCAM_STATUS_OK)
          {
            /* -----> Incident carte ---------------------------------------- */
            eL_ActysStatus = kACTYS_STATUS_ERREUR_CAM;
            /* <----- Incident carte ---------------------------------------- */
          }
          /* <***** Mise hors tension / Remise sous tension carte *********** */

          if (eL_ActysStatus == kACTYS_STATUS_OK)
          {
            /* *****> Recup Numero CSN ********************* */
            /* Sélection du Master File */
            auc_AdresseFichier[0] = 0x3F;
            auc_AdresseFichier[1] = 0x00;
            eL_ActysStatus =  ACTYS_SelectFile(&xL_ACTYSHandle,
                                    pL_CamConf,
                                    kSelectMasterFile,
                                    2,
                                    auc_AdresseFichier);

            /* Sélection du DF System */
            auc_AdresseFichier[0] = 0x01;
            auc_AdresseFichier[1] = 0x00;
            eL_ActysStatus =  ACTYS_SelectFile(&xL_ACTYSHandle,
                                    pL_CamConf,
                                    kSelectDirectoryFile,
                                    2,
                                    auc_AdresseFichier);

            /* Lecture du numéro de série de la carte CSN */
            auc_AdresseFichier[0] = 0x81; /* Sélection du fichier de SFI 1 */
            auc_AdresseFichier[1] = 0x00; /* Offset 0 */
            auc_AdresseFichier[2] = 0x08; /* Lecture des 8 premiers octets */

            eL_ActysStatus = ACTYS_ReadBinary( &xL_ACTYSHandle,
                                    pL_CamConf,
                                    auc_AdresseFichier,
                                    &pL_ActysInfoCarte->aucCSN[1],
                                    &i);
            pL_ActysInfoCarte->aucCSN[0] = i;
            /* <***** Recup Numero CSN ********************* */

            /* *****> Deplacement jusqu'a EFkey *********** */
            eL_CamStatus = CAM_SetCommand(&xL_ACTYSHandle,
                                &pL_CamConf,
                                kCAM_COMMAND_RESET);

            // Select Master File
            auc_AdresseFichier[0]=0x3F;
            auc_AdresseFichier[1]=0x00;
            eL_ActysStatus = ACTYS_SelectFile(&xL_ACTYSHandle,
                                    pL_CamConf,
                                    kSelectMasterFile,
                                    2,
                                    auc_AdresseFichier);

            // Select Directory File
            auc_AdresseFichier[0]=0x03;
            auc_AdresseFichier[1]=0x00;
            eL_ActysStatus = ACTYS_SelectFile(&xL_ACTYSHandle,
                                    pL_CamConf,
                                    kSelectDirectoryFile,
                                    2,
                                    auc_AdresseFichier);

            // Select Elementary File
            auc_AdresseFichier[0]=0x03;
            auc_AdresseFichier[1]=0x07;
            eL_ActysStatus = ACTYS_SelectFile(&xL_ACTYSHandle,
                                    pL_CamConf,
                                    kSelectFile,
                                    2,
                                    auc_AdresseFichier);
            /* <***** Deplacement jusqu'a EFkey *********** */

            // Generation d'un nb aleatoire
            if(eL_ActysStatus == kACTYS_STATUS_OK)
            {
              pL_ActysInfoCarte->puc_Trnd=API_UTILS_GenerateRandomChar(LG_KEY);
                  }

            /* *****> Demande d'authentification *********** */
            if(eL_ActysStatus == kACTYS_STATUS_OK)
            {
              eL_ActysStatus = ACTYS_SelectFileKey(&xL_ACTYSHandle,
                                        pL_CamConf,
                                        pL_ActysInfoCarte->puc_Trnd,
                                        &pL_DataOut);
            }

            if(eL_ActysStatus == kACTYS_STATUS_OK)
            {
              pL_Crypto_LSB_C = OS_Malloc(4);
              puc_MKgr = OS_Malloc(8);
              puc_Kgr = OS_Malloc(8);
              tucKeyLeft = OS_Malloc(8);
              tucKeyRight = OS_Malloc(8);
              pL_Crypto_T = OS_Malloc(8);
              
              if( pL_Crypto_T == NULL)
              {
                OS_Free(puc_MKgr);
                OS_Free(puc_Kgr);
                OS_Free(tucKeyLeft);
                OS_Free(tucKeyRight);
                OS_Free(pL_Crypto_LSB_C);
                
                eL_ActysStatus = kACTYS_STATUS_OUT_OF_MEMORY;
              }
              
              // Copie Crnd
              UTS_memcpy(pL_Crnd,pL_DataOut+5,8);
              
              // Copie Crypto LSB carte
              UTS_memcpy(pL_Crypto_LSB_C,pL_DataOut+1,4);
              
              OS_Free(pL_DataOut);
              
              us_erreur = sReadKey(MKgr, tucKeyLeft, tucKeyRight);
              
              // TEST CARTE REELLE ou TEST
              if(pL_ActysInfoCarte->xFromServer.uc_FlagTest == 0)
                UTS_memcpy2(auc_CRYPTO_Kgr,cauc_CRYPTO_MKGR,LG_KEY); //carte reelle
              else
                UTS_memcpy2(auc_CRYPTO_Kgr,cauc_CRYPTO_MKGR_TEST,LG_KEY); //carte test
              


              CHIF_TripleDES8 (  auc_CRYPTO_Kgr,puc_MKgr,tucKeyLeft,tucKeyRight,DES_DECRYPT);
              
              us_erreur = CHIF_DES8 (  &pL_ActysInfoCarte->aucCSN[1],puc_Kgr,puc_MKgr,DES_ENCRYPT);
              
              if( us_erreur == 0 )
              {
                us_erreur = CHIF_DES8 ( pL_Crnd,pL_ActysInfoCarte->auc_TKgr,puc_Kgr,DES_ENCRYPT);
              }
              
              if( us_erreur == 0 )
              {
                // Calcul Crypto terminal
                us_erreur = CHIF_DES8 ( pL_ActysInfoCarte->puc_Trnd,pL_Crypto_T,pL_ActysInfoCarte->auc_TKgr,DES_ENCRYPT);
                
                if( us_erreur != 0 )
                  eL_ActysStatus = kACTYS_STATUS_ERROR_DES;
              }
              else
                eL_ActysStatus = kACTYS_STATUS_ERROR_DES;
              
              // Verification Crypto
              if(   ( pL_Crypto_T[4] == pL_Crypto_LSB_C[0] )
                && ( pL_Crypto_T[5] == pL_Crypto_LSB_C[1] )
                && ( pL_Crypto_T[6] == pL_Crypto_LSB_C[2] )
                && ( pL_Crypto_T[7] == pL_Crypto_LSB_C[3] ) )
                eL_ActysStatus = kACTYS_STATUS_OK;
              else
                eL_ActysStatus = kACTYS_STATUS_ERROR_INTERNAL_AUTHENT;
              
              OS_Free(puc_MKgr);
              OS_Free(puc_Kgr);
              OS_Free(tucKeyLeft);
              OS_Free(tucKeyRight);
              OS_Free(pL_Crypto_T);
              OS_Free(pL_Crypto_LSB_C);
            }
            /* <***** Demande d'authentification *********** */
               }
//DEBUG: trace resultat Internal authent
#if DEBUG
for(i=0;i<3;i++)
   API_UTILS_SendOnUART0(0xa0);
API_UTILS_SendOnUART0(eL_ActysStatus);
#endif

           ACTYS_EtapeExecute(pL_ActysInfoCarte,
                              eL_ActysStatus,
                              kACTYS_INTERNAL_AUTHENT);
        }
      /* ****************************************************************** */
      /* *************** CONTROLE BLOCAGE              ******************** */
      /* ****************************************************************** */
      else if ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_CHECK_BLOCAGE) != 0)
      {
           /* *****> Deplacement jusqu'a EFSecret Code *********** */
           // Select Elementary File suffisant vu que Check_Blocage
           // indissociable de Internal_Authenticate
           auc_AdresseFichier[0]=0x03;
           auc_AdresseFichier[1]=0x09;

           eL_ActysStatus = ACTYS_SelectFile(&xL_ACTYSHandle,
                                              pL_CamConf,
                                              kSelectFile,
                                              2,
                                              auc_AdresseFichier);
           /* <***** Deplacement jusqu'a EFSecret Code *********** */

           if( eL_ActysStatus ==  kACTYS_STATUS_OK )
           {
              // GetInfo pour gestion du nb d'essai de PIN
              eL_ActysStatus = ACTYS_GetInfo(&xL_ACTYSHandle,
                                              pL_CamConf,
                                              pL_ActysInfoCarte);

              // Controle Nb Essai Code PIN

                /*************************A Verifier sur le terrain**********************************/
                switch(pL_ActysInfoCarte->aucInfo[6])
          {
              case 0x00:
                pL_ActysInfoCarte->xToServer.ui_PinTryCounter = 3;
              break;
              case 0x01:
                pL_ActysInfoCarte->xToServer.ui_PinTryCounter = 2;
              break;
              case 0x03:
                pL_ActysInfoCarte->xToServer.ui_PinTryCounter = 1;
              break;
              case 0x07:
                pL_ActysInfoCarte->xToServer.ui_PinTryCounter = 0;
              break;
          }
                /*************************A Verifier sur le terrain**********************************/

              // 3 essais rates ??
              if ( pL_ActysInfoCarte->xToServer.ui_PinTryCounter == 0 )
              {
               eL_Status = ACTYS_AfficheMessage(  &xL_ACTYSHandle,
                                              1,
                                            2,
                                            pL_ActysInfoCarte->xFromServer.iCodeLangue,
                                          kACTYS_MESS_APPLI_BLOQUEE);

              TEMPO_XMS(&xL_ACTYSHandle, 1500);

               if( eL_Status == kACTYS_STATUS_OK)
                     eL_ActysStatus = kACTYS_STATUS_ERROR_CARTE_BLOQUEE;
                 else
              eL_ActysStatus = eL_Status;
              }
              else
                 eL_ActysStatus = kACTYS_STATUS_OK;
           }
//DEBUG: trace resultat check blocage
#if DEBUG
for(i=0;i<3;i++)
   API_UTILS_SendOnUART0(0xa1);
API_UTILS_SendOnUART0(eL_ActysStatus);
API_UTILS_SendOnUART0(pL_ActysInfoCarte->xToServer.ui_PinTryCounter);
#endif
           ACTYS_EtapeExecute(pL_ActysInfoCarte,
                              eL_ActysStatus,
                              kACTYS_CHECK_BLOCAGE);
      }
      /* ****************************************************************** */
      /* *************** LECTURE C FILE                ******************** */
      /* ****************************************************************** */
      else if ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_READ_C_FILE) != 0)
      {
        /* =====> INITIALISATION DU CONTEXTE DE REPONSE ======================= */
        ACTYS_Free(&pL_ActysInfoCarte->xC1FiletoServer.pC1FileToServer);
        ACTYS_Free(&pL_ActysInfoCarte->xC2FiletoServer.pC2FileToServer);
        ACTYS_Free(&pL_ActysInfoCarte->xC3FiletoServer.pC3FileToServer);

        pL_ActysInfoCarte->xC1FiletoServer.iLgC1FileToServer = 0 ;
        pL_ActysInfoCarte->xC2FiletoServer.iLgC2FileToServer = 0 ;
        pL_ActysInfoCarte->xC3FiletoServer.iLgC3FileToServer = 0 ;

        pL_ActysInfoCarte->xC1FiletoServer.pC1FileToServer   = (unsigned char*) OS_Malloc(LG_C1_FILE);
        pL_ActysInfoCarte->xC2FiletoServer.pC2FileToServer   = (unsigned char*) OS_Malloc(LG_C2_FILE);
        pL_ActysInfoCarte->xC3FiletoServer.pC3FileToServer   = (unsigned char*) OS_Malloc(LG_C3_FILE);

        if ( (pL_ActysInfoCarte->xC1FiletoServer.pC1FileToServer == NULL)
          ||(pL_ActysInfoCarte->xC2FiletoServer.pC2FileToServer == NULL)
          ||(pL_ActysInfoCarte->xC3FiletoServer.pC3FileToServer == NULL) )
        {
          /* -----> Espace memoire insuffisant -------------------------------- */
          eL_ActysStatus = kACTYS_STATUS_OUT_OF_MEMORY;
          /* <----- Espace memoire insuffisant -------------------------------- */
        }
        /* <===== INITIALISATION DU CONTEXTE DE REPONSE ======================= */

        /* ***************> Lecture C1 File ********************* */
        if(eL_ActysStatus == kACTYS_STATUS_OK)
        {
          eL_ActysStatus = ACTYS_Read_File(&xL_ACTYSHandle,
                                 pL_CamConf,
                                 1,
                                 pL_ActysInfoCarte->xC1FiletoServer.pC1FileToServer,
                                 &pL_ActysInfoCarte->xC1FiletoServer.iLgC1FileToServer);

          if(eL_ActysStatus == kACTYS_STATUS_OK)
          {
            //Copie Piste ISO2 en local
            sConvBcd2Asc(pL_ActysInfoCarte->xC1FiletoServer.pC1FileToServer,
                     pL_ActysInfoCarte->aucISO2,
                     LG_ISO2,
                     CONV_LEFT_JUST);

            //Affectation Type PIN
            pL_ActysInfoCarte->uc_TypePIN = pL_ActysInfoCarte->aucISO2[OFFSET_TYPE_PIN];
          }
        }
        /* <*************** Lecture C1 File ********************* */

        /* ***************> Lecture C2 File ********************* */
        if(eL_ActysStatus == kACTYS_STATUS_OK)
        {
          eL_ActysStatus = ACTYS_Read_File(&xL_ACTYSHandle,
                                 pL_CamConf,
                                 2,
                                 pL_ActysInfoCarte->xC2FiletoServer.pC2FileToServer,
                                &pL_ActysInfoCarte->xC2FiletoServer.iLgC2FileToServer);

          // Parametrage en cours ?
          if( (pL_ActysInfoCarte->xC2FiletoServer.pC2FileToServer[0] & 0xF0) == 0x10)
             eL_ActysStatus = kACTYS_STATUS_ERROR_PARAMETRAGE_EN_COURS;

        }
        /* <*************** Lecture C2 File ********************* */

        /* ***************> Lecture C3 File ********************* */
        if(eL_ActysStatus == kACTYS_STATUS_OK)
        {
          eL_ActysStatus = ACTYS_Read_File(&xL_ACTYSHandle,
                                 pL_CamConf,
                                 3,
                                 pL_ActysInfoCarte->xC3FiletoServer.pC3FileToServer,
                                &pL_ActysInfoCarte->xC3FiletoServer.iLgC3FileToServer);

          if(eL_ActysStatus == kACTYS_STATUS_OK)
          {
            //Affectation locale de C3
            UTS_memcpy(  pL_ActysInfoCarte->aucDataC3,
                    pL_ActysInfoCarte->xC3FiletoServer.pC3FileToServer,
                    LG_C3_FILE);
          }
        }
        /* <*************** Lecture C3 File ********************* */

        /* ***************> Lecture C3bis File ****************** */
        if(eL_ActysStatus == kACTYS_STATUS_OK)
        {
          eL_ActysStatus = ACTYS_Read_File(&xL_ACTYSHandle,
                                 pL_CamConf,
                                 4,
                                 pL_ActysInfoCarte->aucDataC3bis,
                                 pL_Tampon);
        }
        /* <*************** Lecture C3bis File ****************** */

#if DEBUG
//DEBUG : trace resultat read c file
for(i=0;i<3;i++)
   API_UTILS_SendOnUART0(0xa2);
API_UTILS_SendOnUART0(eL_ActysStatus);
//API_UTILS_SendOnUART0(pL_ActysInfoCarte->uc_TypePIN);
//pL_ActysInfoCarte->xFromServer.ui_PinTryCounter=3;

/*
IntToTLV(pL,pL_ActysInfoCarte->xC1FiletoServer.iLgC1FileToServer);
API_UTILS_SendOnUART0(pL[0]);
API_UTILS_SendOnUART0(pL[1]);
IntToTLV(pL,pL_ActysInfoCarte->xC2FiletoServer.iLgC2FileToServer);
API_UTILS_SendOnUART0(pL[0]);
API_UTILS_SendOnUART0(pL[1]);
IntToTLV(pL,pL_ActysInfoCarte->xC3FiletoServer.iLgC3FileToServer);
API_UTILS_SendOnUART0(pL[0]);
API_UTILS_SendOnUART0(pL[1]);
*/
//for(i=0;i<40;i++)
//   API_UTILS_SendOnUART0(pL_ActysInfoCarte->aucISO2[i]);
//for(i=0;i<3;i++)
//   API_UTILS_SendOnUART0(0xa2);
//for(i=0;i<LG_C2_FILE;i++)
//   API_UTILS_SendOnUART0(pL_ActysInfoCarte->xC2FiletoServer.pC2FileToServer[i]);
//for(i=0;i<3;i++)
//   API_UTILS_SendOnUART0(0xaa);
//for(i=0;i<50;i++)
//   API_UTILS_SendOnUART0(pL_ActysInfoCarte->aucDataC3[i]);
//for(i=0;i<3;i++)
//   API_UTILS_SendOnUART0(0xbb);
//for(i=0;i<50;i++)
//   API_UTILS_SendOnUART0(pL_ActysInfoCarte->aucDataC3bis[i]);
#endif

        ACTYS_EtapeExecute(  pL_ActysInfoCarte,
                      eL_ActysStatus,
                      kACTYS_READ_C_FILE);
      }
        /* ****************************************************************** */
        /* *****************  VERIFICATION DU MIROIR C3bis    *************** */
        /* ****************************************************************** */
        else if ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_CHECK_MIRROR) != 0)
        {
           eL_ActysStatus = ACTYS_Check_Mirror (&xL_ACTYSHandle, pL_CamConf, pL_ActysInfoCarte, pL_ActysInfoCarte->auc_TKgr);

//DEBUG: trace resultat miroir
#if DEBUG
for(i=0;i<3;i++)
   API_UTILS_SendOnUART0(0xa3);
API_UTILS_SendOnUART0(eL_ActysStatus);
#endif

           ACTYS_EtapeExecute(pL_ActysInfoCarte,
                              eL_ActysStatus,
                              kACTYS_CHECK_MIRROR);
  }
        /* ****************************************************************** */
        /* *******************  EXTERNAL AUTHENTICATE    ******************** */
        /* ****************************************************************** */
        else if ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_EXTERNAL_AUTHENT) != 0)
        {
           eL_ActysStatus = ACTYS_VerifyPin (&xL_ACTYSHandle, &pL_CamConf,pL_ActysInfoCarte);

#if DEBUG
//DEBUG : trace resultat external authent
for(i=0;i<3;i++)
   API_UTILS_SendOnUART0(0xa4);
API_UTILS_SendOnUART0(eL_ActysStatus);
#endif

           ACTYS_EtapeExecute(pL_ActysInfoCarte,
                              eL_ActysStatus,
                              kACTYS_EXTERNAL_AUTHENT);
      }
      /* ****************************************************************** */
      /* ***************    SAISIE REFERENCE CLIENT          ************** */
      /* ****************************************************************** */
      else if ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_CUSTOMER_REFERENCE) != 0)
      {

            eL_ActysStatus = ACTYS_Saisie_Ref_Client(uc_flag_puce,&xL_ACTYSHandle,pL_ActysInfoCarte);

#if DEBUG
//DEBUG : trace resultat external authent
for(i=0;i<3;i++)
   API_UTILS_SendOnUART0(0xa5);
API_UTILS_SendOnUART0(eL_ActysStatus);
#endif

        ACTYS_EtapeExecute(  pL_ActysInfoCarte,
                      eL_ActysStatus,
                      kACTYS_CUSTOMER_REFERENCE);
      }

        /* ****************************************************************** */
        /* ****************  ECRITURE DU FICHIER C3 et C3bis    ************* */
        /* ****************************************************************** */
      else if ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_WRITE_C_FILE) != 0)
        {
//DEBUG: affectation en dur des donnees serveur
#if DEBUG
pL_ActysInfoCarte->puc_DataFromServer = OS_Malloc(145);
UTS_memcpy2(pL_ActysInfoCarte->puc_DataFromServer,pL_BufDebug,145);
#endif

#if !DEBUG
           eL_ActysStatus = GetExternalAllocatedArea(pL_MapMem,
                                                     0xDFE504,
                                                     &(pL_ActysInfoCarte->puc_DataFromServer));

#endif
           if (eL_ActysStatus == kACTYS_STATUS_OK)
           {
              eL_ActysStatus = ACTYS_Write_C3 (&xL_ACTYSHandle,
                                          pL_CamConf,
                                          pL_ActysInfoCarte,
                                                 pL_ActysInfoCarte->auc_TKgr);
           }
#if !DEBUG
           DeAllocateAnswerArea(pL_MapMem,
                                0xDFE504,
                                &(pL_ActysInfoCarte->puc_DataFromServer));
#endif

#if DEBUG
//DEBUG : trace resultat ecriture C3
for(i=0;i<3;i++)
   API_UTILS_SendOnUART0(0xa7);
API_UTILS_SendOnUART0(eL_ActysStatus);
#endif

           ACTYS_EtapeExecute(pL_ActysInfoCarte,
                              eL_ActysStatus,
                              kACTYS_WRITE_C_FILE);
        }
          /* ****************************************************************** */
          /* *************** CALCUL PVV CARTE A PISTE      ******************** */
          /* ****************************************************************** */
      else if ((pL_ActysInfoCarte->xFromServer.lListEtape & kACTYS_CHECK_PVV_TRACK_CARD) != 0)
          {
             eL_ActysStatus = GetExternalAllocatedArea(pL_MapMem,
                                                     0xDFE504,
                                                     &(pL_ActysInfoCarte->puc_DataFromServer));

             //Affectation nb Essai deja realise dans la journee
             pL_ActysInfoCarte->xToServer.ui_PinTryCounter = pL_ActysInfoCarte->xFromServer.ui_PinTryCounter;

             // DKV/UTA
             pL_ActysInfoCarte->uc_TypePIN = '0';

            //Init Flag
            uc_flag_break = 0;

//DEBUG: lecture Piste sur WYMIX
#if DEBUG

pL_Affichage=OS_Malloc(17);
memcpy(pL_Affichage,
       kACTYS_AFF_RETRAIT_CARTE,
       sizeof(kACTYS_AFF_RETRAIT_CARTE));
ACTYS_Affiche(&xL_ACTYSHandle, 1, 2, pL_Affichage, 16) ;
CAM_RetraitCarte (&xL_ACTYSHandle);

memcpy(pL_Affichage,
       kACTYS_AFF_INSERTION_CARTE,
       sizeof(kACTYS_AFF_INSERTION_CARTE));
ACTYS_Affiche(&xL_ACTYSHandle, 1, 2, pL_Affichage, 16) ;
CAM_InsertionCarte (&xL_ACTYSHandle);
// +++++> Lecture Piste ++++++++++++++++++++++++++++++++++++++++++++++++

memcpy(pL_Affichage,
       kACTYS_AFF_LECTURE_PISTE,
       sizeof(kACTYS_AFF_LECTURE_PISTE));
ACTYS_Affiche(&xL_ACTYSHandle, 1, 2, pL_Affichage, 16) ;
ACTYS_Free(&pL_Affichage);
// <+++++ Lecture Piste ISO2 ++++++++++++++++++++++++++++++++++++++++++++++++
cL_ResultOS = CAP_AttenteCarte (&xL_ACTYSHandle,
                                10000,
                               pL_ActysInfoCarte->aucISO2);
if(cL_ResultOS == TRUE)
{
   beep();
   beep();
   beep();
   beep();
   beep();
   beep();
   UTS_memcpy(pL_ActysInfoCarte->puc_DataFromServer,pL_ActysInfoCarte->aucISO2,40);

   //MODIF Date expiration pour anomalie 347
   if( (pL_ActysInfoCarte->xFromServer.ui_TrackCardType == DKV_CLASSIC_CARD) )
   {
     //modif date pour carte test perimee 9538
     if( pL_ActysInfoCarte->puc_DataFromServer[18] == '4' )
     {
       pL_ActysInfoCarte->puc_DataFromServer[17]='0';
       pL_ActysInfoCarte->puc_DataFromServer[18]='5';
       pL_ActysInfoCarte->puc_DataFromServer[19]='0';
       pL_ActysInfoCarte->puc_DataFromServer[20]='1';
     }
   }

//   //DEBUG : trace piste ISO2
//   for(i=0;i<3;i++)
//      API_UTILS_SendOnUART0(0xa8);
//   for(i=0;i<40;i++)
//      API_UTILS_SendOnUART0(pL_ActysInfoCarte->puc_DataFromServer[i]);
}
#endif


             do
             {
                //---- Carte Eurotrafic/GR ---//
                if(pL_ActysInfoCarte->xFromServer.ui_TrackCardType == GR_EUROTRAFIC_CARD)
                {
                   //Transfert ISO2 dans variable locale sans signe de depart
                   UTS_memcpy(pL_ActysInfoCarte->aucISO2,&pL_ActysInfoCarte->puc_DataFromServer[1],LG_ISO2);

            pL_ActysInfoCarte->uc_TypePIN = pL_ActysInfoCarte->aucISO2[OFFSET_TYPE_PIN];

                   // saisie + verification pin
                   eL_ActysStatus = ACTYS_CheckPINSaisie (&xL_ACTYSHandle,
                                                         pL_ActysInfoCarte,
                                                         auc_unused,
                                                         NULL);

             // pas de pin a taper mais PVV incorrect...
             // piste ISO2 erronee
             if( ( (eL_ActysStatus != kACTYS_STATUS_OK)
                &&  (pL_ActysInfoCarte->uc_TypePIN == '9'))
                ||( eL_ActysStatus == kACTYS_STATUS_ERROR_ISO2 ))
             {
                //pas de nouvel essai possible
                pL_ActysInfoCarte->xToServer.ui_PinTryCounter=0;

                /* +++++> Carte Invalide ++++++++++++++++++++++++++++++++++++++++++++ */
                   eL_Status = ACTYS_AfficheMessage(&xL_ACTYSHandle,
                                                1,
                                              2,
                                              pL_ActysInfoCarte->xFromServer.iCodeLangue,
                                            kACTYS_MESS_PAIEMENT_REFUSE);

                  TEMPO_XMS(&xL_ACTYSHandle, 1500);

                   if( eL_Status != kACTYS_STATUS_OK)
                         eL_ActysStatus = eL_Status;

                    /* <+++++ Carte Invalide ++++++++++++++++++++++++++++++++++++++++++++ */
             }
                }
                //---- Carte DKV ----//
                else if( (pL_ActysInfoCarte->xFromServer.ui_TrackCardType == DKV_CLASSIC_CARD)
                     || (pL_ActysInfoCarte->xFromServer.ui_TrackCardType == DKV_CLASSIC_CARD_AND_DRIVER_CODE)
                     || (pL_ActysInfoCarte->xFromServer.ui_TrackCardType == DKV_SELECTION_CARD) )
                {
                   // saisie + verification pin
                   eL_ActysStatus = DKV_CheckPINSaisie (&xL_ACTYSHandle,
                                                        pL_ActysInfoCarte);
              }
               //---- Carte UTA ----//
                else if(pL_ActysInfoCarte->xFromServer.ui_TrackCardType == UTA_CARD)
                {
                   // saisie + verification pin
             eL_ActysStatus = UTA_CheckPINSaisie (&xL_ACTYSHandle,
                                                   pL_ActysInfoCarte);
                }
               //---- Carte AVIA ----//
                else if(pL_ActysInfoCarte->xFromServer.ui_TrackCardType == AVIA_CARD)
                {
                   // saisie + verification pin
             eL_ActysStatus = AVIA_CheckPINSaisie (&xL_ACTYSHandle,
                                                   pL_ActysInfoCarte);
                }

                //---- Carte ARIS ----//
                else if(pL_ActysInfoCarte->xFromServer.ui_TrackCardType == ARIS_CARD)
                {
                   // saisie + verification pin
             eL_ActysStatus = ARIS_CheckPINSaisie (&xL_ACTYSHandle,
                                                   pL_ActysInfoCarte);
                }
                //---- Carte BICA ----//
                else if(pL_ActysInfoCarte->xFromServer.ui_TrackCardType == BICA_CARD)
                {
                   // saisie + verification pin
              eL_ActysStatus = BICA_CheckPINSaisie (&xL_ACTYSHandle,
                                                   pL_ActysInfoCarte);
                }
				//---- Carte LOMO ----//
                else if(pL_ActysInfoCarte->xFromServer.ui_TrackCardType == LOMO_CARD)
                {
                   // saisie + verification pin
              eL_ActysStatus = LOMO_CheckPINSaisie (&xL_ACTYSHandle,
                                                   pL_ActysInfoCarte);
                }
				 //---- Carte CSC ----//
                else if(pL_ActysInfoCarte->xFromServer.ui_TrackCardType == CSC_CARD)
                {
                   // saisie + verification pin
              eL_ActysStatus = CSC_CheckPINSaisie (&xL_ACTYSHandle,
                                                   pL_ActysInfoCarte);
                }
                else //pb... type carte erronee
                {
                  eL_ActysStatus = kACTYS_STATUS_ERREUR_TYPE_CARTE_PISTE;
                }


                if(eL_ActysStatus == kACTYS_STATUS_OK)
                {
                   /* +++++> Code correct ++++++++++++++++++++++++++++++++++++++++++++++++ */
                 eL_Status = ACTYS_AfficheMessage(&xL_ACTYSHandle,
                                              1,
                                           2,
                                           pL_ActysInfoCarte->xFromServer.iCodeLangue,
                                         kACTYS_MESS_CODE_BON);

              TEMPO_XMS(&xL_ACTYSHandle, 1500);

                 if( eL_Status != kACTYS_STATUS_OK)
                     eL_ActysStatus = eL_Status;

                   // Remise a jour du compteur d'essai
                   pL_ActysInfoCarte->xToServer.ui_PinTryCounter = 3;
                   /* <+++++ Code correct ++++++++++++++++++++++++++++++++++++++++++++++++ */
                }
                else if(   (pL_ActysInfoCarte->xToServer.ui_PinTryCounter > 0)
                        && (eL_ActysStatus == kACTYS_STATUS_ERROR_PVV) )
                {
                   /* +++++> Code faux ++++++++++++++++++++++++++++++++++++++++++++++++ */
                 eL_Status = ACTYS_AfficheMessage(&xL_ACTYSHandle,
                                              1,
                                           2,
                                           pL_ActysInfoCarte->xFromServer.iCodeLangue,
                                         kACTYS_MESS_CODE_FAUX);

               TEMPO_XMS(&xL_ACTYSHandle, 1500);

                 if( eL_Status != kACTYS_STATUS_OK)
                     eL_ActysStatus = eL_Status;
                   /* <+++++ Code faux ++++++++++++++++++++++++++++++++++++++++++++++++ */

                   pL_ActysInfoCarte->xToServer.ui_PinTryCounter--;

                   if( pL_ActysInfoCarte->xToServer.ui_PinTryCounter == 0)
                   {
                      /* +++++> Carte Bloquee ++++++++++++++++++++++++++++++++++++++++++++ */
                   eL_Status = ACTYS_AfficheMessage(&xL_ACTYSHandle,
                                                1,
                                             2,
                                             pL_ActysInfoCarte->xFromServer.iCodeLangue,
                                           kACTYS_MESS_3_CODES_FAUX);

                  TEMPO_XMS(&xL_ACTYSHandle, 1500);

                   if( eL_Status != kACTYS_STATUS_OK)
                       eL_ActysStatus = eL_Status;
                     else
                        eL_ActysStatus = kACTYS_STATUS_ERROR_CARTE_BLOQUEE;
                      /* +++++> Carte Bloquee ++++++++++++++++++++++++++++++++++++++++++++ */
            }
                }
                else
                  uc_flag_break=1;
             }
             while(   ( eL_ActysStatus != kACTYS_STATUS_OK )
                   && ( pL_ActysInfoCarte->xToServer.ui_PinTryCounter != 0 )
                   && ( uc_flag_break == 0 ) );

           DeAllocateAnswerArea(pL_MapMem,
                                0xDFE504,
                                &(pL_ActysInfoCarte->puc_DataFromServer));

           ACTYS_EtapeExecute(pL_ActysInfoCarte,
                              eL_ActysStatus,
                              kACTYS_CHECK_PVV_TRACK_CARD);
  }
        /* <***** Realisation *********************************************** */

        /* *****> Mise a jour de l'etat du coupleur CAM ********************* */
        CAM_RefreshStat(&pL_CamConf);

        // Controle carte arrachee uniquement pour carte actys
        if ((((pL_CamConf->iState & kCAM_STATE_REMOVED) != 0)
         || ((pL_CamConf->iState & kCAM_STATE_ENDCOURSE) == 0))
         && ( uc_flag_puce == 1 ))
        {
          /* -----> Carte arrachee ---------------------------------------- */
          eL_ActysStatus = kACTYS_STATUS_CARTE_ARRACHEE;

          pL_ActysInfoCarte->xFromServer.lListEtape = 0;
          /* <----- Carte arrachee ---------------------------------------- */
        }
        /* <***** Mise a jour de l'etat du coupleur CAM ********************* */

      }

      /* <===== REALISATION DES ACTIONS REQUISES ============================ */


      /* =====> GESTION DE LA REPONSE ======================================= */
      if (eL_ActysStatus == kACTYS_STATUS_CARTE_ARRACHEE)
      {
        /* -----> Carte arrachee au cours des traitements ------------------- */
         eL_Status = ACTYS_AfficheMessage(&xL_ACTYSHandle,
                                     1,
                                2,
                                   pL_ActysInfoCarte->xFromServer.iCodeLangue,
                                kACTYS_MESS_CARTE_ARRACHEE);

        TEMPO_XMS(&xL_ACTYSHandle, 1500);

         if( eL_Status != kACTYS_STATUS_OK)
           eL_ActysStatus = eL_Status;
        /* <----- Carte arrachee au cours des traitements ------------------- */
      }

      /* *****> Delivrance du resultat ************************************** */
      if (pL_Message[MS_TAG] == 1)
      {
        /* +++++> Message encode avec des TLV +++++++++++++++++++++++++++++++ */
        pL_TLVMessageReponse = TLV_PrepareReponse(&pL_Message[MS_DebutMessage],
                                                  iL_LongueurRecue,
                                                  pL_MapMem,
                                                  iL_CodeCde);

      /* =====> INITIALISATION DU CONTEXTE DE REPONSE ======================= */
      if( (pL_ActysInfoCarte->xToServer.lEtapesExecute & kACTYS_READ_C_FILE) != 0)
      {
        ACTYS_Free(&pL_ActysInfoCarte->xC1FiletoServer.pC1FileToServer);
        ACTYS_Free(&pL_ActysInfoCarte->xC2FiletoServer.pC2FileToServer);
        ACTYS_Free(&pL_ActysInfoCarte->xC3FiletoServer.pC3FileToServer);
      }
      /* <===== INITIALISATION DU CONTEXTE DE REPONSE ======================= */

        cL_ResultOS = SendMail(pL_Message[MS_Expediteur],
                               pL_Message[MS_Expediteur],
                               xL_ACTYSHandle.cId,
                               1,
                               pL_Message[MS_Chrono],
                               FREE_MEMORY_BEFORE_SENDING,
                               0,
                               pL_TLVMessageReponse->iLongueur,
                               &(pL_TLVMessageReponse->pData),
                               &xL_ACTYSHandle);
        TLV_LibereTLV(pL_TLVMessageReponse, 'w');
        /* <+++++ Message encode avec des TLV +++++++++++++++++++++++++++++++ */
      }
      else
      {
        /* +++++> Message non encode Commande / Etapes / &CAM_CONF ++++++++++ */
        switch (iL_CodeCde)
        {
          case kACTYS_CODE_CDE_GET_INFO:
            /* ************************************************************** */
            /* ***** FOURNITURE DES INFORMATIONS DE L'APPLICATION *********** */
            /* ************************************************************** */
            pL_MessageReponse    = ACTYS_RecupMessageInfo();
            pL_TLVMessageReponse = TLV_PrepareReponse(pL_MessageReponse,
                                                      36,
                                                      pL_MapMem,
                                                      iL_CodeCde);

            ACTYS_Free(&pL_MessageReponse);
            cL_ResultOS = SendMail(pL_Message[MS_Expediteur],
                                   pL_Message[MS_Expediteur],
                                   xL_ACTYSHandle.cId,
                                   0,
                                   pL_Message[MS_Chrono],
                                   FREE_MEMORY_BEFORE_SENDING,
                                   0,
                                   pL_TLVMessageReponse->iLongueur,
                                   &(pL_TLVMessageReponse->pData),
                                   &xL_ACTYSHandle);
            TLV_LibereTLV(pL_TLVMessageReponse, 'w');
            break;

          case kACTYS_CODE_CDE_EXECUTION:
            /* ************************************************************** */
            /* ***** EXECUTION DES COMMANDES ******************************** */
            /* ************************************************************** */
          default:
            if (pL_ActysInfoCarte->xToServer.lEtapesExecute == kACTYS_BUILD)
            {
              /*+++++> Construction de la liste de candidats +++++++++++++++*/
              iL_LgMessageReponse = sizeof(tL_AidActysTLV) + 2 ;
              pL_MessageReponse = (unsigned char*) OS_Malloc(iL_LgMessageReponse);
              UTS_memcpy(&pL_MessageReponse[2],
                         tL_AidActysTLV,
                         sizeof(tL_AidActysTLV));
               /*<+++++ Construction de la liste de candidats +++++++++++++++ */
            }
            else
            {
              /* +++++> Autre action ++++++++++++++++++++++++++++++++++++++++ */
              iL_LgMessageReponse = 2;
              pL_MessageReponse = (unsigned char*) OS_Malloc(iL_LgMessageReponse);  // Malloc de la longueur max
              /* <+++++ Autre action ++++++++++++++++++++++++++++++++++++++++ */
            }

            pL_Tampon  =  (unsigned int*) &pL_MessageReponse[0];
            *pL_Tampon = eL_ActysStatus;

            cL_ResultOS = SendMail(pL_Message[MS_Expediteur],
                                   pL_Message[MS_Expediteur],
                                   xL_ACTYSHandle.cId,
                                   0,
                                   pL_Message[MS_Chrono],
                                   FREE_MEMORY_BEFORE_SENDING,
                                   0,
                                   iL_LgMessageReponse,
                                   &pL_MessageReponse,
                                   &xL_ACTYSHandle);
            break;
        }
        /* <+++++ Message non encode Commande / Etapes / &CAM_CONF ++++++++++ */
      }
      /* <***** Delivrance du resultat ************************************** */
      /* <===== GESTION DE LA REPONSE ======================================= */

      /* *****> Liberation memoire ****************************************** */
      ACTYS_Free(&pL_Message);

      /* *****> Suppression du contexte de l'application ******************** */
      if ((cl_IsContextToFree != 0)
       && (pL_ActysInfoCarte != NULL))
       {
        // Liberation buffer fichiers Cx
        ACTYS_Free(&pL_ActysInfoCarte->xC1FiletoServer.pC1FileToServer);
        ACTYS_Free(&pL_ActysInfoCarte->xC2FiletoServer.pC2FileToServer);
        ACTYS_Free(&pL_ActysInfoCarte->xC3FiletoServer.pC3FileToServer);

        ACTYS_ResetInfoCarte(pL_ActysInfoCarte);

        /* +++++> Suppression requise +++++++++++++++++++++++++++++++++++++++ */
        OS_Free(pL_ActysInfoCarte);
        pL_ActysInfoCarte = NULL;

        TLV_RemoveMapping(&pL_MapMem, 0xDF8200);

        TLV_RemoveMapping(&pL_MapMem, 0xDFE401);

        TLV_RemoveMapping(&pL_MapMem, 0xDFE402);

        TLV_RemoveMapping(&pL_MapMem, 0xDFE504);

        TLV_RemoveMapping(&pL_MapMem, 0xDFE405);

        TLV_RemoveMapping(&pL_MapMem, 0xDFE406);

        TLV_RemoveMapping(&pL_MapMem, 0xDFE407);

        cl_IsCurrentApplication = 0;
  /* <+++++ Suppression requise +++++++++++++++++++++++++++++++++++++++ */
      }

      if ((cl_IsContextToFree != 0)
       && (xL_ActysInfo != NULL))
      {
        /* +++++> Suppression requise +++++++++++++++++++++++++++++++++++++++ */
        OS_Free(xL_ActysInfo);
        xL_ActysInfo = NULL;

        TLV_RemoveMapping(&pL_MapMem, 0xDFE403);

        cl_IsCurrentApplication = 0;
        /* <+++++ Suppression requise +++++++++++++++++++++++++++++++++++++++ */
      }

      cl_IsContextToFree              = 0;
      /* <***** Suppression du contexte de l'application ******************** */
      /* <+++++ Message recu sans erreur ++++++++++++++++++++++++++++++++++++ */
     }
    /* <===== ANALYSE DU MESSAGE RECU ======================================= */
  }
  /* <===== TACHE =========================================================== */
}

