#pragma once

#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H


#define MAIN_DLG_NAME	CTscanDlg

#ifndef SERVER_CONNECTION_MANAGEMENT_H
#include "ServerConnectionManagement.h"
#endif

#define MAX_PAM_BYTES	4000

enum { eListener, eServerConnection };

// CServerSocket command target

class CServerConnectionManagement;
class CServerCommunicationThread;

//*pSCM;
class CServerSocket : public CAsyncSocket
{
public:
	CServerSocket(CServerConnectionManagement *pSCM);
	CServerSocket();


	virtual ~CServerSocket();
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

	int InitListeningSocket(CServerConnectionManagement * pSCM);
	void SetSCM(CServerConnectionManagement *pSCM)		{	m_pSCM = pSCM;	}
	(CServerConnectionManagement *) GetSCM(void)		{	return m_pSCM;	}

	void InitializeConnectionStats(ST_SERVERS_CLIENT_CONNECTION *pscc);
	void KillpClientConnectionStruct(void);

	//int SendPacket(BYTE *pB, int nBytes, int nDeleteFlag);

	void LockRcvPktList(void)		{ EnterCriticalSection(m_pSCC->pCSRcvPkt);	}
	void AddTailRcvPkt(void *pV)	{ m_pSCC->pRcvPktPacketList->AddTail(pV);	}
	void UnLockRcvPktList(void)		{ LeaveCriticalSection(m_pSCC->pCSRcvPkt);	}

	void LockSendPktList(void)		{ EnterCriticalSection(m_pSCC->pCSSendPkt);	}
	void AddTailSendPkt(void *pV)	{ m_pSCC->pSendPktPacketList->AddTail(pV);	}
	void UnLockSendPktList(void)	{ LeaveCriticalSection(m_pSCC->pCSSendPkt);	}

	//void KillMyThread(void)			{ m_pSCC->bExitThread = 1;					}

	UINT GetPacketsSent(void)		{ return m_pSCC->uPacketsSent;				}
	void SetClientIp4(CString s)	{ m_sClientIp4 = s;				}
	CString GetClientIp4(void)		{ return m_sClientIp4;			}


	// vairables
	CServerConnectionManagement *m_pSCM;		// ptr to the controlling class
	ST_SERVERS_CLIENT_CONNECTION *m_pSCC;		// ptr to my connection info/statistics/objects
	int m_nMyServer;							// which server are we connected stSCM[MAX_SERVERS]
	int m_nMyThreadIndex;						// which instance of pClientConnection[MAX_CLIENTS_PER_SERVER];
	CWinThread * m_pThread;						// ptr to thread which created the socket
	int m_nOwningThreadType;					// 0=Listener, 1=ServerConnection thread owns this socket class
	CString m_sClientIp4;						// IP4 address... 192.168.123.123 etc of server connected socket
	};

#endif

