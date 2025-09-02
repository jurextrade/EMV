#ifndef __CARD_H
#define __CARD_H
#include <Winscard.h>
#include "mx.h"
#include "EMVTools.h"
#include "EMVError.h"

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

#define NBR_MAX_READERSTATE		4

#define  EurCode   "978"
#define  FranceCode "x02\x50"

typedef struct _Card
{
	char				UserName[30];
	char				UserPassword[30];
	SCARDHANDLE			hCardHandle;
	SCARD_IO_REQUEST    Request;
	char				strATR[100];
	BYTE				MediaType;
	MXCom* pCom;                 //EMVServer
}CARD;


typedef struct _CardContext {
	SCARDCONTEXT		hContext;
	LPTSTR          	pReaders;
	int					nbrTryConnect;
	int					ReadersCount;
	int					TimeOut;
	SCARD_READERSTATE*	pCurrentState;
	SCARD_READERSTATE   ReadersStates[NBR_MAX_READERSTATE];
	CARD*				pCurrentCard;
	int					ShouldRelase;
	List*				appApduErrors;
	int                 WebUser;
}CardContext;


typedef struct _CardConnector {
	MX* pMX;
	CardContext*		pCardContext;
	MXCom*				pRouterCom;				//Router Com
	char				UserName[30];
	char				UserPassword[30];
	char				UserID[30];
}CC;











#ifdef __cplusplus
extern "C"
{
#endif

extern char		Directory[300];
extern char		EMVServer[300];
extern int		EMVServerPort;
extern char		EMVRooter[300];
extern int		EMVRooterPort;
extern char		LoginServer[300];
extern BYTE		transactiontype;
extern char		amount[100];
extern char		currency[4];
extern char		smessage[1300];
extern CARD*	CurrentCard;
extern CC*		CardConnector;


extern MXCom*	EMVRooterCom;
extern MXCom*	EMVServerCom;

extern int			MXAddAPDUCommands(MX* pmx);
extern int			CardApplicationProcedure(MX* pmx, void* par);
extern int			OnConnect(MXCom* pcom, void* applicationfield);
extern int			OnClose(MXCom* pcom, void* applicationfield);

extern long			Readers_Init(CardContext* pCardContext);
extern int			ReaderPlugging(CardContext* pCardContext, long lReturn);

extern				CC* CCInit(MX* pmx);
extern				void CCEnd(CC* pcc);

extern char*		CardStrError(const long pcscError);
extern char			CardCorrectATR(unsigned char* bufATR, int size);


extern CardContext* CardContext_Init();
extern LONG			CardContext_End(CardContext* pCardContext);



extern int			CardRead ();
extern char			CardAPDU (CARD* pCard, unsigned char cla, unsigned char ins, unsigned char p1, unsigned char p2,  unsigned char dataSize, unsigned char* data,
								            int* outDataSize, unsigned char* outData);
extern int			OnCardConnected(CC* pcc);
extern void			OnCardDisconnected(CARD* pCard);


extern MXCom*		Connect_EMVServer(CC* pcc, CARD* pCard);

extern int			SendTransaction(MXCom* pcom, char type, char* currency, char* amount, BYTE mediatype);
extern int			SendATR(MXCom* pcom, unsigned char* atr);
extern int			SendUserInfo(MXCom* pcom, char* username, char* password);

extern int          OnRecvACFirst            (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnSendACFirst            (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnRecvACSecond           (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnSendACSecond           (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnSendVerify             (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnRecvVerify             (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnRecvAPDU               (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnSendAPDU               (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnSendATR                (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnSendTransaction        (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnRecvSendAppliSelection (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int          OnRecvSendCommand        (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);

//Errors
extern int			EMVReadApduErrorFile(CardContext* pCardContext);
extern void			EMVAddApduError(CardContext* pCardContext, EMVApduError* perror);
extern EMVApduError* EMVGetErrorFromSW1SW2(CardContext* pCardContext, BYTE SW1, BYTE SW2);

extern MXCom*		Connect_RouterServer(CC* pcc);

extern void			Send_Start(MXCom* pcom);
extern void			Send_Trace(MXCom* pcom, char* message);
extern void			Send_Select(MXCom* pcom, char* message);
extern void			Send_Plug(MXCom* pcom, char* message);
void				Send_APDU(MXCom* pcom, BYTE cla, BYTE ins, BYTE p1, BYTE p2, int datasize, unsigned char* data, int way);
extern int			Send_Login(CC* pcc);

extern int			OnSendRouter(MXMessage* pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvRouter(MXMessage* pmessage, MXCom* pcom, void* applicationfield);
extern void 		s_printf(char* message, char* format, char* string);

#ifdef __cplusplus
};
#endif

#endif 

