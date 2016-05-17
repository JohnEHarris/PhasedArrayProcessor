/*
udp_msg.h

Date:	07/16/01
Revised:
		09/21/01 add structure for tracking packet errors
		10/19/01 Take relevant pieces from ut_msg.h (utron) and
				add to this file
		12/07/01 rename channel_config structure to channel_info.
			Channel_info struct now defined for mmi truscop2/utron2
		02/26/02 channel_config now used to tell udp characteristics of channel
		05/10/02 define status word 0 bits
		05/16/02 INCREASE BUF_SIZE from 450 to 600 to handle cdpmsg2/jobrec
		05/29/02 Increase size of UtInsp structure to have flaw amp and clock for
				each chnl in 1 inch segment.  Only the max of the sig value in this inch
				captured.  Multiple clock locations are NOT shown.
		06/11/02 INcrease BUF_SIZE to 800 to handle scope_dmx_cal msg
		
*/
/* Define UDP messages used between master and slave udp's */

#ifndef UDPMSG_H

#define UDPMSG_H

#ifndef INSTDATA_H
#include "instdata.h"
#endif

#ifndef GLOBAL_H
#include "global.h"
#endif

#ifndef  TWO_CHANNEL_4_GATE_BOARD
#define  TWO_CHANNEL_4_GATE_BOARD
#endif

#ifdef  TWO_CHANNEL_4_GATE_BOARD
#ifndef  MMI_CODE
#define  MMI_CODE
#endif
#endif

#ifndef MAX_CHANNEL_PER_INSTRUMENT
#define MAX_CHANNEL_PER_INSTRUMENT 10
#endif
//define BUF_SIZE			800		-- changed on 03-Aug-12 jeh for TCHAR instead of Char elements.
#define BUF_SIZE			1200

#define PRINT_RECEIVE_MSG 0

#define SEND_MMI_MSG_NUM  20000     //size of the array holding the messages sent to MMI from Master .. doesn't seem to be used anywhere

/***** Structures need to maintain 4 byte boundaries *****/

typedef struct
	{
	int nSent;		/* Number sent of this type packet */
	int nRcv;		/* Number received */
	WORD wLast;		/* msg cnt of last received msg */
	WORD nDup;		/* number of duplicate messages */
	int nLost;
	int nReadSum;	/* total bytes read */
	} PACKET_STATS;

/** Truscope uses a byte per gate.. Word here **/
typedef struct
	{
	WORD g[MAX_GATE];		/* 0 = id, 1 = od	*/
	}	UT_CHNL;	/* 2 gates in a chnl	*/


typedef struct
	{
	UT_CHNL Ch[10];
	}	UT_SHOE;

/* local MACROS */

//#define STATIC static   ?????
#define STATIC
#define SHORT  short


#if 0
5-23-02 PHILOSOPHY CHANGE:
1 = BAD OR FAIL, 0 = GOOD OR OK ON STATUS BITS


status[0] saved for legacy truscope assignments

status[1] bit definitions:
	bits 0-3 download to instrument 0=succeed, 1=fail
		bit 0 = inst 1 = slave 1
		bit 1 = inst 2 = slave 2 etc.

	bits 4-7 missed instrument packets
		bit 4 = inst 1 = slave1.  1= missed packet

	bit 15 Wall data status, 0 = EU, 1 = raw counts
	bit 14 Strip Chart, not all chnls displayed 1=single chnl, 0 = all chnls
	/* chnls not displayed are masked off in software... produce not inspection results*/
#endif


#define		WALL_STATUS_NOT_EU		0x8000
#define		STRIP_DISPLAY_NOT_ALL_CHNLS		0x4000



typedef struct
	{
	WORD	Mach;		/* which machine, B: = 0x3a42 */

	WORD	MsgId;
	DWORD	MsgNum;
	WORD	status[3];      /* status[2] is the motion bus value */
	WORD	wLineStatus;	/* status at time msg sent	*/
	short	xloc;
	WORD	Period;		/* Rotation period in 0.1 ms	*/
	DWORD	JointNumber;	/*12/15/00 to dword from word	*/
	WORD	nNorWall;       /* norminal wall */

	short	MinWall;		/* min of all SegWall values*/
	short	MaxWall;		/* max of all SegWall values	*/
	BYTE    MinWallClk;
	BYTE    MaxWallClk;

	DWORD   TruscanMsgId;
	BYTE    Num_of_Beams;
	BYTE    Num_of_Scans;
	WORD    spare;
	WORD	Wall[500];
	} I_MSG_WD;				/* Wall Display message */

// Sent from TruWall MMI to WallBarDisplay
// I_MSG_WD.status[2] == 0 means lamination data -- range 0-100
// otherwise wall data
//
// Num_of_Beams is the number of readings (or bars for display) across the array.
// Num_of_Scans is how many times the array has been scanned.
// The total valid readings stored in the Wall[] will be Num_of_Beams x Num_of_Scans, 
// e.g., if Num_of_Beams=25 and Num_of_Scans=20, the total number of readings in Wall[] will be 500.  
//	The total number of valid readings may be smaller than 500, but it cannot be bigger than 500.


typedef struct 
	{
	WORD	status[3];      /* status[2] is the motion bus value */
	WORD	wLineStatus;	/* status at time msg sent	*/
	DWORD	NextJointNum;   /* next joint number */
	time_t	time;
	short	VelocityDt;		/* delta t to travel 4 inches in 1 ms clocks */
	short	xloc;
	short	nAngleBody1;	/* location of shoe1 in 2 degree increments	*/
	WORD	JointLength;	/* in motion pulses	*/
	DWORD	JointNumber;	/*12/15/00 to dword from word	*/
	WORD	Period;		    /* period of rotation in 0.1 ms	*/
	WORD	nStation;       /* from which station the Idata of this message comes */
	BYTE	ShoeOn[4];		/* bit0 = 0 -> shoe off, bit7 = 1 ->sig> thold this shoe	*/
	WORD	ChnlOn[4];		/* bit mapped for each chnl. 0 = chnl off	*/
	WORD	EchoBit[MAX_SHOES];	/* bit mapped for each chnl. 0 = no echo	*/
	} INSP_HDR;             /* 64 bytes */

typedef struct 
	{
	/* Radial segment info follows	*/
	/* This data is aligned Radially and longitudinally ( x & omega)	*/

	BYTE	MaxLodFlaw;		/* max of the LODFlaw array for graph plot	*/
	BYTE	MaxLidFlaw;		/* max of the LIDFlaw array	*/
	BYTE	MaxTodFlaw;
	BYTE	MaxTidFlaw;
	BYTE	MaxQ1odFlaw;
	BYTE	MaxQ1idFlaw;
	BYTE	MaxQ2odFlaw;
	BYTE	MaxQ2idFlaw;
	BYTE	MaxQ3odFlaw;
	BYTE	MaxQ3idFlaw;
	BYTE	MaxLamAFlaw;    /* Laminar amplitude only */
	BYTE    MaxLamLFlaw;    /* Laminar amplitude and wall dropout at the same pulse (logic) */

	BYTE	LodClk;		/* Clk loc of MaxLod	*/
	BYTE	LidClk;	
	BYTE	TodClk;
	BYTE	TidClk;
	BYTE	Q1odClk;
	BYTE	Q1idClk;
	BYTE	Q2odClk;
	BYTE	Q2idClk;
	BYTE	Q3odClk;
	BYTE	Q3idClk;
	BYTE	LamAClk;
	BYTE	LamLClk;

	BYTE	LodChnl;		/* Chnl of MaxLod	*/
	BYTE	LidChnl;	
	BYTE	TodChnl;
	BYTE	TidChnl;
	BYTE	Q1odChnl;
	BYTE	Q1idChnl;
	BYTE	Q2odChnl;
	BYTE	Q2idChnl;
	BYTE	Q3odChnl;
	BYTE	Q3idChnl;
	BYTE	LamAChnl;
	BYTE	LamLChnl;

	short	MinWall;		/* min of all SegWall values*/
	short	MaxWall;		/* max of all SegWall values	*/
	BYTE	MinWallClk;
	BYTE	MaxWallClk;

	BYTE	MinWallChnl;
	BYTE	MaxWallChnl;

	/* do it in mmi for now from maxecc to avgecc */
	short	MaxEcc;			/* max eccentricity in this inch band	*/
	short	AvgWall;		/* avg of all min & max in SegWall within +/- 50% nominal	*/
	short	AvgMinWall;		/* avg of all min wall readings within +/- 50% nominal	*/
	short	WallDiff;		/* avg of all wall readings within +/- 10% nom	*/
	short	AvgEcc;			/* avg of all eccentricity reading in this band	*/
	/* do it in mmi for now from maxecc to avgecc */

	BYTE	FlawDetected;	/* 0 is no flaw */
	BYTE	spare2;		/* for the future	*/
	DWORD   TruscanMsgId;           /* used to filter out unknown messages */
	BYTE	SegLodFlaw[N_SEG]; /* 0 -0xff flaw amp for Long OD	*/
	BYTE	SegLidFlaw[N_SEG]; /* 0 -0xff flaw amp for Long ID	*/
	BYTE	SegTodFlaw[N_SEG]; /* 0 -0xff flaw amp for Tran OD	*/
	BYTE	SegTidFlaw[N_SEG]; /* 0 -0xff flaw amp for Tran ID	*/
	BYTE	SegQ1odFlaw[N_SEG]; /* 0 -0xff flaw amp for Oblq OD */
	BYTE	SegQ1idFlaw[N_SEG]; /* 0 -0xff flaw amp for Oblq ID */
	BYTE	SegQ2odFlaw[N_SEG]; /* 0 -0xff flaw amp for Oblq OD */
	BYTE	SegQ2idFlaw[N_SEG]; /* 0 -0xff flaw amp for Oblq ID */
	BYTE	SegQ3odFlaw[N_SEG]; /* 0 -0xff flaw amp for Oblq OD */
	BYTE	SegQ3idFlaw[N_SEG]; /* 0 -0xff flaw amp for Oblq ID */
	BYTE	SegLamAFlaw[N_SEG]; /* 0 -0xff flaw amp for Oblq OD */
	BYTE	SegLamLFlaw[N_SEG]; /* 0 -0xff flaw amp for Oblq ID */

	short	SegWallMin[N_SEG];	/* min wall for each 12 degrees	*/
	short	SegWallMax[N_SEG];	/* max wall for each 12 degrees	*/

	BYTE	SegLodChnl[N_SEG]; /* chnl number for Long OD	*/
	BYTE	SegLidChnl[N_SEG]; /* chnl number for Long ID	*/
	BYTE	SegTodChnl[N_SEG]; /* chnl number for Tran OD 	*/
	BYTE	SegTidChnl[N_SEG]; /* chnl number for Tran ID 	*/
	BYTE	SegQ1odChnl[N_SEG]; /* chnl number for Oblq OD	*/ 
	BYTE	SegQ1idChnl[N_SEG]; /* chnl number for Oblq ID	*/ 
	BYTE	SegQ2odChnl[N_SEG]; /* chnl number for Oblq OD	*/ 
	BYTE	SegQ2idChnl[N_SEG]; /* chnl number for Oblq ID	*/ 
	BYTE	SegQ3odChnl[N_SEG]; /* chnl number for Oblq OD	*/ 
	BYTE	SegQ3idChnl[N_SEG]; /* chnl number for Oblq ID	*/ 
	BYTE	SegLamAChnl[N_SEG]; /* 0 -0xff flaw amp for laminar amplitude */
	BYTE	SegLamLChnl[N_SEG]; /* 0 -0xff flaw amp for laminar logic (amplitude + wall drop) */
	BYTE	SegMinChnl[N_SEG]; /* chnl number for Wall Min	*/
	BYTE	SegMaxChnl[N_SEG]; /* chnl number for Wall Max	*/
	/* Add arrays to capture flaw info for "All Flaws" for cdp */
	/* Max gate ok sig level in this 1 inch of pipe for each gate */
	BYTE	GateMaxAmp[80];			/* flaw amp for both gates, 40 chnls */
	BYTE	GateMaxClk[80];			/* Clock value for associated flaw amp */

	} UT_INSP;

#if 0	
typedef struct
	{
	BYTE	MaxLodFlaw;		/* curren max value being peak held between messages	*/
	BYTE	MaxLidFlaw;		/* max of the LidFlaw array	*/
	BYTE	MaxTodFlaw;
	BYTE	MaxTidFlaw;
	BYTE	MaxQod1Flaw;
	BYTE	MaxQid1Flaw;
	BYTE	MaxQod2Flaw;
	BYTE	MaxQid2Flaw;
	BYTE	MaxQod3Flaw;
	BYTE	MaxQid3Flaw;
	short	MinWall;		/* min of all SegWall values	*/
	short	MaxWall;		/* max of all SegWall values	*/
	UT_SHOE Sh[MAX_SHOES];
	UT_SHOE ShWall[MAX_SHOES];		/* wall instead of flaw. g0=min, g1=max	*/
	BYTE	bSkip4[30];
	} UT_TIME_SAMPLE;

#endif


/* Who builds the messages.  Instrument built messages are I_MSG	*/
/* Catcher/MMI built messages are C_MSG	*/
	
/* Command structure used within the UDP processors (master & slaves) */
/* These structure deal with the handling and error detection of the  */
/* packets and not with the inspection itself. */

#ifdef MMI_CODE

typedef struct
	{

/*	BYTE DMac[6];	/* dest mac */
/*	BYTE SMac[6];	/* source mac */
/*	WORD len;		/* packet len */
/*	IPXHEADER IpxHdr;	*/

	WORD	Mach;		/* which machine, B: = 0x3a42 */
	short 	nSlave;		/* Which slave */
	DWORD	MsgNum;
	//WORD	MsgLen;
	WORD	MsgId;
	WORD	nWhichWindow;   /* to which inspect window this message will go */
	PACKET_STATS IdataStat;	/* status of packets from inst to slave */
	PACKET_STATS SlaveStat;	/* status of packets from slave to master */
	PACKET_STATS MmiStat;	/* status of packets from MMI to master */
	} UDP_CMD_HDR;				/* master header 72 bytes */

#else

typedef struct
	{
	BYTE DMac[6];	/* dest mac */
	BYTE SMac[6];	/* source mac */
	WORD len;		/* packet len */
	IPXHEADER IpxHdr;
	WORD	Mach;		/* which machine, B: = 0x3a42 */
	short 	nSlave;		/* Which slave */
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	WORD	spare;
	PACKET_STATS IdataStat;	/* status of packets from inst to slave */
	PACKET_STATS SlaveStat;	/* status of packets from slave to master */
	PACKET_STATS MmiStat;	/* status of packets from MMI to master */
	} UDP_CMD_HDR;

#endif


typedef struct
	{
	BYTE DMac[6];	/* dest mac */
	BYTE SMac[6];	/* source mac */
	WORD len;		/* packet len */
	IPXHEADER IpxHdr;
	WORD	Mach;		/* which machine, B: = 0x3a42 */
	short 	nSlave;		/* Which slave */
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	WORD	status;
	PACKET_STATS IdataStat;	/* status of packets from inst to slave */
	PACKET_STATS SlaveStat;	/* status of packets from slave master */
	PACKET_STATS MmiStat;	/* status of packets from MMI to master */
	} SLAVE_HDR;

/******************  SLAVE_HDR = UDP_CMD_HDR ********************/



/**************************************************************/
/**************** Scope Demux Structures  *********************/
/**************************************************************/

	/* 
	Initially, the scope adjustment values for the scope demux
	board will be left at hardware default values
	*/

	/* Any time the channel changes, the dmx_adj msg must be sent */
	/* Any time there is a change in rf/fw, the dmx_adj msg must be sent */
	/* Reject is the same for all fw channels.  RF channels do not apply reject */

	typedef struct Scope_Dmx_Adj_tag
		{
		WORD SubCmd;	/* SCOPE_DEMUX_ADJUST */
		WORD V_ChNull;	/* hw default */
		WORD V_Rej;		/* hw default */
		WORD V_Gain;	/* hw default */
		WORD V_ScopeNull;	/* hw default */
		WORD V_Shunt;	/* hw default */
		}	SCOPE_DMX_ADJUST;

	typedef struct Scope_Dmx_Dsply_tag
		{
		WORD SubCmd;	/* SCOPE_DEMUX_DISPLAY */
		WORD SeqLen;	/* 0/1 */
		WORD Trig;
		WORD Trace1;	/* rf or fw */
		WORD Trace2Mode;	/* Sync or Gates */
		WORD Trace2;	/* FOF, EOA, or Gates */
		WORD G1Level;
		WORD G2Level;
		}	SCOPE_DMX_DISPLAY;


typedef struct
	{
	WORD SubCmd;		/* SCOPE_DEMUX_AUTOCAL */
	/* All variables set/computed by MMI go at beginning of structure */
	/* All variables set/computer by Master UDP go at end of structure */
	
	/* Set by operator/MMI */
	short	nCmd;		/* which dmx adj cmd send/receive */
	short	nState;		/* which state or step in process */
	short	nRfFw;		/* 0 = rf, 1 = fw mode */
	short	nChnl;		/* which chnl being adjusted */
	/* Trace 2 variables for gate displays */
	short	nShunt[MAX_GATE];	/* Adjust scope offset when no gate signal present */
	/* Use ref dac reading to compute linear eq to set gate levels */
	/* Ref values set by operator with slider */
	short	nG1Ref[2][2];	/* Ref thold level ( 0&100 typically) for rf and fw */
	short	nG2Ref[2][2];	/* Ref thold level ( 0&100 typically) for rf and fw */

	/* Trace 1 variables */
	short	nReject[2];		/* noise reject level for fw only.  No reject on rf */
	short	nVgain[MEM_MAX_CHANNEL][2];	/* chnl gain trim for 40 chnls, rf & fw */
	short	nScope1Null[2];	/* adjust dc offset for scope for final stage, rf & fw */

	/* Set by UDP based on state and cmd from MMI */

	short	nAdcReading;	/* current value of ADC on noise reject output */
	short	nAdcGnd;		/* board ground reading */
	short	nspare1;		/* necessary for vxworks compiler alignment */
	/* a & b coefficients computed by master udp */
	float	fAg1[2];		/* a*gate_lvl + b = dac setting, [2] for rf/fw */
	float	fBg1[2];
	float	fAg2[2];		/* a*gate_lvl + b = dac setting, [2] for rf/fw */
	float	fBg2[2];
	short	nChNull[MEM_MAX_CHANNEL][2];	/* fix dc offset for each chnl, rf & fw */
	float	fArej;			/* a*rej_lvl + b = dac setting, input in 0-100 % */
	float	fBrej;
	short	spare[40][2];
	}	SCOPE_DMX_CAL;

	/* cmd = 1, initial download of structure from mmi to master udp */
	/* cmd = 2, begin autocal operation */
	/* cmd = 3, adj in progress,  */
	/* cmd = 4, final upload of completed adjustment from udp */
	/* cmd = 5, compute linear equation for gate 1 & 2 coeff */
	/* cmd = 6, terminate autocal operation in udp */



/* The inspection message	*/
typedef struct
	{
	UDP_CMD_HDR MstrHdr;	/* header from master udp */
	INSP_HDR	InspHdr;
	UT_INSP	UtInsp;
	}	I_MSG_RUN;

typedef struct
	{
	DWORD PacketNum;
	BYTE bNewData;		/* flag to indicate new packet data */
	BYTE RDP_number;	/* which slave */
	BYTE MsgType;
	BYTE RdpStatus;		/* 3: no hardware found (once in this state, can't be changed to other states); 2: hardware found but no config file downloaded; 1: normal */
	short xloc;			/* location in 1 inch increments */
	WORD wLineStatus;	/* direction/inspect info from ulc or plc */
	WORD EchoBit;		/* bits 0-9 are xdcr echo bit, bit 15 is valid set */
	WORD ShoeAngle;		/* Angle of shoe at start of packet 16 bytes to here*/
	BYTE Amp[10][MAX_GATE];	/* Id/od ok amp value */
	WORD MinWall[10];
	WORD MaxWall[10];
#ifndef  TWO_CHANNEL_4_GATE_BOARD
	BYTE spare2[124];
	UT_SHOE ShWall[MAX_SHOES];		/* wall instead of flaw. g0=min, g1=max	*/
#endif
	}	PEAK_DATA;


/* The calibration message	*/
typedef struct
	{
	UDP_CMD_HDR MstrHdr;	/* header from master udp */
	INSP_HDR	InspHdr;
/*	UT_TIME_SAMPLE	UtSample;	obsolete */
	PEAK_DATA Shoe[MAX_SHOES];		/* peak slave data from 4 shoes */
	}	I_MSG_CAL;


/* The Raw IPX instrument data message */
typedef struct
{
	UDP_CMD_HDR MstrHdr;	/* header from master udp */
	INSP_HDR	InspHdr;
	WORD spare;
	WORD msg_cnt;
	BOARD bd;
#if 0	
	INST_DATA InstData;	/* 16 Ascans*10 chnls*2 gates + msg_cnt */
#endif
}	I_MSG_PKT;


/* The network connectivity message */
typedef struct
{
	UDP_CMD_HDR   MstrHdr;	/* header from master udp */
	INSP_HDR      InspHdr;
	BYTE          bConnected[NUM_OF_SLAVES+1];  /* 0: not connected, 1: connected.  Master in index 0, Slave1 in index 1, etc. */
	char	      MasterCodeRev[40];
	char	      SlaveCodeRev[24][40];
	BYTE	      nBoardRevision[MAX_SHOES+1][MAX_BOARD_PER_INSTRUMENT];
}	I_MSG_NET;

/* A-Scan message */
typedef struct
	{
	WORD	Mach;		/* which machine, B: = 0x3a42 */
	short 	nSlave;		/* Which slave */
	DWORD	MsgNum;
	WORD	MsgId;
	WORD	nWhichWindow;   /* to which inspect window this message will go */
	WORD	AscanDelay;  //in 0.02 microseconds
	WORD	AscanRange;  //in microseconds
	char	GateTrigger[MAX_GATE];  //0=disabled; 1=IP; 2=IF
	short   GateDelay[MAX_GATE];  //in 0.1 microseconds
	short   GateLevel[MAX_GATE];  //in percent, runs from 1 to 100
	short   GateRange[MAX_GATE];  //in 0.1 microseconds
	short   GateBlank[MAX_GATE];  //in 0.1 microseconds
	BYTE	bRFsignal;   //RF=1; FW=0
	BYTE	bInvert;     //If in RF mode, do we need to invert the signal? YES=1; NO=0
	BYTE	nChannel;    //Channel# of the current A-Scan
	BYTE    spare[17]; 
	} ASCAN_MSG_HDR;				/* master header */

typedef struct
{
	UDP_CMD_HDR   MstrHdr;	/* header from master udp */
	BYTE	    Ascan[1024];
}	I_MSG_ASCAN;

typedef struct
{
	ASCAN_MSG_HDR MstrHdr;	/* header from master udp */
	BYTE	    Ascan[1024];
}	I_MSG_ASCANBCAST;

typedef struct {
    unsigned char bMsgID;
    unsigned char bSpare1;
    unsigned char bSeq;
    unsigned char bDin;

    unsigned short wMsgSeqCnt;
    unsigned short wLocation;
    unsigned short wClock; //unit in .2048ms
    unsigned short wPeriod; //.2048ms
    unsigned char bSpare2[4]; //more spare
} ASCAN_HEAD_NIOS; //16

/* Board revision numbers */
typedef struct
{
	char		MasterCodeRev[40];
	char		SlaveCodeRev[40];
	BYTE		nRev[MAX_SHOES+1][MAX_BOARD_PER_INSTRUMENT];
}	BOARD_REV;


/* PLC status struct */
typedef struct
{
	DWORD dwMotionBus;
	DWORD dwXloc;
	DWORD dwOmega;
	DWORD dwDiscreteIO;
}   PLC_STATUS;

/* The PLC status message */
typedef struct
	{
	UDP_CMD_HDR MstrHdr;	/* header from master udp */
	INSP_HDR	InspHdr;
	PLC_STATUS       PlcStatus;  /* 0: not connected, 1: connected.  Master in index 0, Slave1 in index 1, etc. */
	}	I_MSG_PLC;


/* The Raw Flaw message */
typedef struct
	{
	UDP_CMD_HDR MstrHdr;	/* header from master udp */
	BYTE  Buf[1152];
	}	I_MSG_RAW_FLAW;


/* The AD Converter message used for scope trace 1 calibration */
typedef struct
	{
	UDP_CMD_HDR MstrHdr;	/* header from master udp */
	INSP_HDR	InspHdr;
	short       nADC;		/* 0: value of the AD Converter, average of 8 readings */
	WORD		nGate1Amp;
	WORD		V_Ch_Null;
	WORD		V_Nr;
	WORD		V_Gain;
	WORD		V_Scope_Null;
	}	I_MSG_ADC;


/* Diagnostic message showing packet statistics for inst, slaves, master */
typedef struct
	{
	UDP_CMD_HDR MstrHdr;	/* header from master udp */
	PACKET_STATS IdataStat[MAX_SHOES];	/* status of packets from inst to slave */
	PACKET_STATS SlaveStat[MAX_SHOES];	/* status of packets from slave to master */
	PACKET_STATS MmiStat;	/* status of packets from MMI to master */
	PACKET_STATS MstrStat;	/* status of packets from  master to MMI  */
	}	I_MSG_STATS;


/* The Scope Demux Adjust message	*/
/* Is both sent and received by MMI */
typedef struct
	{
	UDP_CMD_HDR MstrHdr;	/* header from master udp */
	INSP_HDR	InspHdr;
	SCOPE_DMX_CAL	DmxCal;
	}	I_MSG_DMX_CAL;

/* Compatability with 1st generation transputer based udp */


typedef struct defect_info_tag
	{	WORD	x;
		WORD	w;
		WORD	val;
		WORD	grade_type;
		WORD	nc;		/* how many A-scans has this flaw been more than nc hits */
		WORD	xdcr_num;	/* which channel got flaw  .. 0-39  */
		/* add 4 bytes to size of defect info on 1/21/98 for flaw sizing */
/*		WORD	tof;	?* delta tof for sizing */
		float	depth;	/* flaw depth in 0.001 inches */
	}	DEFECT_INFO;


#if 0
typedef struct cdpmsg1_tag
	{	
		WORD	seq;
		WORD	mlen;	/* in WORDS ... sizeof(CDPMSG1)/2 */
		WORD	id;
		WORD	status[2];		/* 8 before 4/2/98. Now 2 */
		WORD	rev_cnt;		/* last revolution number */
		WORD	rev_avg_wall;	/* avg wall for last revolution */
		WORD	rev_min_wall;	/* min wall for last revolution */
		WORD	rev_min_xdcr_clk; /* hi byte = xdcr, lo byte = clock */
		WORD	rev_ecc_wall;		/* eccentricity per revolution */
		WORD	rev_max_wall;
		WORD	shoe1_clk;
		WORD	period;
		WORD	total_defect;
		WORD	min_wall_cnt;	/* number of min wall conditions */
		WORD	max_wall_cnt;
		WORD	ecc_cnt;
		WORD	flaw_cnt;
		DEFECT_INFO	min;
		DEFECT_INFO	max;
		DEFECT_INFO	ecc;
		DEFECT_INFO	worst_flaw;	/* not used after 1999 */
		/* NOTE ... UDP only finds 8 defects */
		DEFECT_INFO	f[16]; /* f[0]=lid, f[1]=lod, f[2]=tid, f[3]=tod */
		WORD	idle;
		WORD	pipe_front;
		WORD	pipe_len;
		WORD	spare[7];
		}	CDPMSG1EX;
#endif

/* Make a cdpmsg2ex to fit the mold of other messages sent to the MMI */
/* cdpmsg2 is in config.h */
/* Makes sending routines look similar... hope for easier maintenance and debugging */

#ifdef MMI_CODE
/* DEBUG CDP WITH MMI TO START */

typedef struct cdpmsg1ex_tag
	{	
/*	BYTE DMac[6];	/* dest mac */
/*	BYTE SMac[6];	/* source mac */
/*	WORD len;		/* packet len */
/*	IPXHEADER IpxHdr;
*/
	WORD	seq;
	WORD	mlen;	/* in WORDS ... sizeof(CDPMSG2)/2 */
	WORD	id;				/* 1 */
		WORD	status[2];		/* 8 before 4/2/98. Now 2 */
		WORD	rev_cnt;		/* last revolution number */
		WORD	rev_avg_wall;	/* avg wall for last revolution */
		WORD	rev_min_wall;	/* min wall for last revolution */
		WORD	rev_min_xdcr_clk; /* hi byte = xdcr, lo byte = clock */
		WORD	rev_ecc_wall;		/* eccentricity per revolution */
		WORD	rev_max_wall;
		WORD	shoe1_clk;
		WORD	period;
		WORD	total_defect;
		WORD	min_wall_cnt;	/* number of min wall conditions */
		WORD	max_wall_cnt;
		WORD	ecc_cnt;
		WORD	flaw_cnt;
		DEFECT_INFO	min;
		DEFECT_INFO	max;
		DEFECT_INFO	ecc;
		DEFECT_INFO	worst_flaw;
		/* NOTE ... UDP only finds 8 defects */
		DEFECT_INFO	f[16]; /* f[0]=lid, f[1]=lod, f[2]=tid, f[3]=tod */
		WORD	idle;
		WORD	pipe_front;
		WORD	pipe_len;
		WORD	spare[7];
	} CDPMSG1EX;


typedef struct cdpmsg2ex_tag
	{	
/*	BYTE DMac[6];	/* dest mac */
/*	BYTE SMac[6];	/* source mac */
/*	WORD len;		/* packet len */
/*	IPXHEADER IpxHdr;
*/
	WORD	seq;
	WORD	mlen;	/* in WORDS ... sizeof(CDPMSG2)/2 */
	WORD	id;				/* 2 */
/*	WORD	spare;	/* maintain 32 bit alignment */
	OldconfigREC	cfg;
	OldpipeREC		prec;
	} CDPMSG2EX;


// rac_1
typedef struct cdpmsg1_4x_tag
	{	
/*	BYTE DMac[6];	/* dest mac */
/*	BYTE SMac[6];	/* source mac */
/*	WORD len;		/* packet len */
/*	IPXHEADER IpxHdr;
*/
	WORD	seq;
	WORD	mlen;	/* in WORDS ... sizeof(CDPMSG2)/2 */
	WORD	id;				/* 1 */
		WORD	status[2];		/* 8 before 4/2/98. Now 2 */
		WORD	rev_cnt;		/* last revolution number */
		WORD	rev_avg_wall;	/* avg wall for last revolution */
		WORD	rev_min_wall;	/* min wall for last revolution */
		WORD	rev_min_xdcr_clk; /* hi byte = xdcr, lo byte = clock */
		WORD	rev_ecc_wall;		/* eccentricity per revolution */
		WORD	rev_max_wall;
		WORD	shoe1_clk;
		WORD	period;
		WORD	total_defect;
		WORD	min_wall_cnt;	/* number of min wall conditions */
		WORD	max_wall_cnt;
		WORD	ecc_cnt;
		WORD	flaw_cnt;
		DEFECT_INFO	min;
		DEFECT_INFO	max;
		DEFECT_INFO	ecc;
		DEFECT_INFO	worst_flaw;
		/* NOTE ... UDP only finds 8 defects */
		DEFECT_INFO	f[16]; /* f[0]=lid, f[1]=lod, f[2]=tid, f[3]=tod */
		WORD	idle;
		WORD	pipe_front;
		WORD	pipe_len;
		WORD	spare;
	} CDPMSG1_4X;

typedef struct all_shoe_flaws_tag {  /* ??? check alignment ??? */
		USHORT	x;			/* location of show at time of message */
		USHORT   octant;		/* clock position of shoe at time of message */
		BYTE    id_amp[12];	/* max id amp for each channel of instrument */
		BYTE    od_amp[12];	/* max od amp for each channel of instrument */
	} ALL_SHOE_FLAWS;

typedef struct cdpmsg4x_tag {
	CDPMSG1_4X msg1;
	ALL_SHOE_FLAWS shoe[MAX_SHOES];
	} CDPMSG4EX;

#else

typedef struct cdpmsg1ex_tag
	{	
	BYTE DMac[6];	/* dest mac */
	BYTE SMac[6];	/* source mac */
	WORD len;		/* packet len */
	IPXHEADER IpxHdr;

	WORD	seq;
	WORD	mlen;	/* in WORDS ... sizeof(CDPMSG2)/2 */
	WORD	id;				/* 1 */
		WORD	status[2];		/* 8 before 4/2/98. Now 2 */
		WORD	rev_cnt;		/* last revolution number */
		WORD	rev_avg_wall;	/* avg wall for last revolution */
		WORD	rev_min_wall;	/* min wall for last revolution */
		WORD	rev_min_xdcr_clk; /* hi byte = xdcr, lo byte = clock */
		WORD	rev_ecc_wall;		/* eccentricity per revolution */
		WORD	rev_max_wall;
		WORD	shoe1_clk;
		WORD	period;
		WORD	total_defect;
		WORD	min_wall_cnt;	/* number of min wall conditions */
		WORD	max_wall_cnt;
		WORD	ecc_cnt;
		WORD	flaw_cnt;
		DEFECT_INFO	min;
		DEFECT_INFO	max;
		DEFECT_INFO	ecc;
		DEFECT_INFO	worst_flaw;
		/* NOTE ... UDP only finds 8 defects */
		DEFECT_INFO	f[16]; /* f[0]=lid, f[1]=lod, f[2]=tid, f[3]=tod */
		WORD	idle;
		WORD	pipe_front;
		WORD	pipe_len;
		WORD	spare[7];
	} CDPMSG1EX;

typedef struct cdpmsg2ex_tag
	{	
	BYTE DMac[6];	/* dest mac */
	BYTE SMac[6];	/* source mac */
	WORD len;		/* packet len */
	IPXHEADER IpxHdr;
	WORD	seq;
	WORD	mlen;	/* in WORDS ... sizeof(CDPMSG2)/2 */
	WORD	id;				/* 2 */
/*	WORD	spare;	/* maintain 32 bit alignment */
	OldconfigREC	cfg;
	OldpipeREC		prec;
	} CDPMSG2EX;

// rac_1
typedef struct cdpmsg1_4x_tag
	{	
	BYTE DMac[6];	/* dest mac */
	BYTE SMac[6];	/* source mac */
	WORD len;		/* packet len */
	IPXHEADER IpxHdr;

	WORD	seq;
	WORD	mlen;	/* in WORDS ... sizeof(CDPMSG2)/2 */
	WORD	id;				/* 1 */
		WORD	status[2];		/* 8 before 4/2/98. Now 2 */
		WORD	rev_cnt;		/* last revolution number */
		WORD	rev_avg_wall;	/* avg wall for last revolution */
		WORD	rev_min_wall;	/* min wall for last revolution */
		WORD	rev_min_xdcr_clk; /* hi byte = xdcr, lo byte = clock */
		WORD	rev_ecc_wall;		/* eccentricity per revolution */
		WORD	rev_max_wall;
		WORD	shoe1_clk;
		WORD	period;
		WORD	total_defect;
		WORD	min_wall_cnt;	/* number of min wall conditions */
		WORD	max_wall_cnt;
		WORD	ecc_cnt;
		WORD	flaw_cnt;
		DEFECT_INFO	min;
		DEFECT_INFO	max;
		DEFECT_INFO	ecc;
		DEFECT_INFO	worst_flaw;
		/* NOTE ... UDP only finds 8 defects */
		DEFECT_INFO	f[16]; /* f[0]=lid, f[1]=lod, f[2]=tid, f[3]=tod */
		WORD	idle;
		WORD	pipe_front;
		WORD	pipe_len;
		WORD	spare;
	} CDPMSG1_4X;

typedef struct all_shoe_flaws_tag {  /* ??? check alignment ??? */
		USHORT  x;			/* center line of shoe at time of message */
		USHORT  octant;		/* clock position of shoe at time of message */
		BYTE    id_amp[12];	/* max id amp for each channel of instrument */
		BYTE    od_amp[12];	/* max od amp for each channel of instrument */
	} ALL_SHOE_FLAWS;

typedef struct cdpmsg4x_tag {
	CDPMSG1_4X msg1;
	ALL_SHOE_FLAWS shoe[MAX_SHOES];
	} CDPMSG4EX;

#endif

typedef struct
	{
	SLAVE_HDR	SlvHdr;		/* 116 bytes */
	BYTE	Buf[BUF_SIZE];
	} UDP_CMD;

/* All channels from a shoe are 'peak held' to find the 'qualified'
max and min values in each gate.  Valid peak data is indicated by
bit15 of EchoBit word.
*/
	
typedef struct
	{
	SLAVE_HDR	SlvHdr;		/* 116 bytes */
	PEAK_DATA PeakData;	/* up to 1 sets of qualified data */
	} UDP_SLAVE_DATA;


/************ Command Message Prototypes For Instrument Commands ***********/
/* Instrument commands are from 0- 0xef and are found in instrume.h  */


#ifdef MMI_CODE

/* Structures used by MMI/ Win32/ Visual C++ */

typedef struct
	{
/*... supplied by win32 driver
/*	BYTE DMac[6];	/* dest mac */
/*	BYTE SMac[6];	/* source mac */
/*	WORD len;		/* packet len */
/*	IPXHEADER IpxHdr;	*/
	WORD	Mach;		/* which machine, T: = 0x3a54 */
//	WORD	Slot;		/* Which chassis slot number, 0 = Master UDP */
	BYTE	PAM_Number;	/* which one of the Phase Array Masters to get the command. 1 client connection for each*/
	BYTE	Inst_Number_In_PAM;	/* which instrument in the PAM selected by PAM_Number */
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	WORD	ChnlNum;    /* channel number */
	PACKET_STATS PStat[3];	/* future utilization */
	BYTE	CmdBuf[BUF_SIZE];
	} MMI_CMD;		/* Command from mmi to udp/instrument */

/*
         |--PAM_0-------|--Inst_0...... PAM_Number = 0, Inst_Number_In_PAM = 0
         |              |--Inst_1
         |              |--Inst_7
		 |
MMI -----|--PAM_1-------|--Inst_0
         |              |--Inst_7...... PAM_Number = 1, Inst_Number_In_PAM = 7
		 |--PAM_2....
		 |--PAM_3....


*/


/*  CDP COMMAND STRUCTURE FOR IPX */


typedef struct
	{
/*... supplied by win32 driver
/*	BYTE DMac[6];	/* dest mac */
/*	BYTE SMac[6];	/* source mac */
/*	WORD len;		/* packet len */
/*	IPXHEADER IpxHdr;	*/
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	WORD	spare;
	BYTE	CmdBuf[1024];
	} CDP_CMD;		/* Command from mmi to CDP */

#else

typedef struct
	{
	BYTE DMac[6];	/* dest mac */
	BYTE SMac[6];	/* source mac */
	WORD len;		/* packet len */
	IPXHEADER IpxHdr;
	WORD	Mach;		/* which machine, B: = 0x3a42 */
	WORD	Slot;		/* Which chassis slot number, 0 = Master UDP */
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	WORD	spare;
	PACKET_STATS PStat[3];	/* future utilization */
	BYTE	CmdBuf[BUF_SIZE];
	} MMI_CMD;		/* Command from mmi to udp/instrument */

/*  CDP COMMAND STRUCTURE FOR IPX */


typedef struct
	{
	BYTE DMac[6];	/* dest mac */
	BYTE SMac[6];	/* source mac */
	WORD len;		/* packet len */
	IPXHEADER IpxHdr;	
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	WORD	spare;
	BYTE	CmdBuf[1024];
	} CDP_CMD;		/* Command from mmi to CDP */


#endif

/* Since Ping doesn't work right with cdp, make a pseudo ping command */

typedef struct
	{
	BYTE DMac[6];	/* dest mac */
	BYTE SMac[6];	/* source mac */
	WORD len;		/* packet len */
	IPXHEADER IpxHdr;	
	WORD	seq;
	WORD	mlen;	/* in WORDS ... sizeof(CDPMSG2)/2 */
	WORD	id;				/* 1 */
	WORD	spare[8];
	}	CDP_PSEUDO_PING_CMD;

/* Structure of flaws in flaw list for printing report */

typedef struct
	{
	DWORD	pipenum;	/* 1-65535 pipe number	*/
	short	xloc;		/* in 1 inch increments	*/
	BYTE	radial;		/* omega location in N_SEG units	*/
	BYTE	amp;		/* signal size of flaw in %	*/
	short	wall;		/* wall in 1/1000 thds inch	*/
	BYTE	ChnlType;	/* IS_WALL, IS_LONG, etc.	*/
	BYTE	ChnlNum;	/* 00 - 79 chnl number for flaw	*/
	}	FLAW_REC;


/************  from original truscope project with transputers *********/
	
/* typedef struct Channel_config */
typedef struct Channel_info_tag
	{
	/****
		This info is entered by the operator to govern the operation of
		the system.
	****/
	BYTE id_thold;		/* threshold level for id signals */
	BYTE od_thold;
	BYTE nc_for_id;		/* num of consec amps to qualify for flaw */
	BYTE nc_for_od;
	BYTE nx_for_wall;
	BYTE channel_type;	/* wall/tran/long/oblique */
	BYTE id_pen_num;		/* which chart pen to plot with */
	BYTE od_pen_num;
	BYTE wall_pen_num;		/* which chart pen to plot with */
	BYTE spare;
	short xdcr_offset;		/* how many motion pulses from shoe centerline */
	short TholdWallThds[2];
/*	} CHANNEL_CONFIG; */
	} CHANNEL_INFO;



	typedef struct Chart_tmp_tag
		{
		/* This info is measured and recorded by the 860 */

		WORD last_packet_num;	/* input packet number */

		WORD period;		/* period of rotation in ms */
		WORD rpm;			/* preserve 32 bit boundaried in t800 machines */
		WORD x_loc;			/* x location in motion pulse counts */
		WORD pipe_velocity;	/* use t805 clock to compute pipe speed */
		WORD motion_status;	/* motion bus status.. dir/pipe present/inspect */

		BYTE pen_out[12];	/* value to be ouput to pen # i */
							/* 24 bytes to here */
							
		WORD echo_bits[4];		/* 1 bit for each xdcr, set to one when echo occurs */
								/* 32 bytes to here */
		DEFECT_INFO	min;
		DEFECT_INFO	max;	/* 16 bytes per defect */
		DEFECT_INFO	f[8];	/* f[0]=lid, f[1]=lod, f[2]=tid, f[3]=tod */
							/* 192 bytes to here  */
		
		WORD packets_processed_by_860;
		WORD octant;		/* maintain 32 bit boundaries */
		WORD status_pc104[3];
		WORD status_lamination;	/* lsb set for lamination */
		BYTE id_amp_ok[12];	/* max id amp for each channel of instrument */
		BYTE od_amp_ok[12];	/* max od amp for each channel of instrument */
		/* 04/08/98 wall statistics for exxon */
		WORD	rev_cnt;		/* last revolution number */
		WORD	rev_avg_wall;	/* avg wall for last revolution */
		WORD	rev_min_wall;	/* min wall for last revolution */
		WORD 	rev_min_xdcr_clk; /* hi byte = xdcr, lo byte = clock */
		WORD 	rev_max_wall;

		/* 01/30/98 integrate in flaw depth */
		float id_depth_ok[10];
		float od_depth_ok[10];
		BYTE id_depth_ok_amp[12];	/* amp associated with flaw depth for cal */
		BYTE od_depth_ok_amp[12];
#if 0
		WORD quad_tof[4];	/* min tof by pipe quadrants */
#endif

		short avg_tof[10];	/* avg tof for each channel of instrument */
		BYTE *pen_out_ptr[12];	/* ptr to value to be ouput to pen # i */
		/**** Above is the portion sent to the PC to output to the chart */
		/* 32 bit integers */
		int sum_tof[10];	/* sum of tof for each channel of instrument */
		int tof_num[10];	/* number of tof''s to average */
		BYTE id_amp[12];	/* possible max id amp for each channel of instrument */
		BYTE od_amp[12];	/* possible max od amp for each channel of instrument */
		short	min_tof[10];	/* min tof for each channel of instrument */
		short	max_tof[10];
		BYTE min_wall[12];	/* 8 bit chart scaled value for each channel */
		BYTE max_wall[12];	/* 8 bit chart scaled value for each channel */
		BYTE avg_wall[12];	/* 8 bit chart scaled value for each channel */
		BYTE id_nc[12];		/* num of consec id amp''s above thold per channel */
		BYTE od_nc[12];		/* num of consec id amp''s above thold per channel */
		BYTE id_nc_max[12];	/* biggest nc for this channel */
		BYTE od_nc_max[12];
		short id_w[10];		/* angular location at max signal & more than nc times */
		short od_w[10];
		short id_x[10];		/* longitudinal loc at max signal & more than nc times */
		short od_x[10];
#if 0
		BYTE min_wall_nc[12];
		BYTE max_wall_nc[12];
		CHANNEL_CONFIG chnl_info[10];
#endif
		CHANNEL_INFO chnl_info[10];
		WORD nominal_wall;
		short all_tof_min;	/* min wall from all wall channels */ 
		short all_tof_max;
		short all_tof_avg;
		BYTE all_wall_min;	/* min wall from all wall channels */
		BYTE all_wall_max;
		BYTE all_wall_avg;
		BYTE all_lid;		/* max of all long id''s */
		BYTE all_lod;
		BYTE all_tid;
		BYTE all_tod;
		BYTE all_o2id;		/* max of all oblique id''s */
		BYTE all_o2od;
		BYTE all_o4id;		/* max of all oblique id''s */
		BYTE all_o4od;
		BYTE all_lam;
		BYTE dont_use[4];	/* where pen_ptrs point to on start up... a bit bucket */
		float all_lid_depth;		/* max of all long id''s */
		float all_lod_depth;
		float all_tid_depth;
		float all_tod_depth;
		float all_o2id_depth;		/* max of all oblique id''s */
		float all_o2od_depth;
		float all_o4id_depth;		/* max of all oblique id''s */
		float all_o4od_depth;
		float all_lam_depth;
		}	CHART_TMP;

/* broascast message */
typedef struct
	{
	int   nMustSend;   /* Do the slave must send the peak data? 0=NO, 1=YES */
	int   xloc;        /* distance from front of pipe */
	int   xloc_s2;        /* distance from front of pipe */
	int   tick;        /* 0-29, number of 12 degree segment from top of the pipe */
	} BCAST;

/* A-Scan packet to Master from Slave */
typedef struct
{
	WORD    MsgType;
	WORD	nSlave;
	WORD	AscanDelay;
	WORD	AscanRange;
	char	GateTrigger[MAX_GATE];
	short   GateDelay[MAX_GATE];
	short   GateLevel[MAX_GATE];
	short   GateRange[MAX_GATE];
	short   GateBlank[MAX_GATE];
	BYTE	bRFsignal;   //RF=1; FW=0
	BYTE	bInvert;     //If in RF mode, do we need to invert the signal? YES=1; NO=0
	BYTE	nChannel;    //Channel# of the current A-Scan
	BYTE    RdpStatus; 
	BYTE    Ascan[1024];
} ASCAN_PACKET;

/* resend request from MMI to Master */
typedef struct
{
	DWORD NumMsg;  //number of messages to resend
	DWORD SeqNum[BUF_SIZE/4-1];  //sequence numbers of the messages requested
} MMI_RESEND_REQUEST;

/* structures only used by raw wall system */

#define NUM_WALL_CHANNEL 4
#define NUM_SLAVE_WALL_CHANNEL 4  /* number of wall channels per slave */
#define NUM_MAX_ASCAN    2000   /* maximum number of Ascans per revolution */
#define NUM_MAX_REVOL    2000   /* maximum number of revolutions per joint */
#define MAX_DROP_RATE    20.0f  /* maximum wall drop rate allowed in percent */
#define WALL_HEAD_MSG		0
#define WALL_REVOLUTION_MSG	1
#define WALL_END_MSG		2
#define NUM_WALL_SLAVES		1  /* number of instruments that have at least one wall channel */

/* header of the file holding the raw wall readings for a joint */
typedef struct
{
	JOB_REC JobRec;	     /* work order, customer name, etc. */
	DWORD   nJointNum;   /* joint number */
	float   fWall;       /* nominal pipe wall thickness in inch */
	float   fOD;         /* nominal pipe OD in inch */
	WORD    nNumRev;     /* actual number of revolutions stored in this file */
						 /* no greater than NUM_MAX_REVOL */
	WORD    nMotionBus;  /* Bit 14 (zero-based) is the home or away bit. */
						 /* 1: Home, the end where the clock sensor resides. */
						 /*    The heads are leaving home. */
						 /* 0: Away, the other end. */
						 /*    The heads are going home. */
} RAW_WALL_HEAD;

/* wall readings for one pulse */
typedef struct
{
	WORD    Wall[NUM_WALL_CHANNEL];         /* for all channels  */
} WALL_ASCAN;

/* wall readings for one revolution */
typedef struct
{
	WORD    nNumAscan[NUM_WALL_CHANNEL];    /* actual number of Ascans for this revolution, */
										    /* no greater than NUM_MAX_ASCAN */
	short   nXloc;                          /* x location for this revolution */
	WORD	nMotionBus;						    /* motion bus */
	float   fCrossArea[NUM_WALL_CHANNEL];   /* actual cross-sectional area for this revolution */
	WALL_ASCAN    WallAscan[NUM_MAX_ASCAN]; /* wall readings for this revolution for all wall channels */
} WALL_REVOLUTION;

/* wall data packet to Master from Slave */
typedef struct
{
	WORD    MsgType;
	WORD	RevNum;
	BYTE    Buf[NUM_WALL_CHANNEL*6+4+NUM_MAX_ASCAN*NUM_WALL_CHANNEL*2];
} WALL_DATA_PACKET;


/* amplitude readings for one revolution */
typedef struct
{
	short   nXloc;                          /* x location for this revolution */
	WORD	nMotionBus;						    /* not used */
	BYTE    Amp[2][16][2][180];		 /* [Left/Right][Beam#][ID/OD][Angle], wall readings for this revolution for all wall channels */
} CSCAN_REVOLUTION;



typedef struct
	{
	int nLength;		// number of bytes to send
//	BYTE *pMsg;			// ptr to the message bytes to send.
	BYTE Msg[1];		// ptr to the message bytes to send.
	}	stSEND_PACKET;


/* Only one time thru	*/
#endif

#if 0
Changed MMI cmbbuf size (BUF_SIZE) from 390 to 450 to accomodate
Chnl msg which was 10 bytes too long.


Size of PACKET_STATS is 20
Size of UDP_CMD_HDR is 72
Size of SLAVE_HDR is 116
Size of UDP_CMD is 916
Size of UDP_SLAVE_DATA is 212
Size of INST_DATA is 1282
Size of IPXHEADER is 30
Size of PACKET_DATA is 1328
Size of INST_CMD_DATA is 78
Size of MMI_CMD is 872
Size of UT_SHOE is 80
Size of INSP_HDR is 64
Size of UT_INSP is 1120
Size of I_MSG_RUN is 1256
Size of I_MSG_CAL is 1096
Size of I_MSG_PKT is 396
Size of I_MSG_NET is 936
03/21/2012
#endif
