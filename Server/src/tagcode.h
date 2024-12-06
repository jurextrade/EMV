#ifndef __TAGCODE_H
#define __TAGCODE_H


//TRANSACTION TYPE
//TAG 9C

#define T_9C_00			0x00
#define T_9C_01			0x01
#define T_9C_09			0x09
#define T_9C_20			0x20



//POS Entry Mode
//TAG 9F39


#define T_9F39_01		0x01	//Manuel
#define T_9F39_02		0x02	//Zone magnétique (piste ISO 2 ou piste ISO 1) uniquement
#define T_9F39_03		0x03	//Code barre
#define T_9F39_04		0x04	//Lecture optique
#define T_9F39_05		0x05	//Accès microcircuit uniquement(1)
#define T_9F39_07		0x07	//Transaction puce sans contact utilisant les données puce EMV
#define T_9F39_81		0x81	//Piste magnétique ISO 2, traitement mode dégradé carte à microcircuit (non utilisable en retrait GAB à puce)(2)
#define T_9F39_82		0x82	//Issu d’un serveur
#define T_9F39_91		0x91	//Transaction puce sans contact utilisant les données piste



//TERMINAL TYPE
//=============
//TAG 9F35
//										Operational Control Provided By:
//Environment						Financial Institution		Merchant			Cardholder 19
//Attended
//	Online only								11					21					—
//	Offline with online capability			12					22					—
//	Offline only							13					23					—
//Unattended
//	Online only								14					24					34
//	Offline with online capability			15					25					35
//	Offline only							16					26					36


#define T_9F35_11		0x11	
#define T_9F35_12		0x12	
#define T_9F35_13		0x13	
#define T_9F35_14		0x14	
#define T_9F35_15		0x15	
#define T_9F35_16		0x16	

#define T_9F35_21		0x21	
#define T_9F35_22		0x22	
#define T_9F35_23		0x23	
#define T_9F35_24		0x24	
#define T_9F35_25		0x25	
#define T_9F35_26		0x26	

#define T_9F35_34		0x34	
#define T_9F35_35		0x35	
#define T_9F35_36		0x36	

//TERMINAL CAPABILITIES
//=====================

//Manual key entry						
//x 1 x x x x x x Magnetic stripe
//x x 1 x x x x x IC with contacts
//x x x 0 x x x x RFU
//x x x x 0 x x x RFU
//x x x x x 0 x x RFU
//x x x x x x 0 x RFU
//x x x x x x x 0 RFU

//1 x x x x x x x Plaintext PIN for ICC verification
//x 1 x x x x x x Enciphered PIN for online verification
//x x 1 x x x x x Signature (paper)
//x x x 1 x x x x Enciphered PIN for offline verification
//x x x x 1 x x x No CVM Required
//x x x x x 0 x x RFU
//x x x x x x 0 x RFU
//x x x x x x x 0 RFU


//1 x x x x x x x SDA
//x 1 x x x x x x DDA
//x x 1 x x x x x Card capture
//x x x 0 x x x x RFU
//x x x x 1 x x x CDA
//x x x x x 0 x x RFU
//x x x x x x 0 x RFU
//x x x x x x x 0 RFU

//CHAMP 55 ou CHAMP 58
//TAG 9F33

#define T_9F33_B1_80	0x80	
#define T_9F33_B1_40	0x40	
#define T_9F33_B1_20	0x20	

#define T_9F33_B2_80	0x80	
#define T_9F33_B2_40	0x40	
#define T_9F33_B2_20	0x20	
#define T_9F33_B2_10	0x10	
#define T_9F33_B2_08	0x08	

#define T_9F33_B3_80	0x80	
#define T_9F33_B3_40	0x40	
#define T_9F33_B3_20	0x20	
#define T_9F33_B3_08	0x08	


#endif