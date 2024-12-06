#ifndef __HEXATOOLS_H
#define __HEXATOOLS_H





#ifdef __cplusplus
extern "C"
{
#endif

extern void				ByteArrayToHexStr (unsigned char buffer[], char delimiter, int length, unsigned char Output[]);
extern int				HexStrToDecimal (char* hexatab);
extern int				DecimalToHexStr (unsigned int dec, char* str);
extern int				HexStrToDecimal(char* hexatab);
#ifdef __cplusplus
};
#endif

#endif 