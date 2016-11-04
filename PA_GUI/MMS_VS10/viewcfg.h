#if !defined(AFX_VIEWCFG_H__B2649EB5_D6E8_44D6_959B_7F990F5C1DB0__INCLUDED_)
#define AFX_VIEWCFG_H__B2649EB5_D6E8_44D6_959B_7F990F5C1DB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Viewcfg.h : header file
//
void Tprintf ( CString &s, float f);
void Tprintf ( char *s, float f);

#define		NO_TYPE		0
#define		WALL_TYPE	1
#define		LONG_TYPE	2
#define		TRAN_TYPE	3
#define		OBQ1_TYPE	4
#define		OBQ2_TYPE	5
#define		OBQ3_TYPE	6		
#define		LAM_TYPE	7

#include "..\include\Truscan_version.h"

#if (LOCATION_ID == BESSEMER)
	#define MAX_CHANNEL_PRINT 30
#else
	#define MAX_CHANNEL_PRINT 40
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewcfg dialog

class CViewcfg : public CDialog
{
// Construction
public:
	CViewcfg(CWnd* pParent = NULL);   // standard constructor
	static CViewcfg *m_pDlg;			// ptr to this dialog..always use
									// the name m_pDlg by convention
	virtual BOOL OnInitDialog();

	void KillMe();					// another way to die
	CPoint MakeCfgTableSensor(CDC *pdc, CPoint printOrigin, CRect Rect, int device);
	CPoint MakeCfgTableHead(CDC *pdc, CPoint printOrigin, int channel, CRect Rect, 
		int device);
	CPoint MakeCfgTable(CDC *pdc, CPoint point, int channel, CRect Rect, int device);
	static int PrintHeading(CDC *pDC, CPoint print_origin, int width, int height,
						CString s1, CString s2);
	void OnFilePrintcfg(); 
	CPoint MakeCfgTableDmx(CDC *pdc, CPoint printOrigin, int channel, CRect Rect, 
		int device);
	int GetChnlTypeIndex(int Chnl);

// Dialog Data
	//{{AFX_DATA(CViewcfg)
	enum { IDD = IDD_VIEWCFG_DIALOG };
	CButton	m_Box1;
	CButton	m_Box2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(viewcfg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	afx_msg void OnPaint();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CViewcfg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCfgNext();
	afx_msg void OnCfgPrevious();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_Page;
	virtual   ~CViewcfg();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWCFG_H__B2649EB5_D6E8_44D6_959B_7F990F5C1DB0__INCLUDED_)
