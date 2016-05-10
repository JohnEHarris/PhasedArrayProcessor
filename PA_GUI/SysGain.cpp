// SysGain.cpp : implementation file
//

#include "stdafx.h"
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
#define SYSGAIN_MAX			6
#define SYSGAIN_MIN			-6

// Allow +/- 50% change to flaw scalers
#define FS_ADJ_MAX			50
#define FS_ADJ_MIN			-50

static int fs_sb_val1,lto;			// flaw scaler scroll bar value, gate 1
static int fs_sb_val2;			// flaw scaler scroll bar value, gate 2
static int sysgain_sb_val;		// system gain scroll bar value
bool bSGain;
/////////////////////////////////////////////////////////////////////////////
// CSysGain dialog

short *nPosn;
void CSysGain::Save_Pos()
	{
	WINDOWPLACEMENT wp;
	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition(_T("SYSTEM_GAIN"), &wp);
	}

CSysGain::CSysGain(CWnd* pParent /*=NULL*/)
	: CDialog(CSysGain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSysGain)
	m_sSgain = _T("0   db");
	m_sFScaler = _T("0   %");
	//}}AFX_DATA_INIT
	bSGain=TRUE;
}


void CSysGain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysGain)
	DDX_Control(pDX, IDC_SbSclr, m_sbSclr);
	DDX_Control(pDX, IDC_SB_SGain, m_SBGain);
	DDX_Text(pDX, IDC_SGain, m_sSgain);
	DDV_MaxChars(pDX, m_sSgain, 8);
	DDX_Text(pDX, IDC_FScaler, m_sFScaler);
	DDV_MaxChars(pDX, m_sFScaler, 8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSysGain, CDialog)
	//{{AFX_MSG_MAP(CSysGain)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_gate1, Ongate1)
	ON_BN_CLICKED(IDC_gate2, Ongate2)
	ON_BN_CLICKED(IDC_CKLONG, OnCklong)
	ON_BN_CLICKED(IDC_CKObq1, OnCKObq1)
	ON_BN_CLICKED(IDC_CKObq2, OnCKObq2)
	ON_BN_CLICKED(IDC_CKObq3, OnCKObq3)
	ON_BN_CLICKED(IDC_CKtran, OnCKtran)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysGain message handlers

void CSysGain::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int nDelta;		// where the bar started and amount of change
	int nMax, nMin;
	int nCtlId;
	int dummy = 5;
	CString s;
	nCtlId = pScrollBar->GetDlgCtrlID();
	nPosn = (short*)&dummy;

	switch (nCtlId)
		{
	case IDC_SB_SGain:
		nMin=SYSGAIN_MIN;
		nMax=SYSGAIN_MAX;
		break;
	case IDC_SbSclr:
		nMin=FS_ADJ_MIN;
		nMax=FS_ADJ_MAX;
		break;
	default:
		break;
		}

	*nPosn = pScrollBar->GetScrollPos();
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
		nDelta = -1;	//-(nMax - nMin)/10;
		if (nCtlId == IDC_SbSclr) nDelta = -5;
		break;
	case SB_PAGERIGHT:
		nDelta = 1;	//(nMax - nMin)/10;
		if (nCtlId == IDC_SbSclr) nDelta = 5;
		break;
	case SB_THUMBTRACK:
		nDelta = (int)nPos - *nPosn;
		break;
	default:
		nDelta = 0;		//need a value 8-27-2001
		return;
		break;
		}	// switch on type of motion
	*nPosn += nDelta;
	if ( *nPosn > nMax) *nPosn = nMax;
	if ( *nPosn < nMin) *nPosn = nMin;
	pScrollBar->SetScrollPos(*nPosn, TRUE);

	switch (nCtlId)
		{
	case IDC_SB_SGain:
		// Send gate delay to instrument
	sysgain_sb_val = *nPosn;
	m_sSgain.Format (_T("%1d   db"), *nPosn);
		break;
	case IDC_SbSclr:
		// Send gate delay to instrument
	if ( gGate == 0)
		fs_sb_val1 = *nPosn;
	else 		
		fs_sb_val2 = *nPosn;
	m_sFScaler.Format (_T("%2d  %%"), *nPosn);
		break;
	default:
		break;
		}
	UpdateData(FALSE);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
BOOL CSysGain::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Asidlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CSysGain:: UpdateGates()
{
	if ( gGate == 0){
		m_sbSclr.SetScrollPos(fs_sb_val1,TRUE );
		m_sFScaler.Format (_T("%2d  %%"), fs_sb_val1);
	}
	else {
		m_sbSclr.SetScrollPos(fs_sb_val2,TRUE );
		m_sFScaler.Format (_T("%2d  %%"), fs_sb_val2);
	}
	int button= gGate ? IDC_gate2 : IDC_gate1;
	CheckRadioButton( IDC_gate1, IDC_gate2, button );


}
BOOL CSysGain::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	if ( bSGain )
	{
		bSGain = FALSE;
		GetWindowPlacement(&wp);
		dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		pCTscanDlg->GetWindowLastPosition(_T("SYSTEM_GAIN"), &rect);
		if ( ( (rect.right - rect.left) >= dx ) &&
			 ( (rect.bottom - rect.top) >= dy ))
			{
			wp.rcNormalPosition = rect;
			SetWindowPlacement(&wp);
			}
	}
	gChannel %= 40;
	gGate &= 1;
	//gShoe &= 10;
	if (gShoe < 0) gShoe = 0;
	if (gShoe > 9) gShoe = 9;
	m_SBGain.SetScrollRange(SYSGAIN_MIN,SYSGAIN_MAX,FALSE);
	m_SBGain.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sSgain.Format (_T("%1d   db"), sysgain_sb_val);
	m_SBGain.SetScrollPos(sysgain_sb_val,TRUE);
	m_sbSclr.SetScrollRange(FS_ADJ_MIN,FS_ADJ_MAX,FALSE);
	m_sbSclr.EnableScrollBar(ESB_ENABLE_BOTH);
	UpdateGates();
	BYTE lto=ConfigRec.cal.lto;
		CheckDlgButton( IDC_CKLONG, lto & IS_LONG);
		CheckDlgButton( IDC_CKtran, lto & IS_TRAN);
		CheckDlgButton( IDC_CKObq1, lto & IS_OBQ1);
		CheckDlgButton( IDC_CKObq2, lto & IS_OBQ2);
		CheckDlgButton( IDC_CKObq3, lto & IS_OBQ3);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSysGain::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;

}
CSysGain::~CSysGain()
	{
//	delete m_pUndo;
	}


void CSysGain::Ongate1() 
{
	// TODO: Add your control notification handler code here
	gGate=0;
	pCTscanDlg->UpdateChnlSelected();
	SendMsg(GATE_SELECT);
}

void CSysGain::Ongate2() 
{
	// TODO: Add your control notification handler code here
	gGate=1;
	pCTscanDlg->UpdateChnlSelected();
	SendMsg(GATE_SELECT);
}

void CSysGain::OnOK() 
{
	// TODO: Add extra validation here
/*				
	for ( i = 0; i < MAX_CHANNEL; i++)
		{
		// If this channel selected for gain adjustment, then do it
//	ConfigRec.UtRec.Shoe[i].Ch[i].Type ;

		if (ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Type  & ConfigRec.cal.lto)
			{
			if ( gGate == 0)
				{
				ConfigRec.cal.idScale[i] = lpDefault->cal.idScale[i] *
											(float)(100 + fs_sb_val1) / 100.0f;
//				send_flaw_scaler (i, 0, 1);
				}
			else
				{
				ConfigRec.cal.odScale[i] = lpDefault->cal.odScale[i] *
											(float)(100 + fs_sb_val2) / 100.0f;
//				send_flaw_scaler (i, 1, 1);
				}
			}
		}

*/  
	ConfigRec.cal.lto = lto;
	Save_Pos();
	CDialog::OnOK();
	CDialog::DestroyWindow();

}

void CSysGain::OnCklong() 
{
	// TODO: Add your control notification handler code here
	lto ^= IS_LONG;
}

void CSysGain::OnCKObq1() 
{
	// TODO: Add your control notification handler code here
	lto ^= IS_OBQ1;
	
}

void CSysGain::OnCKObq2() 
{
	// TODO: Add your control notification handler code here
	lto ^= IS_OBQ2;
}

void CSysGain::OnCKObq3() 
{
	// TODO: Add your control notification handler code here
	lto ^= IS_OBQ3;
	
}

void CSysGain::OnCKtran() 
{
	// TODO: Add your control notification handler code here
	lto ^=IS_TRAN;
}
void CSysGain::KillMe()
	{
	// Public access to OnCancel
	// Add to every modeless dialog
	OnCancel();
	}

void CSysGain::OnCancel() 
{
	// TODO: Add extra cleanup here
	Save_Pos();
	CDialog::OnCancel();
	CDialog::DestroyWindow();

}

void CSysGain::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
if (! bSGain) Save_Pos();
}
