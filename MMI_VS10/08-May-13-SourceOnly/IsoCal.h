#if !defined(AFX_ISOCAL_H__E2568A01_0BF1_11D4_9B91_00A0C91FC401__INCLUDED_)
#define AFX_ISOCAL_H__E2568A01_0BF1_11D4_9B91_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IsoCal.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIsoCal dialog

class CIsoCal : public CDialog
{
// Construction
public:
	BYTE m_bPkHold;
	void ShowMaxReadings();
	BOOL SendMsg(int MsgNum);
	BOOL SolveForAB( float *a, float *b, BYTE xmax, BYTE xmin, int ymax, int ymin);
	CIsoCal(CWnd* pParent = NULL);   // standard constructor

	static CIsoCal *m_pDlg;		// ptr to this dialog..always use
									// the name m_pDlg by convention

	void KillMe();					// another way to die
	void StopTimer(void);
	void StartTimer(void);

	time_t m_tTimeNow;
	BYTE bCmin, bCmax, bTmin, bTmax;	// flags for button press
	BYTE bMinChord, bNomChord, bMinThru, bNomThru;	// SAMPLE readings
	BYTE bChordMin, bChordMax, bThruMin, bThruMax;	// inputs for computing
													// coefficients

// Dialog Data
	//{{AFX_DATA(CIsoCal)
	enum { IDD = IDD_ISO_CAL };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIsoCal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIsoCal)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChordMin();
	afx_msg void OnChordNom();
	afx_msg void OnThruMin();
	afx_msg void OnThruNom();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnIsoPkhold();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	UINT m_uIpxTimer;
	UINT m_uTimerTick;
	
	};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISOCAL_H__E2568A01_0BF1_11D4_9B91_00A0C91FC401__INCLUDED_)
