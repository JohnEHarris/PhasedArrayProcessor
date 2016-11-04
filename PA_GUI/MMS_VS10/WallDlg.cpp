// WallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
// include "JointNum.h"
#include "TscanDlg.h"

#include "Extern.h"
// #include "WallDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWallDlg dialog


CWallDlg::CWallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWallDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWallDlg)
	m_szNewWall = _T("");
	//}}AFX_DATA_INIT
}


void CWallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWallDlg)
	DDX_Text(pDX, IDC_NEW_WALL, m_szNewWall);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWallDlg, CDialog)
	//{{AFX_MSG_MAP(CWallDlg)
	ON_EN_CHANGE(IDC_NEW_WALL, OnChangeNewWall)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallDlg message handlers

void CWallDlg::OnChangeNewWall() 
	{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	GetDlgItemText(IDC_NEW_WALL,m_szNewWall);
	pCTscanDlg->m_szNewWall = m_szNewWall;
	
	}

BOOL CWallDlg::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	pCTscanDlg->m_szNewWall = m_szNewWall = pCTscanDlg->m_szStat_Wall;
	UpdateData(FALSE);	// Copy variables to screen
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CWallDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
	{
	// TODO: Add your message handler code here
	
	}

void CWallDlg::OnRButtonDown(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnRButtonDown(nFlags, point);
	}


void CWallDlg::OnSysCommand(UINT nID, LPARAM lParam)
	{
		CDialog::OnSysCommand(nID, lParam);
	}

