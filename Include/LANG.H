/*
lang.h

Function: Define text strings which could be translated into another language

Language header file for 'c' code operations.
Language header file for dialog resources is lang_rc.h
06/30/99
10/27/99 ADD szTof_Tof[]
12/06/99 add szStrip2_PlaybackTime,szDchan_Pen
11/16/00 add Cfg file change info for replay operation

*/
/**********************************************************/
/**** This file is included in control.00x or control.c ***/
/**********************************************************/
#ifndef LANG_H
#define LANG_H
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// variable names prior to 6/30/99

char *recv_detector[] = {
						"rf",
						"fw"
						};
char *polarity_btn[] = {
						"- - pol - -", 
						"++ pol ++"
						};

char *recv_filter[] = {
						"wide band",
						"2.25 MHz",
						"3.50 MHz",
						"5.00 MHz"
						};

/* TCG trigger and step selectors   03/09/98 */
char *tcg_trigger[] = {
						"Off",
						"IP",
						"IF", 
						"Gate 2", 
						"Interface Echo",
						"Slave A", 
						"Slave B"
						};
char *tcg_step[] = {
						"200 ns", 
						"400 ns", 
						"600 ns",
						"800 ns"
						};

char *tcg_fn[] = {
						"At + Btt",
						"Step .. BU(t - A)",
						"B(1-exp(-at) )",
						"(B+1)/( 1+Bexp(-at) )"
				};

#if OLD_SCOPE_MUX
// prior to 12/13/94
char *gate_trigger_g1[] = {
						"Disable",
						"Initial Pulse",
						"Interface Echo",
						"Slave A", 
						"Slave B"
						};
char *gate_trigger_g2[] = {
						"Disable",
						"Initial Pulse",
						"Interface Echo",
						"Slave A", 
						"Slave B"
						};

#else

char *gate_trigger_g1[] = { 
						"Disable",
						"Initial Pulse",
						"Interface Echo",
						"Slave A", 
						"Slave B"
						};
char *gate_trigger_g2[] = { 
						"Disable",
						"Initial Pulse",
						"Interface Echo",
						"Slave A", 
						"Slave B"
						};
#endif


char *st_invd = 		"High";
char *st_low = 			"High";
char *st_high = 		"High";
char *tof_start_g1[] = {
						"Disable",
						"Initial Pulse",
						"Interface Echo",
						"Threshold Detect",
						"Peak Detect",
						"Slave A",
						"Slave B"
						};
char *tof_start_g2[] = {
						"Disable",
						"Initial Pulse",
						"Interface Echo",
						"Threshold Detect",
						"Peak Detect",
						"Slave A",
						"Slave B"
						};
char *tof_start_g1_print[] = {
						"Disable",
						"Initial Pulse",
						"Interface Echo",
						"Threshold",
						"Peak Detect",
						"Slave A",
						"Slave B"
						};

// char *tof_stop_g1[] = {"Threshold Detect","Not Allowed","Max Peak"};

#if OLD_SCOPE_MUX
char *tof_stop_g1[] = {
						"Threshold Detect",
						"Max Peak"
						};
char *tof_stop_g2[] = {
						"Threshold Detect",
						"Peak Detect",
						"Max Peak"
						};
char *tof_stop_g1_print[] = {
						"Threshold",
						"Peak Detect"
						};
#else

char *tof_stop_g1[] = {
						"Threshold Detect",
						"Peak Detect"
						};
char *tof_stop_g2[] = {
						"Threshold Detect",
						"Peak Detect"
						};

char *tof_stop_g1_print[] = {
						"Threshold",
						"Peak Detect"
						};

#endif


char *st_sysOn = 		"SysOn";
char *st_sysOff = 		"SysOff";
char *st_on = 			"On";
char *st_off = 			"Off";

//#if !CONFIG_DUMP
// vc++ doesn't like all these st_on variables....
char *st_On = 			"On ";
char *st_Off = 			"Off";
char *st_ON = 			" ON   ";
char *st_OFF = 			" OFF ";
//#endif

char *st_chnOn = 		"Ch#/On";
char *st_chnOff = 		"Ch#/Off";
char *st_wall = 		"WALL";
char *st_lamn = 		"LAMN";
char *st_long = 		"LONG";
char *st_tran = 		"TRAN";
char *st_obq2 = 		"OBQ2";
char *st_obq4 = 		"OBQ4";
char *st_none = 		"NONE";
char *st_all = 			"ALL";




// Alarm window
#if 0
char *ama_triger[] = {
						"Disable",
						"Threshold",
						"Peak",
						"Max Peak"
						};
char *ama_gate_g1[] = {
						"Disable",
						"Threshold"
						};
char *ama_gate_g2[] = {
						"Disable",
						"Threshold",
						"Peak"
						};
char *ama_prom_g1 = 	"Laminar";
char *ama_prom_g2 = 	"Wall";
#endif

char *ama_triger[] =  {
						"ALARM [OFF]",
						"ALARM [ON]"
						};
char *ama_gate_g1[] = {
						"GATE [OFF]",
						"GATE [ON]"
						};
char *ama_gate_g2[] = {
						"GATE [OFF]",
						"GATE [ON]"
						};
char *ama_prom_g1[2] = { 
						"LAM [OFF]", 
						"LAM [ON]"
						};
char *ama_prom_g2[2] = { 
						"WALL [OFF]", 
						"WALL [ON]"
						};
char *ama_log_pos = 	"LOGIC [ + ]";
char *ama_log_min = 	"LOGIC [ - ]";
char *ama_logic[2] = 	{"LOGIC [ - ]","LOGIC [ + ]"};

char *gate_lam_on = 	"Disable";
char *gate_lam_off = 	"Enable";

#if OLD_SCOPE_MUX

char *scope_opt1[] = {
						"Signal Detect",
						"Pulse Out",
						"PK AMP GateI",
						"PK AMP GateII"};

char *scope_opt2[] = {
						"Gates ",
						"Blank Gates ",
						"Laminar Gates",
						"Main Bang",
						"First Frame"
						}; /* 6.7 */

#else

		/* Not used, eliminate linker error */
char *scope_opt2[] = {
						"Gates ",
						"Blank Gates ",
						"Laminar Gates",
						"Main Bang",
						"First Frame"
						}; /* 6.7 */

#if REV1997_2CHNL_BD==IS_ONE

// would not work for defining rev1997 to be 1, had to be IS_ONE
	// 2 chnl bd developed in summer of 97

char *scope_opt1[] = {	
						"1997xx",
						"Detect Sig Out",
						"Pulse Sig Out",
						"Filtered Sig Out"
						};

char *scope_opt1A[] = {	
						"FW",
						"RF",
						"Zero Ref",
						"Rec Out"	
						};


char *scope_opt1B[] = {	
						"FW",
						"RF",
						"80%  Ref",
						"Rec Out"	
						};

char *scope_opt2A[] = {	
						"Instrument SYNC",		// no effect in instrument
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


char *scope_opt2B[] = {	
						"Instrument SYNC",
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

char *scope_opt1[] = {	
						"A-Scan",
						"Detect Sig Out",
						"Pulse Sig Out",
						"Filtered Sig Out"
						};

char *scope_opt2A[] = {	
						"Instrument SYNC",		// no effect in instrument
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


char *scope_opt2B[] = {	
						"Instrument SYNC",
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
					


char *scope_trigger[] = {	
						"Initial Pulse",
						"First of Frame",
						"Gate 1",
						"Gate 2",
						"Gates 1 & 2"
						};
		
#endif
// Calibration signals for adjusting paper strip chart
char *st_chart_cal_modes[] = { 
						"Idata", 
						"DC Level",
						"Random", 
						"Ramp", 
						"Sine" 
						};



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
// dlg_oset.002
// Trace 1 is for every channel
char *szDemuxT1[] =
						{
						"Full Wave Zero"	,
						"Full Wave Full Scale"	,
						"RF Zero"	,
						"RF Full Scale"	
						};

char *szDemuxT1Label[] ={
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

char szOset2_ChNot0_6[] =			"Channel is not 0-6";
char szOset2_DacErr[] =				"Serial DAC ERROR";
char szOset2_DataRange[]=			"Data not in range 0-1023 for DAC%-d";
char szOset2_CantOpenDemuxFile[]=	"Can't open Demux file";
char szOset2_CantOpenDemuxFile2[]=	"Can't open back up Demux file";
char szOset2_NoTimer[] =			"Unable to create timer function";
char szOset2_ScopeCal[] =			"SCOPE CALIBRATE   ch= %2d";
char szOset2_RejOff[] =				"Reject Off";
char szOset2_RejOn[] =				"Reject On";
char szOset2_Rej__[] =				"Reject --";
char szOset2_NoFlawData[] =			"Failed to Get Flaw Data";
char szOset2_UnDo[] =				"Sure you want to un-DO all scope demux settings ?";
char szOset2_NoTrace1_CB[] =		"Can't get Dialog Item Trace1_CB";
char szOset2_NoTrace2_CB[] =		"Can't get Dialog Item Trace2_CB";



// labels added after 6/30/99 to facilitate translation to Czech.
// includes text from message boxes.  By module

// control.002 module

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

char szAppName[] = "CONTROL";
char szTitleBar[] = 
		"TRUSCOPE INSTRUMENT       Copyright 1996 Tuboscope Vetco";


char szControl_DialogDisp[] =		"Unable to display dialog";
char szControl_SystemError[] =		"System Error";
char szControl_File[] =				"File: ";
char szControl_HasChanged[] =		"  has changed.\nSave  ?";
char szControl_Warning[] =			"WARNING";
char szControl_CantOpen[] =			"Can't open the file:";
char szControl_Error[] =			"ERROR";
char szControl_CantOpenDDriver[] =	"Can't open device driver for Transputer";
char szControl_Notice[] =			"Notice";
char szControl_ActiveChnl[] =		"Active Channel is not a Wall Channel";
char szControl_CommErrorS[] =		"Communication Error:Can't Send";
char szControl_TputerStatus[]=		"Transputer Status: OK";
char szControl_Report[] =			"Report";
char szControl_CommErrorR[] =		"Communication Error:Can't receive";
char szControl_SavedFile[] =		"Saved file:  ";
char szControl_FileOper[] =			"FILE OPERATION";
char szControl_WriteOver[] =		"Write Over Existing File?";
char szControl_MMIStopped[] =		"MMI stopped at: ";
char szControl_LostPackets[]=		"Lost packets = ";
char szControl_HasChangedExit[] =	"  has changed.\nSave before Exit?";


// dlg_rece.001 module
char szRcv_Ch[] =					"ch=";
char szRcv_Rcvr[] =					" RCVR   ";
char szRcv_SureUndo[] =				"Sure you want to un-DO all channels and gates ?";
char szRcv_CantGetFilterCB[] =		"Can't get Dialog Item Filter_CB";
char szRcv_TCGSET[] =				" TCG SET ";
char szRcv_CantGetTriggerCB[] =		"Can't get Dialog Item Trigger_CB";
char szRcv_CantGetStepCB[] =		"Can't get Dialog Item Step_CB";
char szRcv_CantGetFnCB[] =			"Can't get Dialog Item Fn_CB";

// dlg_scop.001 module
char szScop1_ScopeCh[] =			"  OSCILLOSCOPE: Ch = %2d  ";

// instcmd.001 module
char szInst_ChAssign[] =			"UDP Chnl Assign message failed at channel ";
char szInst_AttempToWrite[] =		" attempting to write to UDP.  Last good write was ";
char szInst_GoodBye[] =				".  Good Bye!";
char szInst_UDPCommError[] =		"UDP Communication Error";
char szInst_AttempToRead[] =		" attempting to read from UDP.  Last good read was ";
char szInst_ChartAssign[] =			"UDP Chart Assignment message failed at channel ";
char szInst_SlopeAssign[] =			"UDP Wall Slope Assignment message failed on instrument ";
char szInst_OffsetAssign[] =		"UDP Wall Offset Assignment message failed on instrument ";
char szInst_FiringSeq[] =			"Firing Seq Init failed at channel ";
char szInst_FiringSeqError[] =		"Firing Seq Init Error";


// dlg_stri.001 module
char szStrip2_NoTimeMotion[] =		"Unable to write TimeMotion flag to UDP";
char szStrip2_NoTimeMotionAck[] =	"UDP failed to ack TimeMotion flag";
char szStrip2_Alarm[] =				" Alarm";
char szStrip2_CantCreatTimer[] =	"Unable to creat timer for strip chart";
char szStrip2_ImproperHeader[] =	"Improper header!  Can not replay file: ";
char szStrip2_ImproperHeader2[] =	"Improper header, will now create new file: ";
char szStrip2_StripChartDisplay[] =	"STRIP CHART DISPLAY";
char szStrip2_DataFile[] =			"...Data File = ";
char szStrip2_PrintJoint[] =		"Print Joint %d ?";
char szStrip2_NoChartCalDialog[] =	"Unable to display Chart Cal dialog";
char szStrip2_CantWrite8[] =		"Unable to write 8 byte UDP message";
char szStrip2_CantRead80[] =		"Unable to read 80 byte UDP message";
char szStrip2_CantCreateFile[] =	"Can't create the file:";
char szStrip2_NoOpenDataFile[] =	"No Open Data File";
char szStrip2_PlayBackError[] =		"Play Back Error";
char szStrip2_PlayBackEnded[] =		"Play back ended\ndue to \nEnd of File";
char szStrip2_PlayBackEnded2[] =	"Play back ended\ndue to \nOperator Quit";
char szStrip2_ChartExitError[] =	"Unable to read UDP message at chart exit";
char szStrip2_CantSendChart[] =		"Unable to send Chart Assign UDP message";
char szStrip2_LongID[] =			"LONG ID";
char szStrip2_LongOD[] =			"LONG OD";
char szStrip2_TranID[] =			"TRAN ID";
char szStrip2_TranOD[] =			"TRAN OD";
char szStrip2_Obq2ID[] =			"OBQ2 ID";
char szStrip2_Obq2OD[] =			"OBQ2 OD";
char szStrip2_Obq4ID[] =			"OBQ4 ID";
char szStrip2_Obq4OD[] =			"OBQ4 OD";
char szStrip2_CantSetNCTol[] =		"Unable to set NC Tolerance";
char szStrip2_CantVerifyNCTol[] =	"Unable to verify NC Tolerance message";
char szStrip2_NCTolerance[] =		"NC tolerance: %1d";
char szStrip2_PlaybackTime[] =		"Playback in Time Based Mode?";
char szStrip2_OldCfgFile[] =		"Old Cfg  = ";
char szStrip2_NewCfgFile[] =		"New Cfg  = ";
char szStrip2_CfgFileChange[] =		"Cfg File Changed";


// dchanio.c module
char szDchan_ReadLinkFailed[] =		"ReadLink failed after reading only %d characters out of %d";
char szDchan_LastGoodWrite[] =		" Last good write was ";
char szDchan_AbortProgram[] =		"  Abort Program ? ";
char szDchan_ReadLinkError[] =		"ReadLink Failed Error";
char szDchan_WriteLinkFailed[] =	"WriteLink failed after writing only %d characters out of %d";
char szDchan_WriteBuffer[] =		" write buffer = ";
char szDchan_WriteLinkError[] =	"WriteLink Failed Error";
char szDchan_NoUDPEcho[] =			"Unable to read 4 byte UDP echo";
char szDchan_CantMalloc[] =			"Could not malloc memory for CDP msg";
char szDchan_MallocError[] =		"Memory Allocation Error";
char szDchan_NoCdpConfigMsg[] =		"Failed to send config info to CDP";
char szDchan_Pen[] =				"Pen: ";

// dlg_bar.c module
char szBar_ISW[] =					"W  ";
char szBar_ISL[] =					"L  ";
char szBar_IST[] =					"T  ";
char szBar_ISQ2[] =					"Q2 ";
char szBar_ISQ4[] =					"Q4 ";
char szBar_ISN[] =					"N  ";
char szBar_ResetScalers[] =			"Reset Scalers";
char szBar_Amp[] =					"Amp";
char szBar_Gain[] =					"Gain";
char szBar_Depth[] =				"Depth";
char szBar_PkHldON[] =				"&PkHld  ON";
char szBar_PkHldOFF[] =				"&PkHld OFF";
char szBar_Stopped[] =				"Stopped";
char szBar_Running[] =				"Running";
char szBar_Restore[] =				"Restore";
char szBar_NotFlawCalMode[] =		"UDP failed to enter flaw cal mode";
char szBar_NoDepthScaleData[] =		"Failed to get flaw DEPTH SCALE data from UDP";
char szBar_CantSendAllFlawFlag[] =	"Failed to send ALL FLAWS FLAG to UDP";
char szBar_CantSendFlawDepthFlag[] ="Failed to send FLAW DEPTH FLAG to UDP";
char szBar_BeginCal[] =				"Begin Calibration: ";
char szBar_AMPGate[] =				"AMP gate %1d";
char szBar_DEPTHGate[] =			"DEPTH gate %1d";
char szBar_BeginBeforeEnd[] =		"You must begin before you can end";
char szBar_CalEnd[] =				"End Calibration... Receiver gains will be adjusted";
char szBar_CalSucceeded[] =			"Calibration succeeded.. view old and new gains?";
char szBar_CalFail[] =				"Calibration failed.. attempted gain > 60 or < -20 db";
char szBar_ReCal[] =				"Select either AMP or DEPTH and recalibrate";
char szBar_CalEndDepth[] =			"End Calibration... Flaw Depth scalers will be adjusted";
char szBar_AdjFail[] =				"Adjustment failed, Max/Min > 5\nUse Anyway ?";
char szBar_AdjSucceeded[] =			"Adjustment succeeded";
char szBar_FlawDepthFail[] =		"Failed to Download UDP\nFlaw Depth NOT Calibrated";
char szBar_MustBePkHld[] =			"You must be in Peak Hold Mode to Display Signal Levels";
char szBar_MSG5[] =					"MSG5";
char szBar_ResetScalersTo1[] =		"Reset ALL flaw scalers to 1.0";
char szBar_Preset[] =				"Preset";
char szBar_UDPSoftwareError[] =		"UDP software error";
char szBar_CantGetFlawCalData[] =	"UDP failed to get flaw cal data";
char szBar_CantGetDepthScaleData[] ="UDP failed to get flaw DEPTH SCALE data";
char szBar_CantExitCalMode[] =		"UDP failed to exit flaw cal mode";
char szBar_RestoreGateLevels[] =	"Restore gate LEVELS before exiting?";


// dlg_cal.c module
char szCal_ResetCoef[] =			"Reset Coef";
char szCal_CantGetWallCalInfo[] =	"Unable to read Wall_Cal_Info message";
char szCal_WallCalInfoStruct[] =	"WALL_CAL_INFO structure";
char szCal_NoWallCalTimer[] =		"Unable to creat timer for wall calibration";
char szCal_CantReadUdp[] =			"Unable to read from UDP ";


// dlg_gate.c module
char szGate_uSec[] =				"%-7.1f us";
char szGate_Inch[] =				"%-7.3f in";
char szGate_mm[] =					"%-7.2f mm";
char szGate_Gates[] =				"  GATES  g=";


// dlg_open.c module
char szOpen_ConfigFile[] =			"OPEN Config File";
char szOpen_DataFile[] =			"OPEN Data File";


// dlg_puls.c module
char szPuls_Inch[] =				"%-6.3f inch";
char szPuls_mm[] =					"%-6.2f mm";
char szPuls_Pulser[] =				"     PULSER     ";
char szPuls_Hz[] =					"%7d Hz";

// dlg_stat.c module
char szStat_NoUdpTimer[] =			"Unable to create UDP timer function";
char szStat_NoUlcTimer[] =			"Unable to create ULC timer function";
char szStat_NoStartupTimer[] =		"Unable to create startup Status timer function";
char szStat_ALL[] =					"ALL ";
char szStat_AllFlaws[] =			"All Flaws";
char szStat_FlawDepth[] =			"Flaw Depth";
char szStat_ReportEnglish[] =		"STATUS REPORT - ENGLISH";
char szStat_ReportMetric[] =		"STATUS REPORT - METRIC";
char szStat_WallRangeE[] =			"Wall range is [0.100, 2.39]";
char szStat_WallRangeM[] =			"Wall range is [2.54, 60.9]";
char szStat_InvalidWallInput[] =	"INVALID WALL INPUT";
char szStat_ODRangeE[] =			"Diameter range is [2.25, 16.0]";
char szStat_ODRangeM[] =			"Diameter range is [57.2, 406.4]";
char szStat_ChTypesDifferent[] =	"Channel types are different.  Copy anyway?";
char szStat_LostPackets[] =			"Lost Packets Inst%d";


// dlg_tof.c module
char szTof_Tof[] =					" TIME OF FLIGHT g=";


// dlg_trbl.c module
char szTrbl_NoEtherStats[] =		"Unable to read ETHER_STATS msg, column ";
char szTrbl_NoPacketData[] =		"Failed to get packet data";
char szTrbl_Chnl[] =				"Chnl ";
char szTrbl_NoPacketTimer[] =		"Unable to creat timer for Packet Data";


// prntscrn.c module
char szPrnscrn_PrintDIBScaled[] =	"Print DIB SCALED";
char szPrnscrn_PrintDIBFail[] =		"Print DIB Fail";
char szPrnscrn_PrintDIBBestFit[] =	"Print DIB Best Fit";





#endif

