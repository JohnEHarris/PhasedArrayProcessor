#ifndef _TRUSCAN_VERSION_H_
#define _TRUSCAN_VERSION_H_

// check out in PhasedArrayHeaders project before editing!!!
#define CURRENT_VERSION		_T("Version 1.2")
#define BUILD_VERSION			12

#define VERSION_MAJOR           1
#define VERSION_MINOR           2
#define VERSION_BUILD           BUILD_VERSION
#define TRUSCAN_VERSION _T("Version = Phase Array Wall %d.%d.%d"), VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD 

#if 0
1.2.12		26-Apr-13	Child class CCM_GDP to interface with the GDP
1.2.11		12-Mar-13	Timer tick restarts stalled ConnectionClient. Uses partially implement MMI_CMD command steering
1.2.10		28-Nov-12	Move SysCp specific processing from base class ClientConnectionManagement into CCM_SysCp. Remove
						CLIENT_IDENTITY_DETAIL structure from ST_CLIENT_CONNECTION_MANAGEMENT structure and make it
						a member variable of the CCM_SysCp class. Still a little kludgie when considering using
						CCM for both PAG and PAM. Utilizes define symbol PHASED_ARRAY_GUI in Truscandlg.h to assist
						in properly configuring CLIENT_IDENTITY_DETAIL which is only used in PAG and not in PAM.
1.2.9		22-Oct-12	Cure memory leak on shutdown. Changes to TruscanDlg, ServerSocketPA_Master, ServerSocket, and
						ServerComThread.
1.2.8		20-Sep-12	Debugged Server Management System- exits w/o memory leaks. Still uses TcpThreadRxList to generate
						commands to instrument.
1.2.7		28-Aug-12	Added Server Management System
1.2.6		01-Aug-12	Connection to SysCp seems to work equivalent to TestClient example. Some string table
						strings moved from TestClient project to here.
1.2.5		23-Jul-12	Move code from SycCpTestClient for making multiple client connections to SysCp and GDP and Database.
1.2.4		21-May-12	Converting to VS2010. Eliminate everything IPX from program. This forced primarily due
						to conversion to Windows 7. Eliminate CMemFileA
1.2.3		17-Apr-12	Add routines to get min wall out of last N inputs to running avg.
1.2.2		23-Mar-12	Add classes HwTimer, RunningAverage and InstMsgProcess to handle input from/output to and
						processing of data from the Instruments.
1.2.1		15-Mar-12	Project transferred from YG to JEH. Move to VS2005 instead of VC++ 6. Change # of 
						PA probes = Instrument boxes = MAX_SHOES from 10 to 16. Put in defines to kill warnings
						for depracated string operations. Change Posix close to _close.

#endif



#define SYSTEM_NAME  _T("Phase Array Wall")

#define FOR_TEST  0    /* if 1, for test only, no hardware is used */

#define END_AREA_SYS  0
#define LORAINE_SYS   1
#define RAW_WALL_SYS  2
#define BESSEMER_SYS  3

#define SYSTEM_ID			LORAINE_SYS
#define SYSTEM_SUB_ID		BESSEMER_SYS

#define BESSEMER      0
#define AMELIA_NORTH  1
#define SHELDON_SCAN3 2

#define LOCATION_ID   AMELIA_NORTH

#define RECORD_RAW_WALL 1    //set it to 1 only if (SYSTEM_ID==LORAINE_SYS) AND (SYSTEM_SUB_ID==BESSEMER_SYS), otherwise set to 0

#if (LOCATION_ID != BESSEMER)
#define HIDE_ASCAN 0
#else
#define HIDE_ASCAN 1
#endif

#define ADI_REVISION		0xE3
#define TWOCHNL_REVISION	0xE2

#endif  /* one time through */