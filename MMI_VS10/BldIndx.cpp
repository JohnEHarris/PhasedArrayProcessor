// BldIndx.cpp : implementation file
//
// Date:	03/08/00
// Build/rebuild index file for pipe data file.  Show progress bar as
//	file is rebuilt


#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"


//#include "BldIndx.h"

#include "Extern.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBldIndx dialog


CBldIndx::CBldIndx(CWnd* pParent /*=NULL*/)
	: CDialog(CBldIndx::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBldIndx)
	//}}AFX_DATA_INIT
}


void CBldIndx::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBldIndx)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBldIndx, CDialog)
	//{{AFX_MSG_MAP(CBldIndx)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBldIndx message handlers

void CBldIndx::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CBldIndx::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CBldIndx::PostNcDestroy() 
	{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	}

BOOL CBldIndx::OnInitDialog() 
	{
	CFile *pIndexFile, *pDataFile, *pTmpFile;
	pPrgsDlg = new (CProgressCtrl);
	CRect PRect;
	int i;
	CWnd* pWnd;
	DWORD len, DataLen;
	int nSize;
	I_MSG_RUN	*pImsg01;
	DWORD CurrentJoint;
	INDEX_REC iRec;


	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	pIndexFile = (pCTscanDlg->m_pInspectDlg1)->GetIndexFile();
	if (!pIndexFile)
		{	// fatal error
		GetDlgItem(IDC_REBUILD_TEXT)->SetWindowText(
			_T("Fatal Error .. Can not Rebuild"));
		return TRUE;	// OPERATOR must close with cancel button
		}

	pDataFile = (pCTscanDlg->m_pInspectDlg1)->GetDataFile();
	DataLen = (DWORD) pDataFile->GetLength();
	DataLen++;	// guarantee not 0
	// Build index file here

	pWnd = GetDlgItem(IDC_PRGS_BAR);
	pWnd->GetClientRect(&PRect);
	::MapWindowPoints(pWnd->m_hWnd, m_hWnd, (LPPOINT) &PRect, 2);
	i = pPrgsDlg->Create(WS_VISIBLE | WS_CHILD, PRect, this,IDC_PRGS_BAR);

	// progress bar in per cent
	pPrgsDlg->SetRange(0,100);
	// skip header, record joint number (W), file mark (L) and time
	len = (DWORD) pDataFile->Seek(CONFIG_REC_SIZE, CFile::begin); 

	pImsg01 = new I_MSG_RUN;
	nSize = pDataFile->Read(pImsg01, sizeof(I_MSG_RUN));
	CurrentJoint = pImsg01->InspHdr.JointNumber;

	while (nSize == sizeof(I_MSG_RUN) )
		{
		iRec.JointNumber = CurrentJoint;
		iRec.pos = len;
		iRec.time = pImsg01->InspHdr.time;
		pIndexFile->Write(&iRec, sizeof(INDEX_REC));

		// Update progress bar as per cent of data file read
		pPrgsDlg->SetPos( ( (len*100)/DataLen) % 100);

		while ( (CurrentJoint == pImsg01->InspHdr.JointNumber) &&
				(nSize == sizeof(I_MSG_RUN) ) )
			{
			len = (DWORD) pDataFile->GetPosition();
			nSize = pDataFile->Read(pImsg01, sizeof(I_MSG_RUN));
			}
		CurrentJoint = pImsg01->InspHdr.JointNumber;
		}
	delete pImsg01;


	// leave index open but close a duplicate handle to write file
	pTmpFile = pIndexFile->Duplicate();
	pTmpFile->Close();
	delete pTmpFile;
	//::Sleep(500);

//	MessageBox("Done with Index file");
	OnOK();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

CBldIndx::~CBldIndx()
	{
//	StopTimer();
	delete pPrgsDlg;
	}

void CBldIndx::StartTimer()
	{	// helper function to start timer with one call
	if (m_uIpxTimer)	return;	// already running

	m_uIpxTimer = SetTimer(IDT_IPX_TIMER, 150, NULL);
	if (!m_uIpxTimer) MessageBox(_T("Failed to start IPX timer"));
	}



void CBldIndx::StopTimer()
	{	// helper function to stop timer with one call
	if (m_uIpxTimer)
		{
		KillTimer(m_uIpxTimer);
		m_uIpxTimer = 0;
		}
	}


void CBldIndx::OnTimer(UINT nIDEvent) 
	{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnTimer(nIDEvent);
	m_nTick++;
	if ( m_nTick <= 100)	pPrgsDlg->SetPos(m_nTick);
	else OnOK();
	}
