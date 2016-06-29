
/*******************************************************
Author: jeh
Date:	26-Apr-2013
Purpose:	Derive a specific class from the general base class ClientConnectionManagement
			Since all mill console programs have to talk to the GDP, add a class to do it.
			Copied and modified from CCM_SysCp.cpp


********************************************************/

			
#include "stdafx.h"

#include "Truscan.h"
#include "TscanDlg.h"

#include "ClientSocket.h"
#include "ClientConnectionManagement.h"
#include "ClientCommunicationThread.h"
#include "time.h"

extern THE_APP_CLASS theApp;		

#define I_AM_CCM_GDP
#include "CCM_GDP.h"

// nMyConnection selects which one of MAX_CLIENTS connections we are managing with this instance
// wOriginator selects family type from Constants, eg. TRUSCOPE_COMMAND_PROCESSOR
// This class is a child of the base class CClientConnectionManagement. It was crafted specifically
// for handling communications with the GDP

CCCM_GDP::CCCM_GDP(int nMyConnection, USHORT wOriginator) : CClientConnectionManagement(nMyConnection, wOriginator)
	{
	// call the base constructor explicitly above
	TRACE("CCM_GDP Constructor called\n");
	CString stmp;
	m_pstCCM->pCCM = this;
	}


CCCM_GDP::~CCCM_GDP(void)
	{
	TRACE("CCM_GDP Destructor called\n");
	// base destructor called implicity after this destructor runs
	}


	// not part of the base class... debug
void CCCM_GDP::UniqueProc(void)
	{
	TRACE("Unique to the child class\n");
	}