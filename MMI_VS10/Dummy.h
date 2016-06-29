#pragma once


// CDummy dialog
// Use this class to see what ON_MESSAGE macro is generated for OnNcHitTest message
class CDummy : public CDialog
{
	DECLARE_DYNAMIC(CDummy)

public:
	CDummy(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDummy();

// Dialog Data
	enum { IDD = IDD_DUMMY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
