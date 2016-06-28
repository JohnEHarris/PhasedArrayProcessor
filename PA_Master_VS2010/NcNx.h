#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CNcNx dialog

class CNcNx : public CDialogEx
{
	DECLARE_DYNAMIC(CNcNx)

public:
	CNcNx(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNcNx();
	static CNcNx *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention
	void Save_Pos();
	void UpdateTitle(void);
	void ChangePamOrInstrument(void);
	void CopyListBoxToMessage(void);
	void SendMsg(PAM_INST_CHNL_INFO *pMsg, int nChTypes);


	int m_nPam;		// which PAM
	int m_nLastPam;
	int m_nInst;	// which instrument in this pam
	int m_nLastInst;
	int m_nChnlType, m_nChnlRepeat, m_nChnl;
	int m_nNcId, m_nThldId, m_nMId;
	int m_nNcOd, m_nThldOd, m_nMOd;
	int m_nNx, m_nMaxWall, m_nMinWall, m_nDropCnt;

	NcNxPA2REC *m_pNcNxCfg;		// config struct for [i-PAMs][j-Inst]
	PAM_INST_CHNL_INFO *m_pPamInstChnlInfo;	// config struct for specific PAM and connected Instrument
	int m_nListBoxUnsentQty;	// line in list box not sent to instruments


// Dialog Data
	enum { IDD = IDD_NCNX_PA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	CComboBox m_cbPam;
	CListBox m_lbOutput;
	CSpinButtonCtrl m_spNcId;
	CSpinButtonCtrl m_spThldId;
	CSpinButtonCtrl m_spMId;
	CSpinButtonCtrl m_spNcOd;
	CSpinButtonCtrl m_spThldOd;
	CSpinButtonCtrl m_spMOd;
	CSpinButtonCtrl m_spNx;
	CSpinButtonCtrl m_spMaxWall;
	CSpinButtonCtrl m_spMinWall;
	CSpinButtonCtrl m_spDropCnt;
	CSpinButtonCtrl m_spChnl;
	CSpinButtonCtrl m_spChTypes;
	CSpinButtonCtrl m_spChRepeat;
	CSpinButtonCtrl m_spInst;
	afx_msg void OnCbnSelchangeCbPam();
	afx_msg void OnDeltaposSpInst(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBnUpdate();
	afx_msg void OnBnClickedBnErase();
	afx_msg void OnBnClickedPrint2File();
	afx_msg void OnBnClickedBnSend();
	};
