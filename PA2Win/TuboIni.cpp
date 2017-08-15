/***
Purpose:	A Tuboscope wrapper around the Code Project ini handler code
Author:		jeh
Date:		12-Feb-2013
Article Link:
			http://www.codeproject.com/Articles/12914/INI-Reader-Writer-Class-for-MFC-and-ANSI-C-Windows


***/

#include "stdafx.h"
#include "TuboIni.h"
#include "..\..\Include\global.h"
#include "fcntl.h"
#include "io.h"
#include "inifile_mb_wide_mfc_vs.h"
#include "inifile.h"
#include "tstdlibs.h"

extern  GLOBAL_DLG_PTRS gDlg;


CTuboIni::CTuboIni(void)
	{
	m_pIniFILE = NULL;
	}

// Constructor takes a complete path to the INI file 
// For example D:\PhasedArrayGenerator\PA_Master_VS2010\Debug\HardwareCfg.ini
// Ini file needs to be where the executable is so that systems installed in the field 
// will match the development environment. Hence not a good idea to put the ini file with the source code.
// File must be created manually the first time
// Windows 10 doesn't seem to support stream file io. Try CFile first and derive
// stream file from it.
//
CTuboIni::CTuboIni(CString szIniFile)
	{
	m_szIniFileName = szIniFile;
	m_pIniFILE = new(CIniFile);
	gDlg.pTuboIni = this;
	TRACE1("gDlg.pTuboIni = 0x%08x\n", gDlg.pTuboIni);
	// makes sense to load the ini file into the ini memory structure
	LoadIniFile(true);
	}

CTuboIni::~CTuboIni(void)
	{
	// since we loaded it with the constuctor, may as well save it for good measure on the destructor
	if (m_pIniFILE)
		{
		SaveIniFile();
		delete m_pIniFILE;
		}
	gDlg.pTuboIni = NULL;
	}

// Using the new ini file stream code, save the memory structure to the default ini file.
// A more descriptive name would be save the memory structure to the default ini disk file.
bool CTuboIni::SaveIniFile(void)
	{
	CString fn = m_szIniFileName;
	bool ret = FALSE;
	if (fn.GetLength() < 5) return ret;	// assumes x.ini at minimum
	ret = m_pIniFILE->Save(m_szIniFileName.GetString());
	return ret;
	}

// Save to another file
bool CTuboIni::SaveIniFile(CString szFileName)
	{
	bool ret = FALSE;
	if (szFileName.GetLength() < 5) return ret;	// assumes x.ini at minimum
	ret = m_pIniFILE->Save(szFileName.GetString());
	return ret;
	}

// Good idea to load the ini structure from an existing ini file before changing/ adding sections/keys/values
// That would be a load and merge operation to initialize the memory structure
// bMerge false means no merging of file into the ini memory structure
bool CTuboIni::LoadIniFile(bool bMerge)
	{
	if (NULL == m_pIniFILE)	return FALSE;
	return m_pIniFILE->Load(m_szIniFileName.GetString(), bMerge);
	}

// Load and merge or not any file name into the ini memory structure
bool CTuboIni::LoadIniFile(CString szFileName, bool bMerge)
	{
	if (NULL == m_pIniFILE)	return FALSE;
	CString s = szFileName.GetString();	// debug/testing
	return m_pIniFILE->Load(szFileName.GetString(), bMerge);
	}



UINT CTuboIni::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszKey, int nDefault)
	{
	UINT uReturn = 0;
	CString s = GetProfileString(lpszSection, lpszKey);
	if (_T("") == s) return nDefault;
	uReturn = (unsigned)std::stoi(s.GetString());
	return uReturn;
	}

CString CTuboIni::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszDefault)
	{
	CString s = _T("");
	if (NULL == m_pIniFILE) return s;
	s = (m_pIniFILE->GetKeyValue(lpszSection, lpszKey)).c_str();
	if (_T("") == s) s = lpszDefault;
	return s;
	}

// Write the section header before setting any key values
BOOL CTuboIni::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszKey, int nValue)
	{
	CString s;
	s.Format(_T("%d"), nValue);
	WriteProfileString(lpszSection, lpszKey, s);
	return TRUE;
	}

// Write the section header before setting any key values
BOOL CTuboIni::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszValue)
	{
	m_pIniFILE->AddSection(lpszSection)->AddKey(lpszKey)->SetValue(lpszValue);
	return TRUE;
	}

void CTuboIni::SaveWindowLastPosition(LPCTSTR lpszSection, LPCTSTR lpszKey, WINDOWPLACEMENT *wp)
	{
	// Input is name of dialog to save and last position of window
	CString s;	// string showing top, bottom, left & right
	RECT *rect;

	// add a comment which we hope appears at the top of the section
	CString CommentKey = _T("! format");
	CString sComment = _T("top, bottom, left, right");
	WriteProfileString(lpszSection, CommentKey, sComment);

	rect = (RECT *)&wp->rcNormalPosition;
	s.Format(_T("%4d,%4d,%4d,%4d"), rect->top, rect->bottom,
		rect->left, rect->right);
	WriteProfileString(lpszSection, lpszKey, s);

	}

void CTuboIni::GetWindowLastPosition(LPCTSTR lpszSection, LPCTSTR lpszKey, RECT *rect)
	{
	// Input is section:entry ascii value
	CString s;	// string showing top, bottom, left & right
	//RECT *rect;
	char t[60];
	int i;
	int n;

	s = this->GetProfileString(lpszSection, lpszKey, _T(""));
	memset(t, 0, 60);
	n = s.GetLength();
	if ( n < 4 )
		{
		rect->top = 0;
		rect->bottom = 2;
		rect->left = 0;
		rect->right = 2;
		return;
		}
	for ( i = 0; i < s.GetLength(); i++ ) t[i] = (char)s.GetAt(i);
	t[i] = 0;
	//	_tcscpy(t,s);
	//	sscanf(t, _T("%d,%d,%d,%d"), &rect->top, &rect->bottom, 
	sscanf(t, "%d,%d,%d,%d", &rect->top, &rect->bottom,
		&rect->left, &rect->right);

	}