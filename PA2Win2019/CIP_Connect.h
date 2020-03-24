#pragma once

#include "..\Include\Global.h"
// CIP_Connect dialog

class CIP_Connect : public CDialogEx
	{
	DECLARE_DYNAMIC(CIP_Connect)

public:
	CIP_Connect(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CIP_Connect();
	void PositionWindow();
	void OnTimer(UINT_PTR nIDEvent);
	void StartTimer();
	void StopTimer();
	void RemoteTimer();
	int  m_nTimerCount;  // call SetNetworkAlarm() every m timer events
	UINT m_uStatTimer;
	void UpdateTime(CString t);

// Dialog Data
	CString m_DlgLocationKey;				// Include in all dialogs. Names the entry for the registry to hold location
	CString m_DlgLocationSection;			// Section name, for dialog locations this is "Dialog Locations"

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
