// SequenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "truscan.h"
#include "SequenceDlg.h"
#include "Pulser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* globals */
extern CONFIG_REC ConfigRec ;
extern int gChannel;
/////////////////////////////////////////////////////////////////////////////
// CSequenceDlg dialog


CSequenceDlg::CSequenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSequenceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSequenceDlg)
	m_nChnlBase = gChannel;
	//}}AFX_DATA_INIT

	m_nSeqLen = 1;

	memcpy ( (void *) &m_ConfigRec, (void *) &ConfigRec, sizeof(CONFIG_REC) );
	memcpy ( (void *) &m_OriginalConfigRec, (void *) &ConfigRec, sizeof(CONFIG_REC) );
}


void CSequenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSequenceDlg)
	DDX_Control(pDX, IDC_STATIC_CH9, m_staticCh9);
	DDX_Control(pDX, IDC_STATIC_CH7, m_staticCh7);
	DDX_Control(pDX, IDC_STATIC_CH5, m_staticCh5);
	DDX_Control(pDX, IDC_STATIC_CH8, m_staticCh8);
	DDX_Control(pDX, IDC_STATIC_CH6, m_staticCh6);
	DDX_Control(pDX, IDC_STATIC_CH4, m_staticCh4);
	DDX_Control(pDX, IDC_STATIC_CH3, m_staticCh3);
	DDX_Control(pDX, IDC_STATIC_CH10, m_staticCh10);
	DDX_Control(pDX, IDC_STATIC_CH2, m_staticCh2);
	DDX_Control(pDX, IDC_STATIC_CH1, m_staticCh1);
	DDX_Control(pDX, IDC_CB_CHNL, m_cbChnl);
	DDX_Control(pDX, IDC_COMBO_SEQLEN, m_cbSeqLen);
	DDX_Control(pDX, IDC_CHECK_CH1T1, m_btnCheck[0][0][0]);
	DDX_Control(pDX, IDC_CHECK_CH1T2, m_btnCheck[0][0][1]);
	DDX_Control(pDX, IDC_CHECK_CH1T3, m_btnCheck[0][0][2]);
	DDX_Control(pDX, IDC_CHECK_CH1T4, m_btnCheck[0][0][3]);
	DDX_Control(pDX, IDC_CHECK_CH1T5, m_btnCheck[0][0][4]);
	DDX_Control(pDX, IDC_CHECK_CH1T6, m_btnCheck[0][0][5]);
	DDX_Control(pDX, IDC_CHECK_CH1T7, m_btnCheck[0][0][6]);
	DDX_Control(pDX, IDC_CHECK_CH1T8, m_btnCheck[0][0][7]);
	DDX_Control(pDX, IDC_CHECK_CH1T9, m_btnCheck[0][0][8]);
	DDX_Control(pDX, IDC_CHECK_CH1T10, m_btnCheck[0][0][9]);
	DDX_Control(pDX, IDC_CHECK_CH1R1, m_btnCheck[0][1][0]);
	DDX_Control(pDX, IDC_CHECK_CH1R2, m_btnCheck[0][1][1]);
	DDX_Control(pDX, IDC_CHECK_CH1R3, m_btnCheck[0][1][2]);
	DDX_Control(pDX, IDC_CHECK_CH1R4, m_btnCheck[0][1][3]);
	DDX_Control(pDX, IDC_CHECK_CH1R5, m_btnCheck[0][1][4]);
	DDX_Control(pDX, IDC_CHECK_CH1R6, m_btnCheck[0][1][5]);
	DDX_Control(pDX, IDC_CHECK_CH1R7, m_btnCheck[0][1][6]);
	DDX_Control(pDX, IDC_CHECK_CH1R8, m_btnCheck[0][1][7]);
	DDX_Control(pDX, IDC_CHECK_CH1R9, m_btnCheck[0][1][8]);
	DDX_Control(pDX, IDC_CHECK_CH1R10, m_btnCheck[0][1][9]);
	DDX_Control(pDX, IDC_CHECK_CH2T1, m_btnCheck[1][0][0]);
	DDX_Control(pDX, IDC_CHECK_CH2T2, m_btnCheck[1][0][1]);
	DDX_Control(pDX, IDC_CHECK_CH2T3, m_btnCheck[1][0][2]);
	DDX_Control(pDX, IDC_CHECK_CH2T4, m_btnCheck[1][0][3]);
	DDX_Control(pDX, IDC_CHECK_CH2T5, m_btnCheck[1][0][4]);
	DDX_Control(pDX, IDC_CHECK_CH2T6, m_btnCheck[1][0][5]);
	DDX_Control(pDX, IDC_CHECK_CH2T7, m_btnCheck[1][0][6]);
	DDX_Control(pDX, IDC_CHECK_CH2T8, m_btnCheck[1][0][7]);
	DDX_Control(pDX, IDC_CHECK_CH2T9, m_btnCheck[1][0][8]);
	DDX_Control(pDX, IDC_CHECK_CH2T10, m_btnCheck[1][0][9]);
	DDX_Control(pDX, IDC_CHECK_CH2R1, m_btnCheck[1][1][0]);
	DDX_Control(pDX, IDC_CHECK_CH2R2, m_btnCheck[1][1][1]);
	DDX_Control(pDX, IDC_CHECK_CH2R3, m_btnCheck[1][1][2]);
	DDX_Control(pDX, IDC_CHECK_CH2R4, m_btnCheck[1][1][3]);
	DDX_Control(pDX, IDC_CHECK_CH2R5, m_btnCheck[1][1][4]);
	DDX_Control(pDX, IDC_CHECK_CH2R6, m_btnCheck[1][1][5]);
	DDX_Control(pDX, IDC_CHECK_CH2R7, m_btnCheck[1][1][6]);
	DDX_Control(pDX, IDC_CHECK_CH2R8, m_btnCheck[1][1][7]);
	DDX_Control(pDX, IDC_CHECK_CH2R9, m_btnCheck[1][1][8]);
	DDX_Control(pDX, IDC_CHECK_CH2R10, m_btnCheck[1][1][9]);
	DDX_Control(pDX, IDC_CHECK_CH3T1, m_btnCheck[2][0][0]);
	DDX_Control(pDX, IDC_CHECK_CH3T2, m_btnCheck[2][0][1]);
	DDX_Control(pDX, IDC_CHECK_CH3T3, m_btnCheck[2][0][2]);
	DDX_Control(pDX, IDC_CHECK_CH3T4, m_btnCheck[2][0][3]);
	DDX_Control(pDX, IDC_CHECK_CH3T5, m_btnCheck[2][0][4]);
	DDX_Control(pDX, IDC_CHECK_CH3T6, m_btnCheck[2][0][5]);
	DDX_Control(pDX, IDC_CHECK_CH3T7, m_btnCheck[2][0][6]);
	DDX_Control(pDX, IDC_CHECK_CH3T8, m_btnCheck[2][0][7]);
	DDX_Control(pDX, IDC_CHECK_CH3T9, m_btnCheck[2][0][8]);
	DDX_Control(pDX, IDC_CHECK_CH3T10, m_btnCheck[2][0][9]);
	DDX_Control(pDX, IDC_CHECK_CH3R1, m_btnCheck[2][1][0]);
	DDX_Control(pDX, IDC_CHECK_CH3R2, m_btnCheck[2][1][1]);
	DDX_Control(pDX, IDC_CHECK_CH3R3, m_btnCheck[2][1][2]);
	DDX_Control(pDX, IDC_CHECK_CH3R4, m_btnCheck[2][1][3]);
	DDX_Control(pDX, IDC_CHECK_CH3R5, m_btnCheck[2][1][4]);
	DDX_Control(pDX, IDC_CHECK_CH3R6, m_btnCheck[2][1][5]);
	DDX_Control(pDX, IDC_CHECK_CH3R7, m_btnCheck[2][1][6]);
	DDX_Control(pDX, IDC_CHECK_CH3R8, m_btnCheck[2][1][7]);
	DDX_Control(pDX, IDC_CHECK_CH3R9, m_btnCheck[2][1][8]);
	DDX_Control(pDX, IDC_CHECK_CH3R10, m_btnCheck[2][1][9]);
	DDX_Control(pDX, IDC_CHECK_CH4T1, m_btnCheck[3][0][0]);
	DDX_Control(pDX, IDC_CHECK_CH4T2, m_btnCheck[3][0][1]);
	DDX_Control(pDX, IDC_CHECK_CH4T3, m_btnCheck[3][0][2]);
	DDX_Control(pDX, IDC_CHECK_CH4T4, m_btnCheck[3][0][3]);
	DDX_Control(pDX, IDC_CHECK_CH4T5, m_btnCheck[3][0][4]);
	DDX_Control(pDX, IDC_CHECK_CH4T6, m_btnCheck[3][0][5]);
	DDX_Control(pDX, IDC_CHECK_CH4T7, m_btnCheck[3][0][6]);
	DDX_Control(pDX, IDC_CHECK_CH4T8, m_btnCheck[3][0][7]);
	DDX_Control(pDX, IDC_CHECK_CH4T9, m_btnCheck[3][0][8]);
	DDX_Control(pDX, IDC_CHECK_CH4T10, m_btnCheck[3][0][9]);
	DDX_Control(pDX, IDC_CHECK_CH4R1, m_btnCheck[3][1][0]);
	DDX_Control(pDX, IDC_CHECK_CH4R2, m_btnCheck[3][1][1]);
	DDX_Control(pDX, IDC_CHECK_CH4R3, m_btnCheck[3][1][2]);
	DDX_Control(pDX, IDC_CHECK_CH4R4, m_btnCheck[3][1][3]);
	DDX_Control(pDX, IDC_CHECK_CH4R5, m_btnCheck[3][1][4]);
	DDX_Control(pDX, IDC_CHECK_CH4R6, m_btnCheck[3][1][5]);
	DDX_Control(pDX, IDC_CHECK_CH4R7, m_btnCheck[3][1][6]);
	DDX_Control(pDX, IDC_CHECK_CH4R8, m_btnCheck[3][1][7]);
	DDX_Control(pDX, IDC_CHECK_CH4R9, m_btnCheck[3][1][8]);
	DDX_Control(pDX, IDC_CHECK_CH4R10, m_btnCheck[3][1][9]);
	DDX_Control(pDX, IDC_CHECK_CH5T1, m_btnCheck[4][0][0]);
	DDX_Control(pDX, IDC_CHECK_CH5T2, m_btnCheck[4][0][1]);
	DDX_Control(pDX, IDC_CHECK_CH5T3, m_btnCheck[4][0][2]);
	DDX_Control(pDX, IDC_CHECK_CH5T4, m_btnCheck[4][0][3]);
	DDX_Control(pDX, IDC_CHECK_CH5T5, m_btnCheck[4][0][4]);
	DDX_Control(pDX, IDC_CHECK_CH5T6, m_btnCheck[4][0][5]);
	DDX_Control(pDX, IDC_CHECK_CH5T7, m_btnCheck[4][0][6]);
	DDX_Control(pDX, IDC_CHECK_CH5T8, m_btnCheck[4][0][7]);
	DDX_Control(pDX, IDC_CHECK_CH5T9, m_btnCheck[4][0][8]);
	DDX_Control(pDX, IDC_CHECK_CH5T10, m_btnCheck[4][0][9]);
	DDX_Control(pDX, IDC_CHECK_CH5R1, m_btnCheck[4][1][0]);
	DDX_Control(pDX, IDC_CHECK_CH5R2, m_btnCheck[4][1][1]);
	DDX_Control(pDX, IDC_CHECK_CH5R3, m_btnCheck[4][1][2]);
	DDX_Control(pDX, IDC_CHECK_CH5R4, m_btnCheck[4][1][3]);
	DDX_Control(pDX, IDC_CHECK_CH5R5, m_btnCheck[4][1][4]);
	DDX_Control(pDX, IDC_CHECK_CH5R6, m_btnCheck[4][1][5]);
	DDX_Control(pDX, IDC_CHECK_CH5R7, m_btnCheck[4][1][6]);
	DDX_Control(pDX, IDC_CHECK_CH5R8, m_btnCheck[4][1][7]);
	DDX_Control(pDX, IDC_CHECK_CH5R9, m_btnCheck[4][1][8]);
	DDX_Control(pDX, IDC_CHECK_CH5R10, m_btnCheck[4][1][9]);
	DDX_Control(pDX, IDC_CHECK_CH6T1, m_btnCheck[5][0][0]);
	DDX_Control(pDX, IDC_CHECK_CH6T2, m_btnCheck[5][0][1]);
	DDX_Control(pDX, IDC_CHECK_CH6T3, m_btnCheck[5][0][2]);
	DDX_Control(pDX, IDC_CHECK_CH6T4, m_btnCheck[5][0][3]);
	DDX_Control(pDX, IDC_CHECK_CH6T5, m_btnCheck[5][0][4]);
	DDX_Control(pDX, IDC_CHECK_CH6T6, m_btnCheck[5][0][5]);
	DDX_Control(pDX, IDC_CHECK_CH6T7, m_btnCheck[5][0][6]);
	DDX_Control(pDX, IDC_CHECK_CH6T8, m_btnCheck[5][0][7]);
	DDX_Control(pDX, IDC_CHECK_CH6T9, m_btnCheck[5][0][8]);
	DDX_Control(pDX, IDC_CHECK_CH6T10, m_btnCheck[5][0][9]);
	DDX_Control(pDX, IDC_CHECK_CH6R1, m_btnCheck[5][1][0]);
	DDX_Control(pDX, IDC_CHECK_CH6R2, m_btnCheck[5][1][1]);
	DDX_Control(pDX, IDC_CHECK_CH6R3, m_btnCheck[5][1][2]);
	DDX_Control(pDX, IDC_CHECK_CH6R4, m_btnCheck[5][1][3]);
	DDX_Control(pDX, IDC_CHECK_CH6R5, m_btnCheck[5][1][4]);
	DDX_Control(pDX, IDC_CHECK_CH6R6, m_btnCheck[5][1][5]);
	DDX_Control(pDX, IDC_CHECK_CH6R7, m_btnCheck[5][1][6]);
	DDX_Control(pDX, IDC_CHECK_CH6R8, m_btnCheck[5][1][7]);
	DDX_Control(pDX, IDC_CHECK_CH6R9, m_btnCheck[5][1][8]);
	DDX_Control(pDX, IDC_CHECK_CH6R10, m_btnCheck[5][1][9]);
	DDX_Control(pDX, IDC_CHECK_CH7T1, m_btnCheck[6][0][0]);
	DDX_Control(pDX, IDC_CHECK_CH7T2, m_btnCheck[6][0][1]);
	DDX_Control(pDX, IDC_CHECK_CH7T3, m_btnCheck[6][0][2]);
	DDX_Control(pDX, IDC_CHECK_CH7T4, m_btnCheck[6][0][3]);
	DDX_Control(pDX, IDC_CHECK_CH7T5, m_btnCheck[6][0][4]);
	DDX_Control(pDX, IDC_CHECK_CH7T6, m_btnCheck[6][0][5]);
	DDX_Control(pDX, IDC_CHECK_CH7T7, m_btnCheck[6][0][6]);
	DDX_Control(pDX, IDC_CHECK_CH7T8, m_btnCheck[6][0][7]);
	DDX_Control(pDX, IDC_CHECK_CH7T9, m_btnCheck[6][0][8]);
	DDX_Control(pDX, IDC_CHECK_CH7T10, m_btnCheck[6][0][9]);
	DDX_Control(pDX, IDC_CHECK_CH7R1, m_btnCheck[6][1][0]);
	DDX_Control(pDX, IDC_CHECK_CH7R2, m_btnCheck[6][1][1]);
	DDX_Control(pDX, IDC_CHECK_CH7R3, m_btnCheck[6][1][2]);
	DDX_Control(pDX, IDC_CHECK_CH7R4, m_btnCheck[6][1][3]);
	DDX_Control(pDX, IDC_CHECK_CH7R5, m_btnCheck[6][1][4]);
	DDX_Control(pDX, IDC_CHECK_CH7R6, m_btnCheck[6][1][5]);
	DDX_Control(pDX, IDC_CHECK_CH7R7, m_btnCheck[6][1][6]);
	DDX_Control(pDX, IDC_CHECK_CH7R8, m_btnCheck[6][1][7]);
	DDX_Control(pDX, IDC_CHECK_CH7R9, m_btnCheck[6][1][8]);
	DDX_Control(pDX, IDC_CHECK_CH7R10, m_btnCheck[6][1][9]);
	DDX_Control(pDX, IDC_CHECK_CH8T1, m_btnCheck[7][0][0]);
	DDX_Control(pDX, IDC_CHECK_CH8T2, m_btnCheck[7][0][1]);
	DDX_Control(pDX, IDC_CHECK_CH8T3, m_btnCheck[7][0][2]);
	DDX_Control(pDX, IDC_CHECK_CH8T4, m_btnCheck[7][0][3]);
	DDX_Control(pDX, IDC_CHECK_CH8T5, m_btnCheck[7][0][4]);
	DDX_Control(pDX, IDC_CHECK_CH8T6, m_btnCheck[7][0][5]);
	DDX_Control(pDX, IDC_CHECK_CH8T7, m_btnCheck[7][0][6]);
	DDX_Control(pDX, IDC_CHECK_CH8T8, m_btnCheck[7][0][7]);
	DDX_Control(pDX, IDC_CHECK_CH8T9, m_btnCheck[7][0][8]);
	DDX_Control(pDX, IDC_CHECK_CH8T10, m_btnCheck[7][0][9]);
	DDX_Control(pDX, IDC_CHECK_CH8R1, m_btnCheck[7][1][0]);
	DDX_Control(pDX, IDC_CHECK_CH8R2, m_btnCheck[7][1][1]);
	DDX_Control(pDX, IDC_CHECK_CH8R3, m_btnCheck[7][1][2]);
	DDX_Control(pDX, IDC_CHECK_CH8R4, m_btnCheck[7][1][3]);
	DDX_Control(pDX, IDC_CHECK_CH8R5, m_btnCheck[7][1][4]);
	DDX_Control(pDX, IDC_CHECK_CH8R6, m_btnCheck[7][1][5]);
	DDX_Control(pDX, IDC_CHECK_CH8R7, m_btnCheck[7][1][6]);
	DDX_Control(pDX, IDC_CHECK_CH8R8, m_btnCheck[7][1][7]);
	DDX_Control(pDX, IDC_CHECK_CH8R9, m_btnCheck[7][1][8]);
	DDX_Control(pDX, IDC_CHECK_CH8R10, m_btnCheck[7][1][9]);
	DDX_Control(pDX, IDC_CHECK_CH9T1, m_btnCheck[8][0][0]);
	DDX_Control(pDX, IDC_CHECK_CH9T2, m_btnCheck[8][0][1]);
	DDX_Control(pDX, IDC_CHECK_CH9T3, m_btnCheck[8][0][2]);
	DDX_Control(pDX, IDC_CHECK_CH9T4, m_btnCheck[8][0][3]);
	DDX_Control(pDX, IDC_CHECK_CH9T5, m_btnCheck[8][0][4]);
	DDX_Control(pDX, IDC_CHECK_CH9T6, m_btnCheck[8][0][5]);
	DDX_Control(pDX, IDC_CHECK_CH9T7, m_btnCheck[8][0][6]);
	DDX_Control(pDX, IDC_CHECK_CH9T8, m_btnCheck[8][0][7]);
	DDX_Control(pDX, IDC_CHECK_CH9T9, m_btnCheck[8][0][8]);
	DDX_Control(pDX, IDC_CHECK_CH9T10, m_btnCheck[8][0][9]);
	DDX_Control(pDX, IDC_CHECK_CH9R1, m_btnCheck[8][1][0]);
	DDX_Control(pDX, IDC_CHECK_CH9R2, m_btnCheck[8][1][1]);
	DDX_Control(pDX, IDC_CHECK_CH9R3, m_btnCheck[8][1][2]);
	DDX_Control(pDX, IDC_CHECK_CH9R4, m_btnCheck[8][1][3]);
	DDX_Control(pDX, IDC_CHECK_CH9R5, m_btnCheck[8][1][4]);
	DDX_Control(pDX, IDC_CHECK_CH9R6, m_btnCheck[8][1][5]);
	DDX_Control(pDX, IDC_CHECK_CH9R7, m_btnCheck[8][1][6]);
	DDX_Control(pDX, IDC_CHECK_CH9R8, m_btnCheck[8][1][7]);
	DDX_Control(pDX, IDC_CHECK_CH9R9, m_btnCheck[8][1][8]);
	DDX_Control(pDX, IDC_CHECK_CH9R10, m_btnCheck[8][1][9]);
	DDX_Control(pDX, IDC_CHECK_CH10T1, m_btnCheck[9][0][0]);
	DDX_Control(pDX, IDC_CHECK_CH10T2, m_btnCheck[9][0][1]);
	DDX_Control(pDX, IDC_CHECK_CH10T3, m_btnCheck[9][0][2]);
	DDX_Control(pDX, IDC_CHECK_CH10T4, m_btnCheck[9][0][3]);
	DDX_Control(pDX, IDC_CHECK_CH10T5, m_btnCheck[9][0][4]);
	DDX_Control(pDX, IDC_CHECK_CH10T6, m_btnCheck[9][0][5]);
	DDX_Control(pDX, IDC_CHECK_CH10T7, m_btnCheck[9][0][6]);
	DDX_Control(pDX, IDC_CHECK_CH10T8, m_btnCheck[9][0][7]);
	DDX_Control(pDX, IDC_CHECK_CH10T9, m_btnCheck[9][0][8]);
	DDX_Control(pDX, IDC_CHECK_CH10T10, m_btnCheck[9][0][9]);
	DDX_Control(pDX, IDC_CHECK_CH10R1, m_btnCheck[9][1][0]);
	DDX_Control(pDX, IDC_CHECK_CH10R2, m_btnCheck[9][1][1]);
	DDX_Control(pDX, IDC_CHECK_CH10R3, m_btnCheck[9][1][2]);
	DDX_Control(pDX, IDC_CHECK_CH10R4, m_btnCheck[9][1][3]);
	DDX_Control(pDX, IDC_CHECK_CH10R5, m_btnCheck[9][1][4]);
	DDX_Control(pDX, IDC_CHECK_CH10R6, m_btnCheck[9][1][5]);
	DDX_Control(pDX, IDC_CHECK_CH10R7, m_btnCheck[9][1][6]);
	DDX_Control(pDX, IDC_CHECK_CH10R8, m_btnCheck[9][1][7]);
	DDX_Control(pDX, IDC_CHECK_CH10R9, m_btnCheck[9][1][8]);
	DDX_Control(pDX, IDC_CHECK_CH10R10, m_btnCheck[9][1][9]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSequenceDlg, CDialog)
	//{{AFX_MSG_MAP(CSequenceDlg)
	ON_CBN_SELCHANGE(IDC_CB_CHNL, OnSelchangeCbChnl)
	ON_CBN_SELCHANGE(IDC_COMBO_SEQLEN, OnSelchangeCbSeqLen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSequenceDlg message handlers
BOOL CSequenceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cbChnl.SetCurSel(m_nChnlBase/10);

	m_nSeqLen = m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].sequence_length;

	m_cbSeqLen.SetCurSel(m_nSeqLen-1);

	SetCheckedBox();

	UpdateDlg();

    SetDefID(-1);

	return TRUE;
}

void CSequenceDlg::OnSelchangeCbChnl() 
{
	// TODO: Add your control notification handler code here
	GetCheckedBox();

	m_nChnlBase = m_cbChnl.GetCurSel() * 10;
	m_nSeqLen = m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].sequence_length;
	m_cbSeqLen.SetCurSel(m_nSeqLen-1);

	SetCheckedBox();

	UpdateDlg();
}

void CSequenceDlg::OnSelchangeCbSeqLen() 
{
	// TODO: Add your control notification handler code here
	m_nSeqLen = m_cbSeqLen.GetCurSel()+1;

	//m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].sequence_length = m_nSeqLen;
	for (int i=0; i<MAX_SHOES; i++)
	{
		m_ConfigRec.UtRec.Shoe[i].sequence_length = m_nSeqLen;
	}

	SetCheckedBox();

	UpdateDlg();
}

/*
void CSequenceDlg::OnChangeEditSeqlen() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	int nOldSeqLen = m_nSeqLen;

	UpdateData(TRUE);

	// check range
	if (m_nSeqLen < 1)
	{
		m_nSeqLen = nOldSeqLen;
		UpdateData(FALSE);
	}
	if (m_nSeqLen > 10)
	{
		m_nSeqLen = nOldSeqLen;
		UpdateData(FALSE);
	}

	m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].sequence_length = m_nSeqLen;

	SetCheckedBox();

	UpdateDlg();
}
*/

void CSequenceDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CSequenceDlg::OnOK() 
{
	// TODO: Add extra validation here
	int i;
	int iChannel, iGate;  // active channel and gate

	GetCheckedBox();

	memcpy ( (void *) &ConfigRec, (void *) &m_ConfigRec, sizeof(CONFIG_REC) );

	/* send fire and receive sequences if changed */
	for (i=0; i<MAX_CHANNEL; i++)
	{
		if (   m_OriginalConfigRec.UtRec.Shoe[i/10].Ch[i%10].Fire_Sequence != ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Fire_Sequence
			|| m_OriginalConfigRec.UtRec.Shoe[i/10].Ch[i%10].Rcvr_Sequence != ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Rcvr_Sequence)
		{
			m_pTcpThreadRxList->EnableFireSequence (i, TRUE);
			//if (i == 0) AfxMessageBox(_T("Fire sequence changed"));

			if (CPulser::m_pDlg != NULL)
			{
				CPulser::m_pDlg->m_pUndo->UtRec.Shoe[i/10].Ch[i%10].Fire_Sequence = ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Fire_Sequence;
				CPulser::m_pDlg->m_pUndo->UtRec.Shoe[i/10].Ch[i%10].Rcvr_Sequence = ConfigRec.UtRec.Shoe[i/10].Ch[i%10].Rcvr_Sequence;
			}
		}

	}

	if (CPulser::m_pDlg != NULL)
		CPulser::m_pDlg->UpdateDlg();

	/* send sequence length if changed */
	for (i=0; i<MAX_CHANNEL/MAX_CHANNEL_PER_INSTRUMENT; i++)
	{
		if (   m_OriginalConfigRec.UtRec.Shoe[i].sequence_length != ConfigRec.UtRec.Shoe[i].sequence_length)
			m_pTcpThreadRxList->SetSequenceLength(i*10);
	}

	// trigger scope by the first active sequence point
	iChannel = m_pTcpThreadRxList->m_nActiveChannel;
	iGate = m_pTcpThreadRxList->m_nActiveGate;
	m_pTcpThreadRxList->EnableScopeDisplay(TRUE, iChannel, iGate);

	CDialog::OnOK();
}

void CSequenceDlg::UpdateDlg()
{
	int i, j, k;
	CString sChnl;

	UpdateData(FALSE);

	for (i=0; i<10; i++)
	{
		for (j=0; j<2; j++)
		{
			for (k=0; k<MAX_SEQ_LEN; k++)
			{
				if (k<m_nSeqLen)
					m_btnCheck[i][j][k].EnableWindow(TRUE);
				else
					m_btnCheck[i][j][k].EnableWindow(FALSE);
			}
		}
	}

	sChnl.Format(_T("Ch %02d"),(m_nChnlBase/10)*10+1);
	m_staticCh1.SetWindowText(sChnl);
	sChnl.Format(_T("Ch %02d"),(m_nChnlBase/10)*10+2);
	m_staticCh2.SetWindowText(sChnl);
	sChnl.Format(_T("Ch %02d"),(m_nChnlBase/10)*10+3);
	m_staticCh3.SetWindowText(sChnl);
	sChnl.Format(_T("Ch %02d"),(m_nChnlBase/10)*10+4);
	m_staticCh4.SetWindowText(sChnl);
	sChnl.Format(_T("Ch %02d"),(m_nChnlBase/10)*10+5);
	m_staticCh5.SetWindowText(sChnl);
	sChnl.Format(_T("Ch %02d"),(m_nChnlBase/10)*10+6);
	m_staticCh6.SetWindowText(sChnl);
	sChnl.Format(_T("Ch %02d"),(m_nChnlBase/10)*10+7);
	m_staticCh7.SetWindowText(sChnl);
	sChnl.Format(_T("Ch %02d"),(m_nChnlBase/10)*10+8);
	m_staticCh8.SetWindowText(sChnl);
	sChnl.Format(_T("Ch %02d"),(m_nChnlBase/10)*10+9);
	m_staticCh9.SetWindowText(sChnl);
	sChnl.Format(_T("Ch %02d"),(m_nChnlBase/10)*10+10);
	m_staticCh10.SetWindowText(sChnl);

}

void CSequenceDlg::SetCheckedBox()
{
	int i, j, k;

	for (i=0; i<10; i++)
	{
		for (j=0; j<2; j++)
		{
			for (k=0; k<m_nSeqLen; k++)
			{
				if (j == 0)
					m_nBtnChecked[i][j][k] = (BYTE) ((m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].Ch[i].Fire_Sequence & ((WORD) 0x1 << k)) >> k);
				else
					m_nBtnChecked[i][j][k] = (BYTE) ((m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].Ch[i].Rcvr_Sequence & ((WORD) 0x1 << k)) >> k);

				m_btnCheck[i][j][k].SetCheck (m_nBtnChecked[i][j][k]);
			}
		}
	}
}

void CSequenceDlg::GetCheckedBox()
{
	int i, j, k;
	WORD mask;

	for (i=0; i<10; i++)
	{
		for (j=0; j<2; j++)
		{
			for (k=0; k<m_nSeqLen; k++)
			{
				m_nBtnChecked[i][j][k] = m_btnCheck[i][j][k].GetCheck();

				//TRACE("Base= %d  i=%d  j=%d  k=%d  Check=%d\n",m_nChnlBase, i,j,k,m_nBtnChecked[i][j][k]);

				if (j == 0)
				{
					mask = (WORD) 0x1 << k;
					if (m_nBtnChecked[i][j][k] == 1)
						m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].Ch[i].Fire_Sequence |= mask;
					else
						m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].Ch[i].Fire_Sequence &= ~mask;
				}
				else
				{
					mask = (WORD) 0x1 << k;
					if (m_nBtnChecked[i][j][k] == 1)
						m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].Ch[i].Rcvr_Sequence |= mask;
					else
						m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].Ch[i].Rcvr_Sequence &= ~mask;
				}

				//TRACE("Fire seq = 0x%08X\n",m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].Ch[i].Fire_Sequence);
				//TRACE("Rcvr seq = 0x%08X\n",m_ConfigRec.UtRec.Shoe[m_nChnlBase/10].Ch[i].Rcvr_Sequence);
			}
		}
	}
}
