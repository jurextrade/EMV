#ifndef __EMVFILE_H
#define __EMVFILE_H


typedef struct _EMVFile
{
	int				Index;
	char			Name[200];
	char			Format[1000];
	char			Description[400];
}EMVFile;


#ifdef __cplusplus
extern "C"
{
#endif



#ifdef __cplusplus
};
#endif

#endif // __EMVTAG_H
