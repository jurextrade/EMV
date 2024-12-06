/*==============================================================================
================================================================================
=====                        MODULE CRYPTAGE DECRYPT_KEY.H                 =====
================================================================================
================================================================================
===== PROJET     : TOTAL                                                   =====
=====              ***** DEFINITIONS POUR L'APPLICATION *****              =====
=====                                                                      =====
===== EDITEUR    : WYNID Technologies - 2004                               =====
================================================================================
===== EVOLUTIONS : | 1.00 | 22/11/2004 | Creation			   =====
==============================================================================*/
#ifndef _DECRYPT_KEY_H_
#define _DECRYPT_KEY_H_

#include "total.h"

ACTYS_STATUS DECRYPT_Crypto(  unsigned char   uc_TypeKey,
			      unsigned char * puc_Crypto,
                              unsigned char * puc_MKey);


short sReadKey(unsigned char ucNumKey, unsigned char* pucKeyLeft, unsigned char* pucKeyRight);
#endif
