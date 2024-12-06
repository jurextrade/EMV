#ifndef __EMVERROR_H
#define __EMVERROR_H




typedef struct _EMVApduError {
	int				SW1;
	int				SW2;
	char			Type[5];
	char			Description[500];
}EMVApduError;

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef __cplusplus
};
#endif

#endif

