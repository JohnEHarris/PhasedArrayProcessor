/*
Common.h
DAte:	07/17/01

*/

#define GEO_ADDR_PORT				0xE4
#define GEO_ADDR_MASK				0x1F
#define getGeoAddress()			(sysInByte(GEO_ADDR_PORT) & GEO_ADDR_MASK)


/* IF WE USE HARDWARE CONTROL TO SEQUENCE PACKET TRANSFER FROM SLAVES
	TO MASTER OVER COMPACT NET
*/

#define	COM_PORT1				0x3f8
#define	COM_PORT2				0x2f8
#define	COM_PORT3				0x3e8
#define	COM_PORT4				0x2e8 

#define	LCR				3
#define	LSR				5
#define	IER				1
#define	IIR				2
#define	MCR				4
#define	MSR				6

#define ISEN			8
/* ISEN is mcr interrupt enable for 16450.. p144 of slc90e66 manual */

/* LCR = Line Control Reg, IER = Intr Enable Reg, MSR = Modem Status Reg */
/* IIR = Intr Id reg, MCR = Modem Control Reg LSR = Line Stat Reg */

/*****
DTR = pin 4 on 9 pin D-connector, zt5541
DSR = pin 6 on 9 pin D-connector
*****/



int MakeUdpCmd(int nCmd, UDP_CMD *pCmd, int *pSentCnt);
void ShowUdpCmd(UDP_CMD *pCmd, char *s);
void Ipset(int);
int ClockRate(int nHz);
void HexShow (char *Title, BYTE *buf, char *s, int nBytes);
void UpdatePacketStat(PACKET_STATS *ps, WORD wMsgCnt, int nBytes, int *pResetEc);


void SetDTR(int ComPort);
void ClearDTR(int ComPort);
void ToggleDTR(int ComPort);
void ShowReg(int ComPort);

BYTE IsDSR(int ComPort);
void EnableDSR_Int(int ComPort);
void DisableDSR_Int(int ComPort);
void KillLoopBack(int ComPort);
