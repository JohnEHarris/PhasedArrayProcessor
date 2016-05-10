// AscanReadSeq.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "AscanReadSeq.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int gChannel;

/////////////////////////////////////////////////////////////////////////////
// CAscanReadSeq dialog


CAscanReadSeq::CAscanReadSeq(CWnd* pParent /*=NULL*/)
	: CDialog(CAscanReadSeq::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAscanReadSeq)
	m_nArrayNumber = 1;
	m_nSeqNumber = 0;
	//}}AFX_DATA_INIT
}


void CAscanReadSeq::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAscanReadSeq)
	DDX_Control(pDX, IDC_SCROLLBAR_ARRAY, m_sbArrayNumber);
	DDX_Control(pDX, IDC_SCROLLBAR_SEQ, m_sbSeqNumber);
	DDX_Text(pDX, IDC_EDIT_ARRAY, m_nArrayNumber);
	DDV_MinMaxInt(pDX, m_nArrayNumber, 1, 10);
	DDX_Text(pDX, IDC_EDIT_SEQ, m_nSeqNumber);
	DDV_MinMaxInt(pDX, m_nSeqNumber, 0, 63);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAscanReadSeq, CDialog)
	//{{AFX_MSG_MAP(CAscanReadSeq)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAscanReadSeq message handlers

void CAscanReadSeq::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_pTcpThreadRxList->SendSlaveMsg(SET_ASCAN_READ_SEQ, gChannel, 0, 0, 0, 0, 0);

	CDialog::OnCancel();
}

void CAscanReadSeq::OnOK() 
{
	// TODO: Add extra validation here
	m_pTcpThreadRxList->SendSlaveMsg(SET_ASCAN_READ_SEQ, gChannel, 0, 0, 0, 0, 0);
	
	CDialog::OnOK();
}

void CAscanReadSeq::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int nDelta;		// where the bar started and amount of change
	int nMax = 10;
	int nMin = 0;
	int nCtlId;
	int dummy = 5;
	CString s;
	short *nStart;
	nStart = (short *) &dummy;

	nCtlId = pScrollBar->GetDlgCtrlID();

	switch (nCtlId)
		{
	case IDC_SCROLLBAR_ARRAY:
		nMin=1;
		nMax=10;
		break;

	case IDC_SCROLLBAR_SEQ:
		nMin=0;
		nMax=63;
		break;

	default:
//		nStart = (short*)&dummy;
		break;
		}

	*nStart = pScrollBar->GetScrollPos();

	switch ( nSBCode)
		{	// switch on type of motion
	case SB_LINEUP:
		nDelta = -1;
		break;

	case SB_LINEDOWN:
		nDelta = 1;
		break;

	case SB_PAGEUP:
		nDelta = -5;
		break;

	case SB_PAGEDOWN:
		nDelta = 5;
		break;

	case SB_THUMBTRACK:
		return;
		nDelta = (int)nPos - *nStart;
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - *nStart;
		break;

	default:
		nDelta = 0;		//need a value 8-27-2001
		return;
		break;

		}	// switch on type of motion

	*nStart += nDelta;
	if ( *nStart > nMax) *nStart = nMax;
	if ( *nStart < nMin) *nStart = nMin;


	pScrollBar->SetScrollPos(*nStart, TRUE);


	switch (nCtlId)
		{
	case IDC_SCROLLBAR_ARRAY:
		m_nArrayNumber = *nStart;
		m_pTcpThreadRxList->SendSlaveMsg(SET_ASCAN_READ_BEAM, m_nArrayNumber-1, m_nSeqNumber, 0, 0, 0, 0);
		break;

	case IDC_SCROLLBAR_SEQ:
		m_nSeqNumber = *nStart;
		m_pTcpThreadRxList->SendSlaveMsg(SET_ASCAN_READ_BEAM, m_nArrayNumber-1, m_nSeqNumber, 0, 0, 0, 0);		

	default:
		break;
		}

	UpdateData(FALSE);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CAscanReadSeq::OnInitDialog() 
{
	CDialog::OnInitDialog();

	WINDOWPLACEMENT wp;
	RECT rect;
	int dx, dy;		// width and height of original window
	GetWindowPlacement(&wp);
	dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	rect.left = 100;
	rect.right = rect.left + dx;
	rect.top = 200;
	rect.bottom = rect.top + dy;
	if ( ( (rect.right - rect.left) >= dx ) &&
		 ( (rect.bottom - rect.top) >= dy ))
	{
		wp.rcNormalPosition = rect;
		SetWindowPlacement(&wp);
	}
	
	// TODO: Add extra initialization here
	m_sbSeqNumber.SetScrollRange(0,63,TRUE);
	m_sbSeqNumber.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbSeqNumber.SetScrollPos(m_nSeqNumber, TRUE);

	m_sbArrayNumber.SetScrollRange(1,10,TRUE);
	m_sbArrayNumber.EnableScrollBar(ESB_ENABLE_BOTH);
	m_sbArrayNumber.SetScrollPos(m_nArrayNumber, TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
