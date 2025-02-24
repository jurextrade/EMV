#define _CRT_SECURE_NO_WARNINGS

#include "EMV.h"
#include "EMVError.h"

#include "cb2a.h"


void EMVTraceTransaction(EMVClient* pclient)
{
	EMVTransaction* pTransaction = pclient->pTransaction;

	printf("\n%-0s%s\n", "", "TRANSACTION\n");
	printf("%-0s=================\n", "");
	printf("%-10s : %s\n", "Media", (pTransaction->POSEntryMode == EMV_POS_ENTRY_MODE_contactless) ? "CONTACTLESS" : "ICC");
	printf("%-10s : %s\n", "Amount", pTransaction->AmountAuthorized);
	printf("%-10s : %s\n", "Currency", "");
	printf("%-10s : %s\n", "Type", "");
	printf("\n");

}

void EMVTraceTSIAdded (EMVClient* pclient, BYTE b)
{	
	char strace[1000];
	memset(strace, 0, 1000);

	sprintf(strace, "%5s%s", "", "  ==> [TSI] ");

	int i = strlen (strace);

	if (b == b8) sprintf (strace + i, "%50s : %s\n", "Offline_data_authentication_was_performed",				"YES"); else
	if (b == b7) sprintf (strace + i, "%50s : %s\n", "Cardholder_verification_was_performed",					"YES"); else
	if (b == b6) sprintf (strace + i, "%50s : %s\n", "Card_risk_management_was_performed",						"YES"); else
	if (b == b5) sprintf (strace + i, "%50s : %s\n", "Issuer_authentication_was_performed",						"YES"); else
	if (b == b4) sprintf (strace + i, "%50s : %s\n", "Terminal_risk_management_was_performed",					"YES"); else
	if (b == b3) sprintf (strace + i, "%50s : %s\n", "Script_processing_was_performed",							"YES"); 

	s_printf(smessage, pclient,  "%s", strace);

	Send_Command(EMVRooterCom, pclient, "SETTSI", b);
	
	printf ("\n");
}

void EMVTraceTVRAdded (EMVClient* pclient, BYTE B , BYTE b)
{
	char strace[1000];
	memset(strace, 0, 1000);

	sprintf(strace, "%5s%s", "", "  ==> [TVR] ");

	int i = strlen (strace);

	if (B == B1 && b == b8) sprintf (strace + i,  "%50s : %s\n", "Offline data authentication was not performed",		"YES"); else
	if (B == B1 && b == b7) sprintf (strace + i,  "%50s : %s\n", "SDA failed",											"YES"); else
	if (B == B1 && b == b6) sprintf (strace + i,  "%50s : %s\n", "ICC data missing",									"YES"); else
	if (B == B1 && b == b5) sprintf (strace + i,  "%50s : %s\n", "Card appears on terminal exception file",				"YES"); else
	if (B == B1 && b == b4) sprintf (strace + i,  "%50s : %s\n", "DDA failed",											"YES"); else
	if (B == B1 && b == b3) sprintf (strace + i,  "%50s : %s\n", "CDA failed",											"YES"); else
	if (B == B1 && b == b2) sprintf (strace + i,  "%50s : %s\n", "SDA selected",										"YES"); else
	if (B == B2 && b == b8) sprintf (strace + i,  "%50s : %s\n", "Card and terminal have different application versions","YES"); else
	if (B == B2 && b == b7) sprintf (strace + i,  "%50s : %s\n", "Expired application",									"YES"); else
	if (B == B2 && b == b6) sprintf (strace + i,  "%50s : %s\n", "Application not yet effective",						"YES"); else
	if (B == B2 && b == b5) sprintf (strace + i,  "%50s : %s\n", "Requested service not allowed for card product",		"YES"); else
	if (B == B2 && b == b4) sprintf (strace + i,  "%50s : %s\n", "New Card",											"YES"); else
	if (B == B3 && b == b8) sprintf (strace + i,  "%50s : %s\n", "Cardholder verification was not successful",			"YES"); else
	if (B == B3 && b == b7) sprintf (strace + i,  "%50s : %s\n", "Unrecognised CVM",									"YES"); else
	if (B == B3 && b == b6) sprintf (strace + i,  "%50s : %s\n", "PIN Try Limit exceeded",								"YES"); else
	if (B == B3 && b == b5) sprintf (strace + i,  "%50s : %s\n", "PIN entry required and PIN pad not present or not working",		"YES"); else
	if (B == B3 && b == b4) sprintf (strace + i,  "%50s : %s\n", "PIN entry required, PIN pad present, but PIN was not entered",	"YES"); else
	if (B == B3 && b == b3) sprintf (strace + i,  "%50s : %s\n", "Online PIN entered",									"YES"); else
	if (B == B4 && b == b8) sprintf (strace + i,  "%50s : %s\n", "Transaction exceeds floor limit",						"YES"); else
	if (B == B4 && b == b7) sprintf (strace + i,  "%50s : %s\n", "Lower consecutive offline limit exceeded",			"YES"); else
	if (B == B4 && b == b6) sprintf (strace + i,  "%50s : %s\n", "Upper consecutive offline limit exceeded",			"YES"); else
	if (B == B4 && b == b5) sprintf (strace + i,  "%50s : %s\n", "Transaction selected randomly for online processing",	"YES"); else
	if (B == B4 && b == b4) sprintf (strace + i,  "%50s : %s\n", "Merchant forced transaction online",					"YES"); else
	if (B == B5 && b == b8) sprintf (strace + i,  "%50s : %s\n", "Default TDOL used",									"YES"); else
	if (B == B5 && b == b7) sprintf (strace + i,  "%50s : %s\n", "Issuer authentication failed",						"YES"); else
	if (B == B5 && b == b6) sprintf (strace + i,  "%50s : %s\n", "Script processing failed before final GENERATE AC",	"YES"); else
	if (B == B5 && b == b5) sprintf (strace + i,  "%50s : %s\n", "Script processing failed after final GENERATE AC",	"YES"); 

	s_printf(smessage, pclient, "%s", strace);

	Send_Command(EMVRooterCom, pclient, "SETTVR", (B << 8) + b);
	
	printf ("\n");
}

void EMVTraceAIP(EMVClient* pclient)
{
	int outSize;
	EMV_BITS* EMV_AIP = (EMV_BITS*)TLVGetTag(pclient->pTLV, TAG_AIP, &outSize);

	char strace[1000];
	memset(strace, 0, 1000);

	sprintf(strace, "%5s%s\n", "", "AIP\n");

	int i = strlen(strace);

	sprintf(strace + i, "%50s : %s\n", "SDA supported",									(EMV_AIP[B1] & b7) ? "YES" : "NO"); i = strlen(strace);
	sprintf(strace + i, "%50s : %s\n", "DDA supported",									(EMV_AIP[B1] & b6) ? "YES" : "NO");	i = strlen(strace);
	sprintf(strace + i, "%50s : %s\n", "Cardholder verification is supported",			(EMV_AIP[B1] & b5) ? "YES" : "NO");	i = strlen(strace);
	sprintf(strace + i, "%50s : %s\n", "Terminal risk management is to be performed",	(EMV_AIP[B1] & b4) ? "YES" : "NO");	i = strlen(strace);
	sprintf(strace + i, "%50s : %s\n", "Issuer authentication",							(EMV_AIP[B1] & b3) ? "YES" : "NO");	i = strlen(strace);
	sprintf(strace + i, "%50s : %s\n", "CDA supported",									(EMV_AIP[B1] & b1) ? "YES" : "NO");	i = strlen(strace);
	sprintf(strace + i, "%50s : %s\n", "Domestic_cashback_allowed",						(EMV_AIP[B2] & b8) ? "YES" : "NO"); i = strlen(strace);
	sprintf(strace + i, "%50s : %s\n", "International_cashback_allowed",				(EMV_AIP[B2] & b7) ? "YES" : "NO"); i = strlen(strace);

	s_printf(smessage, pclient, "%s", strace);

	Send_Command(EMVRooterCom, pclient, "AIP", (EMV_AIP[B1] << 8) + EMV_AIP[B2]);

	printf("\n");
}

void EMVTraceAUC (EMVClient* pclient)
{
	int outSize;
	EMV_BITS* EMV_AUC = (EMV_BITS*) TLVGetTag(pclient->pTLV, TAG_AUC, &outSize);

	char strace[1000];
	memset(strace, 0, 1000);

	sprintf(strace, "%5s%s\n", "", "AUC\n");
	int i = strlen(strace);
	
	sprintf (strace + i, "%50s : %s\n", "Valid for domestic cash transactions",			(EMV_AUC[B1] & b8) ? "YES" : "NO"); i= strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Valid for international cash transactions",	(EMV_AUC[B1] & b7) ? "YES" : "NO"); i= strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Valid for domestic goods",						(EMV_AUC[B1] & b6) ? "YES" : "NO"); i= strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Valid for international goods",				(EMV_AUC[B1] & b5) ? "YES" : "NO"); i= strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Valid for domestic services",					(EMV_AUC[B1] & b4) ? "YES" : "NO"); i= strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Valid for international services",				(EMV_AUC[B1] & b3) ? "YES" : "NO"); i= strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Valid at ATMs",								(EMV_AUC[B1] & b2) ? "YES" : "NO"); i= strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Valid at terminals other than ATMs",			(EMV_AUC[B1] & b1) ? "YES" : "NO"); i= strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Domestic cashback allowed",					(EMV_AUC[B2] & b8) ? "YES" : "NO"); i= strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "International cashback allowed",				(EMV_AUC[B2] & b7) ? "YES" : "NO"); i= strlen(strace);
	
	s_printf(smessage, pclient, "%s", strace);

	Send_Command(EMVRooterCom, pclient, "AUC", (EMV_AUC[B1] << 8) + EMV_AUC[B2]);

	printf ("\n");
}

void EMVTraceCTQ (EMVClient* pclient)
{
	int outSize;
	EMV_BITS* EMV_CTQ = (EMV_BITS*) TLVGetTag(pclient->pTLV, TAG_CTQ, &outSize);

	char strace[1000];
	memset(strace, 0, 1000);

	sprintf(strace, "%5s%s\n", "", "CTQ\n");
	int i = strlen(strace);


	
	sprintf (strace + i, "%50s : %s\n", "Online_PIN_Required",                                                          	(EMV_CTQ[B1] & b8) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Signature_Required",                                                           	(EMV_CTQ[B1] & b7) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Go_Online_if_Offline_Data_Authentication_Fails_and_Reader_is_online_capable",  	(EMV_CTQ[B1] & b6) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Switch_Interface_if_Offline_Data_Authentication_fails_and_Reader_supports_VIS",	(EMV_CTQ[B1] & b5) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Go_Online_if_Application_Expired",                                            		(EMV_CTQ[B1] & b4) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Switch_Interface_for_CashTransactions",                                       		(EMV_CTQ[B1] & b3) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Switch_Interface_for_Cashback_Transactions",                                   	(EMV_CTQ[B1] & b2) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Consumer_Device_CVMPerformed",                     				                (EMV_CTQ[B2] & b8) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Card_supports_Issuer_Update_Processing_at_the_POS",				                (EMV_CTQ[B2] & b7) ? "YES" : "NO"); i = strlen(strace);
	
	s_printf(smessage, pclient, "%s", strace);

	Send_Command(EMVRooterCom, pclient, "CTQ", (EMV_CTQ[B1] << 8) + EMV_CTQ[B2]);

	printf ("\n");
}

void EMVTraceCID (EMVClient* pclient)
{
	int outSize;
	BYTE* EMV_CID = TLVGetTag(pclient->pTLV, TAG_CID, &outSize);
	
	EMVTraceTag (pclient, TAG_CID, EMV_CID, outSize);

	char strace[1000];
	memset(strace, 0, 1000);

	sprintf(strace, "%5s%s\n", "", "CID");
	int i = strlen(strace);



	if ((EMV_CID[0] & EMV_CRYPTO_TYPE_AAR) == EMV_CRYPTO_TYPE_AAR)   sprintf (strace + i, "%50s : %s\n", "AAR",	"YES");	else
    if ((EMV_CID[0] & EMV_CRYPTO_TYPE_ARQC) == EMV_CRYPTO_TYPE_ARQC) sprintf (strace + i, "%50s : %s\n", "ARQC","YES");	else
	if ((EMV_CID[0] & EMV_CRYPTO_TYPE_TC) == EMV_CRYPTO_TYPE_TC) 	 sprintf (strace + i, "%50s : %s\n", "TC",	"YES");	else
	 sprintf (strace + i, "%50s : %s\n", "AAC",	"YES"); //00

	i = strlen(strace);

	if (EMV_CID[0] & EMV_CRYPTO_TYPE_PSS)							{ sprintf(strace + i, "%50s : %s\n", "Payment System-specific cryptogram", "YES");  i = strlen(strace); }

	if (EMV_CID[0] & EMV_CRYPTO_AR)									{ sprintf(strace + i, "%50s : %s\n", "Advice Required", "YES");					    i = strlen(strace); } else
																	{ sprintf(strace + i, "%50s : %s\n", "No Advice Required", "YES");					    i = strlen(strace); }

	if (EMV_CID[0] & EMV_CRYPTO_RFU) { sprintf(strace + i, "%50s : %s\n", "Other Values RFU", "YES");		i = strlen(strace); }	else
	if (EMV_CID[0] & EMV_CRYPTO_IAF) { sprintf(strace + i, "%50s : %s\n", "Issuer authentication failed", "YES");		i = strlen(strace); }	else
	if (EMV_CID[0] & EMV_CRYPTO_TLE) { sprintf(strace + i, "%50s : %s\n", "PIN Try Limit exceeded", "YES");				i = strlen(strace); }	else
	if (EMV_CID[0] & EMV_CRYPTO_SNA) { sprintf(strace + i, "%50s : %s\n", "Service not allowed", "YES");					i = strlen(strace); } else
		sprintf(strace + i, "%50s : %s\n", "No Information Given", "YES");

	s_printf(smessage, pclient, "%s", strace);

	Send_Command(EMVRooterCom, pclient, "CID", EMV_CID[0]);

	printf ("\n");
}

void EMVTraceServiceCode(EMVClient* pclient)
{
	BYTE* ServiceCode = pclient->EMV_SC;

	BYTE SB1 = ServiceCode[0] & 0x0F;
	BYTE SB2 = (ServiceCode[1] & 0xF0) >> 4;
	BYTE SB3 = ServiceCode[1] & 0x0F;

	char strace[1000];
	memset(strace, 0, 1000);

	sprintf(strace, "%5s%s\n", "", "  ==> [Service Code] ");
	int i = strlen(strace);

	if (SB1 == 1) sprintf(strace + i, "%50s : %s\n", "International interchange OK", "YES"); else
		if (SB1 == 2) sprintf(strace + i, "%50s : %s\n", "International interchange, use IC (chip) where feasible", "YES"); else
			if (SB1 == 5) sprintf(strace + i, "%50s : %s\n", "National interchange only except under bilateral agreement", "YES"); else
				if (SB1 == 6) sprintf(strace + i, "%50s : %s\n", "National interchange only except under bilateral agreement, use IC (chip) where feasible", "YES"); else
					if (SB1 == 7) sprintf(strace + i, "%50s : %s\n", "No interchange except under bilateral agreement (closed loop)", "YES"); else
						if (SB1 == 9) sprintf(strace + i, "%50s : %s\n", "Test", "YES");

	i = strlen(strace);

	if (SB2 == 0) sprintf(strace + i, "%50s : %s\n", "Normal", "YES"); else
		if (SB2 == 2) sprintf(strace + i, "%50s : %s\n", "Contact issuer via online means", "YES"); else
			if (SB2 == 4) sprintf(strace + i, "%50s : %s\n", "Contact issuer via online means except under bilateral agreement", "YES");

	i = strlen(strace);

	if (SB3 == 0) sprintf(strace + i, "%50s : %s\n", "No restrictions, PIN required", "YES"); else
		if (SB3 == 1) sprintf(strace + i, "%50s : %s\n", "No restrictions", "YES"); else
			if (SB3 == 2) sprintf(strace + i, "%50s : %s\n", "Goods and services only (no cash)", "YES"); else
				if (SB3 == 3) sprintf(strace + i, "%50s : %s\n", "ATM only, PIN required", "YES"); else
					if (SB3 == 4) sprintf(strace + i, "%50s : %s\n", "Cash only", "YES"); else
						if (SB3 == 5) sprintf(strace + i, "%50s : %s\n", "Goods and services only (no cash), PIN required", "YES"); else
							if (SB3 == 6) sprintf(strace + i, "%50s : %s\n", "No restrictions, use PIN where feasible", "YES"); else
								if (SB3 == 7) sprintf(strace + i, "%50s : %s\n", "Goods and services only (no cash), use PIN where feasible", "YES");

	s_printf(smessage, pclient, "%s", strace);

	Send_Command(EMVRooterCom, pclient, "SERVICECODE", SB1 << 16 + SB2 << 8 + SB3);
}

void EMVTraceTerminalType(EMVClient* pclient)
{
	char strace[1000];
	memset(strace, 0, 1000);

	sprintf(strace, "\n%5s%s", "", "Terminal Type :");
	int i = strlen(strace);

	int type = pclient->pTerminal->TerminalType;

	switch (type) {
	case EMV_TERMINAL_TYPE_11:
		sprintf(strace + i, "       %s\n", "Financial Institution Attended Online only");
		break;
	case EMV_TERMINAL_TYPE_12:
		sprintf(strace + i, "       %s\n", "Financial Institution Attended Offline with online capability");
		break;
	case EMV_TERMINAL_TYPE_13:
		sprintf("       %s\n", "Financial Institution Attended Offline only");
		break;
	case EMV_TERMINAL_TYPE_14:
		sprintf(strace + i, "       %s\n", "Financial Institution Unattended Online only");
		break;
	case EMV_TERMINAL_TYPE_15:
		sprintf(strace + i, "       %s\n", "Financial Institution Unattended Offline with online capability");
		break;
	case EMV_TERMINAL_TYPE_16:
		sprintf(strace + i, "       %s\n", "Financial Institution Unattended Offline only");
		break;
	case EMV_TERMINAL_TYPE_21:
		sprintf(strace + i, "       %s\n", "Merchant Attended Online only");
		break;
	case EMV_TERMINAL_TYPE_22:
		sprintf(strace + i, "       %s\n", "Merchant Attended Offline with online capability");
		break;
	case EMV_TERMINAL_TYPE_23:
		sprintf(strace + i, "       %s\n", "Merchant Attended Offline only");
		break;
	case EMV_TERMINAL_TYPE_24:
		sprintf("       %s\n", "Merchant Unattended Online only");
		break;
	case EMV_TERMINAL_TYPE_25:
		sprintf(strace + i, "       %s\n", "Merchant Unattended Offline with online capability");
		break;
	case EMV_TERMINAL_TYPE_26:
		sprintf(strace + i, "       %s\n", "Merchant Unattended Offline only");
		break;
	case EMV_TERMINAL_TYPE_34:
		sprintf(strace + i, "       %s\n", "Cardholder Unattended Online only");
		break;
	case EMV_TERMINAL_TYPE_35:
		sprintf(strace + i, "       %s\n", "Cardholder  Unattended Offline with online capability");
		break;
	case EMV_TERMINAL_TYPE_36:
		sprintf(strace + i, "       %s\n", "Cardholder Unattended Offline only");
		break;
	default:
		sprintf(strace + i, "       %s\n", "Unknown Terminal Type");
		break;
	}

	s_printf(smessage, pclient, "%s", strace);

	Send_Command(EMVRooterCom, pclient, "TT", type);

	printf("\n");

}


void EMVTraceTerminal(EMVClient* pclient)
{
	EMVTerminal* pTerminal = pclient->pTerminal;

	EMVTraceTerminalType(pclient);

	char strace[5000];

	memset(strace, 0, 5000);

	sprintf(strace, "\n%5s%s\n", "", "Terminal Capabilities :");
	int i = strlen(strace);

	if (EMVTerminalIs(pclient, Manual_key_entry)) { sprintf(strace + i, "%50s : %s\n", "Manual Key Entry", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Magnetic_stripe)) { sprintf(strace + i, "%50s : %s\n", "Magnetic_stripe", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, IC_with_contacts)) { sprintf(strace + i, "%50s : %s\n", "IC_with_contacts", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Plaintext_PIN_for_ICC_verification)) { sprintf(strace + i, "%50s : %s\n", "Plaintext_PIN_for_ICC_verification", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Enciphered_PIN_for_online_verification)) { sprintf(strace + i, "%50s : %s\n", "Enciphered_PIN_for_online_verification", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Signature_paper)) { sprintf(strace + i, "%50s : %s\n", "Signature_(paper)", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Enciphered_PIN_for_offline_verification)) { sprintf(strace + i, "%50s : %s\n", "Enciphered_PIN_for_offline_verification", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, No_CVM_Required)) { sprintf(strace + i, "%50s : %s\n", "No_CVM_Required", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, SDA)) { sprintf(strace + i, "%50s : %s\n", "SDA", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, DDA)) { sprintf(strace + i, "%50s : %s\n", "DDA", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Card_capture)) { sprintf(strace + i, "%50s : %s\n", "Card capture", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, CDA)) { sprintf(strace + i, "%50s : %s\n", "CDA", "YES"); i = strlen(strace); }
	
	BYTE TC[3];
	memcpy(&TC, &pclient->pTerminal->TerminalCapabilities, 3);
	Send_Command(EMVRooterCom, pclient, "TC", (TC[0] << 16 | TC[1] << 8 | TC[2]));

	
//additional

	if (EMVTerminalIs(pclient, Cash)) { sprintf(strace + i, "%50s : %s\n", "Cash", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Goods)) { sprintf(strace + i, "%50s : %s\n", "Goods", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Services)) { sprintf(strace + i, "%50s : %s\n", "Services", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Cashback)) { sprintf(strace + i, "%50s : %s\n", "Cashback", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Inquiry)) { sprintf(strace + i, "%50s : %s\n", "Inquiry", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Transfer)) { sprintf(strace + i, "%50s : %s\n", "Transfer", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Payment)) { sprintf(strace + i, "%50s : %s\n", "Payment", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Administrative)) { sprintf(strace + i, "%50s : %s\n", "Administrative", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Cash_Deposit)) { sprintf(strace + i, "%50s : %s\n", "Cash_Deposit", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Numeric_keys)) { sprintf(strace + i, "%50s : %s\n", "Numeric_keys", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Alphabetic_and_special_characters_keys)) { sprintf(strace + i, "%50s : %s\n", "Alphabetic_and_special_characters_keys", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Command_keys)) { sprintf(strace + i, "%50s : %s\n", "Command_keys", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Function_keys)) { sprintf(strace + i, "%50s : %s\n", "Function_keys", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Print_attendant)) { sprintf(strace + i, "%50s : %s\n", "Print_attendant", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Print_cardholder)) { sprintf(strace + i, "%50s : %s\n", "Print_cardholder", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Display_attendant)) { sprintf(strace + i, "%50s : %s\n", "Display_attendant", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Display_cardholder)) { sprintf(strace + i, "%50s : %s\n", "Display_cardholder", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Code_table_10)) { sprintf(strace + i, "%50s : %s\n", "Code_table_10", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Code_table_9)) { sprintf(strace + i, "%50s : %s\n", "Code_table_9", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Code_table_8)) { sprintf(strace + i, "%50s : %s\n", "Code_table_8", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Code_table_7)) { sprintf(strace + i, "%50s : %s\n", "Code_table_7", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Code_table_6)) { sprintf(strace + i, "%50s : %s\n", "Code_table_6", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Code_table_5)) { sprintf(strace + i, "%50s : %s\n", "Code_table_5", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Code_table_4)) { sprintf(strace + i, "%50s : %s\n", "Code_table_4", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Code_table_3)) { sprintf(strace + i, "%50s : %s\n", "Code_table_3", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Code_table_2)) { sprintf(strace + i, "%50s : %s\n", "Code_table_2", "YES"); i = strlen(strace); }
	if (EMVTerminalIs(pclient, Code_table_1)) { sprintf(strace + i, "%50s : %s\n", "Code_table_1", "YES"); i = strlen(strace); }

	s_printf(smessage, pclient, "%s", strace);
	
	BYTE ATC[5];
	memcpy(&ATC, &pclient->pTerminal->AdditionalTerminalCapabilities, 5);

	Send_Command(EMVRooterCom, pclient, "ATC", ((long long)ATC[0] << 32 | ATC[1] << 24 | ATC[2] << 16 | ATC[3] << 8 | ATC[4]));

	printf("\n");
}

void EMVTraceCryptogramType (EMVClient* pclient)
{
	char strace[1000];
	memset(strace, 0, 1000);

	if (pclient->Cryptogram == EMV_CRYPTO_TYPE_AAC)
		sprintf (strace, "\n%-5s", "Terminal Decision AAC : Transaction declined\n");
	else
	if (pclient->Cryptogram == EMV_CRYPTO_TYPE_ARQC)
		sprintf (strace, "\n%-5s", "Terminal Decision ARQC : Online authorisation requested\n");
	else
	if (pclient->Cryptogram == EMV_CRYPTO_TYPE_ARQC)
		sprintf (strace, "\n%-5s", "Terminal Decision TC : Transaction approved\n");

	s_printf(smessage, pclient, "%s", strace);
	Send_Info(EMVRooterCom, pclient, "INFO", strace);
	   
	Send_Command(EMVRooterCom, pclient, "CRYPTO", pclient->Cryptogram);
}

void EMVTraceCandidate (EMV* pemv, EMVClient* pclient, int index)
{
	EMVSelectApplicationInfo candidateAppli = pclient->candidateApplications[index];
	printf ("\n%-5sCandidate %d \n", "", index);
	printf ("%-5s===========\n", "");
	printf("%-10sDFNameLength: %d\n",	"",					candidateAppli.DFNameLength);
	printf("%-10sDFName:", "");								EMVTraceHexaBuffer (pemv, " ", candidateAppli.DFName, candidateAppli.DFNameLength, "\n");
	printf("%-10sstrApplicationLabel: %s\n",	"",			candidateAppli.strApplicationLabel);
	printf("%-10sneedCardholderConfirm: %d\n",	"",			candidateAppli.needCardholderConfirm);
	printf("%-10spriority: %d\n",				"",			candidateAppli.priority);
	printf("%-10sstrLanguagePreference: %s\n",	"",			candidateAppli.strLanguagePreference);
	printf("%-10sissuerCodeTableIndex: %d\n",	"",			candidateAppli.issuerCodeTableIndex);
	printf("%-10sstrApplicationPreferredName: %s\n",	"",	candidateAppli.strApplicationPreferredName);
	printf("\n");
}

void EMVTraceCVM (EMVClient* pclient)
{
	int outSize;
	BYTE* pCVMData = TLVGetTag(pclient->pTLV, TAG_CVM_LIST, &outSize);
	
	BYTE* pCVMList	= pCVMData + 8;		
	int ListSize	= (outSize - 8) / 2;

	char strace[400];
	memset(strace, 0, 400);

	sprintf(strace, "%5s%s\n", "", "CVM List");
	int i = strlen(strace);

	
	for (int j = 0; j < ListSize * 2; j = j + 2)
	{
		printf("\n%02X%02X : ", pCVMList[j] & 0xFF, pCVMList[j+1] & 0xFF);
		switch (pCVMList[j] & 0xFF & 0x3F) {
			case 0 :
					sprintf (strace + i, "%-s\n", "Fail CVM processing"); i = strlen(strace);
			break;
			case 1 :
					sprintf (strace + i, "%-s\n", "Offline plaintext PIN"); i = strlen(strace);
			break;
			case 2 :
					sprintf (strace + i, "%-s\n", "Online PIN (always enciphered)"); i = strlen(strace);
			break;
			case 3 :
					sprintf (strace + i, "%-s\n", "Offline plaintext PIN and paper based Signature"); i = strlen(strace);
			break;
			case 4 :
					sprintf (strace + i, "%-s\n", "Offline enciphered PIN"); i = strlen(strace);
			break;
			case 5 :
					sprintf (strace + i, "%-s\n", "Offline enciphered PIN and paper based Signature"); i = strlen(strace);
			break;
			case 30 :
					sprintf (strace + i, "%-s\n", "Paper based Signature only"); i = strlen(strace); 
			break;
			case 31 :
					sprintf (strace + i, "%-s\n", "Approve CVM processing"); i = strlen(strace); 
			break;
			}
		switch (pCVMList[j+1] & 0xFF & 0x0F) {
			case 0 :
					sprintf (strace + i, "       %s\n", "Always try to apply this rule");  i = strlen(strace);
			break;
			case 1 :
					sprintf (strace + i, "       %s\n", "Only try to apply this rule where this is an unattended cash transaction"); i = strlen(strace);
			break;
			case 2 :
					sprintf (strace + i, "       %s\n", "If not unattended cash and not manual cash and not purchase with cashback"); i = strlen(strace);
			break;
			case 3 :
					sprintf (strace + i, "       %s\n", "Always try to apply this rule where the CVM code is supported"); i = strlen(strace);
			break;
			case 4 :
					sprintf (strace + i, "       %s\n", "If this is a manual cash transaction apply this rule"); i = strlen(strace);
			break;
			case 5 :
					sprintf (strace + i, "       %s\n", "If this is a purchase with cashback apply this rule"); i = strlen(strace);
			break;
			case 6 :
					sprintf (strace + i, "       %s\n", "If transaction is in the application currency and is under X value"); i = strlen(strace);
			break;
			case 7 :
					sprintf (strace + i, "       %s\n", "If transaction is in the application currency and is over X value"); i = strlen(strace);
			break;
			case 8 :
					sprintf (strace + i, "       %s\n", "If transaction is in the application currency and is under Y value"); i = strlen(strace);
			break;
			case 9 :
					sprintf (strace + i, "       %s\n", "If transaction is in the application currency and is over Y value"); i = strlen(strace);
			break;

		}
		sprintf (strace + i, "%s : %s\n", "Rule",	(pCVMList[j] & 0xFF & 0x40) ?  "Move to next rule if verification is unsuccessful." : "Fail cardholder verification if verification is unsuccessful.");
	}

	s_printf(smessage, pclient, "%s", strace);

	printf ("\n");
}


void EMVTraceTVR (EMVClient* pclient, EMV_BITS* pTVR, char* label)
{
	char strace[2000];
	memset(strace, 0, 1000);

	sprintf(strace, "%5s%s\n", "", label);
	int i = strlen(strace);

	sprintf (strace + i, "%50s : %s\n", "Offline data authentication was not performed",		(pTVR[B1] & b8) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "SDA failed",											(pTVR[B1] & b7) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "ICC data missing",										(pTVR[B1] & b6) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Card appears on terminal exception file",				(pTVR[B1] & b5) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "DDA failed",											(pTVR[B1] & b4) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "CDA failed",											(pTVR[B1] & b3) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "SDA selected",											(pTVR[B1] & b2) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "ICC and terminal have different application versions",	(pTVR[B2] & b8) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Expired application",									(pTVR[B2] & b7) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Application not yet effective",						(pTVR[B2] & b6) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Requested service not allowed for card product",		(pTVR[B2] & b5) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "New Card",												(pTVR[B2] & b4) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Cardholder verification was not successful",			(pTVR[B3] & b8) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Unrecognised CVM",										(pTVR[B3] & b7) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "PIN Try Limit exceeded",								(pTVR[B3] & b6) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "PIN entry required and PIN pad not present or not working",	(pTVR[B3] & b5) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "PIN entry required, PIN pad present, but PIN was not entered",	(pTVR[B3] & b4) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Online PIN entered",											(pTVR[B3] & b3) ? "YES" : "NO"); i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Transaction exceeds floor limit",						(pTVR[B4] & b8) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Lower consecutive offline limit exceeded",				(pTVR[B4] & b7) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Upper consecutive offline limit exceeded",				(pTVR[B4] & b6) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Transaction selected randomly for online processing",	(pTVR[B4] & b5) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Merchant forced transaction online",					(pTVR[B4] & b4) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Default TDOL used",									(pTVR[B5] & b8) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Issuer authentication failed",							(pTVR[B5] & b7) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Script processing failed before final GENERATE AC",	(pTVR[B5] & b6) ? "YES" : "NO");		 i = strlen(strace);
	sprintf (strace + i, "%50s : %s\n", "Script processing failed after final GENERATE AC",		(pTVR[B5] & b5) ? "YES" : "NO");		 i = strlen(strace);
	
	s_printf(smessage, pclient, "%s", strace);
	Send_Command(EMVRooterCom, pclient, label, ((long long)pTVR[B1] << 32 |  pTVR[B2] << 24 | pTVR[B3] << 16 | pTVR[B4] << 8 | pTVR[B5]));

	printf ("\n");
}


void EMVTraceTVROnlyYes (EMVClient* pclient, EMV_BITS* pTVR, char* label)
{
	char strace[1000];
	memset(strace, 0, 1000);

	sprintf(strace, "%5s%s\n", "", label);
	int i = strlen(strace);
		
	if (pTVR[B1] & b8) {sprintf (strace + i, "%50s : %s\n", "Offline data authentication was not performed",			"YES");		 i = strlen(strace);}
	if (pTVR[B1] & b7) {sprintf (strace + i, "%50s : %s\n", "SDA failed",											    "YES");		 i = strlen(strace);}
	if (pTVR[B1] & b6) {sprintf (strace + i, "%50s : %s\n", "ICC data missing",											"YES");		 i = strlen(strace);}
	if (pTVR[B1] & b5) {sprintf (strace + i, "%50s : %s\n", "Card appears on terminal exception file",					"YES");		 i = strlen(strace);}
	if (pTVR[B1] & b4) {sprintf (strace + i, "%50s : %s\n", "DDA failed",												"YES");		 i = strlen(strace);}
	if (pTVR[B1] & b3) {sprintf (strace + i, "%50s : %s\n", "CDA failed",												"YES");		 i = strlen(strace);}
	if (pTVR[B1] & b2) {sprintf (strace + i, "%50s : %s\n", "SDA selected",												"YES");		 i = strlen(strace);}
	if (pTVR[B2] & b8) {sprintf (strace + i, "%50s : %s\n", "ICC and terminal have different application versions",		"YES");		 i = strlen(strace);}
	if (pTVR[B2] & b7) {sprintf (strace + i, "%50s : %s\n", "Expired application",										"YES");		 i = strlen(strace);}
	if (pTVR[B2] & b6) {sprintf (strace + i, "%50s : %s\n", "Application not yet effective",							"YES");		 i = strlen(strace);}
	if (pTVR[B2] & b5) {sprintf (strace + i, "%50s : %s\n", "Requested service not allowed for card product",			"YES");		 i = strlen(strace);}
	if (pTVR[B2] & b4) {sprintf (strace + i, "%50s : %s\n", "New Card",													"YES");		 i = strlen(strace);}
	if (pTVR[B3] & b8) {sprintf (strace + i, "%50s : %s\n", "Cardholder verification was not successful",				"YES");		 i = strlen(strace);}
	if (pTVR[B3] & b7) {sprintf (strace + i, "%50s : %s\n", "Unrecognised CVM",											"YES");		 i = strlen(strace);}
	if (pTVR[B3] & b6) {sprintf (strace + i, "%50s : %s\n", "PIN Try Limit exceeded",									"YES");		 i = strlen(strace);}
	if (pTVR[B3] & b5) {sprintf (strace + i, "%50s : %s\n", "PIN entry required and PIN pad not present or not working","YES");		 i = strlen(strace);}
	if (pTVR[B3] & b4) {sprintf (strace + i, "%50s : %s\n", "PIN entry required, PIN pad present, but PIN was not entered",	"YES");  i = strlen(strace);}
	if (pTVR[B3] & b3) {sprintf (strace + i, "%50s : %s\n", "Online PIN entered",										"YES");		 i = strlen(strace);}
	if (pTVR[B4] & b8) {sprintf (strace + i, "%50s : %s\n", "Transaction exceeds floor limit",							"YES");		 i = strlen(strace);}
	if (pTVR[B4] & b7) {sprintf (strace + i, "%50s : %s\n", "Lower consecutive offline limit exceeded",					"YES");		 i = strlen(strace);}
	if (pTVR[B4] & b6) {sprintf (strace + i, "%50s : %s\n", "Upper consecutive offline limit exceeded",					"YES");		 i = strlen(strace);}
	if (pTVR[B4] & b5) {sprintf (strace + i, "%50s : %s\n", "Transaction selected randomly for online processing",		"YES");		 i = strlen(strace);}
	if (pTVR[B4] & b4) {sprintf (strace + i, "%50s : %s\n", "Merchant forced transaction online",						"YES");		 i = strlen(strace);}
	if (pTVR[B5] & b8) {sprintf (strace + i, "%50s : %s\n", "Default TDOL used",										"YES");		 i = strlen(strace);}
	if (pTVR[B5] & b7) {sprintf (strace + i, "%50s : %s\n", "Issuer authentication failed",								"YES");		 i = strlen(strace);}
	if (pTVR[B5] & b6) {sprintf (strace + i, "%50s : %s\n", "Script processing failed before final GENERATE AC",		"YES");		 i = strlen(strace);}
	if (pTVR[B5] & b5) { sprintf(strace + i, "%50s : %s\n", "Script processing failed after final GENERATE AC",			"YES");		 i = strlen(strace);}

	s_printf(smessage, pclient, "%s", strace);

	printf ("\n");
}




void EMVTraceBinStatus(int code)
{
	switch (code)
	{
		case Bin_accepted:
			printf("       %s\n", "BIN ACCEPTED");
			break;
		case Bin_watched:
			printf("       %s\n", "BIN WATCHED");
			break;
		case Bin_forbidden:
			printf("       %s\n", "BIN FORBIDDEN");
			break;
		case Bin_refused:
			printf("       %s\n", "BIN REFUSED");
			break;
	}
}

void EMVTraceCardStatus(int code)
{
	switch (code)
	{
	case Card_forbidden:
		printf("       %s\n", "CARD FORBIDDEN");
		break;
	case Card_watched:
		printf("       %s\n", "CARD WATCHED");
		break;
	case Card_refused:
		printf("       %s\n", "CARD REFUSED");
		break;
	case Card_accepted:
		printf("       %s\n", "CARD ACCEPTED");
		break;
	}
}



void EMVTraceTNAReason(int code)
{
	switch (code)
	{
	case F_58_FF50_01: printf("TNA: %s", "Donnee carte EMV absente"); break;	break;
	case F_58_FF50_02: printf("TNA: %s", "Erreur lors de la lecture des compteurs Carte"); break;
	case F_58_FF50_03: printf("TNA: %s", "Donnees obligatoires de la carte absentes"); break;
	case F_58_FF50_04: printf("TNA: %s", "Donnee carte EMV redondante"); break;
	case F_58_FF50_05: printf("TNA: %s", "Erreur de format dans les donnees Carte"); break;
	case F_58_FF50_06: printf("TNA: %s", "Erreur lors de l'authentification de la Carte"); break;
	case F_58_FF50_07: printf("TNA: %s", "Transaction abandonnee par le porteur lors de la saisie du code"); break;
	case F_58_FF50_08: printf("TNA: %s", "Erreur lors de la verification du code confidentiel offline"); break;
	case F_58_FF50_09: printf("TNA: %s", "Erreur lors de la première demande de cryptogramme à la carte"); break;
	case F_58_FF50_10: printf("TNA: %s", "Erreur lors de la seconde demande de cryptogramme à la carte"); break;
	case F_58_FF50_11: printf("TNA: %s", "Transaction refusee au premier Generate AC"); break;
	case F_58_FF50_12: printf("TNA: %s", "Transaction refusee au second Generate AC"); break;
	case F_58_FF50_13: printf("TNA: %s", "Validation d’une transaction de credit incorrecte (Bin non trouve en table de BIN et mouvement initial non trouve)"); break;
	case F_58_FF50_14: printf("TNA: %s", "Validation d’une transaction d’annulation incorrecte"); break;
	case F_58_FF50_15: printf("TNA: %s", "Structure de la carte invalide (Longueur, date de validite ou cle de lühn invalide)"); break;
	case F_58_FF50_16: printf("TNA: %s", "Donnees permettant de verifier la signature de la carte absentes"); break;
	case F_58_FF50_17: printf("TNA: %s", "La carte est perimee et ne peut être utilisee"); break;
	case F_58_FF50_18: printf("TNA: %s", "La carte est en opposition"); break;
	case F_58_FF50_19: printf("TNA: %s", "La carte est presente en liste de contrôle en « interdit » ou « refuse »"); break;
	case F_58_FF50_20: printf("TNA: %s", "La carte est presente dans la liste des BINs en « interdit » ou « refuse"); break;
	case F_58_FF50_21: printf("TNA: %s", "Une demande d’autorisation a ete transmise et la reponse de la banque emettrice est negative"); break;
	case F_58_FF50_22: printf("TNA: %s", "La carte est interdite ou refusee en reponse à une demande d’autorisation ou la reponse de la banque emettrice demande la capture de la carte"); break;
	case F_58_FF50_23: printf("TNA: %s", "Erreur lors de l'impression du ticket"); break;
	case F_58_FF50_24: printf("TNA: %s", "La transaction d’annulation n’a pas ete acceptee (transaction a annuler non trouvee)"); break;
	case F_58_FF50_25: printf("TNA: %s", "La transaction de credit n’a pas ete acceptee"); break;
	case F_58_FF50_26: printf("TNA: %s", "Problème à l’enregistrement de la transaction"); break;
	case F_58_FF50_27: printf("TNA: %s", "Autorisation partielle refusee par le porteur"); break;
	case F_58_FF50_28: printf("TNA: %s", "Erreur de reponse carte au 1er ou second Generate AC mode CDA (erreur carte)"); break;
	case F_58_FF50_29: printf("TNA: %s", "Erreur de reponse carte au 1er ou second Generate AC mode SDA/DDA (erreur carte)"); break;
	case F_58_FF50_30: printf("TNA: %s", "Problème de distribution du bien ou service"); break;
	case F_58_FF50_87: printf("TNA: %s", "Un timer est arrive à echeance à la presentation du dispositif ou de la carte."); break;

		//contactless
	case F_58_FF50_50: printf("TNA: %s", "Des donnees obligatoires de la carte (AFL, AIP) sont absentes ; la transaction ne peut se poursuivre."); break;
	case F_58_FF50_51: printf("TNA: %s", "Des donnees permettant de verifier la signature de la carte sont absentes. La transaction ne peut se poursuivre."); break;
	case F_58_FF50_52: printf("TNA: %s", "Une zone de traitement du CDA est inferieure à la taille necessaire."); break;
	case F_58_FF50_53: printf("TNA: %s", "La methode d’authentification supportee par la carte n’est pas le CDA qui est la seule methode supportee"); break;
	case F_58_FF50_54: printf("TNA: %s", "La date de debut de validite de l’application carte n’est pas atteinte et ne peut dans ce cadre être utilisee"); break;
	case F_58_FF50_55: printf("TNA: %s", "La carte est perimee et ne peut être utilisee. Sa verification est basee sur les règles EMV."); break;
	case F_58_FF50_56: printf("TNA: %s", "La carte presentee est une carte de tests dans le referentiel des listes d’acceptation."); break;
	case F_58_FF50_57: printf("TNA: %s", "La carte n’est pas parametree (AUC) pour être utilisee sur ce point d’acceptation."); break;
	case F_58_FF50_58: printf("TNA: %s", "L’authentification proposee au porteur n’est pas supportee par l’application de votre terminal."); break;
	case F_58_FF50_59: printf("TNA: %s", "La carte ne possède pas de CVM list"); break;
	case F_58_FF50_60: printf("TNA: %s", "L’authentification du porteur a echoue ou les authentifications du porteur ne sont pas compatibles avec celles du terminal."); break;
	case F_58_FF50_61: printf("TNA: %s", "Erreur durant le deroulement de l’authentification carte (CDA)"); break;
	case F_58_FF50_62: printf("TNA: %s", "La carte etant en opposition, la transaction n’a pu aboutir"); break;
	case F_58_FF50_63: printf("TNA: %s", "Le Bin de la carte est refuse ou interdit dans la table des BIN de l’application"); break;
	case F_58_FF50_64: printf("TNA: %s", "Le certificat reçu de la carte n’est pas interpretable"); break;
	case F_58_FF50_65: printf("TNA: %s", "La date d’expiration est < à la date locale. Le parametrage de la carte demande le refus de la transaction. Cette decision est uniquement une decision carte (Visa)."); break;
	case F_58_FF50_66: printf("TNA: %s", "La carte demande de changer d’interface sur un echec d’authentification (Visa) Cette decision est uniquement une decision carte (Visa)"); break;
	case F_58_FF50_67: printf("TNA: %s", "Abandon de la transaction demandee par le commerçant dans le cadre d’une annulation. Cette decision concerne tous les kernels"); break;
	case F_58_FF50_68: printf("TNA: %s", "Un problème a ete rencontre sur le mobile durant la phase de l’authentification porteur. Ceci peut correspondre à un delai echu avant representation du mobile est echu. Cette decision concerne tous les kernels"); break;
	case F_58_FF50_69: printf("TNA: %s", "Une demande d’autorisation a ete transmise et la reponse de la banque emettrice est negative."); break;
	case F_58_FF50_70: printf("TNA: %s", "Une demande d’autorisation a ete transmise et la reponse de la banque emettrice est de type demande de capture."); break;
	case F_58_FF50_71: printf("TNA: %s", "Erreur lors de l'impression du ticket"); break;
	case F_58_FF50_72: printf("TNA: %s", "La transaction d’annulation n’a pas ete acceptee. Ceci est peut être dû aux elements contrôles ou la transaction d’origine non trouvee"); break;
	case F_58_FF50_73: printf("TNA: %s", "La transaction de credit n’a pas ete acceptee."); break;
	case F_58_FF50_80: printf("TNA: %s", "La transaction n’a pas ete transmise pour des problèmes de communication"); break;
	case F_58_FF50_81: printf("TNA: %s", "La reponse n’est pas parvenue"); break;
	case F_58_FF50_82: printf("TNA: %s", "Erreur lors de l’impression du ticket"); break;
	case F_58_FF50_83: printf("TNA: %s", "Structure de la reponse invalide"); break;
	case F_58_FF50_84: printf("TNA: %s", "Une demande d’autorisation a ete transmise et la reponse de la banque emettrice est negative"); break;
	case F_58_FF50_85: printf("TNA: %s", "L’autorisation partielle a ete refusee par le porteur"); break;
	}
}


void EMVTraceIIN(EMVClient* pclient, char* cardNumbers)
{
	switch (pclient->CardType)
	{
	case EMV_CARDTYPE_IS_UNKNOWN:
		printf("%5sCredit PAN Number: %s Unknown Issuing Network \n", "", cardNumbers);
		break;
	case EMV_CARDTYPE_IS_VISA:
		printf("%5sCredit PAN Number: %s Issuing Network is VISA\n", "", cardNumbers);
		break;
	case EMV_CARDTYPE_IS_MASTERCARD:
		printf("%5sCredit PAN Number: %s Issuing Network is MASTERCARD\n", "", cardNumbers);
		break;
	case EMV_CARDTYPE_IS_AMEX:
		printf("%5sCredit PAN Number: %s Issuing Network is AMERICAN EXPRESS\n", "", cardNumbers);
		break;
	case EMV_CARDTYPE_IS_BANKCARD:
		printf("%5sCredit PAN Number: %s Issuing Network is BANKCARD\n", "", cardNumbers);
		break;
	case EMV_CARDTYPE_IS_DISCOVER:
		printf("%5sCredit PAN Number: %s Issuing Network is DISCOVER\n", "", cardNumbers);
		break;
	case EMV_CARDTYPE_IS_DINERS:
		printf("%5sCredit PAN Number: %s Issuing Network is DINERS\n", "", cardNumbers);
		break;
	case EMV_CARDTYPE_IS_JCB:
		printf("%5sCredit PAN Number: %s Issuing Network is JCB (Japan Credit Bureau)\n", "", cardNumbers);
		break;
	case EMV_CARDTYPE_IS_UP:
		printf("%5sCredit PAN Number: %s Issuing Network is China Union Pay (UP)\n", "", cardNumbers);
		break;
	}

}



void EMVTraceTSI(EMVClient* pclient)
{
}

void EMVTraceDOL (EMVClient* pclient, unsigned short tag)
{
	int outSize  = 0;
	int dolShift = 0;
	int	pdolTagSize = 0;
	BYTE* dol = TLVGetTag(pclient->pTLV, tag, &outSize);
	
	if (!dol) return;
	if (tag == TAG_CDOL_1)
		printf("%50s", "Card Risk Management Data Object List 1 (CDOL1)\n\n");
	else
	if (tag == TAG_CDOL_2)
		printf("%50s", "Card Risk Management Data Object List 2 (CDOL2)\n\n");
	else
	if (tag == TAG_PDOL)
		printf("%50s", "Processing Option Data Object List 2 (PDOL)\n\n");

	while (dolShift < outSize)
	{
		unsigned short tag;
		int size;
		BYTE* pdolTag;

		// Tag could be 1 or 2 byte
		tag = dol[dolShift];
		if ((dol[dolShift] & 0x1F) == 0x1F)
		{
			dolShift++;
			if (dolShift >= outSize)
				break;
			tag <<= 8;
			tag |= dol[dolShift];
		}
		dolShift++;
		if (dolShift >= outSize)
			break;

		// Length could be only 1 byte
		size = dol[dolShift];
		dolShift++;
		printf("%10sTag %4X  size %d  %s : ", "", tag, size, EMVGetTagNameFromDecimal (pclient->pEMV, tag));
		pdolTag = TLVGetTag(pclient->pTLV, tag, &pdolTagSize);
		EMVTraceHexaBuffer(pclient->pEMV, "", pdolTag, size, "\n");
	}
	printf ("\n");
}


void EMVTraceTag (EMVClient* pclient, unsigned short tag, unsigned char* buf, int size)
{
	printf("Tag %4X %s : ", tag, EMVGetTagNameFromDecimal (pclient->pEMV, tag));
	EMVTraceHexaBuffer (pclient->pEMV, "", buf, size, "\n");
}


void EMVTraceHexaBuffer (EMV* pemv, char* strPre, unsigned char* buf, int size, char* strPost)
{

	int i;
	printf(strPre);
	if (buf)
		for (i = 0; i < size; i++)
			printf("%02X", buf[i] & 0xFF);

	printf(strPost);
}


void EMVTraceCB2AMessage (EMVClient* pclient, CB2AMessage* pmessage)
{
	List* listfield = pmessage->FieldsValue;
	List* listtag = NULL; 
	int j;

	while (listfield)
	{
		char strdisplay[1000];
		char strformat[100];
		CB2AFieldValue* pfieldvalue = (CB2AFieldValue*)listfield->car;

		sprintf (strdisplay, "  Champ=[%03d] (%s, %d) ->[", pfieldvalue->pField->Field,  pfieldvalue->pField->Requirements, pfieldvalue->Size);
		if (pfieldvalue->pField->Field == 12)
            j++;
		if (pfieldvalue->pField->DataType == DATA_TYPE_b || pfieldvalue->pField->DataType == DATA_TYPE_n || pfieldvalue->pField->DataType == DATA_TYPE_z)
			for (j = 0; j <  pfieldvalue->Size; j++)
				sprintf(strdisplay + strlen (strdisplay), "%02X", pfieldvalue->Value[j] & 0xFF);
		else
			sprintf(strdisplay + strlen (strdisplay), "%.*s", pfieldvalue->Size, pfieldvalue->Value);

		sprintf(strdisplay + strlen (strdisplay), "]");
		sprintf (strformat, "%%s%%%ds\n", 100 - strlen (strdisplay));
		printf (strformat,  strdisplay, CB2AGetFieldNameFromDecimal (pclient->pEMV->pCB2A, pfieldvalue->pField->Field));		

		listtag = pfieldvalue->TagsValue;
		while (listtag)
		{
			CB2ATagValue* ptagvalue = (CB2ATagValue*)listtag->car;
			if (!ptagvalue->Value) 
			{
				listtag = listtag->cdr;
				continue;
			}
			sprintf (strdisplay, "      Tag=[%.4s] (%s, %d) ->[", ptagvalue->pTag->strTag, ptagvalue->pTag->Requirements, ptagvalue->Size);
			
			if (ptagvalue->pTag->DataType == DATA_TYPE_b || ptagvalue->pTag->DataType == DATA_TYPE_n)
				for (j = 0; j <  ptagvalue->Size; j++)
					sprintf(strdisplay + strlen (strdisplay), "%02X", ptagvalue->Value[j] & 0xFF);
			else
				sprintf(strdisplay + strlen (strdisplay), "%.*s",ptagvalue->Size,  ptagvalue->Value);

			sprintf(strdisplay + strlen (strdisplay), "]");
			sprintf (strformat, "%%s%%%ds\n", 100 - strlen (strdisplay));
			printf (strformat,  strdisplay, CB2AGetTagNameFromDecimal (pclient->pEMV->pCB2A, pfieldvalue->pField,  ptagvalue->pTag->Tag));		

			listtag = listtag->cdr;
		}
		listfield = listfield->cdr;
	}
	return;
}

void EMVTraceTLV (EMVClient* pclient)
{
	int shift = 0;
	unsigned short tag;
	unsigned char* data;
	int length;
	int idx;

	
	int totalSize = 0;
	int tagSize = 0;

	BYTE* tData = (BYTE*)malloc(2 * pclient->pTLV->tlv_length);  //alloc 2 times 
	memset(tData, 0, 2 * pclient->pTLV->tlv_length);


	printf("\n    EMV STREAM\n\n");

	
	while ((shift = TLVGetNextTag(pclient->pTLV, shift, &tag, &data, &length)) != 0)
	{

		tagSize = TLVMake(data, length, tag, tData + totalSize);
		totalSize += tagSize;

		BOOL isAscii = TRUE;
		EMVTag* pTag = NULL;
		pTag = EMVGetTagFromDecimal (pclient->pEMV, tag);
		if (!pTag) 
		{
			printf("- %4X %-20s [%d]: {", tag, "NULL", length);
			return;
		}
		printf("%10s - %4X %-20s [%d]: {", pTag->Origin, tag, pTag->Name, length);
		for (idx = 0; idx < length; idx++)
		{
			printf("%02X", data[idx] & 0xFF);
			if (data[idx] < ' ' || data[idx] > 0x7E)
				isAscii = FALSE;
		}
		
		if (isAscii)
		{
			char* strData = (char*)malloc (length + 1);
			strData[length] = 0;
			memcpy(strData, data, length);
			printf("} ascii: %s\n", strData);
			free (strData);
		}
		else
			printf("}\n");
	}
	Send_TLV(EMVRooterCom, pclient, tData, totalSize);
	free(tData);

}

void EMVTraceAPDU (EMVClient* pclient, BYTE cla, BYTE ins, BYTE p1, BYTE p2,BYTE* Data,  int dataSize, int way)
{
	int i;
	EMVApduError* perror;
	if (way)
	{
		Send_APDU (EMVRooterCom, pclient, cla, ins, p1, p2, dataSize, Data, way);

		printf("R-APDU: ");

		for (i = 0; i < dataSize - 2; i++)
			printf("%02X", Data[i] & 0xFF);

		perror = EMVGetErrorFromSW1SW2 (pclient->pEMV, Data[dataSize - 2] & 0xFF, Data[dataSize - 1] & 0xFF);
		if (perror)
			printf("; SW1/SW2: %02X %02X %s", Data[dataSize - 2] & 0xFF, Data[dataSize - 1] & 0xFF, perror->Description);
		else
			printf("; SW1/SW2: %02X %02X", Data[dataSize - 2] & 0xFF, Data[dataSize - 1] & 0xFF);

	}
	else 
	{
		Send_APDU(EMVRooterCom, pclient, cla, ins, p1, p2, dataSize, Data, way);

		printf("C-APDU: %02X %02X %02X %02X; %02X ", cla & 0xFF, ins & 0xFF, p1 & 0xFF, p2 & 0xFF, dataSize & 0xFF);

		for (i = 0; i < dataSize; i++)
			printf("%02X", Data[i] & 0xFF);
	}
	printf("\n");
}



void EMVTraceSubStep (EMVClient* pclient)
{
	char stepmessage[200];
	printf ("\n");


	switch (pclient->SubStep) {
		case EMV_SUBSTEP_ATTEMP_DIRECTORY_LISTING_PSE	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "1.1 : Attempt To Get Directory List With PSE");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "--------------------------------------------");
		break;
		case EMV_SUBSTEP_READ_RECORD_SFI	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "1.2 : Reading Records");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "---------------------");
		break;
		case EMV_SUBSTEP_BUILD_CANDIDATE_LIST_WITH_SELECT	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "1.3 : Iterate Through Terminal AIDS");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-----------------------------------");
		break;
		case EMV_SUBSTEP_SELECT_APPLICATION_FILE:
			sprintf(stepmessage, "%-5s%-20s\n", "", "2.1 :Select Application File");
			sprintf(stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-----------------------------------");
			break;
		case EMV_SUBSTEP_GET_PROCESSING_OPTIONS:
			sprintf(stepmessage, "%-5s%-20s\n", "", "2.2 : Get Processing Options");
			sprintf(stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "--------------------------");
			break;
		case EMV_SUBSTEP_COMPLETE_EQUIVALENTDATA_TAGS :
			sprintf (stepmessage, "%-5s%-20s\n", "", "3.1 : Complete Equivalent Data Tags");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-----------------------------------");
		break;
		case EMV_SUBSTEP_CHECK_MANDATORY_DATA	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "3.2 : Check Mandatory Data");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "--------------------------");
		break;
		case EMV_SUBSTEP_CHECK_MISSING_DATA	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "3.3 : Check Missing Data");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "------------------------");
		break;
		case EMV_SUBSTEP_CHECK_TERMINAL_TYPE :
			sprintf (stepmessage, "%-5s%-20s\n", "", "4.1 : Check Terminal Type");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-------------------------");
		break;
		case EMV_SUBSTEP_SELECTION_OFFLINE_AUTHENTIFICATION_MECHANISM :
			sprintf (stepmessage, "%-5s%-20s\n", "", "4.2 : Selection of the off-line authentication mechanism");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "--------------------------------------------------------");
		break;
		case EMV_SUBSTEP_OFFLINE_SDA :
			sprintf (stepmessage, "%-5s%-20s\n", "", "4.3 : Processing SDA OFFLINE Static Authentification");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "----------------------------------------------------");
		break;
		case EMV_SUBSTEP_OFFLINE_DDA :
			sprintf (stepmessage, "%-5s%-20s\n", "",  "4.3 : Processing DDA OFFLINE Dynamic Authentification");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "",  "-----------------------------------------------------");
		break;
		case EMV_SUBSTEP_CDA :
			sprintf (stepmessage, "%-5s%-20s\n", "", "4.3 : Processing CDA Combined Dynamic Authentification");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "------------------------------------------------------");
		break;
		case EMV_SUBSTEP_APPLICATION_VERSION_NUMBER	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "5.1 : Check Application Version Number");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "--------------------------------------");
		break;
		case EMV_SUBSTEP_APPLICATION_USAGE_CONTROL	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "5.2 : Check Application Usage Control");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-------------------------------------");
		break;
		case EMV_SUBSTEP_APPLICATION_EFFECTIVE_EXPIRATION_DATES	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "5.3 : Check Application Effective / Expiration Date");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "---------------------------------------------------");
		break;
		case EMV_SUBSTEP_CHECK_CVM_RULES :
			sprintf (stepmessage, "%-5s%-20s\n", "", "6.1 : Checking ICC CVM Rules");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "----------------------------");
		break;
		case EMV_SUBSTEP_EXCEPTION_LIST_CARDS_CHECKING :
			sprintf (stepmessage, "%-5s%-20s\n", "", "7.1 : Check if card pan number is in black list file");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "----------------------------------------------------");
		break;
		case EMV_SUBSTEP_CB_REGISTERED_BIN_CHECKING :
			sprintf (stepmessage, "%-5s%-20s\n", "", "7.2 : Check if card pan number is CB Registered");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-----------------------------------------------");
		break;

		case EMV_SUBSTEP_FLOOR_LIMIT_CHECKING	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "7.3 : Check Floor Limit");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-----------------------");
		break;
		case EMV_SUBSTEP_RANDOM_TRANSACTION_SELECTION	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "7.4 : Check Fro Randoming Transaction");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-------------------------------------");
		break;
		case EMV_SUBSTEP_VELOCITY_CHECKING	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "7.5 : Check Velocity");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "--------------------");
		break;
		case EMV_SUBSTEP_ACTION_IF_DENIAL	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "8.1 : Check Action If Denial");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "----------------------------");
		break;
		case EMV_SUBSTEP_ACTION_IF_ONLINE	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "8.2 : Check Action If OnLine");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "----------------------------");
		break;
		case EMV_SUBSTEP_ACTION_IF_APPROVED	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "8.3 : Check Action If Approved");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "------------------------------");
		break;
		case EMV_SUBSTEP_GENERATE_AC_FIRST	:
			sprintf (stepmessage, "%-5s%-20s\n", "", "9.1 : Generate AC First");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-----------------------");
		break;
		case EMV_SUBSTEP_CONNECT_ACQUIRER :
			sprintf (stepmessage, "%-5s%-20s\n", "", "11.1: Connecting To Acquirers");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-----------------------------");
		break;
		case EMV_SUBSTEP_PREPARE_AND_SEND_TO_ACQUIRER :
			sprintf (stepmessage, "%-5s%-20s\n", "", "11.2 : Prepare and Send Message to Acquirer");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-------------------------------------------");
		break;
		case EMV_SUBSTEP_CHECK_ISSUER_SCRIPTS_TEMPLATE :
			sprintf (stepmessage, "%-5s%-20s\n", "", "12.1: Processing of issuer script templates");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "-------------------------------------------");
		break;
		case EMV_SUBSTEP_POST_ISSUANCE_COMMANDS :
			sprintf (stepmessage, "%-5s%-20s\n", "", "12.2: Post-Issuance Commands");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "----------------------------");
		break;
		case EMV_SUBSTEP_GENERATE_AC_SECOND :
			sprintf (stepmessage, "%-5s%-20s\n", "", "13.1 : Generate AC Second");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "------------------------");
		break;
		case EMV_SUBSTEP_TERMINATE_TRANSACTION :
			sprintf (stepmessage, "%-5s%-20s\n", "", "13.2 : Terminate Transaction");
			sprintf (stepmessage + strlen(stepmessage), "%-5s%-20s\n", "", "----------------------------");
		break;
	}
	s_printf(smessage, pclient, "%s", stepmessage);
//	Send_Command(EMVRooterCom, pclient, "STEP", pclient->SubStep);
	Send_Step(EMVRooterCom, pclient, pclient->SubStep);
	
}
void EMVTraceStep (EMVClient* pclient)
{
	char stepmessage[200];

	printf ("\n");
	
	switch (pclient->Step) {
		case  EMV_STEP_CARD_DETECTION_AND_RESET	:
			sprintf (stepmessage, "%s",					"EMV PROCESS 0 : Card Detection \n");
			sprintf (stepmessage + strlen(stepmessage), "============================== \n");
		break;

		case  EMV_STEP_CANDIDATE_LIST_CREATION	:
			sprintf (stepmessage, "%s",					"EMV PROCESS 1 : Candidate List Creation \n");
			sprintf (stepmessage + strlen(stepmessage), "======================================= \n");

		break;
		
		case  EMV_STEP_APPLICATION_SELECTION	:
			sprintf (stepmessage, "%s",					"EMV PROCESS 2 : Application Selection \n");
			sprintf (stepmessage + strlen(stepmessage), "===================================== \n");

			break;
		
		case  EMV_STEP_READ_APPLICATION_DATA	:
			sprintf (stepmessage, "%s",					"EMV PROCESS 3 : Read Application Data \n");
			sprintf (stepmessage + strlen(stepmessage), "===================================== \n");

			break;
		
		case  EMV_STEP_DATA_AUTHENTIFICATION	:
			sprintf (stepmessage, "%s",					"EMV PROCESS 4 : Data Authentification \n");
			sprintf (stepmessage + strlen(stepmessage), "===================================== \n");

			break;
		
		case  EMV_STEP_PROCESSING_RESTRICTIONS	:
			sprintf (stepmessage, "%s",					"EMV PROCESS 5 : Processing Restrictions \n");
			sprintf (stepmessage + strlen(stepmessage), "======================================= \n");

			break;

		case  EMV_STEP_CARD_HOLDER_VERIFICATION	:	
			sprintf (stepmessage, "%s",					"EMV PROCESS 6 : Card Holder Verification \n");
			sprintf (stepmessage + strlen(stepmessage), "======================================== \n");

			break;
		
		case  EMV_STEP_TERMINAL_RISK_MANAGEMENT	:	
			sprintf (stepmessage, "%s",					"EMV PROCESS 7 : Terminal Risk Management \n");
			sprintf (stepmessage + strlen(stepmessage), "======================================== \n");

			break;
		
		case  EMV_STEP_TERMINAL_ACTION_ANALYSIS	:
			sprintf (stepmessage, "%s",					"EMV PROCESS 8 : Terminal Action Analysis \n");
			sprintf (stepmessage + strlen(stepmessage), "======================================== \n");

			break;
		
		case  EMV_STEP_CARD_ACTION_ANALYSIS		:
			sprintf (stepmessage, "%s",					"EMV PROCESS 9 : Card Action Analysis \n");
			sprintf (stepmessage + strlen(stepmessage), "==================================== \n");

			break;
		
		case  EMV_STEP_ONLINE_OFFLINE_DECISION	:
			sprintf (stepmessage, "%s",					"EMV PROCESS 10 : OnLine OffLine Decision \n");
			sprintf (stepmessage + strlen(stepmessage), "======================================== \n");

			break;
		
		case  EMV_STEP_ONLINE_PROCESSING		:
			sprintf (stepmessage, "%s",					"EMV PROCESS 11 : Online Processing \n");
			sprintf (stepmessage + strlen(stepmessage), "================================== \n");

			break;
		
		case  EMV_STEP_SCRIPT_PROCESSING	:
			sprintf (stepmessage, "%s",					"EMV PROCESS 12 : Script Processing \n");
			sprintf (stepmessage + strlen(stepmessage), "================================== \n");

			break;
		
		case  EMV_STEP_TRANSACTION_COMPLETION	:
			sprintf (stepmessage, "%s",					"EMV PROCESS 13 : Transaction Completion \n");
			sprintf (stepmessage + strlen(stepmessage), "======================================= \n");
			break;
	}
	s_printf(smessage, pclient, "%s", stepmessage);

	Send_Step(EMVRooterCom, pclient, pclient->Step);
}

void EMVTraceAcceptor (EMV* pemv)
{
	printf ("\n%-5sAcceptor\n","");
	printf ("%-5s==========\n", "");
	printf("%-10sMerchantNameAndLocation: %s*\n",	"",	pemv->pAcceptor->MerchantNameAndLocation);
	printf("%-10sMerchantCategoryCode:", "");			EMVTraceHexaBuffer (pemv, " ", pemv->pAcceptor->MerchantCategoryCode, 2, "\n");
	printf("%-10sMerchantContractNumber: %s\n",	"",		pemv->pAcceptor->MerchantContractNumber);
	printf("%-10sTerminalIdentification: %s\n",	"",		pemv->pAcceptor->TerminalIdentification);
	printf("%-10sMerchantIdentifier: %s\n",	"",			pemv->pAcceptor->MerchantIdentifier);
	printf("%-10sSIRET: %s*\n",	"",	                    pemv->pAcceptor->SIRET);
    printf("%-10sMerchantBillMode: %c\n",	"",			pemv->pAcceptor->MerchantBillMode);
	printf("%-10sMerchantActivationCode: %s\n",	"",		pemv->pAcceptor->MerchantActivationCode);
	printf("%-10sSystemAcceptationIdentification: %s\n",	"",			pemv->pAcceptor->SystemAcceptationIdentification);
	printf("%-10sSIRET: %s\n",	"",	pemv->pAcceptor->SIRET);
	printf("\n");
}

void EMVTraceAcquirer (EMV* pemv)
{
	printf ("\n%-5sAcquirer\n","");
	printf ("%-5s==========\n", "");

	printf("%-10sAcquirerIdentifier:", "");			EMVTraceHexaBuffer (pemv, " ", pemv->pAcquirer->AcquirerIdentifier, 6, "\n");
	printf("\n");
}




void EMVTraceApplication (EMV* pemv, EMVApplication* papplication, int index)
{
	printf ("\n%-5sAID with index %d \n", "", index);
	  printf ("%-5s=================\n", "");
	printf("%-10sAID:",	"");							EMVTraceHexaBuffer (pemv, " ", papplication->AID[index].AID, 5,  "-"); 
														EMVTraceHexaBuffer (pemv, "",  papplication->AID[index].AID + 5, papplication->AID[index].Length - 5,  "\n");
	printf("%-10sApplicationVersionNumber:", "");		EMVTraceHexaBuffer (pemv, " ", papplication->AID[index].ApplicationVersionNumber, 2, "\n");
	printf("%-10sTerminalActionCodeDenial:",	"");	EMVTraceHexaBuffer (pemv, " ", papplication->AID[index].TerminalActionCodeDenial, 5, "\n");
	printf("%-10sTerminalActionCodeOnline:",	"");	EMVTraceHexaBuffer (pemv, " ", papplication->AID[index].TerminalActionCodeOnline, 5, "\n");
	printf("%-10sTerminalActionCodeDefault:",	"");	EMVTraceHexaBuffer (pemv, " ", papplication->AID[index].TerminalActionCodeDefault, 5, "\n");
	printf("%-10sForceTransaction:",	"");			EMVTraceHexaBuffer (pemv, " ", &papplication->AID[index].ForceTransaction, 1, "\n");
	printf("%-10sapplicationSelectionIndicator: %d\n",	"",	 papplication->AID[index].applicationSelectionIndicator);
	printf("%-10sPriority: %d\n",	"",					papplication->AID[index].Priority);
	printf("\n");
}


void EMVTraceApplications (EMV* pemv)
{
	int i;
	List* Applications = pemv->Applications;
	while (Applications)
	{
		EMVApplication* pApplication = (EMVApplication*)Applications->car;
		printf ("\n%-5sApplication:", "");  EMVTraceHexaBuffer (pemv, " ", pApplication->RID, 5, "\n");
		printf ("%-5s=======================\n", "");
		for (i = 0; i < pApplication->AIDCount; i++)
			EMVTraceApplication (pemv, pApplication, i);
		Applications = Applications->cdr;
	}
}


void EMVTraceCandidates (EMV* pemv, EMVClient* pclient)
{
	int i;
	for (i = 0; i < pclient->candidateApplicationCount; i++)
		EMVTraceCandidate (pemv, pclient, i);
}


