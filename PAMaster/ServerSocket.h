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
#include "CCmdFifo.h"
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
	CServerConnectionManagement * GetSCM(void)		{	return m_pSCM;	}

	int BuildClientConnectionStructure(ST_SERVERS_CLIENT_CONNECTION *pscc, int nMyServer, int nClientPortIndex);
	int KillClientConnectionStructure (ST_SERVERS_CLIENT_CONNECTION *pscc, int nMyServer, int nClientPortIndex);

	//int SendPacket(BYTE *pB, int nBytes, int nDeleteFlag);

	void LockRcvPktList(void)		{ if (m_pSCC) { if (m_pSCC->cpCSRcvPkt) EnterCriticalSection(m_pSCC->cpCSRcvPkt);	}	}
	void AddTailRcvPkt(void *pV)	{ if (m_pSCC) { if (m_pSCC->cpRcvPktList) m_pSCC->cpRcvPktList->AddTail(pV);	}	}
	int GetRcvListCount(void)		{ if (m_pSCC) { if (m_pSCC->cpRcvPktList) return m_pSCC->cpRcvPktList->GetCount(); } return 0; }
	void UnLockRcvPktList(void)		{ if (m_pSCC) { if (m_pSCC->cpCSRcvPkt) LeaveCriticalSection(m_pSCC->cpCSRcvPkt);	}	}

	void LockSendPktList(void)		{ if (m_pSCC) { if (m_pSCC->cpCSSendPkt) EnterCriticalSection(m_pSCC->cpCSSendPkt);}	}
	void AddTailSendPkt(void *pV)	{ if (m_pSCC) { if (m_pSCC->cpSendPktList) m_pSCC->cpSendPktList->AddTail(pV);}	}
	void UnLockSendPktList(void)	{ if (m_pSCC) { if (m_pSCC->cpCSSendPkt) LeaveCriticalSection(m_pSCC->cpCSSendPkt);}	}

	//void KillMyThread(void)			{ m_pSCC->bExitThread = 1;					}

	UINT GetPacketsSent(void)		{ return (m_pSCC ?  m_pSCC->uPacketsSent :  NULL );	}
	void SetClientIp4(CString s)	{ m_sClientIp4 = s;				}
	CString GetClientIp4(void)		{ return m_sClientIp4;			}

	void * GetWholePacket(int nPacketSize, int *pReceived);

	BYTE GetConnectionStatus(void)		{ return (m_pSCC ?	m_pSCC->bConnected : eInstrumentNotPresent);	}
	void SetConnectionStatus(BYTE s)	{ if	 (m_pSCC)	m_pSCC->bConnected = s;							}

	// variables
	CServerConnectionManagement *m_pSCM;		// ptr to the controlling class -- not created with 'new'
	ST_SERVERS_CLIENT_CONNECTION *m_pSCC;		// ptr to my connection info/statistics/objects -- not created with 'new'
	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// pointer to my global structure instance  -- not created with 'new'
	int m_nMyServer;							// which server are we connected stSCM[MAX_SERVERS]
	int m_nMyThreadIndex;						// which instance of pClientConnection[MAX_CLIENTS_PER_SERVER];
	CWinThread * m_pThread;						// ptr to thread which created the socket
	int m_nOwningThreadType;					// 0=Listener, 1=ServerConnection thread owns this socket class
	CString m_sClientIp4;						// IP4 address... 192.168.123.123 etc of server connected socket

	CCmdFifo *m_pFifo;		// In ClientConnectionManagement get PAG commands. Here gets instrument data
	//BYTE m_RcvBuf[0x10000];						// 16 k receiver buffer .. now 64k
	// debug info
	CString szName;
	//void *pWholePacket;				// -- not created with 'new'
	CHwTimer *m_pElapseTimer;
	int m_nElapseTime;
	int m_nOnAcceptClientIndex;		// cheating to let OnAccept pass info to OnClose
	int m_nSeqCntDbg[1024];
	int m_nSeqIndx;
	USHORT m_nLastSeqCnt;

	// debugging 
	GenericPacketHeader m_HeaderDbg[8];
	int m_dbg_cnt;		// counter to select pHeaderDbg variable
	int m_nListCount;	// how deep is the linked list?
	int m_nListCountChanged;

	};

#endif

