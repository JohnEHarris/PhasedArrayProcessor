/*  Copyright 1994, Tuboscope-Vetco International Inc. All rights reserved */
     /********************************************************************
                                                                        
Source File: CONTROL.002
Date:        12/08/97
History: 11/17,why:include global.h for system control and declare all
			the global variebles.  This is the main module for the Truscope
			operator interface.  It contains WinMain which is the first 
			procedure to run when the windows manager loads the program.


Revisions:	08/24/94 jeh... comments as to what is going on.
			08/30/94 jeh allow reopening config files by opening tputer link
						only once.
			09/06/94 notify when file save complete with messsage box.
					Prompt for file save on exit if unrecorded changes.
			09/07/94 if can't open file, rerun open dialog until we can
			09/14/94 change way default works.  Copy work to default at begin of
					dialog.  Default is now the starting record value, not some
					"default config file" value.
			09/15/94 make instrument adjust diaglog boxes modeless
			09/16/94 ADD an all panels command to create all 6 adjustment panels
			09/19/94 Save now saves work instead of Record.  Major change to
					SAVING data.
			09/20/94 update all instrument adjustment window after opening
					a new data file.
			09/20/94 REPEATEDLY OPENING NEW FILES causes a failure in timer
					creation for udp simulator.  Send message to dlg_stat
					to destroy self rather than destroying in-line.
			10/24/94 variable length response from transputer.  Length is
					in 1st byte of UdpCmd sent by SendToUdp
			11/03/94 to make strip chart run faster, chage bios hw clock.  Restore
					with call to warp_speed.  Not goood windows form but it workd
					here.
			11/11/94 changed lpWork to be larger than lpRecord or lpDefault
			11/17/94 customer info menu item... clive
			12/15/94 merge tof and alarm windows under tof.  Taken from Bench
					test code.
			03/02/95 add trouble shooting enet status to menu/functions
			03/10/95 add MemCmp function to see where lpWork changing from 
					record when running strip or scope windows.
			03/13/95 add menu item scope offset and window
			04/10/95 2 scope  mux windows, one from instrument and one from 
					diagnostic
			04/19/95 ADD instrument sync register control window to diagnostic
					menu selections
			06/19/95 enable bar chart again... for new bar chart
			09/12/95 add error log output on exit.  Works in comjunction
					with xchanix and uses environment variable TRUSCOPE
			09/13/95 add code to init motion tracking board
			10/02/95 post stop_rb message to stripchart window on creation
			10/11/95 SET extension type to config for all save-as operations
			06/13/96 on my own time, move translateaccelerator to head of
					'if' statements in message pump to allow closing windows
					at any time via alt-c
			07/17/96 add environment variable for help file
			02/11/97 prevent multiple copies of the program from running.
			04/25/97 move etherstats structures from dlg_trbl.c
			04/28/97 FREE disk menu command
			05/02/97 popup gate and receiver window with flaw cal window
			05/06/97 ADD borte.fon to resources for free disk window
			08/27/97 LOAD dibabp.dll for screen print functions
			09/02/97 use hWndTop to send messages back to main window
			12/08/97 make the .c file a .00n file for version control
			01/16/98 get error.log path from truscope.ini instead of DOS
					environment
			03/28/98 begin process to read dialog positions from ini file
			04/03/98 SendWallToUDP after reading config file.
			04/06/98 load flaw depth scale factors with config file read
			05/11/98 Add System Gain dialog
			06/22/98 Enable page printing from ini file.
			07/20/98 if using new scope demux board, load and use
					demux.bin info to configure board.  This file 
					is independent of config file being loaded.
			12/17/98 enable/disable ethernet alerts on status window with
						truscope.ini entry.
			06/30/99 add universal message box to announce unable to diaplay
					dialog.  Useful for translating to czech
			10/26/99 keep track of scoreboard position for win95
			09/28/00 Kill xdcr display (hDlgDlg_coupling) before killing
					strip display when running another data consumer such
					as wall or flaw cal display.
		********************************************************************/

#include <windows.h>
#include <dos.h>
#include <dir.h>
#include <CONTROL.h>
#include <global.h>
#include <instrume.h>
#include <lang.h>

#include <bwcc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <print.h>


extern WORD far _D000H;


//GLOBAL_VARIABLES
//GLOBAL_VARIABLES

struct {
		unsigned long edata[13];
		} ether_stats[5], estats_offset[5];


globalREC *lpDefault, *lpRecord; /* global records for instrument control */
GlobalRecExt *lpWork;	/* bigger than default or record */

GLOBALHANDLE hGlobalDefMem; 	// memory for global defaults
GLOBALHANDLE hGlobalRecMem; 	// memory for globals read from file
GLOBALHANDLE hGlobalWorkMem; 	// memory for global work ... bigger than def or rec

GLOBALHANDLE hGlobalDLink; 					/* shared memory between win-app and i860-app */

OFSTRUCT ofDefault, ofRecord; 				/* windows file handles for defaults setup and
																					configuration record */
u_char channel; 									/* global channel index for entire instrument */
u_char channelB; 									/* global channel index for copy operation */
u_char gate;
u_char simple_flag;				// oscilloscope trace selection set flag
BYTE last_good_read_cmnd[4];
BYTE last_good_write_cmnd[8];
WORD echo_bits[4];
int control_mode_sel;			// strip chart operating mode.
WORD AlarmStatus[4];			// status words with alarms bits from udp
int fChartData;					// file handle for archive data replay

char far *lpInst;
//RawDataREC *lpRawData;
void far * GetINSTRamPtr();
int fDefault, fRecord, fErrorLog;
                                          /* needs to create a struct and fixed mem alloction*/
char fnRecord[80], fnDefault[80];
char szHelpDirName[80];				// directory location for help files
char szHelpFileName[80] = "Truscope.hlp";
char szCustomerName[128];
char szHomePath[255];	// ultimately has 'home' directory path

DWORD dwColorT1, dwColorB1;	// default text and background
DWORD dwColorAC1, dwColorACT1;	// active caption/text



short file_extension_type;
char tBuf[80];
BYTE test_scope_demux_ram[12];		// copy of output to scope demux io ports



#if 0
// in lang.h
char enet_label[13][15] =
	{	{"tx_packets   ="},
		{"rx_packets   ="},
		{"framing_errs ="},
		{"crc_errors   ="},
		{"packets_drop ="},
		{"packets_miss ="},
		{"defrd_xmit   ="},
		{"late_collisn ="},
		{"carrier_lost ="},
		{"fail_retries ="},
		{"single_retry ="},
		{"many_retries ="},
		{"avg_tdr_val  ="}	};
#endif

#define modeless		1


HANDLE   hInst;
HANDLE   hAccel;

HWND hWndTop;

HWND hDlgDLG_STATUS;
HWND hDlgDLG_COUPLING;
HWND hDlgDLG_ENET_STAT;
HWND hDlgDLG_RECEIVER;
HWND hDlgDLG_TCGSET;
HWND hDlgDLG_GATES;
HWND hDlgDLG_PULSER;
// HWND hDlgDLG_ALARM;
HWND hDlgDLG_TOF;
HWND hDlgDLG_SCOP;
HWND hDlgDLG_SYNC;
HWND hDlgDLG_CATCH;
HWND hDlgDLG_OSET;
HWND hDlgDLG_UDPIO;
HWND hDlgDLG_CAL;
HWND hDlgDLG_BAR;
HWND hDlgDLG_FREEDISK;
HWND hDlgDLG_FLAWTEXT;
HWND hDlgDLG_SYSGAIN;
HWND hDlgDLG_ALARMSTATUS;

HICON hIconAlarm1, hIconAlarm2;

/***** Printer info... for print screen capability ******/

// Save the ASCII names of the default printer info found in win.ini

char szLptDevice[60], szLptDriver[40], szLptPort[20];
HDC hdcPrinter = 0;
int InitialPrinterOrientation = DMORIENT_PORTRAIT;

// pixed size of crt characters and printer characters
// pixel width of crt and printer

short cxChar, cyChar, cxPrnChar, cyPrnChar;
short  cxCaps;
short dxCrt = 1, dyCrt = 1, dxPrn = 1, dyPrn = 1;
//Scale factors to go from crt to printer resolution
short xPrnScaleFactor, yPrnScaleFactor;
int nAutoPrint = 0;
int nDiskDump = 0;
int EnablePageCount = 1;
int EnableAlerts = 1;
long lLostPacketCnt[4] = {0,0,0,0};


// 03/28/98 Store last position of dialog window in truscope.ini file
// Be sure to update global.h as well as this location

struct
	{
	DLG_INI_POSITION	receiver;
	DLG_INI_POSITION	gates;
	DLG_INI_POSITION	tcg;
	DLG_INI_POSITION	strip;
	DLG_INI_POSITION	status;
	DLG_INI_POSITION	alarms;
	DLG_INI_POSITION	scope;
	DLG_INI_POSITION	dscope;
	DLG_INI_POSITION	pulser;
	DLG_INI_POSITION	sysgain;
	DLG_INI_POSITION	scope_demux;
	DLG_INI_POSITION	alarm_status;
	DLG_INI_POSITION	Xdcrs;
	DLG_INI_POSITION	null[3];
	}	DlgPosition =

	{
		{0,0,0,0, "Receiver"	},	// keep this as 1st entry of table
		{0,0,0,0, "Gates"		},
		{0,0,0,0, "Tcg"			},
		{0,0,0,0, "Strip"		},
		{0,0,0,0, "Status"		},
		{0,0,0,0, "Alarms"		},
		{0,0,0,0, "Scope"		},
		{0,0,0,0, "DiagScope"	},
		{0,0,0,0, "Pulser"		},
		{0,0,0,0, "SysGain"		},
		{0,0,0,0, "ScopeDemux"	},
		{0,0,0,0, "AlarmStatus"	},
		{0,0,0,0, "Xdcr"        },	// scoreboard of xdcr coupling/hits
		{0,0,0,0, NULL		}
	};


	DLG_INI_POSITION	*pDlgPosition;


/*********************************************************************/
/**********   Function Prototypes   **********************************/
/*********************************************************************/

long FAR PASCAL  MainWndProc(HWND, unsigned, WORD, LONG);
// this is the procedure which windows will feed messages to after the program
// starts.

// In general, this is a windows no-no to reference an external function

extern void update_all(void);			// in dlg_stat.c
extern void warp_speed(int onoff);		// in dlg_stri.c
extern void collect_enet_stats (HWND);
extern void read_enet_stats (HWND);
extern void PrintTruscopeScreen(void);	// in prntscrn.c

/*********************************************************************/


	
// Winhelp doesn't seem to change directories easily so we will do it
// here.  All calls to winhelp go through this routine.

void GetTuboHelp (HWND hWnd, LPCSTR HelpFile, UINT help_indx, DWORD dwData)
	{
	char c, far *p;
	p = (char far *)HelpFile;		// kill compiler warn
	if ( strlen(szHelpDirName) > 0)
		{
		if ( (p = _fstrchr(szHelpDirName,':')) != NULL)
			{
			c = *(p-1);
			setdisk(toupper(c)-'A');	// change drives in DOS
            }
		chdir(szHelpDirName);
        }
	// We don't care what file name is passed, we are using Truscope.hlp
	WinHelp(hWnd, szHelpFileName, help_indx, dwData);
	}



// From Petzold, chapter 11 formfeed.c program
HDC GetPrinterDC (void)
     {
     static char szPrinter [80] ;
     char        *szDevice, *szDriver, *szOutput ;

     GetProfileString ("windows", "device", ",,,", szPrinter, 80) ;

     if (NULL != (szDevice = strtok (szPrinter, "," )) &&
         NULL != (szDriver = strtok (NULL,      ", ")) &&
         NULL != (szOutput = strtok (NULL,      ", ")))
		{
		strcpy (szLptDevice, szDevice);
		strcpy (szLptDriver, szDriver);
		strcpy (szLptPort, szOutput);
		return CreateDC (szDriver, szDevice, szOutput, NULL) ;
		}

     return 0 ;
     }


int SetPrinterOrientation( HWND hWnd, HDC *hdcPrn, int NewOrientation)
	{
	// Set the printer into a new orientation.
	// Returns the previous orientation.
	
	HINSTANCE hDriver;
    HANDLE        hDevMode = 0;
    LPDEVMODE     lpDevMode = NULL;
    LPFNDEVMODE   lpfnExtDeviceMode;
    int           count, OldOrientation;
	char sztmpDriver[30];

	// The printer driver itself supplies the ExtDeviceMode function
	strcpy(sztmpDriver,szLptDriver);
	strcat(sztmpDriver,".DRV");		// add .drv extension for driver name
//	hDriver = LoadLibrary(szLptDriver);
	hDriver = LoadLibrary(sztmpDriver);
	if ( hDriver >= 0x20)
		{	//valid driver handle
		// The following code is from Microsoft, PSS ID Number Q112641
		
        // Get a function pointer to the ExtDeviceMode() function.
        // ExtDeviceMode() resides in the driver so we can't call it
        // directly.
		lpfnExtDeviceMode = (LPFNDEVMODE)GetProcAddress(hDriver,
                                               (LPSTR)"EXTDEVICEMODE");
		if (lpfnExtDeviceMode == NULL)
			{
			// Printer driver does not support ExtDeviceMode function
			// abort this operation
			FreeLibrary(hDriver);
			return -1;
			}


		// Get the number of bytes in the full DEVMODE buffer.
		// This includes the device-dependent part at the end
		// of the DEVMODE struct.  DEVMODE is variable in size per article.
		count = lpfnExtDeviceMode(0,hDriver, NULL,
									(LPSTR) szLptDevice, (LPSTR) szLptPort,
									NULL, NULL,
                                     0);    // 0 = get buffer size
 
		if (count != -1)
			{	// DEVMODE buffer size is ok
			// Allocate storage for the DEVMODE buffer.
			hDevMode = GlobalAlloc(GHND, count);
 
			if (hDevMode)
				{	//globalalloc ok
				lpDevMode = (LPDEVMODE)GlobalLock(hDevMode);
 
				// Get the current printer settings.
				count = lpfnExtDeviceMode(0, hDriver,
									lpDevMode,   // Output buffer
									(LPSTR) szLptDevice, (LPSTR) szLptPort,
									NULL, NULL,
									DM_OUT_BUFFER); // aka DM_COPY
 
                  // Check to see if this printer supports changing
                  // the orientation. You should check dmFields
                  // before changing any printer setting.
 
				if (lpDevMode->dmFields & DM_ORIENTATION)
					{	// printer supports landscape/portrait
					// Pass lpDevMode as both the input and output
					// DEVMODE buffers. It is important to pass
					// in the full DEVMODE from the previous call
					// to ExtDeviceMode() as the input buffer because
					// it has been completely initialized by the
					// driver. If you do not do this, the results
					// are sporadic--sometimes it works and sometimes
					// it doesn't depending on the printer driver and
					// the setting you are trying to change.
 
					// Zero out all the fields and then set the bit(s)
					// for the field(s) we want to change.
					lpDevMode->dmFields = 0;
					lpDevMode->dmFields = DM_ORIENTATION;

					// SAVE old orientation for program exit
					OldOrientation = lpDevMode->dmOrientation;
					 
					// Change to landscape.
					lpDevMode->dmOrientation = NewOrientation;
 
					// Call ExtDeviceMode() once more to allow the driver
					// to change the device-dependent portion of the
					// DEVMODE buffer if it needs to.
 
					count = lpfnExtDeviceMode(0, hDriver,
									lpDevMode,  //Output buffer
									(LPSTR) szLptDevice, (LPSTR) szLptPort,
									lpDevMode,  // Input buffer
									(LPSTR) "WIN.INI",
									DM_OUT_BUFFER | DM_IN_BUFFER | DM_UPDATE);
 
									// aka DM_COPY | DM_MODIFY

					DeleteDC(*hdcPrn);
					*hdcPrn = CreateDC (szLptDriver, szLptDevice, szLptPort,
										(void FAR*)lpDevMode);
					}	// printer supports landscape/portrait
				}	//globalalloc ok
			GlobalUnlock(hDevMode);
			GlobalFree(hDevMode);
			}		// DEVMODE buffer size is ok
		FreeLibrary(hDriver);
		}	//valid driver handle
	return OldOrientation;
	
	}

// Used by all dialogs if failed to create.

void DialogCreateError(void)
	{
	MessageBox(NULL, 
				szControl_DialogDisp,
				szControl_SystemError,
				MB_SYSTEMMODAL | MB_ICONHAND | MB_OK);
	}

void FixRandomShort( short *var, short min, short max, short nominal);

void FixRandomShort( short *var, short min, short max, short nominal)
	{
	// Config file may contain wild (random) values.  If outside min/max
	// range, set variable to nominal value

	if ( ( *var < min) || ( *var > max) ) *var = nominal;
	}




void FixUpConfig (void);

void FixUpConfig (void)
	{
	int channel, gate;	// note that these are local variables !!

	int levelmin[2] = { LEVEL1MIN, LEVEL2MIN };
	int levelmax[2] = { LEVEL1MAX, LEVEL2MAX };
	
	// Make certain that critical variables are within range
	// sound velocities in inches/usecond
	
	if ( ( lpRecord->syscfg.CompVelocity < 0.1) ||
		 ( lpRecord->syscfg.CompVelocity > 0.6) ) 
		lpRecord->syscfg.CompVelocity = 0.23; // mild steel

	if ( ( lpRecord->syscfg.ShearVelocity < 0.04) ||
		 ( lpRecord->syscfg.ShearVelocity > 0.4) ) 
		 lpRecord->syscfg.ShearVelocity = 0.13; // mild steel

	if(lpRecord->pulser.prf < PRFMIN)
		lpRecord->pulser.prf = PRFMIN;
	else if(lpRecord->pulser.prf > PRFMAX)
		lpRecord->pulser.prf = PRFMAX;

	if(lpRecord->pulser.prf < PDMIN)
		lpRecord->pulser.prf = PDMIN;
	else if(lpRecord->pulser.prf > PRFMAX)
		lpRecord->pulser.prf = PRFMAX;

	if (lpRecord->pipeinfo.Wall < FWALL_MIN)
		lpRecord->pipeinfo.Wall = FWALL_MIN;	// in inches

	else if (lpRecord->pipeinfo.Wall > FWALL_MAX)
		lpRecord->pipeinfo.Wall = FWALL_MAX;	// in inches

	if (lpRecord->pipeinfo.Diameter < FOD_MIN)
		lpRecord->pipeinfo.Diameter = FOD_MIN;

	else if (lpRecord->pipeinfo.Diameter > FOD_MAX)
		lpRecord->pipeinfo.Diameter = FOD_MAX;

	if ( lpRecord->scope.reject_pcnt > 70)
		lpRecord->scope.reject_pcnt = 70;

	/********************************************************/
	/*******************  Receiver section ******************/
	/********************************************************/

	for ( channel = 0; channel < 40; channel++)
		{
		lpRecord->receiver.fil_option[channel] &= 3;
		lpRecord->receiver.polarity[channel] &= 1;
		lpRecord->receiver.det_option[channel] &= 1;
		lpRecord->receiver.tcg_enable[channel] &= 1;
		lpRecord->receiver.tcg_trigger[channel] &= 7;
		lpRecord->receiver.tcg_step[channel] &= 3;

		if  (lpRecord->receiver.gain[channel] > GAINMAX)
    	   	lpRecord->receiver.gain[channel] = GAINMAX;
		else if(lpRecord->receiver.gain[channel] < GAINMIN)
    	   	lpRecord->receiver.gain[channel] = GAINMIN;
		}


	/********************************************************/
	/******************* Gates ******************************/
	/********************************************************/

	for ( channel = 0; channel < 40; channel++)
	  for (gate = 0; gate < 2; gate++)
		{
		if (lpRecord->gates.delay[channel][gate] > DELAYMAX) 
			lpRecord->gates.delay[channel][gate] = DELAYMAX;
		else if (lpRecord->gates.delay[channel][gate] < DELAYMIN) 
			lpRecord->gates.delay[channel][gate] = DELAYMIN;

		if (lpRecord->gates.level[channel][gate] > levelmax[gate])
			lpRecord->gates.level[channel][gate] = levelmax[gate];
		else if (lpRecord->gates.level[channel][gate] < levelmin[gate])
			lpRecord->gates.level[channel][gate] = levelmin[gate];
		else if ( (lpRecord->gates.level[channel][gate] < 5) &&
				  (lpRecord->gates.level[channel][gate] > -5) )
				  lpRecord->gates.level[channel][gate] = 5;

		if(lpRecord->gates.range[channel][gate] > RANGEMAX)
			lpRecord->gates.range[channel][gate] = RANGEMAX;
		else if(lpRecord->gates.range[channel][gate] < RANGEMIN)
			lpRecord->gates.range[channel][gate] = RANGEMIN;

		if(lpRecord->gates.blank[channel][gate] > BLANKMAX)
			lpRecord->gates.blank[channel][gate] = BLANKMAX;

		else if(lpRecord->gates.blank[channel][gate] < BLANKMIN)
			lpRecord->gates.blank[channel][gate] = BLANKMIN;

		/********************************************************/
		/******************* Alarms & TOF ***********************/
		/********************************************************/

		lpRecord->alarm.trigger[channel][gate] &= 1;
		if(lpRecord->alarm.polarity[channel][gate] != 0)
			lpRecord->alarm.polarity[channel][gate] = 0xff;
		if ( gate == 0)
			lpRecord->timeoff.trigger[channel][gate] %= 5;
		else
			lpRecord->timeoff.trigger[channel][gate] %= 7;

		// stop on.. gate0 always 0, gate 1 is 0 or 1
		lpRecord->timeoff.stopon[channel][gate] &= gate;

		
		}

	/********* Channel Configuration ***************************/
	for ( channel = 0; channel < 40; channel++)
		{
		switch (lpRecord->syscfg.chan_type[channel])
			{
		case IS_WALL:
		case IS_LONG:
		case IS_TRAN:
		case IS_OBQ2:
		case IS_OBQ4:
		case IS_LAM:
			break;

		default:
			lpRecord->syscfg.chan_type[channel] = IS_NOTHING;
			break;
			}
		}

		/********************************************************/
		/******************* Strip Chart NC - Thold *************/
		/********************************************************/
	for ( gate = 0; gate < 2; gate++)
		{

			FixRandomShort( &lpRecord->syscfg.LgIdDispThold[gate], 0, 100,30);
			FixRandomShort( &lpRecord->syscfg.LgIdDispNC[gate], 0, 25, 4);
			FixRandomShort( &lpRecord->syscfg.LgOdDispThold[gate], 0, 100,30);
			FixRandomShort( &lpRecord->syscfg.LgOdDispNC[gate], 0, 25, 4);

			FixRandomShort( &lpRecord->syscfg.TrIdDispThold[gate], 0, 100,30);
			FixRandomShort( &lpRecord->syscfg.TrIdDispNC[gate], 0, 25, 4);
			FixRandomShort( &lpRecord->syscfg.TrOdDispThold[gate], 0, 100,30);
			FixRandomShort( &lpRecord->syscfg.TrOdDispNC[gate], 0, 25, 4);
		}

	}
		
#pragma argsused
int PASCAL WinMain(	HANDLE hInstance,       // Module instance handle
					HANDLE hPrevInstance,   // Handle to previous module instance
					LPSTR  lpszCmdLine,     // Pointer to command line arguments
					int    nCmdShow)		// Show window flag


	{	//WinMain
//	static char szAppName[] = "CONTROL";	lang.h
//	static char szTitleBar[] = 				lang.h
//		"TRUSCOPE INSTRUMENT       Copyright 1996 Tuboscope Vetco";
	MSG      msg;		// a structure where Windows will copy posted messages
	HWND     hWndMain;
	WNDCLASS wndclass;
	char szProgramPath[255];
	int i, ilast, len;
//	int test;


	HANDLE hBorLibrary, hDIBLib;

	lpInst = GetINSTRamPtr();	// point to the shared ram interface with 
								// the transputer.
								
	//	lpRawData = lpInst;

#if 0
SCOPE_DEMUX_1998
// read operation fails here
	LoadDemuxFile();
	CloseDemuxFiles();
#endif

	hGlobalDefMem = GlobalAlloc(GHND, sizeof(globalREC)); /* created piece of mem for RECs' */
	lpDefault = (globalREC *) (GlobalLock(hGlobalDefMem)); /* record pointer for system default*/

	hGlobalWorkMem = GlobalAlloc(GHND, CHART_DATA_HEADER); // bigger than lpwork
	lpWork    = (GlobalRecExt *) (GlobalLock(hGlobalWorkMem));/* for temp record */

	hGlobalRecMem = GlobalAlloc(GHND, sizeof(globalREC));
	lpRecord  = (globalREC *) (GlobalLock(hGlobalRecMem));/* for current record */


	hBorLibrary = LoadLibrary("tubo.dll");
	hDIBLib		= LoadLibrary("dibapi.dll");

	hIconAlarm1 = LoadIcon(hInstance, "alarm1");
	hIconAlarm2 = LoadIcon(hInstance, "alarm2");

	hInst = hInstance;    // Store the application instance handle  moved 8/23/94...jeh


	// If the program is already running in the background
	if (hPrevInstance)
		{
		// Previous copy is running in back ground
		hWndMain = FindWindow (szAppName, szTitleBar);
		if ( hWndMain)
			{
			SetActiveWindow(hWndMain);
			ShowWindow (hWndMain, SW_SHOWMAXIMIZED);
			UpdateWindow (hWndMain);
			}
		return FALSE;
		}
		
	// If this is the first time thru and the app is not already in focus
	else		// if(!hPrevInstance)
		{
		wndclass.style           = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc     = (WNDPROC)MainWndProc;
		wndclass.cbClsExtra      = 0;
		wndclass.cbWndExtra      = 0;
		wndclass.hInstance       = hInstance;
		wndclass.hCursor         = LoadCursor(NULL, IDC_ARROW);
		wndclass.hIcon           = LoadIcon(hInstance,szAppName);
		wndclass.hbrBackground   = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		wndclass.lpszMenuName    = "tubo";
		wndclass.lpszClassName   = szAppName;

		if(!RegisterClass(&wndclass))	// let windows know who will process messages
			return FALSE;

		}
   //REGEN_INITVIEW
   //REGEN_INITVIEW

	if(!(hWndMain = CreateWindow(szAppName,
					szTitleBar,
					WS_OVERLAPPEDWINDOW,	// | WS_VSCROLL,
					CW_USEDEFAULT, 0,
					CW_USEDEFAULT, 0,
					NULL, NULL, hInstance, NULL) ) )
		return FALSE;

   //REGEN_MAINWND
   //REGEN_MAINWND


	hWndTop = hWndMain;		// hWndTop is global to other procedures
//  ShowWindow(hWndMain, nCmdShow);
	ShowWindow(hWndMain, SW_SHOWMAXIMIZED);
	UpdateWindow(hWndMain);


	hAccel = LoadAccelerators(hInstance, "tubo");

	AddFontResource("BORTE.FON");	// used by free disk space
	SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
	

	dwColorAC1 = GetSysColor(COLOR_ACTIVECAPTION);
	dwColorACT1 = GetSysColor(COLOR_CAPTIONTEXT);


	// Set time zone info for replay of pipe
	putenv("TZ=CST6CDT");
	tzset();
	TheLink = 0;	// transputer link not yet established
	memset( (void *) &estats_offset, 0 , sizeof(estats_offset));


	// Save the path to the executable program.  Usually
	// D:\tvi\mmi\custname\control.exe
	len = GetModuleFileName(hInst, szProgramPath, sizeof(szProgramPath));
	if (len)
		{	// zero if fail, else returns length of path
		// whack off full file path and leave only path to 'home' directory
		i = ilast = 0;
		szHomePath[0] = 0;
		while ( i < len)
			{
			while ( (szProgramPath[i] != 0x5c) && ( i < len) ) i++;
			if ( i < len)
				{	// found a '\' character ( 0x5c)
				ilast = i;
				i++;
				}
			}
		// copy the string up to the last '\' character to the homepath string
		szProgramPath[ilast] = 0;
		strcpy(szHomePath, szProgramPath);
		}

   //REGEN_MAINEND
   //REGEN_MAINEND

	/* The Window manager sends messages which belong to our window(s)
		to us via the getmessage call.  The messages are unqueued from
		a system queue into the msg structure.  If none of our dialog
		windows process the message ( the next set of if's) and if
		the message is not a hot-key (translate accelerator) then
		MainWinProc will have to process the message.  MainWinProc is
		called by the dispatch call below.
	*/

	/*
		This is an infinite loop until we quit our app ( our window)
		and the system sends us a result which makes GetMessage False.
		Then we clean up our environment and quit.
	*/
	
	while(GetMessage(&msg, NULL, 0, 0))

#if modeless
		/*
		Only modeless dialog boxes need to intercept the message queue.
		Modal dialog boxes are not fed thru the message queue per 
		Petzold, p446, 3rd edition.
		*/

		if(!TranslateAccelerator(hWndMain, hAccel, &msg))
		if(hDlgDLG_RECEIVER == 0 || !IsDialogMessage(hDlgDLG_RECEIVER, &msg))
		if(hDlgDLG_GATES == 0 || !IsDialogMessage(hDlgDLG_GATES, &msg))
		if(hDlgDLG_PULSER == 0 || !IsDialogMessage(hDlgDLG_PULSER, &msg))
//		if(hDlgDLG_ALARM == 0 || !IsDialogMessage(hDlgDLG_ALARM, &msg))
		if(hDlgDLG_TOF == 0 || !IsDialogMessage(hDlgDLG_TOF, &msg))

// !!else

		// these are modeless dialog boxes.  They must intercept the
		// message queue.


		// status, strip and bar chart are always modeless windows
		
		if(hDlgDLG_STATUS == 0 || !IsDialogMessage(hDlgDLG_STATUS, &msg))
		if(hDlgDLG_STRIP == 0 || !IsDialogMessage(hDlgDLG_STRIP, &msg))
		if(hDlgDLG_COUPLING == 0 || !IsDialogMessage(hDlgDLG_COUPLING, &msg))
		if(hDlgDLG_ChartCal == 0 || !IsDialogMessage(hDlgDLG_ChartCal, &msg))
		if(hDlgDLG_ENET_STAT == 0 || !IsDialogMessage(hDlgDLG_ENET_STAT, &msg))
		if(hDlgDLG_CATCH == 0 || !IsDialogMessage(hDlgDLG_CATCH, &msg))
		if(hDlgDLG_OSET == 0 || !IsDialogMessage(hDlgDLG_OSET, &msg))
		if(hDlgDLG_SYNC == 0 || !IsDialogMessage(hDlgDLG_SYNC, &msg))
		if(hDlgDLG_CAL == 0 || !IsDialogMessage(hDlgDLG_CAL, &msg))
		if(hDlgDLG_BAR == 0 || !IsDialogMessage(hDlgDLG_BAR, &msg))
		if(hDlgDLG_FREEDISK == 0 || !IsDialogMessage(hDlgDLG_FREEDISK, &msg))
		if(hDlgDLG_SYSGAIN == 0 || !IsDialogMessage(hDlgDLG_SYSGAIN, &msg))
		if(hDlgDLG_ALARMSTATUS == 0 || !IsDialogMessage(hDlgDLG_ALARMSTATUS, &msg))
#endif

//		if(!TranslateAccelerator(hWndMain, hAccel, &msg))
			{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}

	// Prepare to exit
	
//	if(hBorLibrary > 32);
		FreeLibrary(hBorLibrary);
		FreeLibrary(hDIBLib);

	//REGEN_APPTERM
	//REGEN_APPTERM

	GlobalUnlock(hGlobalDefMem);
	GlobalFree(hGlobalDefMem);

	GlobalUnlock(hGlobalWorkMem);
	GlobalFree(hGlobalWorkMem);

	GlobalUnlock(hGlobalRecMem);
	GlobalFree(hGlobalRecMem);

	warp_speed(0);	// restore bios timer clock speed

	if (InitialPrinterOrientation > 0)
		SetPrinterOrientation(hWndMain, &hdcPrinter,InitialPrinterOrientation);

	return msg.wParam;
	}	//WinMain




/*********************************************************************/

int MemCmp( const void *s, const void *t, int cnt)
	{	// for testing... change back to memcmp when done
	int i = 0;

	while ( (*((BYTE *)s)++ == *((BYTE *)t)++) && (i < cnt) ) i++;
	if ( i == cnt) return 0;
	else return i;
	}	// for testing... change back to memcmp when done



/*********************************************************************/

// ComputerBoards Inc CIO-DIO24/CTR3 board

#define	PORTBASE		0x300
#define PORTA			PORTBASE
#define PORTB			PORTBASE+1
#define PORTCTL			PORTBASE+3
#define T0				PORTBASE+4
#define T1				PORTBASE+5
#define T2				PORTBASE+6
#define TCTL			PORTBASE+7


#define	STOP_RB			155


void InitMotionTracker (void)
	{
	// Init the 8255 port and 8254 timer/counters which track motion
	// pulses.  Was planned to use motion tracker subsystem which 
	// interfaced to udp.  This is an alternative solution which does
	// not give as accurate an x location.

	

	outp(PORTCTL,0x90);	//A in, B&C outputs

	// t1 and t2 are down counters
	outp(TCTL, 0x34);	// t0 mode 2 binary count.. simulate motion pulse
	outp(TCTL, 0x70);	// t1 mode 0 binary count.. forward motion count
	outp(TCTL, 0xb0);	// t2 mode 0 binary count.. reverse motion count

	// load 0xffff into all 3 down counters
	outp(T0,0xff);
	outp(T0,0xff);
	outp(T1,0xff);
	outp(T1,0xff);
	outp(T2,0xff);
	outp(T2,0xff);

	}


void LoadDlgPosition(char *t, DLG_INI_POSITION *dlg)
	{
	// Main Window routine passes ascii string read from truscope.ini
	// file.  This routine extracts the top & left dlg positions and
	// stores in dlg structure pointed to by dlg
	sscanf(t,"%d, %d, %d, %d", &dlg->left, &dlg->top, &dlg->width, &dlg->height);
	}
	
	/*
	This routine processes the top level messages which the window manager
	sends to our application.
	*/

long FAR PASCAL  MainWndProc(HWND hWnd, unsigned wMessage, WORD wParam, LONG lParam)
	{
	//REGEN_WINDOWPROCVARIABLES
	//REGEN_WINDOWPROCVARIABLES
	HGLOBAL hglb;
	HMENU hMenu;
	LPSTR lpBuffer;
	char tmp[80];
	int i,j;
//	static char szRunUDP[] = "d:\\tvi\\udp\\runudp.bat";
	WORD wResult;
	HDC           hdc, hdcInfo ;
	PAINTSTRUCT   ps ;
	TEXTMETRIC    tm ;

	char *lp, fnErrorLog[80];	// path name for error log file
	OFSTRUCT ofErrorLog;
	char mmi_stop[360];
	time_t t;
	struct tm *tblock;



	switch(wMessage)
		{
		//REGEN_WINDOWPROC
		//REGEN_WINDOWPROC

	case WM_CREATE:

#if 0
		if ( (lp = getenv("TRUSCOPE_HELP")) != NULL)
			{	// add to help file path
			strcpy (szHelpDirName, lp);
			}
		else szHelpDirName[0] = 0;	// none specified
#endif

		if (GetPrivateProfileString("Truscope","HELP","",szHelpDirName,
							sizeof(szHelpDirName), "TRUSCOPE.INI") == 0)
			szHelpDirName[0] = 0;	// none specified
		

		// Get customer info for printer footer
#if 0
[Truscope]
CUSTOMER_NAME=Tuboscope / ABC Mill

[DlgPositions]
Receiver=0,40,100,300

#endif

		if (GetPrivateProfileString("Truscope","CUSTOMER_NAME",
									"Tuboscope   ",szCustomerName,
									sizeof(szCustomerName), 
									"TRUSCOPE.INI") == 0)
			szCustomerName[0] = 0;	// none specified

		// Enable page counting on output screen print

		if (GetPrivateProfileString("Truscope","EnablePageCount",
									"1",tmp,
									sizeof(tmp),
									"TRUSCOPE.INI") == 0)
			EnablePageCount = 1;	// defaults to being "on"
		else EnablePageCount = atoi(tmp);

		// Enable ethernet alarms conditions to show on status window

		if (GetPrivateProfileString("Truscope","EnableAlerts",
									"1",tmp,
									sizeof(tmp),
									"TRUSCOPE.INI") == 0)
			EnableAlerts = 1;	// defaults to being "on"
		else EnableAlerts = atoi(tmp);

		// Get dialog window positions from ini file

#if 0
		if (GetPrivateProfileString("DlgPositions",DlgPosition.receiver.name,
									"",		// no default value
									tmp,
									sizeof(tmp),
									"TRUSCOPE.INI"))
			LoadDlgPosition(tmp, &DlgPosition.receiver);
				


		if (GetPrivateProfileString("DlgPositions",DlgPosition.status.name,
									"",		// no default value
									tmp,
									sizeof(tmp),
									"TRUSCOPE.INI"))
			LoadDlgPosition(tmp, &DlgPosition.status);
#endif


		// Generalize the loading of position info from truscope.ini
		pDlgPosition = &DlgPosition.receiver;	// point to beginning of table
		for ( i = 0; i < 20; i++)
			{
			if (pDlgPosition->name == NULL) break;	// end of list

			// Process list entry
			if (GetPrivateProfileString("DlgPositions",pDlgPosition->name,
									"",		// no default value
									tmp,
									sizeof(tmp),
									"TRUSCOPE.INI"))
			LoadDlgPosition(tmp, pDlgPosition);
			pDlgPosition++;		// move to next entry in table
			}			

		InitMotionTracker();

		// Get crt metrics for later use.
		hdc = GetDC (hWnd) ;
		GetTextMetrics (hdc, &tm) ;
		cxChar = tm.tmAveCharWidth ;
		cyChar = tm.tmHeight + tm.tmExternalLeading ;
		dxCrt = GetDeviceCaps(hdc, HORZRES);
		dyCrt = GetDeviceCaps(hdc, VERTRES);
		ReleaseDC (hWnd, hdc) ;

		// 08/27/97 find out about printer and set to landscape mode
		hdcPrinter = GetPrinterDC();
		if ( hdcPrinter != NULL)
			{
			InitialPrinterOrientation = 
				SetPrinterOrientation(hWnd, &hdcPrinter,DMORIENT_LANDSCAPE);

			// while we're here, lets get the sizing info on the printer
			GetTextMetrics (hdcPrinter, &tm) ;
			cxPrnChar = tm.tmAveCharWidth ;
			cyPrnChar = tm.tmHeight + tm.tmExternalLeading ;
			dxPrn = GetDeviceCaps(hdcPrinter, HORZRES);
			dyPrn = GetDeviceCaps(hdcPrinter, VERTRES);
			xPrnScaleFactor = dxPrn / dxCrt;
			yPrnScaleFactor = dyPrn / dyCrt;
			DeleteDC(hdcPrinter);
			hdcPrinter = 0;
			}


		// If using new scope demux board, load info from file demux.bin
#if 0
SCOPE_DEMUX_1998
// read operation fails here
		LoadDemuxFile();
		CloseDemuxFiles();

#endif

OPEN_CFG_FILE:
		PostMessage(hWnd,WM_COMMAND,IDM_OPEN,0L);	//force user to open popup

		return FALSE;

	case WM_INITMENUPOPUP :
		CheckMenuItem(wParam,IDM_STRIPCHART,hDlgDLG_STRIP ? MF_CHECKED : MF_UNCHECKED);
//		CheckMenuItem(wParam,IDM_BARCHART,hDlgDLG_BAR ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(wParam,IDM_RUNSTATUS,hDlgDLG_STATUS ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(wParam,IDM_AUTOPRINT, nAutoPrint ? MF_CHECKED : MF_UNCHECKED);

#if REV1997_2CHNL_BD
		CheckMenuItem(wParam,IDM_DISKDUMP, nDiskDump ? MF_CHECKED : MF_UNCHECKED);
#endif
//	EnableMenuItem(wParam,IDM_BARCHART,hDlgDLG_STRIP ? MF_GRAYED : MF_ENABLED);
//	EnableMenuItem(wParam,IDM_STRIPCHART,hDlgDLG_BAR ? MF_GRAYED : MF_ENABLED);

		return FALSE;
		


#if 0
// TEXTOUT_TEST

	case WM_PAINT:
		// testing of TextOut function
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 10,30, "Hello world", 11);
		EndPaint(hWnd,&ps);
		return FALSE;

		// testing of TextOut function

	case WM_KEYDOWN:
		i = 3;
		if ( wParam == VK_F1)
			{
			i = 4;
			}
		return FALSE;		

#endif

	case WM_COMMAND :
		if(!LOWORD(lParam))
			{	// Process Menu Commands
			switch(wParam)
				{      // Determine which Menu ID
			case IDM_OPEN :
		
				strlwr(fnRecord);
				file_extension_type = CONFIG_EXT;	// config file

				// if the status window is already in focus, destroy it
				if(hDlgDLG_STATUS)
					{
					DestroyWindow(hDlgDLG_STATUS);
					hDlgDLG_STATUS = 0;
					}


				if ( (strlen(fnRecord) > 0) && (fRecord > -1) )
					{	// A previous open file operation has occurred
					if ( (i = MemCmp(lpWork, lpRecord, sizeof(globalREC)) ) != 0 )
						{	// file info is different from the originally loaded config file
						lstrcpy(tmp,szControl_File);
						lstrcat(tmp,fnRecord);
						lstrcat(tmp,szControl_HasChanged);
						sprintf(tBuf," [%d]",i);		// for debugging
						lstrcat(tmp,tBuf);

					
						switch (MessageBox(hWnd,tmp,
											szControl_Warning,
											MB_YESNOCANCEL) )
							{	//Save  prompt
						case IDYES  :
							SendMessage(hWnd,WM_COMMAND,IDM_SAVE,0L);
							break;

						case IDNO :
							break;
						case IDCANCEL :
							return FALSE;	// return to windows from open menu selection
							}	//Save  prompt
						}	// file info is different from the originally loaded config file

					}	// A previous open file operation has occurred


				// Execute the "open file" dialog.
				if(fnDLG_OPEN(hWnd))
					{	// open dialog returned true
					
					fRecord = OpenFile((LPSTR)fnRecord,&ofRecord,OF_READ);
					strlwr(fnRecord);
					if(fRecord == -1)
						{
						lstrcpy(tmp,szControl_CantOpen);
						lstrcat(tmp,fnRecord);
						MessageBox(hWnd,tmp,
									szControl_Error,
									MB_OK);
						SendMessage(hWnd,WM_COMMAND,IDM_OPEN,0L);

						}
					else
						{	// succeeded in opening cfg file
						_lread(fRecord,lpRecord,sizeof(globalREC));
						lstrcpy((void *) &lpRecord->pipeinfo.fnRecord,fnRecord);

#if CUSTOMER!=MAV_CONROE

						lstrcpy((void *) &lpRecord->szVersionNumber,
													szVersionNumber);
						lpRecord->wVersion = wVersion;
#endif

						//	memcpy(lpWork, lpRecord, sizeof(globalREC));



						_lclose(fRecord);

						// Make certain critical paramaters are in range
						
						FixUpConfig();


						// work is bigger than lpRecord  ... jeh 11/11/94
						memcpy(lpWork, lpRecord, sizeof(globalREC));
						memcpy(lpDefault, lpRecord, sizeof(globalREC));


						if(hDlgDLG_STATUS)
							PostMessage(hDlgDLG_STATUS,WM_INITDIALOG,0,0);

#if 0
//testing

					if (memcmp(lpWork, lpRecord, sizeof(globalREC)) != 0 )
						{	// file info is different from the originally loaded config file
						lstrcpy(tmp,szControl_File);
						lstrcat(tmp,fnRecord);
						lstrcat(tmp,"  has changed.\nWork != Record. About to Open Link.");
						MessageBox(hWnd,tmp,"TESTING",MB_OK);
						}

//testing
#endif


#if 1
						if ( TheLink == 0)
							{	// transputer link not yet open

//						wResult = WinExec(szRunUDP,SW_SHOW);
// this works but we may not want to do it !
// if we do do it, make sure batch file changes working directory to 
// location where all udp files are.

#endif
						CloseLink(TheLink);
							if ( ( TheLink = OpenLink() ) < 0)
								{
								lstrcpy(tmp,szControl_CantOpenDDriver);
								MessageBox(hWnd,tmp,
											szControl_Error,
											MB_OK);
								return FALSE;
								}
							else
								{	// Communicate with transputer
								// Pack UdpCmd with the command to perform
								// UDPSEARCH.  Instrument should displa
								// udp's address on it's monitor if attached.


								lpWork->UdpMem[0] = 80;		// response length
								lpWork->UdpMem[1] = 0xf0;	// UDP command .
								lpWork->UdpMem[2] = 7;		// get chart data mode


								if(!SendToUdp())
									{
									lstrcpy(tmp,szControl_CommErrorS);
									MessageBox(hWnd,tmp,
												szControl_Error,
												MB_OK);
									ReceFromUdp();	// try to clear the transputer link
									return FALSE;
									}

								if(ReceFromUdp())
									{
									lstrcpy(tmp,szControl_TputerStatus);
//								lstrcat(tmp,lpWork->UdpMem);
									MessageBox(hWnd,tmp,
												szControl_Report,
												MB_OK);
									}
								else
									{
									lstrcpy(tmp,szControl_CommErrorR);
									MessageBox(hWnd,tmp,
												szControl_Error,
												MB_OK);
									return FALSE;
									}

								}	// Communicate with transputer

#if 1
							}	// transputer link not yet open
#endif

						//01/13/96 test cursor operations
						SetCursor(LoadCursor(NULL,IDC_WAIT));

#if SCOPE_DEMUX_1998
						// better setup scope demux before sending config file
						// read operation succeeds here
						LoadDemuxFile();
						CloseDemuxFiles();

#endif
						ProcPtrArray[SYSINIT]();


						update_all();	// refresh all open adjustment windows

						// Turn off pc104 debug
						lpWork->scope.pc104_debug = 0;
						ProcPtrArray[DEBUG_CONTROL]();
#if 0
//testing

					if (memcmp(lpWork, lpRecord, sizeof(globalREC)) != 0 )
						{	// file info is different from the originally loaded config file
						lstrcpy(tmp,szControl_File);
						lstrcat(tmp,fnRecord);
						lstrcat(tmp,"  has changed.\nWork != Record. After Open Link.");
						MessageBox(hWnd,tmp,"TESTING",MB_OK);
						}

//testing
#endif
						
						SendMessage(hWnd,WM_COMMAND,IDM_RUNSTATUS,0L);
						SetCursor(LoadCursor(NULL, IDC_ARROW));	//01/13/96

						//PROCESSMENU_OPEN_END
						SendWallToUDP(hWnd,lpWork->pipeinfo.Wall);

						}	// succeeded in opening cfg file

					}	// open dialog returned true
				break;

				
			case IDM_SAVE :
				//PROCESSMENU_SAVE_BEGIN
				fRecord = OpenFile((LPSTR)fnRecord,&ofRecord,OF_CREATE);
				strlwr(fnRecord);
				if(fRecord == -1)
					{
					lstrcpy(tmp,szControl_CantOpen);
					lstrcat(tmp,fnRecord);
					MessageBox(hWnd,tmp,
								szControl_Error,
								MB_OK);
					}
				else
					{
					_lwrite(fRecord,lpWork,sizeof(globalREC));
					memcpy(lpRecord, lpWork, sizeof(globalREC));
					lstrcpy(tmp,szControl_SavedFile);
					lstrcat(tmp,fnRecord);
					MessageBox(hWnd,tmp,
								szControl_FileOper,
								MB_OK);
					}

				_lclose(fRecord);

				//PROCESSMENU_SAVE_END

                break;

			case IDM_SAVEAS :
				file_extension_type = CONFIG_EXT;	// config file
				if(fnDLG_SAVE(hWnd))
					{
					//PROCESSMENU_SAVEAS_BEGIN
					// check to see if file already exists and allow
					// rename or abort  11/11/94
					fRecord = OpenFile((LPSTR)fnRecord,&ofRecord,OF_EXIST);
					if ( fRecord != HFILE_ERROR)
						{	// file already exists
						switch (MessageBox(hWnd,
											szControl_WriteOver,
											szControl_Warning,
											MB_YESNOCANCEL) )
							{	// msg box choice
						case IDYES  :
							fRecord = OpenFile((LPSTR)fnRecord,&ofRecord,OF_CREATE);
							strlwr(fnRecord);
							if(fRecord == -1)
								{
								lstrcpy(tmp,szControl_CantOpen);
								lstrcat(tmp,fnRecord);
								MessageBox(hWnd,tmp,
											szControl_Error,
											MB_OK);
								}
							else
								{
								_lwrite(fRecord,lpWork,sizeof(globalREC));
								memcpy(lpRecord, lpWork, sizeof(globalREC));
								lstrcpy(tmp,szControl_SavedFile);
								lstrcat(tmp,fnRecord);
								MessageBox(hWnd,tmp,
											szControl_FileOper,
											MB_OK);
								}
							_lclose(fRecord);
							if (hDlgDLG_STATUS != 0) PostMessage(hDlgDLG_STATUS,WM_INITDIALOG,0,0);

							break;

						case IDNO :
							PostMessage(hWnd,WM_COMMAND,IDM_SAVEAS,0L);
							break;
	
						case IDCANCEL :
							break;

							}	// msg box choice

						}	// file already exists

					else
						{	// file not pre existing
						fRecord = OpenFile((LPSTR)fnRecord,&ofRecord,OF_CREATE);
						strlwr(fnRecord);
						if(fRecord == -1)
							{
							lstrcpy(tmp,szControl_CantOpen);
							lstrcat(tmp,fnRecord);
							MessageBox(hWnd,tmp,
										szControl_Error,
										MB_OK);
							}
						else
							{
							_lwrite(fRecord,lpWork,sizeof(globalREC));
							memcpy(lpRecord, lpWork, sizeof(globalREC));
							lstrcpy(tmp,szControl_SavedFile);
							lstrcat(tmp,fnRecord);
							MessageBox(hWnd,tmp,
										szControl_FileOper,
										MB_OK);
							}

						_lclose(fRecord);
						if (hDlgDLG_STATUS != 0) PostMessage(hDlgDLG_STATUS,WM_INITDIALOG,0,0);
						//PROCESSMENU_SAVEAS_END
						}	// file not pre existing
					}
				break;
					
			case IDM_LOGOUT :
				//PROCESSMENU_LOGOUT_BEGIN
#if 0
				lpWork->UdpMem[0] = 80;		// response length
				lpWork->UdpMem[1] = 0xf0;	// UDP command .
				lpWork->UdpMem[2] = 7;		// get chart data

				if(!SendToUdp())
					{
					lstrcpy(tmp,szControl_CommErrorS);
					MessageBox(hWnd,tmp,
								szControl_Error,
								MB_OK);
					}

				if(ReceFromUdp())
					{
					lstrcpy(tmp,szControl_TputerStatus);
					MessageBox(hWnd,tmp,
								szControl_Report,
								MB_OK);
					}
				else
					{
					lstrcpy(tmp,szControl_CommErrorR);
					MessageBox(hWnd,tmp,
								szControl_Error,
								MB_OK);
					}
#endif
				// If possible, open error log and update with enet stats
				

				if (GetPrivateProfileString("Truscope","LOG",
									"",fnErrorLog,
									sizeof(fnErrorLog), 
									"TRUSCOPE.INI") == 0)
					fnErrorLog[0] = 0;	// none specified
		
				else
					{	// add to error log
					strcat(fnErrorLog,"\\error.log");
					fErrorLog = OpenFile((LPSTR)fnErrorLog,&ofErrorLog,OF_WRITE);
					strlwr(fnErrorLog);
					if(fErrorLog == HFILE_ERROR)
						{
						lstrcpy(tmp,szControl_CantOpen);
						lstrcat(tmp,fnErrorLog);
						MessageBox(hWnd,tmp,
									szControl_Error,
									MB_OK);
						}
					else
						{	// file open succeeded
						// get enet stats
						collect_enet_stats(hWnd);

						_llseek(fErrorLog,0L,2);	// append

						t = time(NULL);
						tblock = localtime(&t);
						strcpy(mmi_stop,szControl_MMIStopped);
						strcat(mmi_stop, asctime(tblock));
//						strcat(mmi_stop, "\n");

						read_enet_stats(hWnd);	//retrieve enet stats for all enet channels from
							// Master UDP

						// add some delay between read and collect

						_lwrite(fErrorLog,mmi_stop, strlen(mmi_stop));

						for ( j = 0; j < 5; j++)
							{
							sprintf(mmi_stop,"*** Enet %d ***\n",j);
							for ( i = 0 ; i < 13 ; i++)
								{
								strcat(mmi_stop, enet_label[i]);
								sprintf(tBuf,"%10ld", ether_stats[j].edata[i] );
								strcat(mmi_stop,tBuf);
								strcat(mmi_stop,"\n");
								}
							_lwrite(fErrorLog,mmi_stop, strlen(mmi_stop));
							}
						// 12-17-98 count lost packets based on pkt number
						strcpy(mmi_stop,szControl_LostPackets);
						for ( j = 0; j < 4; j++)
							{
							sprintf(tBuf,"%10ld  ",lLostPacketCnt[j]);
							strcat(mmi_stop,tBuf);
							}
						strcat(mmi_stop,"\n");
						_lwrite(fErrorLog,mmi_stop, strlen(mmi_stop));
						_lclose(fErrorLog);
						}	// file open succeeded
					}	// add to error log

				if ( (i = MemCmp(lpWork, lpRecord, sizeof(globalREC)) ) != 0 )
					{	// file info is different from the originally loaded config file
					lstrcpy(tmp,szControl_File );
					lstrcat(tmp,fnRecord);
					lstrcat(tmp,szControl_HasChangedExit);
						sprintf(tBuf," [%d]",i);		// for debugging
						lstrcat(tmp,tBuf);

					
					switch (MessageBox(hWnd,tmp,
										szControl_Warning,
										MB_YESNOCANCEL) )
						{	//Save and quit prompt
					case IDYES  :
						SendMessage(hWnd,WM_COMMAND,IDM_SAVE,0L);
						PostQuitMessage(0);
						break;

					case IDNO :
						PostQuitMessage(0);
						break;
					case IDCANCEL :
						break;
						}	//Save and quit prompt
					}	// file info is different from the originally loaded config file

				else PostQuitMessage(0);
	
                //PROCESSMENU_LOGOUT_END

				break;

				
			case IDM_FREEDISK:
				//Popup free disk dialog window
				if(hDlgDLG_FREEDISK)
					{
					DestroyWindow(hDlgDLG_FREEDISK);
					hDlgDLG_FREEDISK = 0;
					}

				fnDLG_FREEDISK(hWnd);

                break;


			case IDM_PRINTSCREEN:
				PrintTruscopeScreen();
				break;

			case IDM_AUTOPRINT:
				nAutoPrint ^= 1;	// exclusive or lsb
				nAutoPrint &= 1;	// set range to 0-1
				break;


			case IDM_DISKDUMP:
				nDiskDump ^= 1;	// exclusive or lsb
				nDiskDump &= 1;	// set range to 0-1
				break;

	/*  *********  Beginning of Instrument Menu Commands  *****************/

			case IDM_RECEIVER :

#if modeless
				// Don't allow receiver and oscope baseline set on 
				// screen at the same time
				if (hDlgDLG_OSET) break;

				
				if(hDlgDLG_RECEIVER)
					{
					DestroyWindow(hDlgDLG_RECEIVER);
					hDlgDLG_RECEIVER = 0;
					}
				else
				memcpy( &(lpDefault->receiver),&(lpWork->receiver), sizeof(receivREC));

				fnDLG_RECEIVER(hWnd);
				break;

#endif
				 case IDM_GATES :

#if modeless
				if(hDlgDLG_GATES)
					{
					DestroyWindow(hDlgDLG_GATES);
					hDlgDLG_GATES = 0;
					}
				else
				memcpy(&(lpDefault->gates), &(lpWork->gates),sizeof(gateREC));

				fnDLG_GATES(hWnd);
				break;
#else

				memcpy(&(lpDefault->gates), &(lpWork->gates),sizeof(gateREC));
                switch(fnDLG_GATES(hWnd))
					{
					 //PROCESSMENU_GATES_BEGIN
				case TRUE:
					memcpy(&(lpRecord->gates), &(lpWork->gates), sizeof(gateREC));
					break;

				case DEFAULT:
					memcpy(&(lpWork->gates), &(lpDefault->gates), sizeof(gateREC));
					break;

				case FALSE:
					break;
                //PROCESSMENU_GATES_END

                	}
				break;

#endif

				 case IDM_PULSER :

#if modeless
				if(hDlgDLG_PULSER)
					{
					DestroyWindow(hDlgDLG_PULSER);
					hDlgDLG_PULSER = 0;
					}
				else
				memcpy(&(lpDefault->pulser),&(lpWork->pulser),sizeof(pulseREC));

				fnDLG_PULSER(hWnd);
				break;
#else
				memcpy(&(lpDefault->pulser),&(lpWork->pulser),sizeof(pulseREC));
				switch(fnDLG_PULSER(hWnd))
					{
					 //PROCESSMENU_PULSER_BEGIN
				case TRUE:
					memcpy(&(lpRecord->pulser), &(lpWork->pulser), sizeof(pulseREC));
					break;

				case DEFAULT:
					memcpy(&(lpWork->pulser), &(lpDefault->pulser), sizeof(pulseREC));
					break;

				case FALSE:
					break;
                //PROCESSMENU_PULSER_END

                	}
				break;
#endif

#if 0
				 case IDM_ALARM :

#if modeless
				if(hDlgDLG_ALARM)
					{
					DestroyWindow(hDlgDLG_ALARM);
					hDlgDLG_ALARM = 0;
					}
				else
				memcpy(&(lpDefault->alarm),&(lpWork->alarm),sizeof(alarmREC));

				fnDLG_ALARM(hWnd);
				break;
#else
				memcpy(&(lpDefault->alarm),&(lpWork->alarm),sizeof(alarmREC));
                switch(fnDLG_ALARM(hWnd))
					{
					 //PROCESSMENU_ALARM_BEGIN
				case TRUE:
					memcpy(&(lpRecord->alarm), &(lpWork->alarm), sizeof(alarmREC));
					break;

				case DEFAULT:
					memcpy(&(lpWork->alarm), &(lpDefault->alarm), sizeof(alarmREC));
					break;

				case FALSE:
					break;
                //PROCESSMENU_ALARM_END

                	}
				break;

#endif

#endif

				 case IDM_TIMEOFFLIGHT :

#if modeless
				if(hDlgDLG_TOF)
					{
					DestroyWindow(hDlgDLG_TOF);
					hDlgDLG_TOF = 0;
					}
				else
				memcpy(&(lpDefault->timeoff),&(lpWork->timeoff),sizeof(timeREC));

				fnDLG_TOF(hWnd);
				break;
#else
				memcpy(&(lpDefault->timeoff),&(lpWork->timeoff),sizeof(timeREC));
                switch(fnDLG_TOF(hWnd))
					{
					 //PROCESSMENU_TIMEOFFLIGHT_BEGIN
				case TRUE:
					memcpy(&(lpRecord->timeoff), &(lpWork->timeoff), sizeof(timeREC));
					break;

				case DEFAULT:
					memcpy(&(lpWork->timeoff), &(lpDefault->timeoff), sizeof(timeREC));
					break;

				case FALSE:
					break;
                //PROCESSMENU_TIMEOFFLIGHT_END

					}
				break;

#endif
				 case IDM_SCOPE1:

				if(hDlgDLG_SCOP)
					{
					DestroyWindow(hDlgDLG_SCOP);
					hDlgDLG_SCOP = 0;
					}
	
				memcpy(&(lpDefault->scope),&(lpWork->scope),sizeof(scopeREC));
				simple_flag = TRUE;		// operators scope selections

				fnDLG_SCOP(hWnd);
				break;


		/***************  Scope selections made from Diagnostic menu ********/
			
				 case IDM_SCOPE2 :

				if(hDlgDLG_SCOP)
					{
					DestroyWindow(hDlgDLG_SCOP);
					hDlgDLG_SCOP = 0;
					}
	
				memcpy(&(lpDefault->scope),&(lpWork->scope),sizeof(scopeREC));
				simple_flag = FALSE;	// full blown dialog

				fnDLG_SCOP(hWnd);
				break;


		/***** Show Memory transfers between MMI and the UDP *****/
			
				 case IDM_UDPIO:

				if(hDlgDLG_UDPIO)
					{
					DestroyWindow(hDlgDLG_UDPIO);
					hDlgDLG_UDPIO = 0;
					}

				fnDLG_UDPIO(hWnd);
				break;




		/***** Control Sync Registers from diagnostic menu ***************/
			
				 case IDM_SYNC:

				if(hDlgDLG_SYNC)
					{
					DestroyWindow(hDlgDLG_SYNC);
					hDlgDLG_SYNC = 0;
					}

				fnDLG_SYNC(hWnd);
				break;

		/************  Set up TCG for correcting signal attenuation with time **/
			
				 case IDM_TCGSET:

				if(hDlgDLG_TCGSET)
					{
					DestroyWindow(hDlgDLG_TCGSET);
					hDlgDLG_TCGSET = 0;
					}

				fnDLG_TCGSET(hWnd);
				break;


		/*********** Allow variation of system gain and Flaw Scalers ********/
			case IDM_SYSGAIN :

				if(hDlgDLG_SYSGAIN)
					{
					DestroyWindow(hDlgDLG_SYSGAIN);
					hDlgDLG_SYSGAIN = 0;
					}
				else
					{
					memcpy( &(lpDefault->receiver),&(lpWork->receiver), 
													sizeof(receivREC));
					for ( i = 0; i < MAX_CHANNEL; i++)
						{
						lpDefault->cal.idScale[i] = lpWork->cal.idScale[i];
						lpDefault->cal.odScale[i] = lpWork->cal.odScale[i];
						}
					}

				fnDLG_SYSGAIN(hWnd);
				break;


		/*********** Display Status alarms such as missed packets *******/
			case IDM_ALARMSTATUS :

				if(hDlgDLG_ALARMSTATUS)
					{
					DestroyWindow(hDlgDLG_ALARMSTATUS);
					hDlgDLG_ALARMSTATUS = 0;
					}

				fnDLG_ALARMSTATUS(hWnd);
				break;



			case IDM_ALLPANELS:
				// Generate all 6 instrument panes without having to click
				// on each one individually

				PostMessage(hWnd,WM_COMMAND,IDM_RECEIVER,0L);
				PostMessage(hWnd,WM_COMMAND,IDM_GATES,0L);
				PostMessage(hWnd,WM_COMMAND,IDM_PULSER,0L);
//				PostMessage(hWnd,WM_COMMAND,IDM_ALARM,0L);
				PostMessage(hWnd,WM_COMMAND,IDM_TIMEOFFLIGHT,0L);
				PostMessage(hWnd,WM_COMMAND,IDM_SCOPE1,0L);

				break;
				

			case IDM_CLOSEALLPANELS:
				// Close all 6 instrument panes without having to click
				// on each one individually

				DestroyWindow(hDlgDLG_RECEIVER);
				hDlgDLG_RECEIVER = 0;
				DestroyWindow(hDlgDLG_GATES);
				hDlgDLG_GATES = 0;
				DestroyWindow(hDlgDLG_PULSER);
				hDlgDLG_PULSER = 0;
//				DestroyWindow(hDlgDLG_ALARM);
//				hDlgDLG_ALARM = 0;
				DestroyWindow(hDlgDLG_TOF);
				hDlgDLG_TOF = 0;
				DestroyWindow(hDlgDLG_SCOP);
				hDlgDLG_SCOP = 0;

				break;
				

				 case IDM_TRANSDUCERPOSI :
#if 0
					 memcpy(&(lpWork->syscfg),&(lpRecord->syscfg),sizeof(configREC));
					memcpy(&(lpDefault->syscfg),&(lpWork->syscfg),sizeof(configREC));
					switch(fnDLG_TRANPOSI(hWnd))
						{
						//PROCESSMENU_TRANDUCERPOSI_BEGIN
					case TRUE:
						memcpy(&(lpRecord->syscfg), &(lpWork->syscfg), sizeof(configREC));
						break;

					case DEFAULT:
						memcpy(&(lpWork->syscfg), &(lpDefault->syscfg), sizeof(configREC));
						break;

					case FALSE:
						break;
					//PROCESSMENU_TRANDUCERPOSI_END

						}

#endif

					break;

				 case IDM_MACHINEOFFSET :
#if 0
					memcpy(&(lpWork->syscfg), &(lpRecord->syscfg), sizeof(configREC));
					switch(fnDLG_POSITION(hWnd))
						{
						//PROCESSMENU_MACHINEOFFSET_BEGIN
					case TRUE:
						memcpy(&(lpRecord->syscfg), &(lpWork->syscfg), sizeof(configREC));
						break;

					case DEFAULT:
						memcpy(&(lpRecord->syscfg), &(lpDefault->syscfg), sizeof(configREC));
						break;

					case FALSE:
						break;
						//PROCESSMENU_MACHINEOFFSET_END

						}
#endif
					break;



#if 0
				 case IDM_ULCPARA :
					memcpy(&(lpWork->syscfg), &(lpRecord->syscfg), sizeof(configREC));
	                /* customer header config dialog */
	                switch(customer_info(hWnd))
					//switch(fnDLG_POSITION(hWnd))
						{
						//PROCESSMENU_ULCPARA_BEGIN
					case TRUE:
						memcpy(&(lpRecord->syscfg), &(lpWork->syscfg), sizeof(configREC));
						break;

					case DEFAULT:
						memcpy(&(lpRecord->syscfg), &(lpDefault->syscfg), sizeof(configREC));
						break;

					case FALSE:
						break;
						//PROCESSMENU_ULCPARA_END

						}
					break;
#endif



				case IDM_SCOPEOFFSET:
					// adjust dc offsets to each instruments oscilloscope signals
					if (hDlgDLG_OSET)  return FALSE;

					// Don't allow receiver and oscope baseline set on 
					// screen at the same time

					if(hDlgDLG_RECEIVER) return FALSE;

					// substitute low gain values for receiver for setting offset
					// When this window exist, it restores the values saved here

					memcpy( &(lpDefault->receiver),
							&(lpWork->receiver), sizeof(receivREC));
		

					for ( i = 0; i < MAX_CHANNEL; i++)
						{
						lpWork->receiver.reject[i] = 20;
						lpWork->receiver.gain[i] = GAINMIN;
						}
											
					fnDLG_OSET(hWnd);
					break;

					
				case IDM_UDP :
//			 		if(hDlgDLG_STRIP)		return FALSE;

			    	memcpy(&(lpDefault->syscfg),&(lpWork->syscfg),sizeof(configREC));

					fnDLG_UDPCFG(hWnd);	// add un-do.. don't care what exit
										// case is . Undo handled by udpc.c
					break;

				case IDM_STRIPCHART :
					{
					 //PROCESSMENU_STRIP_CHART_BEGIN
					 // Navasota reports multiple coupling windows 09/28/00
					 // Kill this 1st.  Then strip window doesn't see it
					 // if strip gets killed next.
					 
					if (hDlgDLG_COUPLING)
						{
						DestroyWindow(hDlgDLG_COUPLING);
						hDlgDLG_COUPLING = 0;
						}
						
					if(hDlgDLG_STRIP)
						{
						DestroyWindow(hDlgDLG_STRIP);
						hDlgDLG_STRIP = 0;
						return FALSE;
						}

					if(hDlgDLG_CATCH)
						{
						DestroyWindow(hDlgDLG_CATCH);
						hDlgDLG_CATCH = 0;
						}

#if 1
					if(hDlgDLG_BAR)
						{
                  		DestroyWindow(hDlgDLG_BAR);
                  		hDlgDLG_BAR = 0;
						}
#endif


					if(fnDLG_STRIP(hWnd))
						{
						PostMessage(hDlgDLG_STRIP,WM_COMMAND,STOP_RB,0L);

						}
					 //PROCESSMENU_STRIP_CHART_END
					}
					break;


				case IDM_BARCHART :
                	{	// Flaw channel calibration window

#if 1
					if(hDlgDLG_BAR)
						{
                  		DestroyWindow(hDlgDLG_BAR);
                  		hDlgDLG_BAR = 0;
						return FALSE;
						}

					 
					if (hDlgDLG_COUPLING)
						{
						DestroyWindow(hDlgDLG_COUPLING);
						hDlgDLG_COUPLING = 0;
						}

					if(hDlgDLG_STRIP)
						{
                  		DestroyWindow(hDlgDLG_STRIP);
                  		hDlgDLG_STRIP = 0;
						}

					if(fnDLG_BAR(hWnd))
						{	// popup gates and receiver windows
						PostMessage(hWnd,WM_COMMAND,IDM_GATES,0L);
						PostMessage(hWnd,WM_COMMAND,IDM_RECEIVER,0L);
						}
#endif
					}	// Flaw channel calibration window
					break;
			

				case IDM_FLAWTEXT:
                	{	// Flaw channel depth calibration window

					if(hDlgDLG_FLAWTEXT)
						{
                  		DestroyWindow(hDlgDLG_FLAWTEXT);
                  		hDlgDLG_FLAWTEXT = 0;
						return FALSE;
						}
					 
					if (hDlgDLG_COUPLING)
						{
						DestroyWindow(hDlgDLG_COUPLING);
						hDlgDLG_COUPLING = 0;
						}

					if(hDlgDLG_STRIP)
						{
                  		DestroyWindow(hDlgDLG_STRIP);
                  		hDlgDLG_STRIP = 0;
						}

					fnDLG_FlawScaleText(hWnd);

					}	// Flaw channel calibration window
					break;
			
				case IDM_IMAGEDISP :
                	{
                	//PROCESSMENU_STRIP_CHART_BEGIN
					//PROCESSMENU_STRIP_CHART_END
                	}
					break;
				case IDM_ADP :
                	{
                	//PROCESSMENU_ADP_BEGIN
					//PROCESSMENU_ADP_END

					}
                	break;
                	
             case IDM_SDP :
                {
                //PROCESSMENU_SDP_BEGIN
                //PROCESSMENU_SDP_END

                }
                break;
             case IDM_WALL_CALIBRATION :
                {
                //PROCESSMENU_CALIBRATION_BEGIN
					 //PROCESSMENU_CALIBRATION_END

				/*
				Don't even start if we aren't using a wall channel
			
				*/
				if ( lpWork->syscfg.chan_type[channel] != IS_WALL)
					{
					MessageBox(NULL, 
								szControl_ActiveChnl,
								szControl_Notice,
								MB_OK);
					break;
					}

					 
				if (hDlgDLG_COUPLING)
					{
					DestroyWindow(hDlgDLG_COUPLING);
					hDlgDLG_COUPLING = 0;
					}

				if(hDlgDLG_STRIP)
					{
					DestroyWindow(hDlgDLG_STRIP);
					hDlgDLG_STRIP = 0;
					warp_speed(0);
					}

				if(hDlgDLG_CAL)
					{
					DestroyWindow(hDlgDLG_CAL);
					hDlgDLG_CAL = 0;
					}

				memcpy(&(lpDefault->syscfg),&(lpWork->syscfg),sizeof(configREC));

				fnDLG_CAL(hWnd);
				break;

                }

			case IDM_RUNSTATUS :
#if 0
//testing
//.. this message appeared

					if (memcmp(lpWork, lpRecord, sizeof(globalREC)) != 0 )
						{	// file info is different from the originally loaded config file
						lstrcpy(tmp,szControl_File);
						lstrcat(tmp,fnRecord);
						lstrcat(tmp,"  has changed.\nWork != Record. About to Run Status.");
						MessageBox(hWnd,tmp,"TESTING",MB_OK);
						}

//testing
#endif
				if(hDlgDLG_STATUS)
					{

#if 1
					SendMessage(hDlgDLG_STATUS, WM_DESTROY,0,0L);
//					DestroyWindow(hDlgDLG_STATUS);
//					hDlgDLG_STATUS = 0;
#endif
					return FALSE;
					}
             	
				if(fnDLG_STATUS(hWnd))
					{
					//PROCESSMENU_RUNSTATUS_BEGIN
					//PROCESSMENU_RUNSTATUS_END

					}

				break;
             case IDM_NOHARDWARE :
                {
                //PROCESSMENU_NOHARDWARE_BEGIN
                //PROCESSMENU_NOHARDWARE_END

                }
                break;
             case IDM_LOADSERVER :
                {
                //PROCESSMENU_LOADSERVER_BEGIN
                //PROCESSMENU_LOADSERVER_END

                }
                break;

			case IDM_HELP_CONTENTS:
#if 0
				if ( strlen(szHelpDirName) > 0)
					chdir(szHelpDirName);
#endif
				GetTuboHelp(hWnd, szHelpFileName, HELP_INDEX, 0L);
				break;


			case IDM_ABOUT :
                if(fnDLG_ABOUT(hWnd))
                {
                //PROCESSMENU_ABOUT_BEGIN
                //PROCESSMENU_ABOUT_END

                }
                break;

			case IDM_ENETSTATUS:
			case IDM_ENETSTATUS_HOTKEY:
				if(hDlgDLG_ENET_STAT)
					{

#if 1
					DestroyWindow(hDlgDLG_ENET_STAT);
					hDlgDLG_ENET_STAT = 0;
#endif
					return FALSE;
					}
             	
				if(fnDLG_ENET_STAT(hWnd))
					{
					//PROCESSMENU_ENET_STATUS_BEGIN
					//PROCESSMENU_ENET_STATUS_END
					}
				break;


			case IDM_CATCH:
				if(hDlgDLG_CATCH)
					{

#if 1
					DestroyWindow(hDlgDLG_CATCH);
					hDlgDLG_CATCH = 0;
#endif
					return FALSE;
					}

				// Don't let the strip chart request data while doing this
					 
				if (hDlgDLG_COUPLING)
					{
					DestroyWindow(hDlgDLG_COUPLING);
					hDlgDLG_COUPLING = 0;
					}

				if(hDlgDLG_STRIP)
					{
					DestroyWindow(hDlgDLG_STRIP);
					hDlgDLG_STRIP = 0;
					}
             	
				if(fnDLG_CATCH(hWnd))
					{
					//PROCESSMENU_CATCH_BEGIN
					//PROCESSMENU_CATCH_END
					}
				break;

				}      // Determine which Menu ID
			}	// Process Menu Commands

		else
			{
			//REGEN_CUSTOMCOMMAND
			//REGEN_CUSTOMCOMMAND

			}
		break;

	case WM_DESTROY :
		//REGEN_WM_DESTROY
		//REGEN_WM_DESTROY
		warp_speed(0);	// not windows portable
		PostQuitMessage(0);
		break;

	case WM_SETFOCUS:
		warp_speed(0);
		// fall thru to default processing

	default :
		return DefWindowProc(hWnd, wMessage, wParam, lParam);
		}
	return 0L;
	}

//REGEN_CUSTOMCODE
//REGEN_CUSTOMCODE

void far * GetINSTRamPtr()
	{
	lpInst = MK_FP(LOWORD(&_D000H), 0x0);
	//	lpInst = MK_FP(0x2000, 0x0000);
	return(lpInst);
	}
/*  Copyright 1994, Tuboscope-Vetco International Inc. All rights reserved */
