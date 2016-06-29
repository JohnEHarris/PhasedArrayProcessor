#if !defined(AFX_TOF_H__580903D0_5ABB_4BC1_B37D_CE949B68C099__INCLUDED_)
#define AFX_TOF_H__580903D0_5ABB_4BC1_B37D_CE949B68C099__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// tof.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Ctof dialog

class Ctof : public CDialog
{
// Construction
public:
	Ctof(CWnd* pParent = NULL);   // standard constructor
	BOOL SendMsg(int MsgNum);
	void KillMe();
	static Ctof *m_pDlg;			// ptr to this dialog..always use
	CONFIG_REC * m_pUndo;
	void UndoTofSettings();
								// the name m_pDlg by convention
	virtual BOOL OnInitDialog();
	virtual void OnOK();


// Dialog Data
	//{{AFX_DATA(Ctof)
	enum { IDD = IDD_DTOF };
	CButton	m_btnWall;
	CComboBox	m_CBst;
	CComboBox	m_CBsp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Ctof)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Ctof)
	virtual void OnCancel();
	afx_msg void OnBTNAlrm();
	afx_msg void OnBtnLgc();
	afx_msg void OnBtnRsl();
	afx_msg void OnBtnWll();
	afx_msg void OnSelchangeCBStOn();
	afx_msg void OnSelchangeCBSpOn();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnTofUndo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Save_Pos();
	virtual ~Ctof();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOF_H__580903D0_5ABB_4BC1_B37D_CE949B68C099__INCLUDED_)
