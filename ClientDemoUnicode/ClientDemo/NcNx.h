#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "global.h"


// CNcNx dialog

class CNcNx : public CDialogEx
{
	DECLARE_DYNAMIC(CNcNx)

public:
	CNcNx(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNcNx();

// Dialog Data
	enum { IDD = IDD_DLG_NC_NX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	CSpinButtonCtrl m_spChTypes;
	CSpinButtonCtrl m_spChRepeat;
	CComboBox m_cbPam;
	CComboBox m_cbInst;
	CComboBox m_cbNcID;
	CComboBox m_cbThldID;
	CComboBox m_cbMID;
	CComboBox m_cbNcOD;
	CComboBox m_cbThldOD;
	CComboBox m_cbMOD;
	CComboBox m_cbNx;
	CComboBox m_cbMaxWall;
	CComboBox m_cbMinWall;
	CComboBox m_cbDropCnt;
	CListBox m_lbOutput;
	afx_msg void OnBnClickedBnSend();
	};
