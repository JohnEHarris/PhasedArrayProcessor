#pragma once
#include "afx.h"
#include "dictionary.h"
class CTuboIni :
	public CObject
	{
	public:
		CTuboIni(void);
		CTuboIni(CString szIniFile);
		~CTuboIni(void);
		UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
		CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL);

		BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
		BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);

		CString m_szIniFileName;
		dictionary *m_pDictionary;
		FILE *m_pIniFILE;
	};

