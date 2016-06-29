// Echo.cpp : implementation file
//

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"


#include "Extern.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEcho dialog


CEcho::CEcho(CWnd* pParent /*=NULL*/)
	: CDialog(CEcho::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CEcho)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_brRedBrush.CreateSolidBrush(RGB(255,0,0));
	m_brBlueBrush.CreateSolidBrush(RGB(0,0,255));
	}


void CEcho::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEcho)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEcho, CDialog)
	//{{AFX_MSG_MAP(CEcho)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEcho message handlers

void CEcho::OnCancel() 
	{
	// TODO: Add extra cleanup here
	WINDOWPLACEMENT wp;

	StopTimer();

	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition("XDCR_ACTIVITY_DLG", &wp);

	
	CDialog::OnCancel();
	CDialog::DestroyWindow();
	}

void CEcho::OnOK() 
	{
	// TODO: Add extra validation here
	WINDOWPLACEMENT wp;

	StopTimer();

	// Save closing location of window
	GetWindowPlacement(&wp);
	pCTscanDlg->SaveWindowLastPosition("XDCR_ACTIVITY_DLG", &wp);

	CDialog::OnOK();
	CDialog::DestroyWindow();
	}

BOOL CEcho::DestroyWindow() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DestroyWindow();
	}

void CEcho::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
	// Let the top level dialog TscanDlg know we are now dead
	CDialog::OnDestroy();
	m_pDlg = NULL;
	delete this;
	}

void CEcho::KillMe()
	{
	// Public access to OnCancel
	OnCancel();
	}

BOOL CEcho::OnInitDialog() 
	{
	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window

	CDialog::OnInitDialog();

	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	pCTscanDlg->GetWindowLastPosition("XDCR_ACTIVITY_DLG", &rect);
	if ( ( (rect.right - rect.left) >= dx ) &&
		 ( (rect.bottom - rect.top) >= dy ))
		{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
		}


	m_uEchoBitTimer = 0;
	StartTimer();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CEcho::OnTimer(UINT nIDEvent) 
	{
	// TODO: Add your message handler code here and/or call default
	// Update score board of xdcr activity every 3 seconds

	int si, ci;		// shoe and channel indices
	int nId;
	CString s,t;

	if (CInspect::m_pDlg) CInspect::m_pDlg->GetEchoBits(&m_Eb);
	for ( si = 0; si < MAX_SHOES; si++)	// shoes 0 & 2 only at Amelia
		{
		switch (si)
			{
		case 0:
			nId = IDC_xd_CH1;
			break;

		case 1:
			nId = IDC_xd_CH11;
			break;

		case 2:
			nId = IDC_xd_CH21;
			break;

		case 3:
			nId = IDC_xd_CH31;
			break;
		case 4:
			nId = IDC_xd_CH41;
			break;
		case 5:
			nId = IDC_xd_CH51;
			break;
		case 6:
			nId = IDC_xd_CH61;
			break;
		case 7:
			nId = IDC_xd_CH71;
			break;
		case 8:
			nId = IDC_xd_CH81;
			break;
		case 9:
			nId = IDC_xd_CH91;
			break;

			}


		for ( ci = 0; ci < 10; ci++)
			{
			// If od gate hit, display xdcr type on blue background
			// If id gate hit, red background
			// for no echo, white background
			switch (ConfigRec.UtRec.Shoe[si].Ch[ci].Type)
				{
			case IS_NOTHING:
			default:
				s = "N";
				break;

			case IS_WALL:
				s = "W";
//				SetDlgItemText(nId+ci, s);
//				goto Wexit;
				break;

			case IS_LONG:
				s = "L";
				break;

			case IS_TRAN:
				s = "T";
				break;

			case IS_OBQ1:
				s = "Q1";
				break;
			case IS_OBQ2:
				s = "Q2";
				break;
			case IS_OBQ3:
				s = "Q3";
				break;

			case IS_LAM:
				s = "Lm";
				break;
				}

			t.Format ("%01d - ", si*10+ci+1);
			t += s;
			SetDlgItemText(nId+ci, t);
			}
		}
	
//	GetClientRect(&m_Rect);
//	InvalidateRect( &m_Rect,TRUE );
Wexit:
	CDialog::OnTimer(nIDEvent);
	}

void CEcho::StartTimer()
	{	// helper function to start timer with one call
	if (m_uEchoBitTimer)	return;	// already running

	// 7 ticks per second
	m_uEchoBitTimer = SetTimer(IDT_ECHOBIT_TIMER, 300, NULL);
	if (!m_uEchoBitTimer) MessageBox("Failed to start EchoBit timer");
	}

void CEcho::StopTimer()
	{	// helper function to stop timer with one call
	if (m_uEchoBitTimer)
		{
		KillTimer(m_uEchoBitTimer);
		m_uEchoBitTimer = 0;
		}
	}


HBRUSH CEcho::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
	{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	HBRUSH hbrNew;
	int si, ci;		// shoe and channel indices
	int nId;
	
	// TODO: Change any attributes of the DC here
	// Intercept message to paint for ID/OD check boxes and 
	// Change color of text

//	if ( nCtlColor == CTLCOLOR_EDIT)
		{	// Edit Color
		hbrNew = hbr;	// remember original brush
		for ( si = 0; si < MAX_SHOES; si++)
			{
			switch (si)
				{
			case 0:
				nId = IDC_xd_CH1;
				break;

			case 1:
				nId = IDC_xd_CH11;
				break;

			case 2:
				nId = IDC_xd_CH21;
				break;

			case 3:
				nId = IDC_xd_CH31;
				break;
		case 4:
			nId = IDC_xd_CH41;
			break;
		case 5:
			nId = IDC_xd_CH51;
			break;
		case 6:
			nId = IDC_xd_CH61;
			break;
		case 7:
			nId = IDC_xd_CH71;
			break;
		case 8:
			nId = IDC_xd_CH81;
			break;
		case 9:
			nId = IDC_xd_CH91;
			break;

				}


			for ( ci = 0; ci < MAX_SHOES; ci++)
				{
				if ( m_Eb.Shoe[si] & (1 << ci) )
					{	// od gate saw echo on this channel
					if (pWnd->m_hWnd == GetDlgItem(nId+ci)->m_hWnd)
						{
						pDC->SetBkColor(RGB(0,0,255));	// Blue - Od
						pDC->SetTextColor(RGB(255,255,255));// White
						hbrNew = m_brBlueBrush;
						}
					}
				// Let Id echo over ride Od echo
#if 0
				if ( m_Eb.IdMask[si] & (1 << ci) )
					{	// od gate saw echo on this channel
					if (pWnd->m_hWnd == GetDlgItem(nId+ci)->m_hWnd)
						{
						pDC->SetBkColor(RGB(255,0,0));	// Red - Id
						pDC->SetTextColor(RGB(255,255,255));// White
						hbrNew = m_brRedBrush;
						}
					}
#endif
				}	// channel loop
			}	// shoe loop
		}	// Edit Color
	// TODO: Return a different brush if the default is not desired
	return hbrNew;
	}
