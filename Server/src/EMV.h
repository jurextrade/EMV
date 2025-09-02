#ifndef __EMV_H
#define __EMV_H
#define _CRT_SECURE_NO_WARNINGS
#include "mx.h"
#include "tlv.h"
#include "cb2a.h"

#include "EMVTag.h"
#include "EMVTools.h"
#include "EMVConnect.h"
#include "EMVError.h"
#include "EMVFile.h"
#include "EMVTrace.h"



#define MAX_CANDIDATE_APPLICATIONS 20

//PROCESS STEP AND SUBSTEP

	
#define EMV_STEP_CARD_DETECTION_AND_RESET						0

#define EMV_STEP_CANDIDATE_LIST_CREATION						1
	#define EMV_SUBSTEP_ATTEMP_DIRECTORY_LISTING_PSE			100
	#define EMV_SUBSTEP_READ_RECORD_SFI							101
	#define EMV_SUBSTEP_BUILD_CANDIDATE_LIST_WITH_SELECT		102

#define EMV_STEP_APPLICATION_SELECTION							2
	#define EMV_SUBSTEP_SELECT_APPLICATION_FILE					200
	#define EMV_SUBSTEP_GET_PROCESSING_OPTIONS					201

#define EMV_STEP_READ_APPLICATION_DATA							3
	#define EMV_SUBSTEP_COMPLETE_EQUIVALENTDATA_TAGS			300
	#define	EMV_SUBSTEP_CHECK_MANDATORY_DATA					301
	#define	EMV_SUBSTEP_CHECK_MISSING_DATA						302

#define EMV_STEP_DATA_AUTHENTIFICATION							4
	#define	EMV_SUBSTEP_CHECK_TERMINAL_TYPE						400
	#define EMV_SUBSTEP_SELECTION_OFFLINE_AUTHENTIFICATION_MECHANISM 401
	#define EMV_SUBSTEP_OFFLINE_SDA								402
	#define EMV_SUBSTEP_OFFLINE_DDA								403
	#define EMV_SUBSTEP_CDA										404

#define EMV_STEP_PROCESSING_RESTRICTIONS						5
	#define EMV_SUBSTEP_APPLICATION_VERSION_NUMBER				500
	#define EMV_SUBSTEP_APPLICATION_USAGE_CONTROL				501
	#define EMV_SUBSTEP_APPLICATION_EFFECTIVE_EXPIRATION_DATES	502

#define EMV_STEP_CARD_HOLDER_VERIFICATION						6
	#define EMV_SUBSTEP_CHECK_CVM_RULES							600

#define EMV_STEP_TERMINAL_RISK_MANAGEMENT						7
	#define EMV_SUBSTEP_EXCEPTION_LIST_CARDS_CHECKING			700
	#define EMV_SUBSTEP_CB_REGISTERED_BIN_CHECKING				701
	#define EMV_SUBSTEP_FLOOR_LIMIT_CHECKING					702
	#define EMV_SUBSTEP_RANDOM_TRANSACTION_SELECTION			703
	#define EMV_SUBSTEP_VELOCITY_CHECKING						704

#define EMV_STEP_TERMINAL_ACTION_ANALYSIS						8
	#define EMV_SUBSTEP_ACTION_IF_DENIAL						800
	#define EMV_SUBSTEP_ACTION_IF_ONLINE						801
	#define EMV_SUBSTEP_ACTION_IF_APPROVED						802

#define EMV_STEP_CARD_ACTION_ANALYSIS							9
	#define EMV_SUBSTEP_GENERATE_AC_FIRST						900

#define EMV_STEP_ONLINE_OFFLINE_DECISION						10

#define EMV_STEP_ONLINE_PROCESSING								11
	#define EMV_SUBSTEP_CONNECT_ACQUIRER						1100
	#define EMV_SUBSTEP_PREPARE_AND_SEND_TO_ACQUIRER			1101

#define EMV_STEP_SCRIPT_PROCESSING								12
	#define EMV_SUBSTEP_CHECK_ISSUER_SCRIPTS_TEMPLATE			1200
	#define EMV_SUBSTEP_POST_ISSUANCE_COMMANDS    				1201

#define EMV_STEP_TRANSACTION_COMPLETION							13
	#define EMV_SUBSTEP_GENERATE_AC_SECOND						1300
	#define EMV_SUBSTEP_TERMINATE_TRANSACTION					1301



#define EMV_OK							 0	// Ok result
#define EMV_NEED_CONFIRM_APPLICATION	 1	// Cardholder must confirm application
#define EMV_NEED_SELECT_APPLICATION		 2	// Cardholder must selection application from application list
#define EMV_UNKNOWN_ERROR				-1	// Some other error
#define EMV_ERROR_TRANSMIT				-2	// Communication error
#define EMV_NOT_SUPPORTED				-3	// The command is not supported by the ICC (SW1 SW2 = '6A81'), the terminal terminates the card session
#define EMV_TERMINATED					-4	// Transaction is terminated
#define EMV_NOT_SATISFIED				-5	// Get processing option failed

#define EMV_SKIP_OFFLINE_AUTHENTIFICATION	1
#define	EMV_NO_MATCH_AUTHENTIFICATION		4
#define EMV_NEXT_RULE						2
// Bit map, please control out of limits

//Card Type
#define EMV_CARDTYPE_IS_UNKNOWN				0
#define EMV_CARDTYPE_IS_VISA				1
#define EMV_CARDTYPE_IS_MASTERCARD			2
#define EMV_CARDTYPE_IS_AMEX				3
#define EMV_CARDTYPE_IS_BANKCARD			4
#define EMV_CARDTYPE_IS_DISCOVER			5
#define EMV_CARDTYPE_IS_DINERS				6
#define EMV_CARDTYPE_IS_JCB					7
#define EMV_CARDTYPE_IS_UP					8


//CRYPTOGRAMS TYPE
//TAG CID

#define EMV_CRYPTO_TYPE_AAC				0x00 // An AAC is a type of Application Cryptogram that is generated whenever a card declines a transaction during Card Action Analysis.
#define	EMV_CRYPTO_TYPE_TC				0x40 // A TC is a type of Application Cryptogram that is generated whenever a card approves a transaction during Card Action Analysis.
#define	EMV_CRYPTO_TYPE_ARQC			0x80 // An ARQC is a type of Application Cryptogram that is generated whenever the card requests online authorization during Card Action Analysis.
#define	EMV_CRYPTO_TYPE_AAR				0xC0 // Referral requested by the card

#define EMVCardReturnAAR(pclient)       ((pclient->EMV_CID[B1]  & EMV_CRYPTO_TYPE_AAR) == EMV_CRYPTO_TYPE_AAR)
#define EMVCardReturnARQC(pclient)      ((pclient->EMV_CID[B1]  & EMV_CRYPTO_TYPE_ARQC) == EMV_CRYPTO_TYPE_ARQC)
#define EMVCardReturnTC(pclient)        ((pclient->EMV_CID[B1]  & EMV_CRYPTO_TYPE_TC) == EMV_CRYPTO_TYPE_TC)
#define EMVCardReturnAAC(pclient)       ((pclient->EMV_CID[B1]  & EMV_CRYPTO_TYPE_AAR) == EMV_CRYPTO_TYPE_AAC)


#define EMV_CRYPTO_TYPE_PSS				0x30 // Payment System-specific cryptogram",
#define EMV_CRYPTO_AR					0x08 // Advice Required", "YES");
#define EMV_CRYPTO_SNA					0x01 // Service not allowed", "YES");
#define EMV_CRYPTO_TLE					0x02 // PIN Try Limit exceeded", "YES");
#define EMV_CRYPTO_IAF					0x03 // Issuer authentication failed", 
#define EMV_CRYPTO_RFU					0x04 // Other Values RFU", 
#define EMV_CRYPTO_NIG					0x00 // No information given, 



#define B1								0x00
#define B2								0x01
#define B3								0x02
#define B4								0x03
#define B5								0x04

#define	b8								0x80
#define	b7								0x40
#define b6								0x20
#define b5								0x10
#define	b4								0x08
#define	b3								0x04
#define b2								0x02
#define b1								0x01
#define b0								0x00


//status pan 
#define Card_accepted					0x00
#define	Card_forbidden					0x01
#define	Card_watched					0x02	
#define	Card_refused					0x03

//Status CB
#define	Bin_accepted					0x00
#define	Bin_watched						0x01	
#define	Bin_forbidden					0x02
#define	Bin_refused						0x03



//TVR
#define Offline_data_authentication_was_not_performed				0x0080
#define SDA_failed													0x0040
#define ICC_data_missing											0x0020
#define Card_appears_on_terminal_exception_file						0x0010
#define DDA_failed													0x0008
#define CDA_failed													0x0004
#define SDA_selected												0x0002

#define ICC_and_terminal_have_different_application_versions		0x0180
#define Expired_application											0x0140
#define Application_not_yet_effective								0x0120
#define Requested_service_not_allowed_for_card_product				0x0110
#define New_Card													0x0108

#define Cardholder_verification_was_not_successful					0x0280
#define Unrecognised_CVM											0x0240
#define PIN_Try_Limit_exceeded										0x0220
#define PIN_entry_required_and_PIN_pad_not_present_or_not_working	0x0210
#define PIN_entry_required_PIN_pad_present_but_PIN_was_not_entered	0x0208
#define Online_PIN_entered											0x0204

#define Transaction_exceeds_floor_limit								0x0380
#define Lower_consecutive_offline_limit_exceeded					0x0340
#define Upper_consecutive_offline_limit_exceeded					0x0320
#define Transaction_selected_randomly_for_online_processing			0x0310
#define Merchant_forced_transaction_online							0x0308

#define Default_TDOL_used											0x0480
#define Issuer_authentication_failed								0x0440
#define Script_processing_failed_before_final_GENERATE_AC			0x0420
#define Script_processing_failed_after_final_GENERATE_AC			0x0410


// TSI
#define Offline_data_authentication_was_performed					0x0080
#define Cardholder_verification_was_performed						0x0040
#define Card_risk_management_was_performed							0x0020
#define Issuer_authentication_was_performed							0x0010
#define Terminal_risk_management_was_performed						0x0008
#define Script_processing_was_performed								0x0004
														
//CVR 
/*
Byte 1 Bit 3 = 1  Offline PIN Verification Performed            0x0004
Byte 1 Bit 2 = 1  Offline Encrypted PIN Verification Performed  0x0002
Byte 1 Bit 1 = 1  Offline PIN Verification Successful           0x0001



Byte 2 Bit 8 = 1  DDA Returned                              
Byte 2 Bit 7 = 1  Combined DDA/AC Generation Returned In First Generate AC
Byte 2 Bit 6 = 1  Combined DDA/AC Generation Returned In Second Generate AC
Byte 2 Bit 5 = 1  Issuer Authentication Performed
Byte 2 Bit 4 = 1  CIAC-Default Skipped On CAT3

Byte 3 Bits 8-5  Right nibble of Script Counter = 15
Byte 3 Bits 4-1  Right nibble of PIN Try Counter = 15

Byte 4 Bit 7 = 1  Unable To Go Online Indicated
Byte 4 Bit 6 = 1  Offline PIN Verification Not Performed
Byte 4 Bit 5 = 1  Offline PIN Verification Failed
Byte 4 Bit 4 = 1  PTL Exceeded
Byte 4 Bit 3 = 1  International Transaction
Byte 4 Bit 2 = 1  Domestic Transaction
Byte 4 Bit 1 = 1  Terminal Erroneously Considers Offline PIN OK


Byte 5 Bit 8 = 1  Lower Consecutive Offline Limit Exceeded
Byte 5 Bit 7 = 1  Upper Consecutive Offline Limit Exceeded
Byte 5 Bit 6 = 1  Lower Cumulative Offline Limit Exceeded
Byte 5 Bit 5 = 1  Upper Cumulative Offline Limit Exceeded
Byte 5 Bit 4 = 1  Go Online On Next Transaction Was Set
Byte 5 Bit 3 = 1  Issuer Authentication Failed
Byte 5 Bit 2 = 1  Script Received
Byte 5 Bit 1 = 1  Script Failed

Card verification results  
Byte 2 Bit 8 = 1, Byte 2 Bit 7 = 0  Second GENERATE AC not requested
Byte 2 Bit 6 = 1, Byte 2 Bit 5 = 0  ARQC Returned in GPO/first GENERATE AC
Byte 4 Bits 8-5  Issuer Script Commands processed on last transaction = 0
*/
typedef BYTE EMV_BITS;
	
//A6 Authorisation Response Code When transmitted to the card, the Authorisation Response Code obtained 
//from the authorisation response message shall include at least the following: 
// Online approved 
// Online declined 
// Referral (initiated by issuer) 
// Capture card 
//In addition, the terminal shall be able to generate and transmit to the card the new response codes listed in Table 35 
//when transactions are not authorised online: 
 // TAG 8A
//Authorisation Response Code							   Value 
#define Online_approved										(BYTE*)"Y1" 
#define Online_declined										(BYTE*)"Z1" 
#define Unable_to_go_online_offline_approved				(BYTE*)"Y3" 
#define Unable_to_go_online_offline_declined				(BYTE*)"Z3" 


//Table 35:  Authorisation Response Codes The terminal shall never modify the Authorisation Response Code returned in the response message.25 



// Structure of one Certification Authority Public Key
typedef struct _EMVAuthorityPublicKey
{
	BYTE			CertificationAuthorityPublicKeyIndex;	// Certification Authority Public Key Index
	BYTE			IssuerPublicKeyExponent[3];				// Ex. {0x00, 0x00, 0x03}
	int				IssuerPublicKeyModulusSize;				// Size of public key in bits. Ex. 1152
	BYTE			IssuerPublicKeyModulus[256];			// Modulus of public key, maximum: 248 x 8 = 1984 bits
	BYTE			IssuerPublicKeyRemainder[128];			
	BYTE			CheckSum[20];							// A check value calculated on the concatenation of all parts of the Certification Authority Public Key
} EMVAuthorityPublicKey;


typedef struct _EMVExceptionCard
{
	BYTE			PanSize;
	BYTE			Pan[10];								//n19 Pan number	
	BYTE			Status;									//Status of the card
}EMVExceptionCard;

typedef struct _EMVRangeBin
{
	BYTE			FromBinSize;
	BYTE			FromBin[10];								
	BYTE			ToBinSize;
	BYTE			ToBin[10];	
	BYTE			Status;
	BYTE			TreatmentCode;									
}EMVRangeBin;


typedef struct _EMVAcquirerConnection {
	char			Host[250];
	int				Port;
	char*			Output;
	char*			Input;
	int				Error;
	MXCom*			pCom;
}EMVAcquirerConnection;

typedef struct _EMVAcquirer {

	BYTE			AcquirerIdentifier[6];					//n6..11 9F01
	EMVAcquirerConnection* pConnection;						//Current Connection
	List*			Connections;
}EMVAcquirer;

//Enseigne de l’accepteur ansc 60  //DF04  DF20, DF21, DF22, DF23 
//Identification organisme acquéreur ansc 11 
//Type d’activité commerciale ansc 4						 MerchantCategoryCode
//Réservé usage futur (renseigné à blanc) ansc 2 
//Numéro contrat accepteur ansc 7 
//Enveloppe 41 : valeur à utiliser dans le champ 41 ansc 8    Terminal Identification 9F1C
//Enveloppe 42 : valeur à utiliser dans le champ 42 ansc 15   Merchant Identifier 9F16

typedef struct _EMVAcceptor {
	char			MerchantNameAndLocation[41];			// Null terminated C string, Ex. "202B, USELU LAGOS ROAD BENIN CITY" //9F4E ans...40
	BYTE			MerchantCategoryCode[2];				// Merchant Category Code, Ex. {0x30, 0x01} //9F15 n4
	char			MerchantContractNumber[8];				// NEW TAG DF5F  ans7
	char			TerminalIdentification[9];				// Designates the unique location of a terminal at a merchant, Ex. "EMVPOS4 " //9F1C an8
	char			MerchantIdentifier[16];					// Merchant Identifier, Ex. "000000000018003" ans15 //9F16
	char			MerchantBillMode;						// MODE FACTURATION TELECOM		//DF20 
	char			MerchantActivationCode;					// CODE ACTIVATION MODE APPELE. //DF21
	char			SystemAcceptationIdentification[9];		// NEW TAG DF5E					//Type de Site ?? DF23
	char			SIRET[15];								// NEW TAG DF5D					//DF22
}EMVAcceptor;


typedef struct _EMVPointOfSale {
	MXCom*			pCom;
	char			PointAcceptationIdentification[9];		// NEW TAG DF5C
	//char			SystemAcceptationIdentification[9];		// NEW TAG DF5E
	char			PointAcceptationLogicalNumber[4];		// NEW TAG DF5B
	char			SystemAcceptationLogicalNumber[4];		// NEW TAG DF51


}EMVPointOfSale;

#define EMV_TRANSACTION_TYPE_PURCHASE						0x00
#define EMV_TRANSACTION_TYPE_CASH_ADVANCE					0x01
#define EMV_TRANSACTION_TYPE_PURCHASE_CASHBACK				0x09
#define EMV_TRANSACTION_TYPE_REFUND							0x20

#define EMV_POS_ENTRY_MODE_Unknown                                                         0x00
#define EMV_POS_ENTRY_MODE_Manual                                                          0x01
#define EMV_POS_ENTRY_MODE_Magnetic_Stripe                                                 0x02
#define EMV_POS_ENTRY_MODE_Bar_Code                                                        0x03
#define EMV_POS_ENTRY_MODE_OCR                                                             0x04
#define EMV_POS_ENTRY_MODE_ICC_CVV_can_be_checked                                          0x05
#define EMV_POS_ENTRY_MODE_contactless                                                     0x07
#define EMV_POS_ENTRY_MODE_Fallback_Magnetic_Stripe                                        0x80
#define EMV_POS_ENTRY_MODE_Magnetic_Stripe_read_from track2_CVV_can_be_checked             0x90
#define EMV_POS_ENTRY_MODE_Auto_entry_via_contactless_magnetic_stripe                      0x91
#define EMV_POS_ENTRY_MODE_ICC_CVV_may_not_be_checked                                      0x95
#define EMV_POS_ENTRY_MODE_Same_as_original_transaction                                    0x99



typedef struct _EMVTransaction {
	BYTE			TransactionCurrencyCode[2];					//5F2A
	BYTE			TransactionCurrencyExponent;				//5F36
	BYTE			TransactionTime[3];							//9F21
	BYTE			TransactionDate[3];							//9A
	BYTE			TransactionType;							//9C
	BYTE			TransactionSequenceCounter[3];				//9F41
	BYTE			AmountAuthorized[6];						//9F02
	BYTE			AmountOther[6];                             //9F03 
	BYTE			UnpredictableNumber[4];						//9F37
	BYTE			ICCDynamicNumber[4];						//9F4C
	BYTE			DataAuthentificationCode[2];				//9F45
    BYTE            POSEntryMode;                               //9F39      Contactless or ICC
	BYTE			AmountAuthorisation[6];						// NO TAG FIELD 4 CB2A
	BYTE			SecurityRelatedControlInformation[8];       // NO TAG FIELD 53 CB2A

}EMVTransaction;

// Application info for select application
typedef struct _EMVSelectApplicationInfo
{
	int				DFNameLength;
	BYTE			DFName[16];								// byte data with size DFNameLength
	char			strApplicationLabel[17];				// Null terminated c string
	BYTE			strApplicationVersionNumber;			// Version number assigned by the payment system for the application in the Card
	char			needCardholderConfirm;					// 1 - need confirm, 0 - selection without confirmation
	int				priority;								// 0 - no priority, else ranging from 1–15, with 1 being highest priority
	char			strLanguagePreference[9];				// Null terminated c string, 1-4 languages stored in order of preference, each represented by 2 alphabetical characters according to ISO 639
	BYTE			issuerCodeTableIndex;					// Indicates the code table according to ISO/IEC 8859 for displaying the Application Preferred Name
	char			strApplicationPreferredName[17];		// Preferred mnemonic associated with the AID, codepage: issuerCodeTableIndex
	int				indexRID;								// For internal use
	int				indexAID;								// For internal use
} EMVSelectApplicationInfo;
// Settings global, EMV book 4, Application Independent Data
/*
10 Data Management EMV 4.3 Book 4 10.1 Application Independent Data Cardholder, Attendant, and Acquirer  Interface Requirements Page 80   November 2011 
10.1 Application Independent Data The terminal resident application independent data elements identified in this section shall be initialised in and obtainable
from the terminal prior to the issuance of the GET PROCESSING OPTIONS command in the Initiate Application Processing function described in Book 3 section 10.1. 
The data shall not change during the transaction. Any data sent in the authorisation request message shall have the same value as that provided to the card as 
listed in the PDOL (if present) and CDOL1, including Local Date and Local Time if appropriate. 

10.1.1 Terminal Related Data The following data elements are application independent and shall be unique to the terminal (see section 5.3 for different terminal configurations): 
 IFD Serial Number 
 Local Date 
 Local Time 
 Terminal Country Code 
 Transaction Sequence Counter 
The terminal shall have parameters initialised so that it can identify what language(s) are supported to process the card’s Language Preference (see section 11.1). 
10.1.2 Transaction Related Data The following data elements are application independent and may be specific to each device constituting the terminal,
such as a host concentrating a cluster of devices (see Figure 2 for an example): 
 Additional Terminal Capabilities
 Terminal Capabilities
 Terminal Type The terminal shall be constructed in such a way that these data objects cannot be modified unintentionally or by unauthorised access. 
These data objects may be varied on a transaction by transaction basis which means that for each transaction, the terminal may invoke a different value for 
these data objects based on certain characteristics and parameters of the transaction (selection criteria). 
Support of this functionality is optional for the terminal. 
The implementation of this functionality is left to the discretion of the terminal manufacturer and is outside the scope of EMV. 
*/



   
//TERMINAL TYPE
//==============


//Environment						Financial Institution			Merchant			Cardholder 
//Attended    
//	Online only								11							21					— 
//	Offline with online capability			12							22					— 
//	Offline only							13							23					— 
//Unattended    
//	Online only								14							24					34 
//	Offline with online capability			15							25					35 
//	Offline only							16							26					36 


#define		EMV_TERMINAL_TYPE_11	0x11
#define		EMV_TERMINAL_TYPE_12	0x12
#define		EMV_TERMINAL_TYPE_13	0x13
#define		EMV_TERMINAL_TYPE_14	0x14
#define		EMV_TERMINAL_TYPE_15	0x15
#define		EMV_TERMINAL_TYPE_16	0x16
#define		EMV_TERMINAL_TYPE_21	0x21
#define		EMV_TERMINAL_TYPE_22	0x22
#define		EMV_TERMINAL_TYPE_23	0x23
#define		EMV_TERMINAL_TYPE_24	0x24
#define		EMV_TERMINAL_TYPE_25	0x25
#define		EMV_TERMINAL_TYPE_26	0x26
#define		EMV_TERMINAL_TYPE_34	0x34	
#define		EMV_TERMINAL_TYPE_35	0x35
#define		EMV_TERMINAL_TYPE_36	0x36

#define EMVTerminalIsAttendant(pclient) (pclient->pTerminal->TerminalType == EMV_TERMINAL_TYPE_11 || pclient->pTerminal->TerminalType == EMV_TERMINAL_TYPE_12 || pclient->pTerminal->TerminalType == EMV_TERMINAL_TYPE_13 || pclient->pTerminal->TerminalType == EMV_TERMINAL_TYPE_21 || pclient->pTerminal->TerminalType == EMV_TERMINAL_TYPE_22 || pclient->pTerminal->TerminalType == EMV_TERMINAL_TYPE_23)
#define EMVTerminalIsOnlineOnly(pclient) (pclient->pTerminal->TerminalType == EMV_TERMINAL_TYPE_11 || pclient->pTerminal->TerminalType == EMV_TERMINAL_TYPE_21 || pclient->pTerminal->TerminalType == EMV_TERMINAL_TYPE_14 || pclient->pTerminal->TerminalType == EMV_TERMINAL_TYPE_24)
#define EMVTerminalIsATM							FALSE

#define	EMV_TERMINAL_CAPABILITIES_INPUT_TABLE		0x00
#define	EMV_TERMINAL_CAPABILITIES_CVM_TABLE			0x01
#define	EMV_TERMINAL_CAPABILITIES_SECURITY_TABLE	0x02


//Terminal Capabilities TAG_TERMINAL_CAPABILITIES 0x9F33
//======================================================

//This section provides the coding for Terminal Capabilities: 
// Byte 1:  Card Data Input Capability 
// Byte 2:  CVM Capability //
// Byte 3:  Security Capability 
//In the tables:  A ‘1’ means that if that bit has the value 1, the corresponding ‘Meaning’ applies.   An ‘x’ means that the bit does not apply.  
 

//Byte 1 - Card Data Input Capability 

//b8 b7 b6 b5 b4 b3 b2 b1	Meaning 
//1 x x x x x x x			Manual key entry 
//x 1 x x x x x x			Magnetic stripe 
//x x 1 x x x x x			IC with contacts 
//x x x 0 x x x x			RFU 
//x x x x 0 x x x			RFU 
//x x x x x 0 x x			RFU 
//x x x x x x 0 x			RFU 
//x x x x x x x 0			RFU 


//Byte 2 - CVM Capability Byte 2 - CVM Capability 

//b8 b7 b6 b5 b4 b3 b2 b1	Meaning 
//1 x x x x x x x			Plaintext PIN for ICC verification 
//x 1 x x x x x x			Enciphered PIN for online verification 
//x x 1 x x x x x			Signature (paper) 
//x x x 1 x x x x			Enciphered PIN for offline verification 
//x x x x 1 x x x			No CVM Required 
//x x x x x 0 x x			RFU 
//x x x x x x 0 x			RFU 
//x x x x x x x 0			RFU 


//If the terminal supports a CVM of signature, the terminal shall be 
//an attended terminal (Terminal Type = 'x1', 'x2', or 'x3') 
//and shall support a printer (Additional Terminal Capabilities, byte 4, ‘Print, attendant’ bit = 1). 
 
//Terminal Capabilities Byte 3 - Security Capability 

//b8 b7 b6 b5 b4 b3 b2 b1		Meaning 
//1 x x x x x x x				SDA 
//x 1 x x x x x x				DDA  
//x x 1 x x x x x				Card capture 
//x x x 0 x x x x				RFU 
//x x x x 1 x x x				CDA 
//x x x x x 0 x x				RFU 
//x x x x x x 0 x				RFU 
//x x x x x x x 0				RFU 


#define Manual_key_entry						0x0080		
#define Magnetic_stripe							0x0040		
#define	IC_with_contacts						0x0020		

#define Plaintext_PIN_for_ICC_verification		0x0180		
#define Enciphered_PIN_for_online_verification  0x0140		
#define Signature_paper						    0x0120		
#define Enciphered_PIN_for_offline_verification 0x0110		
#define No_CVM_Required							0x0108		

#define SDA										0x0280		
#define DDA										0x0240		
#define Card_capture							0x0220		
#define CDA									    0x0208		


//Terminal Additional Capabilities AG_ADDITIONAL_TERMINAL_CAPABILITIES		0x9F40
//================================================================================

//b8 b7 b6 b5 b4 b3 b2 b1 Meaning
//1 x x x x x x x Cash
//x 1 x x x x x x Goods
//x x 1 x x x x x Services
//x x x 1 x x x x Cashback
//x x x x 1 x x x Inquiry 20
//x x x x x 1 x x Transfer 21
//x x x x x x 1 x Payment 22
//x x x x x x x 1 Administrative

//b8 b7 b6 b5 b4 b3 b2 b1 Meaning
//1 x x x x x x x Cash Deposit 23

//b8 b7 b6 b5 b4 b3 b2 b1 Meaning
//1 x x x x x x x Numeric keys
//x 1 x x x x x x Alphabetic and special characters keys
//x x 1 x x x x x Command keys
//x x x 1 x x x x Function keys


//b8 b7 b6 b5 b4 b3 b2 b1 Meaning 24
//1 x x x x x x x Print, attendant
//x 1 x x x x x x Print, cardholder
//x x 1 x x x x x Display, attendant
//x x x 1 x x x x Display, cardholder
//x x x x 0 x x x RFU
//x x x x x 0 x x RFU
//x x x x x x 1 x Code table 10
//x x x x x x x 1 Code table 9

//b8 b7 b6 b5 b4 b3 b2 b1 Meaning
//1 x x x x x x x Code table 8
//x 1 x x x x x x Code table 7
//x x 1 x x x x x Code table 6
//x x x 1 x x x x Code table 5
//x x x x 1 x x x Code table 4
//x x x x x 1 x x Code table 3
//x x x x x x 1 x Code table 2
//x x x x x x x 1 Code table 1

#define Cash									0x0380
#define Goods									0x0340
#define Services								0x0320
#define Cashback								0x0310
#define Inquiry 								0x0308
#define Transfer 								0x0304
#define Payment 								0x0302
#define Administrative							0x0301
#define Cash_Deposit							0x0480


#define Numeric_keys							0x0580
#define Alphabetic_and_special_characters_keys	0x0540
#define Command_keys							0x0520
#define Function_keys							0x0510


#define Print_attendant							0x0680
#define Print_cardholder						0x0640
#define Display_attendant						0x0620
#define Display_cardholder						0x0610

#define Code_table_10							0x0602
#define Code_table_9							0x0601

#define Code_table_8							0x0780
#define Code_table_7							0x0740
#define Code_table_6							0x0720
#define Code_table_5							0x0710
#define Code_table_4							0x0708
#define Code_table_3							0x0704
#define Code_table_2							0x0702
#define Code_table_1							0x0701
									


#define EMVTerminalAndCardSupportCDA(pclient) ((pclient->EMV_AIP[B1] & b1) && (pclient->pTerminal->TerminalCapabilities[EMV_TERMINAL_CAPABILITIES_SECURITY_TABLE] & b4))
#define EMVTerminalAndCardSupportDDA(pclient) ((pclient->EMV_AIP[B1] & b6) && (pclient->pTerminal->TerminalCapabilities[EMV_TERMINAL_CAPABILITIES_SECURITY_TABLE] & b7))
#define EMVTerminalAndCardSupportSDA(pclient) ((pclient->EMV_AIP[B1] & b7) && (pclient->pTerminal->TerminalCapabilities[EMV_TERMINAL_CAPABILITIES_SECURITY_TABLE] & b8))
#define EMVTerminalIs(pclient, attr)		  (((attr >> 8) < 3) ? ((pclient->pTerminal->TerminalCapabilities[attr >> 8] & (attr & 0x00FF)) != 0) : ((pclient->pTerminal->AdditionalTerminalCapabilities[(attr >> 8) - 3] & (attr & 0x00FF)) != 0))

typedef struct _EMVTerminal
{
	char			IFDSerialNumber[9];					// Ex. "12345678"
	BYTE			TerminalCountryCode[2];				// tag 9F1A Ex. {0x08, 0x40}  
	BYTE			AdditionalTerminalCapabilities[5];	// tag 9F40 Ex. {0xC1, 0x00, 0xF0, 0xA0, 0x01}
	BYTE			TerminalCapabilities[3];			// tag 9F33 Ex. {0xE0, 0xF8, 0xE8}
	BYTE			TerminalType;						// tag 9F35 Ex. 0x22  
    BYTE    		TTQ[4];                             // tag 9F66 Ex. {0x26, 0x40, 0x40, 0x00}
} EMVTerminal;

// TTQ
//b8 b7 b6 b5 b4 b3 b2 b1 Meaning
//1 x x x x x x x Contactless MSD supported
//x 1 x x x x x x Contactless VSDC supported
//x x 1 x x x x x Contactless qVSDC supported
//x x x 1 x x x x EMV contact chip supported
//x x x x 1 x x x Offline-only reader
//x x x x x 1 x x Online PIN supported
//x x x x x x 1 x Signature supported
//x x x x x x x 1 Offline Data Authentication (ODA) for Online Authorizations supported.

#define Contactless_MSD_supported               0x0080
#define Contactless_VSDC_supported              0x0040
#define Contactless_qVSDC_supported             0x0020
#define EMV_contact_chip_supported              0x0010
#define Offline_only_reader                     0x0008
#define Online_PIN_supported                    0x0004
#define Signature_supported                     0x0002
#define Offline_Data_Authentication             0x0001

//b8 b7 b6 b5 b4 b3 b2 b1 Meaning
//1 x x x x x x x Online cryptogram required
//x 1 x x x x x x CVM required
//x x 1 x x x x x (Contact Chip) Offline PIN supported

#define Online_cryptogram_required              0x0180
#define CVM_required                            0x0140
#define Contact_Chip_Offline_PIN_supported      0x0120

//b8 b7 b6 b5 b4 b3 b2 b1 Meaning
//1 x x x x x x x Issuer Update Processing Supported
//x 1 x x x x x x Mobile functionality supported (Consumer Device CVM)

#define Issuer_Update_Processing_Supported                  0x0280
#define Mobile_functionality_supported                      0x0240


//CVM
#define Fail_cardholder_verification_if_verification_is_unsuccessful				0x40
#define Move_to_next_rule_if_verification_is_unsuccessful							0x3F
#define Fail_CVM_processing															0x00 /* Echec CVM*/
#define Offline_plaintext_PIN														0x01 /* Code Pin OffLine En Clair*/
#define Online_PIN_(always_enciphered)												0x02 /* Code Pin OnLine  Crypte*/
#define Offline_plaintext_PIN_and_paper_based_Signature								0x03 /* Code Pin OffLine Clair + Signature */
#define Offline_enciphered_PIN														0x04 /* Code Pin OffLine Crypte*/
#define Offline_enciphered_PIN_and_paper_based_Signature							0x05 /* Code Pin OffLine Crypte*/
#define Paper_based_Signature_only													0x1E /* Signature*/
#define Approve_CVM_processing														0x1F /* Aucune Authentification Porteur*/

#define Always_try_to_apply_this_rule												0x00 /* Application sans Condition */
#define Only_try_to_apply_this_rule_where_this_is_an_unattended_cash_transaction	0x01 /* Application si Type Trs  = Cash */
#define If_not_unattended_cash_and_not_manual_cash_and_not_purchase_with_cashback	0x02 /* Application si Type Trs != Cash */	
#define Always_try_to_apply_this_rule_where_the_CVM_code_is_supported				0x03 /* Application si Supporte */
#define If_this_is_a_manual_cash_transaction_apply_this_rule						0x04
#define If_this_is_a_purchase_with_cashback_apply_this_rule							0x05
#define If_transaction_is_in_the_application_currency_and_is_under_X_value 			0x06 /* Application si M<X */
#define If_transaction_is_in_the_application_currency_and_is_over_X_value			0x07 /* Application si M>X */
#define If_transaction_is_in_the_application_currency_and_is_under_Y_value			0x08 /* Application si M<Y */
#define If_transaction_is_in_the_application_currency_and_is_over_Y_value			0x09 /* Application si M>Y */

// Structure of one AID
typedef struct _EMVAid
{
	int				Length;								// Length in bytes, Ex. 7
	BYTE			AID[16];							// Ex. {0xA0, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10}
	int				Priority;
	BYTE			ApplicationVersionNumber[2];		// Ex. {0x00, 0x8C}
	BYTE			applicationSelectionIndicator;		// Application Selection Indicator, 0 - if application must match AID exactly, 1 - if allow partial
	BYTE			ForceTransaction;					// Indicates if a transaction can be forced to accept by acceptor if refused by terminal
	BYTE			TerminalActionCodeDenial[5];		// Terminal Action Code - Denial
	BYTE			TerminalActionCodeOnline[5];		// Terminal Action Code - Online
	BYTE			TerminalActionCodeDefault[5];		// Terminal Action Code - Default
} EMVAid;

 // Settings global, EMV book 4, Application Dependent Data
/*
10.2 Application Dependent Data The following data elements are application dependent and, if required, are specified by individual payment system specifications: 
 
Data Elements Notes 
Acquirer Identifier  
Application Identifier (AID)  
Application Version Number  
Certification Authority Public Key  
 Certification Authority Public Key Exponent 
 Certification Authority Public Key Modulus Required if terminal supports offline data authentication and/or offline PIN encipherment. 
See Book 2. Certification Authority Public Key Index Required if terminal supports offline data authentication and/or offline PIN encipherment:  The key index in conjunction with the Registered Application Provider Identifier (RID) of the payment system AID identifies the key and the algorithm for offline data authentication and/or PIN encipherment. See Book 2. Default Dynamic Data Authentication Data Object List (DDOL) Required if terminal supports DDA or CDA. Default Transaction Certificate Data Object List (TDOL) If not present, a default TDOL with no data objects in the list shall be assumed. Maximum Target Percentage to be used for Biased Random Selection Required if offline terminal with online capability. Merchant Category Code  Merchant Identifier  Merchant Name and Location  
Table 7:  Application Dependent Data Elements 
10 Data Management EMV 4.3 Book 4 10.2 Application Dependent Data Cardholder, Attendant, and Acquirer  Interface Requirements 
Page 82   November 2011 
Data Elements Notes Target Percentage to be used for Random Selection Required if offline terminal with online capability. 
Terminal Action Code - Default 
Terminal Action Code - Denial 
Terminal Action Code -Online Required if non-zero values to be used 6 
Terminal Floor Limit Required if offline terminal or offline terminal with online capability. 
Terminal Identification  
Terminal Risk Management Data If required by individual payment system rules. 
Threshold Value for Biased Random Selection Required if offline terminal with online capability. 
Transaction Currency Code  
Transaction Currency Exponent  
Transaction Reference Currency Code  
Transaction Reference Currency Conversion 
Transaction Reference Currency Exponent 
 
Table 7:  Application Dependent Data Elements, continued 
The terminal shall provide the necessary logical key slots to handle the active and future replacement 
Certification Authority Public Keys necessary for data authentication and/or offline PIN encipherment.
Each logical key slot shall contain the following data: RID, Certification Authority Public Key Index, and Certification Authority Public Key. 
When the Certification Authority Public Key is loaded to the terminal, the terminal shall verify the Certification Authority Public Key 
Check Sum to detect a key entry or transmission error. This checksum is calculated using the terminal-supported Secure Hash Algorithm. 
If the verification process fails, the terminal shall not accept the Certification Authority Public Key and, if operator action is needed, 
the terminal shall display an error message. After the Certification Authority Public Key is successfully loaded, the terminal should store the 
Certification Authority Public Key Check Sum.   
*/


typedef struct _EMVApplication {
	int				AIDCount;								// Count of AIDs in this configuration
	EMVAid			AID[20];								// EMVAID of current configuration
	BYTE			RID[5];									// Registered Application Provider Identifier, Ex. {0xA0, 0x00, 0x00, 0x00, 0x03}
	
	int				IndexAIDSelected;
	
	
	int				defaultDDOLSize;						// Size in bytes of next element. 0 - if default DDOL is empty
	BYTE			defaultDDOL[64];						// data of default DDOL with size defaultDDOLSize
	
	int				defaultTDOLSize;						// Size in bytes of next element. 0 - if default TDOL is empty
	BYTE			defaultTDOL[64];						// data of default TDOL with size defaultTDOLSize
	
	
	int				maxTargetForBiasedRandomSelection;		// Maximum Target Percentage to be Used for Biased Random Selection (also in the range of 0 to 99 but at least as high as the previous ‘Target Percentage to be Used for Random Selection’).
	
	
	int				TargetForRandomSelection;				// Target Percentage to be Used for Random Selection (in the range of 0 to 99)
	

	BYTE			TerminalFloorLimit[4];					// Terminal Floor Limit, Ex. {0x00, 0x00, 0x10, 0x00} - value 10.00
	char			TerminalIdentification[9];				// Designates the unique location of a terminal at a merchant, Ex. "EMVPOS4 "
	int				TerminalRiskManagementDataSize;			// Size of next element
	BYTE			TerminalRiskManagementData[8];			// Application-specific value used by the card for risk management purposes
	
	BYTE			ThresholdValueForRandomSelection[4];	// Threshold Value for Biased Random Selection (which must be zero or a positive number less than the floor limit)
	BYTE			TransactionReferenceCurrency[2];		// Transaction Reference Currency Code, Ex. {0x09, 0x78}
	BYTE			TransactionReferenceCurrencyConv[4];	// Factor used in the conversion from the Transaction Currency Code to the Transaction Reference Currency Code
	BYTE			TransactionReferenceCurrencyExponent;	// Indicates the implied position of the decimal point from the right of the transaction amount, with the Transaction Reference Currency Code represented according to ISO 4217
	
	List*			AuthorityPublicKeys;					// Public keys, see structure EMVAuthorityPublicKey

	
} EMVApplication;





// Settings of library, optional
typedef struct _EMVSettings
{
	List*			appFiles;								// Usually downloaded (see CB2A corresponding)
	List*			appTags;								// Card, issuer, terminal and appli Tags
	List*			appApduErrors;
	char			appSelectionUsePSE;						// 1 - if use PSE, 0 - don't use PSE
	char			appSelectionSupportConfirm;				// 1 - if support cardholder confirmation of application, 0 - not support
	char			appSelectionPartial;					// 1 - if support partial AID selection
	char			appSelectionSupport;					// 1 - if the terminal supports the ability to allow the cardholder to select an application
} EMVSettings;


typedef struct _EMV {
	MX*				pMX;
	char			ProjectName[200];
	MXCom*			pRouterCom;

	CB2A*			pCB2A;
	EMVSettings		Settings;	
	
	List*			Applications;	
	List*			Terminals;
	List*			Clients;
	
	EMVAcquirer*	pAcquirer;
	EMVAcceptor*	pAcceptor;


	int				ApplicationsCount;
	int				TerminalsCount;
	List*			ExceptionCardList;						// hotlist PAN see structure EMVExceptionCard
	List*			RangBinList;							// CB List of Ranges Registered CB and properties	
	BYTE			ApplicationCurrencyCode[2];				// Application Currency Code 0x9F42
	BYTE			ApplicationCurrencyExponent;			// 0x9F44
	int				DebugEnabled;
	MXMessage*      (*APDU)(struct _EMV* pemv, struct _EMVClient* pclient, BYTE cla, BYTE ins, BYTE p1, BYTE p2, BYTE dataSize, const BYTE* data);
}EMV;


typedef struct _EMVClient {
	char			UserName[30];
	char			UserPassword[30];
	char			UserID[30];

	int				Step;
	int				SubStep;

	EMV*			pEMV;
	TLV*			pTLV;
	int				CardType;						//VISA, AMEX, ...

	char			ATR[250];
					

	EMVPointOfSale*	pPointOfSale;
	EMVTransaction*	pTransaction;
	EMVTerminal*	pTerminal;
	EMVApplication*	pApplication;					//Selected Application
	
	CB2AMessage*	pAutorisationMessage;
	CB2AMessage*	pRedressementMessage;



	int				candidateApplicationCount;
	int				IndexApplicationSelected;
	int				indexApplicationSelected;		// index fetching
	int				indexApplicationAidSelected;	// index Aid fetching
	int				RecordNo;						// Record Number for SFi with PSE
	int				NumberOfRecordsToRead;			// Number of total records to read
	int				RecordNumber;					// Record Number

	BYTE			sfiOfPSE;
	EMV_BITS*		EMV_TSI;
	EMV_BITS*		EMV_TVR;
	BYTE*			EMV_CVMR;

	EMV_BITS*		EMV_AIP;
	EMV_BITS*		EMV_AUC;
    EMV_BITS*		EMV_CTQ;
	EMV_BITS*		EMV_CID;                       //Card Response on Generate AC

	int				EMV_ATC;
	int				EMV_LATC;
	
	BYTE*			EMV_CVM;
	int				EMV_CVMSize;
	int				EMV_PTC;					  // Pin Try Counter

	BYTE*			EMV_SC;						  // Service Code on Track1 or Track2


	BYTE			Cryptogram;
	EMVSelectApplicationInfo candidateApplications[MAX_CANDIDATE_APPLICATIONS];
	int				DifferentCurrency;

}EMVClient;

//AIP
#define CardHolder_verification_is_supported(pclient)			(pclient->EMV_AIP[B1] & b5)
#define Terminal_risk_management_is_to_be_performed(pclient)	(pclient->EMV_AIP[B1] & b4)
#define SDA_supported(pclient)									(pclient->EMV_AIP[B1] & b7)
#define DDA_supported(pclient)									(pclient->EMV_AIP[B1] & b6)
#define CDA_supported(pclient)									(pclient->EMV_AIP[B1] & b1)
#define Issuer_authentication(pclient)							(pclient->EMV_AIP[B1] & b3)
#define EMV_Mode_has_been_selected(pclient)						(pclient->EMV_AUC[B2] & b8) 
#define OTA_capable_mobile_device(pclient)					    (pclient->EMV_AUC[B2] & b7) 
//AUC
#define Valid_for_domestic_cash_transactions(pclient)			(pclient->EMV_AUC[B1] & b8) 
#define Valid_for_international_cash_transactions(pclient)		(pclient->EMV_AUC[B1] & b7) 
#define Valid_for_domestic_goods(pclient)						(pclient->EMV_AUC[B1] & b6) 
#define Valid_for_international_goods(pclient)					(pclient->EMV_AUC[B1] & b5) 
#define Valid_for_domestic_services(pclient)					(pclient->EMV_AUC[B1] & b4) 
#define Valid_for_international_services(pclient)				(pclient->EMV_AUC[B1] & b3) 
#define Valid_at_ATMs(pclient)									(pclient->EMV_AUC[B1] & b2) 
#define Valid_at_terminals_other_than_ATMs(pclient)				(pclient->EMV_AUC[B1] & b1) 
#define Domestic_cashback_allowed(pclient)						(pclient->EMV_AUC[B2] & b8) 
#define International_cashback_allowed(pclient)					(pclient->EMV_AUC[B2] & b7) 

//CTQ
#define Online_PIN_Required                                                             (pclient->EMV_CTQ[B1] & b8)
#define Signature_Required                                                              (pclient->EMV_CTQ[B1] & b7) 
#define Go_Online_if_Offline_Data_Authentication_Fails_and_Reader_is_online_capable     (pclient->EMV_CTQ[B1] & b6) 
#define Switch_Interface_if_Offline_Data_Authentication_fails_and_Reader_supports_VIS   (pclient->EMV_CTQ[B1] & b5) 
#define Go_Online_if_Application_Expired                                                (pclient->EMV_CTQ[B1] & b4) 
#define Switch_Interface_for_CashTransactions                                           (pclient->EMV_CTQ[B1] & b3) 
#define Switch_Interface_for_Cashback_Transactions                                      (pclient->EMV_CTQ[B1] & b2) 
 
 
#define Consumer_Device_CVMPerformed                                                    (pclient->EMV_CTQ[B2] & b8) 
#define Card_supports_Issuer_Update_Processing_at_the_POS                               (pclient->EMV_CTQ[B2] & b7) 


#ifdef __cplusplus
extern "C"
{
#endif

extern char		Directory[300];
extern char		EMVRooter[300];
extern int		EMVRooterPort;
extern char		LoginServer[300];
extern MXCom*	EMVRooterCom;
extern char		smessage[5000];
extern int		EMVServerPort;
extern char		DefaultProjectName[200];


//EMV PROCESS FUNCTIONS
//======================


extern int	        EMVCardDetectionAndReset (EMV* pemv, EMVClient* pclient, char* satr);
extern int			EMVOnRecvCardDetectionAndReset (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize);

extern int			EMVCandidateListCreation (EMV* pemv, EMVClient* pclient);
extern int			EMVOnRecvCandidateListCreation (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize);

extern int			EMVApplicationSelection (EMV* pemv, EMVClient* pclient);
extern int			EMVOnRecvApplicationSelection (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize);

extern int			EMVReadApplicationData (EMV* pemv, EMVClient* pclient);
	extern int			EMVCompleteEquivalentDataTags (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckMissingData (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckMandatoryData (EMV* pemv, EMVClient* pclient);


extern int			EMVOnRecvReadApplicationData (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize);


extern int			EMVDataAuthentification (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckTerminalType (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckAuthentificationMechanism (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckOfflineSDA (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckOfflineDDA (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckOfflineCDA (EMV* pemv, EMVClient* pclient);

extern int			EMVProcessingRestrictions (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckApplicationVersionNumber (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckApplicationUsageControl (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckApplicationExpirationDates (EMV* pemv, EMVClient* pclient);

extern int			EMVCardHolderVerification (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckCVMRule (EMV* pemv, EMVClient* pclient, DWORD X, DWORD Y, BYTE* pCVMList, int ListSize);
	extern int			EMVOnRecvVerify (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize);

extern int			EMVTerminalRiskManagement (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckExceptionListCards (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckCBRegisteredBin (EMV* pemv, EMVClient* pclient);

	extern int			EMVCheckFloorLimit (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckRandomTransactionSelection (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckVelocity (EMV* pemv, EMVClient* pclient);

extern int			EMVTerminalActionAnalysis (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckActionIfDenial (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckActionIfOnLine (EMV* pemv, EMVClient* pclient);
	extern int			EMVCheckActionIfApproved (EMV* pemv, EMVClient* pclient);


extern int			EMVCardActionAnalysis (EMV* pemv, EMVClient* pclient);
	extern int			EMVGenerateACFirst (EMV* pemv, EMVClient* pclient);
	extern int			EMVOnRecvACFirst (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize);


extern int			EMVOnlineOfflineDecision (EMV* pemv, EMVClient* pclient);

extern int			EMVOnlineProcessing (EMV* pemv, EMVClient* pclient);
	extern EMVAcquirerConnection*	EMVConnectToAcquirer (EMV* pemv, EMVClient* pclient);
	extern int EMVSendMessageToAcquirer (EMV* pemv, EMVClient* pclient, CB2AMessage* pcb2amessage);
		


extern int			EMVScriptProcessing (EMV* pemv, EMVClient* pclient);
	extern int EMVCheckIssuerScriptsTemplate (EMV* pemv, EMVClient* pclient);
	extern int	EMVPostIssuanceCommands (EMV* pemv, EMVClient* pclient);

extern int			EMVTransactionCompletion (EMV* pemv, EMVClient* pclient, int forceterminate);
	extern int			EMVGenerateACSecond (EMV* pemv, EMVClient* pclient);
	extern int			EMVOnRecvACSecond (EMV* pemv, EMVClient* pclient, BYTE* outData, int outSize);
	extern int			EMVTerminateTransaction (EMV* pemv, EMVClient* pclient);







/////////////////////////////////////////////////////////////////////////////////////////////////////

extern EMV*			EMVInit(MX* pmx);
extern void			EMVEnd (EMV* pemv);
extern int			EMVLoadProject(EMV* pemv, char* projectname);


extern EMVApplication* EMVInitApplication (EMV* pemv);
extern void			EMVEndApplication (EMV* pemv, EMVApplication* papplication);

extern EMVTerminal* EMVInitTerminal (EMV* pemv);
extern void			EMVEndTerminal (EMV* pemv, EMVTerminal* pTerminal);

extern EMVClient*	EMVInitClient (EMV* pemv);
extern void			EMVEndClient (EMV* pemv, EMVClient* pclient);
extern void			EMVSetClient(EMV* pemv, EMVClient* pclient); 


extern EMVPointOfSale* EMVInitPointOfSale (EMV* pemv, EMVClient* pclient);
extern void			EMVSetPointOfSale (EMV* pemv, EMVClient* pclient, EMVPointOfSale* ppointofsale);


extern EMVTransaction* EMVInitTransaction (EMV* pemv, BYTE type, char* currency, char* amount, BYTE Media);
extern void			EMVSetTransaction (EMV* pemv, EMVClient* pclient, EMVTransaction* pTransaction);

extern EMVAcquirer* EMVInitAcquirer (EMV* pemv);
extern void			EMVEndAcquirer (EMV* pemv, EMVAcquirer* pacquirer);
extern void			EMVSetAcquirer (EMV* pemv, EMVClient* pclient, EMVAcquirer* pacquirer);

extern EMVAcquirerConnection* EMVInitHost (EMV* pemv, EMVAcquirer* pacquirer, char* host, int port);

extern EMVAcceptor* EMVInitAcceptor (EMV* pemv);
extern void			EMVEndAcceptor (EMV* pemv, EMVAcceptor* pacceptorr);
extern void			EMVSetAcceptor (EMV* pemv, EMVClient* pclient, EMVAcceptor* pacceptor);



extern void			EMVAddApplication (EMV* pemv, EMVApplication* papplication);
extern void			EMVSetApplication (EMV* pemv, EMVClient* pclient, EMVApplication* pApplication);
extern EMVApplication* EMVGetApplicationFromAID (EMV* pemv, BYTE* AID);
extern EMVApplication* EMVGetApplicationFromRID (EMV* pemv, BYTE* RID);
extern void			EMVAddTerminal (EMV* pemv, EMVTerminal* pTerminal);
extern void			EMVSetTerminal (EMV* pemv, EMVClient* pclient, EMVTerminal* pTerminal);



extern void			EMVSetTVR (EMV* pemv, EMVClient* pclient, unsigned short tvrflag, int set);
extern void			EMVSetTSI (EMV* pemv, EMVClient* pclient, unsigned short tsiflag, int set);


extern EMVClient*	EMVGetClientFromCom (EMV* pemv, MXCom* pcom);
extern char			EMVCheckCandidateInApplicationList(EMV* pemv, EMVSelectApplicationInfo* candidate);
extern void			EMVDestroySettings(EMV* pemv);

extern void			EMVSetLibrarySettings (EMV* pemv, EMVSettings* settings);

extern int			EMVBuildCandidateListUsingListOfAids (EMV* pemv, EMVClient* pclient);

extern int			EMVSelectApplication  (EMV* pemv, EMVClient* pclient, int indexApplication);
extern int			EMVApplicationPriority (EMV* pemv, EMVClient* pclient); 


extern EMVAuthorityPublicKey* EMVGetAuthorityPublicKeyFromKeyIndex (EMV* pemv, EMVClient* pclient, int index);
extern EMVExceptionCard* EMVGetExceptionPanFromPan (EMV* pemv, EMVClient* pclient, BYTE* pan, int tagPanSize);
extern EMVRangeBin* EMVGetRangeBinFromPan (EMV* pemv, EMVClient* pclient, BYTE* pan, int tagPanSize);


extern int			EMVGetProcessingOption(EMV* pemv, EMVClient* pclient, unsigned char *lcdolData, int lcSize);
extern int			EMVGetICCDynamicNumber (EMV* pemv, EMVClient* pclient);
extern int			EMVGetATC (EMV* pemv, EMVClient* pclient);
extern int			EMVGetLastOnlineATC (EMV* pemv, EMVClient* pclient);
extern int			EMVGetPTC (EMV* pemv, EMVClient* pclient);

extern int			EMVGetApplicationCandidatesCount (EMV* pemv, EMVClient* pclient);
extern				EMVSelectApplicationInfo* EMVGetCandidate (EMV* pemv, EMVClient* pclient, int indexApplication);
extern void			EMVSetDebugEnabled (EMV* pemv, char enabled);
extern void			EMVSetFunctionAPDU(EMV* pemv, MXMessage* (*f_apdu)(EMV* pemv, EMVClient* pclient, BYTE cla, BYTE ins, BYTE p1, BYTE p2,
													BYTE dataSize, const BYTE* data));

extern void			EMVSetTag(EMVClient* pclient, unsigned short tag, BYTE* data, int size);


extern int			EMVLoadApplications (EMV* pemv);
extern int			EMVLoadTacs (EMV* pemv);
extern int			EMVLoadTerminals (EMV* pemv);
extern int			EMVLoadExceptionCards (EMV* pemv);
extern int			EMVLoadRangeBins (EMV* pemv);
extern int			EMVLoadCurrencies (EMV* pemv);
extern int			EMVLoadAcceptor(EMV* pemv);
extern int			EMVLoadAuthorityPublicKeys(EMV* pemv);

extern int			EMVParseSelectADF(EMV* pemv, EMVSelectApplicationInfo* appInfo, BYTE* rApdu, int rApduSize);
extern char			EMVCorrectATR(BYTE* bufATR, int size);

extern int			EMVSendVerify (EMV* pemv, EMVClient* pclient, BYTE enciphered);
extern int			EMVSendCommand (EMV* pemv, EMVClient* pclient,  unsigned char p1, unsigned char p2);

extern MXMessage*	EMVSendAPDU (EMV* pemv, EMVClient* pclient,  BYTE cla, BYTE ins, BYTE p1, BYTE p2, BYTE dataSize, const BYTE* data);
extern int			EMVAPDU (EMV* pemv, BYTE cla, BYTE ins, BYTE p1, BYTE p2,
									BYTE dataSize, const BYTE* data,
									int* outDataSize, BYTE* outData);
extern int			EMVOnRecvLastOnlineATC (EMV* pemv, EMVClient* pclient, BYTE p1, BYTE p2, BYTE* outData, int outSize);
extern int			EMVOnRecvATC (EMV* pemv, EMVClient* pclient, BYTE p1, BYTE p2, BYTE* outData, int outSize);
extern int			EMVOnRecvPinTryCounter (EMV* pemv, EMVClient* pclient, BYTE p1, BYTE p2, BYTE* outData, int outSize);

//CB2A
extern int			EMVFillMessageFields (EMV* pemv, CB2AMessage* pmessage, TLV* pTLV);
extern int			EMVFillFieldFromEMVTag (EMV* pemv, CB2AMessage* pmessage, EMVTag* pemvtag, BYTE* tagvalue, int tagsize);
extern int			EMVSetFieldValueFromEMVTag (CB2AMessage* pmessage, CB2AFieldValue* pfieldvalue, EMVTag* pemvtag, BYTE* value, int size);
extern int			EMVSetTagValueFromEMVTag (CB2AMessage* pmessage, CB2ATagValue* ptagvalue, EMVTag* pemvtag, BYTE* value, int size);

//File
extern int			EMVReadFile (EMV* pemv);
extern void			EMVAddFile (EMV* pemv, EMVFile* pfile);
extern int			EMVGenerateMXFile (EMV* pemv, char* classname, int classindex, char* dialogfilename);



//Tag
extern int			EMVReadTagFile (EMV* pemv);
extern void			EMVAddTag (EMV* pemv, EMVTag* ptag);
extern char*		EMVGetTagNameFromHexStr (EMV* pemv, char* stag);
extern char*		EMVGetTagNameFromDecimal (EMV* pemv, int tag);
extern EMVTag*		EMVGetTagFromDecimal (EMV* pemv, int tag);
extern BYTE*		EMVGetTagValue (EMV* pemv, EMVClient* pclient, EMVTag* ptag, int* outSize);


extern void			EMVCardValidator (EMV* pemv, EMVClient* pclient, char *cardNumbers, int size);

//Errors
extern int				EMVReadApduErrorFile (EMV* pemv);
extern void				EMVAddApduError (EMV* pemv, EMVApduError* perror);
extern EMVApduError*	EMVGetErrorFromSW1SW2 (EMV* pemv, BYTE SW1, BYTE SW2);

//Trace

extern void			EMVTraceAcquirer (EMV* pemv);
extern void			EMVTraceAcceptor (EMV* pemv);
extern void			EMVTraceHexaBuffer (EMV* pemv, char* strPre, unsigned char* buf, int size, char* strPost);
extern void         EMVTraceTransaction (EMVClient* pclient);

extern void			EMVTraceAIP (EMVClient* pclient);
extern void			EMVTraceAUC (EMVClient* pclient);
extern void			EMVTraceCVM (EMVClient* pclient);
extern void			EMVTraceTVR (EMVClient* pclient, EMV_BITS* pTVR, char* label);
extern void			EMVTraceTVROnlyYes (EMVClient* pclient, EMV_BITS* pTVR, char* label);
extern void			EMVTraceTVRAdded (EMVClient* pclient, BYTE B , BYTE b);
extern void			EMVTraceTSIAdded (EMVClient* pclient, BYTE b);
extern void			EMVTraceTSI (EMVClient* pclient);
extern void			EMVTraceCTQ (EMVClient* pclient);
extern void			EMVTraceTTQ (EMVClient* pclient);
extern void			EMVTraceStep (EMVClient* pclient);
extern void			EMVTraceSubStep (EMVClient* pclient);
extern void			EMVTraceAPDU (EMVClient* pclient, BYTE cla, BYTE ins, BYTE p1, BYTE p2, BYTE* Data, int dataSize, int way);
extern void			EMVTraceTag (EMVClient* pclient, unsigned short tag, unsigned char* buf, int size);
extern void			EMVTraceTLV (EMVClient* pclient);
extern void			EMVTraceDOL (EMVClient* pclient, unsigned short tag);
extern void			EMVTraceCID (EMVClient* pclient);
extern void			EMVTraceCTQ (EMVClient* pclient);
extern void			EMVTraceCryptogramType (EMVClient* pclient);
extern void			EMVTraceCandidate (EMV* pemv, EMVClient* pclient, int index);
extern void			EMVTraceCandidates (EMV* pemv, EMVClient* pclient);

extern void			EMVTraceApplication (EMV* pemv, EMVApplication* papplication, int index);
extern void			EMVTraceApplications (EMV* pemv);
extern void			EMVTraceCB2AMessage (EMVClient* pclient, CB2AMessage* pmessage);
extern void			EMVTraceTerminalType (EMVClient* pclient);
extern void			EMVTraceTerminal(EMVClient* pclient);
extern void			EMVTraceTNAReason (int code);
extern void			EMVTraceBinStatus (int code);
extern void			EMVTraceCardStatus (int code);
extern void			EMVTraceServiceCode (EMVClient* pclient);
extern void			EMVTraceIIN (EMVClient* pclient, char*cardNumbers);

extern int			MXAddAPDUCommands(MX* pmx);
extern MXCom*		Connect_RouterServer(EMV* pemv);
extern int			Send_Login(MXCom* pcom, EMVClient* pclient);
extern void			Send_Start(MXCom* pcom, EMVClient* pclient);
extern void			Send_End(MXCom* pcom, EMVClient* pclient);

extern void			Send_Trace(MXCom* pcom, EMVClient* pclient, char* message);
extern void			Send_Info(MXCom* pcom, EMVClient* pclient, char* command, char* message);
extern void			Send_CVM(MXCom* pcom, EMVClient* pclient, BYTE* scvm, int size);
extern void			Send_TLV(MXCom* pcom, EMVClient* pclient, BYTE* tlvbuffer, int tlvsize);

extern void			Send_APDU(MXCom* pcom, EMVClient* pclient, BYTE cla, BYTE ins, BYTE p1, BYTE p2, int datasize, BYTE* data, int way);
extern void			Send_Command(MXCom* pcom, EMVClient* pclient, char* command, long long par);
extern void			Send_Step(MXCom* pcom,  EMVClient* pclient, int step);
extern void			Send_SetTSI(MXCom* pcom, EMVClient* pclient, unsigned short b);
extern void			Send_SetTVR(MXCom* pcom, EMVClient* pclient, unsigned short b);
extern void			Send_AUC(MXCom* pcom, EMVClient* pclient, unsigned short b);
extern void			Send_Tag(MXCom* pcom, EMVClient* pclient, unsigned short tag, BYTE* data, int size);
extern void			s_printf(char* message, EMVClient* pclient, char* format, char* string);
#ifdef __cplusplus
};
#endif

#endif // __EMV_H
