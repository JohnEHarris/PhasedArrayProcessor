// ScpDmx2.cpp : implementation file
//


#include "stdafx.h"
#include "Truscan.h"
// include "ScpDmx2.h"
#include "TscanDlg.h"

#include "Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScpDmx2 dialog


CScpDmx2::CScpDmx2(CWnd* pParent /*=NULL*/)
	: CDialog(CScpDmx2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScpDmx2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CScpDmx2::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScpDmx2)
	DDX_Control(pDX, IDC_EN_INSTRUCT, m_ceInstruct);
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CScpDmx2, CDialog)
	//{{AFX_MSG_MAP(CScpDmx2)
	ON_BN_CLICKED(IDC_BN_COPY10, OnBnCopy10)
	ON_BN_CLICKED(IDC_BN_COPY40, OnBnCopy40)
	ON_BN_CLICKED(IDC_BN_LOAD, OnBnLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScpDmx2 message handlers



void CScpDmx2::GainTrimOpt(CString &s)
	{
	CString t;

	s += _T("Copy the Trim Gain setting to either all other channels in a shoe/instrument \r\n");
	s += _T("or copy the Trim Gain to all the other 39 channels.\r\n");
	s += _T("Copying must be done separately for both RF and FW mode.\r\n");
	s += _T("Loading default values is done by reading the file \r\n");
	s += _T("ScopeDmx.def which must be present on the machine.  This can result in a loss \r\n");
	s += _T("of data if the ScopeDmx.cal file is not saved in some other location. \r\n");
	s += _T("ScopeDmx.def should normally be created by manufacturing when the system is built.\r\n\r\n");
	t.Format(_T("Chnl = %d   "), gChannel+1);
	if (DmxCalRec.nRfFw) t += ("Mode = FW\r\n");
	else t += ("Mode = RF\r\n");
	s += t;

	// Generate labels for buttons
	SetDlgItemText(IDC_STATIC_COPY10, _T("Copy this channel's Trim Gain to all other channels in this shoe"));
	SetDlgItemText(IDC_STATIC_COPY40, _T("Copy this channel's Trim Gain to all other channels in the system"));
	SetDlgItemText(IDC_STATIC_LOAD, _T("Load default values for trim gains (all channels)"));
	}

void CScpDmx2::ChnlNullOpt(CString &s)
	{
	CString t;

	s += _T("Copy the Chnl Null setting to either all other channels in a shoe/instrument \r\n");
	s += _T("or copy the Chnl Null to all the other 39 channels.\r\n");
	s += _T("Copying must be done separately for both RF and FW mode.\r\n");
	s += _T("Loading default values is done by reading the file \r\n");
	s += _T("ScopeDmx.def which must be present on the machine.  This can result in a loss \r\n");
	s += _T("of data if the ScopeDmx.cal file is not saved in some other location. \r\n");
	s += _T("ScopeDmx.def should normally be created by manufacturing when the system is built.\r\n\r\n");
	t.Format(_T("Chnl = %d   "), gChannel+1);
	if (DmxCalRec.nRfFw) t += ("Mode = FW\r\n");
	else t += ("Mode = RF\r\n");
	s += t;

	// Generate labels for buttons
	SetDlgItemText(IDC_STATIC_COPY10, _T("Copy this channel's Chnl Null to all other channels in this shoe"));
	SetDlgItemText(IDC_STATIC_COPY40, _T("Copy this channel's Chnl Null to all other channels in the system"));
	SetDlgItemText(IDC_STATIC_LOAD, _T("Load default values for chnl null's (all channels)"));
	}


BOOL CScpDmx2::OnInitDialog() 
	{

	CString s, t;
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	switch (DmxCalRec.nState)
		{
	default:
		s = ("Load Defaults is the only option available at this time");
		break;

	case 10:
		GainTrimOpt(s);
		break;

	case 11:
		ChnlNullOpt(s);
		break;

		}

	 m_ceInstruct.SetWindowText(s);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CScpDmx2::OnBnCopy10() 
	{
	// TODO: Add your control notification handler code here
	int nRf, nShoe, nChnl, i, nState;
	WORD wLevel;

	nState = DmxCalRec.nState;	// save  the starting state

	if ( (( nState != 10) && ( nState != 11)) || (DmxCalRec.nCmd != 3) )
		{
		MessageBox(_T("This option is only valid when Trim Gains or Chnl Nulls are being adjusted"));
		return;	// do nothing
		}


	nRf = DmxCalRec.nRfFw & 1;
	nChnl = DmxCalRec.nChnl % MAX_CHANNEL;

	nShoe = nChnl / 10;
	i = nChnl % 10;

	switch (DmxCalRec.nState)
		{
	default:
		break;

	case 10:
		// Trim Gains
		wLevel = DmxCalRec.nVgain[nChnl][nRf];
		for ( i = nShoe*10; i < (nShoe+1)*10; i++)	DmxCalRec.nVgain[i][nRf] = wLevel;
		break;

	case 11:
		// Chnl Nulls
		wLevel = DmxCalRec.nChNull[nChnl][nRf];
		for ( i = nShoe*10; i < (nShoe+1)*10; i++)	DmxCalRec.nChNull[i][nRf] = wLevel;
		break;

		}


	// Download the cal record structure
	DmxCalRec.nCmd = 1;		/* cmd = 1, initial download of structure from mmi to master udp */
	DmxCalRec.nState = 0;
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);

	DmxCalRec.nState = nState;	// restore state and cmd operations
	DmxCalRec.nCmd = 3;
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);

	}

void CScpDmx2::OnBnCopy40() 
	{
	// TODO: Add your control notification handler code here
	int nRf, nChnl, i, nState;
	WORD wLevel;

	nState = DmxCalRec.nState;	// save  the starting state


	if ( (( nState != 10) && ( nState != 11)) || (DmxCalRec.nCmd != 3) )
		{
		MessageBox(_T("This option is only valid when Trim Gains or Chnl Nulls are being adjusted"));
		return;	// do nothing
		}


	nRf = DmxCalRec.nRfFw & 1;
	nChnl = DmxCalRec.nChnl % 40;


	switch (DmxCalRec.nState)
		{
	default:
		break;

	case 10:
		// Trim Gains
		wLevel = DmxCalRec.nVgain[nChnl][nRf];
		for ( i = 0; i < 40; i++)	DmxCalRec.nVgain[i][nRf] = wLevel;
		break;

	case 11:
		// Chnl Nulls
		wLevel = DmxCalRec.nChNull[nChnl][nRf];
		for ( i = 0; i < 40; i++)	DmxCalRec.nChNull[i][nRf] = wLevel;
		break;

		}



	// Download the cal record structure
	DmxCalRec.nCmd = 1;		/* cmd = 1, initial download of structure from mmi to master udp */
	DmxCalRec.nState = 0;
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);

	DmxCalRec.nState = nState;	// restore state and cmd operations
	DmxCalRec.nCmd = 3;
	SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);

	}

void CScpDmx2::OnBnLoad() 
	{
	// TODO: Add your control notification handler code here
	// Open the file ScopeDmx.def and download to the master
	CString s;
	char szScopeDmxDefaultFile[300], *pName;
	s = gszScopeDmxCalFile;
	CstringToChar(s,szScopeDmxDefaultFile);
	pName = strstr(szScopeDmxDefaultFile,"ScopeDmx.");
	pName[9] = 0;	// stop string after dmx.
	strcat(pName, "def");
	
	// Attempt to load the scope demux default file
	try
		{
		s = szScopeDmxDefaultFile;
		CFile file (s, 	CFile::modeNoTruncate |  CFile::modeReadWrite);	
		// do not create if not existing
		// else open existing

		file.Read(&DmxCalRec, sizeof(DmxCalRec));
		// when this procedure closes, it will close the open file
		DmxCalRec.nCmd = 1;		/* cmd = 1, initial download of structure from mmi to master udp */
		DmxCalRec.nState = 0;

		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_AUTOCAL);
		SendMsg(SCOPE_DEMUX_CMD + SCOPE_DEMUX_ADJUST);
		}

	catch (CFileException* e)
		{
		e->ReportError();
		e->Delete();
		}
	}

BOOL CScpDmx2::SendMsg(int MsgNum)
	{	// wrapper so we don't have to reference msg sender
		// in TscanDlg window.
	if (pCTscanDlg == NULL) return FALSE;
	
	return 	pCTscanDlg->SendMsg(MsgNum);
	}
