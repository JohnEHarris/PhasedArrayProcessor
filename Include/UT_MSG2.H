/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*   ut_msg.h
*	adapted from asi_msg.h
*   asi_msg.h
*   adapted from wchekmsg.h from the well chek2 project
*
*   COPYRIGHT 1999 BY TUBOSCOPE INC.  ALL RIGHTS RESERVED.
*
*   07-22-99   MWTaylor
	10-05-99 jeh add time and joint number to packet structures
	11/01/99 reorganize for amalog/sonoscope/isolog from one cpu instrument
	12/14/99 make tamsa look like alliance small tube amalog.  Time sample
			now like alliance rawdata structure.
	12/16/99 add max sig/chnl for each shoe and band to insp_ama structure
	01/13/00 Show pckts received/lost/dup on instrument in insp header.
	01/18/00 Add Line status to sono/iso time msg.  Change name from 
			bTrueStatus to bLineStatus.
	01/19/00 Change structure names insp_xxx to xxx_insp..  Change xloc
			from long to short
	020100 Begin to define msg 15, the config file msg
	02/29/00 printed report flaw structure for plist.
	03/20/00 All filter values for a machine  set by one message
	03/27/00 Atuorun cmnd (#21)
	03/31/00 CHANGE bLineStatus to wLineStatus
	04/06/00 Add Iso max/min reading per segment, change from word to byte
				Change number of samples in time based cal from 10 to 2
	05/02/00 add linestatus to insp_hdr.  Replace status[3] with line status.
	05/11/00 Add tcpip/udp message data structure
	05/16/00 Change to Utron messages from asi
	06/01/00 Move N_SEG literal to here.  Have to limit to 30 seg's for
			Image buf to compile in compact model for 16 bit code
	06/09/00	Conceptual change in project (Clive).  Utron row is
			now a shoe.  Do not support 2 banks or Utron shoes.
	07/14/00 Add chnl number to ut_insp structure
	07/26/00 Add favg of wall max/min and ecc measurement to Inspect msg
	10/23/00 Add no clock message
	11/18/00 change wall cal from slope/offset to set of 4 slope/offsets
	11/28/00 Change current reading structure to show peak held chnl info
			for all chnls
	12/15/00 Add additional wall avg stat variables, chang pipenum to dword
	04/20/01 Msg 22, wall range for stavely, 1 inch and 5 inch scales.
	05/23/01 Software ID gains for each chnl
	05/25/01 My mistake, use OD gains instead of Id gains.
*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


// Only one time thru
#ifndef UTMSG_H
#define UTMSG_H

#include <time.h>


/* Basic UT message structures   -- generic send and receive buffer formats */


#define UTRON_MACHINE		0x3a43
// 	0x3a43 = C:


// Number of radial segments in image map.  Max is 60

#define N_SEG				30


/****** UTRON/TRUSCOPE Channel Types *************************/


/**************************************************/
/************  Channel Types **********************/
/**************************************************/

#define		IS_NOTHING		0

#define		IS_WALL			2
#define		IS_WALL_MIN		2
#define		IS_WALL_MAX		3

#define		IS_LONG			4
#define		IS_LONG_ID		4
#define		IS_LONG_OD		5

#define		IS_TRAN			8
#define		IS_TRAN_ID		8
#define		IS_TRAN_OD		9

/* 10/03/95 obliques less than 23 degrees */
#define		IS_OBQ2			16
#define		IS_OBQ2_ID		16
#define		IS_OBQ2_OD		17

#define		IS_LAM 			32

/* 10/03/95 obliques greater than 23 degrees, ... ie., 45's*/
#define		IS_OBQ4			64
#define		IS_OBQ4_ID		64
#define		IS_OBQ4_OD		65

#if INSTRUMENT_CODE
// instrument messages

typedef struct
	{
	WORD	Mach;		/* which machine, B: = 0x3a42 */
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	BYTE	Buf[SND_BUF_SIZE - 20];
	} SENDBUFFER;


typedef struct
	{
	WORD	MsgNum;   /* for message acknowledge */
	WORD	MsgLen;
	WORD	MsgId;
	BYTE	Buf[RCV_BUF_SIZE - 20];
	} RECBUF;

#else
// catcher messages

typedef struct
	{
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	BYTE	Buf[SND_BUF_SIZE - 20];
	} SENDBUFFER;



typedef struct
	{
	WORD	Mach;		/* which machine, C: = 0x3a43 */
	WORD	MsgNum;   /* for message acknowledge */
	WORD	MsgLen;
	WORD	MsgId;
	BYTE	Buf[RCV_BUF_SIZE - 20];
	} RECBUF;

#endif

/************** UDP Message format.. Same for send and Receive ********/
typedef struct
	{
	WORD	Seq;   /* for message acknowledge */
	WORD	Len;
	WORD	MsgId;
	BYTE	Buf[1000];
	} UDPMSGBUF;



/******************************************************************/
/******************************************************************/
/******************************************************************/

#if 0
Messages from the ASI Instrument to the CDP or ASIMMI
#endif


/** Truscope uses a byte per gate.. Word here **/
typedef struct
	{
	WORD g[2];		// 0 = id, 1 = od
	}	UT_CHNL;	// 2 gates in a chnl


typedef struct
	{
	UT_CHNL Ch[10];
	}	UT_SHOE;

typedef struct 
	{
	WORD	status[3];
	WORD	wLineStatus;	// status at time msg sent
	long	idle_cnt;
	time_t	time;
	short	motion_pulse_per_sec;
	short	xloc;
	WORD	JointLength;	// in motion pulses
	DWORD	joint_number;	//12/15/00 to dword from word
	WORD	Rpm;		// period of rotation in 0.1 ms
	WORD	TotalCmnds;	// number of cmnd packet received by instrument
	WORD	PacketsLost;
	WORD	PacketsDup;	// Duplicate packet seq numbers
	} INSP_HDR;

typedef struct 
	{
	short	xloc;
	WORD	wLineStatus;	// amalog status at time data assembled
	BYTE	ShoeOn[4];		// bit0 = 0 -> shoe off, bit7 = 1 ->sig> thold this shoe
	WORD	ChnlOn[4];	// bit mapped for each chnl. 0 = chnl off
	ECHO_BITS	EchoBit;	// bit mapped for each chnl. 0 = no echo
	UT_SHOE Sh[4];
	UT_SHOE ShWall[4];		// wall instead of flaw. g0=min, g1=max
	} CURRENT_READING;

typedef struct 
	{
	short	xloc;			// one inch array of segments location
	WORD	wLineStatus;	// status at time data assembled
	WORD	wIrqTime;		// clock ticks used to complete interrupt

	// Radial segment info follows
	// This data is aligned Radially and longitudinally ( x & omega)

	BYTE	MaxLodFlaw;		// max of the LODFlaw array for graph plot
	BYTE	MaxLidFlaw;		// max of the LIDFlaw array
	BYTE	MaxTodFlaw;
	BYTE	MaxTidFlaw;
	BYTE	MaxQodFlaw;
	BYTE	MaxQidFlaw;
	short	MinWall;		// min of all SegWall values
	short	MaxWall;		// max of all SegWall values
	short	MaxEcc;			// max eccentricity in this inch band
	short	AvgWall;		// avg of all min & max in SegWall within +/- 50% nominal
	short	AvgMinWall;		// avg of all min wall readings within +/- 50% nominal
	short	AvgWall10;		// avg of all wall readings within +/- 10% nom
	short	AvgEcc;			// avg of all eccentricity reading in this band
	short	spare2[2];		// for the future
	BYTE	SegLODFlaw[N_SEG]; // 0 -0xff flaw amp for Long OD
	BYTE	SegLIDFlaw[N_SEG]; // 0 -0xff flaw amp for Long ID
	BYTE	SegTODFlaw[N_SEG]; // 0 -0xff flaw amp for Tran OD
	BYTE	SegTIDFlaw[N_SEG]; // 0 -0xff flaw amp for Tran ID
	BYTE	SegQODFlaw[N_SEG]; // 0 -0xff flaw amp for Oblq OD
	BYTE	SegQIDFlaw[N_SEG]; // 0 -0xff flaw amp for Oblq ID
	short	SegWallMin[N_SEG];	// min wall for each 12 degrees
	short	SegWallMax[N_SEG];	// max wall for each 12 degrees
	BYTE	SegLODChnl[N_SEG]; // chnl number for Long OD
	BYTE	SegLIDChnl[N_SEG]; // chnl number for Long ID
	BYTE	SegTODChnl[N_SEG]; // chnl number for Tran OD 
	BYTE	SegTIDChnl[N_SEG]; // chnl number for Tran ID 
	BYTE	SegQODChnl[N_SEG]; // chnl number for Oblq OD 
	BYTE	SegQIDChnl[N_SEG]; // chnl number for Oblq ID 
	BYTE	SegMinChnl[N_SEG]; // chnl number for Wall Min
	BYTE	SegMaxChnl[N_SEG]; // chnl number for Wall Max
	} UT_INSP;
	
typedef struct
	{
	short	nXencoder;		// encoder counts at A/D conversion time
	short	nAngleBody1;	// location of shoe1 in 2 degree increments
	WORD	wLineStatus;
	WORD	wIrqTime;		// clock ticks to complete interrupt
	BYTE	ShoeOn[4];		// bit0 = 0 -> shoe off, bit7 = 1 ->sig> thold this shoe
	WORD	ChnlOn[4];	// bit mapped for each chnl. 0 = chnl off
	WORD	EchoBit[4];	// bit mapped for each chnl. 0 = no echo
	BYTE	MaxLodFlaw;		// curren max value being peak held between messages
	BYTE	MaxLidFlaw;		// max of the LIDFlaw array
	BYTE	MaxTodFlaw;
	BYTE	MaxTidFlaw;
	BYTE	MaxQodFlaw;
	BYTE	MaxQidFlaw;
	short	MinWall;		// min of all SegWall values
	short	MaxWall;		// max of all SegWall values
	UT_SHOE Sh[4];
	UT_SHOE ShWall[4];		// wall instead of flaw. g0=min, g1=max
	BYTE	bSkip4[32];
	} UT_TIME_SAMPLE;



/* Structure of flaws in flaw list for printing report */

typedef struct
	{
	DWORD	pipenum;	// 1-65535 pipe number
	short	xloc;		// in 1 inch increments
	BYTE	radial;		// omega location in N_SEG units
	BYTE	amp;		// signal size of flaw in %
	short	wall;		// wall in 1/1000 thds inch
	BYTE	ChnlType;	// IS_WALL, IS_LONG, etc.
	BYTE	ChnlNum;	// 00 - 79 chnl number for flaw
	}	FLAW_REC;

	
// Typedef's for specific message structures

// Who builds the messages.  Instrument built messages are I_MSG
// Catcher/MMI built messages are C_MSG

// The inspection message
typedef struct
	{
	WORD	Mach;
	WORD	Seq;
	WORD	Len;
	WORD	MsgID;
	INSP_HDR	InspHdr;
	CURRENT_READING	CurrentValue;
	UT_INSP	UtInsp;
	}	I_MSG11;

// The calibration message
typedef struct
	{
	WORD	Mach;
	WORD	Seq;
	WORD	Len;
	WORD	MsgID;
	INSP_HDR	InspHdr;
	UT_TIME_SAMPLE	UtSample;
	}	I_MSG12;


/******************************************************************/
/******************************************************************/
/******************************************************************/

#if 0
Messages from the CDP or ASIMMI to the ASI Instrument
#endif

#define	SET_INSP_MODE		1
#define	SET_CAL_MODE		2
#define	SET_ALL_CHNL_TYPES	3
#define	SET_NCNX			4
#define	SET_WALL_COEFS		5
#define SET_FLAW_TOL		6
#define	SET_SCOPE_SELECT	7
#define	SET_DROPOUT			8
#define SET_ALL_GAINS		9
#define SET_NO_CLOCK		11
#define	SET_PIPE_NUM		18
#define	SET_ALL_THOLDS		19
#define	INST_AUTORUN		21
#define	SET_WALL_RANGE		22

// msg 1.  Tell ASI to send normal inspection data

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 2
	WORD	MsgID;		// 1
	WORD	MotionTimeFlag;	// 0 = mot, 1 = time..default is motion
	}	C_MSG_01;

// msg 2.  Tell ASI to send time based data

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 1
	WORD	MsgID;		// 2
	}	C_MSG_02;

typedef struct
	{
	BYTE Type;	// = IS_WALL, IS_LONG, etc.
	short nXOffset;		// offset from shoe centerline in inches
	short nWOffset;		// angle offset from shoe centerline in 2 degree
	} CHNL_TYPE;

typedef struct
	{
	CHNL_TYPE Ch[10];
	}	SHOE_TYPE;


// msg 3.  Set All Chn Types

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 
	WORD	MsgID;		// 3
	SHOE_TYPE ShoeType[4];
	}	C_MSG_03;
	

// msg 4.  Set All Flaw type Nc's And Wall Nx

typedef struct
	{
	WORD	Seq;
	WORD	Len;
	WORD	MsgID;		// 4
	BYTE	Long[2];	// [0]=id nc, [1] = od nc
	BYTE	Tran[2];
	BYTE	Oblq[2];
	BYTE	Wall[2];
	BYTE	Tol;		/* Nc tolerance */
	BYTE	spare[3];
	}	C_MSG_04;
	

// msg 5.  Set Wall Coefficients slope and offset

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 
	WORD	MsgID;		// 5
	float	Slope[4];
	short	Offset[4];
	}	C_MSG_05;
	

// msg 6.  Set Flaw Tolerance for all flaw chnls
//  flaw if  n out of (n+tol) A-scans above thold

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 2
	WORD	MsgID;		// 6
	WORD	Tol;
	}	C_MSG_06;

// msg 7.  Set Scope Selections

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 5
	WORD	MsgID;		// 7
	WORD T1MuxNo;	// hardware mux number
	WORD T1MuxChnl;	// channel within that mux
	WORD T2MuxNo;
	WORD T2MuxChnl;
	}	C_MSG_07;


// msg 8. Set drop out time in ms before allowing wall to drop out

typedef struct
	{
	WORD	Seq;
	WORD	Len;		
	WORD	MsgID;		// 8
	WORD	DropOut;	// in ms
	}	C_MSG_08;

// msg 9. Set All Id gains.  Software scalar of chnl reading

typedef struct
	{
	WORD	Seq;
	WORD	Len;		
	WORD	MsgID;		// 9
	short	OdGain[40];
	}	C_MSG_09;

// msg 10. Turn on/off UT channel based on OnOff word

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 5
	WORD	MsgID;		// 10
	WORD	ShoeMask[4];		// bit packed chnl on = 1
	}	C_MSG_10;

// msg 11.  TURN ON/OFF clock reading.  If off, clock always 0

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 2
	WORD	MsgID;		// 11
	WORD	ClockFlag;	// 0 = no clock, 1 = use clock for map
	}	C_MSG_11;
	
// msg 18.  Set Pipe Number

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 2
	WORD	MsgID;		// 18
	DWORD	JntNum;		// Joint number for instrument
	}	C_MSG_18;

// msg 19.  Set All Thresholds

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 6
	WORD	MsgID;		// 14
	BYTE	TholdLong[2];	/* thold in %, id = 0, od = 1 for long chnls */
	BYTE	TholdTran[2];	/* thold in %, id = 0, od = 1 for tran chnls */
	BYTE	TholdOblq[2];	/* thold in %, id = 0, od = 1 for oblq chnls */
	char	TholdWall[2];	/* thold in %, min=0, max= 1 for wall chnls */
	short	TholdWallThds[2];	/* wall tholds in 1/1000th inch incr, min=0 */
	float	fWall;
	float	fOD;
	}	C_MSG_19;

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 2
	WORD	MsgID;		// 21
	WORD	AutoRun;	//1=run,0=no ipx msg
	}	C_MSG_21;

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 2
	WORD	MsgID;		// 22
	WORD	WallRange;	// 1= 1 inch, 5 = 5 inch.  Multiplier for wall thick
	}	C_MSG_22;




/****** User Data Gram (UDP/TCPIP) Messages  *********************/
/*************  UDP Messages to or from the MMI ******************/

typedef struct
	{
	WORD	Seq;
	WORD	Len;		// 
	WORD	MsgID;		// 21
	FLAW_REC FlawRec[75];
	}	U_MSG_01;

typedef struct
	{	//  DEBUGGING MSG for sending ASCII to/from customer
	WORD	Seq;
	WORD	Len;		// 
	WORD	MsgID;		// 99
	char	txt[800];
	}	U_MSG_99;

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*          COPYRIGHT 1999 BY TUBOSCOPE INC.  ALL RIGHTS RESERVED.          /

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


