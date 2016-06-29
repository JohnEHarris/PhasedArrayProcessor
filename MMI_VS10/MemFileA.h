// MemFileA.h: interface for the CMemFileA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMFILEA_H__65DE8781_F792_11D4_ABF1_00A0C91FC401__INCLUDED_)
#define AFX_MEMFILEA_H__65DE8781_F792_11D4_ABF1_00A0C91FC401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMemFileA  
{
public:
	CMemFileA();
	virtual ~CMemFileA();
	BOOL m_initFlag;
	BOOL initServer();
	BOOL initClient();
	LPVOID m_pShareMem;
	int m_iFileSize;
	CString m_csErrMsg;
	HANDLE m_MapHandle;
	HANDLE m_MapFileHandle;
};

#endif // !defined(AFX_MEMFILEA_H__65DE8781_F792_11D4_ABF1_00A0C91FC401__INCLUDED_)
