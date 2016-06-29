// Alarms.cpp : implementation file
//


#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

//#include "Alarms.h included in tscandlg.h

#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarms dialog


CAlarms::CAlarms(CWnd* pParent /*=NULL*/)
	: CDialog(CAlarms::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlarms)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAlarms::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarms)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlarms, CDialog)
	//{{AFX_MSG_MAP(CAlarms)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BN_RESET_INST, OnBnResetInst)
	//}}AFX_MSG_MAP

	// jeh manually added

	ON_CONTROL_RANGE(BN_CLICKED, IDC_CK_ALARM00, IDC_CK_ALARM0F, DoMaskCkBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarms message handlers

void CAlarms::OnCancel() 
	{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
	}

void CAlarms::OnOK() 
	{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
	}

HBRUSH CAlarms::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
	{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	// Intercept message to paint for ID/OD check boxes and 
	// Change color of text

	int nID, i;
	WORD wMask;

	if ( nCtlColor == CTLCOLOR_STATIC)
		{	// Is this one of the alarm dlg check boxes?
		nID = IDC_CK_ALARM00;	// starting point
		for ( i = 0; i < 16; i++)
			{
			if (pWnd->m_hWnd == GetDlgItem(i+nID)->m_hWnd)	
				{	// found a check box 
				wMask = (1 << i);	// which bit in mask
				if ( wMask & gwUdpStatus[1])		// [0] saved for legacy truscope
					{	// the alarm is set ON
					if ( wMask & wAlarmMask[0])
						{	// alarm is not masked OFF, show as RED
						pDC->SetTextColor(RGB(255,0,0));
						}
					else
						{	// alarm is ON but masked, set BLUE
						pDC->SetTextColor(RGB(0,0,255));
						}
					}	// alarm is set

				else
					{	// alarm is not set, show as black
					pDC->SetTextColor(RGB(0,0,0));
					}
				
				return hbr;	// finished with this windows command
				}	// found a check box 
			}	// for loop on i

		}	// Is this one of the alarm dlg check boxes?

#if 0
	else if ( nCtlColor == CTLCOLOR_BTN)
		{
		// Per Prosise, p376 this has no effect.  However, disabling the 
		// button preserves the white background
		if ( pWnd->m_hWnd == GetDlgItem(IDC_MAP)->m_hWnd)
			{	// Make Map button background white
			m_hWhiteBrush.CreateSolidBrush(RGB(255,255,255));
			return (HBRUSH) m_hWhiteBrush;
			}
		}
#endif
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
	}


BOOL CAlarms::OnInitDialog() 
	{

	int i, nID;
	WORD wMask;

	
	// TODO: Add extra initialization here
	// Only working on one alarm mask at present.  Space allocated for 4 masks.

	for ( i = 0; i < 16; i++)
		{	// set or clear check box.  Check means mask bit is 0
		nID = i + IDC_CK_ALARM00;	// must force control id's to be sequential
									// can do this by editing resource.h as last resort

		wMask = (1 << i);	// set appropriate bit

		if (wAlarmMask[0] & wMask)
			{	// alarm is masked ON, clear check box
			((CButton *) GetDlgItem(nID))->SetCheck(0);
			}
		else
			{	// alarm bit is masked OFF, set check box
			((CButton *) GetDlgItem(nID))->SetCheck(1);
			}
		}

	UpdateData(FALSE);	// Copy variables to screen
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CAlarms::DoMaskCkBox(UINT nID)
	{
	int i, j;
	WORD wMask;

	i = nID - IDC_CK_ALARM00;
	ASSERT( i >= 0 && i < 16);
	j = i;	//   /16;		// which word
	wMask = (1 << i);	// set appropriate bit in mask
	
	if ( ((CButton *) GetDlgItem(nID))->GetCheck())
		{	// check box is checked...mask out status bit
		wMask = ~wMask;		// invert word
		wAlarmMask[0] &= wMask;
		}
	else
		{	// button was unchecked or unmasked
		wAlarmMask[0] |= wMask;
		}
	// rewrite the text next to message to force redraw
//	if (i = GetDlgItemText(nID, s, 60) ) SetDlgItemText(nID, s);

	}




void CAlarms::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
	}

void CAlarms::OnBnRefresh() 
{
	// TODO: Add your control notification handler code here
	// Rewrite check box text in hopes of forcing color update
}

void CAlarms::OnBnResetInst() 
{
	// TODO: Add your control notification handler code here
/*	for ( int i = 0; i < 16; i++)
	{	
		wAlarmMask[i]=1;
	}
*/
	wAlarmMask[0] = 0xffff;
	OnInitDialog();	
}
