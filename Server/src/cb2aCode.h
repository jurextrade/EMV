#ifndef __CB2ACODE_H
#define __CB2ACODE_H


#include "tagcode.h"


//Champ 53 Format : n16
//Informations li?es ? la s?curit? (Security related control information).
//Le champ 53 contient des informations n?cessaires ? l'exploitation des donn?es li?es ? la s?curit? contenues dans le
//message.
//? Non utilis? ___________________________________________________________________________ quartet 1
//? V?rifications utilis?es par le demandeur __________________________________________________ quartet 2
//Seul l’?l?ment ‘V?rifications effectu?es par le demandeur’ du champ 53 est actuellement utilis?. Les valeurs peuvent
//?tre :
//0 code confidentiel non contr?l? par le demandeur
//1 code confidentiel contr?l? et correct
//2 code confidentiel contr?l? et incorrect
//3 code confidentiel contr?l? et incorrect, nombre maximal d’essais atteint
//? Non utilis? _______________________________________________________________________

// 
//CHAMP 53

#define F_53_0			0
#define F_53_1			1
#define F_53_2			2
#define F_53_3			3
//Motif de la transaction non aboutie 
//CHAMP 58
//TAG FF50

#define F_58_FF50_01	0x01	//	Donn?e carte EMV absente
#define F_58_FF50_02	0x02	//	Erreur lors de la lecture des compteurs Carte
#define F_58_FF50_03	0x03	//	Donn?es obligatoires de la carte absentes
#define F_58_FF50_04	0x04	//	Donn?e carte EMV redondante
#define F_58_FF50_05	0x05	//	Erreur de format dans les donn?es Carte
#define F_58_FF50_06	0x06	//	Erreur lors de l'authentification de la Carte
#define F_58_FF50_07	0x07	//	Transaction abandonn?e par le porteur lors de la saisie du code
#define F_58_FF50_08	0x08	//	Erreur lors de la v?rification du code confidentiel offline
#define F_58_FF50_09	0x09	//	Erreur lors de la premi?re demande de cryptogramme ? la carte
#define F_58_FF50_10	0x10	//	Erreur lors de la seconde demande de cryptogramme ? la carte
#define F_58_FF50_11	0x11	//	Transaction refus?e au premier Generate AC
#define F_58_FF50_12	0x12	//	Transaction refus?e au second Generate AC
#define F_58_FF50_13	0x13	//	Validation d’une transaction de cr?dit incorrecte (Bin non trouv? en table de BIN et mouvement initial non trouv?)
#define F_58_FF50_14	0x14	//	Validation d’une transaction d’annulation incorrecte
#define F_58_FF50_15	0x15	//	Structure de la carte invalide (Longueur, date de validit? ou cl? de l?hn invalide)
#define F_58_FF50_16	0x16	//	Donn?es permettant de v?rifier la signature de la carte absentes
#define F_58_FF50_17	0x17	//	La carte est p?rim?e et ne peut ?tre utilis?e
#define F_58_FF50_18	0x18	//	La carte est en opposition
#define F_58_FF50_19	0x19	//	La carte est pr?sente en liste de contr?le en ? interdit ? ou ? refus? ?
#define F_58_FF50_20	0x20	//	La carte est pr?sente dans la liste des BINs en ? interdit ? ou ? refus?
#define F_58_FF50_21	0x21	//	Une demande d’autorisation a ?t? transmise et la r?ponse de la banque ?mettrice est n?gative
#define F_58_FF50_22	0x22	//	La carte est interdite ou refus?e en r?ponse ? une demande d’autorisation ou la r?ponse de la banque ?mettrice demande la capture de la carte
#define F_58_FF50_23	0x23	//	Erreur lors de l'impression du ticket
#define F_58_FF50_24	0x24	//	La transaction d’annulation n’a pas ?t? accept?e (transaction a annuler non trouv?e)
#define F_58_FF50_25	0x25	//	La transaction de cr?dit n’a pas ?t? accept?e
#define F_58_FF50_26	0x26	//	Probl?me ? l’enregistrement de la transaction
#define F_58_FF50_27	0x27	//	Autorisation partielle refus?e par le porteur
#define F_58_FF50_28	0x28	//	Erreur de r?ponse carte au 1er ou second Generate AC mode CDA (erreur carte)
#define F_58_FF50_29	0x29	//	Erreur de r?ponse carte au 1er ou second Generate AC mode SDA/DDA (erreur carte)
#define F_58_FF50_30	0x30	//	Probl?me de distribution du bien ou service
//contactless
#define F_58_FF50_50	0x50	//	Des donn?es obligatoires de la carte (AFL, AIP) sont absentes ; la transaction ne peut se poursuivre.
#define F_58_FF50_51	0x51	//	Des donn?es permettant de v?rifier la signature de la carte sont absentes. La transaction ne peut se poursuivre.
#define F_58_FF50_52	0x52	//	Une zone de traitement du CDA est inf?rieure ? la taille n?cessaire.
#define F_58_FF50_53	0x53	//	La m?thode d’authentification support?e par la carte n’est pas le CDA qui est la seule m?thode support?e
#define F_58_FF50_54	0x54	//	La date de d?but de validit? de l’application carte n’est pas atteinte et ne peut dans ce cadre ?tre utilis?e
#define F_58_FF50_55	0x55	//	La carte est p?rim?e et ne peut ?tre utilis?e. Sa v?rification est bas?e sur les r?gles EMV.
#define F_58_FF50_56	0x56	//	La carte pr?sent?e est une carte de tests dans le r?f?rentiel des listes d’acceptation.
#define F_58_FF50_57	0x57	//	La carte n’est pas param?tr?e (AUC) pour ?tre utilis?e sur ce point d’acceptation.
#define F_58_FF50_58	0x58	//	L’authentification propos?e au porteur n’est pas support?e par l’application de votre terminal.
#define F_58_FF50_59	0x59	//	La carte ne poss?de pas de CVM list
#define F_58_FF50_60	0x60	//	L’authentification du porteur a ?chou? ou les authentifications du porteur ne sont pas compatibles avec celles du terminal.
#define F_58_FF50_61	0x61	//	Erreur durant le d?roulement de l’authentification carte (CDA)
#define F_58_FF50_62	0x62	//	La carte ?tant en opposition, la transaction n’a pu aboutir
#define F_58_FF50_63	0x63	//	Le Bin de la carte est refus? ou interdit dans la table des BIN de l’application
#define F_58_FF50_64	0x64	//	Le certificat re?u de la carte n’est pas interpr?table
#define F_58_FF50_65	0x65	//	La date d’expiration est < ? la date locale. Le param?trage de la carte demande le refus de la transaction. Cette d?cision est uniquement une d?cision carte (Visa).
#define F_58_FF50_66	0x66	//	La carte demande de changer d’interface sur un ?chec d’authentification (Visa) Cette d?cision est uniquement une d?cision carte (Visa)
#define F_58_FF50_67	0x67	//	Abandon de la transaction demand?e par le commer?ant dans le cadre d’une annulation. Cette d?cision concerne tous les kernels
#define F_58_FF50_68	0x68	//	Un probl?me a ?t? rencontr? sur le mobile durant la phase de l’authentification porteur. Ceci peut correspondre ? un d?lai ?chu avant repr?sentation du mobile est ?chu. Cette d?cision concerne tous les kernels
#define F_58_FF50_69	0x69	//	Une demande d’autorisation a ?t? transmise et la r?ponse de la banque ?mettrice est n?gative.
#define F_58_FF50_70	0x70	//	Une demande d’autorisation a ?t? transmise et la r?ponse de la banque ?mettrice est de type demande de capture.
#define F_58_FF50_71	0x71	//	Erreur lors de l'impression du ticket
#define F_58_FF50_72	0x72	//	La transaction d’annulation n’a pas ?t? accept?e. Ceci est peut ?tre d? aux ?l?ments contr?l?s ou la transaction d’origine non trouv?e
#define F_58_FF50_73	0x73	//	La transaction de cr?dit n’a pas ?t? accept?e.
#define F_58_FF50_80	0x80	//	La transaction n’a pas ?t? transmise pour des probl?mes de communication
#define F_58_FF50_81	0x81	//	La r?ponse n’est pas parvenue
#define F_58_FF50_82	0x82	//	Erreur lors de l’impression du ticket
#define F_58_FF50_83	0x83	//	Structure de la r?ponse invalide
#define F_58_FF50_84	0x84	//	Une demande d’autorisation a ?t? transmise et la r?ponse de la banque ?mettrice est n?gative
#define F_58_FF50_85	0x85	//	L’autorisation partielle a ?t? refus?e par le porteur
//both
#define F_58_FF50_87	0x87	//	Un timer est arriv? ? ?ch?ance ? la pr?sentation du dispositif ou de la carte.

//Code Raison (Autorisation ou Redressement)
//CHAMP 59
//TAG 0101

//Autorisation
#define F_59_0101_1503	0x1503	//D?clenchement al?atoire par terminal
#define F_59_0101_1506	0x1506	//? on-line ? forc? par l’accepteur de carte
#define F_59_0101_1507	0x1507	//? on-line ? forc? par le dispositif d’acceptation de carte pour MAJ
#define F_59_0101_1508	0x1508	//? on-line ? forc? par le terminal
#define F_59_0101_1509	0x1509	//? on-line ? forc? par l’?metteur de carte (code service)
#define F_59_0101_1510	0x1510	//D?passement seuil d’appel
#define F_59_0101_1511	0x1511	//Soup?ons du commer?ant
#define F_59_0101_1512	0x1512	//BIN interdit
#define F_59_0101_1513	0x1513	//Carte interdite
#define F_59_0101_1651	0x1551	//Cumul/porteur/application
#define F_59_0101_1652	0x1552	//BIN surveill?
#define F_59_0101_1653	0x1553	//BIN inconnu
#define F_59_0101_1654	0x1554	//N? surveill?
#define F_59_0101_1655	0x1555	//Demande de pr?-autorisation
#define F_59_0101_1656	0x1556	//Forc? par l’?metteur (contr?le de flux)
#define F_59_0101_1657	0x1557	//Monnaie ?trang?re
#define F_59_0101_1658	0x1558	//Code monnaie ou devise de la transaction inconnu
#define F_59_0101_1659	0x1559	//Carte refus?e
#define F_59_0101_1660	0x1560	//Appel suite ? un ARQC ?mis par la carte
#define F_59_0101_1663	0x1563	//BIN refus?
#define F_59_0101_1664	0x1564	//Strictement on-line
#define F_59_0101_1665	0x1565	//Off-line avec des capacit?s on-line
#define F_59_0101_1671  0x1571	//Transaction puce sans contact utilisant les donn?es piste
#define F_59_0101_1672	0x1572	//Carte en mode SDA

//Redressement

#define F_59_0101_4000	0x4000	//Annulation par le client
#define F_59_0101_4007	0x4007	//Appareil de l’accepteur de carte incapable d’effectuer la transaction
#define F_59_0101_4200	0x4200	//D?cision porteur
#define F_59_0101_4201	0x4201	//D?cision terminal
#define F_59_0101_4202	0x4202	//D?cision carte
#define F_59_0101_4203	0x4203	//D?cision porteur ou terminal
#define F_59_0101_4204	0x4204	//D?cision accepteur

//code Fonction
//CHAMP 59
//TAG 0100

#define F_59_0100_100	100		//Autorisation initiale – montant exact
#define F_59_0100_101	101		//Autorisation initiale – montant estim?
#define F_59_0100_106	106		//Autorisation suppl?mentaire – montant exact
#define F_59_0100_107	107		//Autorisation suppl?mentaire – montant estim?
#define F_59_0100_108	108		//Demande de renseignement
#define F_59_0100_163	163		//Facture compl?mentaire
#define F_59_0100_164	164		//Facture no-show

//Champ 25
//Code Raison
//Autorisation Redressement

#define F_25_00			0x00	//Conditions normales
#define F_25_01			0x01	//Client non pr?sent
#define F_25_02			0x02	//Terminal libre-service pouvant conserver la carte
#define F_25_03			0x03	//Suspicion de fraude de la part du commer?ant
#define F_25_07			0x07	//Demande par t?l?phone (via centre d’appels)
#define F_25_08			0x08	//Achat par correspondance ou t?l?phone
#define F_25_10			0x10	//Identit? du client v?rifi?e
#define F_25_11			0x11	//Suspicion de fraude
#define F_25_12			0x12	//Raisons de s?curit?
#define F_25_15			0x15	//Terminal ? domicile
#define F_25_27			0x27	//Terminal libre service n’ayant pas la capacit? de conserver la carte

//Reseaux

#define F_25_8010		0x8010	//Programm? par le syst?me acqu?reur (HHMM)
#define F_25_8011		0x8011	//Changement de configuration du logiciel applicatif syst?me d’acceptation
#define F_25_8012		0x8012	//Changement de configuration mat?rielle syst?me d’acceptation
#define F_25_8013		0x8013	//Premi?re initialisation syst?me d’acceptation
#define F_25_8014		0x8014	//Vidage du fichier de transactions – Initiative accepteur de carte
#define F_25_8017		0x8017	//Premi?re initialisation du syst?me d’acceptation, remplacement de mat?riel
#define F_25_8020		0x8020	//Mise ? jour de param?tres par l’accepteur de carte
#define F_25_8021		0x8021	//Fichier de transactions plein
#define F_25_8022		0x8022	//Reprise suite ? incident
#define F_25_8023		0x8023	//Appel acqu?reur
#define F_25_8024		0x8024	//Appel accepteur, alt?ration de param?tre d?tect?e


//TYPE D’APPLICATION CARTE
//CHAMP 55
//TAG DF81 
#define F_55_DF81_2		2		//EMV
#define F_55_DF81_3		3		//Microcircuit sans contact contexte piste

//CHAMP 59 
//TAG 201 
//ITP Systeme acceptation (Identification d’application terminal : Identification d’application terminal du syst?me d’acceptation.

//Code constructeur n3
//Version des sp?cifications de r?f?rence n3
//Mod?le d’?quipement n3
//Version du logiciel applicatif interbancaire n3

//CHAMP 59 
//TAG 215 

//ITP Point acceptation	(Identification d’application terminal : Identification d’application terminal au point d’acceptation.
//Code constructeur n3
//Version des sp?cifications de r?f?rence n3
//Mod?le d’?quipement n3
//Version du logiciel applicatif interbancaire n3


//TERMINAL TYPE
//=============
//CHAMP 55 ou CHAMP 58
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


#define F_55_OR_58_9F35_11	T_9F35_11	
#define F_55_OR_58_9F35_12	T_9F35_12	
#define F_55_OR_58_9F35_13	T_9F35_13	
#define F_55_OR_58_9F35_14	T_9F35_14	
#define F_55_OR_58_9F35_15	T_9F35_15	
#define F_55_OR_58_9F35_16	T_9F35_16	

#define F_55_OR_58_9F35_21	T_9F35_21	
#define F_55_OR_58_9F35_22	T_9F35_22	
#define F_55_OR_58_9F35_23	T_9F35_23	
#define F_55_OR_58_9F35_24	T_9F35_24	
#define F_55_OR_58_9F35_25	T_9F35_25	
#define F_55_OR_58_9F35_26	T_9F35_26	

#define F_55_OR_58_9F35_34	T_9F35_34	
#define F_55_OR_58_9F35_35	T_9F35_35	
#define F_55_OR_58_9F35_36	T_9F35_36	

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

#define F_55_OR_58_9F33_B1_80	T_9F33_B1_80	
#define F_55_OR_58_9F33_B1_40	T_9F33_B1_40	
#define F_55_OR_58_9F33_B1_20	T_9F33_B1_20	

#define F_55_OR_58_9F33_B2_80	T_9F33_B2_80	
#define F_55_OR_58_9F33_B2_40	T_9F33_B2_40	
#define F_55_OR_58_9F33_B2_20	T_9F33_B2_20	
#define F_55_OR_58_9F33_B2_10	T_9F33_B2_10	
#define F_55_OR_58_9F33_B2_08	T_9F33_B2_08	

#define F_55_OR_58_9F33_B3_80	T_9F33_B3_80	
#define F_55_OR_58_9F33_B3_40	T_9F33_B3_40	
#define F_55_OR_58_9F33_B3_20	T_9F33_B3_20	
#define F_55_OR_58_9F33_B3_08	T_9F33_B3_08	

//CHAMP 47
//TAG 30
#define F_47_30_1				1	//Application sans contact active 


//Point of service Entry mode
//FIELD 22

#define F_22_00	0x00 //Non sp?cifi?
#define F_22_01 0x01 //Manuel
#define F_22_02 0x02 //Zone magn?tique (piste ISO 2 ou piste ISO 1) uniquement
#define F_22_03 0x03 //Code barre
#define F_22_04 0x04 //Lecture optique
#define F_22_05 0x05 //Acc?s microcircuit uniquement(1)
#define F_22_07 0x07 //Transaction puce sans contact utilisant les donn?es puce EMV
#define F_22_81 0x81 //Piste magn?tique ISO 2, traitement mode d?grad? carte ? microcircuit (non utilisable en retrait GAB ? puce)(2)
#define F_22_82 0x82 //Issu d’un serveur
#define F_22_91 0x00 //Transaction puce sans contact utilisant les donn?es piste


//TELEPARAMETRAGE
//FIELD 71 Format  :  n 14 
//Gestion transfert (sens Acqu?reur-Accepteur) 
//(Download file management ). 


#define F_71_01  0x01  //Monnaie ou devise 
#define F_71_02  0x02  //Message porteur 
#define F_71_03  0x03  //Message Accepteur 
#define F_71_04  0x04  //Param?tres accepteur 
#define F_71_05  0x05  //Edition ticket porteur 
#define F_71_06  0x06  //Edition ticket compte rendu 
#define F_71_07  0x07  //Param?tres applicatifs 
#define F_71_08  0x08  //Appel 
#define F_71_09  0x09  //Liste des donn?es sp?cifiques EMV par AID 
#define F_71_10  0x10  //RUF 
#define F_71_11  0x11  //Liste des DOL EMV 
#define F_71_12  0x12  //Risque acqu?reur 
#define F_71_13  0x13  //Liste de contr?le de num?ros de carte porteur  
#define F_71_14  0x14  //Liste de BINs agr?es CB 
#define F_71_15  0x15  //RUF   
#define F_71_16  0x16  //Liste des TAC EMV 
#define F_71_17  0x17  //Autre monnaie ou devise 
#define F_71_18  0x18  //Param?tres d’horodatage GMT 
#define F_71_19  0x19  //Liste cl?s publiques d'authentification EMV 
#define F_71_20  0x20  //Liste des AID EMV 
#define F_71_21  0x21  //T?l?communication t?l?collecte 
#define F_71_22  0x22  //T?l?communication t?l?param?trage 
#define F_71_23  0x23  //T?l?communication autorisation 
#define F_71_24  0x24  //RUF 
#define F_71_25  0x25  //T?l?communication t?l?chargement env. 1 
#define F_71_26  0x26  //T?l?communication t?l?chargement env. 2 
#define F_71_27  0x27  //Appel al?atoire EMV 
#define F_71_28  0x28  //Identifiant pseudo-session de l’accepteur de   carte 
#define F_71_29  0x29  //Liste des donn?es compl?mentaires EMV pour   t?l?collecte 
#define F_71_32  0x32  //Identifiant pseudo-session ?tendu de l'accepteur de carte 
#define F_71_34  0x34  //Param?tres sp?cifiques au paiement sans contact 
#define F_71_35  0x35  //Param?tres ? Dynamic Reader Limits ? pour le paiement sans contact 
#define F_71_36  0x36  //Param?tres ?dition premi?re ligne ticket 
#define F_71_37  0x37  //Fonctions 
#define F_71_38  0x38  //D?sactivation Produits Carte 

#endif