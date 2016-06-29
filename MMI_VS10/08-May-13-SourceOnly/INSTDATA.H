/*
Instdata.h
Tuboscope customizations for using ipx protocol with Truscope instrument
02/11/99 
Move to Vxworks 02/22/01
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

#endif

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
#if 0
	INST_DATA InstData;
#endif
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

#define UDPINIT     0x1B80         /* UDP BROADCAST SOCKET ID              */
#define UDPDATA     0x2080         /* UDP DATA SOCKET                      */
#define UDPCOMMAND  0x2180         /* UDP COMMAND SOCKET ID                */
#define UDPSEARCH   0x0FE          /* INITIALIZE PACKET TYPE TO UDP        */

#define TscanDATA     0x2080         /* Tscan DATA SOCKET                  */
#define TscanCOMMAND  0x2180         /* Tscan COMMAND SOCKET ID            */
#define TscanSEARCH   0x0FE          /* INITIALIZE PACKET TYPE TO Tscan     */
#define ECBPOSTED   0x40           /* BIT SET IN SEND ECB waitecb IF DONE  */

#endif
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
#define	MAX_GATE		2

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


/************* UDP COMMANDS BEGIN AT 0X200 ****************/
#define NOP_MODE				0x200
#define RUN_MODE				0x201
#define CAL_MODE				0x202
#define PKT_MODE				0x203




extern char (*ProcPtrArray[])();

extern int ctlMode;
extern char SendToUdp(void);
extern char ReceFromUdp(void);
extern char OpenLink(void);
extern char UdpCmd[];
extern int TheLink;

