#include "tlv.h"
#include <stddef.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>




// Init and destroy application buffer
void TLVInitBuffer (TLV* ptlv)
{
	ptlv->tlv_buffer = 0;
	ptlv->tlv_allocated = 0;
	ptlv->tlv_length = 0;
	ptlv->tlv_debug_enabled = '1';
}

void TLVEndBuffer(TLV* ptlv)
{
	if (ptlv->tlv_buffer)
		free(ptlv->tlv_buffer);
}
// Clear application buffer data (not free memory)
void TLVClearBuffer(TLV* ptlv)
{
	ptlv->tlv_length = 0;
}


static void TLVCheckAndExtendBuffer(TLV* ptlv, int incrSize)
{
	if (ptlv->tlv_length + incrSize <= ptlv->tlv_allocated)
		return;

	if (ptlv->tlv_allocated == 0)
	{
		// Init size
		ptlv->tlv_allocated = 2 * 1024;
		ptlv->tlv_buffer = malloc(ptlv->tlv_allocated);
	} else
	{
		// incrSize musn't very big, but just in case
		while (ptlv->tlv_length + incrSize > ptlv->tlv_allocated)
			ptlv->tlv_allocated *= 2;
		// Realloc must copy old data
		ptlv->tlv_buffer = realloc(ptlv->tlv_buffer, ptlv->tlv_allocated);
	}

	// Unable allocate
	if (ptlv->tlv_buffer == 0)
	{
		if (ptlv->tlv_debug_enabled)
			printf("Unable allocate memory\n");
	}
}
// Get tag value from application buffer
// Return: pointer, memory allocated in buffer; return 0 if not found
BYTE* TLVGetTag (TLV* ptlv, unsigned short tag, int* outSize)
{
	BYTE* currBuf;
	int currPos;	

	currBuf = ptlv->tlv_buffer;
	currPos = 0;
	while (currPos < ptlv->tlv_length)
	{
		unsigned short currTag;
		int currLength;

		// Corrupted buffer
		if (currPos + (int) sizeof(unsigned short) + (int) sizeof(int) > ptlv->tlv_length)
		{
			if (ptlv->tlv_debug_enabled)
				printf("tlv buffer malfunc\n");
			break;
		}

		memcpy(&currTag, currBuf, sizeof(unsigned short));
		currBuf += sizeof(unsigned short);
		currPos += sizeof(unsigned short);
		memcpy(&currLength, currBuf, sizeof(int));
		currBuf += sizeof(int);
		currPos += sizeof(int);
		if (currTag == tag)
		{
			*outSize = currLength;
			return currBuf;
		}
		else
		{
			
		//	printf("Tag %4X: ", currTag);

		}
		currBuf += currLength;
		currPos += currLength;
	}

	// Not found
	return 0;
}

 
// Get next data from application buffer using some shift value
// Returns shift for next element or 0 if end is reached

int TLVGetNextTag(TLV* ptlv, int shift, unsigned short* outTag, BYTE** outBuffer, int* outSize)
{
	int sh;
	sh = shift;

	// Tag
	if (sh + (int) sizeof(unsigned short) > ptlv->tlv_length)
		return 0;
	memcpy(outTag, ptlv->tlv_buffer + sh, sizeof(unsigned short));
	sh += sizeof(unsigned short);

	// Length
	if (sh + (int) sizeof(int) > ptlv->tlv_length)
		return 0;
	memcpy(outSize, ptlv->tlv_buffer + sh, sizeof(int));
	sh += sizeof(int);

	// Value
	if (sh + *outSize > ptlv->tlv_length)
		return 0;
	*outBuffer = ptlv->tlv_buffer + sh;
	sh += *outSize;

	return sh;
}
// Add or update tag in application buffer
void TLVSetTag(TLV* ptlv, unsigned short tag, BYTE* data, int size)
{
	BYTE* findData;
	int findDataSize;

	findData = TLVGetTag(ptlv, tag, &findDataSize);
	if (findData)
	{
		// Replace data
		if (findDataSize == size)
		{
			// Size didn't change, just copy buffer
			memcpy(findData, data, size);
		} else
		{
			// Reserve memory and move data
			TLVCheckAndExtendBuffer(ptlv, size - findDataSize);
			memmove(findData + size, findData + findDataSize, ptlv->tlv_length - (findData - ptlv->tlv_buffer) - findDataSize);
			memcpy(findData - sizeof(int), &size, sizeof(int));
			memcpy(findData, data, size);
			ptlv->tlv_length += size - findDataSize;
		}
	} else
	{
		BYTE* endBuffer;
		// Add data to the end of buffer
		TLVCheckAndExtendBuffer(ptlv, sizeof(unsigned short) + sizeof(int) + size);
		endBuffer = ptlv->tlv_buffer + ptlv->tlv_length;
		memcpy(endBuffer, &tag, sizeof(unsigned short));
		endBuffer += sizeof(unsigned short);
		memcpy(endBuffer, &size, sizeof(int));
		endBuffer += sizeof(int);
		memcpy(endBuffer, data, size);
		ptlv->tlv_length += sizeof(unsigned short) + sizeof(int) + size;
	}
}


// Make tlv from data (1 tag)
// Returns maked size of tlvBuffer
int TLVMake (BYTE* inBuffer, int inBufferSize, unsigned short tag, BYTE* tlvBuffer)
{
	int tlvSize;
	tlvSize = 0;
	if ((tag & 0x1F00) == 0x1F00)
	{
		// 2 byte tag
		tlvBuffer[tlvSize++] = (tag >> 8) & 0xFF;
		tlvBuffer[tlvSize++] = tag & 0xFF;
	} else
	{
		// 1 byte tag
		tlvBuffer[tlvSize++] = tag & 0xFF;
	}

	if (inBufferSize & 0x80)
	{
		// n byte length
		int nBytes = inBufferSize & 0x7F;
		int n;
		if (inBufferSize < 0x100)
			n = 1;
		else if (inBufferSize < 0x10000)
			n = 2;
		else if (inBufferSize < 0x1000000)
			n = 3;
		else
			n = 4;
		tlvBuffer[tlvSize++] = n | 0x80;;
		while (n--)
		{
			tlvBuffer[tlvSize++] = (inBufferSize >> (n * 8)) & 0xFF;
		}
	} else
	{
		// 1 byte length
		tlvBuffer[tlvSize++] = inBufferSize & 0x7F;
	}

	// Copy data
	memcpy(tlvBuffer + tlvSize, inBuffer, inBufferSize);
	tlvSize += inBufferSize;

	return tlvSize;
}

// Parse custom tlv buffer
// outBuffer will point to inBuffer with some shift
// Returns shift to the end of current [tag length value]
int TLVParse (BYTE* inBuffer, int inBufferSize, unsigned short* outTag, BYTE** outBuffer, int* outSize)
{
	BYTE* buf;
	int bufSize;
	buf = inBuffer;
	bufSize = inBufferSize;

	// 1 byte tag
	if (bufSize <= 0)
		return 0;
	*outTag = *buf;	
	if ((*buf & 0x1F) == 0x1F)
	{
		// 2 byte tag
		buf++;
		bufSize--;
		if (bufSize <= 0)
			return 0;

		// Check "Another byte follows"
		if (*buf & 0x80)
			return 0;
		*outTag <<= 8;
		*outTag |= *buf;
	}
	buf++;
	bufSize--;

	// 1 byte length
	if (bufSize <= 0)
		return 0;
	if (*buf & 0x80)
	{
		int nBytes = *buf & 0x7F;
		// Next bytes length
		*outSize = 0;
		while (nBytes--)
		{
			buf++;
			bufSize--;
			if (bufSize <= 0)
				return 0;
			*outSize <<= 8;
			*outSize |= *buf;
		}
	} else
	{
		*outSize = *buf;		
	}
	buf++;
	bufSize--;

	// Check size more than input size or max size
	if (inBufferSize < *outSize + (buf - inBuffer))
		return 0;

	// Point data and return
	*outBuffer = buf;
	return *outSize + (buf - inBuffer);
}



// Make data from DOL list. If no data in buffer - fill zeros
// Returns size of outBuffer
int TLVDol(TLV* ptlv, BYTE* dol, int dolSize, BYTE* outBuffer)
{
	int outSize;
	int dolShift;
	outSize = 0;
	dolShift = 0;
	while (dolShift < dolSize)
	{
		unsigned short tag;
		int size;
		BYTE* findData;
		int findSize;
		int sizeToCopy;

		// Tag could be 1 or 2 byte
		tag = dol[dolShift];
		if ((dol[dolShift] & 0x1F) == 0x1F)
		{
			dolShift++;
			if (dolShift >= dolSize)
				break;
			tag <<= 8;
			tag |= dol[dolShift];
		}
		dolShift++;
		if (dolShift >= dolSize)
			break;

		// Length could be only 1 byte
		size = dol[dolShift];
		dolShift++;

		// Copy data
		findData = TLVGetTag(ptlv, tag, &findSize);
		if (!findData)
			findSize = 0;
		sizeToCopy = findSize < size ? findSize : size;
		if (findData)
		{
			memcpy(outBuffer + outSize, findData, sizeToCopy);
			outSize += sizeToCopy;
		}
		if (size > findSize)
		{
			memset(outBuffer + outSize, 0, size - findSize);
			outSize += size - findSize;
		}
	}

	return outSize;
}
