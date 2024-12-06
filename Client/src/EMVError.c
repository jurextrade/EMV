#define _CRT_SECURE_NO_WARNINGS

#include "Card.h"


void EMVAddApduError (CardContext* pCardContext, EMVApduError* perror)
{
	ListNewr (&pCardContext->appApduErrors, perror);
}


EMVApduError* EMVGetErrorFromSW1SW2 (CardContext* pCardContext, BYTE SW1, BYTE SW2)
{
	List* listerror = pCardContext->appApduErrors;
	while (listerror)
	{
		EMVApduError* perror = (EMVApduError*)listerror->car;
		if (perror->SW1 == SW1 && perror->SW2 == SW2) 
			return perror;
		listerror = listerror->cdr;
	}
	return NULL;
}