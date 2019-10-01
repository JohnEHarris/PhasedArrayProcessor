// CTestThread.cpp : implementation file
//
/*
Author:		JEH
Date:		24-Oct-2012
Purpose:	Test thread operations, particular PostThreadMessage in a windows service

Revised:	30-jAN-13 implement a timer tick for ServiceApp
*/

#pragma once

#include "ClientConnectionManagement.h"	// 30-Jan-13 jeh
//#include "CCM_PAG.h"					// 30-Jan-13 jeh


// CTestThread

class CTestThread : public CWinThread
{
	DECLARE_DYNCREATE(CTestThread)

protected:
	CTestThread();           // protected constructor used by dynamic creation
	virtual ~CTestThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	afx_msg void ThreadHelloWorld(WPARAM w, LPARAM lParam);	// manually added jeh 10-24-2012
	afx_msg void Bail(WPARAM w, LPARAM lParam);		// another way to call ExitInstance??

	// Testing Nc function
	void TestNc(void);
	void TestNx(void);
	void TestAdcFifo(void);
	void InitCmdFifo(void);
	void InitSmallCmdFifo(void);
	

	ST_CLIENT_CONNECTION_MANAGEMENT *m_pstCCM;	// pointer to my global structure instance 
	WORD m_MsgId[256];
protected:
	DECLARE_MESSAGE_MAP()
};


