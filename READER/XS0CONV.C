//****************************************************************************
// DASSAULT AT
//****************************************************************************
// RITMO PROJECT
//****************************************************************************
// EXTENDED SERVICES LAYER
//     conversion functions
//****************************************************************************
// Mar/24/98 LVD - creation
// Apr/30/99 LVD - modification in sConvDate2String function
// Jun/16/99 Bch - Value modification in EBCDIC table and add 128 positions
// Nov/16/00 LVD - Corr. PVCS M_MULTIAPPLICATIONS 746: Do not modify the
//                 the input parameter pulSeconds value of the
//                 sConvSeconds2Date function.
// feb/16/01 BFi   corr pvcs m_multiapplications 914 : sCovEbcdic2Asc
//****************************************************************************

//#include <stdio.h>
#include <string.h>
//#include "RITYPES.h"
#include "XS0CONV.H"

//#define MAX_AMOUNT_LENGTH 30

//**************************************************************************
// MACRO NULL_AMOUNT (ST_AMOUNT xAmount)
// =====================================
// This macro returns TRUE if xAmount is zero.
//**************************************************************************
//#define NULL_AMOUNT(xAmount) (((xAmount).ulHigh | (xAmount).ulLow) == 0L)

//**************************************************************************
// MACRO NEGATIVE (ST_AMOUNT xAmount)
// ==================================
// This macro returns TRUE if xAmount is negative.
//**************************************************************************
//#define NEGATIVE(xAmount) ((xAmount).ulHigh >= 0x80000000L)

// BCD to ASCII conversion table
const unsigned char tucAscii[17] = "0123456789ABCDEF";

// ASCII to BCD conversion table
const unsigned char tucBcd [32] =
{
 15,10,11,12,13,14,15,15,15,15,15,15,15,15,15,15,   // 0x40 to 0x4F
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15     // 0x30 to 0x3F
};

//***************************************************************************
// Function sConvAsc2Bcd
//---------------------------------------------------------------------------
// Purpose: performs a conversion from ASCII to BCD
//---------------------------------------------------------------------------
// Input Parameters:
//   pucAsc: pointer to the string to be converted
//   ucBcdLen: number of characters to be converted to BCD
//   ucJust: rigth or left justification
//
// Output Parameters
//   pucBcd: pointer to the converted string
//
// Return codes:
//   ERR_NONE: The conversion is OK.
//   NOK: error in parameters
//***************************************************************************
short sConvAsc2Bcd(unsigned char *pucAsc, unsigned char *pucBcd, unsigned char ucBcdLen, unsigned char ucJust)
{
 unsigned char ucNbCar;
 unsigned char ucOdd;

 // Check if the number of characters is odd
 ucOdd = 0;
 if ((ucBcdLen % 2) != 0)
 {
  if (ucJust != CONV_LEFT_JUST)
  {
   // Rigth justification ==> a 0 is added at the beginning
   *(pucBcd++) = tucBcd [*(pucAsc++) & 0x1F];
  }
  else
  {
   ucOdd = 1;
  }
 }

 // calcul of bytes number to be converted
 ucNbCar = (unsigned char) (ucBcdLen /2) ;
 while (ucNbCar-- != 0)
 {
  *pucBcd = (unsigned char)(tucBcd [*(pucAsc++) & 0x1F] << 4); // high part
  *(pucBcd++) += tucBcd [*(pucAsc++) & 0x1F]; // low part
 }

 // Check if the number of characters is odd (left justified)
 if (ucOdd == 1)
 {
  *pucBcd = (unsigned char) ((tucBcd [*pucAsc & 0x1F] << 4) + 0x0F);
 }
 return OK;
}

//***************************************************************************
// Function sConvBcd2Asc
//---------------------------------------------------------------------------
// Purpose: performs a conversion from BCD to ASCII
//---------------------------------------------------------------------------
// Input Parameters:
//   pucBcd: pointer to the string to be converted
//   ucAscLen: number of characters to be converted to ASCII
//   ucJust: rigth or left justification
//
// Output Parameters
//   pucAsc: pointer to the converted string
//
// Return codes:
//   OK: The conversion is OK.
//   NOK: error in parameters
//***************************************************************************
short sConvBcd2Asc(unsigned char *pucBcd, unsigned char *pucAsc, unsigned char ucAscLen, unsigned char ucJust)
{
 unsigned char ucNbCar;
 unsigned char ucOdd;

 ucOdd = 0;
 if((ucAscLen % 2) == 1)
 {
  if (ucJust != CONV_LEFT_JUST)
  {
   // Rigth justification ==> a 0 is added at the beginning
   *(pucAsc++) = tucAscii [*(pucBcd++) & 0x0F];
  }
  else
  {
   ucOdd = 1;
  }
 }

 // calcul of bytes number to be converted
 ucNbCar = (unsigned char) (ucAscLen / 2);
 while (ucNbCar-- != 0)
 {
  *(pucAsc++) = tucAscii [*pucBcd >> 4];
  *(pucAsc++) = tucAscii [*(pucBcd++) & 0x0F];
 }

 // Check if the number of characters is odd (left justified)
 if (ucOdd == 1)
 {
  *(pucAsc++) = tucAscii [*pucBcd >> 4];
 }
 return OK;
}

//***************************************************************************
// Function sConvBcd2Bin
//---------------------------------------------------------------------------
// Purpose: performs a conversion from BCD to BINARY
//---------------------------------------------------------------------------
// Input Parameters:
//   ucChar: BCD byte to be converted
//
// Output Parameters
//   pucResult: pointer on conversion result
//
// Return codes:
//   OK: The conversion is OK.
//   NOK: error in parameters
//***************************************************************************
short sConvBcd2Bin(unsigned char ucChar, unsigned char *pucResult)
{
 *pucResult = (unsigned char)(((ucChar >> 4) * 10) + (ucChar & 0x0F));
 return OK;
}

//***************************************************************************
// Function sConvBin2Bcd
//---------------------------------------------------------------------------
// Purpose: performs a conversion from BINARY to BCD
//---------------------------------------------------------------------------
// Input Parameters:
//   ucChar: Character to be converted
//
// Output Parameters
//   pucResult: pointer on conversion result
//
// Return codes:
//   OK: The conversion is OK.
//   NOK: error in parameters
//***************************************************************************
short sConvBin2Bcd(unsigned char ucChar, unsigned char *pucResult)
{
 *pucResult = (unsigned char)(((ucChar / 10) * 16) + (ucChar % 10));
 return OK;
}







