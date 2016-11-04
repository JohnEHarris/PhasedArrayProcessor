#if !defined(AFX_OSCOPE_H__2DC17661_03E3_11D4_8472_00A0C991888B__INCLUDED_)
#define AFX_OSCOPE_H__2DC17661_03E3_11D4_8472_00A0C991888B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Oscope.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COscope dialog

class COscope : public CDialog
{
// Construction
public:
	int m_nIndx1, m_nIndx2;
	SCOPE_MUX_PROTO *pMux1, *pMux2;

// Dialog Data
	//{{AFX_DATA(COscope)
	enum { IDD = IDD_SCOPE };
	CComboBox	m_cbTrace2;
	CComboBox	m_cbTrace1;
	int		m_nMuxSel1;
	int		m_nMuxSel2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COscope)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COscope)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OSCOPE_H__2DC17661_03E3_11D4_8472_00A0C991888B__INCLUDED_)
