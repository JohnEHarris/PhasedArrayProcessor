// MemFileA.cpp: implementation of the CMemFileA class.
// Clive Lam 1/30/01
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Truscan.h"
#include "TscanDlg.h"

#include "Extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemFileA::CMemFileA()
	{
	m_initFlag = FALSE;		//to inidicate that 
							//the class can only be for Server or Client 
	m_iFileSize = 4096;		//the default file size
							//the file size can be change before calling
							//initServer or initClient

	}

CMemFileA::~CMemFileA()
	{
	if(m_pShareMem)
		UnmapViewOfFile(m_pShareMem);
	if(m_MapHandle)
		CloseHandle(m_MapHandle);
	if(m_MapFileHandle)
		CloseHandle(m_MapFileHandle);

	}

//Setup the MemMapFile for Cilent
//there could be many clients but only only one server
//The address pointer is m_pShareMem
BOOL CMemFileA::initClient()
	{
	DWORD errValue;
	CString t;

	if(m_initFlag)
		{
		m_csErrMsg += "Error: Dup init Client";
		return TRUE;
		}
	m_MapHandle = OpenFileMapping(
		FILE_MAP_WRITE | FILE_MAP_READ,  // access mode
		TRUE,    // inherit flag
		"MapName1" 	);		// pointer to name of file-mapping object

	if(!m_MapHandle)
		{
		errValue = GetLastError();
		t.Format("Error: OpenFileMapping Value = %d", errValue);
		m_csErrMsg += t;
		return TRUE;
		}
 
	m_pShareMem = MapViewOfFile(
	  m_MapHandle,							// file-mapping object to map into 
										// address space
	  FILE_MAP_WRITE | FILE_MAP_READ,	// access mode
	  0,								// high-order 32 bits of file offset
	  0,								// low-order 32 bits of file offset
	  m_iFileSize );					// number of bytes to map

	if(m_pShareMem == NULL)
		{
		m_csErrMsg += "Error: MapViewOfFile";
		return TRUE;
		}
	m_initFlag = TRUE;
	m_csErrMsg = "MemMapFile No Error";
	return FALSE;
	}

//Setup the MemMapFile for Server
//there could be many clients but only only one server
//The address pointer is m_pShareMem
BOOL CMemFileA::initServer()
	{
	if(m_initFlag)
		{
		m_csErrMsg += "Error: Dup init Server";
		return TRUE;

		}
	m_MapFileHandle= CreateFile("MapFile.001",
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);	

	if (m_MapFileHandle == INVALID_HANDLE_VALUE)
		{
		m_csErrMsg += "Error: CreateFile";
		return TRUE;
		}

 
	m_MapHandle= CreateFileMapping(m_MapFileHandle,NULL,
									PAGE_READWRITE,  0,
									4096,
									"MapName1");

	if (m_MapHandle == NULL)
		{
		m_csErrMsg += "Error: CreateFileMapping";
		return TRUE;
		}

	m_pShareMem = MapViewOfFile(
					m_MapHandle,		// file-mapping object to map into 
										// address space
					FILE_MAP_WRITE | FILE_MAP_READ,	// access mode
					0,					// high-order 32 bits of file offset
					0,					// low-order 32 bits of file offset
					4096);				// number of bytes to map

	if(m_pShareMem == NULL)
		{
		m_csErrMsg += "Error: MapViewOfFile";
		}
	m_initFlag = TRUE;
	return FALSE;

	}
