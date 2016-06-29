//------------------------------------------------------------------------------
// FILE NAME:	StringParser.h
//
// PURPOSE:		Definition of the CStringParser class.
//
// DESCRIPTION:	This class acepts a string and immediately parses it into 
//				separate fields based on the delimiter character specified by 
//				the programmer.  The separated fields are stored in a 
//				CStringArray, and the 0th element in the array contains the 
//				original string.  Access to the separated fields is 1-based 
//				(the first field is in index 1, and so on).
//
// REVISION LOG:
// DATE      INIT  REVISION  COMMENT
// --------  ----  --------  ---------------------------------------------------
//
// CHANGE HISTORY:
// DATE      INIT  CHANGE DESCRIPTION
// --------  ----  -------------------------------------------------------------
// 07/15/00  jms   New file
//------------------------------------------------------------------------------

#ifndef __QSTRINGPARSER_H
#define __QSTRINGPARSER_H

#include <afxcoll.h>

const int SP_NOTFOUND = -1;

class CQStringParser: public CObject
{
private:
	// data members
	CStringArray m_aStrings;
	int          m_nCount;
	char         m_cDelimiter;
	char         m_cQuoter;

	// methods
	int     ParseString();
	//int     ParseStringWithQuoter();
	void    RebuildOriginalString();

public:
	// data members

	// ctor/dtor
	//CQStringParser(CString sString, char cDelimiter);
	CQStringParser(CString sString, char cDelimiter, char cQuoter='\0');
	~CQStringParser();

	// methods
	void    Clear              ();
	CString GetField           (int nIndex, BOOL bStripQuotes=FALSE);
	int     GetCount           () { return m_nCount;            };
	CString GetOriginalString  () { return m_aStrings.GetAt(0); };
	//void    ResetOriginalString(CString sString, char cDelimiter);
	void    ResetOriginalString(CString sString, char cDelimiter, char cQuoter='\0');
	CString FindExact          (CString sText, int* nElement, BOOL bCaseSensitive=FALSE);
	CString Find               (CString sText, int* nElement, BOOL bCaseSensitive=FALSE);

	// these functions change the string *after* it has been parsed
	void    AddField           (CString sText);
	void    SetField           (int nIndex, CString sText);
	void    InsertField        (int nIndex, CString sText);
	void    DeleteField        (int nIndex);
};


#endif



