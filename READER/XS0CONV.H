//****************************************************************************
// DASSAULT AT
//****************************************************************************
// RITMO PROJECT
//****************************************************************************
// EXTENDED SERVICES LAYER
//     Conversion functions definition
//****************************************************************************
// Mar/24/98 : LVD - creation
//****************************************************************************

#ifndef _XS0CONV_H_
#define _XS0CONV_H_

//#ifndef _SS0DEFS_H_
//#include "ss0defs.h"
//#endif

//#ifndef _SS0CLOCK_H_
//#include "SS0CLOCK.H"
//#endif

#define OK	0
#define NOK	1

// Justification code
#define CONV_RIGHT_JUST 0
#define CONV_LEFT_JUST  1

//typedef struct
//{
// unsigned long ulHigh;
// unsigned long ulLow;
//} ST_AMOUNT;

// character coding type
short sConvAsc2Bcd(unsigned char *pucAsc, unsigned char *pucBcd, unsigned char ucBcdLen,
                   unsigned char ucJust);
short sConvBcd2Asc(unsigned char *pucBcd, unsigned char *pucAsc, unsigned char ucAscLen,
                   unsigned char ucJust);
short sConvBcd2Bin(unsigned char ucChar, unsigned char *pucResult);
short sConvBin2Bcd(unsigned char ucChar, unsigned char *pucResult);
short sConvAsc2Ebcdic(unsigned char *pucAsc, unsigned char *pucEbcdic, unsigned char ucAscLen);
short sConvEbcdic2Asc(unsigned char *pucEbcdic, unsigned char *pucAsc, unsigned char ucEbcdicLen);

// amount type
//extern short sConvAmount2String(ST_AMOUNT *pxAmount, char *pszString);
//extern short sConvString2Amount(char *pszString, ST_AMOUNT *pxAmount);

// clock type
//extern short sConvDate2Seconds(ST_CLOCK *pxDateTime, unsigned long *pulSeconds);
//extern short sConvSeconds2Date(unsigned long *pulSeconds, ST_CLOCK *pxDateTime);
//extern short sConvDate2String(ST_CLOCK *pxDateTime, short sMaxSize, char *pszString,
//                              char *pszFormat);

#endif
