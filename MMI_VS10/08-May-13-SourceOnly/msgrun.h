
typedef struct
	{

/*	BYTE DMac[6];	/* dest mac */
/*	BYTE SMac[6];	/* source mac */
/*	WORD len;		/* packet len */
/*	IPXHEADER IpxHdr;	*/

	WORD	Mach;		/* which machine, B: = 0x3a42 */
	short 	nSlave;		/* Which slave */
	WORD	MsgNum;
	WORD	MsgLen;
	WORD	MsgId;
	WORD	spare;
	PACKET_STATS IdataStat;	/* status of packets from inst to slave */
	PACKET_STATS SlaveStat;	/* status of packets from slave to master */
	PACKET_STATS MmiStat;	/* status of packets from MMI to master */
	} UDP_CMD_HDR;				/* master header */

typedef struct 
	{
	WORD	status[3];
	WORD	wLineStatus;	/* status at time msg sent	*/
	long	idle_cnt;
	time_t	time;
	short	VelocityDt;		/* delta t to travel 4 inches in 0.1 ms clocks */
	short	xloc;
	short	nAngleBody1;	/* location of shoe1 in 2 degree increments	*/
	WORD	JointLength;	/* in motion pulses	*/
	DWORD	JointNumber;	/*12/15/00 to dword from word	*/
	WORD	Period;		/* period of rotation in 0.1 ms	*/
	WORD	spare;
	BYTE	ShoeOn[4];		/* bit0 = 0 -> shoe off, bit7 = 1 ->sig> thold this shoe	*/
	WORD	ChnlOn[4];	/* bit mapped for each chnl. 0 = chnl off	*/
	WORD	EchoBit[MAX_SHOES];	/* bit mapped for each chnl. 0 = no echo	*/
	} INSP_HDR;

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
	short	AvgWall10;		/* avg of all wall readings within +/- 10% nom	*/
	short	AvgEcc;			/* avg of all eccentricity reading in this band	*/
	/* do it in mmi for now from maxecc to avgecc */

	BYTE	FlawDetected;	/* 0 is no flaw */
	BYTE	spare2[3];		/* for the future	*/
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
	BYTE	SegMinChnl[N_SEG]; /* chnl number for Wall Min	*/
	BYTE	SegMaxChnl[N_SEG]; /* chnl number for Wall Max	*/
	/* Add arrays to capture flaw info for "All Flaws" for cdp */
	/* Max gate ok sig level in this 1 inch of pipe for each gate */
	BYTE	GateMaxAmp[80];			/* flaw amp for both gates, 40 chnls */
	BYTE	GateMaxClk[80];			/* Clock value for associated flaw amp */

	} UT_INSP;





typedef struct
	{
	UDP_CMD_HDR MstrHdr;	/* header from master udp */
	INSP_HDR	InspHdr;
	UT_INSP	UtInsp;
	}	I_MSG_RUN;
