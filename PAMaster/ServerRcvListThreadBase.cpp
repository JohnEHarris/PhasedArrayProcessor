/**
Author:		JEH
Date:		16-Nov-2012
Purpose:	Provide a thread to read data out of the RcvdLinkList that holds data from the connected client
			This is a base child class of CWinThread. It is here to provide the class member variables needed
			to access SCC and SCM. It has no specific received data processing functions. These must be created 
			in child classes derived from this one.

Revised:

**/


// ServerRcvListThreadBase.cpp : implementation file
//

#include "stdafx.h"

#ifdef THIS_IS_SERVICE_APP
#include "ServiceApp.h"
#include "../include/cfg100.h"
#else
#include "Truscan.h"
#include "TScanDlg.h"
#endif



// This thread reads the data that the ServerSocket has received via the OnReceive() and placed into 
// the socket's associated Receiver linked list. It sepearates the reception of data via TCP/IP from
// the processing of that data. It also allows us to set a priority of execution different from the
// thread which owns the socket feeding the data if we want/need a different priority

// This thread is created by the ServerSocketOwnerThread in its InitInstance() function

// CServerRcvListThreadBase

IMPLEMENT_DYNCREATE(CServerRcvListThreadBase, CWinThread)

CServerRcvListThreadBase::CServerRcvListThreadBase()
{
}

CServerRcvListThreadBase::~CServerRcvListThreadBase()
	{
	CString s;
	int nId = AfxGetThread()->m_nThreadID;
	s.Format(_T("~CServerRcvListThreadBase[%d][%d] = 0x%08x, Id=0x%04x has run\n"), m_nMyServer, m_nThreadIndex, this, nId);
	TRACE(s);
	}

// We have to over-ride the parents InitInstance since this thread is created by the parent in InitInstance()
// If we don't override, we will get an infinite loop of thread creation.
// This changed on 11-12-2012 when we made this a child of CWinThread
//
BOOL CServerRcvListThreadBase::InitInstance()
	{
	// TODO:  perform and per-thread initialization here
	CString s;
	//UINT uPort;
	
	// TODO:  perform and per-thread initialization here
#ifndef _AFXDLL
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();	// debug checking
	AfxSocketInit();
#endif
		{
		m_ConnectionSocket.Init();
		m_ConnectionSocket.m_pThread	= this;
		m_ConnectionSocket.m_pSCM		= this->m_pMySCM;	// ST_SERVER_CONNECTION_MANAGEMENT
		m_ConnectionSocket.m_nMyThreadIndex = this->m_nThreadIndex;
		m_ConnectionSocket.m_nMyServer	= this->m_nMyServer;
		m_ConnectionSocket.m_pstSCM		= this->m_pMySCM->m_pstSCM;
		m_ConnectionSocket.m_pSCC		= this->m_pSCC;		//m_pMySCM->m_pstSCM->pClientConnection[0]; // Server's client connection
		//m_ConnectionSocket.m_pSCC->pServerRcvListThread = this;
		m_ConnectionSocket.m_pSCC->pSocket = &m_ConnectionSocket;
		// m_hConnectionSocket = Asocket.Detach(); set when thread created suspended
		// attache via the handle to the temporary socket generated in the OnAccept() operation in CServerSocket
		// m_ConnectionSocket.Attach(m_hConnectionSocket, FD_READ | FD_CLOSE );
		//m_ConnectionSocket.GetPeerName(m_Ip4,m_uPort);	// connecting clients info??
		//m_ConnectionSocket.GetPeerName(Ip4,uPort);	// connecting clients info??
		//m_ConnectionSocket.SetClientIp4(m_Ip4);
		//m_ConnectionSocket.m_pSCC->sClientIP4 = m_Ip4;
		//m_ConnectionSocket.m_pSCC->uClientPort = m_uPort;
		m_ConnectionSocket.m_pSCC->szSocketName.Format(_T("ServerSocket Connection Skt[%d][%d]\n"),  m_nMyServer, m_nThreadIndex);
#ifdef _I_AM_PAG 
		m_ConnectionSocket.m_pSCC->sClientName.Format(_T("PAM[%d] on PAG Server[%d]\n"), m_nThreadIndex, m_nMyServer);
#else
#ifdef THIS_IS_SERVICE_APP
		m_ConnectionSocket.m_pSCC->sClientName.Format(_T("Instrument[%d] on Server[%d]\n"), m_nThreadIndex, m_nMyServer);
#endif

#endif
		m_ConnectionSocket.m_nOwningThreadType = eServerConnection;

#if 0
		s.Format(_T("Instrument Client[%d] accepted to server on socket %s : %d\n"), m_nThreadIndex, Ip4, uPort);
		TRACE(s);
		TRACE(m_ConnectionSocket.m_pSCC->szSocketName);
#endif
		}

	return TRUE;
}

int CServerRcvListThreadBase::ExitInstance()
	{
	// TODO:  perform any per-thread cleanup here
	// return CServerSocketOwnerThread::ExitInstance();
	CString s;
	s.Format(_T("CServerRcvListThreadBase, Srv[%d]Instrument[%d] has exited\n"),m_nMyServer, m_nThreadIndex);
	TRACE(s);
	if (m_pSCC->pServerRcvListThread)
		{
		//delete m_pSCC->pServerRcvListThread;
		m_pSCC->pServerRcvListThread= NULL;
		}
	return CWinThread::ExitInstance();
	}

//BEGIN_MESSAGE_MAP(CServerRcvListThreadBase, CServerSocketOwnerThread)	// changed (xx,CWin) to (xx,CServerSocketOwnerThread)
BEGIN_MESSAGE_MAP(CServerRcvListThreadBase, CWinThread)

END_MESSAGE_MAP()
