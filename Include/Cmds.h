#pragma once
/*
jeh
Collect all commands into one module
creation date: 2017-02-16

COPY THIS HEADER FILE INTO E:\PhasedArrayII\Instrument2016_TFS\software\AdcApp -- FOR USE BY
THE INSTRUMENT (NIOS2) CODE

COPY THIS HEADER FILE INTO E:\PhasedArrayII\PhasedArray_II_TFS\Include -- FOR USE BY
the PAP and PAG

*/


// If used with NIOS2, DEFINE:    I_AM_THE_INSTRUMENT
// When used with PAP  and PAG, undefine I_AM_THE_INSTRUMENT
//#include <alt_types.h> //for alt_u32

#undef I_AM_THE_INSTRUMENT

#ifdef I_AM_THE_INSTRUMENT
#include "../GlobalHeaders/Global.h"
#include <stdio.h> //for printf
#include <string.h> //for memcpy
#include <unistd.h> //for usleep

#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "priv/alt_legacy_irq.h"
#include "system.h"
//#include "../GlobalHeaders/Global.h"
#include "../GlobalHeaders/PA2Struct.h"

#include "../AdcApp/process.h"

#include "../AdcApp/wiznet5300_wc3s.h"
#include "../AdcApp/Main.h"

#endif /* I_AM_THE_INSTRUMENT */


/*****************	COMMANDS *********************/
#define TOTAL_COMMANDS				40		// small commands
#define LAST_SMALL_COMMAND			TOTAL_COMMANDS - 1




// SMALL
// modify Cmds.h and Cmds.cpp in the NIOS code files
// also add the command name to: void ( *ProcPtrArray[TOTAL_COMMANDS])() =
// found in the NIOS file Cmds.cpp. Add the 'defined' name to PopulateCmdComboBox()
// in PAG dialog NcNx.cpp
//
// Command names in the NcNx dialog are shown after the numerical value.
//
#define NULL_0						0		// ProcNull
#define FAKE_DATA_CMD_ID			1		// FakeData
#define SET_GATE_DELAY_CMD_ID		2		// GateDelay
#define SET_GATE_RANGE_CMD_ID       3		// GateRange
#define SET_GATE_BLANK_CMD_ID       4		// GateBlank
#define SET_GATE_THRESHOLD_CMD_ID   5		// GateThreshold
#define SET_GATES_TRIGGER_CMD_ID	6		// GatesTrigger 
#define SET_GATES_POLARITY_CMD_ID   7		// GatesPolarity
#define SET_GATES_TOF_CMD_ID		8		// GatesTOF
#define TCG_PROBE_TRIGGER_CMD_ID	9		// SetChnlTrigger
#define TCG_GAIN_CLOCK_CMD_ID		10		// TCGGainClock
#define TCG_BEAM_GAIN_DELAY_ID		11		// what is this in adc board?
#define BLAST_CMDS_ID				12		// Blast test commands.. Send 300 canned commands
#define DEBUG_PRINT_CMD_ID			29		// turn on/off debuggin in ADC debug console.. was 13
#define SET_TCG_CLOCK_RATE_CMD_ID	14		// SetTcgClockRate
#define TCG_TRIGGER_DELAY_CMD_ID	15		// TCGTriggerDelay
#define POW2_GAIN_CMD_ID			16		// Pow2GainBoost

//17-20 are Socomate commands
#define ASCAN_SCOPE_SAMPLE_RATE_ID	21		// AscanScopeSampleRate -- set_ascan_scope
#define SET_ASCAN_SCOPE_DELAY_ID	22		// SetAscanDelay -- set_ascan_delay

#define SET_ASCAN_PEAK_MODE_ID		23		// SelectAscanWaveForm -- set_ascan_peaksel
#define SET_ASCAN_RF_BEAM_ID		24		// SetAscanRfBeamSelect -- set_ascan_rf_beam_sel_reg

#define SET_ASCAN_BEAM_SEQ_ID		25		// SetAscanSeqBeamReg -- set_ascan_seq_beam_setup_reg
#define	SET_ASCAN_GATE_OUTPUT_ID	26		// SetAscanGateOut -- set_ascan_gateout_reg
#define ASCAN_REP_RATE_ID			27		// AscanRepRate
#define SET_WALL_NX_CMD_ID			28		// only runs on PAP, not in the Nios
#define DEBUG_PRINT_CMD_ID			29		// was TCGBeamGainAll  calls set_beam_gain_all with same gain for all 128 elements
											// Moved to 31. DebugPrint moves into 29
#define READ_BACK_CMD_ID			30
#define TCG_BEAM_GAIN_ALL_CMD_ID	31		// TCGBeamGainAll  calls set_beam_gain_all with same gain for all 128 elements
#define INIT_ADC_CMD_ID				32		// initialize gate settings .. small gate cmds.
#define GATE_BLAST_CMD_ID			33		// Set Gate Cmds to test Read Back packet from ADC
#define CMD204H_BLAST_CMD_ID		34		// Set TCG_BEAM_GAIN blast
#define CMD205H_BLAST_CMD_ID		35		// Set SEQ_TCG_GAIN blast
#define CMD_XLOC_SCALE_CMD_ID		36		// divide encoder pulses to get x scale resolution
											
//*******************************************


#define TOTAL_READ_BACK_COMMANDS	10
#define LAST_READ_BACK_COMMAND		TOTAL_READ_BACK_COMMANDS - 1

// Small command format
#define READBACK_CMD_ID				30		// ReadBackCmdData
// Within read back cmd, sub commands are:
// READ BACK CMDS
#define NX_READBACK_ID				0		// returns Nx settings for all channels
#define GET_GATE_DATA_ID			1		// returns all gate data for all channels of bSeq = n
#define GET_TCG_BEAM_GAIN_ID		2		// return setting for all beam gains cmd 0x204 
#define GET_TCG_SEQ_GAIN_ID			3		// return setting for all SEQ gains cmd 0x205 


//*******************************************

#define TOTAL_LARGE_COMMANDS		11
#define LAST_LARGE_COMMAND			TOTAL_LARGE_COMMANDS - 1

// LARGE
#define NC_NX_TEST					1+0x200
#define TCG_GAIN_CMD_ID				4+0x200		// TCGBeamGain
#define SEQ_TCG_GAIN_CMD_ID			5+0x200		// SetSeqTCGGain
//#define SET_ASCAN_BEAMFORM_DELAY_ID	4+0x200		// SetAscanBeamFormDelay


//*******************************************

#define TOTAL_PULSER_COMMANDS		10
#define LAST_PULSER_COMMAND			TOTAL_PULSER_COMMANDS - 1

// Small command format
// PULSER CMDS
#define PULSER_PRF_CMD_ID			0+0x300		// the peiod in 80 Mhz clocks, 5khz->16000 clocks
#define HV_ON_OFF_CMD_ID			1+0x300		// 1=ON, 0=OFF means no pulse
#define PULSE_POLARITY_CMD_ID		2+0x300		// 0=+, 1=-
#define PULSE_SHAPE_CMD_ID			3+0x300		// 0=OFF, 1=1 pulse, 255=1/2 pulse
#define PULSE_WIDTH_CMD_ID			4+0x300		// How wide is pulse in clock cycles at 80 Mhz
#define SEQUENCE_LEN_CMD_ID			5+0x300		// 3 for current system
#define SOCOMATE_SYNC_PULSE_CMD_ID	6+0x300		// Default is 4
#define PULSER_ON_OFF_CMD_ID		7+0x300
#define PULSER_DEBUG_PRINT_CMD_ID	8+0x300		// bit 0 enables printing in NIOS debug, bit 1 resets cmd counts
#define PULSER_WIZ_RESET_CMD_ID		9+0x300		// bit0=0, pulser board Wiznet ONLY init. bit 0 set, reset PULSER BRD also

/*************** Command Structures **************/

// Command ID's are still 16 bit. Data packet ID's are now 8 bit. 2/25/2020
typedef struct
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD 
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read	
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
	}	GenericPacketHeader;	// 12 bytes


// Command format from User interface systems to the PAP
// Command packet can be cut short by specifying a byte count less than 1088
typedef struct 
	{
	// The generic header
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD 
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215

	// Commands can have any format past this point based on MsgId

	BYTE bSeqNumber;	// when relevant, which sequence of virtual probes the command affects
	BYTE bSpare[19];	// 32 bytes to here
    WORD wCmd[512];		// 512 words or 1024 bytes
	} ST_LARGE_CMD;		// 1056 bytes 

typedef struct
	{
	// The generic header
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD 
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215

						// Commands can have any format past this point based on MsgId

	BYTE bSeqNumber;	// when relevant, which sequence of virtual probes the command affects
	BYTE bChnl;			// Chnl number in bSeqNumber above
	BYTE bSpare[18];	// 32 bytes to here
	WORD wDelay[16];
	WORD wCmd[496];		// 512 words or 1024 bytes
	} ST_BEAM_FORM_CMD;		// 1056 bytes 

// 2016-12-12 ALLOW for variable size data and command packets. Data packets will always be 1056
// bytes from NIOS hardware. Command packets may vary in size.


typedef struct
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD 
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream	WORD wMsgID;		// 1 = NC_NX_CMD_ID
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215
	
	BYTE bSpare[4];		// 16
	WORD wCmd[8];		// 16	
	} ST_SMALL_CMD;		// sizeof() = 32

typedef struct
{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD 
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream	WORD wMsgID;		// 1 = NC_NX_CMD_ID
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215

	BYTE bSpare[4];		// 16
	WORD wNx;			// length of averaging filter  0 < wNx < 9
	WORD wMax;			// 12.5 nSec counts
	WORD wMin;			// 12.5 nSec counts
	WORD wDropCount;	// number of consecutive bad wall reading before wall = 0
	WORD wCmd[4];		// 16	
} ST_NX_CMD;			// sizeof() = 32

typedef struct
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD 
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream	WORD wMsgID;		// 1 = NC_NX_CMD_ID
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215

	BYTE bSpare[20];	// sequence number at beginning of stPeakChnlPAP PeakChnl[] // 32 bytes to here
	BYTE bMsg[1024];	// Max unique sets of Nc Nx data per instrument.
	} PAP_GENERIC_MSG; // SIZEOF() = 1056

/*************** Command Structures **************/


// bSeq and bSeqModulo work together. bSeq on subsequent fake data is derived for existing
// data in NIOS memory if bSeq > 15 and bSeqModulo <= 16
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= FAKE_DATA_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// starting sequence number for this data packet. Range is 0-15
	BYTE bChnl;		// which virtual probe - range is 0-128
	BYTE bGateNumber;	// we have room here to set all 4 gates with one command but will not for now.
	BYTE bSeqModulo;	// when does the seq counter wrap (4,8,16??)
	WORD wFake[8];	// dummy argument. Msg ID is all that is required to service this command
	} ST_FAKE_DATA_CMD;	// sizeof() = 32


/*
The user interface program will have to convert the user�s specified time units 
(likely in 0.1 microsecond units) to approximately equivalent 80 MHz clock counts. 
This command has 14 bytes of unused data which might be of use in the future.
*/
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATE_DELAY_CMD_ID, gph is 12 bytes
	BYTE bSeq;			// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// we have room here to set all 4 gates with one command but will not for now.
	BYTE bSpare;	// 16 bytes to here
	WORD wDelay;	// in 80 Mhz clocks
	WORD wFill[7];	// all 0
	} ST_GATE_DELAY_CMD;	// sizeof() = 32

typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATE_RANGE_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// we have room here to set all 4 gates with one command but will not for now.
	BYTE bSpare;	// 16 bytes to here
	WORD wRange;	// in 80 Mhz clocks
	WORD wFill[7];	// all 0
	}	ST_GATE_RANGE_CMD;

typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATE_BLANK_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// we have room here to set all 4 gates with one command but will not for now.
	BYTE bSpare;	// 16 bytes to here
	WORD wBlank;	// in 80 Mhz clocks
	WORD wFill[7];	// all 0
	}	ST_GATE_BLANK_CMD;

typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATE_THRESHOLD_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// we have room here to set all 4 gates with one command but will not for now.
	BYTE bSpare;	// 16 bytes to here
	WORD wThold;	// in ??
	WORD wFill[7];	// all 0
	}	ST_GATE_THRESHOLD_CMD;


// the trigger word sets the values for all 4 gates, hence bGateNumber has no value
// this originally was called gate_control_cmd
/*
The high nibble of wTrigger sets the trigger agent for each gate. 
A bit value of 1 means trigger on threshold, 0 means trigger on main bang strobe. 
Bit7=gate3, bit6=gate2, bit5=gate1, bit4= gate0.

The low nibble of wTrigger is an enable/disable switch for each gate. 
A bit value of 1 means enabled, 0 means disabled.
Bit3=gate3, bit2=gate2, bit1=gate1, bit0= gate0.

*/
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATES_TRIGGER_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// used here only as a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wTrigger;	//
	WORD wFill[7];	// all 0
	}	ST_GATES_TRIGGER_CMD;

// The polarity word sets the values for all 4 gates, hence bGateNumber has no value.
// This originally was called gate_data_mode_cmd
/*
The following gate command sets all 4 gates with one single command. 
This is done by using bit fields. Only the low byte of wPolarity is used. 
�Rf+� means radio frequency, a bipolar wave. �Rf-� means an inverted bipolar wave. 
�Fw� means full wave, a rectified or unipolar wave.
Bits 7-6 for gate3 	   | 5-4 for gate2	       | 3-2 for gate1	        | 1-0 for gate0
00=rf+, 01=rf-, 1x=fw  | 00=rf+, 01=rf-, 1x=fw | 00=rf+, 01=rf-, 1x=fw	| 00=rf+, 01=rf-, 1x=fw

*/
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATES_POLARITY_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// used here only as a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wPolarity;	//
	WORD wFill[7];	// all 0
	}	ST_GATES_POLARITY_CMD;

// The TOF word sets the values for 3 gates, hence bGateNumber has no value.
// This originally was called gate_TOF_mode_cmd.
/*
The following gate command sets the time of flight characteristics for gates 1-3 with one command. 
This is done by using bit fields. Bits 7-6 select which one of the gates will be used 
for time of flight output. Gate 0 is not available. 


Bits 7-6 
11= gate3        | Bit 5 start gate3, 0=Main bang, 1= Pk detect | Bit 2 stop gate3, 0=Thld detect, 1= Pk detect
10= gate2        | Bit 4 start gate2, 0=Main bang, 1= Pk detect | Bit 1 stop gate2, 0=Thld detect, 1= Pk detect
01= gate1        | Bit 3 start gate1, 0=Main bang, 1= Pk detect | Bit 0 stop gate1, 0=Thld detect, 1= Pk detect          
00 not allowed     

*/
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATES_TOF_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// used here only as a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wTOF;		// only the low byte is used
	WORD wFill[7];	// all 0
	}	ST_GATES_TOF_CMD;


/*
TCG runs at 80 Mhz clock - too fast for TCG steps. This command divides down the TCG clock from 80 Mhz
into the gain time-step interval desired.
*/
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= 
	BYTE bSeq;		// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// used here only as a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wStep;		//
	WORD wFill[7];	// all 0
	}	ST_SET_TCG_STEP_SIZE_CMD;

/*
After the TCG trigger we can have a delay before TCG starts to run
This command sets the delay time in 80 Mhz clock counts.
*/
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= 
	BYTE bSeq;		// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// used here only as a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wDelay;	//
	WORD wFill[7];	// all 0
	}	ST_SET_TCG_DELAY_CMD;

typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= 
	BYTE bSeq;		// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// used here only as a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wDelay;	//
	WORD wFill[7];	// all 0
	}	ST_TCG_GAIN_DELAY_CMD;

typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATES_TOF_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// 
	BYTE bChnl;		// which virtual probe
	BYTE bGateNumber;	// used here only as a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wDelay;	//
	WORD wFill[7];	// all 0
	}	ST_CHNL_GAIN_DELAY_CMD;

typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATES_TOF_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// used here only as a place holder
	BYTE bChnl;		// which virtual probe.. used here only as a place holder
	BYTE bGateNumber;	// used here only as a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wPrf;	//
	WORD wFill[7];	// all 0
	}	ST_PRF_CMD;

// A generic command using a WORD argument
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATES_TOF_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// used here only as a place holder
	BYTE bChnl;		// which virtual probe.. used here only as a place holder
	BYTE bGateNumber;	// normally a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wCmd;	//
	WORD wFill[7];	// all 0
	}	ST_WORD_CMD;

// How to select sequence and channel and gate being viewed??
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= NIOS_SCOPE_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// used here only as a place holder
	BYTE bChnl;		// which virtual probe.. used here only as a place holder
	BYTE bGateNumber;	// used here only as a place holder to conform to the command format
	BYTE bAscanPolarity;// 16 bytes to here 0=rf+, 1=rf-,2=
	WORD wTraceAssign;	// bit field.1=Ascan,2=gate0,4=gate1,8=gate2,16=gate3,32=TOF gate...
	WORD wStartDelay;	// in system clocks..12.5 ns increments
	WORD wStepSize;		// sys clocks between A/D samples.. typically < 16
						// Range is redundant since there are 1024 samples at whatever the step size is
	WORD wPacketRate;	// how often to send TCP/IP packet to PAP in ms. 
						// wPacketRate == 0->1 ASCAN packet in data to PAP every 5 seconds
	WORD wFill[4];		// all 0
	}	ST_SET_SCOPE_CMD;

typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= SET_GATES_TOF_CMD_ID, gph is 12 bytes
	BYTE bSeq;		// used here only as a place holder
	BYTE bChnl;		// which virtual probe.. used here only as a place holder
	BYTE bGateNumber;	// normally a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wPeriod;	// how often to send TCP/IP packet to PAP in ms.
					// wPacketRate == 0-> 1 ASCAN packet in data to PAP every 5 seconds
	WORD wFill[7];	// all 0
	}	ST_ASCAN_PERIOD_CMD;


typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= 
	BYTE bSeq;		// used here only as a place holder
	BYTE bChnl;		// which virtual probe.. used here only as a place holder
	BYTE bGateNumber;	// normally a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wBeamType;	//  0=rf 1=fw  2= rf peak hold 3=fw peak hold
	WORD wMultiSeq;	// bit encoded sequence selection- possibly multiple sequences
	WORD wFill[6];	// all 0
	}	ST_BEAM_TYPE_CMD;

// ID =1 Read back all gates commands
// ID =2 Read back TCG Beam Gain 
// ID =3 Read back TCG Seq Gain settings 128 Seq 0
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= READBACK_CMD_ID = 30
	BYTE bSeq;			// 
	BYTE bChnl;			// which virtual probe
	BYTE bGateNumber;	
	BYTE bSeqEnd;		// not currently used
	WORD wReadBackID;	// =1 reads back all gate setting for one sequence
	WORD wFill[7];		// all 0
	} ST_READ_BACK_CMD;	// sizeof() = 32

// Used to turn on debugging output from Eclipes/NIOS debug output
typedef struct
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD 
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream	WORD wMsgID;		// 1 = NC_NX_CMD_ID
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215

	BYTE bSpare[4];		// 16
	// To make my PAG interface work easier, DbgFlag is a bit field globals � printf() operations
	WORD wDbgFlag;		// bit 0=0 no dbg output sets gbDebugMode in ADC globals
						// bit 1=1 reset command counters in ADC and PULSER
						// bit 2=1 reset max FIFO high water mark in NIOS command FIFO's
	WORD wCmd[7];		// 16	
	} ST_DEBUG_CMD;		// sizeof() = 32

typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= 36
	BYTE bSeq;		// used here only as a place holder
	BYTE bChnl;		// which virtual probe.. used here only as a place holder
	BYTE bGateNumber;	// normally a place holder to conform to the command format
	BYTE bSpare;	// 16 bytes to here
	WORD wScaleFactor;	//  number of encoder pulses to count before incrementing x loc
	WORD wFill[7];	// all 0
	}	ST_X_LOC_SCALE_CMD;

//




//=================================================
// LARGE COMMNAD STRUCTURES

// When received as a command, bSeqNumber < 0 means end of command
typedef struct
	{
	BYTE bSeqNumber;	// Sequence number of this channel
	BYTE bChnlNumber;	// channel number in the above sequence for this channel
	// Flaw Nc for ID
	BYTE bNcID;		// how many flaw values required at or above threshold
	BYTE bTholdID;	// Threshold value 0-127
	BYTE bModID;	// active depth of FIFO. This is the 'm' in Nc out of m. eg., 2 out of 3 above thold
					// bModID must be >= bNcID. If not the PAM will make bModID=bNcID
	// Nc for OD
	BYTE bNcOD;		// how many flaw values required at or above threshold
	BYTE bTholdOD;	// Threshold value 0-127
	BYTE bModOD;	// active depth of FIFO. This is the 'm' in Nc out of m. eg., 2 out of 3 above thold
	// Interface gate
	BYTE bNcIf;		// how many flaw values required at or above threshold
	BYTE bTholdIf;	// Threshold value 0-127
	BYTE bModIf;	// active depth of FIFO. This is the 'm' in Nc out of m. eg., 2 out of 3 above thold
	BYTE bSpare;
	// Wall Nx portion
	WORD wNx;		// Number of wall readings to average. Typically not more than 8
	WORD wWallMax;	// maximum allowed hardware wall reading. 2.000" -> 1377 typically
	WORD wWallMin;	// minimum allowed hardware wall reading. 0.040" -> 27
	WORD wDropOut;	// number of bad readings before drop out occurs
	} ST_NC_NX;		// sizeof(ST_NC_NX) = 20

// Since the Receiver/PAM used for this initial systems knows nothing about the type signals it processes,
// this command sets the Nc and Nx variables for all virtual channels.
// In this original system there are up to 32 channels per instrument but only a few channel types
// with the types being repeated. It is the same channel type but at a different physical location, 
// much like the fixed channel types in Truscope where every channel type (for example Long) is replicated
// in each of the 4 shoes.
//

// Assume 64 chnl types per sequence per instrument.
// Doubtful that we will have more than 64 channels in a sequence.
// The easy way to do this is to send messages which terminate at the end
// of a sequence in which case the start channel number is always 0
typedef struct
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Make even number
	UINT uSync;			// 0x5CEBDAAD 
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream	WORD wMsgID;		// 1 = NC_NX_CMD_ID
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215

	BYTE bSpare[4];		// 16
	ST_NC_NX stNcNx[52];		// 20*52 = 1040	
	} PAP_INST_CHNL_NCNX; // SIZEOF() = 1056 replaces CHANNEL_CMD_1
	
typedef struct
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD 
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream	WORD wMsgID;		// 1 = NC_NX_CMD_ID
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215

	BYTE bSpare[4];		// 16
	ST_NC_NX stNcNx[52];		// 1040	
	} CANNED_GATES; // SIZEOF() = 1056 replaces CHANNEL_CMD_1


typedef struct 
	{
	// The generic header
	GenericPacketHeader Head;	// wMsgID= SET_GATES_TOF_CMD_ID, gph is 12 bytes
	BYTE bSeqNumber;	// when relevant, which sequence of virtual probes the command affects
	BYTE bSpare[19];	// 32 bytes to here
    WORD wGain[512];	// 512 words or 1024 bytes .. only first 128 used. wGain[128] to wGain[511]= = 0
	} ST_SEQ_TCG_GAIN;		// 1056 bytes 


typedef struct	// NOT SURE ABOUT THIS COMMAND 2017-03-16
	{
	// The generic header
	GenericPacketHeader Head;	// wMsgID= SET_GATES_TOF_CMD_ID, gph is 12 bytes
	BYTE bSeqNumber;	// when relevant, which sequence of virtual probes the command affects
	BYTE bChnl;
	BYTE bSpare[16];	// 32 bytes to here
	WORD wStartAddr;	// initial hw offset address
    WORD wGain[512];	// 512 words or 1024 bytes .. ony first 128 used. wGain[128] to wGain[511]= = 0
	} ST_TCG_BEAM_GAIN;		// 1056 bytes 



// Since the Receiver/PAM used for this initial systems knows nothing about the type signals it processes,
// this command sets the Nc and Nx variables for all virtual channels.
// In this original system there are up to 32 channels per instrument but only a few channel types
// with the types being repeated. It is the same channel type but at a different physical location, 
// much like the fixed channel types in Truscope where every channel type (for example Long) is replicated
// in each of the 4 shoes.
//

#if 0
typedef struct
	{
	GenericPacketHeader Head;	// wMsgID= 
	} ST_GATE_READBACK;
#endif


/*************** Command Structures **************/

/*****************	COMMANDS  END *********************/

#ifdef I_AM_THE_INSTRUMENT

void CmdExecutive( ST_LARGE_CMD *pCmd );	// This works because ST_SMALL_CMD is a subset of the large command

void ZeroCmdProcessed( void );
void ZeroLargeCmdProcessed( void );


void ProcNull( void );
void NcNx_Test_Cmd( void );

void FakeData( void );
void MakeFakeAscanData(void);	// only in PAP
void FakeAscanData(void);		// only in PAP
/*   GATE COMMANDS */
void GateDelay( void );
void GateRange( void );
void GateBlank( void );
void GateThreshold( void );		// sometimes called level
void GatesTrigger( void );
void GatesPolarity( void );
void GatesTOF( void );

void set_gate_delay( WORD nDelay, WORD nSeq, WORD vChnl, WORD bGate );
void set_gate_range( WORD nRange, WORD nSeq, WORD vChnl, WORD bGate );
void set_gate_blank( WORD nBlank, WORD nSeq, WORD vChnl, WORD bGate );
void set_gate_threshold(WORD nThold, WORD nSeq, WORD vChnl, WORD bGate);

//bit7,6,5,4: gate 4-1 trigger select (0:mbs, 1:threshold);  bit 3-0: gate enable
void set_gates_trigger(WORD nTrigger, WORD nSeq, WORD vChnl, BYTE bGate);	// Sams void set_gate_control

//bit10:mode1, 32:mode2, 54:mode3, 7-6:mode4 --mode: 00:+, 01:-, 1x: abs
//0x55-d85:-, 0x00:+, 0xaa-d170:abs
void set_gates_polarity( WORD nPolarity, WORD nSeq, WORD vChnl, BYTE bGate);

//bit7,6: not used; bit5,4,3: start of gate 4,3,2; 
//bit2,1,0: stop of gate 4,3,2-start:0-mbs,1-PkDet;--stop:0-ThDet,1-PkDet
//tof2: b00_100_111-0x27(start:4Pk 3mbs 2mbs);
void set_gates_tof(WORD nTOF, WORD nSeq, WORD vChnl, BYTE bGate );
/*   GATE COMMANDS */

/*   GAIN COMMANDS */
// Utilize Large Cmd structure
void SetSeqTCGGain( void );		// void set_TCG_gain
void SetTcgClockRate( void );		// set_TCG_step_size
void TCGTriggerDelay( void );		// set_TCG_delay
void TCGBeamGain( void );			// set_beam_gain  0x204
void TCGGainClock( void );			// set_beam_gain_step
void TCGBeamGainDelay(void);		// set_beam_gain_delay
void TCGBeamGainAll(void);			// set_beam_gain_all
// Same beam/chnl for all sequences
void SetChnlTrigger(void);			//set_beam_gain_t_mode, set_beam_gain_t_select
void SetPrf( void );
void set_ascan_scope(short value);	// ascan sample period
void AscanScopeSampleRate( void );	// set_ascan_scope also add in NcNx.cpp  -- 21

void set_ascan_delay(short value);
void SetAscanDelay(void);			// set_ascan_delay before trace begins --22

void set_ascan_peaksel(short nBeamType); // --23
void SelectAscanWaveForm(void);	// multiplexer, selects waveform to show, either Ascan or gates

void set_ascan_rf_beam_sel_reg(short beam);
void SetAscanRfBeamSelect(void);	// --24

void set_ascan_seq_beam_setup_reg(BYTE bChnl, WORD wMultiSeq); // --18
void SetAscanSeqBeamReg(void);	// selects the beam and seq to display

void set_ascan_gateout_reg(short value);	// --19
void SetAscanGateOut(void);

void SetAscanRfBeam( void );		// executes multiple primitives
void SelectAscanGateOutputs(void);	// executes multiple primitives

void MakeScopeCmds(void);		// executes multiple primitives

void ReadBackCmdData(void);
void set_encoder_inch_number(BYTE bLocScale);
void X_LocScale(void);
void AscanRepRate(void);

void set_TCG_step_size( int value );
void set_TCG_delay( int value );
void set_beam_gain_step( int value );
void set_beam_gain_delay( int value );

void set_sel_bit_cut_reg(int value); //default cut 0-4
void Pow2GainBoost(void);
void DebugPrint(void);

void set_PRF( WORD wPrf );	// Set prf in Hertz. Range 10-10,000 -- on pulser board
/*	SMALL TCG commands	*/


/*   GAIN COMMANDS */
// Utilize Large Cmd structure
void set_TCG_gain( int seq, unsigned short value[128] );  // same as sam's set_gain

void set_beam_gain( int beam, int seq, unsigned short value[128] );	// beam is a virtual channel
/*   GAIN COMMANDS */

void set_beam_seq_delay_register( int beam, int seq, /*int delay, */ short value[16] );
void SetAscanBeamFormDelay( void );

BYTE GetbDin();
short GetXLoc();
WORD GetAngle();
WORD GetPeriod();
BYTE GetFPGATemp();
BYTE GetBoardTemp();
WORD GetSpinCount();
WORD GetScopeSetting();
WORD GetVersionHw();

void GetAscanGateN(WORD& GW1, WORD& GW2, WORD nGate);

BYTE GetBeamType();
void SetBeamType(BYTE bBeam);

BYTE GetChCmd24();
void SetChCmd24(WORD wChs);

BYTE GetChCmd25();
void SetChCmd25(BYTE bChs);

WORD GetSeqsCmd25();
void SetSeqsCmd25(WORD wSeqs);

// cmd 26
BYTE GetScopeGates();
void SetScopeGates(BYTE bGates);

int RangeChecksWordCmd(ST_WORD_CMD *pW);	// CheckS plural

void MsgPrint(char *msg);
void BuildReadBackHeader(READBACK_DATA *pHdr);
void GetGateSettings(void);
void GetTCGBeamGain(void);
void GetTCGSeqGain(void);

// 2018-11-05 reset wiznet from PAP or UUI
void ResetWiznetCmd(void);

#endif



