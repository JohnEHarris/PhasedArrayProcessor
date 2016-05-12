/*
Name:	Global.h
Purpose:	Group all variable which are global to a processor into
			one module
Date:	02/22/02
Author:	jeh
Revised:
*/


#ifndef	GLOBAL_H

#define	GLOBAL_H



//#ifdef MAKE_PUBLICS
#ifdef MAIN_MODULE
/* Global variables are public in the including module */
#define PubExt

#else
#define PubExt	extern
/* Global variables are external in the including module */
#endif
#include "CFG100.H"

#define UTRON_MACHINE		0x3a43
/* 	0x3a43 = C:	*/

#define TRUSCOPE_MACHINE		0x3a54
/* 	0x3a54 = T:	*/


/* Number of radial segments in image map.  Max is 60	*/

#define N_SEG				30

/* Max number of protocols to search for ipx and udp */

#define MAX_PROTOCOLS		80


/* Inch Limits for od and wall */
/* floating point and integer values in 1/1000th inch */
#define FWALL_MIN			0.09
#define NWALL_MIN			90
/* BEFORE 5-2-02  FWALL_MAX			2.1	*/
#define FWALL_MAX			4.095
#define NWALL_MAX			4095
#define FOD_MIN				2.25
#define FOD_MAX				16.0

#define VELOCITY_DX_FACTOR		200000
/* Assumes measuring over 4 inch = 8 motion pulses and computing fpm */
/* Assumes scope demux ckt using 10 khz clock to measure dt */



/**************************************************************/
/** Global parameters maintained in each element of Truscope **/
/**************************************************************/
//PubExt int gChannel, gCopyTo, gGate, gPrf, gSysInitFlag;
/*PubExt WORD wXdcrType[4][10];	/* by shoe and channel */

/*PubExt UT_OFFSET UtOffset;		/* defined in instdata.h */
PubExt BYTE bFwd;				/* x direction of travel  1 = fwd*/
PubExt CHANNEL_CONFIG2 ChannelCfg;

PubExt	int NCtolerance;
PubExt	int nShoeXOffset;	/* correct flaw loc by this amount */
class CTuboIni;

typedef struct 
	{
//	CInstSimulatorDlg *pUIDlg;		// the user interface dialog
//	CDoNothingDlg *pDoNothingDlg;
//	CInstSimulatorApp *ptheApp;
	CTuboIni *pTuboIni;
	}	GLOBAL_DLG_PTRS;

PubExt	GLOBAL_DLG_PTRS gDlg;

#endif		/* include global_h */
