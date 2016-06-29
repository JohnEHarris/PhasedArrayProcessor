/* options statment  for UDP
	created by HY Wang 1/4/94
file:	english.h
Function: Define text strings which could be translated into another language
Revised:	09/07/94 by jeh... add polarity button labels
revised:	11/29/94 for more scope selections  jeh.  Now have 3 variables,
			scope_opt1, scope_opt2a and scope_opt2b for a and b channels on board
			12/13/94 New gate windows trigger opts, buttons for alarms instead
					of combo boxes.
			01/12/95 new scope mux selects
			10/03/95 2 categories of oblique defects


*/
#ifndef ENGLISH_H

#define ENGLISH_H
char *recv_detector[] = {"rf", "fw"};
char *polarity_btn[] = {"- - pol - -", "++ pol ++"};

char *recv_filter[] = {"wide band","2.25 MHz","3.50 MHz","5.00 MHz"};

/* TCG trigger and step selectors   03/09/98 */
char *tcg_trigger[] = {"Off","IP",
						"IF", "Gate 2", "Intface Echo",
						"Slave A", "Slave B"};
char *tcg_step[] = {"200 ns", "400 ns", "600 ns","800 ns"};

char *tcg_fn[] = {"At + Btt",
				  "Step .. BU(t - A)",
				  "B(1-exp(-at) )",
				  "(B+1)/( 1+Bexp(-at) )"
				};

#if OLD_SCOPE_MUX
// prior to 12/13/94
char *gate_trigger_g1[] = {"Disable","Init Pulse","Interface Echo","Slave A", "Slave B"};
char *gate_trigger_g2[] = {"Disable","Init Pulse","Interface Echo","Slave A", "Slave B"};

#else

char *gate_trigger_g1[] = { "Disable","Init Pulse","Interface Echo",
							"Slave A", "Slave B"};
char *gate_trigger_g2[] = { "Disable","Init Pulse","Interface Echo",
							"Slave A", "Slave B"};
#endif


char *st_invd = "High";
char *st_low = "High";
char *st_high = "High";
char *tof_start_g1[] = {"Disable","Init Pulse","IF Echo","Threshold Det","Peak Detect","Slave A","Slave B"};
char *tof_start_g2[] = {"Disable","Init Pulse","IF Echo","Threshold Det","Peak Detect","Slave A","Slave B"};
// char *tof_stop_g1[] = {"Threshold Detect","Not allowed","Max Peak"};

#if OLD_SCOPE_MUX
char *tof_stop_g1[] = {"Threshold Detect","Peak Detect"};
char *tof_stop_g2[] = {"Threshold Detect","Peak Detect"};
char *tof_stop_g1_print[] = {"Threshold","Peak Detect"};

#else

char *tof_stop_g1[] = {"Threshold Detect","Peak Detect"};
char *tof_stop_g2[] = {"Threshold Detect","Peak Detect"};
char *tof_stop_g1_print[] = {"Threshold","Peak Detect"};

#endif


char *st_sysOn = "SysOn";
char *st_sysOff = "SysOff";
char *st_on = "on";
char *st_off = "off";

#if !CONFIG_DUMP
// vc++ doesn't like all these st_on variables....
char *st_On = "On";
char *st_Off = "Off";
char *st_ON = " ON ";
char *st_OFF = " OFF";
#endif

char *st_chnOn = "Ch#/On";
char *st_chnOff = "Ch#/Off";
char *st_wall = "WALL";
char *st_lamn = "LAMN";
char *st_long = "LONG";
char *st_tran = "TRAN";
char *st_obq2 = "OBQ2";
char *st_obq4 = "OBQ4";
char *st_none = "NONE";
char *st_all = "ALL";




// Alarm window
#if 0
char *ama_triger[] = {"Disable","Threshold","Peak","Max Peak"};
char *ama_gate_g1[] = {"Disable","Threshold"};
char *ama_gate_g2[] = {"Disable","Threshold","Peak"};
char *ama_prom_g1 = "Laminar";
char *ama_prom_g2 = "Wall";
#endif

char *ama_triger[] =  {"ALARM [OFF]","ALARM  [ON]"};
char *ama_gate_g1[] = {"GATE [OFF]","GATE [ON]"};
char *ama_gate_g2[] = {"GATE [OFF]","GATE [ON]"};
char *ama_prom_g1[2] = { "LAM [OFF]", "LAM [ON]"};
char *ama_prom_g2[2] = { "WALL [OFF]", "WALL [ON]"};
char *ama_log_pos = "LOGIC [ + ]";
char *ama_log_min = "LOGIC [ - ]";
char *ama_logic[2] = 	{"LOGIC [ - ]","LOGIC [ + ]"};

char *gate_lam_on = "Disable";
char *gate_lam_off = "Enable";

#if OLD_SCOPE_MUX

char *scope_opt1[] = {"Signal Detect",
							 "Pulse out",
							 "PK AMP GateI",
							 "PK AMP GateII"};

char *scope_opt2[] = {"Gates ",
							 "Blank Gates ",
							 "Laminar Gates",
							 "Main Bang","First Frame"}; /* 6.7 */

#else

		/* Not used, eliminate linker error */
char *scope_opt2[] = {"Gates ",
							 "Blank Gates ",
							 "Laminar Gates",
							 "Main Bang","First Frame"}; /* 6.7 */

#if REV1997_2CHNL_BD==IS_ONE

// would not work for defining rev1997 to be 1, had to be IS_ONE
	// 2 chnl bd developed in summer of 97

char *scope_opt1[] = {	"1997xx",
						"Detect Sig Out",
						"Pulse Sig Out",
						"Filtered Sig Out"};

char *scope_opt1A[] = {	"FW",
						"RF",
						"Zero Ref",
						"Rec Out"	};


char *scope_opt1B[] = {	"FW",
						"RF",
						"80%  Ref",
						"Rec Out"	};

char *scope_opt2A[] = {	"Instrument SYNC",		// no effect in instrument
						"End of A-scan",		// no effect in instrument
						"First of Frame",		// no effect in instrument
						"Gates 1 & 2",
						"Blank G1-Th G2-Pk",
						"Blank G1-Th G2-Th",
						"Laminar Gate",
						"Laminar Alarm",
						"Wall Alarm",
						"TOF Gate 1",
						"TOF Gate 2",
						"TCG curve",
						"Pulse out",
						"Test Point 1",
						"TCG Gate"
						};


char *scope_opt2B[] = {	"Instrument SYNC",
						"End of A-scan",
						"First of Frame",
						"Gates 1 & 2",
						"Blanking G1-Th G2-Pk",
						"Blanking G1-Th G2-Th",
						"Laminar Gate",
						"Laminar Alarm",
						"Wall Alarm",
						"TOF Gate 1",
						"TOF Gate 2",
						"TCG curve",
						"Pulse out",
						"Test Point 2",
						"TCG Gate"
						};

#else
// pre 1997 2 chnl bd

char *scope_opt1[] = {	"A-Scan",
						"Detect Sig Out",
						"Pulse Sig Out",
						"Filtered Sig Out"};

char *scope_opt2A[] = {	"Instrument SYNC",		// no effect in instrument
						"End of A-scan",		// no effect in instrument
						"First of Frame",		// no effect in instrument
						"Gates 1 & 2",
						"Blank G1-Th G2-Pk",
						"Blank G1-Th G2-Th",
						"Laminar Gate",
						"Laminar Alarm",
						"Wall Alarm",
						"TOF Gate 1",
						"TOF Gate 2",
						"Pk Amp Gate 1",
						"Pk Amp Gate 2",
						"FIFO Full Error"
						};


char *scope_opt2B[] = {	"Instrument SYNC",
						"End of A-scan",
						"First of Frame",
						"Gates 1 & 2",
						"Blanking G1-Th G2-Pk",
						"Blanking G1-Th G2-Th",
						"Laminar Gate",
						"Laminar Alarm",
						"Wall Alarm",
						"TOF Gate 1",
						"TOF Gate 2",
						"Pk Amp Gate 1",
						"Pk Amp Gate 2",
						"FIFO Empty Error"
						};
#endif
					


char *scope_trigger[] = {	"Initial Pulse",
							"First of Frame",
							"Gate 1",
							"Gate 2",
							"Gates 1 & 2"
							};
		
#endif
// Calibration signals for adjusting paper strip chart
char *st_chart_cal_modes[] = { "Idata", "DC Level","Random", "Ramp", "Sine" };



char *trig_mode[] =		{
						"local trig - no out",
						"local trig - slave out",
						"local trig - master out",
						"slave trig - pass thru",
						"slave trig - no out..end of chain"
						};

char*sync_mode[] =		{
						"local in - slave pass thru",
						"local in - master out",
						"slave in - pass thru"
						};

///Scope demux 1998

// Trace 1 is for every channel
char *szDemuxT1[] =
		{
		"Full Wave Zero"	,
		"Full Wave Full Scale"	,
		"RF Zero"	,
		"RF Full Scale"	
		};

char *szDemuxT1Label[] =
		{
		"FW Z"	,
		"FW FS"	,
		"RF Z"	,
		"RF FS"	
		};

// Trace 2 is for the entire system
char *szDemuxT2[] =
		{
		"Full Wave Zero"	,
		"Gate 1 Zero"	,
		"Gate 1 Full Scale"	,
		"RF Zero"	,
		"Gate 2 Zero"	,
		"Gate 2 Full Scale"	,
		"FW A-Scan Signal Offset"	,
		"RF A-Scan Signal Offset"	,
		"FW Reject Offset 10%"	,
		"FW Reject Offset 90%"
		};

char *szDemuxT2Label[] =
		{
		"FW Z"	,
		"G1 Z"	,
		"G1 FS"	,
		"RF Z"	,
		"G2 Z"	,
		"G2 FS"	,
		"FW Offset"	,
		"RF Offset"	,
		"Reject10"	,
		"Reject90"
		};
// dlg_gate.c module
char szGate_uSec[] =				"%-7.1f us";
char szGate_Inch[] =				"%-7.3f in";
char szGate_mm[] =					"%-7.2f mm";
char szGate_Gates[] =				"  Gates  g=";


// dlg_open.c module
char szOpen_ConfigFile[] =			"OPEN Config File";
char szOpen_DataFile[] =			"OPEN Data File";


// dlg_puls.c module
char szPuls_Inch[] =				"%-6.3f inch";
char szPuls_mm[] =					"%-6.2f mm";
char szPuls_Pulser[] =				"   Pulser     ";
char szPuls_Hz[] =					"%7d Hz";
char *szBar_ResetScalersTo1=		"Reset ALL Flaw Scalers to 1.0 ?";
char *szControl_Warning=			"WARNING !";



#endif