#pragma once
/*
jeh
Collect all commands into one module
creation date: 2017-02-16

COPY THIS HEADER FILE INTO E:\PhasedArrayII\Instrument2016_TFS\software\AdcApp FOR USE BY
THE INSTRUMENT (NIOS2) CODE

COPY THIS HEADER FILE INTO E:\PhasedArrayII\PhasedArray_II_TFS\PA2Win\Include FOR USE BY
the PAP and PAG

*/


// If used with NIOS2, DEFINE I_AM_THE_INSTURMENT
// When used with PAP  and PAG, undefine I_AM_THE_INSTURMENT

#undef I_AM_THE_INSTURMENT

#ifdef I_AM_THE_INSTURMENT
//#include <alt_types.h> //for alt_u32

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


/*****************	COMMANDS *********************/
#define TOTAL_COMMANDS				20
#define TOTAL_LARGE_COMMANDS		5

// SMALL
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



