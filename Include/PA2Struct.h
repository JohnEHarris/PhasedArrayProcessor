/*
Define structures and definitions for PhasedArray2 project
jeh
2016-05-17

Naming convention:
This project began in 2013. Originally the server to the instruments was called PAM - Phased Array Master
in 2016 it was redesignated as PAP - Phased Array Processor.  PAP = PAM
In this scheme, anything that communicates with the PAP via an Ethernet connection as a client is called an Instrument.
This includes pulser boards and phased array processor boards built by Tuboscope.

*/

#ifndef PA2_STRUCTS_H
#define PA2_STRUCTS_H

#ifndef BYTE
typedef unsigned char	BYTE;
#endif
#ifndef WORD
typedef unsigned short	WORD;
#endif

#ifndef UINT
typedef unsigned int	UINT;
#endif

enum IdataTypes {eRawInsp=10, eAscan=12, eKeepAlive=0xff};
enum IdOdTypes {eId, eOd, eIf};

#define MAX_PAM_QTY			1
#define MAX_PAM_INSTS_QTY	8
#define NC_NX_CMD_ID		1+0x200
#define ASCANS_TO_AVG		16

#define THE_APP_CLASS	CPA2WinApp

// Idata message types. This is the data that comes from the PAP and is sent 
// To the Receiver system
#define NC_NX_IDATA_ID				1		// PAP processed inspection data sent to PAG/Receiver system





// edit this value if more client connections to servers are needed
#define	MAX_SERVERS							1	//do it in ServerConnection Management for which ever type of server we are using
// Likely will have at least 2 server types. 1 for inspetion data and 1 for pulsers
// Mixing pulsers in with gate boards will make it more difficult to put dimensions on things like virtual channels. 2016-10-19

#define MAX_CLIENTS_PER_SERVER				8

// For the Client Communication System
#define MAX_CLIENTS							8


// An instrument client can have up to this many virtual channels for each UT firing or Main Bang
// Each MAIN BANG is a "sequence" until the sequence number repeats
// 64 until 11-08-16 then 32
#define MAX_CHNLS_PER_MAIN_BANG			32

// Channels may be redefined on each main bang. The counter which counts main bangs is called the sequence counter
// the maximum value the sequence counter can have is =
#define MAX_SEQ_COUNT					16
// choose 5 for seq length while doing simulated input data so that one packet has all the virtual channels in it

// The number of virtual channels is finite. Channels repeat after MAX_SEQ_COUNT number of main bangs.
// On any given main bang (sequence count) there can only be a max number of channels define by 
// MAX_CHNLS_PER_MAIN_BANG. The max the number of channels in a transducer array is 
// [MAX_SEQ_COUNT][MAX_CHNLS_PER_MAIN_BANG] = 16*32 = 512

// How many peak held data Results can we get in the max size tcpip packet.
// stPeakData Results[179]
#define MAX_RESULTS						179

#define INSTRUMENT_PACKET_SIZE			1056		//old 1040.. 1460 is max TCPIP size
#define MASTER_PACKET_SIZE				1260
#define SYNC							0x5CEBDAAD
#define CMD_FIFO_MEM_SIZE				0x4000		// must be greater than 0x1800
#define NC_NX_STRUCT_SIZE				52
#define MAX_CMD_PACKET_SIZE				1056

#define MAX_FQDN_LENGTH						 64	// max length of a fully qualified domain name
#define LOCAL_HOST						_T("localhost")

/*****************	STRUCTURES	*********************/
// A channel is a UT echo or reflection assigned a physical position in the transducer.
// Since this is a multiplexed system, the same channel repeats eventually.
// The number of A-Scans that occur before the repeat is the SeqModulo.
// This number of A-Scans is a data frame.
// Raw data begins with the first channel of the frame. This channel can be of type NONE
// For this particular implementation it is assumed that there are up to 32 A-Scans in a data frame.
// bSeqModulo would be 32 in this case.

// REVISED 2016-10-20

#if 0
// keep old versions for a while to use with Yiqing's simulator.
typedef struct 
	{
    BYTE bMsgID;		// = eIdataTypes
	BYTE bChannelTypes;	// eg. Long, Wall, Tran, Oblq. All long type have same focal law. All tran types have same focal law, etc
    BYTE bChannelRepeats;	//how many times a channel type repeats during a frame before coming to the 1st virtual channel again
	BYTE bFramesInDataPacket;	// bFramesInDataPacket*bChannelRepeats*bChannelTypes <= 128
    BYTE bDin;			//digital input, Bit1=Direction, Bit2=Inspection Enable, Bit4=Away(1)/Toward(0)
    BYTE bSpare[3];
    WORD wMsgSeqCnt;
    WORD wLocation;		//x location in motion pulses
    WORD wClock;		//unit in .2048ms
    WORD wPeriod;		//unit in .2048ms
	} SDataHeadOld;		//16 bytes


typedef struct 
	{
    SDataHeadOld DataHead;		// 16 bytes
    stRawData RawData[128];	// raw data of 128 consecutive pulses 128*8=1024
	} SRawDataPacketOld;		//1040 bytes

#endif

// Data coming from the Tuboscope electronics, that is the phased array boards.
#ifdef CLIVE_STYLE
typedef struct 
	{
	BYTE bStatus;	// tbd
	BYTE bAmp1;		// interface gate
    BYTE bAmp2;		//Gate 2 amplitude (0-255)
    BYTE bAmp3;		//Gate 3 amplitude (0-255)
    WORD wTof;     //time of flight of Gate 4
	} stRawData;		//6 bytes

#else
// Sam Style

typedef struct 
	{
//	BYTE bStatus;	// tbd
//	BYTE bAmp1;		// interface gate
    WORD wTof;     //time of flight of Gate 4
    BYTE bAmp3;		//Gate 3 amplitude (0-255)
    BYTE bAmp2;		//Gate 2 amplitude (0-255)
	} stRawData;		//4 bytes
#endif
// NEW stDataHead  .. compare to old
typedef struct 
	{
	BYTE bSeqNumber;	// sequence number of the first element in stRawData
	BYTE bChnlNumber;	// channel number of  the first element in stRawData
	} stDataHead;		// 2 bytes


// The data for one sequence from one inspection array
typedef struct
	{
	stDataHead DataHead;	// 2 bytes
	stRawData RawData[MAX_CHNLS_PER_MAIN_BANG];	// sizeof = 32*6 = 192: Clive, 128: Sam
	} stRawSeqPacket;	// sizeof = 194 Clive, 130 Sam

// Match Sam's data header .. 32 bytes long

typedef struct 
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD
    WORD wMsgSeqCnt;

    BYTE bDin;			//digital input, Bit1=Direction, Bit2=Inspection Enable, Bit4=Away(1)/Toward(0)
	BYTE bCmdQDepth;	// How deep is the command queue in the instrument NIOS processor
    WORD wLocation;		//x location in motion pulses
    WORD wClock;		//unit in .2048ms - ticks from TOP OF PIPE
    WORD wPeriod;		//unit in .2048ms
	WORD wRotationCnt;	// Number of rotations since pipe present signal
	BYTE bSpare[12];	// 32 bytes to here
	} InputRawDataPacketHeader;		//sizeof = 194*7 + 102 = 1460 bytes

// Raw data packet is built by the instrument over 7 main bang periods
// at 7k prf this is 1 packet every millisecond or 1k packets per second.
// This is the packet which is sent to the PAP
//
// Match Sams header for now
typedef struct 
	{
	WORD wMsgID;		// commands are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD
    WORD wMsgSeqCnt;

    BYTE bDin;			//digital input, Bit1=Direction, Bit2=Inspection Enable, Bit4=Away(1)/Toward(0)
	BYTE bCmdQDepth;	// How deep is the command queue in the instrument NIOS processor
    WORD wLocation;		//x location in motion pulses
    WORD wClock;		//unit in .2048ms - ticks from TOP OF PIPE
    WORD wPeriod;		//unit in .2048ms
	WORD wRotationCnt;	// Number of rotations since pipe present signal

						// NIOS has limited memory. Q likely to be [8][1460] = 11,680 bytes
	BYTE bSpare[12];	// 32 bytes to here
						// 194 per packet for clive, 130 per packet for sam
	stRawSeqPacket stSeqPkt[7];	// Raw data for  sequence points = 32 + 7*130 = 942 sam
	} InputRawDataPacket;		//sizeof = 194*5 + 32 = 1002 bytes clive

#define SET_DROPOUT		 ( 1 << 5)
#define CLR_DROPOUT		~( 1 << 5)
#define SET_OVERRUN		 ( 1 << 6)
#define CLR_OVERRUN		~( 1 << 6)
#define DATA_READY		 ( 1 << 7)		// 16 Ascan peak held and copied to local PeakData Structure
#define CLR_DATA_READY	~( 1 >> 7)
#define SET_READ		 ( 1 << 4)		// PAP sets when read. If vChannel resets fifo's with this
#define CLR_READ		~( 1 << 4)		// not set, it is overrun condition
#define DEFAULT_CFG		 ( 1 << 8)		// Nc Nx have default constructor values
#define STATUS_CLEAR_MASK	~SET_OVERRUN

// peak held data collected over 16 AScans for a single virtual channel and held in PeakData structure
// Processed data sent from the PAP. One stPeakData structure for every virtual channel
typedef struct
	{
	BYTE bStatus;	// bits 0..3 bad wall reading count, bit 5 wall dropout, bit 6 data over-run. 
					// ie, PAP did not service PeakData fast enough
	BYTE bG1;		// Gate 1 interface gate
	BYTE bId2;		// Gate 2 peak held data 0-255
	BYTE bOd3;		// Gate 3 peak held data 0-255
	WORD wTofMin;	// gate 4 min
	WORD wTofMax;	// gate 4 max
	} stPeakData;	// sizeof = 8
/*
2016-09-08 New definition of Idata Packet
*/
#if 1
// Based on processing rate of PAP,	 may have to have one PAP for each instrument.
// Idata is the peak held data from the Instrument boards and sent after 16 ascans
// Idata is the input to the down stream system
typedef struct
	{
	WORD wMsgID;		// commands and data are identified by their ID
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream 0-0xffff	
	BYTE bPAPNumber;	// One PAP per transducer array. 0-n. Based on last digit of IP address.
						// PAP-0 = 192.168.10.40, PAP-1=...41, PAP-2=...42
	BYTE bBoardNumber;	// 0-255. 0 based ip address of instruments for each PAP
						// Flaw-0=192.168.10.200, Flaw-1=...201, Flaw-2=...202 AnlogPlsr=...206
						// Wall = ...210 DigPlsr=...212, gaps allow for more of each board type
	BYTE bStartSeqNumber;	// sequence number at beginning of stPeakData Results[]
	BYTE bSequenceLength;	// how many main bangs before repeating the virtual channels
	BYTE bStartChannel;		// First virtual channel in peak data Results
	BYTE bMaxVChnlsPerSequence;	// maximum number of virtual channels generated on a firing.
								// Some sequence points may have channel type NOTHING
	WORD wStatus;		// see below
	WORD wLoc;			// x location in motion pulses relative to 1st packet from instrument
	WORD wAngle;		// 0.2048 ms clock counts from TOP relative to 1st packet from instrument
	WORD wPeriod;		// period of rotation in 0.2048 ms
	WORD wLastCmdSeqCnt;	//last command sequence cnt received by this PAP
	WORD wSendQDepth;	// Are packets accumulating in send queue.... 28 bytes to here
	stPeakData Results[MAX_RESULTS];	// Some "channels" at the end may be channel-type NONE 179*8=1432
	// need to add commmand queue depth in adc boards since they can only take 5 commands at a time 2017-02-22
	} IDATA_PACKET;	// sizeof = 1460 - the maximum TCPIP packet size for data

// https://blog.apnic.net/2014/12/15/ip-mtu-and-tcp-mss-missmatch-an-evil-for-network-performance/
// 1460 is max 

/***** Preliminary assignments - taken from Truscope4 definitions
IDATA_PACKET.wStatus
Bit		Meaning when bit is set
0		PAP cmd buffer full
1
2		Forward pipe motion
3		Pipe Present
4		Inspection Enabled
5		Array is down (on the pipe)

*/

#else
typedef struct
	{
	BYTE bvChannelQty;	// How many channels in this packet.
	BYTE instNumber;	// 0-255. Inst 0 -> base ip address of instruments
	WORD wStatus;		// tbd
	WORD wLoc;			// x location in motion pulses relative to 1 packet from instrument
	WORD wAngle;		// angle in degrees from TOP relative to 1 packet from instrument
	WORD wPeriod;		// period of rotation in 0.2048 ms
	UINT uMsgSeqCnt;	// counter to uniquely identify each packet
	UINT uSync;			// 0x5CEBDAAD ... 18 bytes to here
	RESULTS Results[32];	// Some "channels" at the end may be channel type NONE
	} IDATA_PACKET;	// sizeof = 210

#endif






// Command format from User interface systems to the PAP
// Command packet can be cut short by specifying a byte count less than 1056
// All commands now moved to the file \Iclude\Cmds.h
//
#if 0
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
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream
	BYTE bPapNumber;	// which PAP is the command for
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
	}	GenericPacketHeader;	// 12 bytes


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
#endif

// legacy structure
typedef struct
	{
	int nLength;		// number of bytes to send
//	BYTE *pMsg;			// ptr to the message bytes to send.
	BYTE Msg[1];		// ptr to the message bytes to send.
	}	stSEND_PACKET;


// If we want 2 out of 3 above threshold for Nc qualified, then bMod = 3. The Fifo is 3 elements deep.
// Each flaw reading goes into the fifo at location bInPt and overwrites the oldest element in the fifo.
// That is why we call it a FIFO.
// bInPt advances 1 fifo postion modulo bMod. After the input, the fifo is scanned from element 0 to bMod
// bAboveThld counts the number of readings above Thold. If it is >= bNc we have a bMax flaw.
// bMaxTemp is the max value in the FIFO w/o regard to Nc. If we have less than Nc AboveThld and bMaxTemp > thold
// then bMax is set to 80% of bThold. If we meet Nc count above thold then bMax = bMaxTemp and is returned by the 
// FifoInput() function
typedef struct
	{
	BYTE bCell[16];	// data cells containing input amplitudes 
	BYTE bInPt;		// next location to fill in the FIFO
	BYTE bNc;		// how many flaw values above threshold
	BYTE bThold;	// Threshold value 0-255
	BYTE bMax;		// Max Nc qualified value in FIFO
	BYTE bMaxTemp;	// Max value in FIFO w/o regard to Nc
	BYTE bMod;		// active depth of FIFO.
	BYTE bAboveThld;	// how many FIFO element above thold
	BYTE bMaxFinal;	// A peak hold of the peak hold bMax until ResetGatesAndWalls
	} Nc_FIFO;

// Each wall reading goes into a wall averaging FIFO. The memory size of the FIFO is selected to be longer 
// than the expected averaging interval which should be 8 or less (typically 2 to 4 readings).
// The FIFO output is actually the sum of the values in the FIFO.
// Values in the FIFO are not summed on each call, but the current location content in the FIFO is subtraced from the
// sum and then the new value is stored and added to the sum. Then the next location in the FIFO is selected for
// the next call (input) to the FIFO. This is exactly how a hardware implementation would work
// Wall reading are 16 bit values. Values less than 0.040 or greater than 2.00 inches are consider to be error.
// In case of a bad (erroneous) reading a counter is incremented and the FIFO does not advance
// Consecutive good wall readings above Nx length will reset the bad wall reading counter. 
// If the bad wall reading reaches a dropout value, the output reading from the FIFO 
// will be set to size of the averaging length (the average will be 1).
// The approximate conversion from machine reading to 0.001" increments is 1.452*hardware value
// Machine hardware readings for bad wall therefore are w < 27 or w > 1377
//

typedef struct
	{
	WORD wCell[16];	// data cell containing wall thickness readings
	BYTE bInPt;		// next location to fill in the FIFO
	BYTE bNx;		// how many wall values to average - the divisor
	UINT uSum;		// sum of all wall values in FIFO
	WORD wBadWall;	// how many out of range range wall readings
	WORD wGoodWall;	// consecutive good wall readings
	WORD wWallMax;	// maximum allowed hardware wall reading
	WORD wWallMin;	// minimum allowed hardware wall reading
	WORD wDropOut;	// number of bad readings before drop out occurs
	} Nx_FIFO;

// Data structures returned to Robert/PT
// replaced by stPeakData
#if 0
typedef struct
	{
	BYTE bFlaw[2];		// id = gate 2 /od = gate 3 0-255  
	WORD wTOFsum[2];	// divide by Nx and multiply by scaling factor for this vChannel
	}	RESULTS;		// sizeof = 6
#endif


typedef struct
	{
	WORD wStatus;		// tbd
	WORD wLoc;			// x location in motion pulses relative to 1 packet from instrument
	WORD wAngle;		// angle in degrees from TOP relative to 1 packet from instrument
	WORD wPeriod;		//unit in .2048ms
	} RAW_INSTRUMENT_STATUS;	// status info from Instrument collected to build output msg to Receiver/PAG

// If Nc = 0, the output value will be 0. This will decrease processing time in the PAM
// Effectively the associated channel will be a wall channel or nothing
// If Nx = 0, the output wall value will be 10. This will decrease processing time in the PAM
// Effectively the associated channel will be a flaw channel or nothing
//

#if 0
// When received as a command, bSeqNumber < 0 means end of command
typedef struct
	{
	BYTE bSeqNumber;	// Sequence number of this channel
	BYTE bChnlNumber;	// channel number in the above sequence for this channel
	// Flaw Nc for ID
	BYTE bNcID;		// how many flaw values required at or above threshold
	BYTE bTholdID;	// Threshold value 0-255
	BYTE bModID;	// active depth of FIFO. This is the 'm' in Nc out of m. eg., 2 out of 3 above thold
					// bModID must be >= bNcID. If not the PAM will make bModID=bNcID
	// Nc for OD
	BYTE bNcOD;		// how many flaw values required at or above threshold
	BYTE bTholdOD;	// Threshold value 0-255
	BYTE bModOD;	// active depth of FIFO. This is the 'm' in Nc out of m. eg., 2 out of 3 above thold
	// Interface gate
	BYTE bNcIf;		// how many flaw values required at or above threshold
	BYTE bTholdIf;	// Threshold value 0-255
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

/*****************	STRUCTURES	END *********************/

#if 0

/*****************	COMMANDS *********************/

#define NC_NX_TEST					1
#define SET_GATE_DELAY_CMD_ID		2
#define SET_GATE_RANGE_CMD_ID       3
/*****************	COMMANDS  END *********************/



#define TEST_UT                          20
#define SET_INSPECT_MODE                 21
#define SET_INSPECT_ENABLE				 30

#define RAW_DATA_ID                      10
#define DATA_ID                          11
#define ASCAN_ID                         12
#define CHANNEL_CMD_1_ID				 1

#define READ_TIMER_CMD_ID                201 //read command>200
#define READ_LM86_LT_CMD_ID              210
#define READ_LM86_RT_CMD_ID              211

#define SET_PULSE_RATE_CMD_ID           50
#define SET_SEQ_CMD_ID                  51
#define SET_PULSE_WIDTH_CMD_ID          52
#define SET_PULSE_CONTROL_CMD_ID        53
#define SET_PULSE_IN_OUT_CMD_ID         54
#define SET_PULSE_ENABLE_CMD_ID         55
#define SET_RECEIVE_ENABLE_CMD_ID       56

#define SET_PULSE_MEMORY_CMD_ID         57
#define SET_PULSE_ENABLE_ALL_CMD_ID     58
#define SET_PULSE_CPU_ENABLE_CMD_ID     59
#define SET_PULSE_AUTO_ENABLE_CMD_ID    40
#define SET_PULSE_ENABLE_MEM_CMD_ID     41


//#define SET_GATE_DELAY_CMD_ID           60
#define SET_GATE_RANGE_CMD_ID           61
#define SET_GATE_BLANK_CMD_ID           62
#define SET_GATE_THRESHOLD_CMD_ID       63
#define SET_GATE_CONTROL_CMD_ID         64
#define SET_GATE_DATA_MODE_CMD_ID       65
#define SET_GATE_TOF_MODE_CMD_ID        66
#define SET_GATE_SEQ_REG_CMD_ID         67

#define SET_HV_SWITCH_MEM_CMD_ID        70
#define SET_HV_SEQ_GROUP_CMD_ID         71

#define SET_GAIN_DAC_CMD_ID             79
#define SET_GAIN_STEP_CMD_ID            76
#define SET_GAIN_TGC_DELAY_CMD_ID       77
#define SET_GAIN_SEQ_CMD_ID             78

#define SET_LM86_ACT_CMD_ID             81
#define SET_LM86_CMD_CMD_ID             82
#define SET_LM86_DATA_CMD_ID            83
#define SET_LM86_CONF_CMD_ID            84
#define SET_LM86_LH_CMD_ID              85
#define SET_LM86_LL_CMD_ID              86


#define RESET_SCAN_CMD_ID              110
#define SET_SCAN_1X1_CMD_ID            100
#define SET_SCAN_2X2_CMD_ID            101
#define SET_SCAN_16X16_CMD_ID          102
#define SET_SCAN_DELAY_CMD_ID          103
#define SET_SCAN_DELAY_16X16_CMD_ID    104
#define SET_SCAN16_16X16_CMD_ID        105

#define SET_ASCAN_DELAY_CMD_ID         120
#define SET_ASCAN_RANGE_CMD_ID         121
#define SET_ASCAN_SEQ_REG_CMD_ID       122
#define ENABLE_ASCAN_INT_CMD_ID        123
#define DISABLE_ASCAN_INT_CMD_ID       124
#define SET_ASCAN_PEAK_SEL_REG_CMD_ID  125
#define SET_ASCAN_ZERO_MEM_CONFIG_ID   126

//add for lvds setup
#define LVDS_DATA_ALIGN                  130
#define LVDS_ALIGN_RESET                 131
#define ADC_SERIAL_SET                   132
#define ADC_REG_UPDATE                   133

#define SET_RECEIVER_MEMORY_CMD_ID      135 
#define SET_SCAN_REC_DELAY_16X16_CMD_ID       136
#define SET_SCAN_PITCH_PULSER_DELAY_16X16_CMD_ID    137

#define SET_SCAN_PULSER_MODE                 138 //0: echo mode, 1: pitch mode


#define WRITE_SPI0_REG      140
#define RESET_ALT_LVDS      141

#define CHANGE_9272_REG     161
#define CHANGE_9272_GAIN     162

#define SET_ASCAN_SEQ_ONE_BEAM_CMD_ID  170

#define SET_TCG_STEP_CMD_ID			   197
#define SET_TCG_FN_CMD_ID              198
#define SET_SCAN_TYPE_CMD_ID           199

#define LINEAR_SCAN_0_DEGREE           0
#define LINEAR_SCAN_37_DEGREE          1
#define THREE_SCAN_LRW_8_BEAM          2
#define THREE_SCAN_LRW_16_BEAM         3
#define TWO_SCAN_LR_8_BEAM             4
#define TWO_SCAN_LR_16_BEAM            5
#define LONG_8_BEAM_12345678		   6
#define LONG_8_BEAM_56781234		   7
#define LONG_24_BEAM_800               8
#define LONG_24_BEAM_080               9
#define THREE_SCAN_LRW_8_BEAM_FOCUS    10
#define THREE_SCAN_LO1LO1R_8_BEAM_12345678    11
#define THREE_SCAN_LO1LO1R_8_BEAM_56781234    12
#define LONG_24_BEAM_12345678		   13
#define LONG_24_BEAM_56781234		   14
#define WALL_25_BEAM_90_DEGREE_PROBE   15

#define DataHeadLength 16
#define DataPacketLength 24
#define RAW_DATA_PACKET_LENGTH 1040
#define AscanPacketLength 1040
#define CmdPacketLength 1040

#endif


#endif	// PA2_STRUCTS_H
