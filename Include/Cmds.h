#pragma once
/*
jeh
Collect all commands into one module
creation date: 2017-02-16

COPY THIS HEADER FILE INTO E:\PhasedArrayII\Instrument2016_TFS\software\AdcApp FOR USE BY
THE INSTRUMENT (NIOS2) CODE

COPY THIS HEADER FILE INTO E:\PhasedArrayII\PhasedArray_II_TFS\Include FOR USE BY
the PAP and PAG

*/


// If used with NIOS2, DEFINE:    I_AM_THE_INSTURMENT
// When used with PAP  and PAG, undefine I_AM_THE_INSTURMENT
//#include <alt_types.h> //for alt_u32

#undef I_AM_THE_INSTURMENT

#ifdef I_AM_THE_INSTURMENT
#include "../GlobalHeaders/Global.h"
#include <stdio.h> //for printf
#include <string.h> //for memcpy
#include <unistd.h> //for usleep

#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "system.h"
//#include "../GlobalHeaders/Global.h"
#include "../GlobalHeaders/PA2Struct.h"

#include "process.h"

#include "wiznet5300_wc3s.h"
#include "Main.h"

extern ST_LARGE_CMD *pCmdGlobal;
#endif

#ifndef WORD
#define WORD unsigned short
#endif
#ifndef BYTE
#define BYTE unsigned char
#endif
#ifndef UINT
#define UINT unsigned int
#endif


/*****************	COMMANDS *********************/
#define TOTAL_COMMANDS				20
#define TOTAL_LARGE_COMMANDS		5

// SMALL
#define NULL_0						0
#define NULL_1						1
#define SET_GATE_DELAY_CMD_ID		2
#define SET_GATE_RANGE_CMD_ID       3
#define SET_GATE_BLANK_CMD_ID       4
#define SET_GATE_THRESHOLD_CMD_ID   5
#define SET_GATES_TRIGGER_CMD_ID	6    
#define SET_GATES_POLARITY_CMD_ID   7
#define SET_GATES_TOF_CMD_ID		8
#define SET_SEQ_GAIN_STEP_CMD_ID	9
#define SET_SEQ_GAIN_DELAY_CMD_ID	10
#define SET_CHNL_GAIN_STEP_CMD_ID	11
#define SET_CHNL_GAIN_DELAY_CMD_ID	12


// LARGE
#define NC_NX_TEST					1+0x200
#define SEQ_GAIN_CMD_ID				2+0X200
#define CHNL_GAIN_CMD_ID			3+0X200

/*************** Command Structures **************/

typedef struct
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD ... 22 bytes before Results
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream
	BYTE bPapNumber;	// which PAP is the command for
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
	}	GenericPacketHeader;	// 12 bytes


// Command format from User interface systems to the PAP
// Command packet can be cut short by specifying a byte count less than 1056
typedef struct 
	{
	// The generic header
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD ... 22 bytes before Results
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream
	BYTE bPapNumber;	// which PAP is the command for
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215

	// Commands can have any format past this point based on MsgId

	BYTE bSeqNumber;	// when relevant, which sequence of virtual probes the command affects
	BYTE bSpare[19];	// 32 bytes to here
    WORD wCmd[512];		// 512 words or 1024 bytes
	} ST_LARGE_CMD;		// 1056 bytes 

// 2016-12-12 ALLOW for variable size data and command packets. Data packet will always be the max
// allowed by TCPIP. Command packets may vary in size.


typedef struct
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD ... 22 bytes before Results
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream	WORD wMsgID;		// 1 = NC_NX_CMD_ID
	BYTE bPAPNumber;	// One PAP per transducer array. 0-n. Based on last digit of IP address.
						// PAP-0 = 192.168.10.40, PAP-1=...41, PAP-2=...42
	BYTE bBoardNumber;	// 0-255. 0 based ip address of instruments for each PAP
						// Flaw-0=192.168.10.200, Flaw-1=...201, Flaw-2=...202 AnlgPlsr=...206
						// Wall = ...210 DigPlsr=...212, gaps allow for more of each board type

	BYTE bSpare[4];		// 16
	WORD wCmd[8];		// 16	
	} ST_SMALL_CMD;		// sizeof() = 32

typedef struct
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD ... 22 bytes before Results
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream	WORD wMsgID;		// 1 = NC_NX_CMD_ID
	BYTE bPAPNumber;	// One PAP per transducer array. 0-n. Based on last digit of IP address.
						// PAP-0 = 192.168.10.40, PAP-1=...41, PAP-2=...42
	BYTE bBoardNumber;	// 0-255. 0 based ip address of instruments for each PAP
						// Flaw-0=192.168.10.200, Flaw-1=...201, Flaw-2=...202 AnlgPlsr=...206
						// Wall = ...210 DigPlsr=...212, gaps allow for more of each board type

	BYTE bSpare[20];	// sequence number at beginning of stPeakData Results[] // 32 bytes to here
	BYTE bMsg[1024];	// Max unique sets of Nc Nx data per instrument.
	} PAM_GENERIC_MSG; // SIZEOF() = 1056

/*************** Command Structures **************/

#if 1
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
	UINT uSync;			// 0x5CEBDAAD ... 22 bytes before Results
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream
	BYTE bPAPNumber;	// One PAP per transducer array. 0-n. Based on last digit of IP address.
						// PAP-0 = 192.168.10.40, PAP-1=...41, PAP-2=...42
	BYTE bBoardNumber;	// 0-255. 0 based ip address of instruments for each PAP
						// Flaw-0=192.168.10.200, Flaw-1=...201, Flaw-2=...202 AnlgPlsr=...206
						// Wall = ...210 DigPlsr=...212, gaps allow for more of each board type

	BYTE bSpare[4];		// 16
	ST_NC_NX stNcNx[52];		// 1040	
	} PAP_INST_CHNL_NCNX; // SIZEOF() = 1056 replaces CHANNEL_CMD_1
	

// The next several command formats are for Short Commands of 32 bytes
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
	WORD wThold;	// in 80 Mhz clocks
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

// the polarity word sets the values for all 4 gates, hence bGateNumber has no value
// this originally was called gate_data_mode_cmd
/*
The following gate command sets all 4 gates with one single command. 
This is done by using bit fields. Only the low byte of wPolarity is used. 
“Rf+” means radio frequency, a bipolar wave. “Rf-“ means an inverted bipolar wave. 
“Fw” means full wave, a rectified or unipolar wave.
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

// the TOF word sets the values for 3 gates, hence bGateNumber has no value
// this originally was called gate_TOF_mode_cmd
/*
The following gate command sets the time of flight characteristics for gates 1-3 with one command. 
This is done by using bit fields. Bits 7-6 select which one of the gates will be used 
for time of flight output. Gate 0 is not available. 


Bits 7-6         | Bit 5 start gate3, 0=Main bang, 1= Pk detect | Bit 2 stop gate3, 0=Thld detect, 1= Pk detect
11= gate3        | Bit 4 start gate2, 0=Main bang, 1= Pk detect | Bit 1 stop gate2, 0=Thld detect, 1= Pk detect
10= gate2        | Bit 3 start gate1, 0=Main bang, 1= Pk detect | Bit 0 stop gate1, 0=Thld detect, 1= Pk detect
01=gate1          
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
into the gain time step interval desired.
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
	WORD wStep;	//
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

//
//=================================================
// LARGE COMMNAD STRUCTURES

typedef struct
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD ... 22 bytes before Results
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream	WORD wMsgID;		// 1 = NC_NX_CMD_ID
	BYTE bPAPNumber;	// One PAP per transducer array. 0-n. Based on last digit of IP address.
						// PAP-0 = 192.168.10.40, PAP-1=...41, PAP-2=...42
	BYTE bBoardNumber;	// 0-255. 0 based ip address of instruments for each PAP
						// Flaw-0=192.168.10.200, Flaw-1=...201, Flaw-2=...202 AnlgPlsr=...206
						// Wall = ...210 DigPlsr=...212, gaps allow for more of each board type

	BYTE bSpare[4];		// 16
	ST_NC_NX stNcNx[52];		// 1040	
	} CANNED_GATES; // SIZEOF() = 1056 replaces CHANNEL_CMD_1


typedef struct 
	{
	// The generic header
	GenericPacketHeader Head;	// wMsgID= SET_GATES_TOF_CMD_ID, gph is 12 bytes
	BYTE bSeqNumber;	// when relevant, which sequence of virtual probes the command affects
	BYTE bSpare[19];	// 32 bytes to here
    WORD wGain[512];	// 512 words or 1024 bytes .. ony first 128 used. wGain[128] to wGain[511= = 0
	} ST_ELEMENT_GAIN;		// 1056 bytes 


typedef struct	// NOT SURE ABOUT THIS COMMAND 2017-03-16
	{
	// The generic header
	GenericPacketHeader Head;	// wMsgID= SET_GATES_TOF_CMD_ID, gph is 12 bytes
	BYTE bSeqNumber;	// when relevant, which sequence of virtual probes the command affects
	BYTE bChnl;
	BYTE bSpare[18];	// 32 bytes to here
    WORD wGain[512];	// 512 words or 1024 bytes .. ony first 128 used. wGain[128] to wGain[511= = 0
	} ST_CHNL_GAIN;		// 1056 bytes 


#endif

/*****************	COMMANDS  END *********************/

#ifdef I_AM_THE_INSTURMENT

void CmdExecutive( ST_LARGE_CMD *pCmd );	// This works because ST_SMALL_CMD is a subset of the large command

void ZeroCmdProcessed( void );
void ZeroLargeCmdProcessed( void );


void ProcNull( void );
void NcNx_Test_Cmd( void );
/*   GATE COMMANDS */
void GateDelay( void );
void GateRange( void );
void GateBlank( void );
void GateThreshold( void );
void GatesTrigger( void );
void GatesPolarity( void );
void GatesTOF( void );

void set_gate_delay( WORD GateNumber, WORD nDelay, WORD nSeq, WORD vChnl);
void set_gate_range( WORD GateNumber, WORD nRange, WORD nSeq, WORD vChnl);
void set_gate_blank( WORD GateNumber, WORD nBlank, WORD nSeq, WORD vChnl );
void set_gate_threshold(WORD GateNumber, WORD nThold, WORD nSeq, WORD vChnl );

//bit7,6,5,4: gate 4-1 trigger select (0:mbs, 1:threshold);  bit 3-0: gate enable
void set_gates_trigger(WORD nTrigger, WORD nSeq, WORD vChnl );

//bit10:mode1, 32:mode2, 54:mode3, 7-6:mode4 --mode: 00:+, 01:-, 1x: abs
//0x55-d85:-, 0x00:+, 0xaa-d170:abs
void set_gates_polarity( WORD nPolarity, WORD nSeq, WORD vChn3 );

//bit7,6: tof out sel; bit5,4,3: start of gate 4,3,2; bit2,1,0: stop of gate 4,3,2-start:0-mbs,1-PkDet;--stop:0-ThDet,1-PkDet
//tof2: b00_100_111-0x27(start:4Pk 3mbs 2mbs);
void set_gates_tof(WORD nTOF, WORD nSeq, WORD vChn3 );
/*   GATE COMMANDS */

/*   GAIN COMMANDS */
// Utilize Large Cmd structure
void SetElementGain( void );
void SeqTCGStepSize( void );
void TCGGainDelay( void );
void ChnlSeqGain( void );
void ChnlGainStep( void );
void ChnlGainDelay( void );

void set_seq_element_gain( int seq, unsigned short value[128] );
void set_TCG_step_size( int value );
void set_TCG_delay( int value );
void set_beam_gain( int beam, int seq, unsigned short value[128] );	// beam is a virtual channel
void set_beam_gain_step( int value );
void set_beam_gain_delay( int value );

/*   GAIN COMMANDS */

#endif



