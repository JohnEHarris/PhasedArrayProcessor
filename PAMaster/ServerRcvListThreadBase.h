#pragma once

#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
#include "../include/pa2struct.h"
#else
#include "Truscan.h"
#include "TScanDlg.h"
#endif


// CServerRcvListThreadBase

// Prior to 11-12-2012 this class was a child of CServerSocketOwnerThread
// After this date, make it a child of CWinThread
// It exists primarily to include the global structures and the socket for 
// specific classes derived from it
//
//class CServerRcvListThreadBase : public CServerSocketOwnerThread
class CServerSocket;
class CServerSocketPA_Master;
class CServerConnectionManagement;

class CServerRcvListThreadBase : public CWinThread
	{
	DECLARE_DYNCREATE(CServerRcvListThreadBase)

protected:
	CServerRcvListThreadBase();           // protected constructor used by dynamic creation
	virtual ~CServerRcvListThreadBase();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// 11-16-2012
	CServerConnectionManagement *m_pMySCM;		// managing class ptr
	int m_nMyServer;							// which instance of stSCM[] we are
	int m_nThreadIndex;							// which one of SSC are we
	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// managing structure ptr
	ST_SERVERS_CLIENT_CONNECTION *m_pSCC;		// ptr to my connection info/statistics/objects

	//SOCKET m_hConnectionSocket;
	//CServerSocket m_ConnectionSocket;				// server's connection to the client .. on stack
	//CString m_Ip4;
	//UINT m_uPort;


protected:
	DECLARE_MESSAGE_MAP()

	};


