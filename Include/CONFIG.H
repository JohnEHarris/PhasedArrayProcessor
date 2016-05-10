/*
config.h

Date:	07/26/99
Revised:
		07/30/99 Change split record
		01/13/2000 Change from Wellcheck to ASI for Tamsa.
			 Uses CFP boards and old electronics + NDT hardware
		02/16/00 make sono record almost same as amalog.  Except, no
				diff gain mode.
		03/08/00 Add a replay record structure which is a global 'c'
				variable to keep track of last pipe replayed.
		03/28/00 Define scope_entries
		03/31/00 move line status bits from hardware.h to here
		05/19/00 copied from asi project to utron/truscan project
		05/22/00 INsert channel_config structure from truscope
		08/08/00 Typedef for IpxStats
		10/30/00 Echo bits for xdcr activity in time
		11/18/00 change wall cal from slope/offset to set of 4 slope/offsets
		12/18/00 Add wall stat structures for Exxon and Tubo Statistics

		08/22/01 REPLACE GLOBAL.H in Truscope project with this file.
				Add struct elements for instrument from global.h in
				d:\tvi\include into this file.
		
				
Gather configuration file definition into one structure
*/


/* Only one time thru	*/

#ifndef CONFIG_H

#define CONFIG_H

/* The global declaration may be used in non-windows system which has not 
   BOOL type system declarations, so keep these typedef for convenience
*/

#if INSTRUMENT_CODE

#define MAX_CHANNEL        10

#else
/* These apply only to the UDP/ Operator Interface */

#define MAX_CHANNEL        20
#endif

/*
Memory structures are based on a max channel dimension.  After 03/28/95 let
MAX_CHANNEL	 refer to the number of channels provided in a system.
MEM_MAX_CHANNEL will set the size of memory structures and will always (?)
be 40.  Systems with less than 40 channels will be structured architecturally
like a 40 channel system.  This will insure that object such as the config
files will have the same size no matter what the system implementation size
is, ie. the config file for a ten channel system will occupy the same number
of bytes as a config file for a 40 channel system.
*/

#define MEM_MAX_CHANNEL		40



#define MAX_DELTA_WALL		20

#define SCOPE_MUX_STD		0
#define SCOPE_MUX_ABS		1
#define SCOPE_MUX_DIF		2


/* Scope selection text and mux info */
#define SCOPE1_MUX		0
#define SCOPE2_MUX		1

#define ABS1_MUX		2
#define ABS2_MUX		3

#define DIF1_MUX			4
#define DIF2_MUX			5

#define LOG1_MUX		6
#define LOG2_MUX		7


/************************************************************/

#if 0

#define	PIPE_PRESENT			(WORD) 0x0008 IN GLOBAL.H
#define	PIPE_ENABLE				(WORD) 0x0008
#define	PC1_BLOCKED				(WORD) 0x020
#define BUF_FLUSH				(WORD) 0x4000

#define	INSPECT_ENABLE_UT_12	(WORD) 0x0010
#define	INSPECT_ENABLE_UT_34	(WORD) 0x0020

#define	HOME					(WORD) 0x8000
#define	AWAY					0

//#define	FORWARD					(WORD) 0x0004
#define	REVERSE					0
#define	MOTION_BASED			0x100
#define	TIME_BASED				   0
#define	FLAW_MASK				0x8
#define	TOP						1

#endif

/************************************************************/

#define WALL_OPT_NONE			0
#define WALL_OPT_EXXON			1
#define WALL_OPT_TUBO			2
#define WALL_OPT_FUTURE			3


/* Scope demux base address */
#define SCOPE_DEMUX_BASE_ADDR	0x310
#define RANGEMIN				1
#define RANGEMAX				5000
#define BLANKMIN				1
#define BLANKMAX				250
#define PRFMIN					1
#define PRFMAX					1000
#define PDMIN					40
#define PDMAX					40
#define PFWIDMIN				1
#define PFWIDMAX				15
/**************************************************/
/**********  Scroll Bar Limits  *******************/
/**************************************************/

#define GAINMIN				-200
#define GAINMAX				600
#define BARLEFTX				173
#define BARGAINY				120
#define BARSIGY				110
#define BARCHNSP				35
#define BARMAXX				240
#define GAINSCALE				((GAINMAX / BARMAXX) + 1)
#define BARRIGHTX				592
#define SIGSCALE				1			/* WILL BE CHANGED LATE */
#define REJECTMIN				0
#define REJECTMAX				100
#define DELAYMIN				1
#define DELAYMAX				5000

#define LEVEL1MIN				5
#define LEVEL1MAX				100
/* / gate 1 can have different thoold limits from gate 2 */
#define LEVEL2MIN				5
#define LEVEL2MAX				100
/* Scope demux base address */
#define SCOPE_DEMUX_BASE_ADDR	0x310
#define RANGEMIN				1
#define RANGEMAX				5000
#define BLANKMIN				1
#define BLANKMAX				250
#define PRFMIN					1
#define PRFMAX					1000
#define PDMIN					40
#define PDMAX					40
#define PFWIDMIN				1
#define PFWIDMAX				15

#define ULC_CONNECT			0x01
#define UDP_CONNECT			0x02

#define CHART_DATA_HEADER		8192
#define CHART_DATA_HEADER_L		8192L


#define NSSH				1
#define MAV_CONROE			2
#define SHELDON				3
#define SOS					4
#define NOVA_HUT			5
#define NAVASOTA			6

#define IS_ONE				1


// SOS Project

#define CUSTOMER			SOS

#if CUSTOMER==SOS
#define REV1997_2CHNL_BD		IS_ONE
#define REV1997_SCOPE_MUX		IS_ONE
#endif


#if CUSTOMER==NOVA_HUT
#define REV1997_2CHNL_BD		IS_ONE
#define REV1997_SCOPE_MUX		IS_ONE
#define ENABLE_LAMINATIONS		IS_ONE
#endif

/***********************************************/
/**** Pulser Control Defines  ******************/
/***********************************************/
#define PLS_ON_BIT			0x40
#define XMT_BITS			0X22
#define RCV_BITS			0X11

typedef struct
	{
	char CfgFile[128];
	char WO[10];			/* work order number */
	char Grade[10];
	char LotNum[10];
	char Heat[34];			/* may not be used */
	char Customer[32];
	char Operator[10];
	char BusUnit[32];		/* which Tubo business unit */
	char Comment[80];
	char Version[16];		/* software version number */
	WORD VerNum;			/* version num * 100 */
	float Wall;
	float OD;
	char Location[40];
	char WellName[40];
	char Date[10];
	char Range[8];
	char WellFoot[12];
	BYTE WallStatFlag;		/* 0=no wall stats, 1=Exxon wall stats */
	BYTE spare[113];
	} JOB_REC;


	/****
		This info is entered by the operator to govern the operation of
		the system.  Taken from Truscope 5-20-00 and modified. jeh

		ALL short gains in 1/10th of db!
		
	****/


typedef struct 
	{
	
	BYTE Type;			/* wall/tran/long/oblique/none */
	BYTE On;			/* 0 = off, 1 = on  */
	BYTE udp_mult;
	BYTE spare;
	char cXOffset;		/* how many motion pulses from shoe centerline */
	char cWOffset;		/* how many 2 degree incr from shoe centerline */
	short Gain[2];		/* id=0, od=1 for receiver gain */
	} CHANNEL_CONFIG;


typedef struct
	{
	CHANNEL_CONFIG Ch[4][10];	/* by shoe and channel */
	char cClockOffset;
	BYTE spare[3];
	}	CHANNEL_CONFIG2;
	
typedef struct
	{
	CHANNEL_CONFIG Ch[10];	/* up to 10 chnls per shoe */
	short	LGain[2];		/* Long Gain of this shoe 0 = ID, 1 = OD */
	float	fLGain[2];		/* Gain of this shoe as floating pt */
	short	TGain[2];		/* Gain of this shoe 0 = ID, 1 = OD */
	float	fTGain[2];		/* Gain of this shoe as floating pt */
	short	Q1Gain[2];		/* Gain of this shoe 0 = ID, 1 = OD */
	float	fQ1Gain[2];		/* Gain of this shoe as floating pt */
	short	Q2Gain[2];		/* Gain of this shoe 0 = ID, 1 = OD */
	float	fQ2Gain[2];		/* Gain of this shoe as floating pt */
	short	Q3Gain[2];		/* Gain of this shoe 0 = ID, 1 = OD */
	float	fQ3Gain[2];		/* Gain of this shoe as floating pt */
	short	WGain[2];		/* Wall Gain of this shoe 0 = ID, 1 = OD */
	float	fWGain[2];		/* Gain of this shoe as floating pt */
	short	ShoeOffset;		/* offset in motion pulses of center line from  */
							/* entry photocell */
	short	ShoeWOffset;	/* Offset in 2 degrees counts from reference angle */
	BYTE	Gmode;			/* Gain adj mode, ALWAYS  1= proportional */
	BYTE	On;				/* 1= SHOE turned on, 0 = SHOE turned off */
	BYTE	CurrentCh;		/* which chnl of this shoe is being displayed */
	BYTE	Spare[7];
	}	SHOE_CONFIG;
	

	
typedef struct
	{
	SHOE_CONFIG	Shoe[4];
	BYTE		NcLong[2];	/* id = 0, od = 1.  Same nc for all channels long */
	BYTE		NcTran[2];	/* id = 0, od = 1.  Same nc for all channels tran */
	BYTE		NcOblq1[2];	/* id = 0, od = 1.  Same nc for all channels oblq */
	BYTE		NcOblq2[2];	/* id = 0, od = 1.  Same nc for all channels oblq */
	BYTE		NcOblq3[2];	/* id = 0, od = 1.  Same nc for all channels oblq */
	BYTE		NxWall[2];	/* min = 0, max =  1.  Same nx for all wall */
	BYTE		TholdLong[2];	/* thold in %, id = 0, od = 1 for long chnls */
	BYTE		TholdTran[2];	/* thold in %, id = 0, od = 1 for tran chnls */
	BYTE		TholdOblq1[2];	/* thold in %, id = 0, od = 1 for oblq chnls */
	BYTE		TholdOblq2[2];	/* thold in %, id = 0, od = 1 for oblq chnls */
	BYTE		TholdOblq3[2];	/* thold in %, id = 0, od = 1 for oblq chnls */
	char		TholdWall[2];	/* thold in %, min=0, max= 1 for wall chnls */
	BYTE		Tol;			/* same tolerance for all flaw channels */
	BYTE		CurrentChnl;	/* which chnl being operated on */
	short		TholdWallThds[2];	/* wall tholds in 1/1000th inch incr, min=0 */
	short	LGain[2];		/* Global Long Gain  0 = ID, 1 = OD */
	float	fLGain[2];		/* Global Gain  as floating pt */
	short	TGain[2];	
	float	fTGain[2];	
	short	Q1Gain[2];	
	float	fQ1Gain[2];	
	short	Q2Gain[2];	
	float	fQ2Gain[2];	
	short	Q3Gain[2];	
	float	fQ3Gain[2];	
	short	WGain[2];	
	float	fWGain[2];	
	BYTE	bOdCalChk;
	BYTE	bIdCalChk;	/* cal screen, is Id chked */
	BYTE	bCalTypes;	/* cal screen, which of Long/Tran/Obq selected */
	BYTE	bCalLvlOd;	/* 0-100 % cal level for od flaws */
	BYTE	bCalLvlId;	/* 0-100 % cal level for id flaws */
	BYTE	bClockFlag;		/* TOP photo cell good/bad */
	/*  Wall Calibration Variables */
	short	WallThkRef;		/* thick cal reference in 1/1000 thds */
	short	WallThnRef;		/* thin  cal reference in 1/1000 thds */
	short	WallCalChnl;	/* 0-39 chnl num of cal chnl */
	float	fWallSlope[4];
	short	WallOffset[4];		/* offset in 1/1000thds inch */
	short	nDropOutMs;		/* keep wall from drop out for this # of ms */

	char trigger_a;
	char trigger_b;
	char trigger;
	char sequence_length;
	char trigger_select[MEM_MAX_CHANNEL];
	BYTE DisplayChannel[12];	// [0] = wall, [1] = long
							// value = 0xff is all, else chnl number displayed
	WORD SyncControl[4];	/* Control operation of sync boards */
	float CompVelocity;			/* speed of sound in this material */
	float ShearVelocity;

	char cClockOffset;		/* -6 to + 6 */
	BYTE	Spare[21];
	
	}	UT_REC;

/**** Note:
Any signal below threshold or that occurs less than nc out of nc+tol times
is reported as 0 by the DSP stage processing unit.
THESE STRUCTURES FOR UTRON-2 SYSTEM
****/

typedef struct
	{
	float	fWallSlope[4];
	short	WallOffset[4];		/* offset in 1/1000thds inch */
	float CompVelocity;			/* speed of sound in this material */
	float ShearVelocity;
	}	WALL_COEF;


/* These parameters are stored in the registry for the MMI and sent each time a config
	file is sent.  The values vary from one installation to another.
*/

typedef struct
	{
	short	nDefaultXOffset;		/* Adjust flaw x location for all shoes by this amount */
	short	nDefaultWOffset;		/* -45 or +45 ticks, depending on rotation direction of
										Truscope.  This is the angle separating shoes
										-/+ 90 degrees = -/+45 ticks
									*/
	char	nDefaultClockOffset;	/* -6 to +6 clock adj for reporting flaw loc */
	BYTE	bRotClkCountsUp;
	BYTE	spare[16];
	}	SITE_SPECIFIC_DEFAULTS;


typedef struct
	{
	WORD T1Button;	/* in MMI, which radio button selected */
	WORD T1Indx;	/* combo box selected item */
	WORD T1MuxNo;	/* hardware mux number */
	WORD T1MuxChnl;	/* channel within that mux */
	WORD T2Button;
	WORD T2Indx;
	WORD T2MuxNo;
	WORD T2MuxChnl;
	char chan;
	char pc104_debug;		/* enable/inhibit pc104 crt output */
	BYTE rf_baseline[4];	/* 4 insturments.... 40 channels */
	BYTE fw_baseline[4];		/* demux dc offset for fw signals */
	BYTE trace1[MEM_MAX_CHANNEL];
	BYTE trace2[MEM_MAX_CHANNEL];
	BYTE trigger[MEM_MAX_CHANNEL];	/* actual value written to demux port */
	BYTE trigger_index[MEM_MAX_CHANNEL];	/* index into combo box for trigger selection */
	int reject_pcnt;	/*operational reject level for new scope demux board */
	BYTE T2GateOrSync[MEM_MAX_CHANNEL];	/* what is on trace 2 */
	BYTE SeqPtTrigger;	/* trigger on pt 1 or 2.. data 0/1 */
	BYTE NormalOrDiagnostic;	// 0 = normal, 1 = diagnostic
	BYTE spare[14];
	} OSCOPE_REC;

typedef struct
	{
	char	Txt[20];
	BYTE	MuxNo;
	BYTE	MuxChnl;
	}	SCOPE_ENTRY;

typedef struct
	{
	int Entries;
	SCOPE_ENTRY se[1];
	}	SCOPE_MUX_PROTO;


/*
THESE STRUCTURES FOR NEW UDP - TRUSCOP2 WITH COMPACT PCI DEMUX BOARD
*/


typedef struct
	{
	char	*Txt;	/* what text string to put in combo box */
	BYTE	bIndx;	/* index of the string in this structure */
	BYTE	bSelect;	/* what conditions make cmd available */
	}	TRUSCOP2_SCOPE_ENTRY;


typedef struct
	{
	int Entries;
	TRUSCOP2_SCOPE_ENTRY se[1];
	}	TRUSCOP2_SCOPE_MUX_PROTO;


#if 0
typedef struct
	{
	short PhotoX[8];			/* photocell location */
	short EncoderX[4];			/* encoder location relative to pc-0 */
								/* in 0.5 inch increments */
	short EncoderDiv[4];		/* divider for each encoder */
	short PaintX[8];			/* loc of each gun */
	short PaintClock[8];		/* o'clock loc of each gun */
	short StripeLen[8];			/* in encoder pulses */
	short StripePattern[8];
	short Bank1Offset;			/* inches from entry photo cell */
	short Bank2Offset;			/* inches from entry photo cell */
	BYTE spare[64];
	}	LINE_REC;
	
#endif



/**********************************/
/* The receiver control structure */
/**********************************/


typedef struct
	{
	short gain[MEM_MAX_CHANNEL];
#if 0	
		short amp1[MEM_MAX_CHANNEL];	/* not currently used */
		short amp2[MEM_MAX_CHANNEL];
		short tof1[MEM_MAX_CHANNEL];
		short tof2[MEM_MAX_CHANNEL];

		char detectoR[MEM_MAX_CHANNEL];
		char filter[MEM_MAX_CHANNEL];
		char flow_cnt[MEM_MAX_CHANNEL]; /* FLAW COUNTER PER CHANNEL */
#endif
	char filter[MEM_MAX_CHANNEL];
	char reject[MEM_MAX_CHANNEL];
	char polarity[MEM_MAX_CHANNEL]; /* POLARITY by channel */
	char det_option[MEM_MAX_CHANNEL];
	char fil_option[MEM_MAX_CHANNEL];

	/* testing the dac circuitry and prototyping */
	short arg1[MEM_MAX_CHANNEL];	/* arg1*time gain factor */
	short arg2[MEM_MAX_CHANNEL];	/* arg2*t*t gain factor, 2nd order term */
	BYTE tcg_step[MEM_MAX_CHANNEL];	/* 200/400/600/800 ns steps */
	BYTE tcg_trigger[MEM_MAX_CHANNEL];	/* no, IP, g1, g2, IE, slaveA/B */
	BYTE tcg_enable[MEM_MAX_CHANNEL]; /* enable by individual channel */
	BYTE tcg_function[MEM_MAX_CHANNEL]; /* gain function by channel */
	BYTE tcg_system;				/* System on/off flag */
	BYTE tcg_warn;			/* warn in scoreboard when tcg off */
	BYTE bspare[2];
	
	short spare[20];


	} receivREC;



/****************************/
/* The gates control struct */
/****************************/


typedef struct
{
	short delay[MEM_MAX_CHANNEL][2];	/* 2 gates per channel */
	short level[MEM_MAX_CHANNEL][2];
	short range[MEM_MAX_CHANNEL][2];
	short blank[MEM_MAX_CHANNEL][2];
	char trg_option[MEM_MAX_CHANNEL][2];
	char triger[MEM_MAX_CHANNEL];
	char g1_invrt[MEM_MAX_CHANNEL];	/*1=inverted, 0=non-inverted gate 1 */
	short spare[20];

} gateREC;




/****************************
The Laminar gate control struct
Laminar gate is a pseudo gate... does not produce its own data
in the idata packet
****************************/


typedef struct
{
	short level[MEM_MAX_CHANNEL];
	short min_level;	/* minimum of all laminar levels */
	short spare[39];

} lamgateREC;





/*****************************/
/* The pulser control struct */
/*****************************/


typedef struct
	{
	short prf;		/* in Hz  */
	short pulse_width[MEM_MAX_CHANNEL];
	char pulse_on;
	BYTE mode;		/* absolute prf or set to pulse density */
	char pulse_chn_on[MEM_MAX_CHANNEL];
	WORD PulsePerRev;
	WORD density;	/* pulse density in 0.001 */
	BYTE LocalPRF;	/* 1=local prf, 0=xtrigger gens prf */
	BYTE bspare; 
	short spare[37];

	} pulseREC;



/*******************************/
/* The alarm control structure */
/*******************************/


typedef struct
	{
	char trigger[MEM_MAX_CHANNEL][2];
	char laminar[MEM_MAX_CHANNEL][2];
	char polarity[MEM_MAX_CHANNEL][2];
	short spare[40];

	} alarmREC;



/*********************************/
/* The timeflight control struct */
/*********************************/


typedef struct
	{
	char trigger[MEM_MAX_CHANNEL][2];
	char stopon[MEM_MAX_CHANNEL][2];
	char resolution[MEM_MAX_CHANNEL];
	short spare[40];

	} timeREC;


/**********************************************/
/* Calibration specific calibration structure */
/**********************************************/


typedef struct
	{
	short StdLg1;			/* reference standard, long gate 1 */
	short StdLg2;			/* reference standard, long gate 2 */
	short StdTg1;			/*  reference standard, tran gate 1 */
	short StdTg2;			/*  reference standard, tran gate 2 */
	short StdO1g1;			/*  reference standard, oblq gate 1 */
	short StdO1g2;			/*  reference standard, oblq gate 2 */
	short StdO2g1;			/*  reference standard, oblq gate 1 */
	short StdO2g2;			/*  reference standard, oblq gate 2 */
	BYTE lto;			/*  long/tran/oblq selected */
	BYTE shoesel;		/*  shoes selected */
	BYTE gatesel;		/*  which gate selected for cal */
	BYTE displaymode;	/*  signal = 0, gain = 1 */
	float idScale[40]	;//[MEM_MAX_CHANNEL];
	float odScale[40]	;//[MEM_MAX_CHANNEL];
#if 0
	/* not doing flaw depth in truscope 2 udp */
	BYTE FlwDptLg1;			/* reference standard, long gate 1 % bw */
	BYTE FlwDptLg2;			/* reference standard, long gate 2 % bw*/
	BYTE FlwDptTg1;			/*  reference standard, tran gate 1 */
	BYTE FlwDptTg2;			/*  reference standard, tran gate 2 */
	BYTE FlwDptOg1;			/*  reference standard, oblq gate 1 */
	BYTE FlwDptOg2;			/*  reference standard, oblq gate 2 */
	BYTE FlwDptOga;			/*  reference standard, obq2 gate 1 */
	BYTE FlwDptOgb;			/*  reference standard, oblq2 gate 2 */
	BYTE FlwDptOgc;			/*  reference standard, oblq3 gate 1 */
	BYTE FlwDptOgd;			/*  reference standard, oblq3 gate 2 */
	BYTE FlwDptOge;		/*  spare */
	BYTE FlwDptOgf;
	BYTE scroll_ctl;		/* which transducer is showing in scroll window */
#endif


	BYTE AllFlawsFlag;		/* tell cdp to report all flaws */
	BYTE FlawDepthFlag;		/* report flaw depth instead of amplitude */
	BYTE bspare;
	short StdO3g1;			/*  reference standard, oblq gate 1 */
	short StdO3g2;			/*  reference standard, oblq gate 2 */
	short spare[28];
	} calREC;

	
typedef struct
	{
	JOB_REC		JobRec;
/*	LINE_REC	LineRec;	*/
	OSCOPE_REC	OscopeRec;
	receivREC	receiver;
	gateREC		gates;
	pulseREC	pulser;
	alarmREC 	alarm;
	timeREC		timeoff;
	calREC		cal;
	lamgateREC	lamgate;
	UT_REC		UtRec;
	BYTE bMotionTime;		/* 0 = motion mode, 1 = time for inspect */
	BYTE bEnglishMetric;	/* 0 = English, 1 = Metric */
	WORD wOpMode;			/* 0x200=NOP 0x201=RUN, 2=CAL, 3=PKT */
	BYTE spare[126];
	} CONFIG_REC;		/* the configuration file structure */

typedef struct
	{
	int	NextJoint;		/* index of next joint to replay */
	int LastJoint;		/* index of one previously played */
	DWORD Pos;			/* file pointer for next joint */
	DWORD EndPos;		/* file pointer for last joint */
	}	REPLAY_REC;

/* Each data file has an index record locating the data start point for */
/* each joint of pipe */

typedef struct
	{
	DWORD	JointNumber;
	DWORD	pos;		/* where in disk file the joint starts */
	time_t	time;		/* start time of data record for this joint */
	}	INDEX_REC;

/* structure for containing network statistics for a given protocol */

typedef struct
	{
	int	TotalRcvPackets;
	int LostRcvPackets;
	int DupRcvPackets;
	int	TotalSentPackets;
	int LostSentPackets;
	int DupSentPackets;
	}	NET_STATS;

typedef struct
	{
	UINT uStatus;
	NET_STATS IpxStat[4];
	char dummy[4068-96];				//char dummy[4068];	
	} MEMBUF;


/* Echo bits for transducer activity */

typedef struct
	{
	WORD	Shoe[4];		/* 4 shoes, 16 xdcr bit encoded per shoe */
	}	ECHO_BITS;

typedef struct
	{
	short nMinAvgWall;	/* minimum avg filtered average */
	short nMinWall;		/* minimum of filtered mins */
	short nMaxWall;		/* max of filtered maxs */
	short nEcc;			/* single point max ecc */
	short nMaxWallPt;	/* single point max ecc */
	short nAvgLen;		/* number of samples(inches) to average over */
	short nAvgRounding;	/* rounding factor for integer divides */
	short nPassCnt;		/* number of times Exxon Avg called during pipe */
	short nLastX;		/* last x coord used to average */
	short nMinWallX;	/* x loc of win wall reading */
	long lSumMin;		/* accumulated min's for averaging */
	long lSumMax;
	long lSumAvg;
	short nMinSample[128];	/* buffer for averaging min's */
	short nMaxSample[128];	/* buffer for averaging max's */
	short nAvgSample[128];	/* buffer for averaging avg's */
	}	EXXON_WALL_STAT;

typedef struct
	{
	short nAvgWall;		/* average of avg walls over pipe length */
	short nAvgMinWall;	/* average of min walls over pipe length */
	short nMinWall;		/* minimum over pipe length */
	short nMaxWall;		/* max over pipe length */
	short nEcc; 
	short nLastX;		/* last x coord used to average */
	short nMinWallX;	/* x loc of win wall reading */
	long lSumMin;		/* accumulated min's for averaging */
	long lSumMax;
	long lSumAvg;
	short nMinCnt;		/* samples in full pipe avg of min's */
	short nMaxCnt;		/* samples in full pipe avg of max's */
	short nAvgCnt;		/* samples in full pipe avg of avg's */
	}	TUBO_WALL_STAT;

/* 
// Implement custom mutual exclusion mechanism for sharing IPX linked list
// plistUtData.  High priority ipx thread will divert to local temp link
// list when mmi thread (TscanDlg) is using plistUtData.  TscanDlg will sleep
// for 2 ms when high priority input thread is using the linked list.
// Each user will request the list by setting its use byte to 0xff.  If
// the other user has not also set his use byte, the list is free to use
// byt the requestor.  If the other user has set the byte, the new requestor
// detects a collision and backs off the request.
// Use dword variables for speed, byte alignment by 32 bit processors. 
*/

typedef struct
	{
	DWORD ipx;		/* IPX thread requesting use */
	DWORD mmi;		/* mmi, TscanDlg requesting use */
	} LIST_SHARE;

/**************************************************************************/
/***********  Message from udp to cdp for Configuration Info  *************/
/**************************************************************************/


/*****************************/
/* A configuration structure */
/*****************************/

#define TIMEBASED		0
#define ENGLISH			0

#define DEFAULT				2
#define MAX_PROBE_PER_SHOE 10
#define MAX_RPM            600
#define MAX_QUE_NUM        50
#define MAX_REJ_NUM        20
#define MAX_MULTIPLEXER    4
#define MAX_PROB_PER_SHOE	8
#define MAX_PINCH_ROLLER	4
#define MAX_POSI				3
#define MAX_PHOT				3
#define MAX_MARKER			4

typedef struct
	{	/* Wall calibartion slope and offset coefficients */
	float slope;	/* normall should be close to 1.0 */
	short offset;		/* offset in 1/1000th inch */
	short spare;
	}	WALL_CAL;

typedef struct
{
	float prob_x[MEM_MAX_CHANNEL]; /* linear offset of transducer */
	float prob_w[MEM_MAX_CHANNEL]; /* angle offset of transducer */
	float proller_x[MAX_PINCH_ROLLER];
	float positioner_x[MAX_POSI];
	float photo_x[MAX_PHOT];
	float maker_x[MAX_MARKER];
	WORD udp_mult[MEM_MAX_CHANNEL];
	char chan_type[MEM_MAX_CHANNEL]; 			/* could be wall, lamn,long,tran,obly */
	char trigger_a;
	char trigger_b;
	char trigger;
	char sequence_length;
	char trigger_select[MEM_MAX_CHANNEL];
	BYTE WallDispChan[2];		/* 2 glass charts to config 10 chnls of paper chart */
	BYTE LgIdDispChan[2];
	BYTE LgOdDispChan[2];
	BYTE TrIdDispChan[2];
	BYTE TrOdDispChan[2];
	BYTE EnglishMetricFlag;
	BYTE TimeDistanceFlag;
/*	short WallDispThold[2];  05/20/98 now have max, min wall tholds */
	short WallDispTholdMax;	/* pens 0 & 6 */
	short WallDispTholdMin;	/* pens 1 & 7 */
	short LgIdDispThold[2];
	short LgOdDispThold[2];
	short TrIdDispThold[2];
	short TrOdDispThold[2];
	short WallDispNC[2];
	short LgIdDispNC[2];
	short LgOdDispNC[2];
	short TrIdDispNC[2];
	short TrOdDispNC[2];
	WORD SyncControl[4];	/* Control operation of sync boards */
	float CompVelocity;			/* speed of sound in this material */
	float ShearVelocity;
	/* move to calREC */
	WALL_CAL coef[4];
	float CalRef1;
	float CalRef2;		/* reference body wall for 2nd cal point */
	short NC_Tolerance;	/* 0-4 */
	WORD CopyControl;	/* what parameters to copy */
	BYTE lto;			/* used with system gain operation */
	BYTE bspare;
	short spare[11];

}  OldconfigREC;	/* used by win 3.11 and transputer truscope */


/**************************************/
/**** Pipe Record  ********************/
/**************************************/

typedef struct
	{
	char fnRecord[80];
	char Grade[80];
	char LotNum[8];
	float Wall;
	float Diameter;
	float TotalLen;
	float RejLen;
	short TotalJnt;
	short RejJnt;
	short PipeNum;
	short spare[35];


	}  OldpipeREC;	/* used by win 3.11 and transputer truscope */


	/* Config record info send to cdp */

typedef struct cdpmsg2_tag
	{	
		WORD	seq;
		WORD	mlen;	/* in WORDS ... sizeof(CDPMSG2)/2 */
		WORD	id;				/* 2 */
		WORD	spare;	/* maintain 32 bit alignment */
		OldconfigREC	cfg;
		OldpipeREC		prec;
	} CDPMSG2;


typedef struct chan_levels_tag
	{
	BYTE chan;
	BYTE chan_type;
	BYTE levelg1;	/* amplitude in gate 1 */
	BYTE levelg2;
	float gaindb;	/* gain in db of this channel */
	} CHAN_LEVELS;

typedef struct cdpmsg5_tag
	{	
		WORD	seq;
		WORD	mlen;	/* in WORDS ... sizeof(CDPMSG5)/2 */
		WORD	id;				/* 5 */
		WORD	spare;	/* maintain 32 bit alignment */
		CHAN_LEVELS chan_lvls;
	} CDPMSG5;




typedef struct ChnlGate
	{
	/* The amplitude of both gates for all 10 channels */
	WORD last_packet_num;	/* input packet number */
	WORD pad;
	BYTE idgate[12];
	BYTE odgate[12];		/* multiples of 4 bytes */
	} GATE_SIG;



typedef struct FlawCalData
	{
	BYTE cmnd21[4];		/* 116, 0xf0, 21, spare */
	GATE_SIG inst[4];
	} FLAW_CAL_DATA;

typedef struct ChnlGateD
	{
	/* The depth of flaw in both gates for all 10 channels */
	WORD last_packet_num;	/* input packet number */
	WORD rpm;
	float idDepth[10];
	float odDepth[10];
	BYTE idDepthAmp[12];
	BYTE odDepthAmp[12];
	} GATE_DEPTH;



typedef struct FlawDepthData
	{
	BYTE cmnd24[4];		/* sizeof(FLAW_DEPTH_DATA) & 0xff, 0xf0, 24, spare */
	GATE_DEPTH inst[4];
	} FLAW_DEPTH_DATA;



typedef struct ChnlGateDS
	{
	/* The flaw depth Scale factor of both gates for all 10 channels */
	WORD last_packet_num;	/* input packet number */
	WORD pad;
	float idScale[10];
	float odScale[10];
	} GATE_DEPTH_SCALE;



typedef struct FlawDepthScale
	{
	BYTE cmnd26[4];		/* sizeof(FLAW_DEPTHSCALE_DATA) & 0xff, 0xf0, 26, spare */
	GATE_DEPTH_SCALE inst[4];
	} FLAW_DEPTHSCALE_DATA;


#endif
