// Packet.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
//#include "Packet.h"
#include "TscanDlg.h"
#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPacket dialog
/* The B431_ETHER_STATS message contains ethernet statistics returned by the
   driver. The statistics are contained in the following structure.
*/

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



extern struct {
		unsigned long edata[13];
		} ether_stats[5], estats_offset[5];


//BOARDPACKET board_packet, board_packet_old;	// where catch data is stored from udp

int		hTimer4;	// not really a handle
int msgposted = 0;		// timer incr when posting, UPDATE_BN decr when servicing

int packet_format = 0;	// New packet format
/************************************************************************/


void collect_enet_stats (HWND);

void collect_enet_stats (HWND hDlg)
	{
	int i;
	
#if 0
	// For testing, generate test data
	int j;
	unsigned long k;
	k = 100000;
	for ( j = 0; j < 5; j++)
		for ( i = 0; i < 13; i++)
			ether_stats[j].edata[i] = k++;

	UdpMem[0] = 4;		// length of reply from udp
	UdpMem[1] = (char)0xf0;	// udp msg instead of inst msg
	UdpMem[2] = 12;		// cmnd Master to gather stats

	i = WriteLink(1,(char *) &UdpMem,8,10000);
	i = ReadLink(1,(char *) &UdpMem,UdpMem[0],50000);

	if ( i != UdpMem[0])
		MessageBox(hDlg, 
					"Can not write",
					"UDPCommError",
					MB_OK);
#endif
			
	}

void read_enet_stats (HWND);

void read_enet_stats (HWND hDlg)
	{	// retrieve all enets stats for each TRAM, one TRAM per message
#if 0
	int i, j, k, repeat_cnt;
	char msg_chk[4], buf[80];
	BYTE id;

	id = 0xff;		// start with Master UDP
	repeat_cnt = 0;
	
	for ( k = 0; k < 5; /* no 3rd condition */  )
		{
		UdpMem[0] = sizeof(ETHER_STATS) + 4;
		UdpMem[1] = (char)0xf0;	// udp msg instead of inst msg
		UdpMem[2] = 13;		// cmnd Master to return stats for channel
		UdpMem[3] = (id+k);	// The Master to start with

		memcpy(msg_chk, (void *) &UdpMem[0], 4);

		i = WriteLink(1,(char *) &UdpMem,8,10000);

		i = ReadLink(1,(char *) &UdpMem,UdpMem[0],50000);

		j = memcmp(msg_chk, (void *) &UdpMem[0], 4);

		if ( ( i != UdpMem[0]) || ( j != 0) )
			{
			if ( repeat_cnt++ > 3)
				{
				strcpy(buf,"NoEtherStats");
				sprintf(msg_chk,"%2d", ( (k+1) &7) );
				strcat(buf,msg_chk);
				MessageBox(hDlg, buf, 
							"UDPCommError",
							MB_OK);
				return;
				}
			}
		else
        	{
			memcpy( (void *) &ether_stats[k], (void *) &UdpMem[4],
        			sizeof(ETHER_STATS) );
			repeat_cnt = 0;
			k++;	// next TRAM's stats.
            }
		}	/* for ( k ) */
#endif	
	}
	



CPacket::CPacket(CWnd* pParent /*=NULL*/)
	: CDialog(CPacket::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPacket)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPacket::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPacket)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPacket, CDialog)
	//{{AFX_MSG_MAP(CPacket)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPacket message handlers

void CPacket::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
}

void CPacket::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CPacket::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPacket::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

void CPacket::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
