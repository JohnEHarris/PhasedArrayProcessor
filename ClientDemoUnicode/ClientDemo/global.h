/*
Define global dialog ptrs.
*/

#ifndef GLOBAL_H
#define GLOBAL_H

// global.h includes all other headers except the application and the main dialog
//#include "ClientDemoDlg.h"
#include "NcNx.h"
#include "ClientSocket.h"

class CCommDemoDlg;

typedef struct
	{
	CCommDemoDlg *pUIDlg;	// the user interface dialog
	CNcNx *pNcNx;
	} GLOBAL_DLG_PTRS;

#ifdef MAIN_MODULE
#define PUBEXT
#else
#define PUBEXT extern
#endif

PUBEXT GLOBAL_DLG_PTRS gDlg;



#endif

