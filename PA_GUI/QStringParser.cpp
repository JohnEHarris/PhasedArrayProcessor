//------------------------------------------------------------------------------
// FILE NAME:	QStringParser.cpp
// HEADER:		QStringParser.h
//
// PURPOSE:		Implementation of the CQStringParser class.
//
// DESCRIPTION:	This class acepts a string and immediately parses it into 
//				separate fields based on the delimiter character specified by 
//				the programmer.  The separated fields are stored in a 
//				CStringArray, and the 0th element in the array contains the 
//				original string.  Access to the separated fields is 1-based 
//				(the first field is in index 1, and so on).
//
//				If you are parsing quoted strings, it is IMPARATIVE that the 
//				quoted strings be formatted correctly. Otherwise you will get 
//				what appears to be bizarre results.
//
// DISCLAIMER:	At first glance, it may seem like the code is somewhat verbose 
//				and I might have taken the long way around to perform some 
//				tasks, but I think it makes it easier for someone else to follow 
//				when I do that.  Maintainability is probably the most important 
//				aspect of coding (after reliability of course).
//
// LEGAL STUFF:	You're welcomed to use this code in any project that might 
//				benefit from it, as long as you don't change any of the original 
//				text in this comment block.
//
// REQUEST:		If you make changes to this code, please comment your changes 
//				in the same style as has already been done.
//
// TO DO:		When I have a chance (and the motivation), I want to add support 
//				for bracketed sub-strings. In other words, the following string: 
//
//					"this string, {another, field}"
//
//				would parse out to these fields:
//
//					this string
//					{another, field}
//
// AUTHOR:		John Simmons
//				john@paddedwall.org
//
// CHANGE HISTORY:
// DATE			INIT	CHANGE DESCRIPTION
// --------		----	--------------------------------------------------------
// 07/20/2000	jms		Initial code completed.
//
// 08/09/2000	jms		Added a way to submit a new string to facilitate the use 
//						of the same object instance when a subsewquent string 
//						needs to be parsed.
//
// 01/12/2001	jms		Added a way to find strings withion the parsed results.
//
// 01/20/2001	jms		Added support for quoted strings.
//
// 02/25/2001   jms     Added the following functions:
//						  AddField                   (public) 
//						  SetField                   (public)
//
// 05/06/2001   jms     1) I simplified the code by implementing the following 
//						   changes:
//						   a) Deleted the version of the constructor ythat did 
//						      not accpet a quote character
//						   b) Change the remaining constructor so that the quote 
//						      character parameter has a default value of '\0'.
//						   c) Deleted the version of ResetOriginalString() that 
//						      did not accept a quote character.
//						   d) Changed the remaining ResetOriginalString()
//						      function so that the cQuoter parameter has a 
//						      default value of '\0'.
//						   e) Deleted the function ParseString(), and renamed 
//						      the function ParseStringWithQuoter() to 
//						      ParseString(). 
//						2)	Added the following functions:
//							InsertField                (public) 
//							DeleteField                (public)
//							RebuildOriginalString()    (private)
// 02/22/2002	jms		Fixed out-of-range error when parsing a string that 
//						looks like this:
//
//							command "c:\documents and settings"
//
//						and parsed like this:
//
//							CQStdStringParser *sp = new CQStdStringParser test,' ','\"');
//
// 03/15/2002	jms		Added code to handle the absence of a delimiter 
//						character.
//------------------------------------------------------------------------------

#include "stdafx.h"

#include "QStringParser.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//------------------------------------------------------------------------------
// NAME:		CQStringParser::CQStringParser()
//
// PURPOSE:		Constructor - initializes the array of strings and calls the 
//				ParseString() function.
//
// PARMETERS:	CString sString		The string to be parsed.
//				char cDelimiter		The delimiter character used to parse the 
//									string.
//				char cQuoter		The quote character we should be looking 
//									for.
//
// RETURNS:		N/A
//
// CHANGE LOG:
// DATE			INIT	DESCRIPTION
// ----------	----	--------------------------------------------------------
// 01/20/2001	jms		Added overloaded version of constructor to support 
//						quoted strings.
//------------------------------------------------------------------------------
CQStringParser::CQStringParser(CString sString, char cDelimiter, char cQuoter/*='\0'*/)
{
	m_aStrings.SetSize(0,10);
	ResetOriginalString(sString, cDelimiter, cQuoter);
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::~CQStringParser()
//
// PURPOSE:		Destructor - removes all of the array elements from the 
//				CStringArray.
//
// PARMETERS:	None
//
// RETURNS:		None
//
// CHANGE LOG:
// DATE        INIT  DESCRIPTION
// ----------  ----  -----------------------------------------------------------
// 07/20/2000  jms   Initial development.
// 08/09/2000  jms   Added call to new Clear() function.
//------------------------------------------------------------------------------
CQStringParser::~CQStringParser()
{
	Clear();
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::Clear()
//
// PURPOSE:		Clears the string table.
//
// PARMETERS:	None
//
// RETURNS:		None
//
// CHANGE LOG:
// DATE        INIT  DESCRIPTION
// ----------  ----  -----------------------------------------------------------
// 08/09/2000  jms   Added function.
//------------------------------------------------------------------------------
void CQStringParser::Clear()
{
	if (m_aStrings.GetSize() > 0)
	{
		m_aStrings.RemoveAll();
		m_aStrings.FreeExtra();
	}
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::ResetOriginalString()
//
// PURPOSE:		Clears the string table.
//
// PARMETERS:	None
//
// RETURNS:		None
//
// CHANGE LOG:
// DATE        INIT  DESCRIPTION
// ----------  ----  -----------------------------------------------------------
// 08/09/2000  jms   Added function.
//------------------------------------------------------------------------------
void CQStringParser::ResetOriginalString(CString sString, char cDelimiter, char cQuoter/*='\0'*/)
{
	Clear();
	m_aStrings.Add(sString);
	m_cDelimiter = cDelimiter;
	m_cQuoter    = cQuoter;
	m_nCount     = 0;
	ParseString();
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::GetField()
//
// PURPOSE:		Returns the CString stored in the array element specified by 
//				the nIndex parameter.  If the specified index is greater than 
//				the number of elements, it is an error.
//
// PARMETERS:	int nIndex			The index of the desired array item.
//				BOOL bStripQuotes	Strip the quotes out of quoted fields. 
//									Default value is FALSE.
//
// RETURNS:		CString sBuffer	the string retrieved from the array.
//
// CHANGE LOG:
// DATE        INIT  DESCRIPTION
// ----------  ----  -----------------------------------------------------------
// 07/20/2000  jms   Initial development.
// 05/06/2001  jms   Added bStripQuotes parameter (default value is FALSE). 
//------------------------------------------------------------------------------
CString CQStringParser::GetField(int nIndex, BOOL bStripQuotes/*=FALSE*/)
{
	CString sBuffer;
	sBuffer.Empty();
	if (m_nCount >= nIndex)
	{
		sBuffer = m_aStrings.GetAt(nIndex);
		if (bStripQuotes)
		{
			sBuffer.Remove(m_cQuoter);
		}
	}
	else
	{
		sBuffer = "ERROR: Array index out of range.";
	}
	return sBuffer;
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::ParseStringWithQuoter()
//
// PURPOSE:		Separates the fields out of a copy of the original string by 
//				searching for the specified delimiter character.  As the string 
//				is searched, it becomes shorter by deleting the most recently 
//				found field (and the next delimiter, if one exists) from the 
//				beginning of itself.
//
//				This version of ParseString handles a quoted string. A quoted 
//				string may have the specified delimiter in an embedded between 
//				quote characters, and this instance of the delimiter must be 
//				ignored.
//
// PARMETERS:	None
//
// RETURNS:		int		The number of fields found in the original string.
//
// CHANGE LOG:
// DATE			INIT	DESCRIPTION
// ----------	----	-----------------------------------------------------------
// 01/20/2001	jms		Added this function.
// 03/15/2002   jms		Added code to handle a string passed in with no 
//						delimiter character specified.  This is actually a bug 
//						in the calling function, but there's no reason it should 
//						be able to recover gracefully.
//------------------------------------------------------------------------------
//int CQStringParser::ParseStringWithQuoter()
int CQStringParser::ParseString()
{
	// get a copy of the string
	CString sWorkString = GetOriginalString();
	CString sBuffer;
	int     pos = 0;
	BOOL    m_bLastFieldEmpty;

	
	if (m_cDelimiter == '\0')
	{
		m_aStrings.Add(sWorkString);
		m_nCount = 1;
		return m_nCount;
	}
	
	if (sWorkString.IsEmpty())
	{
		m_bLastFieldEmpty = TRUE;
	}
	else
	{
		m_bLastFieldEmpty = (sWorkString.GetAt(sWorkString.GetLength()-1) == m_cDelimiter);
	}
	m_nCount    = 0;

	// while the string is not empty...
	while (!sWorkString.IsEmpty())
	{
		// find the position of the next delimiter
		pos = sWorkString.Find(m_cDelimiter);

		// if a delimiter is found
		if (pos >= 0)
		{
			// if qwe have to wqorry about quoted strings
			if (m_cQuoter != '\0')
			{
				// if the first character is the specified quoter
				if (sWorkString.GetAt(0) == m_cQuoter)
				{
					int nLength = sWorkString.GetLength();
					// we have work to do
					sBuffer.Empty();
					int nStrPos = 0;
					BOOL bDone = FALSE;
					BOOL bQuoting = TRUE;

					do
					{
						char cCharToAdd = (char)sWorkString.GetAt(nStrPos);
						if (bQuoting)
						{
							//add the character
							sBuffer	+= CString(cCharToAdd);
							nStrPos++;
							// see if we're done with the quoted string
							if (nStrPos > 1 && sBuffer.GetAt(nStrPos - 1) == m_cQuoter)
							{
								bQuoting = FALSE;
							}
						}
						else
						{
							if (cCharToAdd != m_cDelimiter)
							{
								//add the character
								sBuffer	+= CString(cCharToAdd);
								nStrPos++;
							}
							else
							{
								// we are done
								bDone = TRUE;
								pos = sBuffer.GetLength();
							}
						}
						if (nStrPos >= nLength)
						{
							pos   = sBuffer.GetLength() - 1;
							bDone = TRUE;
						}
					} while (!bDone);
				}
				else // normal string (un-quoted)
				{
					sBuffer     = sWorkString.Left(pos);
				}
			}
			else // don't have to worry about quoted strings
			{
				sBuffer     = sWorkString.Left(pos);
			}
			// adjust our work string
			sWorkString = sWorkString.Mid(pos + 1);
		}
		// otherwise, if a delimiter isn't found
		else
		{
			// the rest of the string is a field
			sBuffer = sWorkString;
			// and make the string empty
			sWorkString.Empty();
		}
		// add the field to the CStringArray
		m_aStrings.Add(sBuffer);
		// increment the counter
		m_nCount++;
	}

	if (m_bLastFieldEmpty)
	{
		m_aStrings.Add("");
		m_nCount++;
	}

	return m_nCount;
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::FindExact()
//
// PURPOSE:		Searches the entire array (excluding item 0) for an exact match 
//				of the specified string.
//
// PARMETERS:	CString sText			The string we're looking for
//				int& nElement			The element number of the found string 
//										or SP_NOTFOUND if not found.
//				BOOL bCaseSensitive		Case sensitivity matters
//
// RETURNS:		CString					The matching string if any.
//
// CHANGE LOG:
// DATE        INIT  DESCRIPTION
// ----------  ----  -----------------------------------------------------------
// 01/12/2001  jms   Added function.
//------------------------------------------------------------------------------
CString CQStringParser::FindExact(CString sText, int* nElement, BOOL bCaseSensitive/*=FALSE*/)
{
	if (!bCaseSensitive)
	{
		sText.MakeUpper();
	}
	*nElement = SP_NOTFOUND;

	CString sResult = "";
	BOOL    bFound  = FALSE;

	for (int i = 1; i <= m_nCount; i++)
	{
		sResult = m_aStrings.GetAt(i);
		if (!bCaseSensitive)
		{
			sResult.MakeUpper();
		}
		if (sResult == sText)
		{
			*nElement = i;
			sResult = m_aStrings.GetAt(i);
			bFound = TRUE;
			break;
		}
		sResult.Empty();
	}
	return sResult;
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::Find()
//
// PURPOSE:		Searches the entire array (excluding item 0) for any string that 
//				contains the  specified string.
//
// PARMETERS:	CString sText			The string we're looking for
//				int& nElement			The element number of the found string 
//										or SP_NOTFOUND if not found.
//				BOOL bCaseSensitive		Case sensitivity matters
//
// RETURNS:		CString					The matching string if any.
//
// CHANGE LOG:
// DATE      INIT  DESCRIPTION
// --------  ----  -------------------------------------------------------------
// 01/12/01  jms   Added function.
//------------------------------------------------------------------------------
CString CQStringParser::Find(CString sText, int* nElement, BOOL bCaseSensitive/*=FALSE*/)
{
	if (!bCaseSensitive)
	{
		sText.MakeUpper();
	}
	*nElement = SP_NOTFOUND;

	CString sResult = "";
	BOOL    bFound = FALSE;
	for (int i = 1; i <= m_nCount; i++)
	{
		sResult = m_aStrings.GetAt(i);
		if (!bCaseSensitive)
		{
			sResult.MakeUpper();
		}
		if (sResult.Find(sText) >= 0)
		{
			//save the position at which we found the string
			*nElement = i;
			// get it again (so we can get it in it's native form)
			sResult = m_aStrings.GetAt(i);
			bFound = TRUE;
			break;
		}
		sResult.Empty();
	}
	return sResult;
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::AddField()
//
// PURPOSE:		Adds a new field to the original string, and re-parses the 
//				string.
//
// PARMETERS:	CString sText			The string we're adding
//
// RETURNS:		None.
//
// CHANGE LOG:
// DATE      INIT  DESCRIPTION
// --------  ----  -------------------------------------------------------------
// 02/25/01  jms   Added function.
//------------------------------------------------------------------------------
void CQStringParser::AddField(CString sText)
{
	CString original = GetOriginalString() + CString(m_cDelimiter) + sText;
	ResetOriginalString(original, m_cDelimiter, m_cQuoter);
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::SetField()
//
// PURPOSE:		Changes the value of the specified a field as well as the 
//				original string.
//
// PARMETERS:	int nIndex				Index of the string we're changing
//				CString sText			The new string value
//
// RETURNS:		None.
//
// CHANGE LOG:
// DATE      INIT  DESCRIPTION
// --------  ----  -------------------------------------------------------------
// 02/25/01  jms   Added function.
//------------------------------------------------------------------------------
void CQStringParser::SetField(int nIndex, CString sText)
{
	if (nIndex > 0 && nIndex <= m_nCount)
	{
		m_aStrings.SetAt(nIndex, sText);
		RebuildOriginalString();
	}
	else
	{
		// error
	}
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::InsertField()
//
// PURPOSE:		Inserts a new field at the specified index and rebuilds the 
//				original string.  If the index exceeds the current size of the 
//				array, the field is added at the end of the array.
//
// PARMETERS:	int nIndex				Where we'll be inserting the new field
//				CString sText			The string we're inserting.
//
// RETURNS:		None.
//
// CHANGE LOG:
// DATE      INIT  DESCRIPTION
// --------  ----  -------------------------------------------------------------
// 05/06/01  jms   Added function.
//------------------------------------------------------------------------------
void CQStringParser::InsertField(int nIndex, CString sText)
{
	nIndex = max(nIndex, 1);
	if (nIndex > m_nCount)
	{
		AddField(sText);
	}
	else
	{
		m_aStrings.InsertAt(nIndex, (LPCTSTR)sText);
		m_nCount++;
		RebuildOriginalString();
		m_aStrings.FreeExtra();
	}
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::DeleteField()
//
// PURPOSE:		Deletes the field at the specified index, and adjusts the 
//				original string to match.
//
// PARMETERS:	int nIndex			The index of the string we're deleting.
//
// RETURNS:		None.
//
// CHANGE LOG:
// DATE      INIT  DESCRIPTION
// --------  ----  -------------------------------------------------------------
// 05/06/01  jms   Added function.
//------------------------------------------------------------------------------
void CQStringParser::DeleteField(int nIndex)
{
	if (nIndex > 0 && nIndex <= m_nCount)
	{
		m_aStrings.RemoveAt(nIndex);
		m_aStrings.FreeExtra();
		m_nCount--;
		RebuildOriginalString();
	}
	else
	{
		// error
	}
}


//------------------------------------------------------------------------------
// NAME:		CQStringParser::RebuildResetOriginalString()
//
// PURPOSE:		Rebuilds the original string and puts it in the array.
//
// PARMETERS:	None.
//
// RETURNS:		None.
//
// CHANGE LOG:
// DATE      INIT  DESCRIPTION
// --------  ----  -------------------------------------------------------------
// 05/06/01  jms   Added function.
//------------------------------------------------------------------------------
void CQStringParser::RebuildOriginalString()
{
	CString sTemp;
	for (int i = 1; i <= m_nCount; i++)
	{
		sTemp += m_aStrings.GetAt(i);
		if (i < m_nCount)
		{
			sTemp += CString(m_cDelimiter);
		}
	}
	m_aStrings.SetAt(0, sTemp);
}

