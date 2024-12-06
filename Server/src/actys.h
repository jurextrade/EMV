#ifndef __ACTYS_H
#define __ACTYS_H
#define _CRT_SECURE_NO_WARNINGS
#include "EMV.h"


#define kACTYS_P1_SELECT_MASTERFILE             0x00
#define kACTYS_P1_SELECT_DIRFILE                0x01


#define kACTYS_P2_SELECT                        0x00
#define kACTYS_Lc_SELECT                        0x02
#define kACTYS_Le_SELECT                        0x00



#define kACTYS_INS_READ_BINARY                  0xB0 /* INS READ BINARY       */
#define kACTYS_READ_BINARY_LEN                  3


/* Commande Get Info */
#define kACTYS_INS_GET_INFO							0xC0 /* INS Get Info          */
#define kACTYS_P1_GET_INFO								0x02
#define kACTYS_P2_GET_INFO								0x05
#define kACTYS_Le_GET_INFO								0x08 /* Le Get Info           */





#define LG_C1_FILE                     			24
#define LG_C2_FILE                     			492
#define LG_C3_FILE                     			132




#define kSelectMasterFile								0x00
#define kSelectDirectoryFile							0x01
#define kSelectFile										0x02
#define kSelectAID										0x04



#define kSelectAID										0x04



#ifdef __cplusplus
extern "C"
{
#endif

extern void CallACTYSS (EMV* pemv, EMVClient* pclient);


#ifdef __cplusplus
};
#endif

#endif // __EMV_H
