#define _CRT_SECURE_NO_WARNINGS

#include "EMV.h"


void EMVAddApduError (EMV* pemv, EMVApduError* perror)
{
	ListNewr (&pemv->Settings.appApduErrors, perror);
}


EMVApduError* EMVGetErrorFromSW1SW2 (EMV* pemv, BYTE SW1, BYTE SW2)
{
	List* listerror = pemv->Settings.appApduErrors;
	while (listerror)
	{
		EMVApduError* perror = (EMVApduError*)listerror->car;
		if (perror->SW1 == SW1 && perror->SW2 == SW2) 
			return perror;
		listerror = listerror->cdr;
	}
	return NULL;
}