#ifndef __HEXATOOLS_H
#define __HEXATOOLS_H
#include <time.h>
#include <sys\timeb.h> 
#include "EMV.h"


#define DATA_TYPE_n			1    // n
#define DATA_TYPE_b			2	 // b, ansb, anscb, … 
#define DATA_TYPE_a			3	 // a, an, as, ns, ans, … 
#define DATA_TYPE_z			4	 // z
#define DATA_TYPE_xplusn	5	 // x+n 

#define DATA_GTYPE_date		8	 // AAMMJJ 
#define DATA_GTYPE_time		9	 // hhmmss
#define DATA_GTYPE_LLVAR	10	 // LLVAR 
#define DATA_GTYPE_LLLVAR	11	 // LLLVAR 



#ifdef __cplusplus
extern "C"
{
#endif
extern int          memcpyHexa (char* buffer, int bufferlength, int from, int to, char* format, BYTE* Output);
extern int			CharArrayToHexaCharArray (BYTE* buffer, char delimiter, int length, char* Output);
extern int			CharArrayToBCDArray (char*  buffer,  int bufferlength, int size, char* format, BYTE* Output);
extern int			CharArrayToHexaArray (char* buffer, int bufferlength, int size, char* format, BYTE* Output);
extern int			CharArrayToCharArray (char* buffer, int bufferlength, int size, char* format, BYTE* Output);
extern int			HexStrToDecimal (char* hexatab);
extern int			DecimalToHexStr (unsigned int dec, char* str);
extern char			BCDToChar (unsigned char bcdNibble);
extern BYTE			CharToBCD (char asciiByte);
extern BYTE			CharToHexa (char asciiByte);
extern int			HexaCharToChar (unsigned char *sta, unsigned char *std, unsigned int la);
extern int			CharToHexaChar (unsigned char *std, unsigned char *sta, unsigned int l);
extern void			NumericDecimalCharToHexaChar(char* Input, int Size, char* Output);


extern void			DateGetGMTDateHour (char* strdate, char* strtime);
extern void			DateGetLocalDateHour (char* strdate, char* strtime);	

extern int			EMVGetFormatProperties (char* strformat, BYTE* datatype, BYTE* variable, int* size);
extern int			EMVGetDataType (char* str);

extern int			EMVTypeCardValidator (char *cardNumbers, int size);
extern int			EMVNumberCardValidator (char *cardNumbers, int size);
extern int			EMVLuhnCardValidator(char* cardNumbers, int size); 



#ifdef __cplusplus
};
#endif

#endif 