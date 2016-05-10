/*
Instdata.h
Tuboscope customizations for using ipx protocol with Truscope instrument
02/11/99 
Move to Vxworks 02/22/01
Revision:	11/05/01 Add shoe angle to packet data
*/


#ifndef INSTDATA_H

#define INSTDATA_H


/*typedef unsigned char		BYTE;	*/
/*typedef unsigned short		WORD;	*/

#if INSTRUMENT_CODE
/* This is the pc104 sending data from the instrument to the udp */

#define NUMRCV					32 		/* NUMBER OF RCV ECBs MAX  */
#define RCV_BUF_SIZE			600		/* Max size of a rcv buffer */
#define NUMSND					4		/* Number of Sender ECBs Max */
#define SND_BUF_SIZE			1460	/* Max size of a Send buffer */

#else
/*  This is the UDP or a Catcher program receiving data from the instrument */

#define NUMRCV					8		/* NUMBER OF RCV ECBs MAX  */
#define RCV_BUF_SIZE			1460	/* Max size of a rcv buffer */
#define NUMSND					32 		/* Number of Sender ECBs Max */
#define SND_BUF_SIZE			600		/* Max size of a Send buffer */

/* FOR COMMANDS FROM MMI TO CDP on second thought, send msgs thru master instead of
directly to cdp from mmi  */

#define NUMSNDCDP				8 		/* Number of Sender ECBs Max */
#define	SND_TO_CDP_BUF_SIZE		1024


#endif


/* Used by master and slave udp code to limit size of cmds from mmi/master */

#define SIZEOF_MMIBUF		800

#define TRUSCAN_MSG_ID      0x87FC41DA
#define TRUSCAN_MACH_ID     0xE6B3


struct Gate
	{
	BYTE alarm;
	BYTE amp;
	WORD tof;
	};
typedef struct Gate GATE;


struct Channl
	{
	GATE g[2];
	};
typedef struct Channl CHANNEL;

struct Ascan
	{
	CHANNEL ch[2];
	};
typedef struct Ascan ASCAN;

struct Board
	{
	ASCAN ascan[16];
	};
typedef struct Board BOARD;



struct packet_struct1
	{
	WORD msg_cnt;
	BOARD bd[5];
	};
typedef struct packet_struct1	INST_DATA;


struct IpxHeaderStruct
	{
	WORD checksum;
	WORD length;
	BYTE transport_control;
	BYTE packet_type;
	BYTE dest_network_number[4];
	BYTE dest_network_node[6];
	WORD dest_network_socket;
	BYTE source_network_number[4];
	BYTE source_network_node[6];
	WORD  source_network_socket;
	};
typedef struct IpxHeaderStruct IPXHEADER;

struct PacketStruct
	{
	BYTE DMac[6];	/* dest mac */
	BYTE SMac[6];	/* source mac */
	WORD len;		/* packet len */
	IPXHEADER IpxHdr;
	WORD msg_cnt;
	BOARD bd[5];
	WORD ShoeAngle;	/* angle of shoe at beginning of packet */
	};
typedef struct PacketStruct PACKET_DATA;

struct CmdPacketStruct
	{
	BYTE DMac[6];	/* dest mac */
	BYTE SMac[6];	/* source mac */
	WORD len;		/* packet len */
	IPXHEADER IpxHdr;
	WORD msg_cnt;
	BYTE buf[32];	/* buffer with cmd data in it */
	};
typedef struct CmdPacketStruct INST_CMD_DATA;

/* Generic command structure */

struct ScopeDemuxCmdStruct
	{
	WORD SubCmd;
	WORD Arg[32];
	};
typedef struct ScopeDemuxCmdStruct ScopeDemuxCmd;

#define UDPINIT     0x1B80         /* UDP BROADCAST SOCKET ID              */
#define UDPDATA     0x2080         /* UDP DATA SOCKET                      */
#define UDPCOMMAND  0x2180         /* UDP COMMAND SOCKET ID                */
#define UDPSEARCH   0x0FE          /* INITIALIZE PACKET TYPE TO UDP        */

#define TscanDATA     0x2080         /* Tscan DATA SOCKET                  */
#define TscanCOMMAND  0x2180         /* Tscan COMMAND SOCKET ID            */
#define TscanSEARCH   0x0FE          /* INITIALIZE PACKET TYPE TO Tscan     */
#define ECBPOSTED   0x40           /* BIT SET IN SEND ECB waitecb IF DONE  */

#define UDP_TO_CDP_DATA		0x1080	/* data from master udp to cdp */

#endif

/* end of INSTDATA_H */
/*
File:	instrumen.h

revised:	12/13/94 from bench test model for firing seq/ slave trigger
*/


#ifndef  OLD_SCOPE_MUX
#define  OLD_SCOPE_MUX  0
#endif


#define	PROCARRAYSIZE   200
#define	MAXMSGTYPE      0xA0       /* MAX ALLOWED MESSAGE TYPE FOR INPUT   */
#define	PROCNUM		    0x95
#define	CMD_OFFSET      0x3000
#define	IDATA_OFFSET    0x2000
#define	CHANNEL_OFFSET  0x100
#define	GATE_OFFSET     0x40
#define	FUNCTION_OFFSET 0x04
#define	MAXINTBYTE      4

#define BUF_SIZE_8051                   0x1000
/* BUF_SIZE_8051 !!! MUST be a power of 2 !!!   <<<<<<<<<<<<< */

/* command word */

#define	ALARM_LAMIN_OFF       0xCF
#define	ALARM_LAMIN_THOLD     0x10
#define	ALARM_LAMIN_PEAK      0x20
#define	ALARM_POLA_LOW        0xFB /* changed from 0x7F ...6/7/94 */
#define	ALARM_TRIG_OFF        0xFC
#define	ALARM_TRIG_THOLD      0x01
#define	ALARM_TRIG_PEAK       0x02
#define	ALARM_TRIG_MPEAK      0x03

#define	GATE_LAM_ENAB		0x08
#define	GATE_TRIG_OFF		0x07
#define	GATE_TRIG_INI		0x01
#define	GATE_TRIG_ECO		0x02
#define	GATE_TRIG_SLA		0x03
#define	GATE_TRIG_SLB		0x04

#define	RECV_DETM_RF       0x01
#define	RECV_POL_NE        0x01
#define	RECV_FILT_WB       0x03
#define	RECV_FILT_2        0x02
#define	RECV_FILT_3        0x01
#define	RECV_FILT_5        0x00


/* TCG control operations */
/* 
RESET clear tcg counters and control logic
LOAD load 512 word of gain into table
RUN run tcg gains when gate goes true
OFF load and use only 1st gain entry... like old boards w/o tcg
*/


#define TCG_RESET			0xc000
#define TCG_LOAD_TABLE		0x8000
#define TCG_RUN				0x4000
#define TCG_OFF				0



#define	TOF_RESO_HIGH		0x80
#define	TOF_TRIG_OFF			0x8F
#define	TOF_TRIG_INIT		0x10
#define	TOF_START_THLD		0x20
#define	TOF_TRIG_PEAK		0x30
#define	TOF_TRIG_SLVA		0x40
#define	TOF_TRIG_SLVB		0x50
#define TOF_START_G1_IF		0x60
#define	TOF_STOPON_OFF		0xFC
#define	TOF_STOPON_THOLD	0x01     
#define	TOF_STOPON_PEAK	   0x02
#define	TOF_STOPON_MPEAK   0x03

/* command register address */
//#define	MAX_GATE		2

#define	ALARM_TRIG_ADDR		0x07

#define	TOF_TRIG_ADDR		0x06

#define	GATE_TRIG_ADDR		0x05
#define	GATE_RANGE_ADDR		0x02
#define	GATE_LEVEL_ADDR		0x08
#define	LAM_GATE_LEVEL_ADDR		0x09
#define	GATE_DELAY_ADDR		0x00
#define	GATE_BLANK_ADDR		0x04

#define	PULSER_WIDTH_ADDR	0x1F
#define	PULSER_PRF_ADDR		0x00

#define	SEQ_ENB_TABLE_ADDR		0x2A

#define	RECEIVER_GAIN_ADDR      0x18
#define	TCG_GAIN_ADDR			0x18
#define	RECEIVER_REJECT_ADDR    0x1D
#define	TCG_TRIGGER_ADDR		0X1d
#define	RECEIVER_DETM_ADDR      0x1C
#define	RECEIVER_POL_ADDR       0x1B
#define	RECEIVER_FILT_ADDR      0x1A

#if OLD_SCOPE_MUX

#define	SCOPE_TRACE_ADDR		  0x2F
#else
#define	SCOPE_TRACE_ADDR		  0x28

#endif

#define	HARDWARE_RESET_ADDR	  0x2D
#define	SLAVE_TRIG_ADDR         0x2E

#define	PULSE_PRF_ADDR			  0x04
#define	SLAVE_MODE_ADDR			  0x02
#define	SYNC_REG_ADDR			  0x02
#define	CMD_REG_ADDR 			  0

/* These literals are agruments to ProcPtrArray */
/* These are the COMMAND ID's of the INSTRUMENT/UDP commands */

#define	SYSINIT					0x00			
#define	CHAN_GATE_CONT			0x01
#define	PROCNULL				0x02
#define	SYSINIT_COMPLETE		0x03
#define DEBUG_CONTROL			0x04
#define SLAVE_TRIG_RESET		0x05
#define SET_SYNC_REG			0x06
#define FIRST_INSTRUMENT		0x07
#define LOCAL_PRF_CONTROL		0x08
#define MMI_VERSION				0x09
#define	HARDWARE_RESET			0x10
#define	SEND_ALL_FLAWS			0X12	//28
#define	SEND_FLAW_DEPTH		0X13	//29
#define	GATE_SELECT				0x14
#define	CHANNEL_SELECT			0x15		
#define	COPY_SELECT				0x16
#define	COPY_OK					0x17
#define	PULSERREAD				0x1A
#define	PULSER_WIDTH			0x1B
#define	PULSER_PRF				0x1C
#define	PULSER_DAMPING			0x1D
#define	PULSER_ONOFF			0x1E
#define	PULSER_ALL_ONOFF		0x1F
#define	RECEIVERREAD			0x21
#define	RECEIVER_GAIN			0x22
#define	RECEIVER_REJECT			0x23
#define	RECEIVER_FCNT			0x24
#define	RECEIVER_FILT			0x25
#define	RECEIVER_POLT			0x26
#define	RECEIVER_RF_VIDEO		0x27
#define RECEIVER_PROCESS		0x29
#define	GATEREAD				0x2A
#define	GATES_DELAY				0x2B
#define	GATES_LEVEL				0x2C
#define	GATES_RANGE				0x2D
#define	GATES_BLANK				0x2E
#define	GATES_TRIGGER			0x2F
#define	GATES_LAMIN				0x30
#define	LAM_GATE_LEVEL			0x31
#define	ALARM_LAMIN				0x40
#define	ALARM_POLARITY			0x41
#define	ALARM_TRIGGER			0x42
#define	SCOPE_TRACE1			0x48
#define	SCOPE_TRACE2			0x49
#define	SET_SCOPE_MUX			0x4A
#define	SET_SCOPE_ADIBOARD		0x4B
#define	SCOPE_TRACE2_SHUNTDAC	0x4C
#define	SCOPE_TRACE2_CALDATA	0x4D
#define	SCOPE_TRACE1_MDAC		0x4E
#define	TOF_RESOLUTION			0x50
#define	TOF_STOPON				0x51		
#define	TOF_TRIGGER				0x52


/* Dac testing and prototype development */
#define TCG_ARG1				0x60
#define TCG_ARG2				0x61
#define TCG_COMPUTE				0x62
#define TCG_TRIGGER				0x63
#define TCG_STEP				0x64
#define TCG_ENABLE				0x65
#define TCG_SYSTEM 				0x66
#define TCG_COMPUTE_ALL			0x67
#define TCG_FN					0x68

#define  FIRING_SEQ				0x80
#define  UDPCONFIG				0x81
#define	SLAVE_TRIGGER			0x82
#define  TRANSD_POSI			0x8E
#define  MACH_POSI				0x8F


#define  ENET_STAT				0x90


/************* UDP COMMANDS BEGIN AT 0X200 ****************/
#define NOP_MODE				0x200
#define RUN_MODE				0x201
#define CAL_MODE				0x202
#define PKT_MODE				0x203
#define ADC_MODE				0xFFFB   /* read A/D Converter */
#define NET_MODE				0x207
#define PLC_MODE				0xFFFE
#define ASCAN_MODE				0xFFF7
#define SET_X_SCALE				0xFFF6
#define SET_ASCAN_READ_SEQ		0xFFF5
#define SET_ASCAN_PEAK_SEL_REG  0xFFF4
#define RAW_FLAW_MODE			0xFFF3
#define SET_PIPE_PRESENT		0xFFF2
#define SET_ASCAN_READ_BEAM		0xFFF1
#define TURN_OFF_MASTER			0xFFF0
#define ENET_STATS_MODE			0x204
#define SCOPE_DMX_CAL_MODE		0x205
#define  ENET_STATS_STOP	0x206

#define RESET_NET_STATS			0x210
#define CHANNEL_CONFIG_MSG		0x211
#define JOINT_NUMBER_MSG		0x212
#define SET_TIME_MTN_FLAG		0x213
#define SET_NOMINAL_WALL		0x214
#define SET_FIRING_SEQ			0x215
#define SET_WALL_THICK_THIN		0x216
#define SET_WALL_COEFS			0x217
#define SET_ALL_THOLDS			0x218
#define SET_NC_NX				0x219

/* Sheldon testing, slave don't talk to instrument */

#define SLAVES_HUSH				0x21a
#define SLAVES_TALK				0x21b

/* Set parameter that vary from installation to installation.
   Info written into registry of MMI
*/

#define SITE_SPECIFIC_MSG		0x21c

/*****
	sets slope = 1.0, offset = 0 without changing values stored in config rec.
	used only for wall cal dialog and wall cal operations
******/
#define SET_WALL_COEFS_RAW		0x21d

/* Send copy of JobRec from mmi to master to hold for cdp */

#define STORE_JOBREC_MSG		0x21e

#define SET_ASCAN_REGISTERS     0x21f

#define ASCAN_REFRESH_RATE		0x221

#define REQUEST_RESEND_MSG		0x222

#define ASCAN_BROADCAST			0x223

#define RUN_CAL_JOINT           0xFFF9

#define LAST_JOINT_NUMBER       0xFFFA

#define WALL_DROP_TIME          0xFFFC

#define LOAD_CONFIG_FILE        0xFFFD

#define SCOPE_DEMUX_CMD				0x208

#define RAW_DATA_ID                      10
#define DATA_ID                          11
#define ASCAN_ID                         12

/***************************** Scope demux has 5 major parts

1.  Scope display commands .. what you see on the scope
2.  Scope offset adjustments .. how you adjust offset on scope
3.  Scope autocal .. how you read the dc offsets
4.  Strip Chart output .. what is output for a strip chart display
5.	32 bit general purpose digital IO .. how we implement motion bus

These sections are sub-commands of the scope demux command
************************************/

#define SCOPE_DEMUX_DISPLAY		0
#define SCOPE_DEMUX_ADJUST		1
#define SCOPE_DEMUX_AUTOCAL		2
#define SCOPE_DEMUX_STRIPOUT	3
#define SCOPE_DEMUX_DIGIO		4


/*********************   MMI TO CDP MSG's (Thru the Master UDP)  *****************/
/****************** Works just like the windows 3.11/ transputer system **********/


#define	CDP_PING				0x300			
#define	CDP_MSG1				0x301			
#define	CDP_MSG2				0x302			
#define	CDP_MSG3				0x303			
#define	CDP_MSG4				0x304			
#define	CDP_MSG5				0x305			


/**************************************************/
/*******  Message Types from Slave to Master ******/
/**************************************************/
#define	NET_CONNECT_MSG			(BYTE) 0x1			
#define	AD_CONVERTER_MSG		(BYTE) 0x2			


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
#define		IS_LAM_LOG 		32
#define		IS_LAM_AMP		33

#define		IS_OBQ2			64
#define		IS_OBQ2_ID		64
#define		IS_OBQ2_OD		65

#define		IS_OBQ3			128
#define		IS_OBQ3_ID		128
#define		IS_OBQ3_OD		129

/******** pipe location / handling parameters ************/
#define	INSPECT_ENABLE_TRUSCOPE			0x10
#define	PIPE_NOT_PRESENT			    0x0
#define MOTION_BASED					0x100
#define TIME_BASED						0
#define IMAGE_BUF_OUTPUT_DELAY			4

#define	PIPE_PRESENT			(WORD) 0x0008	
#define	PIPE_ENABLE				(WORD) 0x0008
#define	PC1_BLOCKED				(WORD) 0x020
#define BUF_FLUSH				(WORD) 0x4000

#define	INSPECT_ENABLE_UT_12	(WORD) 0x0010
#define	INSPECT_ENABLE_UT_34	(WORD) 0x0020
#define	REAL_TIME_DATA			(WORD) 0x8000
#define WALL_INCLUDED			(WORD) 0x4000
#define CALIBRATION_JOINT		(WORD) 0x2000
//#define	HOME					(WORD) 0x8000
//#define	AWAY					0

#define	FORWARD					(WORD) 0x0004
#define	REVERSE					0
#define	FLAW_MASK				0x8
//#define	TOP						1
/* output from trailing end transducer is 10 index counts behind input index.
On first inspected inch of pipe, input referenced to center line of shoe (ch 5)
is into location 10, but output is from image buf[0] to display trailing 
transverse signal.
*/


extern char (*ProcPtrArray[])();

//extern int ctlMode;
//extern char SendToUdp(void);
//extern char ReceFromUdp(void);
//extern char OpenLink(void);
//extern char UdpCmd[];
//extern int TheLink;

