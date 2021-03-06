/*
Define structures and definitions for PhasedArray2 project
jeh
2016-05-17

Naming convention:
This project began in 2013. Originally the server to the instruments was called PAM - Phased Array Master
in 2016 it was redesignated as PAP - Phased Array Processor.  PAP = PAM
In this scheme, anything that communicates with the PAP via an Ethernet connection as a client is called an Instrument.
This includes pulser boards and phased array processor boards built by Tuboscope.
2017 Instrument is now called Board. Assumes that the Board has an Ethernet part.

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

enum IdOdTypes {eId, eOd, eIf};
enum CallSource {eMain, eInterrupt};

#define MAX_PAM_QTY			1
#define MAX_PAM_INSTS_QTY	8
#define NC_NX_CMD_ID		1+0x200		// also known as NC_NX_TEST
#define ASCANS_TO_AVG		16

#define THE_APP_CLASS	CPA2WinApp

// Idata message types. This is the data that comes from the PAP and is sent 
// To the Receiver system
// Read Back data replaces AScan data when read back is requested
//
enum Arria {eRawInspID = 1};	// hardware flaw/wall data for every chnl and seq
enum IdataTypes { eNcNxInspID = 1, eAscanID = 2, eReadBackID = 3, eAdcIdataID = 4, eKeepAliveID = 0xff };
enum DmaBlocks { eIdataBlock = 3, eAscanBlock = 0x83};
#define NC_NX_IDATA_ID				1		// PAP processed inspection data sent to PAG/Receiver system
#define ASCAN_DATA_ID				2
#define READBACK_DATA_ID			3
#define ADC_DATA_ID					4		// Idata and header from ADC - what comes in goes out 

// build 64 byte map to map Sam' digital input patter to Roberts
#define HD_SAM		(1 << 5)
#define IE_SAM		(1 << 4)
#define PP_SAM		(1 << 2)
//#define FWD_SAM	ALWAYS Roberts bit2 for now and always 1
#define HD_RC		(1 << 3)
#define FWD_RC		(1 << 2)
#define IE_RC		(1 << 1)

#define PP_RC		(1 << 0)


// edit this value if more client connections to servers are needed
//#define	MAX_SERVERS		1	//do it in ServerConnection Management for which ever type of server we are using
// Likely will have at least 2 server types. 1 for inspetion data and 1 for pulsers
// Mixing pulsers in with gate boards will make it more difficult to put dimensions on things like virtual channels. 2016-10-19

#define MAX_CLIENTS_PER_SERVER				8

// For the Client Communication System
#define MAX_CLIENTS							8


// An instrument client can have up to this many virtual channels for each UT firing or Main Bang
// Each MAIN BANG is a "sequence" until the sequence number repeats
// 64 until 11-08-16 then 32 == FOR Sam board, 8
#define MAX_CHNLS_PER_MAIN_BANG			8

// Channels may be redefined on each main bang. The counter which counts main bangs is called the sequence counter
// the maximum value the sequence counter can have was 16, now (2017/07/20) =
#define MAX_SEQ_COUNT					3

// The number of virtual channels is finite. Channels repeat after MAX_SEQ_COUNT number of main bangs.
// On any given main bang (sequence count) there can only be a max number of channels define by 
// MAX_CHNLS_PER_MAIN_BANG. The max the number of channels in a transducer array is 
// [MAX_SEQ_COUNT][MAX_CHNLS_PER_MAIN_BANG] = 32*8 = 256

// How many peak held data PeakChnl can we get in the max size tcpip packet.
// stPeakChnl PeakChnl[256] -> 4*256 = 1024
#define MAX_RESULTS						128			// before 8/14/17 was 176

#define INSTRUMENT_PACKET_SIZE			1088		//old 1040.. 1460 is max TCPIP size
//#define MASTER_PACKET_SIZE				1088		// 1260
#define SYNC							0x5CEBDAAD
#define CMD_FIFO_MEM_SIZE				0x400000		// must be greater than 0x1800
#define NC_NX_STRUCT_SIZE				52
#define MAX_CMD_PACKET_SIZE				1056

#define IDATA_MSG_ID					1
#define ASCAN_MSG_ID					2


#define MAX_FQDN_LENGTH					64	// max length of a fully qualified domain name
#define LOCAL_HOST						_T("localhost")
#define MSec50							50000
#define MSec40							40000

// IDATA_FROM_HW  wStatus bit definitions
#define BAD_CMD_FORMAT					(1 << 0)	// TCPIP message does not match header format in some way
#define BAD_SYNC_CHARS					(1 << 1)	// SYNC character not found at expected location
#define SMALL_CMD_BUF_OVERFLOW			(1 << 2)	// Small cmds arrived too fast - buffer overflowed
#define BIG_CMD_BUF_OVERFLOW			(1 << 3)	// Large cmds arrived too fast - buffer overflowed

/*****************	STRUCTURES	*********************/
// A channel is a UT echo or reflection assigned a physical position in the transducer.
// Since this is a multiplexed system, the same channel repeats eventually.
// The number of A-Scans that occur before the repeat is the SeqModulo.
// This number of A-Scans is a data frame.
// Raw data begins with the first channel of the frame. This channel can be of type NONE
// For this particular implementation it is assumed that there are up to 32 A-Scans in a data frame.
// bSeqModulo would be 32 in this case.

// REVISED 2016-10-20


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

						/* ++++++++++++++++++++++++++++++++++++++  */


// 2017-07-18 Back to the real world -- how this hardware really works
// Sam's hardware supports this only.
typedef struct 
	{
    WORD wTof;		//time of flight of Gate 4
    BYTE bAmp3;		//Gate 3 amplitude (0-255)
    BYTE bAmp2;		//Gate 2 amplitude (0-255)
	} CHNL_DATA;	// sizeof = 4

// The current Sam designed PA machine has 8 channels, each formed from 16 elements
// The elements overlap to produce 8 chnls from 64 elements.
// virtual channels are made from 16 contiguous elements.

typedef struct 
	{
	CHNL_DATA vChnl[8];		// 8 virtual channels each formed from 16 beams = 1 sequence
	} SEQ_DATA;				// sizeof = 32

// This is the structure format of data transmitted from the hardware front end
// If Wiznet is reset, this causes a glitch in serial stream. Must flush PAP process and start anew to 
// keep data synchronized with location information
typedef struct	// IDATA_FROM_HW
	{
	//BYTE bMsgID;		// Commands are word length, data returned is byte length
	WORD wMsgID;		// commands and data are identified by their ID	= eNcNxInspID
	//BYTE bSeqError;		// number of missed commands  -- wraps around at 256
	WORD wByteCount;	// Number of bytes in this packet. Make it even number		
	UINT uSync;			// 0x5CEBDAAD													
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream 0-0xffff	
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
						// AKA Instrument
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						//AKA card
	BYTE bBoardType;	// what kind of inspection device 1= wall 2 = socomate
	BYTE bPapStatus;	// AKA Instrument status, 0 means no errors. new on 2/25/20

	BYTE bStartSeqNumber;	// the NIOS start seq number which produced the packet. 
							// but in order of time occurrence, seq 0 might be last. Depends on NIOS board
	BYTE bSeqModulo;	// modulo of the sequence number. Last seq, modulo-1.  in 2020 modulo = 3 
	BYTE bMaxVChnlsPerSequence;	// maximum number of virtual channels generated on a firing.		16
								// Some sequence points may have channel type NOTHING
	BYTE bStartChannel;	// First virtual channel in peak data PeakChnl--always 0 for this hardware
	BYTE bSeqPerPacket;	// Nominally 32 sequences but can be less. Data at back end of packet is invalid
						// Maintains packet size of 1088. All seq valid when bSeqPerPacket = 32
	BYTE bNiosGlitchCnt;
	BYTE bCmdQDepthS;	// How deep is the Small command queue in the instrument NIOS processor
	BYTE bCmdQDepthL;	// How deep is the Large command queue in the instrument NIOS processor
						// NIOS has limited memory. Msg Q likely to be 
						// Large, [16][1056] = 16896 bytes, Small [128][32] = 4096

	BYTE bMsgSubMux;	// small Msg from NIOS. This is the Feedback msg Id
	BYTE bNiosFeedback[9];// eg. FPGA version, C version, self-test info		

	WORD wLastCmdSeqCnt;	//last command sequence cnt received by this PAP
	WORD wSendQDepth;	// Are packets accumulating in send queue.... 28 bytes to here

	// Pipe position information
	BYTE bDin;			// digital input, Bit1=Direction, Bit2=Inspection Enable, Bit4=Away(1)/Toward(0)
	BYTE bCmdQDepthP;	// Pulser cmd Q depth
	WORD wLocation;		// x location in motion pulses
    WORD wAngle;		// unit in .2048ms - ticks from TOP OF PIPE
    WORD wPeriod;		// unit in .2048ms
	WORD wRotationCnt;	// Number of rotations since pipe present signal
	WORD wStatus;		// see below
	WORD wVersionHW;	// Sams altera code version
	WORD wVersionSW;	// Johns C++ code version
	// Debugging command activation in instrument
	WORD wLastCmdId;	// the last command executed by the NIOS ADC program
	WORD w1stWordCmd;	// Most commands are 'WCmds' and the first word is only part of the command
	BYTE bCmdSeq;		// sequence selection of last command executed in NIOS
	BYTE bCmdChnl;		// channel in sequence selected for command
	BYTE bCmdGate;		// gate addressed by last command
	BYTE bCmdSpare;		// maintain 16/32 bit boundaries
	WORD wSpare[2];

	SEQ_DATA Seq[32];	// 32 sequences each of 8 virtual channels. 32*32 = 1024
	} IDATA_FROM_HW;	// sizeof = 1024 + 64 byte header = 1088

// Estimated 13 uSec to copy header into Wiznet
typedef struct	// IDATA_FROM_HW_HDR
	{
	//BYTE bMsgID;		// Commands are word length, data returned is byte length
	WORD wMsgID;		// commands and data are identified by their ID	= eNcNxInspID
	//BYTE bSeqError;		// number of missed commands  -- wraps around at 256
	WORD wByteCount;	// Number of bytes in this packet. Make it even number		
	UINT uSync;			// 0x5CEBDAAD													
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream 0-0xffff	
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
						// AKA Instrument
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						//AKA card
	BYTE bBoardType;	// what kind of inspection device 1= wall 2 = socomate
	BYTE bPapStatus;	// AKA Instrument status, 0 means no errors. new on 2/25/20

	BYTE bStartSeqNumber;	// the NIOS start seq number which produced the packet. 
							// but in order of time occurrence, seq 0 might be last. Depends on NIOS board
	BYTE bSeqModulo;	// modulo of the sequence number. Last seq = modulo-1
	BYTE bMaxVChnlsPerSequence;	// maximum number of virtual channels generated on a firing.		
								// Some sequence points may have channel type NOTHING
	BYTE bStartChannel;	// First virtual channel in peak data PeakChnl--always 0 for this hardware
	BYTE bSeqPerPacket;	// Nominally 32 sequences but can be less. Data at back end of packet is invalid
						// Maintains packet size of 1088. All seq valid when bSeqPerPacket = 32
	BYTE bNiosGlitchCnt;
	BYTE bCmdQDepthS;	// How deep is the Small command queue in the instrument NIOS processor
	BYTE bCmdQDepthL;	// How deep is the Large command queue in the instrument NIOS processor
						// NIOS has limited memory. Msg Q likely to be 
						// Large, [16][1056] = 16896 bytes, Small [128][32] = 4096

	BYTE bMsgSubMux;	// small Msg from NIOS. This is the Feedback msg Id
	BYTE bNiosFeedback[9];// eg. FPGA version, C version, self-test info		

	WORD wLastCmdSeqCnt;	//last command sequence cnt received by this PAP
	WORD wSendQDepth;	// Are packets accumulating in send queue.... 28 bytes to here

						// Pipe position information
	BYTE bDin;			// digital input, Bit1=Direction, Bit2=Inspection Enable, Bit4=Away(1)/Toward(0)
	BYTE bCmdQDepthP;	// Pulser cmd Q depth
	WORD wLocation;		// x location in motion pulses
	WORD wAngle;		// unit in .2048ms - ticks from TOP OF PIPE
	WORD wPeriod;		// unit in .2048ms
	WORD wRotationCnt;	// Number of rotations since pipe present signal
	WORD wStatus;		// see below
	WORD wVersionHW;	// Sams altera code version
	WORD wVersionSW;	// Johns C++ code version
						// Debugging command activation in instrument
	WORD wLastCmdId;	// the last command executed by the NIOS ADC program
	WORD w1stWordCmd;	// Most commands are 'WCmds' and the first word is only part of the command
	BYTE bCmdSeq;		// sequence selection of last command executed in NIOS
	BYTE bCmdChnl;		// channel in sequence selected for command
	BYTE bCmdGate;		// gate addressed by last command
	BYTE bCmdSpare;		// maintain 16/32 bit boundaries
	WORD wSpare[2];

	} IDATA_FROM_HW_HDR;		//sizeof = 64 bytes

/****************************************************************************************************************/


	// peak held data collected over 16 AScans for a single virtual channel and held in PeakData structure
	// Processed data sent from the PAP. One stPeakChnlPAP structure for every virtual channel
	// Nc_Nx processing receives 1 wall reading per AScan but produces a max and min reading 
	// after 16 AScans.
	// 2017-08-14 reduce size of PeakData to match input data from NIOS front end
	// 2017-10-24 in future have id/od flaw, max, min wall and seq number where found for each one.
	// Then structure size = 10
	// Will limit max seq modulo to 16 or 128 channels

typedef struct // stPeakChnlPAP
	{
	//	BYTE bStatus;	// bits 0..3 bad wall reading count, bit 5 wall dropout, bit 6 data over-run. 
	// ie, PAP did not service PeakData fast enough
	//	BYTE bG1;		// Gate 1 interface gate -- FUTURE FEATURE. place holder in 2017 version
	BYTE bStatus;	// to be determined
	BYTE bChNum;	// 0-255. bChNum = SeqNum*8 + Chnl. SeqNum = [0,15], Chnl  = [0,7]--goes away in future
	BYTE bId2;		// Gate 2 peak held data 0-255
	BYTE bOd3;		// Gate 3 peak held data 0-255
	WORD wTofMin;	// gate 4 min
	WORD wTofMax;	// gate 4 max -- joins sturcture in future-- then sizeof = 10
	} stPeakChnlPAP;	// sizeof = 8  -- From PAP

typedef struct // stPeakChnlNIOS
	{
	//	BYTE bStatus;	// bits 0..3 bad wall reading count, bit 5 wall dropout, bit 6 data over-run. 
	// ie, PAP did not service PeakData fast enough
	//	BYTE bG1;		// Gate 1 interface gate -- FUTURE FEATURE. place holder in 2017 version
	//	BYTE bStatus;	// to be determined
	BYTE bChNum;	// 0-255. bChNum = SeqNum*8 + Chnl. SeqNum = [0,15], Chnl  = [0,7]--goes away in future
	BYTE bId2;		// Gate 2 peak held data 0-255
	BYTE bOd3;		// Gate 3 peak held data 0-255
	WORD wTofMin;	// gate 4 min
					//WORD wTofMax;	// gate 4 max -- joins sturcture in future-- then sizeof = 10
	} stPeakChnlNIOS;	// sizeof = 5  -- From ADC

/*
 wStatus  bit definitions for IDATA_PAP structure
 bit	meaning when bit is set

 0		PAP small cmd count != NIOS cmd count
 1		PAP large cmd count != NIOS cmd count
 2		PAP pulser cmd count != NIOS cmd count
 3		Idata sequence error
*/

typedef struct // IDATA_PAP
	{
	//BYTE bMsgID;		// Commands are word length, data returned is byte length
	WORD wMsgID;		// commands and data are identified by their ID	= eNcNxInspID
	//BYTE bSeqError;		// number of missed commands  -- wraps around at 256
	WORD wByteCount;	// Number of bytes in this packet. Make it even number		
	UINT uSync;			// 0x5CEBDAAD													
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream 0-0xffff	
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
						// AKA Instrument
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						//AKA card
	BYTE bBoardType;	// what kind of inspection device 1= wall 2 = socomate
	BYTE bPapStatus;	// AKA Instrument status, 0 means no errors. new on 2/25/20
	
	BYTE bStartSeqNumber;	// the NIOS start seq number which produced the packet. 
							// but in order of time occurrence, seq 0 might be last. Depends on NIOS board
	BYTE bSeqModulo;	// modulo of the sequence number. Last seq = modulo-1
	BYTE bMaxVChnlsPerSequence;	// maximum number of virtual channels generated on a firing.		
								// Some sequence points may have channel type NOTHING
	BYTE bStartChannel;	// First virtual channel in peak data PeakChnl--always 0 for this hardware
	BYTE bSeqPerPacket;	// Nominally 32 sequences but can be less. Data at back end of packet is invalid
						// Maintains packet size of 1088. All seq valid when bSeqPerPacket = 32
	BYTE bNiosGlitchCnt;
	BYTE bCmdQDepthS;	// How deep is the Small command queue in the instrument NIOS processor
	BYTE bCmdQDepthL;	// How deep is the Large command queue in the instrument NIOS processor
						// NIOS has limited memory. Msg Q likely to be 
						// Large, [8][1056] = 16896 bytes, Small [128][32] = 4096

	BYTE bMsgSubMux;	// small Msg from NIOS. This is the Feedback msg Id
	BYTE bNiosFeedback[9];// eg. FPGA version, C version, self-test info	..30	

	WORD wLastCmdSeqCnt;	//last command sequence cnt received by this PAP
	WORD wSendQDepth;	// Are packets accumulating in send queue.... 28 bytes to here

						// Pipe position information
	BYTE bDin;			// digital input, Bit1=Direction, Bit2=Inspection Enable, Bit4=Away(1)/Toward(0)
	BYTE bCmdQDepthP;	// Pulser cmd Q depth
	WORD wLocation;		// x location in motion pulses
	WORD wAngle;		// unit in .2048ms - ticks from TOP OF PIPE
	WORD wPeriod;		// unit in .2048ms
	WORD wRotationCnt;	// Number of rotations since pipe present signal
	WORD wStatus;		// bit0 PAP small cmd!= NIOS, bit 1 PAP large cmd!= NIOS, bit 2 PAP pulser cmd!= NIOS
	WORD wVersionHW;	// Sams altera code version
	WORD wVersionSW;	// Johns C++ code version ..50
						// Debugging command activation in instrument
	WORD wLastCmdId;	// the last command executed by the NIOS ADC program
	WORD w1stWordCmd;	// Most commands are 'WCmds' and the first word is the only part of the command
	BYTE bCmdSeq;		// sequence selection of last command executed in NIOS
	BYTE bCmdChnl;		// channel in sequence selected for command
	BYTE bCmdGate;		// gate addressed by last command
	BYTE bAdcMsgIdErrorCntPAP;		// does the PAP lose msg receiving from UUI
	WORD wSpare[2];
	stPeakChnlPAP PeakChnl[MAX_RESULTS];	// Some "channels" at the end may be channel-type NONE 
	} IDATA_PAP;	// sizeof = 1024 + 64 =1088


typedef struct // IDATA_PAP_HDR
	{
	//BYTE bMsgID;		// Commands are word length, data returned is byte length
	WORD wMsgID;		// commands and data are identified by their ID	= eNcNxInspID
	//BYTE bSeqError;		// number of missed commands  -- wraps around at 256
	WORD wByteCount;	// Number of bytes in this packet. Make it even number		
	UINT uSync;			// 0x5CEBDAAD													
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream 0-0xffff	
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
						// AKA Instrument
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						//AKA card
	BYTE bBoardType;	// what kind of inspection device 1= wall 2 = socomate

	BYTE bPapStatus;	// AKA Instrument status, 0 means no errors. new on 2/25/20

	BYTE bStartSeqNumber;	// the NIOS start seq number which produced the packet. 
							// but in order of time occurrence, seq 0 might be last. Depends on NIOS board
	BYTE bSeqModulo;	// modulo of the sequence number. Last seq = modulo-1
	BYTE bMaxVChnlsPerSequence;	// maximum number of virtual channels generated on a firing.		
								// Some sequence points may have channel type NOTHING
	BYTE bStartChannel;	// First virtual channel in peak data PeakChnl--always 0 for this hardware
	BYTE bSeqPerPacket;	// Nominally 32 sequences but can be less. Data at back end of packet is invalid
						// Maintains packet size of 1088. All seq valid when bSeqPerPacket = 32
	BYTE bNiosGlitchCnt;
	BYTE bCmdQDepthS;	// How deep is the Small command queue in the instrument NIOS processor
	BYTE bCmdQDepthL;	// How deep is the Large command queue in the instrument NIOS processor
						// NIOS has limited memory. Msg Q likely to be 
						// Large, [8][1056] = 16896 bytes, Small [128][32] = 4096

	BYTE bMsgSubMux;	// small Msg from NIOS. This is the Feedback msg Id
	BYTE bNiosFeedback[9];// eg. FPGA version, C version, self-test info	..30	

	WORD wLastCmdSeqCnt;//last command sequence cnt received by this PAP
	WORD wSendQDepth;	// Are packets accumulating in send queue.... 28 bytes to here

						// Pipe position information
	BYTE bDin;			// digital input, Bit1=Direction, Bit2=Inspection Enable, Bit4=Away(1)/Toward(0)
	BYTE bCmdQDepthP;	// Pulser cmd Q depth
	WORD wLocation;		// x location in motion pulses
	WORD wAngle;		// unit in .2048ms - ticks from TOP OF PIPE
	WORD wPeriod;		// unit in .2048ms
	WORD wRotationCnt;	// Number of rotations since pipe present signal
	WORD wStatus;		// see below
	WORD wVersionHW;	// Sams altera code version
	WORD wVersionSW;	// Johns C++ code version
						// Debugging command activation in instrument
	WORD wLastCmdId;	// the last command executed by the NIOS ADC program
	WORD w1stWordCmd;	// Most commands are 'WCmds' and the first word is the only part of the command
	BYTE bCmdSeq;		// sequence selection of last command executed in NIOS
	BYTE bCmdChnl;		// channel in sequence selected for command
	BYTE bCmdGate;		// gate addressed by last command
	BYTE bAdcMsgIdErrorCntPAP;		// does the PAP lose msg receiving from UUI
	WORD wSpare[2];
	} IDATA_PAP_HDR;	// sizeof = 64

// Ascan data has same basic structure as Idata
// 2019-02-19 remove data redundant in Idata and replace with ascan gate location data
typedef struct // ASCAN_DATA
	{
	WORD wMsgID;		// commands and data are identified by their ID	= eAscanID	
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD
	WORD wMsgSeqCnt;	// interleaved with Idata, uses Idata seq count	10
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215
	WORD wBoardType;	// what kind of inspection device 1= wall 2 = socomate
	BYTE bSeqNumber;
	BYTE bVChnlNumber;	// what channel of the sequence is this data for?
						//BYTE bMsgSubMux;	
						//BYTE bNiosFeedback[7]****;	8 byte change

	WORD wScopeSetting;	// inform about trigger, thold, other scope settings
						// WORD wSendQDepth;	// Are packets accumulating in send queue    *** 2

						// Pipe position information
	BYTE bDin;			// digital input, Bit1=Direction, Bit2=Inspection Enable, Bit4=Away(1)/Toward(0)
	BYTE bCmdQDepthP;	// Pulser cmd Q depth
						// WORD wLocation;		// x location in motion pulses ****   8 byte change
						// WORD wAngle;		// unit in .2048ms - ticks from TOP OF PIPE
						// WORD wPeriod;		// unit in .2048ms
						// WORD wRotationCnt;	// Number of rotations since pipe present signal
	BYTE bFPGATempA;		// ADC board
	BYTE bBoardTempA;
	WORD wFPGA_VersionA;	// adc hw version
	WORD wNIOS_VersionA;	// adc NIOS version

						//  WORD wStatus;	*** 2	

	BYTE bBeamType;		// 0=rf 1=fw  2=peak hold,  4 = gate out-- from cmd 23
	BYTE bChCmd25;		// chnl  -- from cmd 25
	WORD wSeqCmd25;		// sequences which contribute to above ChSelected  -- from cmd 25
	BYTE bChCmd24;		// chnl selected by cmd 24 only for Ascan, not for gates -- from cmd 24
	BYTE bScopeGates;	// 1 or more gates to display. Selected by bits -- from cmd 26
						// 1=gate 0, 2=gate 1, 4=gate 2, 8=gate3, 16=TOF, 32=blanking
	// New gate location info 2019-02-19
	WORD G1[2];			// start,stop location
	WORD G2[2];			// start,stop location
	WORD G3[2];			// start,stop location
	WORD G4[2];			// start,stop location
	WORD TOF[2];		// start,stop locatio

	WORD wLargeCmds;	// number of large commands since reset
	WORD wSmallCmds;	// number of small commands since reset
	WORD wPulserCmds;	// number of pulser commands since reset
	WORD wFPGA_VersionP;	// Pulser fpga version
	WORD wNIOS_VersionP;	// Pulser NIOS version
	WORD wCPU_TempP;		// Pulser cpu temp - could be a byte
	char ascan[1024];	// 1024 8-bit scope amplitude samples

	} ASCAN_DATA;		// sizeof() = 1088

typedef struct // ASCAN_DATA_HDR
	{
	WORD wMsgID;		// commands and data are identified by their ID	= eAscanID	
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number
	UINT uSync;			// 0x5CEBDAAD
	WORD wMsgSeqCnt;	// interleaved with Idata, uses Idata seq count	10
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215
	WORD wBoardType;	// what kind of inspection device 1= wall 2 = socomate
	BYTE bSeqNumber;
	BYTE bVChnlNumber;	// what channel of the sequence is this data for?
						//BYTE bMsgSubMux;	
						//BYTE bNiosFeedback[7]****;	8 byte change

	WORD wScopeSetting;	// inform about trigger, thold, other scope settings
						//WORD wSendQDepth;	// Are packets accumulating in send queue    *** 2

						// Pipe position information
	BYTE bDin;			// digital input, 
	BYTE bCmdQDepthP;	// Pulser cmd Q depth
						// WORD wLocation;		// x location in motion pulses ****   8 byte change
	// WORD wAngle;		// unit in .2048ms - ticks from TOP OF PIPE
	// WORD wPeriod;		// unit in .2048ms
	// WORD wRotationCnt;	// Number of rotations since pipe present signal
	BYTE bFPGATempA;		// ADC board
	BYTE bBoardTempA;
	WORD wFPGA_VersionA;	// adc hw version
	WORD wNIOS_VersionA;	// adc NIOS version

						//  WORD wStatus;	*** 2	

	BYTE bBeamType;		// 0=rf 1=fw  2=peak hold,  4 = gate out-- from cmd 23
	BYTE bChCmd25;		// chnl  -- from cmd 25
	WORD wSeqCmd25;		// sequences which contribute to above ChSelected  -- from cmd 25
	BYTE bChCmd24;		// chnl selected by cmd 24 only for Ascan, not for gates -- from cmd 24
	BYTE bScopeGates;	// 1 or more gates to display. Selected by bits -- from cmd 26
						// 1=gate 0, 2=gate 1, 4=gate 2, 8=gate3, 16=TOF, 32=blanking
	// New gate location info 2019-02-19
	WORD G1[2];			// start,stop location gate 1
	WORD G2[2];			// start,stop location
	WORD G3[2];			// start,stop location
	WORD G4[2];			// start,stop location
	WORD TOF[2];		// start,stop location

	WORD wLargeCmds;	// number of large commands since reset
	WORD wSmallCmds;	// number of small commands since reset
	WORD wPulserCmds;	// number of pulser commands since reset
	WORD wFPGA_VersionP;	// Pulser fpga version
	WORD wNIOS_VersionP;	// Pulser NIOS version
	WORD wCPU_TempP;		// Pulser cpu temp - could be a byte
	//char ascan[1024];	// 1024 8-bit scope amplitude samples
	} ASCAN_DATA_HDR;	

// Read Back Data is loaded by NIOS and not by a dma process
typedef struct // READBACK_DATA
	{
	WORD wMsgID;		// commands and data are identified by their ID	= eReadBackID = 3
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number		
	UINT uSync;			// 0x5CEBDAAD													
	WORD wMsgSeqCnt;	// interleaved with Idata, uses Idata seq count
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215
	WORD wBoardType;	// what kind of inspection device 1= wall 2 = socomate
	BYTE bSeqNumber;
	BYTE bVChnlNumber;	// what channel of the sequence is this data for?
/* Change here from format of ASCAN_DATA_HDR */
	WORD wReadBackID;	// Read back ID for the command data requested- a sub command 18 bytes to here

	WORD wSpare[11];	// 40 bytes to here
	BYTE ReadBackBlock[1048];	// 1048 byte. Info depends on what is requested to be read back

	} READBACK_DATA;		// sizeof() = 1088


							// A packet of data sent from the Pulser to the PAP server updating pulser status
typedef struct // PULSER_DATA
	{
	WORD wMsgID;		// commands are identified by their ID 0x300+
	WORD wByteCount;	// Number of bytes in this packet. 64
	UINT uSync;			// 0x5CEBDAAD 
	WORD wMsgSeqCnt;	// counter to sequence command stream or data stream	WORD wMsgID;		// 1 = NC_NX_CMD_ID
	WORD wPulserCmds;	// number of pulser commands received since reset
	WORD wFPGA_Version;	// Maj.Minor.Build  FpgaVersion = (0x0 << 12) + (2 << 8) + 0; //0.2.0  -- nibble, nibble, byte
	WORD wNIOS_Version;	// nibble, nibble, byte maj.min.build
	WORD wCPU_Temp;		// in Celsius
	BYTE bPulserCmdQ;
	BYTE bSpare;
	WORD wSpare[22];
	}	PULSER_DATA;


/*************************************************************/



// The settings for one gate used by read back command
// Done instead of a readback from hardware addresses
// which was deemed unpractical 2019-04-03
// pairs of data, a control value plus the address where the value is written
// addresses are built from sequence, channel and gate values
typedef struct
	{
	WORD wDelay;	//the delay value written to hardware 
	WORD wRange;
	WORD wBlank;
	WORD bThold;
	WORD bTrigger;
	WORD bPolarity;
	WORD bTOF;
	//
	WORD wDelayAddr;	//the address of the delay value written to hardware
	WORD wRangeAddr;
	WORD wBlankAddr;
	WORD wTholdAddr;
	WORD wTriggerAddr;
	WORD wPolarityAddr;
	WORD wTOFAddr;
	} ST_GATE_SETTINGS;	// 28 bytes

typedef struct
	{
	ST_GATE_SETTINGS Gate[4];
	} ST_GATEINFO_PER_CHANNEL;	// 112 bytes

typedef struct 
	{
	ST_GATEINFO_PER_CHANNEL Ch[MAX_CHNLS_PER_MAIN_BANG];	// 8
	} ST_GATE_CH_PER_SEQ;	// 8*112 = 896

typedef struct // MAX_SEQ_COUNT = 3 in 2018 hardware
	{
	WORD wSeq;	// selects which one of 3 to copy into Wiznet xmit buffer
	ST_GATE_CH_PER_SEQ Seq[3];
	}	ST_GATE_READBACK_DATA;	// 896 + 2 = 898	//2690 must send 3 msg of 898 each

/****************************************************************/
// NEED ALL 8 BEAMS FOR 3 SEQUENCES ABOUT 3K. SEND BACK 3 READBACKS 1 FOR EACH SEQUENCE

// TCG Beam gain profile cmd 0x204
typedef struct // Gain-Address set
	{
	WORD wStartAddr;
	BYTE bGainPerCh[128];	// only saving 128 bytes
	} ST_ELEMENT_GAIN_PER_Ch;	// sizeof = 130

typedef struct
	{
	ST_ELEMENT_GAIN_PER_Ch Chnl[8];
	} ST_CHNL_PER_SEQ;		// 1040


typedef struct // cmd 205H
	{
	WORD wSeq;	// which one of 3 sequences to return on ReadBack
	ST_CHNL_PER_SEQ Seq[3];	// Only saving 3 sequences. Takes 3 read backs to get all info
	} ST_TCG_BEAM_GAIN_READBACK_DATA;	// save tcg DIGITAL gain values for each element in virtual probe
	// sizeof = 3122 read back
// ReadBack gets one squence at at time


// Data sturcture to overlay the structure of READBACK_DATA for cmd  0x204
typedef struct // CMD204H_READBACK
	{
	WORD wMsgID;		// commands and data are identified by their ID	= eReadBackID = 3
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number		
	UINT uSync;			// 0x5CEBDAAD													
	WORD wMsgSeqCnt;	// interleaved with Idata, uses Idata seq count
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215
	WORD wBoardType;	// what kind of inspection device 1= wall 2 = socomate
	BYTE bSeqNumber;
	BYTE bVChnlNumber;	// what channel of the sequence is this data for?
/* Change here from format of ASCAN_DATA_HDR */
	WORD wReadBackID;	// Read back ID for the command data requested- a sub command = 2
	ST_CHNL_PER_SEQ Seq;
	} CMD204H_READBACK;		// sizeof() = 1058

/************************************************************************************/

// CMD 0x205 TCGSeqGain - similar to 0x204 but word values instead of byte values. 
// This is a hardware gain 0-0X3FFF

// TCG Sequence gain  cmd 0x205
typedef struct // Gain-Address set
	{
	WORD wStartAddr;
	WORD wGainPerSeq[128];		// saving 256 bytes -- word gains
	} ST_ELEMENT_GAIN_PER_SEQ;	// sizeof = 258

typedef struct // cmd 205H
	{
	ST_ELEMENT_GAIN_PER_SEQ Seq[3];	// Only saving 3 sequences. 
	} ST_TCG_SEQ_GAIN_READBACK_DATA;	// save tcg analog gain values for each element in virtual probe
	// sizeof = 774 read back
// ReadBack gets all sequence gains in one packet



// Data sturcture to overlay the structure of READBACK_DATA for cmd  0x205
typedef struct // CMD205H_READBACK
	{
	WORD wMsgID;		// commands and data are identified by their ID	= eReadBackID = 3
	WORD wByteCount;	// Number of bytes in this packet. Try to make even number		
	UINT uSync;			// 0x5CEBDAAD													
	WORD wMsgSeqCnt;	// interleaved with Idata, uses Idata seq count
	BYTE bPapNumber;	// One PAP per transducer array. NO longer tied to IP address. Now assigned from file read
	BYTE bBoardNumber;	// which PAP network device (pulser, phase array board) is the intended target
						// this is the last 2 digits of the IP4 address of the board 
						// 192.168.10.200+boardNumber  range is .200-.215
	WORD wBoardType;	// what kind of inspection device 1= wall 2 = socomate
	BYTE bSeqNumber;
	BYTE bVChnlNumber;	// what channel of the sequence is this data for?
/* Change here from format of ASCAN_DATA_HDR */
	WORD wReadBackID;	// Read back ID for the command data requested- a sub command = 3
	ST_TCG_SEQ_GAIN_READBACK_DATA RB205; //774
	} CMD205H_READBACK;		// sizeof() = 792



/*
**************************************************************************************************
*/

#define SET_DROPOUT		 ( 1 << 5)
#define CLR_DROPOUT		~( 1 << 5)
#define SET_OVERRUN		 ( 1 << 6)
#define CLR_OVERRUN		~( 1 << 6)
#define DATA_READY		 ( 1 << 7)		// 16 Ascan peak held and copied to local PeakData Structure
#define CLR_DATA_READY	~( 1 >> 7)
#define SET_READ		 ( 1 << 4)		// PAP sets when read. If vChannel resets fifo's with this
#define CLR_READ		~( 1 << 4)		// not set, it is overrun condition
#define DEFAULT_CFG		 ( 1 << 3)		// Nc Nx have default constructor values
#define STATUS_CLEAR_MASK	~SET_OVERRUN

// legacy structure
typedef struct
	{
	int nLength;		// number of bytes to send
//	BYTE *pMsg;			// ptr to the message bytes to send.
	BYTE Msg[1];		// ptr to the message bytes to send.
	}	stSEND_PACKET;

/*
2016-09-08 New definition of Idata Packet
*/

// Based on processing rate of PAP,	 may have to have one PAP for each instrument.
// IDATA_PAP is the peak held data from the PAP and sent after 16 ascans
// IDATA_PAP is the input to the down stream system, ie goes to the GUI
// PAP receives 5 bytes for every channel

						
// Gates read back data structure

// https://blog.apnic.net/2014/12/15/ip-mtu-and-tcp-mss-missmatch-an-evil-for-network-performance/
// 1460 is max 

/***** Preliminary assignments - taken from Truscope4 definitions
IDATA_PAP.wStatus
Bit		Meaning when bit is set
0		PAP cmd buffer full
1
2		Forward pipe motion
3		Pipe Present
4		Inspection Enabled
5		Array is down (on the pipe)

*/

// If we want 2 out of 3 above threshold for Nc qualified, then bMod = 3. The Fifo is 3 elements deep.
// Each flaw reading goes into the fifo at location bInPt and overwrites the oldest element in the fifo.
// That is why we call it a FIFO.
// bInPt advances 1 fifo postion modulo bMod. After the input, the fifo is scanned from element 0 to bMod
// bAboveThld counts the number of readings above Thold. If it is >= bNc we have a bMax flaw.
// bMaxTemp is the max value in the FIFO w/o regard to Nc. If we have less than Nc AboveThld and bMaxTemp > thold
// then bMax is set to 80% of bThold. If we meet Nc count above thold then bMax = bMaxTemp and is returned by the 
// FifoInput() function
typedef struct // Nc_FIFO
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

typedef struct //Nx_FIFO
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

typedef struct  //RAW_INSTRUMENT_STATUS
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


// A structure to hold packets per second info. Used to move screen output out of
// real time thread into main dialog output on time
// Need one structure for all wall and another for Nx data
typedef struct
	{
	float fPksPerSec;	// = 2048000000.0f / ((float)m_nElapseTime);
	UINT uPktsPerSec;	// computed in server socket m_pSCC->uPacketsPerSecond = (UINT)fPksPerSec; 	
	int nClientIndx;
	WORD wMsgSeqCnt;
	int nElapseTime;
	UINT uPktsSent;
	int nTrigger;		// 0 = no update in main dlg. 1 = update in main dialog
	} PACKET_PER_SEC;	// sizeof = 26
/*****************	STRUCTURES	END *********************/



#endif	// PA2_STRUCTS_H
