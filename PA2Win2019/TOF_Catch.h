#pragma once

#include "..\Include\Global.h"

// TOF_Catch dialog

class TOF_Catch : public CDialogEx
	{
	DECLARE_DYNAMIC(TOF_Catch)

public:
	TOF_Catch(CWnd* pParent = nullptr);   // standard constructor
	virtual ~TOF_Catch();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOF_Catch };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel( );
		public:
		virtual BOOL OnInitDialog( );
		virtual void OnOK( );
		afx_msg void OnDebugTofshow( );
		afx_msg void OnBnClickedTofLog( );
	};
