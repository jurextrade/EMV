/* ***************************************************************************
* Thales-e-Transactions
* ---------------------------------------------------------------------------
* Station Manager
* ---------------------------------------------------------------------------
* Key Manager
* ---------------------------------------------------------------------------
* KYM_DEF.h : Private Definitions of the Key Manager
* ---------------------------------------------------------------------------
* File history:
*  01/04/04 APR : Creation
* *************************************************************************** */
#ifndef KYM_ALGOPVV_H
#define KYM_ALGOPVV_H


/*
**	PVV Definitions
*/

/*	LENGTH */
#define	LENGTH_KEY						8
#define	LENGTH_PVV						4
#define	LENGTH_ISSUER_CODE			6
#define	LENGTH_CUSTOMER_CODE		6
#define	LENGTH_CARD_NUMBER			4
#define	LENGTH_CODE					4
#define	LENGTH_TRACK					37
#define	LENGTH_TRACK_MAX				40

#define	LENGTH_MAC_INPUT				24

/* TYPE OF PIN */
#define	TYPE_PIN_4						'4'
#define	TYPE_PIN_9						'9'

/* POSITION ON TRACK ISO2 */
#define	INDEX_ISSUER_CODE						0
#define	INDEX_CUSTOMER_CODE					LENGTH_ISSUER_CODE
#define	INDEX_CARD_NUMBER					INDEX_CUSTOMER_CODE+LENGTH_CUSTOMER_CODE
#define	INDEX_DUPLICATA_NUMBER				INDEX_CARD_NUMBER+LENGTH_CARD_NUMBER
#define	INDEX_LUHN_KEY						INDEX_DUPLICATA_NUMBER+1
#define	INDEX_FILLER							INDEX_LUHN_KEY+1
#define	INDEX_EXPIRATION_DATE				INDEX_FILLER+1
#define	INDEX_EXCHANGE_FLAG					INDEX_EXPIRATION_DATE+LENGTH_DATE_TRACK
#define	INDEX_SERVICE_CODE_1					INDEX_EXCHANGE_FLAG+1
#define	INDEX_SERVICE_CODE_2					INDEX_SERVICE_CODE_1+1
#define	INDEX_SERVICE_CODE_3					INDEX_SERVICE_CODE_2+1
#define	INDEX_SERVICE_CODE_4					INDEX_SERVICE_CODE_3+1
#define	INDEX_PIN_TYPE							INDEX_SERVICE_CODE_4+1
#define	INDEX_PVV								INDEX_PIN_TYPE+1
#define	INDEX_SUPPORT_TYPE					INDEX_PVV+LENGTH_PVV
#define	INDEX_UNDEFINED						INDEX_SUPPORT_TYPE+1
#define	INDEX_RELEASE							INDEX_UNDEFINED+LENGTH_UNDEFINED



/*
**	External Functions
*/


unsigned char ucPVV_ControlDKV(unsigned char* pucTrack, unsigned char* pucSecretCode, unsigned char* pucPINOffset, unsigned char* pucKey);
unsigned char ucPVV_ControlUTA(unsigned char ucPinType, unsigned char* pucTrack, unsigned char* pucSecretCode, unsigned char* pucPINOffset,unsigned char* pucKey);
unsigned char ucPVV_ControlGR(unsigned char ucPinType, unsigned char* pucTrack, unsigned char* pucSecretCode, unsigned char* pucDriverCode,
			unsigned char* pucPVV, unsigned char* pucKey,unsigned char* pucRES2);
unsigned char bKEY_PVV_Control(unsigned char ucAlgorithm, unsigned char ucPinType, unsigned char* pucTrack, unsigned char* pucSecretCode,
			unsigned char* pucDriverCode, unsigned char* pucPVV, unsigned char* pucCrypto, unsigned char* pucRES2);

void vKEY_CalculateMAC(unsigned char* pucKey, unsigned char* pucData, unsigned int usDataLength, unsigned char* pucRES1);
#endif /* KYM_ALGOPVV_H */
