// IpxStat.cpp : implementation file
//


#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

//#include "IpxStat.h"	included in tscandlg.h

#include "Extern.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIpxStat dialog


CIpxStat::CIpxStat(CWnd* pParent /*=NULL*/)
	: CDialog(CIpxStat::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIpxStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIpxStat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIpxStat)
	DDX_Control(pDX, IDC_EN_IPX, m_ceIpxStats);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIpxStat, CDialog)
	//{{AFX_MSG_MAP(CIpxStat)
	ON_BN_CLICKED(IDC_BN_STATS, OnBnStats)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIpxStat message handlers

BOOL CIpxStat::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	OnBnStats()	;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CIpxStat::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}


void CIpxStat::KillMe() 
	{
	// Public access to OnCancel
	OnCancel();
	}



void CIpxStat::OnCancel() 
	{
	// TODO: Add extra cleanup here
	// Need this to get to PostNcDestroy which nulls ptr to the routine
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	
	}

void CIpxStat::OnOK() 
	{
	// TODO: Add extra validation here
	CDialog::OnOK();
	CDialog::DestroyWindow();
	
	}

void CIpxStat::OnBnStats()
	{ 
#if 0
	CString s,t;
	NET_STATS Ipx;
	pCTscanDlg->GetIpxStats(&Ipx);
	s.Format(_T("Data Packets =  %8ld    Cmnd Packets =  %8ld\r\n",
			Ipx.TotalRcvPackets, Ipx.TotalSentPackets );
	t.Format(_T("Lost Packets =  %8ld    Cmnd Lost    =  %8ld\r\n",
			Ipx.LostRcvPackets , Ipx.LostSentPackets);
	s += t;
	t.Format(_T("Dup  Packets =  %8ld    Cmnd Dup     =  %8ld",
			Ipx.DupRcvPackets, Ipx.DupSentPackets);
	s += t;
	m_ceIpxStats.SetWindowText(s);
#endif
	}
