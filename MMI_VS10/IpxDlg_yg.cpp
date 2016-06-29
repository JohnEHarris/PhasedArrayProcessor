// IpxDlg.cpp : implementation file
#if 0
01/19/01	Let the ipx thread class CIpxIn start this hidden dialog
			the same way the tscanapp starts tscandlg.  Provides windows
			message pump to handle ipx messages at higher priority than
			the mmi window TscanDlg.
			Must start as a modeless dialog to allow other code to run

#endif
//


#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

//#include "IpxDlg.h"	included in tscandlg.h

#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCriticalSection g_CriticalSection;  /* Critical section for mutual exclusion access of plistUtData */

/////////////////////////////////////////////////////////////////////////////
// CIpxDlg dialog


CIpxDlg::CIpxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIpxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIpxDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


CIpxDlg::~CIpxDlg()
	{
	// jeh 03/30/01 make sure no linked lists left over
	I_MSG_CAL	*pImsg02;

	while (!m_plistTemp.IsEmpty())
		{
		pImsg02 = (I_MSG_CAL *)m_plistTemp.RemoveHead();
		delete pImsg02;
		}

	while (!plistIpxIn.IsEmpty())
		{
		pImsg02 = (I_MSG_CAL *)plistIpxIn.RemoveHead();
		delete pImsg02;
		}
	}

void CIpxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIpxDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIpxDlg, CDialog)
	//{{AFX_MSG_MAP(CIpxDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP

	//Manually add message to kill window  .. jeh
	ON_MESSAGE(WM_USER_KILL_IPX_THREAD, OnCancel)
	ON_MESSAGE(WM_IPX_RXRDY, OnIpxRxRdy)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIpxDlg message handlers

void CIpxDlg::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class

	::PostQuitMessage(0);	// kill CWinThread app IpxIn
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}


void CIpxDlg::KillMe() 
	{
	// Public access to OnCancel
	OnCancel();
	}



void CIpxDlg::OnCancel() 
	{
	// TODO: Add extra cleanup here
	// Need this to get to PostNcDestroy which nulls ptr to the routine
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CIpxDlg::OnOK() 
	{
	// TODO: Add extra validation here
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

BOOL CIpxDlg::OnInitDialog() 
	{
#ifdef TWO_CHANNEL_4_GATE_BOARD
	return (TRUE);
#endif

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
#if 0
	m_uIpxThreadTimer = SetTimer(IDT_IPX_THREAD_TIMER, 1000, NULL);
	if (!m_uIpxThreadTimer) AfxMessageBox("Failed to start IPX timer");
//	else AfxMessageBox("IPX timer started");
#endif
	hIpxDlg = m_hWnd;	// copy this window handle to global variable

	InitIPX();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CIpxDlg::OnTimer(UINT nIDEvent) 
	{
	// TODO: Add your message handler code here and/or call default
//	nLoop++;
	int nPriT = GetThreadPriority(GetCurrentThread());
	int nPriC = GetPriorityClass(GetCurrentProcess());
	nLoop = nPriC + nPriT;

	CDialog::OnTimer(nIDEvent);
	}



// Encapsulate all IPX initialization into the following module
// jeh 1-12-00
// Moved from TscanDlg.cpp to here on 1/19/2001

BOOL CIpxDlg::InitIPX()
	{	// on success, return 1, failure return 0
	// 05/08/00 try setting up tcpip as well
	int rv;
	WSAPROTOCOL_INFO strProtoInfo[MAX_PROTOCOLS];
	ULONG nProtoInfo;
	int optval;
	int nSockCmndLen;
	int nSockDataLen;
	int nIpx;		// protocol index of each protocol
	int i;

	CString s,t;
	// copied in toto from wellchekdlg
		

	ListShare.ipx = ListShare.mmi = 0;	// init sharing

//  Open a socket connection to the instrument.  Must have Winsock v2.0 or
//  higher for IPX support.

	nProtoInfo = sizeof(strProtoInfo);
//  strProtoInfo[0].dwMessageSize = 1500;  no effect, still 0x240
	rv = WSAEnumProtocols(NULL, strProtoInfo, &nProtoInfo);
//	rv = WSCEnumProtocols(protoList, strProtoInfo, &nProtoInfo, &ErrorNum);
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSockNA("EnumProtocol ");
		return 0;
		}

	nIpx = 1000;		// out of range number

	//FIND 1st protocol of each type
	for ( i = 0; i < rv; i++)
		{
		if (strProtoInfo[i].iProtocol == NSPROTO_IPX)
			{
			nIpx = i;
			break;
			}
		}

	
	if ( nIpx  > MAX_PROTOCOLS ) MessageBox("Did not find IPX protocol");

//  Returned dwServiceFlags1 was:
//    XP1_CONNECTIONLESS     | XP1_MESSAGE_ORIENTED | XP1_SUPPORT_BROADCAST
//  | XP1_SUPPORT_MULTIPOINT | XP1_IFS_HANDLES
#if 0
	t.Format("\r\nProto %08X", strProtoInfo[0].dwServiceFlags1);
	s += t;
//    m_ceStdOut.SetWindowText(s);

	/**** THIS FOR ALSO WORKS OK FOR TRUSCOPE 2  5-17-02 ****/
	//create a socket bound to a service provider transport
	sktIpxCmnd = WSASocket( FROM_PROTOCOL_INFO , FROM_PROTOCOL_INFO , FROM_PROTOCOL_INFO ,  //  Family, type, protocol
								&strProtoInfo[nIpx], 0,              //  Protocol info, group
								0                                 //  Flags
//      WSA_FLAG_OVERLAPPED
//    | WSA_FLAG_MULTIPOINT_C_LEAF 
//    | WSA_FLAG_MULTIPOINT_D_LEAF
    );
#endif

	sktIpxCmnd = 0;
	//create a socket bound to a service provider transport
	sktIpxCmnd = WSASocket( AF_IPX, SOCK_DGRAM, NSPROTO_IPX+4,  //  Family, type, protocol
								&strProtoInfo[nIpx], 0,              //  Protocol info, group
								0                                 //  Flags
//      WSA_FLAG_OVERLAPPED
//    | WSA_FLAG_MULTIPOINT_C_LEAF 
//    | WSA_FLAG_MULTIPOINT_D_LEAF
    );
	if (sktIpxCmnd == INVALID_SOCKET)
		{ 
		ErrMsgSock("Bind ");
		ErrMsgSockNA("Socket");
		//  Need sock_na flag here
		return 0;
		}

//  Instrument will send broadcast messages so he does not need to know this
//  IP address or Ethernet adapter ID, only the socket number.  We will get
//  his address when he transmits.  

	
	optval = TRUE;
	rv = setsockopt( sktIpxCmnd, SOL_SOCKET, SO_BROADCAST,
					(char*)&optval, sizeof(BOOL) );
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Setopt ");
		return 0;
		}



//  Bind the cmnd  socket  to a local address
	memset(&SockCmndAddr, 0, sizeof(SockCmndAddr));
	SockCmndAddr.sa_family = AF_IPX;

//  sa_netnum is the network address (in network order) that was specified
//  for IPX/SPX compatible protocol advanced properties network address in
//  network setup.  It will be filled in by the driver.

//  SockCmndAddr.sa_nodenum[6] is the unique ID of the Ethernet adapter.
//  It will be filled in by the driver.

//  sa_socket is the port number for this application.
  // this is the IPX 'socket number'
#if 1
	SockCmndAddr.sa_socket = TscanCOMMAND;
	rv = bind(sktIpxCmnd, (SOCKADDR*)&SockCmndAddr, sizeof(SockCmndAddr));
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Bind ");
		// Already running 1 copy of program
		MessageBox("Tscan is already running.  Close this copy and Maximize the copy already Running");
		return 0;
		} 
#endif

//  Fill the address structure with the bound values
	nSockCmndLen = sizeof(SockCmndAddr);
	rv = getsockname(sktIpxCmnd, (struct sockaddr*)&SockCmndAddr, &nSockCmndLen);
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Name ");
		return 0;
		}

  	memset (&SockCmndAddr.sa_nodenum[0],0xff,6);





//  Bind the data  socket  to a local address
	sktIpxData = WSASocket( AF_IPX, SOCK_DGRAM, NSPROTO_IPX+4,  //  Family, type, protocol
								&strProtoInfo[nIpx], 0,              //  Protocol info, group
								0                                 //  Flags
//      WSA_FLAG_OVERLAPPED
//    | WSA_FLAG_MULTIPOINT_C_LEAF 
//    | WSA_FLAG_MULTIPOINT_D_LEAF
							);
	if (sktIpxData == INVALID_SOCKET)
		{ 
		ErrMsgSockNA("Socket");
//		Need sock_na flag here
		return 0;
		}

//  Instrument will send broadcast messages so he does not need to know this
//  IP address or Ethernet adapter ID, only the socket number.  We will get
//  his address when he transmits.  
	optval = TRUE;
	rv = setsockopt( sktIpxData, SOL_SOCKET, SO_BROADCAST, 
					(char*)&optval, sizeof(BOOL) );
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Setopt ");
		return 0;
		}

	memset(&SockDataAddr, 0, sizeof(SockDataAddr));
	SockDataAddr.sa_family = AF_IPX;

//  sa_netnum is the network address (in network order) that was specified
//  for IPX/SPX compatible protocol advanced properties network address in
//  network setup.  It will be filled in by the driver.

//  SockDataAddr.sa_nodenum[6] is the unique ID of the Ethernet adapter.
//  It will be filled in by the driver.

//  sa_socket is the port number for this application.
  // this is the IPX 'socket number'
#if 1
	SockDataAddr.sa_socket = TscanDATA;
	rv = bind(sktIpxData, (SOCKADDR*)&SockDataAddr, sizeof(SockDataAddr));
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Bind ");
		return 0;
		} 
#endif

//  Fill the address structure with the bound values
	nSockDataLen = sizeof(SockDataAddr);
	rv = getsockname(sktIpxData, (struct sockaddr*)&SockDataAddr, &nSockDataLen);
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("Name ");
		return 0;
		}

//  This sets the socket to non-blocking mode and sends the WM_IPX_RXRDY msg to
//  this window when IPX data is available.  MESSAGE_MAP is configured to
//  call OnIpxRxRdy() when the msg is received.

#if 0
	// for debug, send wm msg to tscandlg window
	if (0)	//pCTscanDlg)
		rv = WSAAsyncSelect( sktIpxData, pCTscanDlg->m_hWnd, 
							WM_IPX_RXRDY, FD_READ );  //  winuser.h

	else
#endif

		rv = WSAAsyncSelect( sktIpxData, m_hWnd, WM_IPX_RXRDY, FD_READ );  //  winuser.h
//		   ( sktIpxData, m_hWnd, WM_IPX_RXRDY, FD_READ | FD_WRITE );  //  winuser.h
    
	
	if (rv == SOCKET_ERROR)
		{ 
		ErrMsgSock("AsyncSelect ");
 //		return 0;
		}

 
	return 1;

	}

void CIpxDlg::ErrMsgSockNA(char *t)
	{
	// Copy from TscanDlg.cpp
	CString s;

	s  = t;
	s += GetWSAError();
	s += "\nEthernet Comm Not Available";
	AfxMessageBox(s);

	}

void CIpxDlg::ErrMsgSock(char *t)
	{
	// Copy from TscanDlg.cpp
	CString s;

	s  = t;
	s += GetWSAError();
	AfxMessageBox(s);
	}

void CIpxDlg::ErrMsgSockStartup(int errval)
	{
	// Copy from TscanDlg.cpp
	CString s = "Startup ";

	s += GetWSAErrStr(errval);
	s += "\nEthernet Comm Not Available";
	AfxMessageBox(s);
	}

BOOL CIpxDlg::GrantList()
	{	// If no other use of list, grant to caller
		// else withdraw request and return false
		// Use global structure ListShare
	ListShare.ipx = 1;
	if (ListShare.mmi == 0) return TRUE;	//ipx thread has list

	else
		{
		ListShare.ipx = 0;	// withdraw request
		return FALSE;	// IPX thread must not use
		}

	}

// Copy from TscanDlg.cpp
LRESULT CIpxDlg::OnIpxRxRdy(WPARAM skt, LPARAM err_event)
	{

	// Winsock2 notifies us that we have received data from the Tscan instrument
#ifdef TWO_CHANNEL_4_GATE_BOARD
	return (TRUE);
#endif

	static int nmsg = 0, nmtx = 0;
	static WORD wLastSeq = 0;
//	USHORT machineID;

	int errval, event;
	WSABUF WsaBuf[1];
	int rv;
	ULONG nrx, flagrx;           //  bytes sent, flags
//	ULONG ntx, flagtx;
	SOCKADDR_IPX strAddrThere;
	int nAddrThere;

	I_MSG_CAL	*pImsg02;			// msg02 is much bigger than msg 01.

#if 1
	// pre debug cdp operation with udp
	CDPMSG2EX *pCdpMsg;
	int i;
#endif


	typedef struct
		{
		RECBUF	rb;
		BYTE	pad[20];
		}	BIGBUF;

	CString s;

	event = sizeof(BIGBUF);	/* DEBUG */

	errval = err_event >> 16;
	if (errval)
		{ 
		s = "OnRx ";
		s += GetWSAErrStr(errval);
		AfxMessageBox(s);
		return 1;
		}

	event = err_event & 0x00FF;
	if (event & FD_READ) //while (1)
		{	// (event & FD_READ) while (1)
		WsaBuf[0].buf = (char*)(new BIGBUF);	// Select biggest possible I msg
		if (WsaBuf[0].buf == NULL)
			{ 
			AfxMessageBox("OOM");	//out of memory
			return 1;
			}

		WsaBuf[0].len = sizeof(BIGBUF);
		flagrx = 0;
		nAddrThere = sizeof(strAddrThere);
		rv = WSARecvFrom
				( sktIpxData, WsaBuf, 1,   //  Socket, buffer array, buffer count
				&nrx, &flagrx,             //  Bytes received, flags
				(SOCKADDR*)&strAddrThere,  //  Sender return addr
				&nAddrThere,
				NULL,                      //  Overlap struct
				NULL                       //  Callback when would block
				);

		
		if (rv == SOCKET_ERROR)
			{ 
			rv = WSAGetLastError();
			if (rv != WSAEWOULDBLOCK)
				{ 
				s = "Rx ";
				s += GetWSAErrStr(rv);
				AfxMessageBox(s);
				}
			delete WsaBuf[0].buf;
			return 0;
			}   //  End if (RecvFrom() == SOCKET_ERROR)
	
		else  //  (RecvFrom() != SOCKET_ERROR)
			{
			// pre debug cdp operation

#if 1
			pCdpMsg = (CDPMSG2EX *) WsaBuf[0].buf;
			if ( pCdpMsg->id == 2)
				{	//got config msg
				i = nrx;	// place to break
				delete WsaBuf[0].buf;
				return 0;
				}
#endif


			pImsg02 = (I_MSG_CAL*) WsaBuf[0].buf;
			IpxStat.TotalRcvPackets++;
			if ( wLastSeq == pImsg02->MstrHdr.MsgNum) IpxStat.DupRcvPackets++;
			else if (++wLastSeq != pImsg02->MstrHdr.MsgNum) IpxStat.LostRcvPackets++;
			wLastSeq = pImsg02->MstrHdr.MsgNum;

			// Capture Udp status info
			gwUdpStatus[0] = pImsg02->InspHdr.status[0];
			gwUdpStatus[1] = pImsg02->InspHdr.status[1];

			// Capture period of rotation for Truscope, velocity measurement
			gnTruscopePeriod	= pImsg02->InspHdr.Period;
			gnVelocityDt		= pImsg02->InspHdr.VelocityDt;

			// Get comm statistics from instrument header
//			IpxStat.TotalSentPackets = pImsg02->MstrHdr.IdataStat.nSent;
			IpxStat.LostSentPackets = pImsg02->MstrHdr.IdataStat.nLost;
			IpxStat.DupSentPackets = pImsg02->MstrHdr.IdataStat.nDup;

			//Get data to Web if requested
			if(pWebMemPtr->uStatus == 0)
				{
				pCTscanDlg->GetIpxStats((NET_STATS *)&pWebMemPtr->IpxStat);
				pWebMemPtr->uStatus = 1;
				}
		

#if 0
			MOST UNHAPPILY, WIN2K DOES NOT SEND THE MESSAGES WHEN THE SPECIFIC
			MAC ADDRESS IS SET.  THIS IS OBVIOUS BY WATCHING THE HUB AND NOTING THAT
			ONCE THE MAC ADDRESS OF THE MASTER UDP IS SET, SENDING THE CONFIG
			FILE NO LONGER LIGHTS THE ACTIVITY LIGHTS ON THE HUB.
			BAD !!!!!



			if (pImsg02->MstrHdr.MsgId   == RUN_MODE)
				memcpy(&SockCmndAddr.sa_nodenum,
					&strAddrThere.sa_nodenum, 6);
#endif

			// If no one available to empty ipx queue, discard packet
			// Is this Kosher to call a function in another thread
			if (!pCTscanDlg->IsDataClient() )
				{
				delete WsaBuf[0].buf;
				return 0;
				}

#if 1

			if (GrantList())
				{	// IpxIn List is free.. use it
				// first empty temp list to IpxIn list
				while (!m_plistTemp.IsEmpty())
					{
					pImsg02 = (I_MSG_CAL *)m_plistTemp.RemoveHead();
					plistIpxIn.AddTail( (void *) pImsg02);
					}

				// Now add new packet from os ipx handler
				plistIpxIn.AddTail( (void *) WsaBuf[0].buf);
				ListShare.ipx = 0;		// release list
				}

			else
				{	// divert data to local temp list
				m_plistTemp.AddTail( (void *) WsaBuf[0].buf);
				}

			::PostMessage(pCTscanDlg->m_hWnd, WM_USER_GET_IPX_LIST,0,0);
#else
// Try clive's method, bypass copy from ipxin to utdata list
// Sometimes results in gap in data on screen

			m_plistTemp.AddTail( (void *) WsaBuf[0].buf);
			if ( plistUtData.IsEmpty() )
				{
				while ( !m_plistTemp.IsEmpty())
					{
					pImsg02 = (I_MSG_CAL *)m_plistTemp.RemoveHead();
					plistUtData.AddTail( (void *) pImsg02);
					}
				}
#endif

			return 1;

			}   //  End if (RecvFrom() != SOCKET_ERROR)
		}   //  End if (event & FD_READ) while (1)

	return 0; 

	}   //  End OnIpxRxRdy()  07-28-99

