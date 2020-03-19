#pragma once

#include "..\Include\Global.h"
// CIP_Connect dialog

class CIP_Connect : public CDialogEx
	{
	DECLARE_DYNAMIC(CIP_Connect)

public:
	CIP_Connect(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CIP_Connect();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CIP_Connect };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	};
