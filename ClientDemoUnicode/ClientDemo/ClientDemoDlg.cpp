// CommDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo.h"
#include "ClientDemoDlg.h"
#include ".\clientdemodlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCommDemoDlg dialog

CCommDemoDlg::CCommDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommDemoDlg::IDD, pParent)
	, m_sIPAddr(_T(""))
	, m_sUniString(_T(""))
	, m_iMode(NO_CONNECT)
	, m_iSeq( 0 )
	, m_sMsg2WO(_T(""))
	, m_sMsg2Lot(_T(""))
	, m_sMsg3Lot(_T(""))
	, m_iPipeNum(0)
	, m_iClass(0)
	, m_bWatchdogReceived(false)
	, m_iCounter(0)
	, m_bSendWatchdog(TRUE)
	, m_iUnitID(1)
	, m_bUnit1(FALSE)
	, m_iInspSpeed(0)
	, m_fLineSpeed(0)
	, m_iRpm(0)
	, m_fAmaMag(0)
	, m_fSonoMag1(0)
	, m_fSonoMag2(0)
	, m_fSonoMag3(0)
	, m_fSonoMag4(0)
	, m_fQuesTholdID(0)
	, m_fQuesTholdOD(0)
	, m_fRejTholdID(0)
	, m_fRejTholdOD(0)
	, m_pSocket(0)	//jeh
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCommDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RECEIVE, m_listReceive);
	DDX_Text(pDX, IDC_EDIT_IP, m_sIPAddr);
	DDX_Text(pDX, IDC_EDIT_TCHAR, m_sUniString);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_cbConnect);
	DDX_Text(pDX, IDC_EDIT_WO2, m_sMsg2WO);
	DDX_Text(pDX, IDC_EDIT_LOT2, m_sMsg2Lot);
	DDX_Text(pDX, IDC_EDIT_LOT3, m_sMsg3Lot);
	DDX_Check(pDX, IDC_CHECK_WATCHDOG, m_bSendWatchdog);
	DDX_Radio(pDX, IDC_RADIO_UNIT1, m_bUnit1);
	DDX_Text(pDX, IDC_EDIT_LINE_SPEED, m_iInspSpeed);
	DDX_Text(pDX, IDC_EDIT_LINE_SPEED_MSG4, m_fLineSpeed);
	DDV_MinMaxFloat(pDX, m_fLineSpeed, 0, 10000);
	DDX_Text(pDX, IDC_EDIT_RPM, m_iRpm);
	DDX_Text(pDX, IDC_EDIT_PIPE_NUM, m_iPipeNum);
	DDX_Text(pDX, IDC_EDIT_CLASS, m_iClass);
	DDV_MinMaxInt(pDX, m_iRpm, 0, 1000);
	DDX_Text(pDX, IDC_EDIT_AMA_MAG, m_fAmaMag);
	DDX_Text(pDX, IDC_EDIT_SONO_MAG1, m_fSonoMag1);
	DDX_Text(pDX, IDC_EDIT_SONO_MAG2, m_fSonoMag2);
	DDX_Text(pDX, IDC_EDIT_SONO_MAG3, m_fSonoMag3);
	DDX_Text(pDX, IDC_EDIT_SONO_MAG4, m_fSonoMag4);
	DDX_Text(pDX, IDC_EDIT_QUES_ID, m_fQuesTholdID);
	DDX_Text(pDX, IDC_EDIT_QUES_OD, m_fQuesTholdOD);
	DDX_Text(pDX, IDC_EDIT_REJ_ID, m_fRejTholdID);
	DDX_Text(pDX, IDC_EDIT_REJ_OD, m_fRejTholdOD);
}

BEGIN_MESSAGE_MAP(CCommDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_SEND1, &CCommDemoDlg::OnBnClickedButtonSend1)
	ON_BN_CLICKED(IDC_BUTTON_SEND3, &CCommDemoDlg::OnBnClickedButtonSend3)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RADIO_UNIT1, &CCommDemoDlg::OnBnClickedRadioUnit1)
	ON_BN_CLICKED(IDC_RADIO_UNIT2, &CCommDemoDlg::OnBnClickedRadioUnit2)
	ON_BN_CLICKED(IDC_CHECK_WATCHDOG, &CCommDemoDlg::OnBnClickedCheckWatchdog)
END_MESSAGE_MAP()


// CCommDemoDlg message handlers

BOOL CCommDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_listReceive.InsertColumn( 1, _T("No."), LVCFMT_LEFT, 50 );
	m_listReceive.InsertColumn( 2, _T("Messages"), LVCFMT_LEFT, 500 );
	m_listReceiveDeleteCount = 0;

	m_sIPAddr = DEFAULT_IP_ADDR;

	SetTimer(TIMER_10_SEC, 3000, NULL);
	SetTimer(TIMER_11_SEC, 11000, NULL);

	UpdateData( FALSE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCommDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCommDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCommDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCommDemoDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	KillTimer( TIMER_10_SEC );
	KillTimer( TIMER_11_SEC );

	CDialog::OnClose();
	CDialog::OnCancel();
}

void CCommDemoDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	return;

	CDialog::OnOK();
}

void CCommDemoDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	CDialog::OnCancel();
}

void CCommDemoDlg::AddReceive(CString s)
{
	int count;
	CString str,s1;

	count = m_listReceive.GetItemCount();
	str.Format( _T("%d."), count+1+m_listReceiveDeleteCount );
	m_listReceive.InsertItem( count, str );

	m_listReceive.SetItemText( count, 1, s );
	if( count >= 8000 )
	{
		m_listReceive.DeleteItem( 0 );
		m_listReceiveDeleteCount++;
		if( m_listReceiveDeleteCount >= 0 ) m_listReceiveDeleteCount = -8000;
	}
	m_listReceive.EnsureVisible( count, false );
}

void CCommDemoDlg::OnBnClickedButtonConnect()
{
	// TODO: Add your control notification handler code here
	UpdateData();	// make sure the IP address is in the memory.

	if ( NULL == m_pSocket)
		{
		CClientSocket * pSocket = new CClientSocket;
		m_pSocket = pSocket;
		pSocket->Create(0, SOCK_STREAM, _T("192.168.10.203") );

		if( !pSocket->Connect( m_sIPAddr, SERVER_LISTEN_PORT) )
		{
			MessageBox( _T("Failed to connect.") );
			SetDlgItemText( IDC_STATIC_STATUS, _T("  SERVER  ") );
			return;
		}
		m_iMode = CLIENT;
		// disable the connect button
		//SetConnectButton( DISABLE );
		AddReceive( _T("Connected to a server.") );
		m_cbConnect.SetWindowTextW(_T("Disconnect"));
		}
	else
		{
		m_pSocket->Close();
		m_pSocket = NULL;
		AddReceive( _T("disConnected FROM server.") );
		m_cbConnect.SetWindowTextW(_T("Connect"));
		}
	SetDlgItemText( IDC_STATIC_STATUS, _T("  SERVER  ") );
}

void CCommDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	DUMMY msg;

	if( nIDEvent == TIMER_10_SEC )
	{
		if( m_pSocket != NULL )
		{
			// send watchdog
			msg.MessageLength = 203;
			m_pSocket->Send( &msg, sizeof(DUMMY), 0 );
		}
		CDialog::OnTimer(nIDEvent);
		return;
	}

	if( nIDEvent == TIMER_11_SEC )
	{
		if( m_bWatchdogReceived )
		{
			// to trigger the background color change
			SetDlgItemText( IDC_STATIC_STATUS, _T("  SERVER  ") );
			m_bWatchdogReceived = false;
		}
		else
		{
			// if no ping or other messages received in 1.1 seconds, show connection status to red
			// to trigger the background color change
			SetDlgItemText( IDC_STATIC_STATUS, _T("  SERVER  ") );
		}

		CDialog::OnTimer(nIDEvent);
		return;
	}

	CDialog::OnTimer(nIDEvent);
}
void CCommDemoDlg::SetConnectButton( int state)
{
	switch( state )
	{
	case DISABLE:
		m_cbConnect.EnableWindow( 0 );
		break;
	case ENABLE: 
		m_cbConnect.EnableWindow( 1 );
		break;
	}
}

void CCommDemoDlg::OnBnClickedButtonSend1()
{
	// send message 1: request job info
	// check if this is a server is connected.
	if( m_iMode == NO_CONNECT )
	{
		MessageBox( _T("Fail to send. Please setup a connection first.") );
		return;
	}
	if (m_pSocket == NULL)
		{
		MessageBox( _T("m_pSocket is null") );
		return;
		}

	BYTE * pChar;
	int i;
	
	UpdateData();
	TCPTOPT msg;

	pChar = (BYTE *)&msg;
	for( i=0; i<1024; i++ )
		*(pChar+i) = i%256;

	msg.spare[0] = 101;
	msg.spare[1] = 102; // 8 bytes
	msg.spare[2] = 103;	
	msg.spare[256] = 201;
	msg.spare[257] = 202; // 8 bytes
	msg.spare[258] = 203;	

	m_pSocket->Send( pChar, 4, 0 );
	AddReceive( _T("Sent 4 bytes. go into sleep for 5s") );
	Sleep( 3000 );
	m_pSocket->Send( pChar+4, 512, 0 );
	AddReceive( _T("Sent another 512 bytes.") );
	Sleep( 3000 );
	m_pSocket->Send( pChar+516, 508, 0 );
	AddReceive( _T("Sent another 508 bytes.") );
	//m_pSocket->Send( &msg, sizeof(REQ_JOB_INFO), 0 );
}

void CCommDemoDlg::OnBnClickedButtonSend()
{
	// send message 2: inspection results
	// check if this is a server is connected.
	if( m_iMode == NO_CONNECT )
	{
		MessageBox( _T("Fail to send. Please setup a connection first.") );
		return;
	}
		
	if (m_pSocket == NULL)
		{
		MessageBox( _T("m_pSocket is null") );
		return;
		}

	UpdateData();	// make sure the IP address is in the memory.

	DUMMY msg;

	msg.MessageLength = 228;
	msg.MessageID = 314902;
/*	msg.UnitID = m_iUnitID;
	msg.Sequence = m_iSeq++;

	_stprintf_s( msg.JobID, _T("%s"), m_sMsg2WO );
	_stprintf_s( msg.Lot, _T("%s"), m_sMsg2Lot );
	msg.PipeNum = m_iPipeNum;
	msg.Classification = m_iClass;
	msg.InspectionSpeed = m_iInspSpeed;
*/
	m_pSocket->Send( &msg, sizeof(DUMMY), 0 );
	AddReceive( _T("Sent 1040 bytes.") );
	//m_sUniString = _T("");
	//UpdateData( FALSE );
}

void CCommDemoDlg::OnBnClickedButtonSend3()
{
	// send message 3: practical job parameters
	// check if this is a server is connected.
	if( m_iMode == NO_CONNECT )
	{
		MessageBox( _T("Fail to send. Please setup a connection first.") );
		return;
	}

	if (m_pSocket == NULL)
		{
		MessageBox( _T("m_pSocket is null") );
		return;
		}

	UpdateData();

	INSP_PARAMETER msg;

	msg.MessageLength = 160;
	msg.MessageID = 314903;
	msg.UnitID = m_iUnitID;
	msg.Sequence = m_iSeq++;

	_stprintf_s( msg.Lot, _T("%s"), m_sMsg3Lot );

	msg.LineSpeed = m_fLineSpeed;
	msg.rpm = m_iRpm;
	msg.AmaMag = m_fAmaMag;
	msg.SonoMag1 = m_fSonoMag1;
	msg.SonoMag2 = m_fSonoMag2;
	msg.SonoMag3 = m_fSonoMag3;
	msg.SonoDeMag = m_fSonoMag4;
	msg.QuesThold[0] = m_fQuesTholdID;
	msg.QuesThold[1] = m_fQuesTholdOD;
	msg.RejThold[0] = m_fRejTholdID;
	msg.RejThold[1] = m_fRejTholdOD;

	m_pSocket->Send( &msg, sizeof(INSP_PARAMETER), 0 );
}

HBRUSH CCommDemoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// if it is the connection status static control
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_STATUS)
	{
		// Set the text color to black
		pDC->SetTextColor(RGB(0, 0, 0));

		//if( m_bWatchdogReceived )
		if( m_pSocket )
			{
			// Set the background color to green
			pDC->SetBkColor( RGB(0, 255, 0) );
			}
		else
			{
			// Set the background color to red
			pDC->SetBkColor( RGB(255, 0, 0) );
			}
	}
	return hbr;
}


void CCommDemoDlg::OnBnClickedRadioUnit1()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_iUnitID = 1;
}


void CCommDemoDlg::OnBnClickedRadioUnit2()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_iUnitID = 2;
}


void CCommDemoDlg::OnBnClickedCheckWatchdog()
{
	// TODO: Add your control notification handler code here
	UpdateData();
}
