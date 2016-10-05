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
	03/29/01 Make compatible with Truscope.  Add 2nd Oblique cat, remove
			UT_SHOW structures from CURRENT_READING
	04/03/01 Bias all truscan cmnds by 1000 to make room in 0-140 for
			Truscope instrument commands
*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* Only one time thru	*/
#ifndef UTMSG_H
#define UTMSG_H

#include <time.h>


/* Basic UT message structures   -- generic send and receive buffer formats */


#define UTRON_MACHINE		0x3a43
/* 	0x3a43 = C:	*/

#define TRUSCOPE_MACHINE		0x3a54
/* 	0x3a54 = T:	*/


/* Number of radial segments in image map.  Max is 60	*/

/*#define N_SEG				30 */

/* Max number of protocols to search for ipx and udp */

#define MAX_PROTOCOLS		80

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
#define		IS_OBQ1			16
#define		IS_OBQ1_ID		16
#define		IS_OBQ1_OD		17

#define		IS_LAM 			32

#define		IS_OBQ2			64
#define		IS_OBQ2_ID		64
#define		IS_OBQ2_OD		65

#define		IS_OBQ3			128
#define		IS_OBQ3_ID		128
#define		IS_OBQ3_OD		129


#if INSTRUMENT_CODE
/* instrument messages	*/

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
/* catcher messages	*/

#ifdef OLD_STRUCT_DEFS
typedef struct

	{
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	BYTE	Buf[SND_BUF_SIZE - 20];
	} SENDBUFFER;
#endif



typedef struct
	{
	WORD	Mach;		/* which machine, C: = 0x3a43 */
	WORD	MsgNum;   /* for message acknowledge */
	WORD	MsgLen;
	WORD	MsgId;
	BYTE	Buf[RCV_BUF_SIZE - 20];
	} RECBUF;

#endif

/******************  IPX buffer used by MMI to send Config file info to CDP  ************/


typedef struct

	{
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	BYTE	Buf[SND_TO_CDP_BUF_SIZE - 20];
	} SENDTOCDPBUFFER;


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


//Messages from the ASI Instrument to the CDP or ASIMMI
//#endif


	
/* Typedef's for specific message structures	*/

/* Who builds the messages.  Instrument built messages are I_MSG	*/
/* Catcher/MMI built messages are C_MSG	*/

/* The inspection message	*/
#if 0
typedef struct
	{
	WORD	Mach;
	WORD	Seq;
	WORD	Len;
	WORD	MsgID;		/* 1 */
	INSP_HDR	InspHdr;
	CURRENT_ACTIVITY CurrentActivity;
	UT_INSP	UtInsp;
	}	I_MSG11;

/* The calibration message	*/
typedef struct
	{
	WORD	Mach;
	WORD	Seq;
	WORD	Len;
	WORD	MsgID;	/* 2 */
	INSP_HDR	InspHdr;
	UT_TIME_SAMPLE	UtSample;
	}	I_MSG12;


/* The Raw IPX instrument data message */
typedef struct
	{
	WORD	Mach;
	WORD	Seq;
	WORD	Len;
	WORD	MsgID;	/* 3 */
	INST_DATA InstData;	/* 16 Ascans*10 chnls*2 gates + msg_cnt */
	}	I_MSG13;
#endif

/******************************************************************/
/******************************************************************/
/******************************************************************/


//Messages from the CDP or mMMI to the Truscope Instrument


#define	SET_INSP_MODE		1001
#define	SET_CAL_MODE		1002
#define	SET_ALL_CHNL_TYPES	1003
#define	SET_NCNX			1004
#define SET_FLAW_TOL		1006
#define	SET_SCOPE_SELECT	1007
//#define	SET_DROPOUT			1008
#define SET_RAW_IPX_MODE	1009
#define SET_NO_CLOCK		1011
#define	SET_PIPE_NUM		1018
#define	INST_AUTORUN		1021




/* msg 1.  Tell ASI to send normal inspection data	*/

typedef struct
	{
	WORD	MotionTimeFlag;	/* 0 = mot, 1 = time..default is motion	*/
	}	C_MSG_01;


typedef struct
	{
	BYTE Type;	/* = IS_WALL, IS_LONG, etc.	*/
	short nXOffset;		/* offset from shoe centerline in inches	*/
	short nWOffset;		/* angle offset from shoe centerline in 2 degree	*/
	} CHNL_TYPE;

typedef struct
	{
	CHNL_TYPE Ch[10];
	}	SHOE_TYPE;


/* msg 3.  Set All Chn Types	*/

typedef struct
	{
	SHOE_TYPE ShoeType[4];
	}	C_MSG_03;
	

/* msg 4.  Set All Flaw type Nc's And Wall Nx	*/

typedef struct
	{
	BYTE	Long[2];	/* [0]=id nc, [1] = od nc	*/
	BYTE	Tran[2];
	BYTE	Oblq1[2];
	BYTE	Oblq2[2];
	BYTE	Oblq3[2];
	BYTE	Lamin[2];   /* laminar */
	BYTE	Wall[2];
	BYTE	Tol;		/* Nc tolerance */
	BYTE	spare[1];
	}	C_MSG_NC_NX;
	

/* msg 5.  Set Wall Coefficients slope and offset	*/

typedef struct
	{
	WORD	MsgID;		/* 5	*/
	float	Slope[4];
	short	Offset[4];
	}	C_MSG_05;
	

/* msg 6.  Set Flaw Tolerance for all flaw chnls	*/
/*  flaw if  n out of (n+tol) A-scans above thold	*/

typedef struct
	{
	WORD	Tol;
	}	C_MSG_06;

/* msg 7.  Set Scope Selections	*/

typedef struct
	{
	WORD T1MuxNo;	/* hardware mux number	*/
	WORD T1MuxChnl;	/* channel within that mux	*/
	WORD T2MuxNo;
	WORD T2MuxChnl;
	}	C_MSG_07;


/* msg 8. Set drop out time in ms before allowing wall to drop out	*/

typedef struct
	{
	WORD	DropOut;	/* in ms	*/
	}	C_MSG_08;


/* msg 10. Turn on/off UT channel based on OnOff word	*/

typedef struct
	{
	WORD	ShoeMask[4];		/* bit packed chnl on = 1	*/
	}	C_MSG_10;

/* msg 11.  TURN ON/OFF clock reading.  If off, clock always 0	*/

typedef struct
	{
	WORD	ClockFlag;	/* 0 = no clock, 1 = use clock for map	*/
	}	C_MSG_11;
	
/* msg 18.  Set Pipe Number	*/

typedef struct
	{
	DWORD	JntNum;		/* Joint number for instrument	*/
	}	C_MSG_JNT_NUM;

/* msg 19.  Set All Thresholds	*/

typedef struct
	{
	BYTE	TholdLong[MAX_GATE];	/* thold in %, id = 0, od = 1 for long chnls */
	BYTE	TholdTran[MAX_GATE];	/* thold in %, id = 0, od = 1 for tran chnls */
	BYTE	TholdOblq1[MAX_GATE];	/* thold in %, id = 0, od = 1 for oblq chnls */
	BYTE	TholdOblq2[MAX_GATE];	/* thold in %, id = 0, od = 1 for oblq chnls */
	BYTE	TholdOblq3[MAX_GATE];	/* thold in %, id = 0, od = 1 for oblq chnls */
	BYTE	TholdLamin[MAX_GATE];	/* thold in %, id = 0, od = 1 for laminar (wall) chnls */
	char	TholdWall[MAX_GATE];	/* thold in %, min=0, max= 1 for wall chnls */
	short	TholdWallThds[MAX_GATE];	/* wall tholds in 1/1000th inch incr, min=0 */
	float	fWall;
	float	fOD;
	}	C_MSG_ALL_THOLD;

typedef struct
	{
	WORD	MsgID;		/* 21	*/
	WORD	AutoRun;	/*1=run,0=no ipx msg	*/
	}	C_MSG_21;




/****** User Data Gram (UDP/TCPIP) Messages  *********************/
/*************  UDP Messages to or from the MMI ******************/

typedef struct
	{
	WORD	Seq;
	WORD	Len;		/* 	*/
	WORD	MsgID;		/* 21	*/
	FLAW_REC FlawRec[75];
	}	U_MSG_01;

typedef struct
	{	/*  DEBUGGING MSG for sending ASCII to/from customer	*/
	WORD	Seq;
	WORD	Len;		/* 	*/
	WORD	MsgID;		/* 99	*/
	char	txt[800];
	}	U_MSG_99;




#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*          COPYRIGHT 1999 BY TUBOSCOPE INC.  ALL RIGHTS RESERVED.          /

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


