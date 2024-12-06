#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "EMVTools.h"



unsigned char  hexchar[] = { '0', '1', '2', '3', '4', '5', '6', '7','8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};




void ByteArrayToHexStr (unsigned char buffer[], char delimiter, int length, unsigned char Output[])
{

	int i = 0;
	int j;

	for (j = 0; j < length; j++) 
	{
		Output[i] = hexchar[(buffer[j] >> 4) & 0x0F]; 
		Output[i+1] = hexchar[buffer[j] & 0x0F]; 
		i = i + 2;
		if ((delimiter != 0) && (i < length - 1)) 
		{
			Output[i++] =  delimiter;
		}
	}

}
	

int HexStrToDecimal (char* hexatab)
{
	int num;
	sscanf(hexatab, "%x", &num);
	return num;
}

int DecimalToHexStr (unsigned int dec, char* str)
{
	sprintf(str, "%X", dec);
	return strlen(str);
}


