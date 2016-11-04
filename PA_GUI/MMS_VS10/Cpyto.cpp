// Cpyto.cpp : implementation file
//

#include "stdafx.h"
//#include "Asi.h"
#include "Truscan.h"
//#include "GatesDlg.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



bool bGates,bAlms,bTCG,bFlaw,bRcv,bTOF,bScp;
/////////////////////////////////////////////////////////////////////////////
// CCpyto dialog


CCpyto::CCpyto(CWnd* pParent /*=NULL*/)
	: CDialog(CCpyto::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCpyto)
	m_bAlarms = FALSE;
	m_bFlaw = FALSE;
	m_bRcvr = FALSE;
	m_bScope = FALSE;
	m_bTcg = FALSE;
	m_bTof = FALSE;
	m_bGates = FALSE;
	//}}AFX_DATA_INIT
	bGates=bAlms=bTCG=bFlaw=bRcv=bTOF=bScp=FALSE;
}


void CCpyto::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCpyto)
	DDX_Check(pDX, IDC_CHECKAlms, m_bAlarms);
	DDX_Check(pDX, IDC_CHECKFlawS, m_bFlaw);
	DDX_Check(pDX, IDC_CHECKRcv, m_bRcvr);
	DDX_Check(pDX, IDC_CHECKScp, m_bScope);
	DDX_Check(pDX, IDC_CHECKTCG, m_bTcg);
	DDX_Check(pDX, IDC_CHECKTOF, m_bTof);
	DDX_Check(pDX, IDC_CHGates, m_bGates);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCpyto, CDialog)
	//{{AFX_MSG_MAP(CCpyto)
	ON_BN_CLICKED(IDC_RB_ALL, OnRbAll)
	ON_BN_CLICKED(IDC_RB_SEL, OnRbSel)
	ON_BN_CLICKED(IDC_CHECKAlms, OnCHECKAlms)
	ON_BN_CLICKED(IDC_CHECKFlawS, OnCHECKFlawS)
	ON_BN_CLICKED(IDC_CHECKRcv, OnCHECKRcv)
	ON_BN_CLICKED(IDC_CHECKScp, OnCHECKScp)
	ON_BN_CLICKED(IDC_CHECKTCG, OnChecktcg)
	ON_BN_CLICKED(IDC_CHECKTOF, OnChecktof)
	ON_BN_CLICKED(IDC_CHGates, OnCHGates)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCpyto message handlers
void CCpyto::CopyChannel(BYTE Dest)
	{
	int i, iChannel, iGate;
	iChannel = gChannel;
	iGate = gGate;
	gChannel = Dest;
	SendMsg(CHANNEL_SELECT);
	if (m_bGates) 
		{
		for (i=0; i<MAX_GATE; i++)
		{
			gGate = i;
			pCTscanDlg->SendMsg(GATE_SELECT);

			ConfigRec.gates.delay[Dest][i] = ConfigRec.gates.delay[iChannel][i] ;
			SendMsg(GATES_DELAY);

			ConfigRec.gates.level[Dest][i] = ConfigRec.gates.level[iChannel][i] ;
			SendMsg(GATES_LEVEL);

			ConfigRec.gates.range[Dest][i] = ConfigRec.gates.range[iChannel][i] ;
			SendMsg(GATES_RANGE);

			ConfigRec.gates.blank[Dest][i] = ConfigRec.gates.blank[iChannel][i] ;
			SendMsg(GATES_BLANK);

			ConfigRec.gates.trg_option[Dest][i] = ConfigRec.gates.trg_option[iChannel][i] ;
			SendMsg(GATES_TRIGGER);
		}

		ConfigRec.gates.triger[Dest] = ConfigRec.gates.triger[iChannel] ;
		ConfigRec.gates.g1_invrt[Dest] = ConfigRec.gates.g1_invrt[iChannel] ;

		ConfigRec.lamgate.level[Dest] = ConfigRec.lamgate.level[iChannel] ;
			
		}

	if (m_bAlarms)
		{
		ConfigRec.alarm.laminar[Dest][0] = 	ConfigRec.alarm.laminar[iChannel][0];
		ConfigRec.alarm.laminar[Dest][1] = 	ConfigRec.alarm.laminar[iChannel][1];
			
		ConfigRec.alarm.polarity[Dest][0] = 	ConfigRec.alarm.polarity[iChannel][0];
		ConfigRec.alarm.polarity[Dest][1] = 	ConfigRec.alarm.polarity[iChannel][1];

		ConfigRec.alarm.trigger[Dest][0] = 	ConfigRec.alarm.trigger[iChannel][0];
		ConfigRec.alarm.trigger[Dest][1] = 	ConfigRec.alarm.trigger[iChannel][1];

		}

	if (m_bTcg)
		{
		//ConfigRec.receiver.arg1[Dest] = 		ConfigRec.receiver.arg1[iChannel];
		//ConfigRec.receiver.arg2[Dest] = 		ConfigRec.receiver.arg2[iChannel];

		ConfigRec.receiver.tcg_step[Dest] = 	ConfigRec.receiver.tcg_step[iChannel];
		m_pTcpThreadRxList->SetTcgUpdateRate(gChannel, ConfigRec.receiver.tcg_step[gChannel]);

		ConfigRec.receiver.tcg_trigger[Dest] = 	ConfigRec.receiver.tcg_trigger[iChannel];
		m_pTcpThreadRxList->SetTcgTrigSel(gChannel, ConfigRec.receiver.tcg_trigger[gChannel]);

		//ConfigRec.receiver.tcg_enable[Dest] = ConfigRec.receiver.tcg_enable[iChannel];

		ConfigRec.receiver.TcgRec[Dest] = ConfigRec.receiver.TcgRec[iChannel];
		SendMsg(TCG_FN);
		}
	if (m_bFlaw)
		{
		ConfigRec.cal.idScale[Dest]		= ConfigRec.cal.idScale[iChannel];
		ConfigRec.cal.odScale[Dest]		= ConfigRec.cal.odScale[iChannel];

		}
	
	if (m_bRcvr)
		{
			ConfigRec.receiver.gain[Dest] = 		ConfigRec.receiver.gain[iChannel];
			SendMsg(RECEIVER_GAIN);

			ConfigRec.receiver.reject[Dest] = 	ConfigRec.receiver.reject[iChannel];

			ConfigRec.receiver.fil_option[Dest] = ConfigRec.receiver.fil_option[iChannel];
			SendMsg(RECEIVER_FILT);

			for (i=0; i<MAX_GATE; i++)
			{
				gGate = i;
				SendMsg(GATE_SELECT);

				ConfigRec.gates.polarity[Dest][i] = ConfigRec.gates.polarity[iChannel][i];
				ConfigRec.receiver.polarity[Dest] = ConfigRec.receiver.polarity[iChannel];
				SendMsg(RECEIVER_POLT);

				ConfigRec.gates.det_option[Dest][i] = ConfigRec.gates.det_option[iChannel][i];
				ConfigRec.receiver.det_option[Dest] = ConfigRec.receiver.det_option[iChannel];
				SendMsg(RECEIVER_RF_VIDEO);
			}
		}

	if (m_bTof)
		{
		for (i=0; i<MAX_GATE; i++)
		{
			gGate = i;
			SendMsg(GATE_SELECT);

			ConfigRec.timeoff.trigger[Dest][i] = 	ConfigRec.timeoff.trigger[iChannel][i];
			SendMsg(TOF_TRIGGER);

			ConfigRec.timeoff.stopon[Dest][i] = 	ConfigRec.timeoff.stopon[iChannel][i];
			SendMsg(TOF_STOPON);
		}

		ConfigRec.timeoff.resolution[Dest] = 	ConfigRec.timeoff.resolution[iChannel];
		}

	if (m_bScope)
	{
		//ConfigRec.OscopeRec.trace1[Dest] = 		ConfigRec.OscopeRec.trace1[iChannel];
		//ConfigRec.OscopeRec.trace2[Dest] = 		ConfigRec.OscopeRec.trace2[iChannel];
		ConfigRec.OscopeRec.trigger[Dest] = 	ConfigRec.OscopeRec.trigger[iChannel];
		//ConfigRec.OscopeRec.trigger_index[Dest] = ConfigRec.OscopeRec.trigger_index[iChannel];
		ConfigRec.receiver.ascan_delay[Dest] = 		ConfigRec.receiver.ascan_delay[iChannel];
		ConfigRec.receiver.ascan_range[Dest] = 		ConfigRec.receiver.ascan_range[iChannel];
		ConfigRec.receiver.ascan_baseline[Dest] = 		ConfigRec.receiver.ascan_baseline[iChannel];
	}
	ConfigRec.pulser.pulse_width[Dest] = 	ConfigRec.pulser.pulse_width[iChannel];
	ConfigRec.pulser.pulse_chn_on[Dest] = 	ConfigRec.pulser.pulse_chn_on[iChannel];


	//ConfigRec.UtRec.Shoe[((Dest) / 10) ].Ch[((Dest) % 10)].Type=ConfigRec.UtRec.Shoe[((iChannel) / 10) ].Ch[((iChannel) % 10)].Type;
	ConfigRec.UtRec.trigger_select[Dest]	=	ConfigRec.UtRec.trigger_select[iChannel];

	// restore original active channel and gate
	gGate = iGate;
	SendMsg(GATE_SELECT);

	gChannel = iChannel;
	SendMsg(CHANNEL_SELECT);
	}


void CCpyto::OnOK() 
	{
	// TODO: Add extra validation here

	UpdateData(TRUE);	// Copy screen to variables
	CWaitCursor waitCursor;

	if ( gCopyTo == MAX_CHANNEL)
		{
		for (BYTE i=0; i<MAX_CHANNEL;i++)
			{
			if ( ConfigRec.UtRec.Shoe[((gChannel) / 10) ].Ch[((gChannel) % 10)].Type 
						== ConfigRec.UtRec.Shoe[((i) / 10) ].Ch[((i) % 10)].Type  )
					CopyChannel(i);
			}
		}
	
	else 		CopyChannel(gCopyTo);

	
	CDialog::OnOK();
	}


void CCpyto::OnRbAll() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen to variables
	m_bAlarms = m_bGates = m_bRcvr = m_bTof = m_bTcg = m_bScope = m_bFlaw = TRUE;
	UpdateData(FALSE);	// Copy variables to screen
	}

void CCpyto::OnRbSel() 
	{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	// Copy screen to variables
	m_bAlarms = m_bGates = m_bRcvr = m_bTof = m_bTcg = m_bScope = m_bFlaw = FALSE;
	UpdateData(FALSE);	// Copy variables to screen
	}

void CCpyto::SendMsg(int nMsgId)
{
	pCTscanDlg->SendMsg(nMsgId);
}

void CCpyto::OnCHECKAlms() 	{	UpdateData(TRUE);	/* Copy screen to variables*/	}

void CCpyto::OnCHECKFlawS()  {	UpdateData(TRUE);	}

void CCpyto::OnCHECKRcv()  	{	UpdateData(TRUE);	}

void CCpyto::OnCHECKScp() 	{	UpdateData(TRUE);	}

void CCpyto::OnChecktcg()  	{	UpdateData(TRUE);	}

void CCpyto::OnChecktof()  	{	UpdateData(TRUE);	}

void CCpyto::OnCHGates()  	{	UpdateData(TRUE);	}


