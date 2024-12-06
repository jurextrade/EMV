/**************************************************************************/
/*                   Copyright 2000 WYNID TECHNOLOGIES                    */
/**************************************************************************/
/*
* MODULE NAME : API_CAP
*
* FILE NAME : API_CAP.H
*
* PRIVATE :
*
* DESCRIPTION :
*
***************************************************************************/

#ifndef _API_CAP_H_
#define _API_CAP_H_

/**************************************************************************/
/*                              Includes                                  */
/**************************************************************************/
/* Module External includes  */
#include "OS.h"

/* Module Internal includes  */

/**************************************************************************/
/*                               Defines                                  */
/**************************************************************************/

#define CAP_LONGUEUR_ISO2_MAX 40

/* CODES FONCTION CAP  */
#define ISO2_READING_COMMAND            0x10
#define ISO_STATUS_READ  			     	 0x2000

#define CAP_MESSAGE_ISO2_DETECT         0x20
#define CAP_MESSAGE_ISO2_ACCEPT         0x21
#define CAP_MESSAGE_ISO2_REJECT         0x22
#define CAP_MESSAGE_ISO2_TEST           0x23
#define CAP_MESSAGE_ISO2_TO             0x24         // ERREUR DE TIMEOUT


//MODIF: gestion retrait + insertion carte
#define CAM_ACTION_WAIT_INSERTION_INTERRUPT     5
#define CAM_ACTION_WAIT_REMOVAL_INTERRUPT       6

// STATUS CAP
/* C'est ce qu'il y a dans OS.h
#define TE_NO_ERROR                   0x00    //
#define TE_CAP_FORMAT                 0x53    //
#define TE_CAP_LRC                    0x52    //
#define OS_TIMEOUT   		            10	    //
*/
#define TW_CAP_WAIT     0x55
#define TW_CAP_INACTIVE 0x56

/**************************************************************************/
/*                              Typedefs                                  */
/**************************************************************************/


/**************************************************************************/
/*                     Variables Declarations (IMPORT)                    */
/**************************************************************************/

/**************************************************************************/
/*                          Functions Declarations                        */
/**************************************************************************/

unsigned char CAP_AttenteCarte ( OS_HANDLE *,
										 unsigned int,
										 unsigned char *) ;

#endif  // _API_CAP_H_
