#ifndef __CB2A_H
#define __CB2A_H
#include "cb2aCode.h"
#include "EMVTag.h"

#define CB2A_MAXFIELD_NBR			128
#define CB2A_IDENTIFIER_SIZE		4

#define CB2A_IDENTIFIER_FIELD_NBR	0
#define CB2A_BITMAP_FIELD_NBR		1

#define CB2A_MAX_FIELD_NUMBER		128					/* Max Fields */

//	a caract�re alphab�tique ( �A� � �Z�, �a� � �z�) 
//	n caract�re num�rique ( �0� � �9�) 
//	p Caract�re �espace� 
//	s caract�re sp�cial (espace inclus) 
//	an caract�re alphanum�rique 
//	as caract�re alphab�tique ou sp�cial 
//	ns caract�re num�rique ou sp�cial
//	ans caract�re alphanum�rique ou sp�cial 
//	b donn�e binaire 
//	z codes relatifs aux pistes magn�tiques 2 et 3 
//	AA ann�e (2 caract�res num�riques) 
//	MM mois (2 caract�res num�riques) 
//	JJ jour (2 caract�res num�riques) 
//	hh heures (2 caract�res num�riques) 
//	mm minutes (2 caract�res num�riques) 
//	ss secondes (2 caract�res num�riques) 
//	x � C � pour cr�dit, � D � pour d�bit ; est toujours associ� avec un champ num�rique d�signant un montant de transaction ;
//	par exemple, x + n16 signifie cr�dit ou d�bit d�un montant sur 16 caract�res num�riques. 
//	Les montants sont attach�s � un sens : - Le sens "D" signifie "d�bit porteur" transpos� dans la relation accepteur/acqu�reur, 
//	il signifie "d�bit de la banque acqu�reur" ; 
//	ce qui signifie dans tous les cas qu'il s'agit d'un "cr�dit" pour l'accepteur. "D" = Cr�dit de l'accepteur - 
//	Le sens "C" signifie "cr�dit porteur" transpos� dans la relation accepteur/acqu�reur, il signifie "cr�dit de la banque acqu�reur" ; 
//	ce qui signifie dans tous les cas qu'il s'agit d'un "d�bit" pour l'accepteur. "C" = D�bit de l'accepteur � Tableau 

//	L longueur d�un TLV 
//	LL longueur pouvant �tre exprim�e sur 2 caract�res significatifs (1 � 99) 
//	LLL longueur pouvant �tre exprim�e sur 3 caract�res significatifs (1 � 255) 
//	3 longueur fixe de 3 unit�s(1) 
//	...15 longueur variable jusqu'� 15 unit�s(1) 
//	3�15 longueur variable de 3 � 15 unit�s(1) 
/*----------------------------------------------------------------------- */
//	Format de la donn�e					Num�rique n		   Binaire b, ansb, �			Caract�res a, an, ns, �		Piste magn�tique z 
//	Num�rique n								DCB (1) 		DCB (1)						ASCII (2.1)
//	Caract�res a, an, as, ns, ans, �							ASCII (3)					ASCII (2.2) 
//	Num�rique sign� x+n										ASCII + DCB (4)				ASCII (2.3) 
//	Binaire b, ansb, anscb, �									(5)							ASCII (6)
//	Piste magn�tique z																										(7) 





//0100 Demande d'autorisation
//0110 R�ponse � demande d'autorisation
//0400 Demande de redressement
//0401 R�p�tition de demande de redressement
//0410 R�ponse � demande de redressement
//0800 Demande de gestion de r�seau
//0810 R�ponse � demande de gestion de r�seau




typedef struct _CB2AMessage {
	BYTE			Identifier[2];
	BYTE			Bitmap[16];
	List* FieldsValue;
	BYTE* tlv_buffer;
	int				tlv_allocated;
	int				tlv_length;
	char			tlv_debug_enabled;
}CB2AMessage;



typedef struct _CB2AField
{
	int				Field;
	char			strField[7];
	char			Name[200];
	char			Format[20];				//LLVAR ans...120
	BYTE			DataType;				// b, n, a
	BYTE			IsVariable;
	int				Size;
	int             From;
	int             To;
	char			Requirements[20];
	int				EMVTag;
	BYTE* Value;

	List* Tags;
}CB2AField;




typedef struct _CB2ATag
{
	int				Tag;
	char			strTag[7];
	char			Name[200];
	char			Format[20];
	BYTE			DataType;
	BYTE			IsVariable;
	int				Size;
	int             From;
	int             To;

	char			Requirements[20];

	int				EMVTag;
	BYTE* Value;
	CB2AField* pField;
}CB2ATag;


typedef struct _CB2AFieldValue
{
	CB2AField* pField;
	int				Size;
	BYTE* Value;
	List* TagsValue;
}CB2AFieldValue;


typedef struct _CB2ATagValue
{
	CB2ATag* pTag;
	int				Size;
	BYTE* Value;
}CB2ATagValue;



typedef struct _CB2A {
	List* Fields;
}CB2A;



#ifdef __cplusplus
extern "C"
{
#endif


	extern CB2A* CB2AInit(int protocol);
	extern void			CB2AEnd(CB2A* pcb2a);
	extern int			CB2AReadTagFile(CB2A* pcb2a);
	extern int          CB2AGetFieldSize(CB2AField* pField);
	extern int          CB2AGetTagSize(CB2ATag* pTag);
	extern void			CB2AAddTag(CB2A* pcb2a, CB2AField* pfield, CB2ATag* ptag);
	extern void			CB2AAddField(CB2A* pcb2a, CB2AField* pfield);

	extern int			CB2AWriteTag(CB2ATag* ptag, char* buffer, BYTE* Output);


	extern void			CB2AInitBuffer(CB2AMessage* pmessage);
	extern void			CB2AEndBuffer(CB2AMessage* pmessage);
	extern void			CB2AClearBuffer(CB2AMessage* pmessage);
	extern void			CB2ACheckAndExtendBuffer(CB2AMessage* pmessage, int incrSize);


	extern char* CB2AGetTagNameFromHexStr(CB2A* pcb2a, CB2AField* pfield, char* stag);
	extern char* CB2AGetTagNameFromDecimal(CB2A* pcb2a, CB2AField* pfield, int tag);
	extern CB2ATag* CB2AGetTagFromDecimal(CB2A* pcb2a, CB2AField* pfield, int tag);

	extern CB2AField* CB2AGetFieldFromDecimal(CB2A* pcb2a, int field);
	extern char* CB2AGetFieldNameFromDecimal(CB2A* pcb2a, int field);

	extern CB2AMessage* CB2AInitMessage(CB2A* pcb2a, char* sidentifier);
	extern void			CB2AEndMessage(CB2AMessage* pmessage);


	extern CB2AFieldValue* CB2AInitFieldValue(CB2A* pcb2a, CB2AField* pfield);
	extern void			CB2ASetTag(CB2AMessage* pmessage, CB2ATagValue* ptagvalue, EMVTag* pemvtag, BYTE* data, int size);
	extern void			CB2ASetField(CB2AMessage* pmessage, CB2AFieldValue* pfieldvalue, EMVTag* pemvtag, BYTE* data, int size);
	extern int	CB2AWriteEMVTag(int tagtype, int size, int fieldtype, char* buffer, int buffersize, BYTE* Output);
#ifdef __cplusplus
};
#endif

#endif // __EMV_H
