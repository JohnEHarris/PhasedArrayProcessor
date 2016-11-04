// ChnlRangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "ChnlRangeDlg.h"
#include "tscandlg.h"

#include "QStringParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CONFIG_REC ConfigRec;

/////////////////////////////////////////////////////////////////////////////
// CChnlRangeDlg dialog


CChnlRangeDlg::CChnlRangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChnlRangeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChnlRangeDlg)
	m_bLong = FALSE;
	m_bTran = FALSE;
	m_bObq1 = FALSE;
	m_bObq2 = FALSE;
	m_bObq3 = FALSE;
	m_bWall = FALSE;
	//}}AFX_DATA_INIT
}


void CChnlRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChnlRangeDlg)
	DDX_Control(pDX, IDC_EDIT_RANGE, m_editChnlRange);
	DDX_Check(pDX, IDC_LONG, m_bLong);
	DDX_Check(pDX, IDC_TRAN, m_bTran);
	DDX_Check(pDX, IDC_OBQ1, m_bObq1);
	DDX_Check(pDX, IDC_OBQ2, m_bObq2);
	DDX_Check(pDX, IDC_OBQ3, m_bObq3);
	DDX_Check(pDX, IDC_WALL, m_bWall);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChnlRangeDlg, CDialog)
	//{{AFX_MSG_MAP(CChnlRangeDlg)
	ON_BN_CLICKED(IDC_LONG, OnLong)
	ON_BN_CLICKED(IDC_OBQ1, OnObq1)
	ON_BN_CLICKED(IDC_OBQ2, OnObq2)
	ON_BN_CLICKED(IDC_OBQ3, OnObq3)
	ON_BN_CLICKED(IDC_TRAN, OnTran)
	ON_BN_CLICKED(IDC_WALL, OnWall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChnlRangeDlg message handlers

void CChnlRangeDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CChnlRangeDlg::OnOK() 
{
	// TODO: Add extra validation here
	GetChannelRange();
	
	CDialog::OnOK();
}

BOOL CChnlRangeDlg::GetChannelRange() 
{
	// TODO: Add your control notification handler code here
	char ch[128];
	ResetChannelRange();

	CString sChnlRange;
	m_editChnlRange.GetWindowText(sChnlRange);
	CQStringParser sParser(sChnlRange, ',','\"');
	int i, j;

	int nCount = sParser.GetCount();
	if (nCount > 0)
	{
		CString str = "";
		CQStringParser sTemp(sChnlRange, ',','\"');
		int nCnt;
		int nStart;
		int nStop;

		for (i=1; i<=nCount; i++)
		{
			//str += (sParser.GetField(i) + CString("\n"));
			str = sParser.GetField(i);
			if (str.Find('-') >= 0)
			{
				//AfxMessageBox(str);
				sTemp.ResetOriginalString(str,'-','\"');
				//AfxMessageBox(sTemp.GetField(1));
				nCnt = sTemp.GetCount();
				if (nCnt != 2)
				{
					ResetChannelRange();
					return FALSE;
				}
				else
				{
					// range start
					str = sTemp.GetField(1);
					if (!IsStringNumber(str))
					{
						ResetChannelRange();
						return FALSE;
					}
					else
					{
						CstringToChar(str,ch);
						sscanf(ch, "%d", &nStart);
						if ( (nStart < 1) || (nStart > 100) )
						{
							ResetChannelRange();
							return FALSE;
						}
					}
					// range stop
					str = sTemp.GetField(2);
					if (!IsStringNumber(str))
					{
						ResetChannelRange();
						return FALSE;
					}
					else
					{
						CstringToChar(str,ch);
						sscanf(ch, "%d", &nStop);
						if ( (nStop < 1) || (nStop > 100) )
						{
							ResetChannelRange();
							return FALSE;
						}
					}
					
					if (nStop < nStart)
					{
						ResetChannelRange();
						return FALSE;
					}

					for (j=nStart; j<=nStop; j++)
						m_bChnlSelect[j-1] = TRUE;
				}
			}
			else
			{
				if (!IsStringNumber(str))
				{
					ResetChannelRange();
					return FALSE;
				}
				else
				{
					CstringToChar(str,ch);
					sscanf(ch, "%d", &nStart);
					if ( (nStart < 1) || (nStart > 100) )
					{
						ResetChannelRange();
						return FALSE;
					}
					else
					{
						m_bChnlSelect[nStart-1] = TRUE;
					}
				}
			}
		}
	}
	else
	{
		ResetChannelRange();
		return FALSE;
	}

	return TRUE;
}


void CChnlRangeDlg::ResetChannelRange()
{
	int i;
	for ( i=0; i<MEM_MAX_CHANNEL; i++)
		m_bChnlSelect[i] = FALSE;

	for (i=0; i<40; i++)
	{
		switch ( ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Type)
		{
		case IS_WALL:
			if (m_bWall)
				m_bChnlSelect[i] = TRUE;
			break;

		case IS_LONG:
			if (m_bLong)
				m_bChnlSelect[i] = TRUE;
			break;

		case IS_TRAN:
			if (m_bTran)
				m_bChnlSelect[i] = TRUE;
			break;

		case IS_OBQ1:
			if (m_bObq1)
				m_bChnlSelect[i] = TRUE;
			break;

		case IS_OBQ2:
			if (m_bObq2)
				m_bChnlSelect[i] = TRUE;
			break;

		case IS_OBQ3:
			if (m_bObq3)
				m_bChnlSelect[i] = TRUE;
			break;

		default:
			break;
		}
	}

}


BOOL CChnlRangeDlg::IsStringNumber(CString str)
{
	int nLength = str.GetLength();
	TCHAR c;

	if (nLength < 1)
		return FALSE;

	for (int i=0; i<nLength; i++)
	{
		c = str.GetAt(i);

		if (i==0)
			if (c == _T('0'))
				return FALSE;

		switch(c)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			break;

		default:
			return FALSE;
			break;
		}
	}

	return TRUE;
}


void CChnlRangeDlg::OnLong() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CChnlRangeDlg::OnObq1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CChnlRangeDlg::OnObq2() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CChnlRangeDlg::OnObq3() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CChnlRangeDlg::OnTran() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CChnlRangeDlg::OnWall() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

BOOL CChnlRangeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//m_editChnlRange.SetWindowText(_T("1-40"));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
