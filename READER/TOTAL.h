/*==============================================================================
================================================================================
=====                        MODULE TOTAL TOTAL.H                          =====
================================================================================
================================================================================
===== PROJET     : TOTAL                                                   =====
=====              ***** DEFINITIONS POUR L'APPLICATION *****              =====
=====                                                                      =====
===== EDITEUR    : WYNID Technologies - 2004                               =====
================================================================================
===== EVOLUTIONS : | 1.00 | 22/11/2004 | Creation			   					=====
=====----------------------------------------------------------------------=====
=====					 | 1.10 | 18/03/2005 | Ajout parametre aucCodeChauff dans=====
=====					 |      |            | xStructTO_SERVER                  =====
==============================================================================*/


#ifndef _TOTAL_H_
#define _TOTAL_H_

/******************************************************************************/
/*                            Includes                                        */
/******************************************************************************/
#include "API_CAM.h"
#include "OS.h"

/******************************************************************************/
/*                          OPTIONS D'APPLICATIONS                            */
/******************************************************************************/
#define DEBUG 					0 			/* ---> pour affichage trace -------- */

#define BICA      			1        /* ---> Affiche Nom APPLI BICA/TOTAL- */

/******************************************************************************/
/*                          Cryptogrammes pour l'application                  */
/******************************************************************************/
// pour carte de test avec cles techniques bidons
//#define CRYPTO_KEY1     "\x4B\x5B\x7A\x8F\x86\xE1\x37\x25" // CK1: 00/01/../0f
//#define CRYPTO_KEY2     "\xE3\x1B\x87\x63\xC7\x70\x0F\x1B" // CK2: bb/bb/../bb
//#define CRYPTO_MKGR     "\x33\xDD\x0F\x45\xE4\x4B\xF1\xBF" // CK3: ee/ee/../ee
//#define CRYPTO_MDKV     "\x9F\x9F\x67\x1D\xC8\x93\xE0\x75" // CK5: 0f/0e/../00
//#define CRYPTO_MUTA     "\xC1\xB9\xDA\xA3\xFB\xDF\xE2\x06" // CK6: cc/cc/../cc

// pour carte de test
//#define CRYPTO_KEY1_TEST     "\xEA\x60\x0F\x35\x6F\x91\xC9\x3D"
//#define CRYPTO_KEY2_TEST     "\xC9\x5A\x9A\x75\x89\x21\x2C\x07"
//#define CRYPTO_MKGR_TEST     "\x61\x6C\x45\x03\x78\xE4\x75\x5E"
//#define CRYPTO_MDKV_TEST     "\xFB\x40\x32\x7C\x4C\x63\x94\x9F"
//UTA reelle
//	#define CRYPTO_MUTA     "\xD6\xF2\x2C\xE1\x74\xFA\x66\x1A"
//UTE test
//	#define CRYPTO_MUTA_TEST     "\x01\x63\xEF\x9C\x10\xF4\xCA\xBE"


// pour carte reelle
#define CRYPTO_KEY1     "\xEA\x60\x0F\x35\x6F\x91\xC9\x3D"
#define CRYPTO_KEY2     "\xC9\x5A\x9A\x75\x89\x21\x2C\x07"
#define CRYPTO_MDKV     "\xFB\x40\x32\x7C\x4C\x63\x94\x9F"

//#define CRYPTO_MUTA     "\xD6\xF2\x2C\xE1\x74\xFA\x66\x1A"
//#define CRYPTO_MUTA_TEST     "\x01\x63\xEF\x9C\x10\xF4\xCA\xBE"

extern const unsigned char cauc_CRYPTO_MUTA_TEST[];
extern const unsigned char cauc_CRYPTO_MUTA[];


/******************************************************************************/
/*                          Constantes pour l'application                     */
/******************************************************************************/
// Valeurs de ACTYS_INFO
#define kACTYS_INFO_VER_SPECIFICATIONS          "100 "
#if BICA
	#define kACTYS_INFO_VER_APPLICATION             "3.03"
	#define kACTYS_VER_EDITION                      3
	#define kACTYS_VER_REVISION                     02
#else
	#define kACTYS_INFO_VER_APPLICATION             "3.01"
	#define kACTYS_VER_EDITION                      3
	#define kACTYS_VER_REVISION                     1
#endif
#define kACTYS_INFO_EDITEUR_APPLICATION         "109 "

/* ID pour le noyau de l'application Moneo */
#define kACTYS_ID                               0x6001

/* Type de Commande a effectuer */
#define kACTYS_CODE_CDE_GET_INFO                0x0001
#define kACTYS_CODE_CDE_RESET_APPLI             0x0002
#define kACTYS_CODE_CDE_EXECUTION               0x0004

/* Indice de la commande a effectuer */
#define kACTYS_BUILD                            0x00000001
#define kACTYS_INTERNAL_AUTHENT                 0x00000002
#define kACTYS_CHECK_BLOCAGE                    0x00000004
#define kACTYS_READ_C_FILE                      0x00000008
#define kACTYS_CHECK_MIRROR                     0x00000010
#define kACTYS_EXTERNAL_AUTHENT                 0x00000020
#define kACTYS_CUSTOMER_REFERENCE					0x00000040
#define kACTYS_WRITE_C_FILE                     0x00000080
#define kACTYS_CHECK_PVV_TRACK_CARD             0x00000100


//Gestion Message Bloc Info

#define kACTYS_TAG_APPLI_TOTAL	        			(unsigned long) 0xBFE001
#define kACTYS_TAG_LANGUE			        			(unsigned long) 0xFF808000
#define kACTYS_TAG_ListeMessages						0xF1
#define kACTYS_TAG_VERSION								0x80
#define kACTYS_TAG_MESSAGE_0		        			(unsigned long) 0x9FC000

#define kACTYS_MESS_CARTE_ARRACHEE              5
#define kACTYS_MESS_PAIEMENT_REFUSE             6
#define kACTYS_MESS_PREMIER_ESSAI               7
#define kACTYS_MESS_DEUXIEME_ESSAI      			8
#define kACTYS_MESS_DERNIER_ESSAI       			9
#define kACTYS_MESS_CODE_BON            			10
#define kACTYS_MESS_CODE                			11
#define kACTYS_MESS_CODE_FAUX           			12
#define kACTYS_MESS_APPLI_BLOQUEE       			13
#define kACTYS_MESS_3_CODES_FAUX                14
#define kACTYS_MESS_ABANDON             			15
#define kACTYS_MESS_CODE_PIN                    16
#define kACTYS_MESS_CODE_CHAUFF                 17
#define kACTYS_MESS_REF_CLIENT                  18
#define kACTYS_MESS_REF_CLIENT2                 19
#define kACTYS_MESS_REF_FAUSSE           			20

//DEBUG: lecture piste
#define kACTYS_AFF_LECTURE_PISTE                " LECTURE PISTE "
#define kACTYS_AFF_RETRAIT_CARTE                " RETRAIT CARTE "
#define kACTYS_AFF_INSERTION_CARTE              "INSERTION CARTE"

#define kACTYS_OPTION_SAISIE_AUTO      			0x0001 /* Poursuite saisie code faux   */
#define kACTYS_OPTION_ARRET_ARRACHAGE  			0x0002 /* Arret dur arrachage carte    */
#define kACTYS_OPTION_CORR_ANNUL       			0x0004 /* Correction comme annulation  */

#define kACTYS_PIN_LEN			        				4
#define kBICA_PIN_LEN			                  4 /* en fait 6 */
#define LG_KEY 											8
#define LG_ISO2											40
#define LG_ISO2_BCD										20
#define LG_ISO2_MODIFIED								37
#define LG_BUF_FROM_SERVER             			255
#define LG_C1_FILE                     			24
#define LG_C2_FILE                     			492
#define LG_C3_FILE                     			132
#define LG_C3b_FILE                    			136
#define LG_BUF_TO_SERVER               			512
#define LG_BLOC_WRITE                  			60
#define kACTYS_LG_MAX_DATA_TO_SERVER            512
#define kACTYS_TEMPO_SAISIE                     100 //10s
#define kACTYS_TEMPO_SAISIE_1ERE_TOUCHE         300 //30s
#define LG_REF_CLIENT									6
#define LG_MESS_CLIENT									11

#define GR_EUROTRAFIC_CARD								    1
#define DKV_CLASSIC_CARD								    2
#define DKV_CLASSIC_CARD_AND_DRIVER_CODE			        3
#define DKV_SELECTION_CARD								    4
#define UTA_CARD											5
#define AVIA_CARD											6
#define ARIS_CARD											8
#define BICA_CARD											9
#define LOMO_CARD											10
#define CSC_CARD											11

/******************************************************************************/
/*                  Definitions  pour l'arborescence Carte                    */
/******************************************************************************/

/* AID MONEO sous forme TLV pour transmission */
#define kACTYS_AID_ACTYS_TLV                    "\xBF\xE0\x01\x82\x00\x0C\xE4\x0A\x9F\x06\x07\x70\x10\xFF\x02\x50\x00\x00"
#define kACTYS_AID_ACTYS                        "\x70\x10\xFF\x02\x50\x00\x00"

#define kSelectMasterFile								0x00
#define kSelectDirectoryFile							0x01
#define kSelectFile										0x02
#define kSelectAID										0x04

/******************************************************************************/
/*                           Definitions dialogue carte                       */
/******************************************************************************/
                              /************/
                              /*  GENERAL */
                              /************/

/* Longueur de la chaîne de caractère
   passée dans les différentes commandes */

#define kACTYS_SELECT_LEN								2
#define kACTYS_READ_BINARY_LEN                  3
#define kACTYS_FILEKEY_LEN                      8


/* Classe de commande */
#define kACTYS_CLA_INTER_INDUSTRY               0x00 /* Classe inter-industry */
#define kACTYS_CLA_INTER_INDUSTRY_SECURED       0x04 /* Classe inter-industry */
#define kACTYS_CLA_80									0x80

/* Commande Select */
#define kACTYS_INS_SELECT                       0xA4 /* INS SELECT            */

#define kACTYS_P1_SELECT_MASTERFILE             0x00
#define kACTYS_P1_SELECT_DIRFILE                0x01

#define kACTYS_P2_SELECT                        0x00
#define kACTYS_Lc_SELECT                        0x02
#define kACTYS_Le_SELECT                        0x00

/* Commande Read Binary */
#define kACTYS_INS_READ_BINARY                  0xB0 /* INS READ BINARY       */

/* Commande Update Binary */
#define kACTYS_INS_UPDATE_BINARY                0xD6 /* INS Update Record     */

/* Commande Get Challenge */
#define kACTYS_INS_GET_CHALLENGE                0x84 /* INS Get Challenge     */
#define kACTYS_P1_GET_CHALLENGE                 0x00
#define kACTYS_P2_GET_CHALLENGE                 0x00
#define kACTYS_Le_GET_CHALLENGE                 0x08 /* Le Get Challenge      */

/* Commande Get Info */
#define kACTYS_INS_GET_INFO							0xC0 /* INS Get Info          */
#define kACTYS_P1_GET_INFO								0x02
#define kACTYS_P2_GET_INFO								0x05
#define kACTYS_Le_GET_INFO								0x08 /* Le Get Info           */

/* Commande Select File Key */
#define kACTYS_INS_FILEKEY                      0x28 /* INS SELECT FILE KEY   */
#define kACTYS_P1_FILEKEY								0x00
#define kACTYS_P2_FILEKEY                       0x27
#define kACTYS_Lc_FILEKEY                       0x08
#define kACTYS_Le_FILEKEY                       0x0C

/* Commande Verify */
#define kACTYS_INS_VERIFY                       0x20 /* INS                    */
#define kACTYS_P1_VERIFY                        0x00 /* P1                     */
#define kACTYS_Lc_VERIFY                        0x08 /* Lc                     */
#define kACTYS_PADDING_BEFORE_PIN               0x20 /* Padding before code    */
#define kACTYS_PADDING_AFTER_PIN                0xFF /* Padding after code     */

/******************************************************************************/
/*                              Statuts                                       */
/******************************************************************************/

/* Statuts CAM */
#define kACTYS_WS1_OK                           0x90 /*  Valeur SW1 correcte   */
#define kACTYS_WS1_WARNING                      0x61 /*  Warning de SW1        */
#define kACTYS_WS1_ERROR_SEQUENCE               0x9F /*  Stat d'erreur ds SW2  */
#define kACTYS_WS2_OK                           0x00 /*  SW2 OK si SW1=90      */

#define kACTYS_WS1_WRONG_PIN                    0x63 /*  Wrong Pin Code        */
#define kACTYS_WS2_WRONG_PIN                    0xCF /*  Wrong Pin Code        */

#define kACTYS_WS1_PIN_BLOCKED                  0x69 /*  Application blocked   */
#define kACTYS_WS2_PIN_BLOCKED                  0x83 /*  Application blocked   */


/* Statuts renvoyés par les commandes ACTYS */
typedef enum
{
   kACTYS_STATUS_OK = 0,                          /* Par d'erreur             */
   kACTYS_STATUS_OUT_OF_MEMORY,                   /* Echec Allocation mémoire */
   kACTYS_STATUS_ERREUR_CAM,                      /* Erreur du driver CAM     */
   kACTYS_STATUS_ERREUR_SW1,                      /* ERREUR SW1               */
   kACTYS_STATUS_CARTE_ARRACHEE,                  /* Carte arrachee par client*/
   kACTYS_STATUS_ABANDON,                         /* Abandon client           */
   kACTYS_STATUS_TIME_OUT_SAISIE,                 /* Timeout saisie           */
   kACTYS_STATUS_ERREUR_PRESENTATION_CODE_CONF,   /* ERREUR de code conf      */
   kACTYS_STATUS_NB_SAISIE_NULLE,                 /* Plus de saisie possible  */
   kACTYS_STATUS_ERROR_PVV,							  /* Erreur PVV, PIN faux     */
   kACTYS_STATUS_ERROR_ISO2,                      /* Piste ISO2 erronee       */
   kACTYS_STATUS_ERROR_DES,
   kACTYS_STATUS_ERROR_INTERNAL_AUTHENT,
   kACTYS_STATUS_ERROR_CARTE_BLOQUEE,
   kACTYS_STATUS_ERROR_REF_CLIENT,
   kACTYS_STATUS_ERROR_PARAMETRAGE_EN_COURS,
   kACTYS_STATUS_ERREUR_DONNEES_TELECHARGE,
   kACTYS_STATUS_ERREUR_TYPE_CARTE_PISTE
}ACTYS_STATUS;

/******************************************************************************/
/*                                Structures                                  */
/******************************************************************************/

/* Structure permettent de récupérer des infos générales sur l'application */
typedef struct
{
        unsigned int  iId;
        unsigned char tVerSpecifications        [5];
        unsigned char tVerApplication           [5];
        unsigned char tEditeurApplication       [5];
        unsigned char tVerListeMessages         [5];
/* AG DEBUG --> Pour faire fonctionner le GET INFO                            */
        unsigned char tVerListeCles             [5];
        unsigned char tVerListeAID              [5];
/* AG DEBUG --> Pour faire fonctionner le GET INFO                            */
}xStructINFO;

/* Structure des données renvoyées au serveur */
typedef struct
{
        unsigned int    iCaracteristiqueComposant ; 						/* caract compo */
        unsigned long   lEtapesExecute ;             						/* Liste Etapes executées*/
        unsigned int    ui_PinTryCounter ;             					/* Nombre saisies code possibles  */
        unsigned char   aucRefClient				[LG_REF_CLIENT+1];  	/* Reference client a imprimer */
		  unsigned char   aucCodeChauf				[4+1];               /* Code chauff */
}xStructTO_SERVER;

/* Structure des données reçues du serveur */
typedef struct
{
        /* Chaque tableau a comme premier élément sa longueur */
		  unsigned int   iCodeLangue ;			// Le serveur dit dans quelle langue je parle.
        unsigned long  lListEtape;                   /* etapes a executer     */
        unsigned int   ui_TrackCardType;
        unsigned int   ui_PinTryCounter;
		  unsigned char  uc_FlagTest;
}xStructFROM_SERVER;

typedef struct
{
			unsigned char 	*pC1FileToServer ;				// Regroupement des données pour le serveur
			unsigned int    iLgC1FileToServer ;				// Longueur de DataToServeur

}xStructC1FILE_TO_SERVER;

typedef struct
{
			unsigned char 	*pC2FileToServer ;				// Regroupement des données pour le serveur
			unsigned int    iLgC2FileToServer ;				// Longueur de DataToServeur

}xStructC2FILE_TO_SERVER;

typedef struct
{
			unsigned char 	*pC3FileToServer ;				// Regroupement des données pour le serveur
			unsigned int    iLgC3FileToServer ;				// Longueur de DataToServeur

}xStructC3FILE_TO_SERVER;

/* Structure principale des données de l'application */
typedef struct
{
        	/* !!!! Chaque tableau a comme premier élément sa longueur !!! */

        	/* Données générales */
        	xStructTO_SERVER         xToServer;
        	xStructFROM_SERVER       xFromServer;
        	xStructC1FILE_TO_SERVER  xC1FiletoServer;
        	xStructC2FILE_TO_SERVER  xC2FiletoServer;
        	xStructC3FILE_TO_SERVER  xC3FiletoServer;
        	/* Data echangees avec le serveur */
        	unsigned char *         puc_DataFromServer;

        	unsigned char           aucDataC3      [LG_C3_FILE];
        	unsigned char           aucDataC3bis   [LG_C3b_FILE];
	/* ----------------Temporaire ---------------------------------*/
			unsigned char				aucInfo			[9];
        	unsigned char           aucCSN			[9];
        	unsigned char           aucISO2        [LG_ISO2];
        	unsigned char           uc_TypePIN;
        	unsigned char  *        puc_Trnd;
        	unsigned char    			auc_TKgr			[8];
	/* ------------------------------------------------------------*/

} xACTYS_CARTE;


/******************************************************************************/
/*                               Prototypes                                   */
/******************************************************************************/

                            /***********************/
                            /* Ordres CAM généraux */
                            /***********************/

ACTYS_STATUS ACTYS_SelectFile	(	OS_HANDLE*     pP_Handle,
                              	CAM_CONF*      pL_CamConf,
                              	unsigned char  ucP_TSelect,
                              	unsigned int   ip_LgFileName,
                              	unsigned char* ucP_FileName);

ACTYS_STATUS ACTYS_ReadBinary	(	OS_HANDLE *,
		              					CAM_CONF *,
			      						unsigned char *,
			      						unsigned char *,
			      						unsigned int *);


ACTYS_STATUS ACTYS_UpdateBinary(	OS_HANDLE *,
								   		CAM_CONF *,
								   		unsigned char *,
								   		unsigned char *,
								   		unsigned char * );

ACTYS_STATUS ACTYS_Get_Challenge(OS_HANDLE*     pP_Handle,
                                 CAM_CONF*      pL_CamConf,
                                 unsigned char* pL_Dataout);

ACTYS_STATUS ACTYS_GetInfo		(	OS_HANDLE *,
								   		CAM_CONF *,
								   		xACTYS_CARTE * );

ACTYS_STATUS ACTYS_SelectFileKey(OS_HANDLE *,
				 				   		CAM_CONF *,
				 				   		unsigned char *,
				 				   		unsigned char ** );

ACTYS_STATUS ACTYS_Verify		(	OS_HANDLE *,
								   		CAM_CONF *,
								   		unsigned char ,
								   		unsigned char *,
								   		xACTYS_CARTE * );

 			    					/***************************/
                           /* Fonctions CAM générales */
                           /***************************/


ACTYS_STATUS ACTYS_Build 		(	OS_HANDLE *,
                          			CAM_CONF *);

ACTYS_STATUS ACTYS_VerifyPin 	(	OS_HANDLE *,
                              	CAM_CONF **,
                              	xACTYS_CARTE *);

ACTYS_STATUS ACTYS_Write_C3   (	OS_HANDLE *,
                               	CAM_CONF *,
                               	xACTYS_CARTE *,
                               	unsigned char *);

ACTYS_STATUS ACTYS_Check_Mirror(	OS_HANDLE *,
                                 CAM_CONF *,
                                 xACTYS_CARTE *,
                                 unsigned char *);


			   					/*************************/
                           /* Fonctions utilitaires */
                           /*************************/

ACTYS_STATUS ACTYS_Write_Bloc	(	OS_HANDLE *,
                              	CAM_CONF *,
                              	unsigned char,
                              	unsigned char *,
                              	int,
                              	int,
                              	unsigned char *);

ACTYS_STATUS ACTYS_Write_Chip_C3(OS_HANDLE*,
                                 CAM_CONF*,
                                 unsigned char*,
                                 unsigned char,
                                 unsigned char*);

void ACTYS_Free 						(void **);


void ACTYS_ResetInfoCarte			(xACTYS_CARTE *);


void ACTYS_Affiche				(	OS_HANDLE *,
                   					unsigned char,
                   					unsigned char,
                   					unsigned char *,
                   					unsigned char);

ACTYS_STATUS ACTYS_SaisieCodes(	OS_HANDLE *,
                                	xACTYS_CARTE *,
                                	unsigned char *,
                                	unsigned char *,
                                	CAM_CONF     **);

ACTYS_STATUS ACTYS_SaisieCode	(	OS_HANDLE *,
                                	xACTYS_CARTE *,
                                	unsigned char *,
                                	unsigned char,
                                	CAM_CONF   **);

	       		/******************************************************/
               /* Fonction principale qui communique avec le serveur */
               /******************************************************/

void ACTYS_Tache(void);



#endif
