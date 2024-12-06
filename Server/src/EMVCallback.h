#ifndef __EMVCALLBACK_H
#define __EMVCALLBACK_H

#include "mx.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern int			OnConnect (MXCom* pcom, void* applicationfield);
extern int			OnClose (MXCom* pcom, void* applicationfield);
//Client
extern int			OnRecvUserInfo(MXMessage* pmessage, MXCom* pcom, void* applicationfield);
extern int			OnSendACFirst (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvACFirst (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnSendVerify (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvVerify (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);

extern int			OnSendACSecond (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvACSecond (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnSendAPDU (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvAPDU (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvATR (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvError (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvTransaction (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvCommand (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvVerify (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
//CB2A
extern int			OnRecvReply (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnRecvAppliSelection (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);


////////////////////////////////////////
extern int			OnLoadAcceptor (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnLoadApplications (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnLoadTacs (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnLoadExceptionCards (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnLoadRangeBins (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnLoadAuthorityPublicKeys (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
extern int			OnLoadCurrencies (MXMessage*  pmessage, MXCom* pcom, void* applicationfield);
#ifdef __cplusplus
};
#endif

#endif 
