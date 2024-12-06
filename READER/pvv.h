/*=====================================================================
		M O D U L E   PVV . H
	 DECLARATIONS Pour PVV au niveau lecteur
 ===================================================================== */
#ifndef _PVV
#define _PVV

#include "Total.h"
#include "OS.h"

#define LG_VAL    24
#define LG_RES1   8
#define LG_RES2   8

// Offset data piste ISO2 sans le 1er octet separateur
#define OFFSET_EXCHANGE_CODE 			23
#define OFFSET_TYPE_PIN      			28
#define OFFSET_PVV_GR_EURO   			29
#define OFFSET_PVV_DKV       			28
#define OFFSET_PVV_UTA       			25
#define OFFSET_FLAG_CODE_CONF       15
#define OFFSET_CODE_CRYPTE_AVIA     27
#define OFFSET_NUM_CARTE_AVIA     	10


#define LG_PIN 		         4
#define LG_VAL_ASCII         48
#define LG_EMETTEUR           6
#define LG_CLIENT             6
#define LG_NUM_CARTE          4
/* ========================================================================== */

ACTYS_STATUS bLuhnControl (unsigned char* pucBuffer, unsigned char ucLgBuffer);

ACTYS_STATUS PVV_CheckPVV_GR_EURO(unsigned char * ,
                                  unsigned char * ,
                                  unsigned char * ,
                                  unsigned char * );

ACTYS_STATUS PVV_CheckPVV_AVIA(unsigned char * ,
                               unsigned char * );

ACTYS_STATUS PVV_CheckPVV_DKV(unsigned char * ,
                              unsigned char * ,
                              unsigned char * ,
                              unsigned char   );

ACTYS_STATUS PVV_CheckPVV_UTA(unsigned char * ,
                              unsigned char * ,
                              unsigned char);

ACTYS_STATUS PVV_CheckPVV_ARIS(unsigned char * ,
                              unsigned char * );

ACTYS_STATUS PVV_CheckPVV_BICA(OS_HANDLE    * pP_Handle,
                              unsigned char * ,
                              unsigned char * );

ACTYS_STATUS PVV_CheckPVV_LOMO(unsigned char * ,
                              unsigned char * );

ACTYS_STATUS PVV_CheckPVV_CSC(unsigned char * ,
                              unsigned char * );
/* ========================================================================== */


#endif
