#pragma once
#include "afxcmn.h"
#include "TuboIni.h"


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
	int m_nInst;
	int m_nLastInst;
	void Save_Pos(void);

	void PositionWindow();
	//void SaveWindowLastPosition(LPCTSTR lpszKey, WINDOWPLACEMENT *wp);
	//void GetWindowLastPosition(LPCTSTR lpszKey, RECT *rect);

	// Ini file section and key values for saving screen location of the dialog
	CString m_DlgLocationKey;				// Include in all dialogs. Names the entry for the registry to hold location
	CString m_DlgLocationSection;			// Section name, for dialog locations this is "Dialog Locations"


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
	afx_msg void OnDeltaposSpInst( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnDeltaposSpPap( NMHDR *pNMHDR, LRESULT *pResult );
	CSpinButtonCtrl m_spInst;
	CSpinButtonCtrl m_spPap;
	afx_msg void OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	};