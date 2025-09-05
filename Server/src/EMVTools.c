#define _CRT_SECURE_NO_WARNINGS
#include "EMVTools.h"



BYTE  hexchar[] = { '0', '1', '2', '3', '4', '5', '6', '7','8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};


int EMVGetDataType (char* str)
{
	if (strstr (str, "b")  != NULL)		
		return DATA_TYPE_b;
	else
	if (strstr (str, "a")  != NULL)		
		return DATA_TYPE_a;
	else
	if (strstr (str, "z")  != NULL)		
		return DATA_TYPE_z;
	else
	if (strstr (str, "n")  != NULL)		
		return DATA_TYPE_n;
	else
	if (strstr (str, "x+n")  != NULL)		
		return DATA_TYPE_xplusn;
	else
		return -1;
}



int EMVGetFormatProperties (char* str, BYTE* datatype, BYTE* variable, int* size)
{
	char strformat[20];
	char strpoints[20];
	char strvariable[20];

	int fromsize = -1;
	int tosize	 = -1;

	memset (strformat, 0, sizeof (strformat));
	memset (strvariable, 0, sizeof (strvariable));

	*size = 1;
	*variable = 0;

//Check for Global Types
	sscanf (str, "%[LVARhmsAMJ ]%s", 
		strformat, 
		str);
	

	sscanf (str, "%[ansb]%d%s", 
		strformat, 
		&fromsize,
		strvariable);

	if (strformat[0] == 0) 
		return -1;

	if (fromsize != -1) //check if variable
	{
		if (strvariable[0] != 0)
		{
			sscanf (strvariable, "%[.]%d", 
				strpoints,
				&tosize);
			if (tosize != -1)
				*variable = 1;

		}
	}
	else
	{
		sscanf (str, "%[ansbz]%[.]%d", 
			strformat,
			strpoints,
			&tosize);
		if (tosize != -1)
			*variable = 1;

	}
	if (tosize != -1)   *size = tosize;
	else
		if (fromsize != -1)	*size = fromsize;
	
	*datatype = EMVGetDataType (strformat);
	
	return 1;
}


int CharArrayToHexaCharArray (BYTE* buffer, char delimiter, int length, char* Output)
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
	return i;

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

// range check failed return 0
char BCDToChar (BYTE bcdNibble)
{
	char result;

	if( bcdNibble < 10 )
		result = (char)( bcdNibble + 48 );	// +48 is applicable to [0,9] input range.
	else
		result = '0';

	return( result );
	
}
// range check failed return 0
BYTE CharToBCD (char asciiByte)
{	
	BYTE result;

	if (asciiByte >= '0' && asciiByte <= '9')
		result = (BYTE)(asciiByte - 48);	// -48 offset gives the decimal value of the ASCII character.
	else
		result = 0;

	return result;
}

BYTE CharToHexa (char asciiByte)
{	
	BYTE result = 0;
	if (asciiByte >= '0' && asciiByte <= '9')
		result = result << 4 | asciiByte & 0xf;
	else
	if ((asciiByte >= 'A' && asciiByte <= 'F') ||
		(asciiByte >= 'a' && asciiByte <= 'f'))
		result = result << 4 | 9 + asciiByte & 0xf;
	else result = 0;

	return result;
}



int memcpyHexa (char* buffer, int bufferlength, int from, int to, char* format, BYTE* Output)
{
    int j = 0;
    int i = 0;
    int pos = 0;
    int size;

    if (to == -1)
        size = bufferlength - from;
    else
        size = to - from;

    if (from % 2)
    {
        while (pos < size)
        {
            i = (from / 2);
            Output[j] = buffer[i] << 4; 
            i++; 
            pos++;
            if (pos == size) break;
            Output[j] = buffer[i] & 0xF0 | Output[j]; 
            j++;
            from = from + 2;
            pos++;
        }
    }
    else
    {
        while (pos < size)
        {
            i = (from / 2);
            Output[j] = buffer[i] & 0xF0; 
            pos++;
            if (pos == size) break;
            
            Output[j] = buffer[i] & 0x0F | Output[j]; 
            i++; j++;
            from = from + 2;
            pos++;
        }
    }
    if (size % 2) return (size / 2) + 1;
    else return size / 2;
}

// Field Format n or b (n, anb, b, ...)
// Data format n

//(1) Codage en DCB sur des quartets: 
// Format de la donnee :  n12 (numerique sur 12 positions) 
//Valeur de la donnee : 12345 
//Codage : (sur 6 octets) [00][00][00][01][23][45]  
 
//(4) Codage en ASCII sur un octet et en DCB sur des quartets
//Format de la donnee :  x + n12 (numerique signe sur 12 positions) 
//Valeur de la donnee : C12345 
//Codage ASCII: (sur 7 octets) [43][00][00][00][01][23][45] 

//size represents 2 for each Byte

int CharArrayToBCDArray (char* buffer, int bufferlength, int size, char* format, BYTE* Output) // fill with 0 on the left
{
	int i = 0;
	int j = 0;
	int k = 0;
	int diff;

	if (strcmp (format, "x+n") == 0)	// 4
	{
		Output[i] = buffer[j];
		bufferlength -= 1;
		Output++;
		buffer++;
		k = 1;
	}
	else
	if (strcmp (format, "n") != 0)		// 1
		return -1;
	
	
	diff = (size - bufferlength) / 2;

	if (diff < 0) return -1;
	
	while (i < diff)
	{
		 Output[i] = 0; 
		 i++;
	}
	
	if (bufferlength % 2) 
	{
		Output[i] = CharToBCD (buffer[j]) ; 
		j++;
		i++;
	}

	while (j < bufferlength)
	{
		Output[i] = CharToBCD (buffer[j]) << 4; 
		Output[i] = CharToBCD (buffer[j + 1]) | Output[i]; 
		j = j + 2;
		i++;
	}
	return i + k;
}

//(5) Codage en binaire sur des octets: 
// Format de la donnee :  b12 (binaire sur 12 positions) 
//Valeur de la donnee : 3CDE1245EF7684172048CBFF 
//Codage: (sur 12 octets) [3C][DE][12][45][EF][76][84][17][20][48][CB][FF]  
// size > bufferlength / 2


//(7) Codage d'une donnee de format z dans un champ de format z: 
// Format de la donnee :  z12 (12 positions) 
//Valeur de la donnee : 45567D874 (où D est le separateur) 
//Codage: (sur 6 octets) [00][04][55][67][D8][74] 
 //size > 12/2


int CharArrayToHexaArray (char* buffer, int bufferlength, int size, char* format, BYTE* Output) // fill with 0 on the left
{
	int i = 0;
	int j = 0;
	int diff = size * 2 - bufferlength;
	
	if (strcmp (format, "z") == 0)		// 7
	{
        diff = size * 2 - bufferlength;
	}
	else
	if (strstr (format, "b") == NULL)	// 5
		return -1;
	
	
	if (diff < 0) return -1;
	
	while (i < diff/2)
	{
		 Output[i] = 0; 
		 i++;
	}
	
	if (format[0] == 'z' && bufferlength % 2) 
	{
		Output[i] = CharToHexa (buffer[0]) ; 
		j = 1;
		i++;
	}

	while (j < bufferlength)
	{
		if (j == bufferlength - 1)
		{
			Output[i] = CharToHexa (buffer[j]);
			i++;
			break;
		}
		Output[i] = CharToHexa (buffer[j]) << 4; 
		Output[i] = CharToHexa (buffer[j + 1]) | Output[i]; 
		j = j + 2;
		i++;
	}
	return i;
}

// Field Format (an, a, ns, ...)
// Data format n => left = 1
//(2) Codage en ASCII sur des octets: 
 
//(2.1) Format de la donnee :  n12 (numerique sur 12 positions)  
//Valeur de la donnee : 12345 
//Codage ASCII: (sur 12 oct.) [30][30][30][30][30][30][30][31][32][33][34][35] 
// left = 1
// carac = '0'

//(2.2) Format de la donnee :  an12 (alpha-numerique sur 12 positions)  
//Valeur de la donnee : AGENCE2  
//Codage ASCII: (sur 12 oct.) [41][47][45][4E][43][45][32][20][20][20][20][20] 
// left = 1;
// carac = blank


//(2.3) Format de la donnee :  x + n12 (numerique signe sur 12 positions)  
//Valeur de la donnee : C12345  
//Codage ASCII: (sur 13 oct.) [43][30][30][30][30][30][30][30][31][32][33][34][35]
//left = 1
//carac = '0'

//(3) Codage en ASCII sur des octets: 
//Format de la donnee :  ans12 (alpha-numerique sur 12 positions) 
//Valeur de la donnee : AGENCE 2 
//Codage ASCII: (sur 12 octets) [41][47][45][4E][43][45][20][32][20][20][20][20] 

int CharArrayToCharArray (char* buffer, int bufferlength, int size, char* format, BYTE* Output)   //fill with caracter on the left
{
	int i = 0;
	int j = 0;
	BYTE carac;
	int left;

	int diff = (size - bufferlength);

	if (strcmp (format, "n") == 0)			// 2.1
	{
		left = 1;
		carac = '0';
	}
	else
	if (strstr (format, "a")  != NULL)		// 2.2
	{
		left = 0;
		carac = ' ';
	}
	else
	if (strstr (format, "x+n")  != NULL)	// 2.3
	{
		left = 1;
		carac = '0';
		Output[i] = buffer[j];
		bufferlength -= 1;
		Output++;
		buffer++;
		j = 1;
	}
	else return -1;


	diff = (size - bufferlength);

	if (diff < 0) 
	{
		memcpy (Output, buffer, size);
		return size;
	}
	if (left == 1)
		while (i < diff)
		{
			Output[i] = carac; 
			i++;
		}
	memcpy (Output + i, buffer, bufferlength);
	i += bufferlength;
	if (left == 0)
		while (i < size)
		{
			Output[i] = carac; 
			i++;
		}

	return size + j;
}

/* ----------------------------------------------------------------------
 Hexa representation in char to ascii decimal char 

 '"61 31 dz z" ---> "A 1 (208) (0)"
	Fill characters not hexa with 0
------------------------------------------------------------------------*/
 int HexaCharToChar (unsigned char *sta, unsigned char *std, unsigned int la)
{
	while (la > 0)
	{
		if (la % 2) *std = 0;
		else
		{
			if (*sta >= '0' && *sta <= '9')  *std = (unsigned char)(*sta - '0');
			else 
			if (*sta >= 'a' && *sta <= 'f') *std = (unsigned char)(*sta -'a' + 0x0A);
			else 
			if (*sta >= 'A' && *sta <= 'F') *std = (unsigned char)(*sta - 'A' + 0x0A);
			else 
				*std = 0;
			*std <<= 4;
			sta++;
			la--;
		}
		if (*sta >= '0' && *sta <= '9') *std += (unsigned char)(*sta - '0');
		else 
		if (*sta >= 'a' && *sta <= 'f') *std += (unsigned char)(*sta - 'a' + 0x0A);
		else 
		if (*sta >= 'A' && *sta <= 'F') *std += (unsigned char)(*sta - 'A' + 0x0A);
		else 
			*std += 0;
		sta++;
		la--;
		std++;
   }
	return (0);
}

/* ----------------------------------------------------------------------
   l must be double size of std
   "aZZZZZ111"----> "61 5A 5A 5A 5A 5A 31 31 31"
/------------------------------------------------------------------------*/
int CharToHexaChar (unsigned char *std, unsigned char *sta, unsigned int l)
{
	while (l > 0)
	{
		if (l % 2) ;
		else
		{
			*sta = (unsigned char)(*std >> 4);
			if (*sta > 9) *sta += ('A'- 0x0A);
			else 
				*sta += '0';
			sta++;
			l--;
		}
		*sta = (unsigned char)(*std & 0x0F);
		if(*sta > 9) *sta += ('A'- 0x0A);
		else 
			*sta += '0';
		sta++;
		l--;
		std++;
	}
	return (0);
}


/*!
 * \brief Copie une chaine de caracteres ascii reprsentant un
 * developpement hexa decimal en valeur HexaDecimale
 * ex :  '3141' -> "1A"
 *
 * \param Output Pointeur de la chaine resultat
 * \param Input Pointeur de la chaine  origine (2*lg(Size))
 * \param Size Nombre de caracteres hexadecimal
 *
 */
void NumericDecimalCharToHexaChar(char* Input, int Size, char* Output)
{

    char buf [3];
    int rt;

    for ( ; Size; Size-- ) {

        memcpy( buf, Input, 2 );
        buf [2] = '\0';
        
		sscanf((char * )buf, "%02x", &rt);

        *Output++ = ( char )( rt % 256 );
        
		Input += 2;
    }/* for */
}


void DateGetGMTDateHour (char* strdate, char* strtime)
{
	struct timeb t;
    struct tm *ptm;
    ftime(&t);

    ptm = gmtime(&t.time);
    sprintf (strdate, "%02d%02d%02d", ptm->tm_year, ptm->tm_mon + 1, ptm->tm_mday);
    sprintf (strtime, "%02d%02d%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
}


void DateGetLocalDateHour (char* strdate, char* strtime)
{
	time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

	sprintf(strdate, "%02d%02d%02d",timeinfo->tm_year-100, timeinfo->tm_mon + 1, timeinfo->tm_mday);
	sprintf(strtime, "%02d%02d%02d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}


int EMVLuhnCardValidator(char* cardNumbers, int size) 
{
    int sum = 0, nxtDigit, i;

    for (i = 0; i < size ; i++) 
	{
         nxtDigit = cardNumbers[i] - '0';
    
		 if (i % 2 == 0) 
          nxtDigit  = (nxtDigit > 4) ? (nxtDigit * 2 - 10) + 1 : nxtDigit * 2;
        
		 sum += nxtDigit;
    }
    
	return ((sum % 10) == 0);
}

int EMVNumberCardValidator (char *cardNumbers, int size)
{
	int i;
	if (size > 16)
	{
		return 0;
	}
	// number to validate

	for (i = 0; i < size; i++) 
		if (!isdigit(cardNumbers[i]))
			return 0;
	return 1;
}

int EMVTypeCardValidator (char *cardNumbers, int size)
{
	int number[19]; 
	int i;
	
	for (i = 0; i < size; i++) 
		number[i] = cardNumbers[i] - '0';

	if ((size == 16  || size == 19) && number[0] == 6)
		return EMV_CARDTYPE_IS_UP;
	else
	if (size == 16 && number[0] == 5 && number[1] != 0 && number[1] <= 5)
		return EMV_CARDTYPE_IS_MASTERCARD;
	else
	if (size == 16 && number[0] == 5 && number[1] == 6 && number[2] <= 1)
		return EMV_CARDTYPE_IS_BANKCARD;
	else
	if ((size == 16 || size == 13)  && number[0] == 4)
		return EMV_CARDTYPE_IS_VISA;
	else
	if (size == 15  && number[0] == 3 && (number[1] == 4 || number[1] == 7))
		return EMV_CARDTYPE_IS_AMEX;
	else
	if (size == 16 && number[0] == 6 && number[1] == 0 && number[2] == 1 && number[3] == 1)
		return EMV_CARDTYPE_IS_DISCOVER;
	else
	if (size == 14 && number[0] == 3 && (number[1] == 0 || number[1] == 6 || number[1] == 8) && (number[1] != 0 || number[2] <= 5))
		return EMV_CARDTYPE_IS_DINERS;
	else
	if ((size == 16 || size == 15) && number[0] == 3 && number[1] == 5)
		return EMV_CARDTYPE_IS_JCB;
	else
		return EMV_CARDTYPE_IS_UNKNOWN;
}

void EMVCardValidator (EMV* pemv, EMVClient* pclient, char *cardNumbers, int size)
{
	if (!EMVNumberCardValidator (cardNumbers, size))
	{
		if (pemv->DebugEnabled)
			printf("%5sCredit PAN Number: %s is not a valid Number\n", "", cardNumbers);
	}
	if (!EMVLuhnCardValidator (cardNumbers, size))
	{
		if (pemv->DebugEnabled)
			printf("%5sCredit PAN Number: %s is not a valid Number after Luhn Card Validator\n", "", cardNumbers);
	}
	else
		if (pemv->DebugEnabled)
			printf("%5sCredit PAN Number: %s is a valid Number after Luhn Card Validator\n", "", cardNumbers);

	pclient->CardType = EMVTypeCardValidator (cardNumbers, size);

}