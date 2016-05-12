#pragma once
#include "afx.h"
#include "Inifile.h"


class CTuboIni :
	public CObject
	{
	public:
		CTuboIni(void);
		CTuboIni(CString szIniFile);
		~CTuboIni(void);
		UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszKey, int nDefault);
		CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszDefault = NULL);

		BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszKey, int nValue);

		BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszValue);

		void SaveWindowLastPosition(LPCTSTR lpszSection, LPCTSTR lpszKey, WINDOWPLACEMENT *wp);
		void GetWindowLastPosition(LPCTSTR lpszSection, LPCTSTR lpszKey, RECT *rect);
		bool SaveIniFile(void);   // the file specified in the constructor
		bool SaveIniFile(CString szFileName);   // any file name
		bool LoadIniFile(bool bMerge = false);  // merge the ini file contents into the ini memory structured or not
		bool LoadIniFile(CString szFileName, bool bMerge = false);  // merge any ini file contents into the ini memory structured or not

		CIniFile *m_pIniFILE;
		CString m_szIniFileName;
	};

