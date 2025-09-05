#include "EMV.h"


CB2A* CB2AInit(int protocol)
{
	CB2A* pcb2a = (CB2A*)malloc (sizeof (CB2A));
	pcb2a->Fields = NULL;
	CB2AReadTagFile (pcb2a);

	return pcb2a;
}

void CB2AEnd (CB2A* pcb2a)
{
	if (!pcb2a)
	{
		return;
	}

	while (pcb2a->Fields)
	{
		CB2AField* pfield = (CB2AField*)pcb2a->Fields->car;
		while (pfield->Tags)
		{
			CB2ATag* ptag = (CB2ATag*)pfield->Tags->car;
			ListRemove (&pfield->Tags, ptag);
			free (ptag);
		}
		ListRemove (&pcb2a->Fields, pfield);
		free (pfield);
	}

	free (pcb2a);
}

void CB2AInitBuffer (CB2AMessage* pmessage)
{
	pmessage->tlv_buffer = 0;
	pmessage->tlv_allocated = 0;
	pmessage->tlv_length = 0;
	pmessage->tlv_debug_enabled = '1';
}

void CB2AEndBuffer(CB2AMessage* pmessage)
{
	if (pmessage->tlv_buffer)
		free(pmessage->tlv_buffer);
}


void CB2AClearBuffer(CB2AMessage* pmessage)
{
	pmessage->tlv_length = 0;
}


void CB2ACheckAndExtendBuffer(CB2AMessage* pmessage, int incrSize)
{
	if (pmessage->tlv_length + incrSize <= pmessage->tlv_allocated)
		return;

	if (pmessage->tlv_allocated == 0)
	{
		// Init size
		pmessage->tlv_allocated = 2 * 1024;
		pmessage->tlv_buffer = (BYTE*)malloc(pmessage->tlv_allocated);
        memset (pmessage->tlv_buffer, 0, pmessage->tlv_allocated);
	} else
	{
        int old_size = pmessage->tlv_allocated;
		// incrSize musn't very big, but just in case
		while (pmessage->tlv_length + incrSize > pmessage->tlv_allocated)
			pmessage->tlv_allocated *= 2;
		// Realloc must copy old data
		pmessage->tlv_buffer = (BYTE*)realloc(pmessage->tlv_buffer, pmessage->tlv_allocated);
        memset (pmessage->tlv_buffer + old_size, 0, pmessage->tlv_allocated - old_size  );
	}

	// Unable allocate
	if (pmessage->tlv_buffer == 0)
	{
		if (pmessage->tlv_debug_enabled)
			printf("Unable allocate memory\n");
	}
}



void CB2AAddTag (CB2A* pcb2a, CB2AField* pfield, CB2ATag* ptag)
{
	ListNewr (&pfield->Tags, ptag);
}


void CB2AAddField (CB2A* pcb2a, CB2AField* pfield)
{
	ListNewr (&pcb2a->Fields, pfield);
}


CB2AFieldValue* CB2AInitFieldValue (CB2A* pcb2a, CB2AField* pfield)
{
	CB2AFieldValue* pfieldvalue = (CB2AFieldValue *)malloc (sizeof (CB2AFieldValue));
	pfieldvalue->Value	= NULL;
	pfieldvalue->Size	= 0;
	pfieldvalue->pField = pfield;
	pfieldvalue->TagsValue = NULL;
	return pfieldvalue;
}

CB2ATagValue* CB2AInitTagValue (CB2A* pcb2a, CB2ATag* ptag)
{
	CB2ATagValue* ptagvalue = (CB2ATagValue* )malloc (sizeof (CB2ATagValue));
	ptagvalue->Value	= NULL;
	ptagvalue->Size	= 0;
	ptagvalue->pTag = ptag;
	return ptagvalue;
}




void CB2AAddTagValue (CB2A* pcb2a, CB2AFieldValue* pfieldvalue, CB2ATagValue* ptagvalue)
{
	ListNewr (&pfieldvalue->TagsValue, ptagvalue);
}

void CB2AAddFieldValue (CB2A* pcb2a, CB2AMessage* pmessage, CB2AFieldValue* pfieldvalue)
{
	ListNewr (&pmessage->FieldsValue, pfieldvalue);
}

CB2AMessage* CB2AInitMessage (CB2A* pcb2a, char* sidentifier)
{
	int idsize;
	List* listfield = NULL;  
	List* listtag = NULL; 
	CB2AFieldValue* pfirstvalue;

	CB2AMessage* pmessage = (CB2AMessage*)malloc(sizeof(CB2AMessage));
	pmessage->FieldsValue = NULL;

	idsize = CharArrayToBCDArray (sidentifier, strlen (sidentifier), CB2A_IDENTIFIER_SIZE, "n", pmessage->Identifier);

	memset (pmessage->Bitmap, 0, sizeof (pmessage->Bitmap));


	listfield = pcb2a->Fields;
	while (listfield)
	{
		CB2AField* pfield		= (CB2AField*)listfield->car;
		CB2AFieldValue* pfieldvalue;
		if (strcmp (pfield->Requirements, "") != 0)
		{
			pfieldvalue	= CB2AInitFieldValue (pcb2a, pfield);
			CB2AAddFieldValue (pcb2a, pmessage, pfieldvalue);

			listtag = pfield->Tags;
			while (listtag)
			{
				CB2ATag* ptag			= (CB2ATag*)listtag->car;
				CB2ATagValue* ptagvalue;
				if (strcmp (ptag->Requirements, "") != 0) 
				{
					ptagvalue = CB2AInitTagValue (pcb2a, ptag);
					CB2AAddTagValue (pcb2a, pfieldvalue, ptagvalue);
				}
				listtag = listtag->cdr;
			}
		}
		listfield = listfield->cdr;
	}

	pfirstvalue		= (CB2AFieldValue*)ListGetElt (pmessage->FieldsValue, 0);
	
	pfirstvalue->Value = pmessage->Bitmap;
	pfirstvalue->Size  = 16;

	CB2AInitBuffer (pmessage);


	return pmessage;
}

void CB2AEndMessage (CB2AMessage* pmessage)
{
	if (!pmessage) return;
	while (pmessage->FieldsValue)
	{
		CB2AFieldValue* pfieldvalue = (CB2AFieldValue*)pmessage->FieldsValue->car;
		while (pfieldvalue->TagsValue)
		{
			CB2ATagValue* ptagvalue = (CB2ATagValue*)pfieldvalue->TagsValue->car;
			ListRemove (&pfieldvalue->TagsValue, ptagvalue);
			free (ptagvalue);
		}
		ListRemove (&pmessage->FieldsValue, pfieldvalue);
		free (pfieldvalue);
	}
	CB2AEndBuffer (pmessage);
	free (pmessage);
}



  


char* CB2AGetTagNameFromHexStr (CB2A* pcb2a, CB2AField* pfield, char* stag)
{
	List* listtag = pfield->Tags;
	while (listtag)
	{
		CB2ATag* ptag = (CB2ATag*)listtag->car;
		if (strcmp (ptag->strTag, stag) == 0) return ptag->Name;
		listtag = listtag->cdr;
	}
	return NULL;
}


char* CB2AGetTagNameFromDecimal (CB2A* pcb2a, CB2AField* pfield, int tag)
{
	List* listtag = pfield->Tags;
	while (listtag)
	{
		CB2ATag* ptag = (CB2ATag*)listtag->car;
		if (ptag->Tag == tag) return ptag->Name;
		listtag = listtag->cdr;
	}
	return NULL;
}


CB2ATag* CB2AGetTagFromDecimal (CB2A* pcb2a,  CB2AField* pfield, int tag)
{
	List* listtag = pfield->Tags;
	while (listtag)
	{
		CB2ATag* ptag = (CB2ATag*)listtag->car;
		if (ptag->Tag == tag) return ptag;
		listtag = listtag->cdr;
	}
	return NULL;
}




char* CB2AGetFieldNameFromDecimal (CB2A* pcb2a, int field)
{
	List* listfield = pcb2a->Fields;
	while (listfield)
	{
		CB2AField* pfield = (CB2AField*)listfield->car;
		if (pfield->Field == field) return pfield->Name;
		listfield = listfield->cdr;
	}
	return NULL;
}


CB2AField* CB2AGetFieldFromDecimal (CB2A* pcb2a,  int field)
{
	List* listfield = pcb2a->Fields;
	while (listfield)
	{
		CB2AField* pfield = (CB2AField*)listfield->car;
		if (pfield->Field == field) return pfield;
		listfield = listfield->cdr;
	}
	return NULL;
}


BYTE* CB2AGetTag (CB2AMessage* pmessage, unsigned short tag, int* outSize)
{
	 return 0;
}

//Get Field Size in Bytes
int CB2AGetFieldSize (CB2AField* pField)
{
    if (pField->DataType == DATA_TYPE_n)
    {
        if (pField->Size % 2) return (pField->Size / 2) + 1;
        else return (pField->Size / 2);
    }
    else
        return pField->Size;
}

//Get Tag Size in Bytes
int CB2AGetTagSize (CB2ATag* pTag)
{
    if (pTag->DataType == DATA_TYPE_n)
    {
        if (pTag->Size % 2) return (pTag->Size / 2) + 1;
        else return (pTag->Size / 2);
    }
    else
        return pTag->Size;
}

void CB2ASetField (CB2AMessage* pmessage, CB2AFieldValue* pfieldvalue, EMVTag* pemvtag,  BYTE* tagvalue, int tagsize)
{
	BYTE* endBuffer;		
	BYTE fielddatatype = pfieldvalue->pField->DataType;
    BYTE emvtagdatatype =  pemvtag->DataType;
    int emvtagsize      =  pemvtag->Size;   

	CB2ACheckAndExtendBuffer(pmessage, 1 + tagsize);			
	endBuffer = pmessage->tlv_buffer + pmessage->tlv_length;
			

	pfieldvalue->Value  = endBuffer;		

	endBuffer += 1;
//        if (pfieldvalue->pField->From == -1)
//			    totalsize += EMVSetFieldValueFromEMVTag (pmessage, pfieldvalue, pemvtag, value + pfieldvalue->pField->From/2, (pfieldvalue->pField->To - pfieldvalue->pField->From) /2);

  
    if (pfieldvalue->pField->From != -1)
       tagsize = memcpyHexa ((char*)tagvalue, tagsize, pfieldvalue->pField->From, pfieldvalue->pField->To, "n", endBuffer);
    else
	if (fielddatatype ==  emvtagdatatype)
    {
        memcpy(endBuffer, tagvalue, tagsize);
    }
	else
		tagsize = CB2AWriteEMVTag (emvtagdatatype,  emvtagsize,  fielddatatype, (char*)tagvalue, tagsize, endBuffer);
		
    if (!pfieldvalue->pField->IsVariable)
        tagsize = CB2AGetFieldSize (pfieldvalue->pField);

    pfieldvalue->Value[0] = (char)tagsize;

	pfieldvalue->Size	  = 1 + tagsize;
	pmessage->tlv_length += pfieldvalue->Size;

}


void CB2ASetTag (CB2AMessage* pmessage, CB2ATagValue* ptagvalue, EMVTag* pemvtag, BYTE* tagvalue, int tagsize)
{
    BYTE emvtagdatatype =  pemvtag->DataType;
    int emvtagsize      =  pemvtag->Size;   
	BYTE* endBuffer;
	char ssize[10];

	unsigned short tag = ptagvalue->pTag->Tag; 
//    if (ptagvalue->pTag->From == -1)
    //		fieldsize += EMVSetTagValueFromEMVTag (pmessage, ptagvalue, pemvtag, value + ptagvalue->pTag->From/2, (ptagvalue->pTag->To - ptagvalue->pTag->From) / 2);

	BYTE fielddatatype = ptagvalue->pTag->pField->DataType;

	if (fielddatatype == DATA_TYPE_a)  // type 2 octes ascii longeur 2 octets ascii  // content ascii
	{
		char stype[6];
			
		CB2ACheckAndExtendBuffer(pmessage, 4 + tagsize);			
		endBuffer = pmessage->tlv_buffer + pmessage->tlv_length;
			
		ptagvalue->Size	  = 4 + tagsize;
		ptagvalue->Value  = endBuffer;			

		DecimalToHexStr (tag, stype);

		memcpy(endBuffer, stype, 2);
		endBuffer += 2;
			
			
		sprintf (ssize, "%.2d", tagsize);
		memcpy(endBuffer, ssize, 2);
		endBuffer += 2;
			
		memcpy(endBuffer, tagvalue, tagsize);
		pmessage->tlv_length += 4 + tagsize;

	}
	else
	if (fielddatatype == DATA_TYPE_b || fielddatatype == DATA_TYPE_n)  // type 2 octes DCB longeur 1 octets DCB, content dcb
	{
		BYTE Output[5];
		int i;
		char stype[6];
		BYTE* sizeBuffer;

		CB2ACheckAndExtendBuffer(pmessage, 3 + tagsize);			
		endBuffer = pmessage->tlv_buffer + pmessage->tlv_length;
			
			
		ptagvalue->Value  = endBuffer;


		sprintf (stype, "%.4X", tag);

		i = CharArrayToHexaArray (stype, 4, 2, "b", Output); 

		memcpy(endBuffer, Output, i);
		endBuffer += i;
			
		sizeBuffer = endBuffer;
		endBuffer += 1;

	

		if (emvtagdatatype ==  DATA_TYPE_a)
			tagsize = CB2AWriteEMVTag (ptagvalue->pTag->DataType,  ptagvalue->pTag->Size,  ptagvalue->pTag->pField->DataType, (char*)tagvalue, tagsize, endBuffer);
		else
			memcpy(endBuffer, tagvalue, tagsize);
			
			
		sprintf (ssize, "%.2d", tagsize);
		CharArrayToBCDArray (ssize, 2, 2, "n", Output); 
		memcpy(sizeBuffer, Output, 1);
		ptagvalue->Size	  = 3 + tagsize;


		pmessage->tlv_length += 3 + tagsize;
	}
}



// for a fixed size
// buffersize always in Byte Size (n4 = 2)

int	CB2AWriteEMVTag (int tagtype, int size, int fieldtype, char* ptagvalue, int tagsize, BYTE* Output)
{

	if (fieldtype < 0 || tagtype < 0) 
		return -1;

	if (fieldtype ==  DATA_TYPE_z)
		return CharArrayToHexaArray  (ptagvalue, tagsize, size, "z", Output); // (7)



	if (tagtype  == DATA_TYPE_n)
	{
		if (fieldtype ==  DATA_TYPE_n || fieldtype ==  DATA_TYPE_b)
			return CharArrayToBCDArray (ptagvalue, tagsize, size, "n", Output); // (1)
		else
		if (fieldtype ==  DATA_TYPE_a)
			return CharArrayToBCDArray (ptagvalue, tagsize, size, "a", Output); //(2.1)
		else 
			return -1;
	}
	else
	if (tagtype  == DATA_TYPE_a)
	{
		if (fieldtype ==  DATA_TYPE_b)
			return CharArrayToCharArray  (ptagvalue, tagsize, size, "a", Output); // (3)
		else
		if (fieldtype ==  DATA_TYPE_a)
			return CharArrayToCharArray (ptagvalue, tagsize, size, "a", Output);	//(2.2) 
		else 
			return -1;
	}
	else
	if (tagtype  == DATA_TYPE_b)
	{
		if (fieldtype ==  DATA_TYPE_b)
			return CharArrayToHexaArray  (ptagvalue, tagsize, size, "b", Output); // (5)
		else
		if (fieldtype ==  DATA_TYPE_a)
			return CharArrayToCharArray (ptagvalue, tagsize, size, "a", Output); // (6)
		else 
			return -1;
	}
	else
	if (tagtype  == DATA_TYPE_xplusn)
	{
		if (fieldtype ==  DATA_TYPE_b)
			return CharArrayToBCDArray  (ptagvalue, tagsize, size, "x+n", Output); // (4)	
		else
		if (fieldtype ==  DATA_TYPE_a)
			return CharArrayToCharArray (ptagvalue, tagsize, size, "x+n", Output); // (2.3) 
		else 
			return -1;
	}
	return -1;
}


