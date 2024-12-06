#include "des.h"
#include "pvv.h"
#include "Total.h"
#include "utils.h"
#include "DECRYPT_KEY.h"

#include "Key_Manager.h"
#include "XS0CONV.H"
#include "KYM_algopvv.H"

#include <string.h>




void CHIF_DES8_BICA (unsigned char *DataIN, unsigned char *DataOUT ) {

  char	abcdef[8]={0X00,0X41,0X50,0X50,0X33,0X66,0X33,0X66};

  CHIF_DES8 (DataIN , DataOUT, abcdef,  DES_ENCRYPT);
}

