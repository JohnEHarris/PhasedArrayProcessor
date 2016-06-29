// StrpChnl.cpp : implementation file
// Allow the operator to specify the channel being displayed on the strip charts
// Includes the option ALL
// 5/23/2002
//


#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

#include "Extern.h"

//#include "StrpChnl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStrpChnl dialog


CStrpChnl::CStrpChnl(CWnd* pParent /*=NULL*/)
	: CDialog(CStrpChnl::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStrpChnl)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CStrpChnl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStrpChnl)
	DDX_Control(pDX, IDC_CB_WALL, m_cbWall);
	DDX_Control(pDX, IDC_CB_TRAN, m_cbTran);
	DDX_Control(pDX, IDC_CB_OBQ3, m_cbObq3);
	DDX_Control(pDX, IDC_CB_OBQ2, m_cbObq2);
	DDX_Control(pDX, IDC_CB_OBQ1, m_cbObq1);
	DDX_Control(pDX, IDC_CB_LONG, m_cbLong);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStrpChnl, CDialog)
	//{{AFX_MSG_MAP(CStrpChnl)
	ON_CBN_SELCHANGE(IDC_CB_LONG, OnSelchangeCbLong)
	ON_CBN_SELCHANGE(IDC_CB_OBQ1, OnSelchangeCbObq1)
	ON_CBN_SELCHANGE(IDC_CB_OBQ2, OnSelchangeCbObq2)
	ON_CBN_SELCHANGE(IDC_CB_OBQ3, OnSelchangeCbObq3)
	ON_CBN_SELCHANGE(IDC_CB_TRAN, OnSelchangeCbTran)
	ON_CBN_SELCHANGE(IDC_CB_WALL, OnSelchangeCbWall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStrpChnl message handlers

void CStrpChnl::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();

	// Same code for all windows
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}



BOOL CStrpChnl::OnInitDialog() 
	{

	int  i, si, ci, nCh;
	int nSel, nLine;	// what line in combo box and which chnl selected
	CString s;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// Init Wall Selections
	m_cbWall.ResetContent();
	s = _T("ALL");	// 1st entry (line) is all chnls
	m_cbWall.AddString(s);
	// nCh = 0-39 or 0xff for all
	nCh = ConfigRec.UtRec.DisplayChannel[0];		// Channel last selected and saved in config rec
	nSel = 0;		// assume all selected
	nLine = 1;
	// Add lines to combo box
	for ( si = 0; si < 4; si++)
		{
		for ( ci = 0; ci < 10; ci++)
			{
			if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == IS_WALL)
				{
				i = 10*si + ci;	// chnl number
				if (i == nCh) nSel = nLine;
				s.Format(_T("Ch%02d"),i+1);
				m_cbWall.AddString(s);
				nLine++;
				}
			}
		}

	m_cbWall.SetCurSel(nSel);
	// Init Wall Selections


	// Init Long Selections
	m_cbLong.ResetContent();
	s = _T("ALL");	// 1st entry is all chnls
	m_cbLong.AddString(s);
	nCh = ConfigRec.UtRec.DisplayChannel[1];		// Channel last selected and save in config rec
	nSel = 0;		// assume all selected
	nLine = 1;
	for ( si = 0; si < 4; si++)
		{
		for ( ci = 0; ci < 10; ci++)
			{
			if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == IS_LONG)
				{
				i = 10*si + ci;	// chnl number
				if (i == nCh) nSel = nLine;
				s.Format(_T("Ch%02d"),i+1);
				m_cbLong.AddString(s);
				nLine++;
				}
			}
		}
	m_cbLong.SetCurSel(nSel);
	// Init Long Selections

	// Init Tran Selections
	m_cbTran.ResetContent();
	s = _T("ALL");	// 1st entry is all chnls
	m_cbTran.AddString(s);
	nCh = ConfigRec.UtRec.DisplayChannel[2];		// Channel last selected and save in config rec
	nSel = 0;		// assume all selected
	nLine = 1;
	for ( si = 0; si < 4; si++)
		{
		for ( ci = 0; ci < 10; ci++)
			{
			if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == IS_TRAN)
				{
				i = 10*si + ci;	// chnl number
				if (i == nCh) nSel = nLine;
				s.Format(_T("Ch%02d"),i+1);
				m_cbTran.AddString(s);
				nLine++;
				}
			}
		}
	m_cbTran.SetCurSel(nSel);
	// Init Tran Selections

	// Init Obq1 Selections
	m_cbObq1.ResetContent();
	s = _T("ALL");	// 1st entry is all chnls
	m_cbObq1.AddString(s);
	nCh = ConfigRec.UtRec.DisplayChannel[3];		// Channel last selected and save in config rec
	nSel = 0;		// assume all selected
	nLine = 1;
	for ( si = 0; si < 4; si++)
		{
		for ( ci = 0; ci < 10; ci++)
			{
			if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == IS_OBQ1)
				{
				i = 10*si + ci;	// chnl number
				if (i == nCh) nSel = nLine;
				s.Format(_T("Ch%02d"),i+1);
				m_cbObq1.AddString(s);
				nLine++;
				}
			}
		}
	m_cbObq1.SetCurSel(nSel);
	// Init Obq1 Selections

	// Init Obq2 Selections
	m_cbObq2.ResetContent();
	s = _T("ALL");	// 1st entry is all chnls
	m_cbObq2.AddString(s);
	nCh = ConfigRec.UtRec.DisplayChannel[4];		// Channel last selected and save in config rec
	nSel = 0;		// assume all selected
	nLine = 1;
	for ( si = 0; si < 4; si++)
		{
		for ( ci = 0; ci < 10; ci++)
			{
			if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == IS_OBQ2)
				{
				i = 10*si + ci;	// chnl number
				if (i == nCh) nSel = nLine;
				s.Format(_T("Ch%02d"),i+1);
				m_cbObq2.AddString(s);
				nLine++;
				}
			}
		}
	m_cbObq2.SetCurSel(nSel);
	// Init Obq2 Selections

	// Init Obq3 Selections
	m_cbObq3.ResetContent();
	s = _T("ALL");	// 1st entry is all chnls
	m_cbObq3.AddString(s);
	nCh = ConfigRec.UtRec.DisplayChannel[5];		// Channel last selected and save in config rec
	nSel = 0;		// assume all selected
	nLine = 1;
	for ( si = 0; si < 4; si++)
		{
		for ( ci = 0; ci < 10; ci++)
			{
			if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == IS_OBQ3)
				{
				i = 10*si + ci;	// chnl number
				if (i == nCh) nSel = nLine;
				s.Format(_T("Ch%02d"),i+1);
				m_cbObq3.AddString(s);
				nLine++;
				}
			}
		}
	m_cbObq3.SetCurSel(nSel);
	// Init Obq3 Selections


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CStrpChnl::OnCancel() 
	{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	// Activate or deactivate chnls to effect chart display
	// If Chnl 1 is displayed on Long, all other long chnls are turned off
	// SendMsg(SET_CHNLS_DISPLAYED);
	
	}

void CStrpChnl::OnOK() 
	{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

BOOL CStrpChnl::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in Tscan dlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}

void CStrpChnl::KillMe()
	{
	// Public access to OnCancel
	OnCancel();
	}

void CStrpChnl::OnSelchangeCbWall() 
	{
	// TODO: Add your control notification handler code here
	int i;
	char  s[20];
	CString t;

	i = m_cbWall.GetCurSel();

	if ( i == 0)	TurnOnAll(IS_WALL);	// ALL selected
	else
		{
		TurnOffAll(IS_WALL);
		m_cbWall.GetLBText(i, t);
		CstringToChar(t,s);
		i = GetChnlNumber(s);
		ConfigRec.UtRec.Shoe[i/10].Ch[i%10].On = 1;	// Turn on this one chnl
		ConfigRec.UtRec.DisplayChannel[0] = i;		// wall = chnl i
		}
	
	}

void CStrpChnl::OnSelchangeCbLong() 
	{
	// TODO: Add your control notification handler code here
	int i;
	char  s[20];
	CString t;

	i = m_cbLong.GetCurSel();

	if ( i == 0) TurnOnAll(IS_LONG);	// ALL selected
	else
		{
		TurnOffAll(IS_LONG);
		m_cbLong.GetLBText(i, t);
		CstringToChar(t,s);
		i = GetChnlNumber(s);
		ConfigRec.UtRec.Shoe[i/10].Ch[i%10].On = 1;	// Turn on this one chnl
		ConfigRec.UtRec.DisplayChannel[1] = i;		// wall = chnl i
		}
	
	
	}

void CStrpChnl::OnSelchangeCbTran() 
	{
	// TODO: Add your control notification handler code here
	int i;
	char  s[20];
	CString t;

	i = m_cbTran.GetCurSel();

	if ( i == 0) TurnOnAll(IS_TRAN);	// ALL selected
	else
		{
		TurnOffAll(IS_TRAN);
		m_cbTran.GetLBText(i, t);
		CstringToChar(t,s);
		i = GetChnlNumber(s);
		ConfigRec.UtRec.Shoe[i/10].Ch[i%10].On = 1;	// Turn on this one chnl
		ConfigRec.UtRec.DisplayChannel[2] = i;		// tran = chnl i
		}
	
	
	}

void CStrpChnl::OnSelchangeCbObq1() 
	{
	// TODO: Add your control notification handler code here
	int i;
	char  s[20];
	CString t;

	i = m_cbObq1.GetCurSel();

	if ( i == 0) TurnOnAll(IS_OBQ1);	// ALL selected
	else
		{
		TurnOffAll(IS_OBQ1);
		m_cbObq1.GetLBText(i, t);
		i = GetChnlNumber(s);
		ConfigRec.UtRec.Shoe[i/10].Ch[i%10].On = 1;	// Turn on this one chnl
		ConfigRec.UtRec.DisplayChannel[3] = i;		// obq1 = chnl i
		}
	
	
	}

void CStrpChnl::OnSelchangeCbObq2() 
	{
	// TODO: Add your control notification handler code here
	int i;
	char  s[20];
	CString t;

	i = m_cbObq2.GetCurSel();

	if ( i == 0) TurnOnAll(IS_OBQ2);	// ALL selected
	else
		{
		TurnOffAll(IS_OBQ2);
		m_cbObq2.GetLBText(i, t);
		CstringToChar(t,s);
		i = GetChnlNumber(s);
		ConfigRec.UtRec.Shoe[i/10].Ch[i%10].On = 1;	// Turn on this one chnl
		ConfigRec.UtRec.DisplayChannel[4] = i;		// obq2 = chnl i
		}
	
	
	}

void CStrpChnl::OnSelchangeCbObq3() 
	{
	// TODO: Add your control notification handler code here
	
	int i;
	char  s[20];
	CString t;

	i = m_cbObq3.GetCurSel();

	if ( i == 0) TurnOnAll(IS_OBQ3);	// ALL selected
	else
		{
		TurnOffAll(IS_OBQ3);
		m_cbObq3.GetLBText(i, t);
		CstringToChar(t,s);
		i = GetChnlNumber(s);
		ConfigRec.UtRec.Shoe[i/10].Ch[i%10].On = 1;	// Turn on this one chnl
		ConfigRec.UtRec.DisplayChannel[5] = i;		// Obq3 = chnl i
		}
	
	}

void CStrpChnl::TurnOnAll(int ChnlType)
	{
	// Turn ON all channels of type ChnlType (IS_WALL, IS_LONG, etc )
	int si, ci, i;

	for ( si = 0; si < 4; si++)
		for ( ci = 0; ci < 10; ci++)
			if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == ChnlType)
				ConfigRec.UtRec.Shoe[si].Ch[ci].On = 1;

	// Record in config file what is being displayed on this trace
	switch (ChnlType)
		{
	case IS_NOTHING:
	default:			i = 11;				break;

	case IS_WALL:		i = 0;				break;
	case IS_LONG:		i = 1;				break;
	case IS_TRAN:		i = 2;				break;
	case IS_OBQ1:		i = 3;				break;
	case IS_OBQ2:		i = 4;				break;
	case IS_OBQ3:		i = 5;				break;		
		}

	ConfigRec.UtRec.DisplayChannel[i] = 0xff;		// all for this type
	}

void CStrpChnl::TurnOffAll(int ChnlType)
	{
	// Turn OFF all channels of type ChnlType (IS_WALL, IS_LONG, etc )
	int si, ci;

	for ( si = 0; si < 4; si++)
		for ( ci = 0; ci < 10; ci++)
			if (ConfigRec.UtRec.Shoe[si].Ch[ci].Type == ChnlType)
				ConfigRec.UtRec.Shoe[si].Ch[ci].On = 0;

	}

int CStrpChnl::GetChnlNumber(char *s)
	{
	// pick the chan number of the the text string of a combo box
	// Combo string is 1-40 based, storage is 0-39 based

	char buf[64], *p;
	int i;
	if ( strlen(s) > 60)
		{
		TRACE("CStrpChnl::GetChnlNumber(S) s is too big\n ");
		return 0;
		}

	strcpy(buf, s);
	p = strstr(buf,"Ch");
	sscanf(&p[2], "%d", &i);	// human form 1-40
	if ( i ) 	return i-1;
	return 0;
	}
