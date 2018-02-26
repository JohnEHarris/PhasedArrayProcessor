#pragma once
#include "afxcmn.h"
#include "TuboIni.h"
#include "afxwin.h"


// CNcNx dialog

class CNcNx : public CDialogEx
{
	DECLARE_DYNAMIC(CNcNx)

public:
	CNcNx(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNcNx();
	void UpdateTitle(void);

	int m_nPam;		// which PAM
	int m_nLastPam;
	int m_nBoard;
	int m_nPAP;
	int m_nSeq;
	int m_nCh;
	int m_nGate;
	int m_nParam;
	int m_nPopulated;
	int m_wPacketRate;
	//int m_nLastInst;
	void Save_Pos(void);

	void PositionWindow();
	//void SaveWindowLastPosition(LPCTSTR lpszKey, WINDOWPLACEMENT *wp);
	//void GetWindowLastPosition(LPCTSTR lpszKey, RECT *rect);

	// Ini file section and key values for saving screen location of the dialog
	CString m_DlgLocationKey;				// Include in all dialogs. Names the entry for the registry to hold location
	CString m_DlgLocationSection;			// Section name, for dialog locations this is "Dialog Locations"
	void DebugOut( CString s );
	void SendMsg( GenericPacketHeader *pMsg );

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NCNX_PA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	CSpinButtonCtrl m_spPap;
	CSpinButtonCtrl m_spBoard;
	CSpinButtonCtrl m_spSeq;
	CSpinButtonCtrl m_spCh;
	CSpinButtonCtrl m_spGate;
	CSpinButtonCtrl m_spParam;

	afx_msg void OnDeltaposSpPap( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnDeltaposSpBoard( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnDeltaposSpSeq( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnDeltaposSpCh( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnDeltaposSpGate( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnDeltaposSpParam( NMHDR *pNMHDR, LRESULT *pResult );
	CListBox m_lbOutput;
	afx_msg void OnBnClickedBnErase();
	afx_msg void OnCbnSelchangeCbCmds();
	afx_msg void OnBnClickedBnDonothing();
	CComboBox m_cbCommand;
	CButton m_bnDoNoting;
	int GetSpinValue( LPNMUPDOWN pNMUpDown, CSpinButtonCtrl *m_spButton );


	int m_nCmdId;
	void PopulateCmdComboBox();

	void GateCmd( int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue);
	// All gate commands have the same template. The Cmd ID determines which one of the
	// 7 gate commands is actually sent.
	ST_GATE_DELAY_CMD m_GateCmd;
	void TcgCmd( int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue );
	// TCG commands have the same gain template
	ST_SET_TCG_DELAY_CMD m_TcgCmd;
	void FakeData( int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue );
	// WordCmd, use nValue to assign to hardware functions
	void WordCmd(int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, int nValue);
	ST_WORD_CMD m_WordCmd;
	void LargeCmd(int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, WORD wValue);
	ST_LARGE_CMD m_wLargeCmd;
	afx_msg void OnChangeEdParam();
	CEdit m_edParam;
	//int m_nAscanCnt;
	void IncrementAscanCnt(void);
	afx_msg void OnBnClickedRbSmallcmd();
	afx_msg void OnBnClickedRbLargecmds();
	int m_nShowSmallCmds;
	void DebugFifo(int nPap, int nBoard, int nSeq, int nCh, int nGate, int nCmd, WORD wValue);
	};
