#ifndef __TLV_H
#define __TLV_H

#include "ostypes.h"

typedef struct _TLV 
{
	BYTE*			tlv_buffer;
	int				tlv_allocated;
	int				tlv_length;
	char			tlv_debug_enabled;
}TLV;





#ifdef __cplusplus
extern "C"
{
#endif
extern void			TLVInitBuffer (TLV* ptlv);
extern void			TLVEndBuffer (TLV* ptlv);
extern void			TLVClearBuffer (TLV* ptlv);
extern BYTE*		TLVGetTag (TLV* ptlv, unsigned short tag, int* outSize);
extern int			TLVGetNextTag (TLV* ptlv, int shift, unsigned short* outTag, BYTE** outBuffer, int* outSize);
extern void			TLVSetTag (TLV* ptlv, unsigned short tag, BYTE* data, int size);
extern int			TLVDol (TLV* ptlv, BYTE* dol, int dolSize, BYTE* outBuffer);

extern int			TLVParse (BYTE* inBuffer, int inBufferSize, unsigned short* outTag, BYTE** outBuffer, int* outSize);
extern int			TLVMake  (BYTE* inBuffer, int inBufferSize, unsigned short tag, BYTE* tlvBuffer);

#ifdef __cplusplus
};
#endif



#endif // __TLV_H






