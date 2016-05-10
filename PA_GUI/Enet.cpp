// Enet.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
//#include "Catch.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnet dialog


CEnet::CEnet(CWnd* pParent /*=NULL*/)
	: CDialog(CEnet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnet)
	//}}AFX_DATA_INIT
}


void CEnet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnet)
	DDX_Control(pDX, IDC_EDreport, m_report);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnet, CDialog)
	//{{AFX_MSG_MAP(CEnet)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDUpdate, OnUpdate)
	ON_BN_CLICKED(IDReset, OnReset)
	ON_BN_CLICKED(IDRestore, OnRestore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/* The B431_ETHER_STATS message contains ethernet statistics returned by the
   driver. The statistics are contained in the following structure.
*/
#if 0
typedef struct ether_stats {
	unsigned long	tx_packets;
	unsigned long	rx_packets;
	unsigned long	framing_errors;
	unsigned long	crc_errors;
	unsigned long	packets_dropped;
	unsigned long	packets_missed;
	unsigned long	deferred_transmits;
	unsigned long	late_collisions;
	unsigned long	carrier_lost;
	unsigned long	no_more_retries;
	unsigned long	single_retries;
	unsigned long	multiple_retries;
	unsigned long	average_tdr_value;
} ETHER_STATS;
struct {
		unsigned long edata[13];
		} ether_stats[5], estats_offset[5];

#endif

typedef struct ether_stats {
	int nSent;		/* Number sent of this type packet */
	int nRcv;		/* Number received */
	WORD wLast;		/* msg cnt of last received msg */
	WORD nDup;		/* number of duplicate messages */
	int nLost;
	int nReadSum;	/* total bytes read */

} ETHER_STATS;

struct {
		int edata[6];
		} ether_stats[10], estats_offset[10];


//BOARDPACKET board_packet, board_packet_old;	// where catch data is stored from udp

int		hTimer4;	// not really a handle
int msgposted = 0;		// timer incr when posting, UPDATE_BN decr when servicing

int packet_format = 0;	// New packet format
/************************************************************************/


void collect_enet_stats ();

void collect_enet_stats ()
	{
	int i;
	
#if 1
	// For testing, generate test data
	int j;
	unsigned long k;
	k = 1000;
	for ( j = 0; j < 10; j++)
		for ( i = 0; i < 6; i++)
		{
			ether_stats[j].edata[i] =i+ k++;
			estats_offset[j].edata[i] =  k - 2*i;
		}
#endif
/*
	UdpMem[0] = 4;		// length of reply from udp
	UdpMem[1] = (char)0xf0;	// udp msg instead of inst msg
	UdpMem[2] = 12;		// cmnd Master to gather stats
/*
	i = WriteLink(1,(char *) &UdpMem,8,10000);
	i = ReadLink(1,(char *) &UdpMem,UdpMem[0],50000);

	if ( i != UdpMem[0])
		MessageBox(	NULL,"Can not write",
					"UDPCommError",
					MB_OK);
*/			
	}

void read_enet_stats ();

void read_enet_stats ()
	{	// retrieve all enets stats for each TRAM, one TRAM per message
	int i, j, k; 
	int repeat_cnt;
//	char buf[80];
	BYTE id;

	id = 0xff;		// start with Master UDP
	repeat_cnt = i = j = k = 0;
	
	
	}

/////////////////////////////////////////////////////////////////////////////
// CEnet message handlers

BOOL CEnet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int i, j, k;	
	// TODO: Add extra initialization here
	m_uEnetTimer = SetTimer(IDT_IPX_TIMER, 100, NULL);
	if (!m_uEnetTimer) MessageBox(_T("Failed to start Enet timer"));
	collect_enet_stats();
	i = j = k = 0;
/*
	for ( j = 0; j < 5; j++)
		for ( i = 0 ; i < 13 ; i++)
			{
			
			m_report.Format(
				hCtl = GetDlgItem(hDlg,i+200+(j*20) );
			SendMessage(hCtl,EM_LIMITTEXT,12,0);
			}

	read_enet_stats();	//retrieve enet stats for all enet channels from
						// Master UDP

	// Now display info in correct box.
	for ( j = 0; j < 5; j++)
		for ( i = 0 ; i < 13 ; i++)
			{
			m_report.Format(_T("%10ld", (ether_stats[j].edata[i] - estats_offset[j].edata[i]) );
			}
*/	
	SendMsg(ENET_STATS_MODE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CEnet::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscandlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CEnet::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	I_MSG_STATS	*pImsg02;
	int i, j, k;
	CString s,st;
	int tmp[6];
	j = k = 0;
	for (i=0;i<6;i++) 	tmp[i]=	(ether_stats[9].edata[i] - estats_offset[9].edata[i]);
		st.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp[0],
			tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);

	while (plistUtData.GetCount() > 4)
		{
		pImsg02 = (I_MSG_STATS *)plistUtData.RemoveHead();
		delete pImsg02;
		}

	while (!plistUtData.IsEmpty() )
	{

		pImsg02 = (I_MSG_STATS *)plistUtData.RemoveHead();
			memcpy( (void *) &ether_stats, (void *) &pImsg02->IdataStat,
        			sizeof(ether_stats) );
	
		switch (pImsg02->MstrHdr.MsgId )
			{	// switch on  Msg id
		case 1:
///			PrintMsg01( (I_MSG_RUN *) pImsg02);
			break;

		case 2:
//			PrintMsg02(pImsg02);
			break;
		case ENET_STATS_MODE:

			memcpy( (void *) &ether_stats, (void *) &pImsg02->IdataStat,
        			sizeof(ether_stats) );

									// Master UDP
			break;
		default:
		case 3:
		if (nIDEvent == IDT_IPX_TIMER)
	{
/*		read_enet_stats();	//retrieve enet stats for all enet channels from
									// Master UDP
*/
		st="";
				// Now display info in correct box.
	for (i=0;i<6;i++) 	tmp[i]=	(ether_stats[9].edata[i] - estats_offset[9].edata[i]);
		s.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp);
					st += s;
					for (i=0;i<6;i++) 	tmp[i]=		(ether_stats[8].edata[i] - estats_offset[8].edata[i]);
		s.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp[0],
			tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
					st += s;
					for (i=0;i<6;i++) 	tmp[i]=		(ether_stats[4].edata[i] - estats_offset[4].edata[i]);
		s.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp[0],
			tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
					st += s;
					for (i=0;i<6;i++) 	tmp[i]=		(ether_stats[5].edata[i] - estats_offset[5].edata[i]);
		s.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp[0],
			tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
					st += s;
					for (i=0;i<6;i++) 	tmp[i]=		(ether_stats[6].edata[i] - estats_offset[6].edata[i]);
		s.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp[0],
			tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
					st += s;
					for (i=0;i<6;i++) 	tmp[i]=		(ether_stats[7].edata[i] - estats_offset[7].edata[i]);
		s.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp[0],
			tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
					st += s;
					for (i=0;i<6;i++) 	tmp[i]=		(ether_stats[0].edata[i] - estats_offset[0].edata[i]);
		s.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp[0],
			tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
					st += s;
					for (i=0;i<6;i++) 	tmp[i]=		(ether_stats[1].edata[i] - estats_offset[1].edata[i]);
		s.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp[0],
			tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
					st += s;
					for (i=0;i<6;i++) 	tmp[i]=		(ether_stats[2].edata[i] - estats_offset[2].edata[i]);
		s.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp[0],
			tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
					st += s;
					for (i=0;i<6;i++) 	tmp[i]=		(ether_stats[3].edata[i] - estats_offset[3].edata[i]);
		s.Format(_T("%10ld  %10ld  %10ld  %10ld  %10ld  %10ld \r\n\r\n"),tmp[0],
			tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
					st += s;

/*					(ether_stats[4].edata 
					(ether_stats[5].edata 
					(ether_stats[6].edata
					(ether_stats[7].edata
					(ether_stats[0].edata
					(ether_stats[1].edata
					(ether_stats[2].edata
					(ether_stats[3].edata
*/
				}
		
		}
	}	//while

	m_report.SetWindowText(st);
	m_uEnetTimer = SetTimer(IDT_IPX_TIMER, 1000, NULL);
	//	UpdateData(FALSE);
	CDialog::OnTimer(nIDEvent);
}
void CEnet::KillMe() 
	{
	// Public access to OnCancel
	// Add to every modeless dialog
	OnCancel();
	}

void CEnet::OnOK() 
{
	// TODO: Add extra validation here
	
	if ( m_uEnetTimer ) KillTimer(m_uEnetTimer);
	SendMsg(ENET_STATS_STOP);	
	
	CDialog::OnOK();
	CDialog::DestroyWindow();
}

void CEnet::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;

}

void CEnet::OnUpdate() 
{
	// TODO: Add your control notification handler code here
	OnInitDialog();
	
}

void CEnet::OnReset() 
{
	// TODO: Add your control notification handler code here
	memcpy( (void *) &estats_offset, 
			(void *) &ether_stats , sizeof(estats_offset));
	OnInitDialog();
	
}

void CEnet::OnRestore() 
{
	// TODO: Add your control notification handler code here
	memset( (void *) &estats_offset, 0 , sizeof(estats_offset));
	OnInitDialog();
	
}
