#pragma once

#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

// I_AM_PAP is defined in the PAP project under C++ | Preprocessor Definitions 

#ifdef I_AM_PAP
//#define MAIN_DLG_NAME	PA2WinDlg
#else
//#define MAIN_DLG_NAME	PA2WinDlg
#endif


#ifndef SERVER_CONNECTION_MANAGEMENT_H
#include "afxsock.h"
#include "ServerConnectionManagement.h"
#include "CCmdFifo.h"
#endif
#include "../Include/Cmds.h"
#include "CCmdFifo.h"
#include "HwTimer.h"


#define MAX_PAM_BYTES	1260*8			
// expected msg size is 1260*8 = 10080

// Listener thread creates and holds the eListener socker
// Permanent Server connection thread are designated as eServerConnection
// The temporary socket used by OnAccept to build the server connetion socket is the eOnStack socket
enum { eListener, eServerConnection,eOnStack };

// CServerSocket command target

class CServerConnectionManagement;
class CServerSocketOwnerThread;

//*pSCM;
class CServerSocket : public CAsyncSocket
{
public:
	CServerSocket();
	CServerSocket(CServerConnectionManagement *pSCM, int nOwningThreadType);

	virtual ~CServerSocket();
	void Init(void);
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

	int InitListeningSocket(CServerConnectionManagement * pSCM);
	void SetSCM(CServerConnectionManagement *pSCM)		{	m_pSCM = pSCM;	}
	CServerConnectionManagement * GetSCM(void)		{	return m_pSCM;	}

	void OnAcceptInitializeConnectionStats(ST_SERVERS_CLIENT_CONNECTION *pscc, int nMyServer, int nClientPortIndex);
	//void KillpClientConnectionStruct(void);

	//int SendPacket(BYTE *pB, int nBytes, int nDeleteFlag);

	void LockRcvPktList(void)		{ if (m_pSCC) { if (m_pSCC->pCSRcvPkt) EnterCriticalSection(m_pSCC->pCSRcvPkt);	}	}
	void AddTailRcvPkt(void *pV)	{ if (m_pSCC) { if (m_pSCC->pRcvPktList) m_pSCC->pRcvPktList->AddTail(pV);	}	}
	int GetRcvListCount(void)		{ if (m_pSCC) { if (m_pSCC->pRcvPktList) return m_pSCC->pRcvPktList->GetCount(); } return 0; }
	void UnLockRcvPktList(void)		{ if (m_pSCC) { if (m_pSCC->pCSRcvPkt) LeaveCriticalSection(m_pSCC->pCSRcvPkt);	}	}

	void LockSendPktList(void)		{ if (m_pSCC) { if (m_pSCC->pCSSendPkt) EnterCriticalSection(m_pSCC->pCSSendPkt);}	}
	void AddTailSendPkt(void *pV)	{ if (m_pSCC) { if (m_pSCC->pSendPktList) m_pSCC->pSendPktList->AddTail(pV);}	}
	void UnLockSendPktList(void)	{ if (m_pSCC) { if (m_pSCC->pCSSendPkt) LeaveCriticalSection(m_pSCC->pCSSendPkt);}	}

	//void KillMyThread(void)			{ m_pSCC->bExitThread = 1;					}

	UINT GetPacketsSent(void)		{ return (m_pSCC ?  m_pSCC->uPacketsSent :  NULL );	}
	void SetClientIp4(CString s)	{ m_sClientIp4 = s;				}
	CString GetClientIp4(void)		{ return m_sClientIp4;			}

	//void * GetWholePacket(int nPacketSize, int *pReceived);

	BYTE GetConnectionStatus(void)		{ return (m_pSCC ?	m_pSCC->bConnected : eInstrumentNotPresent);	}
	void SetConnectionStatus(BYTE s)	{ if	 (m_pSCC)	m_pSCC->bConnected = s;							}

	void SetClientConnectionIndex( int indx ) { m_nClientIndex = indx; }
	// call these get/set function from ServerSocketOwnerThread ExitInstance to update
	// the values
	ST_SERVERS_CLIENT_CONNECTION * GetpSCC( void );
	// is is the client connection number
	void SetpSCC( ST_SERVERS_CLIENT_CONNECTION* p ) { m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] =  p; }
	void NullpSCC(void)								{m_pSCM->m_pstSCM->pClientConnection[m_nClientIndex] =  0;}
	
	
	// variables
	CServerConnectionManagement *m_pSCM;		// ptr to the controlling class
	ST_SERVERS_CLIENT_CONNECTION *m_pSCC;		// ptr to my connection info/statistics/objects
	ST_SERVER_CONNECTION_MANAGEMENT *m_pstSCM;	// pointer to my global structure instance 
	int m_nMyServer;							// which server are we connected stSCM[MAX_SERVERS]
	int m_nClientIndex;							// which instance of pClientConnection[MAX_CLIENTS_PER_SERVER];
	CWinThread * m_pThread;						// ptr to thread which created the socket
	int m_nOwningThreadType;					// 0=Listener, 1=ServerConnection thread owns this socket class
	int m_nOwningThreadId;				// debugging
	int m_nAsyncSocketCnt;				// debugging

	CString m_sClientIp4;						// IP4 address... 192.168.123.123 etc of server connected socket
	CString szName;

	CCmdFifo *m_pFifo;		// In ClientConnectionManagement get PAG commands. Here gets instrument data
							// created in CServerSocket::CServerSocket(CServerConnectionManagement *pSCM)
							// deleted in CServerSocket::~CServerSocket()
	CHwTimer *m_pElapseTimer;	// created in CServerSocket::CServerSocket()
								// deleted in CServerSocket::~CServerSocket()
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

