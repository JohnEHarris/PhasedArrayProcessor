// WCalDlg.cpp : implementation file
// 11/18/00 Allow each shoe to have own offset but common slope
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"


#include "Extern.h"

// #include "WCalDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPtrList g_pTcpListUtData;  /* linked list holding the IData */
extern CCriticalSection g_CriticalSection;  /* Critical section for mutual exclusion access of g_pTcpListUtData */

/////////////////////////////////////////////////////////////////////////////
// CWCalDlg dialog

CWCalDlg::CWCalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWCalDlg::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CWCalDlg)
	m_nOffset = 0;
	m_bShoe1 = FALSE;
	m_bShoe2 = FALSE;
	m_bShoe3 = FALSE;
	m_bShoe4 = FALSE;
	m_bShoe0 = FALSE;
	m_bShoe5 = FALSE;
	m_bShoe6 = FALSE;
	m_bShoe7 = FALSE;
	m_bShoe8 = FALSE;
	m_bShoe9 = FALSE;
	m_nThnXdcr = 0;
	m_nThkXdcr = 0;
	m_nThkRef = 0.0f;
	m_nThnRef = 0.0f;
	//}}AFX_DATA_INIT

	m_pUndo = new UT_REC;
	memcpy ( (void *) m_pUndo, (void *) &ConfigRec.UtRec, 
				sizeof(UT_REC));
	}


void CWCalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWCalDlg)
	DDX_Control(pDX, IDC_CB_XDCR, m_cbXdcr);
	DDX_Control(pDX, IDC_SB_OFFSET, m_sbOffset);
	DDX_Text(pDX, IDC_EN_OFFSET, m_nOffset);
	DDV_MinMaxInt(pDX, m_nOffset, -20, 20);
	DDX_Text(pDX, IDC_EN_SLOPE, m_fSlope);
	DDV_MinMaxFloat(pDX, m_fSlope, -20.f, 20.f);
	DDX_Check(pDX, IDC_CK_USEWITH1, m_bShoe1);
	DDX_Check(pDX, IDC_CK_USEWITH2, m_bShoe2);
	DDX_Check(pDX, IDC_CK_USEWITH3, m_bShoe3);
	DDX_Check(pDX, IDC_CK_USEWITH4, m_bShoe4);
	DDX_Check(pDX, IDC_CK_USEWITH10, m_bShoe0);
	DDX_Check(pDX, IDC_CK_USEWITH5, m_bShoe5);
	DDX_Check(pDX, IDC_CK_USEWITH6, m_bShoe6);
	DDX_Check(pDX, IDC_CK_USEWITH7, m_bShoe7);
	DDX_Check(pDX, IDC_CK_USEWITH8, m_bShoe8);
	DDX_Check(pDX, IDC_CK_USEWITH9, m_bShoe9);
	DDX_Text(pDX, IDC_EN_THK_REF, m_nThkRef);
	DDV_MinMaxFloat(pDX, m_nThkRef, 0.f, 4000.f);
	DDX_Text(pDX, IDC_EN_THN_REF, m_nThnRef);
	DDV_MinMaxFloat(pDX, m_nThnRef, 0.f, 4000.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWCalDlg, CDialog)
	//{{AFX_MSG_MAP(CWCalDlg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BN_COMPUTE, OnBnCompute)
	ON_BN_CLICKED(IDC_BT_THICK, OnBtThick)
	ON_BN_CLICKED(IDC_BT_THIN, OnBtThin)
	ON_BN_CLICKED(IDC_BN_INIT, OnBnInit)
	ON_BN_CLICKED(IDC_BN_UNDO, OnBnUndo)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_CB_XDCR, OnSelchangeCbXdcr)
	ON_EN_CHANGE(IDC_EN_THK_REF, OnChangeEnThkRef)
	ON_EN_CHANGE(IDC_EN_THN_REF, OnChangeEnThnRef)
	ON_BN_CLICKED(IDC_BN_SHEAR, OnBnShear)
	ON_BN_CLICKED(IDC_BN_COMPRESS, OnBnCompress)
	ON_BN_CLICKED(IDC_CK_USEWITH1, OnCkUsewith1)
	ON_BN_CLICKED(IDC_CK_USEWITH2, OnCkUsewith2)
	ON_BN_CLICKED(IDC_CK_USEWITH3, OnCkUsewith3)
	ON_BN_CLICKED(IDC_CK_USEWITH4, OnCkUsewith4)
	ON_BN_CLICKED(IDC_BSet, OnBSet)
	ON_BN_CLICKED(IDC_CK_USEWITH5, OnCkUsewith5)
	ON_BN_CLICKED(IDC_CK_USEWITH6, OnCkUsewith6)
	ON_BN_CLICKED(IDC_CK_USEWITH7, OnCkUsewith7)
	ON_BN_CLICKED(IDC_CK_USEWITH8, OnCkUsewith8)
	ON_BN_CLICKED(IDC_CK_USEWITH9, OnCkUsewith9)
	ON_BN_CLICKED(IDC_CK_USEWITH10, OnCkUsewith10)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWCalDlg message handlers

BOOL CWCalDlg::OnInitDialog() 
	{
	int si, ci, i, j;
	CString s;

	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	m_bShoe1 = m_bShoe2 =m_bShoe3 =m_bShoe4 =m_bShoe5 = TRUE;
	m_bShoe6 = m_bShoe7 =m_bShoe8 =m_bShoe9 =m_bShoe0 = TRUE;

	GetConfigData();

	m_sbOffset.SetScrollRange(-20,20,TRUE);
	m_sbOffset.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbOffset.SetScrollPos(m_nOffset, TRUE);

	m_nAcquireSel = 0xff;	//neither
	m_nAcquireSum = 0.0f;
	m_nAcquireCnt = 0;

	// Liberally copied from Setup.cpp for combo box init
//	m_nCalChnl = 0xff;	// none selected yet

	m_cbXdcr.ResetContent();
	j = 0;
	for ( si = 0; si < MAX_CHANNEL/10; si++)
		{	// LOOP thru shoes
		for ( ci = 0; ci < 10; ci++)
			{
			switch (ConfigRec.UtRec.Shoe[si].Ch[ci].Type)
				{
			case IS_NOTHING:
			default:
				i = 0;
				break;

			case IS_WALL:
				i = 1;
#if 0
				// pick the 1st wall chnl as the cal chnl
				if ( m_nCalChnl == 0xff)
					m_nCalChnl = 10*si + ci;	// found 1st wall chnl
#endif
				break;

			case IS_LONG:
				i = 2;
				break;

			case IS_TRAN:
				i = 3;
				break;

			case IS_OBQ1:
				i = 4;
				break;
			case IS_OBQ2:
				i = 5;
				break;
			case IS_OBQ3:
				i = 6;
				break;
			case IS_LAM:
				i = 7;
				break;

				}
			if ( i > XDCR_TYPE) i = 0;
			s.Format(_T("S%1d:Ch%1d  %s"), si+1, ci+1, XdcrText[i]);
			m_cbXdcr.AddString(s);
			m_MapIndexToChnl[j++] = 10*si + ci;	// index j pts to chnl 10si+ci
			}
		if ( si < 9)
			{
			s = _T("===========");
			m_cbXdcr.AddString(s);
			m_MapIndexToChnl[j++] = 0xff;	// index j pts separator line
			}
		}	// LOOP thru shoes

	if ( m_nCalChnl < MEM_MAX_CHANNEL) 
		{
		si = m_nCalChnl/10;
		ci = m_nCalChnl % 10;
		m_cbXdcr.SetCurSel(11*si + ci);
		}


	if (ConfigRec.bEnglishMetric & 1) //metric
		s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity*25.4f);
	else
		s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity);
	GetDlgItem(IDC_BN_COMPRESS)->SetWindowText(s);
	
	if (ConfigRec.bEnglishMetric & 1) //metric
		s.Format(_T("Shear = %7.4f"), ConfigRec.UtRec.ShearVelocity*25.4f);
	else
		s.Format(_T("Shear = %7.4f"), ConfigRec.UtRec.ShearVelocity);
	GetDlgItem(IDC_BN_SHEAR)->SetWindowText(s);

	UpdateData(FALSE);	// copy data to screen

	// Make sure in Calibration mode and not inspect mode
	SendMsg(CAL_MODE);
	SendMsg(SET_WALL_COEFS_RAW);

	m_uIpxTimer = 0;
	StartTimer();

	m_pTcpThreadRxList->SendSlaveMsg(CAL_MODE,0,0,0,0,0,0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CWCalDlg::OnCancel() 
	{
	// TODO: Add extra cleanup here
	m_pTcpThreadRxList->SendSlaveMsg(NOP_MODE,0,0,0,0,0,0);

	StopTimer();
	SendMsg(SET_WALL_COEFS);
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CWCalDlg::OnOK() 
	{
	// TODO: Add extra validation here
	// Send slope and offset msg to instrument
	m_pTcpThreadRxList->SendSlaveMsg(NOP_MODE,0,0,0,0,0,0);

	StopTimer();
	pCTscanDlg->FileSaveAs();
	SendMsg(SET_WALL_COEFS);
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

void CWCalDlg::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	// Same code for all windows
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	// Tholds can be modal or modeless dialog
	// if modeless destroy this
	if (m_pDlg)
		{
		m_pDlg = NULL;
		delete this;
		}
	}


void CWCalDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{

	// TODO: Add your message handler code here and/or call default
	int nStart, nDelta;		// where the bar started and amount of change
	int nResult;			// normally start + delta
	int nMax, nMin;
	int nCtlId;
	int si;		// which shoe being calibrated



	nStart = pScrollBar->GetScrollPos();
	pScrollBar->GetScrollRange(&nMin, &nMax);
	si = m_nCalChnl/10;
	si %= 10;		// make sure 0-3

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
	case IDC_SB_OFFSET:
		m_nOffset = m_nOffsetNew[si] = ConfigRec.UtRec.WallOffset[si] = nResult;
		break;
	
	
	default:
		break;
		}

		
	pScrollBar->SetScrollPos(nResult, TRUE);

	UpdateData(FALSE);	// copy data to screen
	SetConfigData();
	SendMsg(SET_WALL_COEFS);

	}
	

void CWCalDlg::OnBnCompute() 
	{
	// TODO: Add your control notification handler code here
	float r1, r2, w1, w2;	// wall reading counts, wall ref in thds
	// w = m*r + b;
	int si;

	si = m_nCalChnl/10;
	si %= 10;		// make sure 0-3

	if ( m_nThkXdcr == m_nThnXdcr)
	{
		MessageBox(	_T("Thick = Thin not allowed"),
			_T("Calibration Error"));
		return;
	}

	r1 = (float) m_nThkXdcr;
	w1 = (float) m_nThkRef;

	r2 = (float) m_nThnXdcr;
	w2 = (float) m_nThnRef;

	m_fSlope = m_fSlopeNew[si] = (w1-w2)/(r1-r2);
	m_nOffset = m_nOffsetNew[si] = (int)(w1 - m_fSlope*r1);

	if (ConfigRec.bEnglishMetric & 1)  //metric
		m_nOffset = (int) ((float) m_nOffset / 25.4f);

	m_nAcquireSel = 0xff;	// neither

	// update the compression velocity
	CString s;
	if (m_fSlope != 0.0f)
	{
		ConfigRec.UtRec.CompVelocity = 0.232f/m_fSlope;
		if ( ConfigRec.bEnglishMetric & 1 )  // Metric
		{
			s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity * 25.4f);
			GetDlgItem(IDC_BN_COMPRESS)->SetWindowText(s);
		}
		else
		{
			s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity);
			GetDlgItem(IDC_BN_COMPRESS)->SetWindowText(s);
		}
	}
	else
	{
		MessageBox(	_T("Slope can not be zero."),
			_T("Calibration Error"));
		return;
	}

	// Copy coef's to config structure.  each slave has own coef's
	SetConfigData();
	SendMsg(SET_WALL_COEFS);
	UpdateData(FALSE);	// copy data to screen
	}

void CWCalDlg::OnBtThick() 
	{
	// TODO: Add your control notification handler code here
	// Acquire  instrument reading associated with thick cal ref
	m_nAcquireSel = 0;
	m_nAcquireSum = 0.0f;
	m_nAcquireCnt = 0;
	SendMsg(SET_WALL_COEFS_RAW);
	UpdateData(TRUE);	// copy screen to data
	}

void CWCalDlg::OnBtThin() 
	{
	// TODO: Add your control notification handler code here
	// Acquire  instrument reading associated with thin cal ref
	m_nAcquireSel = 1;
	m_nAcquireSum = 0.0f;
	m_nAcquireCnt = 0;
	SendMsg(SET_WALL_COEFS_RAW);
	UpdateData(TRUE);	// copy screen to data
	}

void CWCalDlg::KillMe()
	{
	// Public access to OnCancel
	OnCancel();
	}

BOOL CWCalDlg::GetConfigData()
	{
	int si, ci, i;
	m_nCalChnl = gChannel;
	CString str, s;

	si = m_nCalChnl / 10;	//shoe
	si %= 10;
	ci = m_nCalChnl % 10;	//chnl

	if ( ConfigRec.bEnglishMetric & 1 )  // Metric
	{
		s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity * 25.4f);
		GetDlgItem(IDC_BN_COMPRESS)->SetWindowText(s);
		
		s.Format(_T("Shear = %7.4f"), ConfigRec.UtRec.ShearVelocity * 25.4f);
		GetDlgItem(IDC_BN_SHEAR)->SetWindowText(s);

		m_nThkRef = (float) ConfigRec.UtRec.WallThkRef * 25.4f / 1000.0f;
		m_nThnRef = (float) ConfigRec.UtRec.WallThnRef * 25.4f / 1000.0f;
		str.Format(_T("Reference:\r\n(mm)"));
		SetDlgItemText(IDC_STATIC_THICK, str);
		SetDlgItemText(IDC_STATIC_THIN, str);
		str.Format(_T("Transducer:\r\n(mm)"));
		SetDlgItemText(IDC_STATIC_TRANSDUCER, str);
	}
	else  //  English
	{
		s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity);
		GetDlgItem(IDC_BN_COMPRESS)->SetWindowText(s);

		s.Format(_T("Shear = %7.4f"), ConfigRec.UtRec.ShearVelocity);
		GetDlgItem(IDC_BN_SHEAR)->SetWindowText(s);

		m_nThkRef = ConfigRec.UtRec.WallThkRef;
		m_nThnRef = ConfigRec.UtRec.WallThnRef;
		str.Format(_T("Reference:\r\n(mils)"));
		SetDlgItemText(IDC_STATIC_THICK, str);
		SetDlgItemText(IDC_STATIC_THIN, str);
		str.Format(_T("Transducer:\r\n(mils)"));
		SetDlgItemText(IDC_STATIC_TRANSDUCER, str);
	}

	if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == IS_WALL)
		{
		m_fSlope = ConfigRec.UtRec.fWallSlope[si];
		if ( m_fSlope > 10.0f) m_fSlope = 1.0f;
		if ( m_fSlope < -10.0f) m_fSlope = 1.0f;

		ConfigRec.UtRec.fWallSlope[si] = m_fSlope;

		m_nOffset = ConfigRec.UtRec.WallOffset[si];
		if ( m_nOffset > 20) m_nOffset = 20;
		if ( m_nOffset < -20) m_nOffset = -20;

		ConfigRec.UtRec.WallOffset[si] = m_nOffset;

		for ( i = 0; i < MAX_SHOES; i++)
			{
			m_fSlopeNew[i] = ConfigRec.UtRec.fWallSlope[i];
			m_nOffsetNew[i] = ConfigRec.UtRec.WallOffset[i];
			}

		return TRUE;
		}

	else 
		{
		MessageBox(_T("Select channel is not a wall channel.\nPlease select a wall channel."),
			_T("Calibration Error"));
		return FALSE;
		}

	}

void CWCalDlg::SetConfigData()
	{
	if ( ConfigRec.bEnglishMetric & 1 )  // Metric
	{
		ConfigRec.UtRec.WallThkRef = (short) (m_nThkRef * 1000.0f / 25.4f);  // convert to thousandth of inch
		ConfigRec.UtRec.WallThnRef = (short) (m_nThnRef * 1000.0f / 25.4f);
		ConfigRec.UtRec.WallCalChnl = m_nCalChnl;
	}
	else
	{
		ConfigRec.UtRec.WallThkRef = (short) m_nThkRef;
		ConfigRec.UtRec.WallThnRef = (short) m_nThnRef;
		ConfigRec.UtRec.WallCalChnl = m_nCalChnl;
	}

	if ( m_bShoe1)
		{
		ConfigRec.UtRec.fWallSlope[0] = m_fSlopeNew[0] = m_fSlope;
		ConfigRec.UtRec.WallOffset[0] = m_nOffsetNew[0] = m_nOffset;
		}
	if ( m_bShoe2)
		{
		ConfigRec.UtRec.fWallSlope[1] = m_fSlopeNew[1] = m_fSlope;
		ConfigRec.UtRec.WallOffset[1] = m_nOffsetNew[1] = m_nOffset;
		}
	if ( m_bShoe3)
		{
		ConfigRec.UtRec.fWallSlope[2] = m_fSlopeNew[2] = m_fSlope;
		ConfigRec.UtRec.WallOffset[2] = m_nOffsetNew[2] = m_nOffset;
		}
	if ( m_bShoe4)
		{
		ConfigRec.UtRec.fWallSlope[3] = m_fSlopeNew[3] = m_fSlope;
		ConfigRec.UtRec.WallOffset[3] = m_nOffsetNew[3] = m_nOffset;
		}
	if ( m_bShoe5)
		{
		ConfigRec.UtRec.fWallSlope[4] = m_fSlopeNew[4] = m_fSlope;
		ConfigRec.UtRec.WallOffset[4] = m_nOffsetNew[4] = m_nOffset;
		}
	if ( m_bShoe6)
		{
		ConfigRec.UtRec.fWallSlope[5] = m_fSlopeNew[5] = m_fSlope;
		ConfigRec.UtRec.WallOffset[5] = m_nOffsetNew[5] = m_nOffset;
		}
	if ( m_bShoe7)
		{
		ConfigRec.UtRec.fWallSlope[6] = m_fSlopeNew[6] = m_fSlope;
		ConfigRec.UtRec.WallOffset[6] = m_nOffsetNew[6] = m_nOffset;
		}
	if ( m_bShoe8)
		{
		ConfigRec.UtRec.fWallSlope[7] = m_fSlopeNew[7] = m_fSlope;
		ConfigRec.UtRec.WallOffset[7] = m_nOffsetNew[7] = m_nOffset;
		}
	if ( m_bShoe9)
		{
		ConfigRec.UtRec.fWallSlope[8] = m_fSlopeNew[8] = m_fSlope;
		ConfigRec.UtRec.WallOffset[8] = m_nOffsetNew[8] = m_nOffset;
		}
	if ( m_bShoe0)
		{
		ConfigRec.UtRec.fWallSlope[9] = m_fSlopeNew[9] = m_fSlope;
		ConfigRec.UtRec.WallOffset[9] = m_nOffsetNew[9] = m_nOffset;
		}
	}

void CWCalDlg::OnBnInit() 
	{
	// TODO: Add your control notification handler code here
	// Must set slope to 1.0 and offset to 0 before computing
	// New values for slope and offset.  Ie., must have known
	// starting point
	CString s;


	m_fSlope = 1.0f;
	m_nOffset = 0;

	SetConfigData();

	ConfigRec.UtRec.CompVelocity = 0.232f;
	ConfigRec.UtRec.ShearVelocity = 0.122f;

	UpdateData(FALSE);	// copy data to screen
	m_sbOffset.SetScrollPos(m_nOffset, TRUE);

	if ( ConfigRec.bEnglishMetric & 1 )  // Metric
	{
		s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity * 25.4f);
		GetDlgItem(IDC_BN_COMPRESS)->SetWindowText(s);
		
		s.Format(_T("Shear = %7.4f"), ConfigRec.UtRec.ShearVelocity * 25.4f);
		GetDlgItem(IDC_BN_SHEAR)->SetWindowText(s);
	}
	else
	{
		s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity);
		GetDlgItem(IDC_BN_COMPRESS)->SetWindowText(s);
	
		s.Format(_T("Shear = %7.4f"), ConfigRec.UtRec.ShearVelocity);
		GetDlgItem(IDC_BN_SHEAR)->SetWindowText(s);
	}

	SendMsg(SET_WALL_COEFS);
	}

BOOL CWCalDlg::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscan dlg window.
	//SetConfigData();	// Only msg from this module

	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}


void CWCalDlg::OnBnUndo() 
	{
	// TODO: Add your control notification handler code here

	int i, si, ci;

	if (MessageBox(	_T("Undo all wall coefficient changes since last SAVE operation?"),
			_T("Restore Configuration"),MB_YESNO)== IDYES)
		{
		// Only undo the wall cal coef's
		for ( i = 0; i < MAX_SHOES; i++)
			{
			ConfigRec.UtRec.fWallSlope[i] = m_pUndo->fWallSlope[i];
			ConfigRec.UtRec.WallOffset[i] = m_pUndo->WallOffset[i];
			}
		ConfigRec.UtRec.CompVelocity = m_pUndo->CompVelocity;
		ConfigRec.UtRec.ShearVelocity = m_pUndo->ShearVelocity;
		ConfigRec.UtRec.WallThkRef = m_pUndo->WallThkRef;
		ConfigRec.UtRec.WallThnRef = m_pUndo->WallThnRef;


		GetConfigData();
		SendMsg(SET_WALL_COEFS);
		}
	si = gChannel/10;
	ci = gChannel%10;
	i = si*11 + ci;
	m_cbXdcr.SetCurSel(i);
	UpdateData(FALSE);	// copy data to screen
	}


void CWCalDlg::UpdateUndoBuffer() 
{
	// TODO: Add your control notification handler code here

	int i;

	// Only undo the wall cal coef's
	for ( i = 0; i < MAX_SHOES; i++)
	{
		m_pUndo->fWallSlope[i] = ConfigRec.UtRec.fWallSlope[i];
		m_pUndo->WallOffset[i] = ConfigRec.UtRec.WallOffset[i];
	}

	m_pUndo->CompVelocity = ConfigRec.UtRec.CompVelocity;
	m_pUndo->ShearVelocity = ConfigRec.UtRec.ShearVelocity;
	m_pUndo->WallThkRef = ConfigRec.UtRec.WallThkRef;
	m_pUndo->WallThnRef = ConfigRec.UtRec.WallThnRef;
}


CWCalDlg::~CWCalDlg()
	{
	delete m_pUndo;
	}

void CWCalDlg::OnTimer(UINT nIDEvent)
	{ 
	// IPX timer to empty data messages from data msg queue
	CString s;

	I_MSG_CAL	*pCalMsg;
	int si, ci;
	WORD wRaw;		// raw or eu mode ?

	// copy calibration data from g_pListUtData to plistUtdata
	I_MSG_RUN *pImsgRun;
	g_CriticalSection.Lock();
	while (!g_pTcpListUtData.IsEmpty() )
	{
		pImsgRun = (I_MSG_RUN *) g_pTcpListUtData.RemoveHead();
		plistUtData.AddTail((void *) pImsgRun);
	}
	g_CriticalSection.Unlock();

	si = m_nCalChnl/10;
	si %= 10;
	ci = m_nCalChnl % 10;

	while (!plistUtData.IsEmpty() )
		{

		pCalMsg = (I_MSG_CAL *)plistUtData.RemoveHead();
		// Select the one and only Max wall channel being displayed

		// if receiving raw data, show in cnts box
		// if receiving eu data, show only in mils now box
		wRaw = pCalMsg->InspHdr.status[1] & WALL_STATUS_NOT_EU;
		if (wRaw)
			{	// raw tof counts
			m_nXdcr = (float) pCalMsg->Shoe[si].MaxWall[ci];
			m_nCurrentRawCnts = (float) pCalMsg->Shoe[si].MaxWall[ci];
			//SetDlgItemInt(IDC_EN_NOW,m_nCurrentRawCnts, FALSE);
			}
		else
			{	// Engineering Units, ie., 0.001 inch
			m_nXdcr = (float) pCalMsg->Shoe[si].MaxWall[ci];
			m_nCurrentRawCnts = (float) pCalMsg->Shoe[si].MaxWall[ci];
			//SetDlgItemInt(IDC_EN_XDCR,m_nXdcr, FALSE);
			}

		if ( ConfigRec.bEnglishMetric & 1 )  // Metric
		{
			m_nCurrentRawCnts = m_nXdcr = m_nXdcr * 25.4f / 1000.0f;
			s.Format(_T("%6.2f"), m_nXdcr);
		}
		else
			s.Format(_T("%d"), (int) m_nXdcr);

		SetDlgItemText(IDC_EN_NOW, s);

		switch ( m_nAcquireSel)
			{	// which aquire button pressed last
		case 0:
			// Thick
			m_nAcquireSum += m_nXdcr;
			m_nAcquireCnt++;
			if (m_nAcquireCnt >= 16)
			{
				//m_nAcquireSum += m_nAcquireCnt/2;
				m_nThkXdcr = m_nAcquireSum/m_nAcquireCnt;
				//SetDlgItemInt(IDC_EN_THK_XDCR,m_nThkXdcr, FALSE);
				if ( ConfigRec.bEnglishMetric & 1 )  // Metric
					s.Format(_T("%6.2f"), m_nThkXdcr);
				else
					s.Format(_T("%d"), (int) (m_nThkXdcr+0.5f));
				SetDlgItemText(IDC_EN_THK_XDCR, s);
				m_nAcquireSum = 0.0f;
				m_nAcquireCnt = 0;
				m_nAcquireSel = 0xff;
			}
			break;
	
		case 1:
			// Thin
			m_nAcquireSum += m_nXdcr;
			m_nAcquireCnt++;
			if (m_nAcquireCnt >= 16)
			{
				//m_nAcquireSum += m_nAcquireCnt/2;
				m_nThnXdcr = m_nAcquireSum/m_nAcquireCnt;
				//SetDlgItemInt(IDC_EN_THN_XDCR,m_nThnXdcr, FALSE);
				if ( ConfigRec.bEnglishMetric & 1 )  // Metric
					s.Format(_T("%6.2f"), m_nThnXdcr);
				else
					s.Format(_T("%d"), (int) (m_nThnXdcr+0.5f));
				SetDlgItemText(IDC_EN_THN_XDCR, s);
				m_nAcquireSum = 0.0f;
				m_nAcquireCnt = 0;
				m_nAcquireSel = 0xff;
			}
			break;
	
		default:
			// Neither
			break;
			}
	
		
		delete pCalMsg;

		}


	CDialog::OnTimer(nIDEvent);
	}

void CWCalDlg::OnSelchangeCbXdcr() 
{
	// TODO: Add your control notification handler code here
	// Taken from Setup.cpp

	int i, j, si, ci;

	i = m_cbXdcr.GetCurSel();
	j = m_MapIndexToChnl[i];
	si = (j/10 ) % 10;
	ci = j%10;

	if ((j != 0xff) && (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == IS_WALL) )
	{	// pointing to a chnl instead of the separator
		gChannel = j;
		SendMsg(CHANNEL_SELECT);
		pCTscanDlg->ChangeActiveChannel();
		ConfigRec.UtRec.WallCalChnl = m_nCalChnl = j;
		if (GetConfigData() )
		{
			m_nOffset = m_nOffsetNew[si] = ConfigRec.UtRec.WallOffset[si];
			m_sbOffset.SetScrollPos(m_nOffset, TRUE);
	
			UpdateData(FALSE);	// copy data to screen
			// SendMsg(SET_WALL_COEFS);

		}
	}
	else
	{
		MessageBox(_T("Select channel is not a wall channel.\nPlease select a wall channel."),
			_T("Calibration Error"));

		m_cbXdcr.SetCurSel( MapChnlToIndex(m_nCalChnl) );
	}

}


int CWCalDlg::MapChnlToIndex(int nChannel)
{
	int nIndex;

	nIndex = nChannel + nChannel/10;

	return nIndex;
}

	/* Timer routines from acal.cpp*/
void CWCalDlg::StopTimer()
	{
	if (m_uIpxTimer)
		{
		KillTimer(m_uIpxTimer);
		m_uIpxTimer = 0;
		}
	}

void CWCalDlg::StartTimer()
	{
	if (m_uIpxTimer)	return;	// already running

	m_uIpxTimer = SetTimer(IDT_IPX_TIMER, 150, NULL);
	if (!m_uIpxTimer) MessageBox(_T("Failed to start IPX timer"));
	}

void CWCalDlg::OnChangeEnThkRef() 
	{
	UpdateData(TRUE);	/* copy screen to data */
	}

void CWCalDlg::OnChangeEnThnRef() 
	{
	UpdateData(TRUE);	/* copy screen to data */
	}


void CWCalDlg::OnBnShear() 
	{
	// TODO: Add your control notification handler code here
	// Pop up shear velocity dialog.
	
	}

void CWCalDlg::OnBnCompress() 
	{
	// TODO: Add your control notification handler code here
	// Pop up compression velocity dialog.
	// Use operator entered velocity to adjust slope
	// Pop up a dialog to allow the operator to set the joint number
	// On closing, sends new joint number to instrument

	CCompV dlg;
	CString s;
	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
		{
		m_fSlope = 0.232f/ConfigRec.UtRec.CompVelocity;
		SetConfigData();
		SendMsg(SET_WALL_COEFS);
		if (ConfigRec.bEnglishMetric & 1) //metric
			s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity * 25.4f);
		else
			s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity);
		GetDlgItem(IDC_BN_COMPRESS)->SetWindowText(s);
		UpdateData(FALSE);	// copy data to screen
		}
	else if (nResponse == IDCANCEL)
		{
		return;
		}

	
	}

void CWCalDlg::OnCkUsewith1()  	{ 	UpdateData(TRUE);	/* copy screen to data */	}
	
void CWCalDlg::OnCkUsewith2()  	{ 	UpdateData(TRUE);	/* copy screen to data */	}

void CWCalDlg::OnCkUsewith3()  	{ 	UpdateData(TRUE);	/* copy screen to data */	}

void CWCalDlg::OnCkUsewith4()  	{ 	UpdateData(TRUE);	/* copy screen to data */	}



void CWCalDlg::OnBSet() 
{
	// TODO: Add your control notification handler code here
//		m_fSlope = ConfigRec.UtRec.fWallSlope[si];
	UpdateData(TRUE);	// copy data from screen

	SetConfigData();

	// update the compression velocity
	CString s;
	if (m_fSlope != 0.0f)
	{
		ConfigRec.UtRec.CompVelocity = 0.232f/m_fSlope;
		if ( ConfigRec.bEnglishMetric & 1 )  // Metric
		{
			s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity * 25.4f);
			GetDlgItem(IDC_BN_COMPRESS)->SetWindowText(s);
		}
		else
		{
			s.Format(_T("Comp = %7.4f"), ConfigRec.UtRec.CompVelocity);
			GetDlgItem(IDC_BN_COMPRESS)->SetWindowText(s);
		}
	}
	else
	{
		MessageBox(	_T("Slope can not be zero."),
			_T("Calibration Error"));
		return;
	}

	SendMsg(SET_WALL_COEFS);

}

void CWCalDlg::OnCkUsewith5() 
{
	// TODO: Add your control notification handler code here
UpdateData(TRUE);	
}

void CWCalDlg::OnCkUsewith6() 
{
	// TODO: Add your control notification handler code here
UpdateData(TRUE);	
}

void CWCalDlg::OnCkUsewith7() 
{
	// TODO: Add your control notification handler code here
UpdateData(TRUE);	
}

void CWCalDlg::OnCkUsewith8() 
{
	// TODO: Add your control notification handler code here
UpdateData(TRUE);	
}

void CWCalDlg::OnCkUsewith9() 
{
	// TODO: Add your control notification handler code here
UpdateData(TRUE);	
}

void CWCalDlg::OnCkUsewith10() 
{
	// TODO: Add your control notification handler code here
UpdateData(TRUE);	
}
