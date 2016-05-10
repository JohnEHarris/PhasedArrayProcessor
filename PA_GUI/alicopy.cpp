// alicopy.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
//#include "GatesDlg.h"
// include only the parent dialog header asidlg.h for all child dialogs
#include "TscanDlg.h"
#include "Extern.h"

#include "alicopy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BOOL bGate;
/////////////////////////////////////////////////////////////////////////////
// Calicopy dialog


Calicopy::Calicopy(CWnd* pParent /*=NULL*/)
	: CDialog(Calicopy::IDD, pParent)
{
	//{{AFX_DATA_INIT(Calicopy)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	bGate = FALSE;
}


void Calicopy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Calicopy)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Calicopy, CDialog)
	//{{AFX_MSG_MAP(Calicopy)
	ON_BN_CLICKED(IDC_BGG, OnBgg)
	ON_BN_CLICKED(IDC_BGG2, OnBgg2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Calicopy message handlers

void Calicopy::OnBgg() 
{
	// TODO: Add your control notification handler code here
		bGate = TRUE;
		OnBgg2();
	
}

void Calicopy::OnBgg2() 
{
	// TODO: Add your control notification handler code here

	int  iChannel, iGate;
	for (char Dest=0; Dest<MAX_CHANNEL;Dest++)
	{
		if ( ConfigRec.UtRec.Shoe[((gChannel) / 10) ].Ch[((gChannel) % 10)].Type 
						== ConfigRec.UtRec.Shoe[((Dest) / 10) ].Ch[((Dest) % 10)].Type  )
		{

			iChannel = gChannel;
			iGate = gGate;
			gChannel = Dest;
			CTscanDlg::SendMsg(CHANNEL_SELECT);
			ConfigRec.receiver.gain[Dest] = ConfigRec.receiver.gain[iChannel];
			CTscanDlg::SendMsg(RECEIVER_GAIN);
			if (bGate)
			{
				for (char i=0; i<MAX_GATE; i++)
				{
					gGate = i;
					pCTscanDlg->SendMsg(GATE_SELECT);

					ConfigRec.gates.delay[Dest][i] = ConfigRec.gates.delay[iChannel][i] ;
					CTscanDlg::SendMsg(GATES_DELAY);

					ConfigRec.gates.level[Dest][i] = ConfigRec.gates.level[iChannel][i] ;
					CTscanDlg::SendMsg(GATES_LEVEL);

					ConfigRec.gates.range[Dest][i] = ConfigRec.gates.range[iChannel][i] ;
					CTscanDlg::SendMsg(GATES_RANGE);

					ConfigRec.gates.blank[Dest][i] = ConfigRec.gates.blank[iChannel][i] ;
					CTscanDlg::SendMsg(GATES_BLANK);

					ConfigRec.gates.trg_option[Dest][i] = ConfigRec.gates.trg_option[iChannel][i] ;
					CTscanDlg::SendMsg(GATES_TRIGGER);
				}

				ConfigRec.gates.triger[Dest] = ConfigRec.gates.triger[iChannel] ;
				ConfigRec.gates.g1_invrt[Dest] = ConfigRec.gates.g1_invrt[iChannel] ;

				ConfigRec.lamgate.level[Dest] = ConfigRec.lamgate.level[iChannel] ;
		ConfigRec.receiver.arg1[Dest] = 		ConfigRec.receiver.arg1[iChannel];
		ConfigRec.receiver.arg2[Dest] = 		ConfigRec.receiver.arg2[iChannel];

		ConfigRec.receiver.tcg_step[Dest] = 	ConfigRec.receiver.tcg_step[iChannel];
		m_pTcpThreadRxList->SetTcgUpdateRate(gChannel, ConfigRec.receiver.tcg_step[gChannel]);

		ConfigRec.receiver.tcg_trigger[Dest] = 	ConfigRec.receiver.tcg_trigger[iChannel];
		m_pTcpThreadRxList->SetTcgTrigSel(gChannel, ConfigRec.receiver.tcg_trigger[gChannel]);

		ConfigRec.receiver.tcg_enable[Dest] = ConfigRec.receiver.tcg_enable[iChannel];

				gGate = iGate;
				CTscanDlg::SendMsg(GATE_SELECT);

				gChannel = iChannel;
				CTscanDlg::SendMsg(CHANNEL_SELECT);
			}
		}

	}
}
