#pragma once

#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

// THIS_IS_SERVICE_APP is defined in the PAM project under C++ | Preprocessor Definitions 

#ifdef THIS_IS_SERVICE_APP
//#define MAIN_DLG_NAME	CTscanDlg
#else
#define MAIN_DLG_NAME	CTscanDlg
#endif


#ifndef SERVER_CONNECTION_MANAGEMENT_H
#include "afxsock.h"
#include "ServerConnectionManagement.h"
#endif
#include "HwTimer.h"


#define MAX_PAM_BYTES	1260*8			
// expected msg size is 1260*8 = 10080

enum { eListener, eServerConnection };

// CServerSocket command target

class CServerConnectionManagement;
class CServerSocketOwnerThread;

//*pSCM;
class CServerSocket : public CAsyncSocket
{
public:
	CServerSocket(CServerConnectionManagement *pSCM);
	CServerSocket();


	virtual ~CServerSocket();
	void Init(void);
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

	int InitListeningSocket(CServerConnectionManagement * pSCM);
	void SetSCM(CServerConnectionManagement *pSCM)		{	m_pSCM = pSCM;	}
	(CServerConnectionManagement *) GetSCM(void)		{	return m_pSCM;	}

	void OnAcceptInitializeConnectionStats(ST_SERVERS_CLIENT_CONNECTION *pscc, int nMyServer, int nClientPortIndex);
	void KillpClientConnectionStruct(void);

	//int SendPacket(BYTE *pB, int nBytes, int nDeleteFlag);

	void LockRcvPktList(void)		{ if (m_pSCC) { if (m_pSCC->pCSRcvPkt) EnterCriticalSection(m_pSCC->pCSRcvPkt);	}	}
	void AddTailRcvPkt(void *pV)	{ if (m_pSCC) { if (m_pSCC->pRcvPktList) m_pSCC->pRcvPktList->AddTail(pV);	}	}
	void UnLockRcvPktList(void)		{ if (m_pSCC) { if (m_pSCC->pCSRcvPkt) LeaveCriticalSection(m_pSCC->pCSRcvPkt);	}	}

	void LockSendPktList(void)		{ if (m_pSCC) { if (m_pSCC->pCSSendPkt) EnterCriticalSection(m_pSCC->pCSSendPkt);}	}
	void AddTailSendPkt(void *pV)	{ if (m_pSCC) { if (m_pSCC->pSendPktList) m_pSCC->pSendPktList->AddTail(pV);}	}
	void UnLockSendPktList(void)	{ if (m_pSCC) { if (m_pSCC->pCSSendPkt) LeaveCriticalSection(m_pSCC->pCSSendPkt);}	}

	//void KillMyThread(void)			{ m_pSCC->bExitThread = 1;					}

	UINT GetPacketsSent(void)		{ return (m_pSCC ?  m_pSCC->uPacketsSent :  NULL );	}
	void SetClientIp4(CString s)	{ m_sClientIp4 = s;				}
	CString GetClientIp4(void)		{ return m_sClientIp4;			}

	void * GetWholePacket(int nPacketSize, int *pReceived);

	BYTE GetConnectionStatus(void)		{ return (m_pSCC ?	m_pSCC->bConnected : eInstrumentNotPresent);	}
	void SetConnectionStatus(BYTE s)	{ if	 (m_pSCC)	m_pSCC->bConnected = s;							}

	// vairables
	CServerConnectionManagement *m_pSCM;		// ptr to the controlling class
	ST_SERVERS_CLIENT_CONNECTION *m_pSCC;		// ptr to my connection info/statistics/objects
	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// pointer to my global structure instance 
	int m_nMyServer;							// which server are we connected stSCM[MAX_SERVERS]
	int m_nMyThreadIndex;						// which instance of pClientConnection[MAX_CLIENTS_PER_SERVER];
	CWinThread * m_pThread;						// ptr to thread which created the socket
	int m_nOwningThreadType;					// 0=Listener, 1=ServerConnection thread owns this socket class
	CString m_sClientIp4;						// IP4 address... 192.168.123.123 etc of server connected socket

	int m_nStart;								// used by GetWholePacket
	BYTE m_RcvBuf[0x10000];						// 16 k receiver buffer .. now 64k
	int m_BufOffset;
	// debug info
	int m_nMaxBufOffset;
	int m_nMaxStart;
	int m_nRcvRqst;								// how many can we take
	int m_nMinRcvRqst;							// smallest packet requested to receive
	int m_nMaxRcvRqst;
	CString szName;
	void *pWholePacket;
	CHwTimer *m_pElapseTimer;
	int m_nElapseTime;
	};

#endif

