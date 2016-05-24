// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo.h"
#include "ClientDemoDlg.h"
#include "ClientSocket.h"
#include ".\clientsocket.h"

extern CCommDemoApp theApp;

// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket member functions

void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	int n;
	CCommDemoDlg * pMainDlg;
	pMainDlg = (CCommDemoDlg *)theApp.m_pMainWnd;

	DUMMY * pRecvBuf;
	CString s;
	JOB_INFO * pJobInfo;

	pRecvBuf = new DUMMY;

	n = Receive( (void *)pRecvBuf, sizeof(DUMMY), 0);
	s.Format( _T("Received ID = %d"), pRecvBuf->MessageLength );
	pMainDlg->AddReceive( s );

#if 0
	if( n != pRecvBuf->MessageLength )
	{
		s.Format( _T("Message %d size error"), pRecvBuf->MessageID );
		pMainDlg->MessageBox( s );
		delete pRecvBuf;
		CSocket::OnReceive(nErrorCode);
		return;
	}

	switch( pRecvBuf->MessageID )
	{
	case 493100:
		pMainDlg->m_bWatchdogReceived = true;
		s.Format( _T("------ seq=%d, id=%d, length=%d Server Watchdog ------------"),
			pRecvBuf->Sequence, pRecvBuf->MessageID, pRecvBuf->MessageLength );
		pMainDlg->AddReceive( s );
		break;
	case 493101:
		s.Format( _T("------ seq=%d, id=%d, length=%d Job Information ------------"),
			pRecvBuf->Sequence, pRecvBuf->MessageID, pRecvBuf->MessageLength );
		pMainDlg->AddReceive( s );

		pJobInfo = (JOB_INFO *)pRecvBuf;

		s.Format( _T("JobID= \"%s\""), pJobInfo->JobID );
		pMainDlg->AddReceive( s );
		s.Format( _T("Lot= \"%s\""), pJobInfo->Lot );
		pMainDlg->AddReceive( s );
		s.Format( _T("Heat= \"%s\""), pJobInfo->Heat );
		pMainDlg->AddReceive( s );
		s.Format( _T("OD= %7.3f"), pJobInfo->OD );
		pMainDlg->AddReceive( s );
		s.Format( _T("Wall= %7.3f"), pJobInfo->Wall );
		pMainDlg->AddReceive( s );
		s.Format( _T("Length= %7.3f"), pJobInfo->Length );
		pMainDlg->AddReceive( s );
		break;
	default:
		s.Format( _T("Unrecognize message ID: %d"), pRecvBuf->MessageID ),
		pMainDlg->MessageBox( s );
		break;
	}
#endif
	delete pRecvBuf;

	CSocket::OnReceive(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class
	// enable the connect button
	((CCommDemoDlg *)theApp.m_pMainWnd)->SetConnectButton( ENABLE );
	((CCommDemoDlg *)theApp.m_pMainWnd)->AddReceive( _T("Server connection closed.") );
	((CCommDemoDlg *)theApp.m_pMainWnd)->m_iMode = NO_CONNECT;

	CSocket::OnClose(nErrorCode);
}
