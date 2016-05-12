/*
File:    Global.h
Purpose: Group global elements such as pointers to classes into one file
Author:  Jeh
Date:    2016-01-11
Revision:	2016-05-12 moved from InstSimvs2015 (dialog program) to here.

*/

#ifndef GLOBAL_H
#define GLOBAL_H


#ifdef MAIN_MODULE
#define PUBEXT	
#else
#define PUBEXT	extern
#endif

/******************Global Defines *********************************/
#define	MAX_CLIENTS				4		
// edit this value if more client connections to servers are needed
/* Number of radial segments in image map.  Max is 60	*/



#define N_SEG				30

/* Max number of protocols to search for ipx and udp */
#if 0

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

/******************Global Defines *********************************/

#define WM_USER_KILL_COM_DLG						WM_USER+0x200
#define WM_USER_KILL_ADP_CONNECTION					WM_USER+0x201
#define WM_USER_RESTART_ADP_CONNECTION				WM_USER+0x202
#define WM_USER_INIT_TCP_THREAD						WM_USER+0x207
#define WM_USER_RESTART_TCP_COM_DLG					WM_USER+0x208
#define WM_USER_DO_NOTHING							WM_USER+0x209		// for debugging purposes
#define WM_USER_SERVER_SEND_PACKET					WM_USER+0x20A		// post thread msg when Server needs to send packet
#define WM_USER_CLOSE_TCPIP_CONNECTION				WM_USER+0x210
#define WM_USER_SEND_TCPIP_PACKET					WM_USER+0x211
#define WM_USER_CLIENT_PKT_RECEIVED					WM_USER+0x212
// Adding/deleting connections from PA Masters to PA GUI
#define WM_USER_ADD_PA_MASTER						WM_USER+0x213
#define WM_USER_DELETE_PA_MASTER					WM_USER+0x214
#define WM_USER_INIT_LISTNER_THREAD					WM_USER+0x215
#define WM_USER_STOP_LISTNER_THREAD					WM_USER+0x216

//#define WM_USER_INIT_COMMUNICATION_THREAD			WM_USER+0x217
#define WM_USER_KILL_COMMUNICATION_THREAD			WM_USER+0x218
#define WM_USER_INIT_RUNNING_AVERAGE				WM_USER+0x219		// not used in PAG
#define WM_USER_SERVERSOCKET_PKT_RECEIVED			WM_USER+0x21A
#define WM_USER_TIMER_TICK							WM_USER+0x21B


#endif


/****************** Structures ************************************/

/****************** Structures ************************************/


/*********** INCLUDE FILES WHICH NEED GLOBAL VISIBILITY ***********/
/*********** INCLUDE FILES WHICH NEED GLOBAL VISIBILITY ***********/
// Generally these will be modeless dialogs or threads

#include "resource.h"
//#include "DoNothingDlg.h"

//#include "../TmpInc/cfg100.h"
//#include "../TmpInc/udp_msg.h"
//#include "../TmpInc/ut_msg.h"
//#include "../TmpInc/nios_msg.h"
#include "TuboIni.h"				// 2016-02-09

/*********** INCLUDE FILES WHICH NEED GLOBAL VISIBILITY ***********/
/*********** INCLUDE FILES WHICH NEED GLOBAL VISIBILITY ***********/

// A structure to hold pointers to all dialogs contained in this application.
// Same capability has previously been done with static variable m_pDlg
// in every dialog. This change puts all the dialog pointers into one concise
// structure and cuts down on typing in the executable code. 04-Mar-2011
// These are usually modeless dialogs for which we want only one copy running
// at a given time.
//

//class CInstSimulatorDlg;
//class CInstSimulatorApp;
class CTuboIni;

typedef struct 
	{
//	CInstSimulatorDlg *pUIDlg;		// the user interface dialog
//	CDoNothingDlg *pDoNothingDlg;
//	CInstSimulatorApp *ptheApp;
	CTuboIni *pTuboIni;
	}	GLOBAL_DLG_PTRS;

PUBEXT	GLOBAL_DLG_PTRS gDlg;
//PUBEXT FILE *pIniFILE;
//PUBEXT dictionary *pDictionary;


/**************************************************************/
/** Global parameters maintained in each element of Truscope **/
/**************************************************************/
PUBEXT int gChannel, gCopyTo, gGate, gPrf, gSysInitFlag;
/*PubExt WORD wXdcrType[4][10];	/* by shoe and channel */

/*PubExt UT_OFFSET UtOffset;		/* defined in instdata.h */
PUBEXT BYTE bFwd;				/* x direction of travel  1 = fwd*/
PUBEXT CHANNEL_CONFIG2 ChannelCfg;

PUBEXT	int NCtolerance;
PUBEXT	int nShoeXOffset;	/* correct flaw loc by this amount */

#endif
