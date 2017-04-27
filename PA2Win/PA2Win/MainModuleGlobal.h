// File name:	MainModuleGlobal.h
// Purpose:		Pass MAIN_MODULE define constant into global.h
// jeh 12-02-2010


#define MAIN_MODULE
#include "..\Include\global.h"
//#include "Lang.h"

// This technique successfully passes the constant MAIN_MODULE into the global.h
// include file.  Defining MAIN_MODULE in the main module cpp does not do this.

