#if !defined(AFX_PACKET_H__116325CE_ABE3_4E57_8A2A_9C6A6AFA1900__INCLUDED_)
#define AFX_PACKET_H__116325CE_ABE3_4E57_8A2A_9C6A6AFA1900__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Packet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPacket dialog

class CPacket : public CDialog
{
// Construction
public:
	CPacket(CWnd* pParent = NULL);   // standard constructor
	void KillMe();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	static CPacket *m_pDlg;			// ptr to this dialog..always use

// Dialog Data
	//{{AFX_DATA(CPacket)
	enum { IDD = IDD_DPacketD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPacket)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPacket)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKET_H__116325CE_ABE3_4E57_8A2A_9C6A6AFA1900__INCLUDED_)
