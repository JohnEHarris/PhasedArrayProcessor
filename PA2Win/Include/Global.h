#pragma once
/*
Name:	Global.h
Purpose:	Group all variable which are global to a processor into
			one module
Date:	02/22/02
Author:	jeh
Revised: 2017-04-20 for PA2WIN

*/


//#ifdef MAKE_PUBLICS
#ifdef MAIN_MODULE
/* Global variables are public in the including module */
#define PubExt

#else
#define PubExt	extern
/* Global variables are external in the including module */
#endif

class CTuboIni;
class CPA2WinDlg;
class CPA2WinApp;
class CTuboIni;
class CNcNx;

typedef struct 
	{
	CPA2WinDlg *pUIDlg;		// the user interface dialog
//	CDoNothingDlg *pDoNothingDlg;
	CPA2WinApp *ptheApp;
	CTuboIni *pTuboIni;		// not actually a dialog, but no harm, no foul
	CNcNx *pNcNx;			// Nc Nx test dialog
	}	GLOBAL_DLG_PTRS;

PubExt	GLOBAL_DLG_PTRS gDlg;
PubExt int gMaxChnlsPerMainBang;
PubExt int gMaxSeqCount;
PubExt int gnMaxServers, gnMaxClientsPerServer;		// Server Connection Management
PubExt int gnMaxClients;							// Client Connection Management
PubExt int gnFifoCnt, gnAsyncSocketCnt;
