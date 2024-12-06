#ifndef __EMVTAG_H
#define __EMVTAG_H

#include "EMV.h"

#define INS_SELECT									0xA4
#define INS_READ_RECORD								0xB2
#define INS_GET_DATA								0xCA

#define INS_GET_CHALLENGE							0x84
#define INS_GET_PROCESSING_OPTIONS					0xA8
#define INS_GENERATE_AC								0xAE
#define INS_VERIFY									0x20	


#define TAG_ADF_NAME								0x4F
#define TAG_APPLICATION_LABEL						0x50
#define TAG_PAN										0x5A

#define TAG_APPLICATION_TEMPLATE					0x61
#define TAG_FCI_TEMPLATE							0x6F
#define TAG_DF_NAME									0x84
#define TAG_APPLICATION_PRIORITY_INDICATOR			0x87
#define TAG_SFI										0x88
#define TAG_CDOL_1									0x8C
#define TAG_CDOL_2									0x8D
#define TAG_FCI_PROPRIETARY_TEMPLATE				0xA5
#define TAG_AIP										0x82
#define TAG_AFL										0x94
#define TAG_RESPONSE_FORMAT_1						0x80
#define TAG_RESPONSE_FORMAT_2						0x77
#define TAG_READ_RECORD_RESPONSE_TEMPLATE			0x70
#define	TAG_CVM_LIST								0x8E
#define TAG_LANGUAGE_PREFERENCE						0x5F2D
#define TAG_APPLICATION_EXPIRATION_DATE				0x5F24
#define TAG_APPLICATION_EFFECTIVE_DATE				0x5F25

#define TAG_ISSUER_CODE_TABLE_INDEX					0x9F11
#define TAG_ISSUER_COUNTRY_CODE						0x9F57

#define TAG_ISSUER_PUBLIC_KEY_CERTIFICATE			0x90
#define TAG_ISSUER_PUBLIC_KEY_REMAINDER				0x92
#define TAG_ISSUER_PUBLIC_KEY_EXPONENT				0x9F32
#define TAG_SIGNED_STATIC_APPLICATION_DATA			0x93
#define TAG_CERTIFICATION_AUTHORITY_PUBLIC_KEY_INDEX_CARD 0x8F
#define TAG_CERTIFICATION_AUTHORITY_PUBLIC_KEY_INDEX_TERMINAL  0x9F22


//STRIPE EQUIVALENT DATA

#define TAG_SERVICE_CODE							0x5F30
#define TAG_TRACK1_EQUIVALENT_DATA					0x56
#define TAG_TRACK1_DISCRETIONARY_DATA				0x9F1F
#define TAG_CARD_HOLDER_NAME						0x5F20
#define TAG_CARD_EXPIRATION_DATE					0x59
#define TAG_ISSUER_IDENTIFICATION_NUMBER			0x42


#define TAG_TRACK2_EQUIVALENT_DATA					0x57
#define TAG_TRACK2_DISCRETIONARY_DATA				0x9F20


#define TAG_TRACK3_EQUIVALENT_DATA					0x58
//DDA
#define TAG_ICC_PUBLIC_KEY_CERTIFICATE				0x9F46
#define TAG_ICC_PUBLIC_KEY_REMAINDER				0x9F48
#define TAG_ICC_PUBLIC_KEY_EXPONENT					0x9F47
#define TAG_DYNAMIC_DATA_AUTHENTIFICATION_DATA_OBJECT_LIST	0x9F49


#define TAG_APPLICATION_TRANSACTION_COUNTER			0x9F36
#define TAG_LAST_ONLINE_APPLICATION_TRANSACTION_COUNTER		0x9F13


#define TAG_APPLICATION_VERSION_NUMBER_CARD			0x9F08
#define TAG_APPLICATION_VERSION_NUMBER_TERMINAL		0x9F09
#define TAG_CVM_RESULTS								0x9F34

#define	TAG_LAST_ONLINE_ATC_REGISTER				0x9F13
#define	TAG_UCOL									0x9F23
#define	TAG_LCOL									0x9F14

#define	TAG_ATC										0x9F36
#define TAG_CID										0x9F27  //Cryptogram InformationData
#define TAG_AC										0x9F26  //Cryptogram returned by the ICC in response of the GENERATE AC or RECOVER AC command

#define TAG_IAD										0x9F10	//Issuer Application Data (IAD)
#define TAG_PDOL									0x9F38
#define	TAG_AUC										0x9F07
#define	TAG_CTQ										0x9F6C
#define	TAG_TTQ										0x9F66

#define TAG_APPLICATION_CAPABILITIES_INFORMATION    0x9F5D                   //(ACI)
#define TAG_APPLICATION_PRIMARY_ACCOUNT_NUMBER_SEQUENCE_NUMBER 0x5F34        //(PSN)

#define TAG_APPLICATION_PREFERRED_NAME				0x9F12
#define TAG_APPLICATION_CURRENCY_CODE				0x9F42
#define TAG_APPLICATION_CURRENCY_EXPONENT			0x9F44

#define TAG_APPLICATION_REFERENCE_CURRENCY_CODE		0x9F3B
#define TAG_APPLICATION_REFERENCE_CURRENCY_EXPONENT	0x9F43

#define TAG_FCI_ISSUER_DISCRETIONARY_DATA			0xBF0C

//ISSUER/TERMINAL

#define TAG_AUTHORISATION_CODE						0x8A    //an2

//TERMINAL
#define TAG_TVR										0x95	//Status of the different functions as seen from the terminal
#define TAG_TSI										0x9B

#define TAG_COMMAND_TEMPLATE						0x83
#define TAG_TERMINAL_AID							0x9F06	//Identifies the application as described in ISO/IEC 7816-5
#define TAG_ACQUIRER_IDENTIFIER						0x9F01	//Uniquely identifies the acquirer within each payment system
#define TAG_APPLICATION_VERSION_NUMBER				0x9F09	//Version number assigned by the payment system for the Kernel application
#define TAG_MERCHANT_CATEGORY_CODE					0x9F15  //Classifies the type of business being done by the merchant, represented according to ISO 8583:1993 for Card Acceptor Business Code
#define TAG_MERCHANT_IDENTIFIER						0x9F16	//Indicates the name and location of the merchant
#define TAG_MERCHANT_NAME_AND_LOCATION				0x9F4E



#define TAG_TERMINAL_FLOOR_LIMIT					0x9F1B
#define TAG_TERMINAL_IDENTIFICATION					0x9F1C
#define TAG_TERMINAL_RISK_MANAGEMENT_DATA			0x9F1D	//Application-specific value used by the card for risk management purposes


#define TAG_ISSUER_ACTION_CODE_DEFAULT				0x9F0D
#define TAG_ISSUER_ACTION_CODE_DENIAL				0x9F0E
#define TAG_ISSUER_ACTION_CODE_ONLINE				0x9F0F

#define TAG_TERMINAL_ACTION_CODE_DEFAULT			0xDF56 //0xFF0D
#define TAG_TERMINAL_ACTION_CODE_DENIAL				0xDF57 //0xFF0E
#define TAG_TERMINAL_ACTION_CODE_ONLINE				0xDF58 //0xFF0F


//TERMINAL TAGS

#define TAG_TERMINAL_COUNTRY_CODE					0x9F1A
#define TAG_TERMINAL_CAPABILITIES					0x9F33
#define TAG_ADDITIONAL_TERMINAL_CAPABILITIES		0x9F40	//Indicates the data input and output capabilities of the Terminal and Reader. The Additional Terminal Capabilities is coded according to Annex A.3 of [EMV Book 4].
#define TAG_TERMINAL_TYPE							0x9F35
#define TAG_IFD_SERIAL_NUMBER						0x9F1E

// TRANSACTION TAGS
#define TAG_TRANSACTION_CURRENCY_CODE				0x5F2A
#define TAG_TRANSACTION_CURRENCY_EXPONENT			0x5F36
#define TAG_TRANSACTION_REFERENCE_CURRENCY_CODE		0x9F3C
#define TAG_TRANSACTION_REFERENCE_CURRENCY_EXPONENT	0x9F3D

#define TAG_TRANSACTION_TIME						0x9F21
#define TAG_TRANSACTION_DATE						0x9A
#define TAG_TRANSACTION_TYPE						0x9C
#define TAG_TRANSACTION_SEQUENCE_COUNTER			0x9F41
#define TAG_AMOUNT_AUTHORIZED						0x9F02
#define TAG_AMOUNT_OTHER							0x9F03
#define	TAG_UNPREDICTABLE_NUMBER					0x9F37
#define TAG_ICC_DYNAMIC_NUMBER						0x9F4C
#define TAG_DATA_AUTHENTIFICATION_CODE				0x9F45

#define TAG_POS_ENTRY_MODE							0x9F39

// NEW TAG

#define TAG_SIRET									0xDF5D
#define TAG_IDPA									0xDF5C
#define TAG_IDSA									0xDF5E
#define TAG_PA_NUMBER								0xDF5B
#define TAG_SA_NUMBER								0xDF51
#define TAG_MERCHANT_CONTRACT_NUMBER				0xDF5F
#define TAG_TRANSMISSION_DATETIME					0xDF5A

typedef struct _EMVTag
{
	int				Tag;
	char			strTag[7];
	char			Name[200];
	char			Format[20];
	BYTE			DataType;
	BYTE			IsVariable;
	int				Size;
	char			Origin[20];
}EMVTag;


#ifdef __cplusplus
extern "C"
{
#endif





#ifdef __cplusplus
};
#endif

#endif // __EMVTAG_H
