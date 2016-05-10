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

	HANDLE	m_hTimerTick;
	ST_CLIENT_CONNECTION_MANAGEMENT *m_pstCCM;	// pointer to my global structure instance 

protected:
	DECLARE_MESSAGE_MAP()
};


