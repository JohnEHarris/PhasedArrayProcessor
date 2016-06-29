// Catch.cpp : implementation file
//

/*
Author:	jeh
Date:	1-12-2000
Purpose:	Show ipx data from asi instrument
Revised:	Show cal msg as Truscope shoes (10ch) rather than utron
			shoes of 6 ch
			03/07/02 Show truscope2 inspection data

*/
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

extern CPtrList g_pTcpListUtData;  /* linked list holding the IData */
extern CCriticalSection g_CriticalSection;  /* Critical section for mutual exclusion access of g_pTcpListUtData */
extern ASCAN_HEAD_NIOS g_AscanHead;	

/////////////////////////////////////////////////////////////////////////////
// CCatch dialog


CCatch::CCatch(CWnd* pParent /*=NULL*/)
	: CDialog(CCatch::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CCatch)
	m_nOd = 0;
	m_nWall = 0;
	m_nChnl = 1;
	//}}AFX_DATA_INIT

	m_bPrintRevision = FALSE;

	m_nPlcSel = 0;

	m_nMode = NOP_MODE;
	}


void CCatch::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCatch)
	DDX_Control(pDX, IDC_COMBO_PLC, m_cbPlc);
	DDX_Control(pDX, IDC_CATCH_CHNL_SB, m_sbChnl);
	DDX_Control(pDX, IDC_Stdout, m_ceStdOut);
	DDX_Control(pDX, IDC_FromInst, m_ceFromInst);
	DDX_Radio(pDX, IDC_OD_BN, m_nOd);
	DDX_Radio(pDX, IDC_WALL_BN, m_nWall);
	DDX_Text(pDX, IDC_CATCH_CHNL_EN, m_nChnl);
	DDV_MinMaxInt(pDX, m_nChnl, 1, 40);
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CCatch, CDialog)
	//{{AFX_MSG_MAP(CCatch)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_MSG1, OnMsg1)
	ON_BN_CLICKED(IDC_MSG2, OnMsg2)
	ON_BN_CLICKED(IDC_STATS, OnStats)
	ON_BN_CLICKED(ID_RESET, OnReset)
	ON_BN_CLICKED(IDC_PKHOLD, OnPkhold)
	ON_BN_CLICKED(IDC_UDP_MSG, OnUdpMsg)
	ON_BN_CLICKED(IDC_OD_BN, OnOdBn)
	ON_BN_CLICKED(IDC_ID_BN, OnIdBn)
	ON_BN_CLICKED(IDC_LONG_BN, OnLongBn)
	ON_BN_CLICKED(IDC_OBQ2_BN, OnObq2Bn)
	ON_BN_CLICKED(IDC_WALL_BN, OnWallBn)
	ON_BN_CLICKED(IDC_TRAN_BN, OnTranBn)
	ON_BN_CLICKED(IDC_MSG3, OnMsg3)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_TST_IPX, OnTstIpx)
	ON_BN_CLICKED(IDC_OBQ1_BN, OnObq1Bn)
	ON_BN_CLICKED(IDC_OBQ3_BN, OnObq3Bn)
	ON_BN_CLICKED(IDC_BTN_PLC, OnBtnPlc)
	ON_BN_CLICKED(IDC_BTN_REV, OnBtnRev)
	ON_CBN_SELCHANGE(IDC_COMBO_PLC, OnSelchangeComboPlc)
	//}}AFX_MSG_MAP

	//Manually add message to kill window  .. jeh
//	ON_MESSAGE(WM_USER_ACAL_ERASE, VS10_EraseBars)  // this required to go from Visual Studio 6 to Visual Studio 2010
//  add to header file
//	afx_msg LRESULT VS10_EraseBars(WPARAM, LPARAM);	// this foolishness required to go from Visual Studio to Visual NET
// IN this cpp file call EraseBars from inside VS10_EraseBars

//	ON_MESSAGE(WM_USER_KILL_CATCH, OnCancel)
	ON_MESSAGE(WM_USER_KILL_CATCH, VS10_OnCancel)


END_MESSAGE_MAP()


// Wrappers to get from VC6 to Visual Studio 10
// this foolishness required to go from Visual Studio to Visual NET
afx_msg LRESULT CCatch::VS10_OnCancel(WPARAM, LPARAM)
	{	OnCancel();	return 0;	}

/////////////////////////////////////////////////////////////////////////////
// CCatch message handlers

BOOL CCatch::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// Start timer
	// which will process ipx data msg's collected from OnIpxRXRDY
	// procedure in TscanDlg
	CString s;

	m_nPkHold = 0;
	if (m_nPkHold) s = _T("PkHold");
	else s = _T("PkOff");
	GetDlgItem(IDC_PKHOLD)->SetWindowText(s);

	m_uIpxTimer = SetTimer(IDT_IPX_TIMER, 175, NULL); 	// was 100.. flickered
	if (!m_uIpxTimer) MessageBox(_T("Failed to start IPX timer"));

	UpdateData(FALSE);	// Copy variables to screen

	m_sbChnl.SetScrollRange(1,40,TRUE);
	m_sbChnl.EnableScrollBar(ESB_DISABLE_BOTH);
	m_sbChnl.SetScrollPos(m_nChnl);

	m_cbPlc.SetCurSel(m_nPlcSel);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CCatch::OnTimer(UINT nIDEvent) 
	{
	// TODO: Add your message handler code here and/or call default
	// IPX timer to empty data messages from data msg queue
	CString s;

	// copy inspection data from g_pListUtData to plistUtdata
	I_MSG_RUN *pImsgRun;
	g_CriticalSection.Lock();
	while (!g_pTcpListUtData.IsEmpty() )
	{
		pImsgRun = (I_MSG_RUN *) g_pTcpListUtData.RemoveHead();
		plistUtData.AddTail((void *) pImsgRun);
	}
	g_CriticalSection.Unlock();

#if 1
	I_MSG_RUN	*pImsg;

	while (plistUtData.GetCount() > 8)
		{
		pImsg = (I_MSG_RUN *)plistUtData.RemoveHead();
		delete pImsg;
		}

	while (!plistUtData.IsEmpty() )
		{

		pImsg = (I_MSG_RUN *)plistUtData.RemoveHead();
	
		switch (pImsg->MstrHdr.MsgId )
			{	// switch on  Msg id
		case RUN_MODE:
			PrintMsgRun(  pImsg);
			break;

		case CAL_MODE:
			PrintMsg02((I_MSG_CAL *)pImsg);
			break;
/*		case ENET_STATS_MODE:
			PrintMsg04( (I_MSG_STATS *) pImsg);
			break;
*/
		case PLC_MODE:
			PrintMsgPlc( (I_MSG_PLC *) pImsg);
			break;

		default:
		case PKT_MODE:
			PrintMsg03( (I_MSG_PKT *) pImsg);
			break;

			}	// switch on  msg id
		
		delete pImsg;

		}

#endif

	//dEBUG FOR UDP MSGS
	if ( strlen(UdpMsg99.txt) )
		{
		s.Format(_T("\r\nUdpMsg:[%d] %s"),UdpMsg99.Seq, UdpMsg99.txt);
		UdpMsg99.txt[0] = 0;	// clear txt msg
		m_ceStdOut.SetWindowText(s);
		}

	if (m_bPrintRevision)
		PrintBoardRevision();

	if (m_nMode == PLC_MODE)
		PrintMsgPlc(NULL);

	CDialog::OnTimer(nIDEvent);
	}

// Helpers from dos verison
	
/*********************************************************************/

// Inspection msg

void CCatch::PrintMsgRun(I_MSG_RUN *pMsgRun)
	{
	int i,j,k;
	CString txt, s;
	char B[2];
	static CString t;
	static int it = 0;
	static short nAmp[2][16];
	BYTE *pFlaw;
	short *pWall;

	*(WORD *) &B[0] = pMsgRun->MstrHdr.Mach;
	
	s.Format(_T("Mach = %04x  Seq = %08x  Msg = %3xH Stat = %3xH"),
		pMsgRun->MstrHdr.Mach, pMsgRun->MstrHdr.MsgNum,
		pMsgRun->MstrHdr.MsgId, pMsgRun->InspHdr.wLineStatus);




	if (1)	// (!m_nAmaSonoSel)
		{	// amalog
		
		if (gnTruscopePeriod > 50) i = VELOCITY_DX_FACTOR/gnTruscopePeriod;
		else i = 0;
		txt.Format(_T("\r\nX = %4d  Jnt = %7u  Prd = %5d  FPM = %4d TRU seg's follow:"),
		(int) pMsgRun->InspHdr.xloc, (DWORD) pMsgRun->InspHdr.JointNumber, 
		gnTruscopePeriod, i);

		s += txt;
		txt.Format(_T("\r\nS_0 S_1 S_2 S_3 S_4 S_5 S_6 S_7 S_8 S_9 S_0 S_1 S_2 S_3 S_4\r\n"));
		s += txt;

		switch (2*m_nWall + m_nOd)
			{
		case 0:
		default:
			// wall od
			pWall = &pMsgRun->UtInsp.SegWallMax[0];
			goto WallCode;

		case 1:
			// wall id
			pWall = &pMsgRun->UtInsp.SegWallMin[0];
			goto WallCode;

		case 2:
			// Lod
			pFlaw = &pMsgRun->UtInsp.SegLodFlaw[0];
			goto FlawCode;

		case 3:
			// Lid
			pFlaw = &pMsgRun->UtInsp.SegLidFlaw[0];
			goto FlawCode;

		case 4:
			// Tod
			pFlaw = &pMsgRun->UtInsp.SegTodFlaw[0];
			goto FlawCode;


		case 5:
			// Tid
			pFlaw = &pMsgRun->UtInsp.SegTidFlaw[0];
			goto FlawCode;
					break;

		case 6:
			// Qod1
			pFlaw = &pMsgRun->UtInsp.SegQ1odFlaw[0];
			goto FlawCode;

		case 7:
			// Qid1
			pFlaw = &pMsgRun->UtInsp.SegQ1idFlaw[0];
			goto FlawCode;

		case 8:
			// Qod2
			pFlaw = &pMsgRun->UtInsp.SegQ2odFlaw[0];
			goto FlawCode;

		case 9:
			// Qid2
			pFlaw = &pMsgRun->UtInsp.SegQ2idFlaw[0];
			goto FlawCode;
		case 10:
			// Qod3
			pFlaw = &pMsgRun->UtInsp.SegQ3odFlaw[0];
			goto FlawCode;

		case 11:
			// Qid3
			pFlaw = &pMsgRun->UtInsp.SegQ3idFlaw[0];
			goto FlawCode;

					}

WallCode:

		for ( j = 0; j < 2; j++)
			{
			for ( i = 0; i < 15; i++)
				{
				k = 15*j + i;

				if (m_nPkHold)
					{
					if ( m_nOd)
						{	// id chnl, min hold
						if (nAmp[j][i] == 0)	nAmp[j][i] = 0x3fff;
						else if (nAmp[j][i] > pWall[k])	nAmp[j][i] = pWall[k];
						}
					else
						{
						if (nAmp[j][i] < pWall[k])	nAmp[j][i] = pWall[k];
						}
					}
				else
					{
					nAmp[j][i] = pWall[k];
					}

				if (nAmp[j][i] == 0x3fff) txt = "000 ";
				else txt.Format(_T("%03x "),nAmp[j][i]);
				s += txt;
				}
			s += _T("\r\n");
			}	// for j
		m_ceFromInst.SetWindowText(s);
		return;

FlawCode:

		for ( j = 0; j < 2; j++)
			{
			for ( i = 0; i < 15; i++)
				{
				k = 15*j + i;

				if (m_nPkHold)
					{
					if (nAmp[j][i] < pFlaw[k])
						nAmp[j][i] = pFlaw[k];
					}
				else
					{
					nAmp[j][i] = pFlaw[k];
					}

				// debugging problem
				if ( (k == 29) && (pFlaw[k] == 0x35) && (m_nOd) )
					{
					pFlaw[k] = 0x35;
					}
				txt.Format(_T("%03x "),nAmp[j][i]);
				s += txt;
				}
			s += "\r\n";
			}	// for j
		m_ceFromInst.SetWindowText(s);
		return;

		}

#if 0
	m_ceFromInst.SetWindowText(t);
	if(it==9)
		t.Format(_T("");
#endif
	
	}

/*********************************************************************/

// Cal msg
void CCatch::PrintMsg02(I_MSG_CAL *pCalMsg)
	{

	int i, j;
	int si, ci, gi;
	// 06/12/00 shoe is now 1 plate or utron, what was a row
	CString txt, s;
	char B[2];
	static int nAmp[40][2];

	*(WORD *) &B[0] = pCalMsg->MstrHdr.Mach;
	
	s.Format(_T("Mach = %04x Seq = %08x  Msg = %3x  \r\n"),
		pCalMsg->MstrHdr.Mach, pCalMsg->MstrHdr.MsgNum, pCalMsg->MstrHdr.MsgId);

	// printf does not handle long's correctly, cast to int

	txt.Format(_T("Stat = %04x, %04x, %04x, %04x  ShoeOn = %02x %02x  Ch = %04x,%04x,%04x,%04x\r\n"),
		pCalMsg->InspHdr.status[0], pCalMsg->InspHdr.status[1],
		pCalMsg->InspHdr.status[2], pCalMsg->InspHdr.status[3],	// WORD status[3]
		pCalMsg->InspHdr.ShoeOn[0], pCalMsg->InspHdr.ShoeOn[1], 
		pCalMsg->InspHdr.ChnlOn[0], pCalMsg->InspHdr.ChnlOn[1], 
		pCalMsg->InspHdr.ChnlOn[2], pCalMsg->InspHdr.ChnlOn[3]);

	s += txt;

		if (gnTruscopePeriod > 50) i = VELOCITY_DX_FACTOR/gnTruscopePeriod;
		else i = 0;

	txt.Format(_T("X = %4d Jnt = %7u  Prd = %5d  FPM = %4d\r\n"),
		(int) pCalMsg->InspHdr.xloc, (DWORD) pCalMsg->InspHdr.JointNumber, 
		gnTruscopePeriod, i);

	s += txt;
	txt.Format(_T("Sample: x = %4d Shoe0_Ang = %3d  Line stat = %02x  LoopTime = %4d \r\n"),
		(int)  pCalMsg->InspHdr.xloc, 
		(int)  pCalMsg->InspHdr.nAngleBody1,
		(WORD) pCalMsg->InspHdr.wLineStatus,
		(WORD) 0);				//Old DOS time, ignored.
//		(WORD)(pCalMsg->InspHdr.wIrqTime*5)/6);

	s += txt;

	txt.Format(_T("C01 C02 C03 C04 C05 C06 C07 C08 C09 C10 C11 C12 C13 C14 C15 C16 C17 C18 C19 C20 \r\n"));
	s += txt;

	for (si = 0; si < 4; si++)
	  for ( ci = 0; ci < 10; ci++)
			{	// loop all chnls
			i = 10*si + ci;
			switch (ConfigRec.UtRec.Shoe[si].Ch[ci].Type)
				{
				case IS_NOTHING:
				default:
					nAmp[i][0] = 0;
					nAmp[i][1] = 0;
					break;

				case IS_WALL:
					if (m_nPkHold)
						{
						if (nAmp[i][1] < pCalMsg->Shoe[si].MaxWall[ci])
							nAmp[i][1] = pCalMsg->Shoe[si].MaxWall[ci];

						if (nAmp[i][0] == 0)	nAmp[i][0] = 0x3fff;
						if (nAmp[i][0] > pCalMsg->Shoe[si].MinWall[ci])
							nAmp[i][0] = pCalMsg->Shoe[si].MinWall[ci];
							
						}
					else
						{
						nAmp[i][0] = pCalMsg->Shoe[si].MinWall[ci];
						nAmp[i][1] = pCalMsg->Shoe[si].MaxWall[ci];
						}

					if (nAmp[i][0] == 16383) nAmp[i][0] = 999;
					break;

				case IS_LONG:
				case IS_TRAN:
				case IS_OBQ1:
				case IS_OBQ2:
				case IS_OBQ3:
					for ( gi = 0; gi < 2; gi++)
						{
						if (m_nPkHold)
							{
							if (nAmp[i][gi] < pCalMsg->Shoe[si].Amp[ci][gi])
								nAmp[i][gi] = pCalMsg->Shoe[si].Amp[ci][gi];
							}
						else 
							nAmp[i][gi] = pCalMsg->Shoe[si].Amp[ci][gi];
						}
					break;
				}	// end switch

			  }	// loop all chnls


	for ( j = 0; j < 2; j++)
		{
		for ( i = 20*j; i < 20*(j+1); i++)
			{
			txt.Format(_T("%3d "),nAmp[i][0]);
			s += txt;
			}
	
		 s += _T(" gate0\r\n");

	  // now do gate 1
		 for ( i = 20*j; i < 20*(j+1); i++)
			{
			txt.Format(_T("%3d "),nAmp[i][1]);
			s += txt;
			}
		s += _T(" gate1\r\n\r\n");

		}	// j loop

	m_ceFromInst.SetWindowText(s);

	}


/*********************************************************************/

// Truscope  raw IPX packet 'catch' display

void CCatch::PrintMsg03(I_MSG_PKT *pMsg03)
	{
	// 8 Ascans, 2 channels, 2 gates per channel
	int ai, bi, ci, gi;	// ascan, board, chnl, gate
	int ChStart;
	CString txt, s;
	char B[3];
	static  INST_DATA InstData;	// truscope packet data
	static int ChnlLast;

	*(WORD *) &B[0] = pMsg03->MstrHdr.Mach;
	B[2] = 0;
	

	bi = (((m_nChnl-1)%10) /2);		// chnl starts on 1 for operator, 0 for hw
	ChStart = bi*2;	// start on even chnl
	//ChStart = m_pTcpThreadRxList->m_nActiveChannel;
	bi = 0;	// alsway just 1 board of data from master udp
	if (m_nChnl != ChnlLast)
		{	// operator selected another board for display
		memset ( (void *) &InstData, 0, sizeof(InstData));
		}
	ChnlLast = m_nChnl;
	

	//s.Format(_T("gate 1   CH%02d  gate 2    | gate 3   CH%02d  gate 4    "),ChStart+1, ChStart+1);     
	s.Format(_T("gate 1         gate 2    | gate 3         gate 4    "));     
		
	txt.Format(_T("M=%04x Seq= %08x  Msg= %03x\r\n"),
			pMsg03->MstrHdr.Mach, pMsg03->MstrHdr.MsgNum, 
			pMsg03->MstrHdr.MsgId
			);
	s += txt;

	txt.Format(_T("Sta Amp TOF  Sta Amp TOF | Sta Amp TOF  Sta Amp TOF \r\n"));
	s += txt;

	if (!m_nPkHold) memset ( (void *) &InstData, 0, sizeof(InstData));

	for (ai = 0; ai < 16; ai++)
		{	// loop 8 Ascans on selected pair of channels
//	  for ( ci = ChStart; ci <= ChStart+1; ci++)
		for ( ci = 0; ci < 2; ci++)
		  {
		  for ( gi = 0; gi < 2; gi++)
			{
			if (InstData.bd[bi].ascan[ai].ch[ci].g[gi].amp <
				pMsg03->bd.ascan[ai].ch[ci].g[gi].amp )
					InstData.bd[bi].ascan[ai].ch[ci].g[gi].amp =
					pMsg03->bd.ascan[ai].ch[ci].g[gi].amp;

			InstData.bd[bi].ascan[ai].ch[ci].g[gi].alarm=
				pMsg03->bd.ascan[ai].ch[ci].g[gi].alarm;

			InstData.bd[bi].ascan[ai].ch[ci].g[gi].tof =
				pMsg03->bd.ascan[ai].ch[ci].g[gi].tof;


			txt.Format(_T(" %02x %02x %04x  "),InstData.bd[bi].ascan[ai].ch[ci].g[gi].alarm,
						InstData.bd[bi].ascan[ai].ch[ci].g[gi].amp,
						InstData.bd[bi].ascan[ai].ch[ci].g[gi].tof);
			s += txt;
			}
		  s += _T(" ");
		  }
		s += _T("\r\n");
	  }	// loop 8 Ascans on selecte pair of channels


	m_ceFromInst.SetWindowText(s);

	}

void CCatch::SendMsgTextOut(int nMsg)
	{
	CString s,t;

	s.Format(_T("Sent Command Msg 0x%x"), nMsg);
#if 0
	t.Format
		( "\r\nTo node Addr: %02X-%02X-%02X-%02X-%02X-%02X",
		(UCHAR)SockCmndAddr.sa_nodenum[0],
		(UCHAR)SockCmndAddr.sa_nodenum[1],
		(UCHAR)SockCmndAddr.sa_nodenum[2],
		(UCHAR)SockCmndAddr.sa_nodenum[3],
		(UCHAR)SockCmndAddr.sa_nodenum[4],
		(UCHAR)SockCmndAddr.sa_nodenum[5]
		);
#endif
	s += t;
    m_ceStdOut.SetWindowText(s);
	}

void CCatch::OnMsg1() 
	{
	// TODO: Add your control notification handler code here
	if (m_nMode != NOP_MODE)
		m_pTcpThreadRxList->SendSlaveMsg(NOP_MODE, 0,0,0,0,0,0);
	m_nMode = RUN_MODE;
	m_bPrintRevision = FALSE;

	SendMsg(RUN_MODE);
	m_pTcpThreadRxList->SendSlaveMsg (RUN_MODE, 0,0,0,0,0,0);
	SendMsgTextOut(RUN_MODE);
	m_sbChnl.EnableScrollBar(ESB_DISABLE_BOTH);
	}

void CCatch::OnMsg2() 
	{
	// TODO: Add your control notification handler code here
	if (m_nMode != NOP_MODE)
		m_pTcpThreadRxList->SendSlaveMsg(NOP_MODE, 0,0,0,0,0,0);
	m_nMode = CAL_MODE;
	m_bPrintRevision = FALSE;

	SendMsg(CAL_MODE);
	m_pTcpThreadRxList->SendSlaveMsg (CAL_MODE, 0,0,0,0,0,0);
	SendMsgTextOut(CAL_MODE);
	m_sbChnl.EnableScrollBar(ESB_DISABLE_BOTH);
	}

void CCatch::OnMsg3() 
	{
	// TODO: Add your control notification handler code here
	if (m_nMode != NOP_MODE)
		m_pTcpThreadRxList->SendSlaveMsg(NOP_MODE, 0,0,0,0,0,0);
	m_nMode = PKT_MODE;
	m_bPrintRevision = FALSE;

	SendMsg(CHANNEL_SELECT);			// 
	SendMsg(PKT_MODE);
	m_pTcpThreadRxList->SendSlaveMsg (PKT_MODE, 0,0,0,0,0,0);
	SendMsgTextOut(PKT_MODE);	
	m_sbChnl.EnableScrollBar(ESB_ENABLE_BOTH);
	}

#if 0
void CCatch::OnNetmsg() 
	{
	// TODO: Add your control notification handler code here
	SendMsg(0);
	SendMsgTextOut(0);
	}
#endif

void CCatch::OnStats() 
	{
	CString s,t;
//	NET_STATS Ipx;

#if 0
	// Show lost/duplicate/ total msg stats
	pCTscanDlg->GetIpxStats(&Ipx);
	s.Format(_T("Data Packets =  %8ld      Cmnd Packets =  %8ld\r\n"),
			Ipx.TotalRcvPackets, Ipx.TotalSentPackets );
	t.Format(_T("Lost Packets =  %8ld      Cmnd Lost    =  %8ld\r\n"),
			Ipx.LostRcvPackets , Ipx.LostSentPackets);
	s += t;
	t.Format(_T("Dup  Packets =  %8ld      Cmnd Dup     =  %8ld"),
			Ipx.DupRcvPackets, Ipx.DupSentPackets);
	s += t;
#endif
	// UPD status
	t.Format(_T("\r\nUdp  Packets =  %8ld\r\n"),
			pCTscanDlg->m_nUdpRcvCnt);
	s += t;
	t.Format(_T("Udp  Lost    =  %8ld\r\n"),
			pCTscanDlg->m_nUdpRcvLostCnt);
	s += t;
	t.Format(_T("Udp  Dup     =  %8ld"),
			pCTscanDlg->m_nUdpRcvDupCnt);
	s += t;

	m_ceStdOut.SetWindowText(s);
	
	}

void CCatch::OnOK() 
	{
	// TODO: Add extra validation here
	// Need this to get to PostNcDestroy which nulls ptr to the routine
	m_pTcpThreadRxList->SendSlaveMsg (NOP_MODE, 0,0,0,0,0,0);

	if ( m_uIpxTimer ) KillTimer(m_uIpxTimer);
	
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

void CCatch::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}

void CCatch::OnCancel() 
	{
	// TODO: Add extra cleanup here
	// Need this to get to PostNcDestroy which nulls ptr to the routine
	m_pTcpThreadRxList->SendSlaveMsg (NOP_MODE, 0,0,0,0,0,0);

	if ( m_uIpxTimer ) KillTimer(m_uIpxTimer);
	
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CCatch::KillMe() 
	{
	// Public access to OnCancel
	// Add to every modeless dialog
	OnCancel();
	}

void CCatch::OnReset() 
	{
#if 0
	// TODO: Add your control notification handler code here
	// Reset received msg status and display
//	for (i=0;i<4;i++)
	{
		IpxStat.TotalRcvPackets = 0;
		IpxStat.LostRcvPackets = 0;
		IpxStat.DupRcvPackets = 0;
	}
	OnStats();
	memset(&SockCmndAddr.sa_nodenum,0xff,6);
#endif
	}

BOOL CCatch::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Asidlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}


void CCatch::OnPkhold() 
	{
	// TODO: Add your control notification handler code here
	CString s;

	m_nPkHold ^= 1;
	m_nPkHold &= 1;
	if (m_nPkHold) s = _T("PkHold");
	else s = _T("PkOff");
	GetDlgItem(IDC_PKHOLD)->SetWindowText(s);
	}

void CCatch::OnUdpMsg() 
	{
	// TODO: Add your control notification handler code here
	// Development only, test sending udp messages to milburn taylor
	//SendUdpMsg(99,"Hello Milburn");
	}

void CCatch::OnOdBn() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen to variables
	
	}

void CCatch::OnIdBn() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen to variables
	
	}

void CCatch::OnLongBn() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen to variables
	
	}

void CCatch::OnObq1Bn() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen to variables
	
	}

void CCatch::OnObq2Bn() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen to variables
	
	}

void CCatch::OnObq3Bn() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen to variables
	
	}

void CCatch::OnWallBn() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen to variables
	
	}

void CCatch::OnTranBn() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen to variables
	
	}

void CCatch::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// TODO: Add your message handler code here and/or call default

	// TODO: Add your message handler code here and/or call default
	int nStart, nDelta;		// where the bar started and amount of change
	int nResult;			// normally start + delta
	int nMax, nMin;
	int nCtlId;



	nStart = pScrollBar->GetScrollPos();
	pScrollBar->GetScrollRange(&nMin, &nMax);

	switch ( nSBCode)
		{	// switch on type of motion
	case SB_LINELEFT:
		nDelta = -1;
		break;

	case SB_LINERIGHT:
		nDelta = 1;
		break;

	case SB_PAGELEFT:
		nDelta = -5;
		break;

	case SB_PAGERIGHT:
		nDelta = 5;
		break;

	case SB_THUMBTRACK:
		nDelta = (int)nPos - nStart;
		break;


	default:
		return;
		break;

		}	// switch on type of motion

	nResult = nStart;
	nResult += nDelta;
	if ( nResult > nMax) nResult = nMax;
	if ( nResult < nMin) nResult = nMin;


	// Now determine which scroll bar was used
	nCtlId = pScrollBar->GetDlgCtrlID();



	switch(nCtlId)
		{
		// Abs controls
	case IDC_CATCH_CHNL_SB:
		m_nChnl = nResult;
		gChannel = m_nChnl - 1;;
		pCTscanDlg->ChangeActiveChannel();
		break;
	
	
	default:
		break;
		}

		
	pScrollBar->SetScrollPos(nResult, TRUE);

	UpdateData(FALSE);	// copy data to screen
//	UpdateConfigRec();	// copy data to config rec structure

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	
	}

void CCatch::OnTstIpx() 
	{
	// TODO: Add your control notification handler code here
	SendMsg(DEBUG_CONTROL);	// Test ipx function
	SendMsg(CHANNEL_SELECT);	// Test ipx function
	}

void CCatch::OnBtnPlc() 
{
	// TODO: Add your control notification handler code here
	m_bPrintRevision = FALSE;

	if (m_nMode != NOP_MODE)
		m_pTcpThreadRxList->SendSlaveMsg(NOP_MODE, 0,0,0,0,0,0);
	m_nMode = PLC_MODE;
	m_pTcpThreadRxList->SendSlaveMsg (PLC_MODE, (WORD) m_nPlcSel,0,0,0,0,0);
	
}

void CCatch::PrintMsgPlc(I_MSG_PLC *pMsg)
{
	CString txt, s;
	DWORD nXloc = g_AscanHead.wLocation;  //pMsg->PlcStatus.dwXloc;
	DWORD nOmega = g_AscanHead.wClock;    //pMsg->PlcStatus.dwOmega;
	DWORD dwMotionBus = g_AscanHead.bDin; //pMsg->PlcStatus.dwMotionBus;
	DWORD dwDIO = g_AscanHead.bDin; //pMsg->PlcStatus.dwDiscreteIO;
	int i;		// , x, omega;

	s.Format (_T("     Motion & PLC Status\r\n\r\n"));

	txt.Format (_T("                              15                0\r\n"));
	s += txt;

	txt.Format (_T("Motion Bus    = 0x%08X  ( "), dwMotionBus);
	s += txt;

	for (i=0; i<16; i++)
	{
		if ( ((i+1)%4) == 0 )
			txt.Format (_T("%01d "), (dwMotionBus >> (15-i)) & 0x1 );
		else
			txt.Format (_T("%01d"), (dwMotionBus >> (15-i)) & 0x1 );
		s += txt;
	}

	txt.Format (_T(")\r\n\r\n"));
	s += txt;

	txt.Format (_T("Discrete I/O  = 0x%08X  ( "), dwDIO);
	s += txt;

	for (i=0; i<16; i++)
	{
		if ( ((i+1)%4) == 0 )
			txt.Format (_T("%01d "), (dwDIO >> (15-i)) & 0x1 );
		else
			txt.Format (_T("%01d"), (dwDIO >> (15-i)) & 0x1 );
		s += txt;
	}

	txt.Format (_T(")\r\n\r\n"));
	s += txt;

	txt.Format (_T("X counter     = %d\r\n\r\n"), nXloc);
	s += txt;

	txt.Format (_T("Clock counter = %d\r\n\r\n"), nOmega);
	s += txt;

	txt.Format (_T("Clock period = %d\r\n\r\n"), g_AscanHead.wPeriod);
	s += txt;

/*
	txt.Format ("X counter     = 0x%08X\r\n\r\n", nXloc);
	s += txt;

	txt.Format ("Omega counter = 0x%08X\r\n\r\n", nOmega);
	s += txt;

	x = (int) ( (short) ((nXloc << 2) & 0x0000FFFF) );
	txt.Format ("X location 1  = %d\r\n\r\n", x/4);
	s += txt;

	x = (int) ( (short) (((nXloc >> 16) << 2) & 0x0000FFFF) );
	txt.Format ("X location 2  = %d\r\n\r\n", x/4);
	s += txt;

	omega = (int) ( (short) ((nOmega >> 16) & 0x000001FF) );
	if (omega < 180)
		txt.Format ("Omega         = %d\r\n\r\n", omega);
	else
		txt.Format ("Omega         =        %d\r\n\r\n", omega);
	s += txt;
*/

	txt.Format(_T("Motion Bus bit field definition:\r\n\r\n"));
	s += txt;

	txt.Format(_T("0: Station 1 Enable;  1: Station 2 Enable;  2: Station 1 Pulse;  3: Station 2 Pulse\r\n"));
	s += txt;
	txt.Format(_T("4: X Direction (1=F); 5: Fast Input Spare;  6: Undefined;        7: Undefined\r\n"));
	s += txt;
	txt.Format(_T("8: 12:00 Clock Pulse; 9: Undefined;         10:Box 1 Enable;     11:Box 2 Enable\r\n"));
	s += txt;
	txt.Format(_T("12:Box 3 Enable;      13:Box 4 Enable;      14:Home or Away;     15:End Enable\r\n"));
	s += txt;

	m_ceFromInst.SetWindowText(s);
}


void CCatch::PrintBoardRevision()
{
	CString txt, s;
	int i, j;
	BOARD_REV BdRev;

	// Hardware revision numbers

	m_pTcpThreadRxList->GetBoardRevision(&BdRev);

	s.Format(_T("     Hardware Revision Numbers\r\n\r\n"));

	txt.Format(_T("           Two-Channel-Gate Board\r\n"));
	s += txt;

	txt.Format(_T("Chassis    1    2    3    4    5    ADI\r\n"));
	s += txt;

	for (i=1; i<MAX_SHOES+1; i++)
	{

		if (i<10)
			txt.Format(_T("   %01d       "), i );
		else
			txt.Format(_T("  %02d       "), i );
		s += txt;

		for (j=0; j<MAX_BOARD_PER_INSTRUMENT; j++)
		{
			txt.Format(_T("%02X   "), BdRev.nRev[i][j]);
			s += txt;
		}

		txt.Format(_T("\r\n"));
		s += txt;
	}
		
	txt.Format(_T("  Sys       "));
	s += txt;
	txt.Format(_T("                        %02X\r\n\r\n"), BdRev.nRev[0][MAX_BOARD_PER_INSTRUMENT-1]);
	s += txt;

	// Software revision numbers

	//char MmiRevision[40];
	CString MmiRevision;
	//strncpy(MmiRevision, TRUSCAN_VERSION, 40);
	//sprintf(MmiRevision,TRUSCAN_VERSION);
	MmiRevision.Format(TRUSCAN_VERSION);

	txt.Format (_T("     Software Revision Numbers\r\n\r\n"));
	s += txt;

	txt.Format (_T("MMI:     %s\r\n"), MmiRevision);
	s += txt;

	txt.Format (_T("Master:  %s\r\n"), BdRev.MasterCodeRev);
	s += txt;

	txt.Format (_T("RDP:     %s\r\n"), BdRev.SlaveCodeRev);
	s += txt;

	m_ceFromInst.SetWindowText(s);
}

void CCatch::OnBtnRev() 
{
	// TODO: Add your control notification handler code here
	if (m_nMode != NOP_MODE)
		m_pTcpThreadRxList->SendSlaveMsg(NOP_MODE, 0,0,0,0,0,0);
	m_nMode = NOP_MODE;

	m_bPrintRevision = TRUE;
}

void CCatch::OnSelchangeComboPlc() 
{
	// TODO: Add your control notification handler code here
	m_nPlcSel = m_cbPlc.GetCurSel();

	OnBtnPlc();
}
