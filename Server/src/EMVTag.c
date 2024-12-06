#define _CRT_SECURE_NO_WARNINGS

#include "EMV.h"


void EMVAddTag (EMV* pemv, EMVTag* ptag)
{
	ListNewr (&pemv->Settings.appTags, ptag);
}


char* EMVGetTagNameFromHexStr (EMV* pemv, char* stag)
{
	List* listtag = pemv->Settings.appTags;
	while (listtag)
	{
		EMVTag* ptag = (EMVTag*)listtag->car;
		if (strcmp (ptag->strTag, stag) == 0) return ptag->Name;
		listtag = listtag->cdr;
	}
	return NULL;
}


char* EMVGetTagNameFromDecimal (EMV* pemv, int field)
{
	List* listtag = pemv->Settings.appTags;
	while (listtag)
	{
		EMVTag* ptag = (EMVTag*)listtag->car;
		if (ptag->Tag == field) return ptag->Name;
		listtag = listtag->cdr;
	}
	return NULL;
}


EMVTag* EMVGetTagFromDecimal (EMV* pemv, int tag)
{
	List* listtag = pemv->Settings.appTags;
	while (listtag)
	{
		EMVTag* ptag = (EMVTag*)listtag->car;
		if (ptag->Tag == tag) return ptag;
		listtag = listtag->cdr;
	}
	return NULL;
}


BYTE* EMVGetTagValue (EMV* pemv, EMVClient* pclient, EMVTag* ptag, int* outSize)
{
	return TLVGetTag (pclient->pTLV, ptag->Tag, outSize);
}


int EMVFillMessageFields (EMV* pemv, CB2AMessage* pmessage, TLV* pTLV)
{
	int shift = 0;
	unsigned short tag;
	unsigned char* tagvalue;
	int taglength;
	int totalsize = 0;
	EMVTag* pemvtag;

	while ((shift = TLVGetNextTag(pTLV, shift, &tag, &tagvalue, &taglength)) != 0)
	{
		BOOL isAscii = TRUE;
		pemvtag = EMVGetTagFromDecimal (pemv, tag);
		totalsize += EMVFillFieldFromEMVTag (pemv, pmessage, pemvtag, tagvalue, taglength);
	}
	return totalsize;

}



int EMVFillFieldFromEMVTag (EMV* pemv, CB2AMessage* pmessage, EMVTag* pemvtag, BYTE* tagvalue, int tagsize)
{
	CB2A* pcb2a = pemv->pCB2A;

	List* listfield = pmessage->FieldsValue;
	List* listtag; 
	int totalsize = 0;
	while (listfield)
	{
		CB2AFieldValue* pfieldvalue = (CB2AFieldValue*)listfield->car;
		if (pfieldvalue->pField->EMVTag == pemvtag->Tag) 
		{
            totalsize += EMVSetFieldValueFromEMVTag (pmessage, pfieldvalue, pemvtag, tagvalue, tagsize);
		}
		else
		{
			int fieldsize = 0;
			listtag = pfieldvalue->TagsValue;

			while (listtag)
			{
				CB2ATagValue* ptagvalue = (CB2ATagValue*)listtag->car;
				if (ptagvalue->pTag->EMVTag == pemvtag->Tag) 
				{
                    fieldsize += EMVSetTagValueFromEMVTag (pmessage, ptagvalue, pemvtag, tagvalue, tagsize);
				}

				listtag = listtag->cdr;
			}

			totalsize += fieldsize;
		}
		listfield = listfield->cdr;
	}
	return totalsize;
}





int EMVSetFieldValueFromEMVTag (CB2AMessage* pmessage, CB2AFieldValue* pfieldvalue, EMVTag* pemvtag, BYTE* tagvalue, int tagsize)
{
	int fieldnumber = pfieldvalue->pField->Field;


	if (fieldnumber > CB2A_MAX_FIELD_NUMBER) 
		return -1;

	if (fieldnumber > 64)
		pmessage->Bitmap[0] |= 0x80;

	pmessage->Bitmap[(fieldnumber - 1) >> 3] |= 0x80 >> ((fieldnumber - 1) & 0x7);

	
	CB2ASetField (pmessage, pfieldvalue, pemvtag, tagvalue, tagsize);
	return tagsize;
}

int EMVSetTagValueFromEMVTag (CB2AMessage* pmessage, CB2ATagValue* ptagvalue, EMVTag* pemvtag, BYTE* tagvalue, int tagsize)
{
	int fieldnumber = ptagvalue->pTag->pField->Field;


	if (fieldnumber > CB2A_MAX_FIELD_NUMBER) 
		return -1;

	if (fieldnumber > 64)
		pmessage->Bitmap[0] |= 0x80;

	pmessage->Bitmap[(fieldnumber - 1) >> 3] |= 0x80 >> ((fieldnumber - 1) & 0x7);
	

	CB2ASetTag (pmessage, ptagvalue, pemvtag, tagvalue, tagsize);
	return tagsize;
}


