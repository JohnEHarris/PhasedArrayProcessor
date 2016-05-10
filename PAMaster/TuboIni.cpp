/***
Purpose:	A Tuboscope wrapper around the MIT developed ANSI C IniParser program
Author:		jeh
Date:		12-Feb-2013


***/

#include "stdafx.h"
#include "dictionary.h"
#include "iniparser.h"
#include "TuboIni.h"

void CstringToChar(CString s, char *pChar);

CTuboIni::CTuboIni(void)
	{
	}

// Constructor takes a complete path to the INI file 
// For example D:\PhasedArrayGenerator\PA_Master_VS2010\Debug\HardwareCfg.ini
// Ini file needs to be where the executable is so that systems installed in the field 
// will match the development environment. Hence not a good idea to put the ini file with the source code.
//
CTuboIni::CTuboIni(CString szIniFile)
	{
	char fn[256];
	m_pIniFILE = NULL;
	m_pDictionary = NULL;
	if (szIniFile.IsEmpty())
		{
		ASSERT(0);
		return;
		}
	m_szIniFileName = szIniFile;
	CstringToChar( szIniFile, fn);
	m_pIniFILE = fopen(fn, "r+" );
	m_pDictionary = iniparser_load(fn);
	}

CTuboIni::~CTuboIni(void)
	{
	if (m_pIniFILE)	// save the ini structure back into the file
		{
		if (m_pDictionary)
			{

			// So why would we not want to rewrite the dictionary back into the file?
			// Because if we edit the file in notepad, we can add comments which are ignored.
			// When the dictionary is written into the file, the comments are deleted.
#ifdef CREATE_NEW_INI_FILE
			iniparser_dump_ini(m_pDictionary,m_pIniFILE);
#endif
			iniparser_freedict(m_pDictionary);
			m_pDictionary = NULL;
			}
		fclose(m_pIniFILE);
		m_pIniFILE = NULL;
		}
	}

UINT CTuboIni::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
	{
	UINT uReturn;
	char ch[256];
	if (NULL == m_pIniFILE)		return 0;
	if (NULL == m_pDictionary)	return 0;
	CString s = lpszSection;
	s += _T(":");
	s += lpszEntry;		// search is on 'section:keyword' in lower case
	s.MakeLower();
	CstringToChar( s, ch);
	uReturn = iniparser_getint(m_pDictionary, ch, nDefault);
	return uReturn;
	}

CString CTuboIni::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
	{
	CString s = _T("");
	char ch[256], def[256];
	if (NULL == m_pIniFILE)		return s;
	if (NULL == m_pDictionary)	return s;
	s = lpszSection;
	s += _T(":");
	s += lpszEntry;		// search is on 'section:keyword' in lower case
	s.MakeLower();
	CstringToChar( s, ch);
	s = lpszDefault;
	CstringToChar( s, def);
	char *p = iniparser_getstring(m_pDictionary, ch, def);
	strcpy(ch,p);
	s = ch;
	return s;
	}

// Write the section header before setting any key values
BOOL CTuboIni::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
	{
	CString s;
#if 0
	char ch[256], val[32];
	int nReturn;
	if (NULL == m_pIniFILE)		return FALSE;
	if (NULL == m_pDictionary)	return FALSE;
	CString s = lpszSection;
	s += _T(":");
	s += lpszEntry;		// search is on 'section:keyword' in lower case
	s.MakeLower();
	CstringToChar( s, ch);
#endif
	s.Format(_T("%d"),nValue);
	return WriteProfileString(lpszSection, lpszEntry, s.GetString());
#if 0
	CstringToChar( s, val);
	nReturn = iniparser_set(m_pDictionary,ch, val);
	if (!nReturn)	return FALSE;
	return TRUE;
#endif
	}

// Write the section header before setting any key values
BOOL CTuboIni::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
	{
	CString s = _T("");
	CString t = _T("");
	int nReturn;
	char ch[256], val[512];
	if (NULL == m_pIniFILE)		return FALSE;
	if (NULL == m_pDictionary)	return FALSE;
	s = lpszSection;
	t = lpszEntry;
	// test for section header insertion
	if (!t.IsEmpty())
		{
		s += _T(":");
		s += t;			// search is on 'section:keyword' in lower case
		}
	//s.MakeLower();
	CstringToChar( s, ch);
	s = lpszValue;
	CstringToChar( s, val);
	nReturn = iniparser_set(m_pDictionary,ch, val);	// 0 retrun means no error
	if (!nReturn)	return TRUE;
	return FALSE;
	}
