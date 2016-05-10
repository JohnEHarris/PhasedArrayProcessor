// Inspection Display Processor Constants File

#ifndef __MCIDP_CONSTANTS
#define __MCIDP_CONSTANTS

#define AMA		0
#define SONO	1
#define TRU		2
// ==========================================================================
// Define the language used in the program.
// ==========================================================================
#define LANGUAGEDLL "GDPChinese.dll"
//#define LANGUAGEDLL "GDPRussian.dll"
//#define LANGUAGEDLL "GDPEnglish.dll"
//#define LANGUAGEDLL "GDPSpanish.dll"

// ---------------------------------------------------------------------------
// Database version:
//		1 -- before 2006-10-10
//		2 -- 2006-10-10, tblOperators is introduced.
//		3 -- 2007-03-22, wall fields changed from decimal(4,3) to (6,5).
//		4 -- 2011-09-14, more fields in TraceDetails table:ques thold, rej thold, 
//						 IE start loc, IE stop loc.
// ---------------------------------------------------------------------------
#define DB_VERSION				4
#define DRAW_TRUSCOPE_NOISE		1	// 1 -- draw noise, 0 -- not draw
#define SAVE_FLAW_NUMBER		1	// 1 -- save, 0 -- not save. 
									// Save flaw count number to ProveUpClassification 
// ---------------------------------------------------------------------------
#define SIMULATOR_EQUIPMENT		SONO // AMA
/******************************************************************************
Version		Date		Description
-------------------------------------------------------------------------------
6.6.0.0		10/28/2012	Send traces and flaws to TCI directly through TCP/IP. 
6.5.2.0		10/25/2012	0 Blob damage the Blob pointer in the Table class. 
						Set trace length to at least 1.
6.5.1.5		10/17/2012	Remove log messages for showing bkgnd value.  bad version.
6.5.1.4		09/20/2012	\r added in front of \n for log file. 
						Add log to check the trace data. 
						bad version, crash when trace too short.
6.5.1.3		08/29/2012	Create a TraceDetails record even the TraceLength is 
						less than MIN_TRACE_SIZE.
6.5.1.2		08/23/2012	Add Czech language dll file.
6.5.1.1		08/19/2012	Improve log message scheme. 1 file per day.
6.5.0.8		08/18/2012	Do not draw before 1st data point for thin wall (65535).
6.5.0.7		08/17/2012	Change log file open mode from "a" to "a+" to allow sharing.
6.5.0.6		08/15/2012	Rolled back from 6.5.0.5. Add one more digit to the x location display.
6.5.0.2		05/23/2012	Fixed crash problem on Joint Num debug info display.
6.5.0.1		05/18/2012	Put flaw# to proveupClassification.
6.5.0.0		04/26/2012	Disable auto reconnect to database.
6.4.0.0		02/21/2012	Save Average Wall blob without display the trace.
6.3.0.1		09/22/2011	Fixed bugs on DatabaseInterface.cpp.
6.3.0.0		09/14/2011	Handle the extra fields in TraceDetails table: ques thold,
						rej thold, IE start loc, IE stop loc. 
6.2.0.1		05/04/2011	Show flaw coil # and angle etc. 
6.2.0.0		04/04/2011	Adjust to handle SonoscopeB1, SonoscopeB2. 
6.1.0.0		02/11/2011	Port to Windows 7: change listening port number. 
6.0.0.1		01/21/2011	Get Windows version and adjust layout. 
6.0.0.0		01/13/2011	Port to VS2010. 
5.1.0.5		10/22/2010	Keep filling previous noise value on the 0-value spot 
						for curve graphs only. No more fillings for bar graphs.
5.1.0.1		09/03/2010	Reverse change on 4.3.0.1. NOT allow UT machines to 
						specify flaw bar color.
5.1.0.0		07/06/2010	Add Database Ping: Query EquipmentDescription table 
						every second.
5.0.0.0		10/14/2009	Convert to .net 2005 and "Release" configuration. 
4.3.0.1		10/07/2009	Allow UT machines to specify flaw bar color. 
4.3.0.0		07/10/2009	Add Chinese user interface. 
4.2.0.2		06/13/2008	Change "Truscope Wall Selection" to "Wall Selection". 
4.2.0.1		06/05/2008	When map data is <= 0, set it to 0 instead 1%.To ensure 
						fill in empty unit in the buffer, when Amalog/Sonoscope
						send 0% signal, GDP set it to 1%.
4.2.0.0		06/05/2008	New strings for language dlls.
4.1.1.5		06/04/2008	More debug messages.
4.1.1.4		06/02/2008	Fill in the untouched trace buffer elements with prev.
						value. If input value is 0 or less, change it to 0.001 
						inches or 1%.
4.1.1.3		05/31/2008	Take the modification on 4.1.1.1 away.
4.1.1.2		05/29/2008	Remove limit on wall flaw data saving to database.
4.1.1.1		05/23/2008	Fill in the untouched trace buffer elements with prev.
						value.
4.1.1.0		05/22/2008	Fixed the bottom wall trace comparison problem. Keep 
						the smaller value instead of bigger value.
4.1.0.0		05/15/2008	Add % or value choise in the Options window for wall.
******************************************************************************/

#define MAX_TRACE_FLAW	1000

#define TRU_PHASE			1900000
#define TRUSCAN_FIELD_UNIT	2000000


//#define MC_TCI_IP_ADDR	_T("192.168.8.1")
//#define MC_TCI_IP_ADDR	_T("USA-CNU0491M01.nov.com")
#define MC_TCI_IP_ADDR	_T("127.0.0.1")

// port definitions for all the servers
#define MC_SQL_LISTENPORT		1433		// database server listens on this port

// for Windows XP
#define	MC_SysCP_LISTENPORT		0xc000		// system control processor listens on this port
#define MC_ICP_LISTENPORT		0xc001		// inspection command processors listen on this port
#define MC_IDP_LISTENPORT		0xc002		// inspection display processors listen on this port
#define MC_PRO_LISTENPORT		0xc003		// protocol translator listens on this port
#define MC_RPG_LISTENPORT		0xc004		// report generator listens on this port
#define MC_PRV_LISTENPORT		0xc005		// proveup stations listen on this port

// for Windows 7
#define	MC_SYSCP_LISTENPORT_WIN7 	0xfff0		// 0xfff0(65520), 0xc000	// system control processor listens on this port
#define MC_ICP_LISTENPORT_WIN7		0xfff1		// Amalog command processors listen on this port
#define MC_IDP_LISTENPORT_WIN7		0xfff2		// GDP inspection display processors listen on this port
#define MC_PRO_LISTENPORT_WIN7		0xfff3		// protocol translator listens on this port
#define MC_RPG_LISTENPORT_WIN7		0xfff4		// report generator listens on this port
#define MC_PRV_LISTENPORT_WIN7		0xfff5		// proveup stations listen on this port
//#define MC_MT_LISTENPORT_WIN7		0xfff6		// TCI listen on this port
#define MC_SCP_LISTENPORT_WIN7		0xfff7		// Sonoscope command processors listen on this port
#define MC_4P_LISTENPORT_WIN7		0xfff8		// Four Probe command processors listen on this port
#define MC_MT_LISTENPORT2_WIN7		0xfff9		// TCI listen on this port for GDP connection

//#define MC_LCP_LISTENPORT			0xaf12		// PLC listens on this port

#define MC_TRU_PHASE_LISTENPORT_WIN7            0xfffa        // TruPhase listen on this port
#define MC_TRUSCAN_FIELD_UNIT_LISTENPORT_WIN7   0xfffb        // TruScanFieldUnit listen on this port

// =================================================================================================
#define MAX_PACKET_LENGTH   8000
//This structure is used as container for all messages
#ifdef YIQING
typedef struct {
   USHORT	Originator;			// entity that originated message
   USHORT	Target;				// entity that should respond to message
   USHORT	MessageType;		// what does this message contain
   USHORT	MessageContent;		// what structure is contained in message
   USHORT   MessageID;           // 0=Trace Detail;  1=Signal Detail
   UCHAR    Spare[MAX_PACKET_LENGTH - (5 * sizeof(USHORT))];
} TCPDUMMY;
#endif

// signal types
#define                UNKNOWN_SIGNAL	0xa000	// 40960
// graph 1 Title bar shows the joint number

// graph 2 Wall
#define                THICK_WALL		0xa00e	// 40974 m_TraceBuffer[0]
#define                THIN_WALL		0xa005	// 40965 m_TraceBuffer[1]
// graph 3
#define                ID_LONGITUDINAL	0xa001	// 40961 m_TraceBuffer[2]
#define                OD_LONGITUDINAL	0xa002	// 40962 m_TraceBuffer[3]
// graph 4
#define                ID_TRANSVERSE	0xa003	// 40963 m_TraceBuffer[4]
#define                OD_TRANSVERSE	0xa004	// 40964 m_TraceBuffer[5]
// graph 5
#define                ID_OBLIQUE_1		0xa008	// 40968 m_TraceBuffer[6]
#define                OD_OBLIQUE_1		0xa009	// 40969 m_TraceBuffer[7]
// graph 6
#define                ID_OBLIQUE_2		0xa00a	// 40970 m_TraceBuffer[8]
#define                OD_OBLIQUE_2		0xa00b	// 40971 m_TraceBuffer[9]
// graph 7
#define                ID_OBLIQUE_3		0xa00c	// 40972 m_TraceBuffer[10]
#define                OD_OBLIQUE_3		0xa00d	// 40973 m_TraceBuffer[11]
// graph 8
#define                LAMINATION_AMP	0xa006	// 40966 m_TraceBuffer[12]
#define                LAMINATION_LOGIC	0xa00f	// 40975 m_TraceBuffer[13]

// Trace or Flaw Signal sub-Structures
typedef struct {
	float      Xloc;
	float      Yloc;
} TRACE_SIGNAL;

typedef struct {
	float	Xloc;
	float	Yloc;
	int		Sensor;
	int		Angle;
	int		SignalType;
	int		SignalClass;
} FLAW_SIGNAL;

typedef struct {
	int		SignalType;
	int		MaxHeight;
	int		MinHeight;
	float	ThresholdQus;
	float	ThreshRej;
} TRACE_INFO;

// Structure to GDP interface program containing flaw signal points, i.e., signal details
#define MAX_CID_CHAR_LENGTH		50	// max length of char array
#define MAX_NUM_TRACES			16	// max number of traces for an equipment
#define TOTAL_NUM_TRACES		14	// Total number of traces used for the equipment
#define MAX_NUM_TRACE_POINTS	900	// max number of points in a trace
#define MAX_NUM_EQUIP_FLAWS		600	// max number of flaws for an equipment
#define MAX_NUM_MSG_FLAWS		300	// max number of flaws in a message

typedef struct {
	USHORT	Originator;			// entity that originated message
	USHORT	Target;				// entity that should respond to message
	USHORT	MessageType;		// what does this message contain
	USHORT	MessageContent;		// what structure is contained in message
	USHORT	MessageID;			// 0=Trace Detail;  1=Signal Detail

	USHORT	End_Of_Joint;		// 0=not end of joint;  1=end of joint
	int		Units;				// 0=Feet;  1=meters, for future use
	int		SignalType;			// ignore if in SIGNAL_DETAIL
	TCHAR	JointDetailID[MAX_CID_CHAR_LENGTH];
	UINT	JointNumber;
	int		NominalWall;		// in thousandth of a inches
	int		NominalOD;
	float	PipeLength;
	int		RPM;
	float	LineSpeed;
	float	MPLength;			// Motion Pulse length in inches
	float	IEStartLoc;
	float	IEStopLoc;
	float	MinWall;
	float	MaxWall;
	float	MeanWall;
	TRACE_INFO	Threshold[MAX_NUM_TRACES];
} HEADER;

typedef struct {
	HEADER			Header;
	int				Num_Of_Valid_Flaws;
	FLAW_SIGNAL		Flaw[MAX_NUM_MSG_FLAWS]; // for each pipe, at most 600 flaws for all signal types
} SIGNAL_DETAIL_PGT;

// Structure to GDP interface program containing trace details for drawing traces
typedef struct {
	HEADER			Header;
	int				Num_Of_Valid_Points;
	TRACE_SIGNAL	Point[MAX_NUM_TRACE_POINTS];
} TRACE_DETAIL_PGT;
// =================================================================================================


#define DISCONNECT	0
#define CONNECTED	1
#define CONNECTING	2

#define SZATTRIBUTE_BUFSIZE		1024	// size of the temporary string buffer
#define MAX_LENGTH_MP			3072	// 128 Feet with 0.5" motion pulses
#define MAX_DB_RECORD_SIZE		MAX_TRACE_SIZE+1000
//#define MAX_CMD_SIZE			256
#define MAXGRAPH				16
#define MAXTRACE				40
//#define MAXFIELD				8

// Equipment number
#define AMALOG		      0
#define SONOSCOPE	 100000	// standard Sonoscope
#define SONOSCOPEB1	 100001	// 96-ch Sonoscope Bank 1
#define SONOSCOPEB2	 100002	// 96-ch Sonoscope Bank 2
#define TRUSCOPE	 200000
#define FOUR_PROBE	1400000	// 4-probe
#define WALLMONITOR	1600000	// Algoma
#define TRUSCAN		1500000	// KSP C9
#define DIAMETER_GAUGE	1700000	// KSP C9

#define MAX_EQUIP_FLAW	500		// // for each equipment, if > MAX_EQUIP_FLAW, not save to db and set value to 0	

// signal classification
#define SIGNAL_UNKNOWN			0xb000		// 45056 unknown
#define SIGNAL_GOOD				0xb001		// 45057 ADP send once every second, amplitude will be set to 0 by PT
											//       questionable 1, background noise
#define SIGNAL_QUESTIONABLE		0xb002		// 45058 questionable 2 and 3
#define SIGNAL_REJECT			0xb003		// 45059 reject
#define SIGNAL_DROP_OUT			0xb007		// 45063 wall drop out

// Threshold Type
#define NONE	0
#define DASHED	1
#define DOTTED	2
#define SOLID	3

#define MC_CONFIG_DIR	_T("..\\")// _T("T:\\") // C:\\devtools\\"

#define LOG_FILE_NAME	_T("GDPLog.txt")
#define ERR_FILE_NAME_1	_T("GDPErr1.txt")
#define ERR_FILE_NAME_2	_T("GDPErr2.txt")


// mins and maxs
#define MAX_LOGIN_TIMEOUT		300	// database connection timeout in 300 seconds
#define MAX_QUERY_TIMEOUT		300	// RecordSet operation (Open, AddNew, Edit, and Delete) timeout in 300 seconds
#define MIN_SIGNAL_VALUE		0
#define MAX_SIGNAL_VALUE		255
#define MIN_SIGNAL_ANGLE		0
#define MAX_SIGNAL_ANGLE		359
#define MIN_SIGNAL_CLASSIFICATION		0xb000	// 45056
#define MAX_SIGNAL_CLASSIFICATION		0xbfff	// 49151
#define MIN_SEGMENTS_PER_MOTION_PULSE	  1
#define MAX_SEGMENTS_PER_MOTION_PULSE	 30
#define MIN_WALLMATRIX_VALUE	0
#define MAX_WALLMATRIX_VALUE	255

#define MIN_SENSOR_NUM		1
#define MAX_SENSOR_NUM		99

#define MIN_EQUIPMENT_NUMBER	0
#define MAX_EQUIPMENT_NUMBER	0XFFFFFF

#define MIN_SIGNAL_TYPE		0
#define MAX_SIGNAL_TYPE		0XFFFF

#define MAX_DB_DATA		1024

// structure definitions
typedef struct {
	BYTE TableName;
    BYTE dummy[MAX_DB_RECORD_SIZE-1];
} DUMMY;

typedef struct {
	BYTE TableName;
	long EquipmentNumber;			// unique number assigned by database
	long SignalType;				// unique number assigned by database
	CString PipeNumberID;			// unique number assigned by database
	CString SignalXloc;				// signal's xLoc in inches
	int		SensorNumber;
    int		SignalAngularLocation;	// 0 - 359 degrees
	double	SignalValue;			// amplitude 0 - 255
    long	SignalClassification;	// good, questionable, reject, etc.
    CString ProveUpValue;			// magnitude in inches
    long	ProveUpClassification;	// good, questionable, reject, etc.
} SIGNAL_DETAIL;

typedef struct {
	BYTE TableName;
	long EquipmentNumberInt;
	int	 SaveFlag;
	CString	m_CustomerJointIdentifier;
	CString	m_CustomerJointSuffix;
	long	FinalDisposition;
	CString	JointDetailID;
	CString JointLength;			// Joint length xxx.xx
	long	JointSequenceNumber;
	CString	JointSuffix;
	CTime	JointTimeStamp;
	CString	JointType;
	CString	MotionPulseLength;
	long	ProveUpClassification;
	long	WallDropoutCount;
	double	WallDropoutDuration;
	CString WorkOrderID;			// unique number assigned by database
	double	AmalogRPM;	
	double	TruscopeRPM;
	double	LineSpeed;
	double	AmalogMagCurrent;
	double	SonoscopeMagCurrent1;
	double	SonoscopeMagCurrent2;
	double	SonoscopeMagCurrent3;
	double	SonoscopeDemagCurrent;
	long	GradeComparator;
	long	PipeGrade;
	long	SonoscopePipeAnalysisResult;
	long	SonoscopeDetectedDefectCount;
	long	AmalogPipeAnalysisResult;
	long	AmalogDetectedDefectCount;
	long	UltrasonicPipeAnalysisResult;
	long	UltrasonicDetectedDefectCount;
    CString	MinimumWallThickness;
    CString	MeanWallThickness;
    CString	MaximumWallThickness;
} JOINT_DETAIL;

typedef struct {
	BYTE TableName;
	CString PipeNumberID;			// unique number assigned by database
	long EquipmentNumber;			// unique number assigned by database
	long SignalType;				// unique number assigned by database
    int	TraceLength;				// number of points in the trace
	float Questionable;
	float Reject;
	float Start;
	float Stop;
	CByteArray TraceArray;			// y values of a trace
} TRACE_DETAIL;

typedef struct trace_buffer{
	CString PipeNumberID;			// unique number assigned by database
	long SignalType;				// unique number assigned by database
    int	TraceLength;				// number of points in the trace
	unsigned short buf[MAX_LENGTH_MP];
} TRACE_BUFFER;

typedef struct {
	BYTE TableName;
	CString WorkOrderID;			// unique number assigned by database
	CString EquipmentID;			// unique number assigned by database
	CString PipeNumberID;			// unique number assigned by database
    BYTE GraphNum;					// 1-8
    BYTE GraphTop;					// 0 = top graph, 1 = bottom graph
    long ThresholdColor;			// RGB color 24 bits
    BYTE ThresholdValue;				// 1-99
    BYTE ThresholdType;				// 0 = none, 1 = Dashed, 2 = dotted, 3 = solid
	CString TraceName;				// plaintext trace legend
	BYTE	AutoColor;					// 0 = false, 1 = true
	CString ThresholdLabel;			// plaintext threshold legend
} GRAPH_DETAIL;

typedef struct {
	BYTE TableName;
	CString WorkOrderID;			// unique number assigned by database
	CString EquipmentID;			// unique number assigned by database
	CString WallMatrixTypeID;		// unique number assigned by database
	CString PipeNumberID;			// unique number assigned by database
    CString MotionPulseLength;		// length of this motion pulse in inches
	CString WallMatrixXloc;			// signal's xLoc in inches
    BYTE	SegmentsPerMotionPulse;	// 0 - 30
    BYTE	WallMatrixData01;		// data for segment 1
    BYTE	WallMatrixData02;		// data for segment 2
    BYTE	WallMatrixData03;		// data for segment 3
    BYTE	WallMatrixData04;		// data for segment 4
    BYTE	WallMatrixData05;		// data for segment 5
    BYTE	WallMatrixData06;		// data for segment 6
    BYTE	WallMatrixData07;		// data for segment 7
    BYTE	WallMatrixData08;		// data for segment 8
    BYTE	WallMatrixData09;		// data for segment 9
    BYTE	WallMatrixData10;		// data for segment 10
    BYTE	WallMatrixData11;		// data for segment 11
    BYTE	WallMatrixData12;		// data for segment 12
    BYTE	WallMatrixData13;		// data for segment 13
    BYTE	WallMatrixData14;		// data for segment 14
    BYTE	WallMatrixData15;		// data for segment 15
    BYTE	WallMatrixData16;		// data for segment 16
    BYTE	WallMatrixData17;		// data for segment 17
    BYTE	WallMatrixData18;		// data for segment 18
    BYTE	WallMatrixData19;		// data for segment 19
    BYTE	WallMatrixData20;		// data for segment 20
    BYTE	WallMatrixData21;		// data for segment 21
    BYTE	WallMatrixData22;		// data for segment 22
    BYTE	WallMatrixData23;		// data for segment 23
    BYTE	WallMatrixData24;		// data for segment 24
    BYTE	WallMatrixData25;		// data for segment 25
    BYTE	WallMatrixData26;		// data for segment 26
    BYTE	WallMatrixData27;		// data for segment 27
    BYTE	WallMatrixData28;		// data for segment 28
    BYTE	WallMatrixData29;		// data for segment 29
    BYTE	WallMatrixData30;		// data for segment 30
} WALLMATRIX_DETAIL;

typedef union{ 
	int Cmd;
	unsigned Sequence;
	int Response;
	int ErrorCode;
} SENDBUFFER;

typedef struct{
	int NumOfFld;
	FIELD Field[MAXFIELD];  // Fields for a graph
	float Value[MAXFIELD];	// hold float or int values
	CString ValueStr[MAXFIELD];		// hold string values
} GRAPHFIELD;

typedef struct{
	GRAPHFIELD GraphField[MAXGRAPH];
} STATUS;


class CSemaphore;

typedef struct{
	HWND hWnd;
	BOOL *lpKillThread;
	CSemaphore * psemKillThread;
	CSocket * pServiceSocket;
	CPtrList* pDataPacketList;
	CDocument * pDoc;
	int ViewerRef;
	CSemaphore * psemDataPacketList;
} THREAD_INFO;

typedef struct{
	BOOL *lpKillThread;
	CPtrList* pDataList;
	BOOL * bDBConnected;
	CSemaphore * psemKillThread;
	CSemaphore * psemDataList;
	int * iDBUpdateFailed;
	long * pTimeUsed;
} DB_THREAD_INFO;

typedef struct{
	TCHAR str[16];
} NAME;

typedef struct{
	float Value[4];	// [0]: for Top trace, [1]: for Bottom trace
	int Type[4];
	int Color[4];
	int AutoColor[4];	// 1 -- use threshold color in cmd3. 0 --- use color in cmd4.
	NAME Name[4];
}THRESHOLD;

typedef struct{
	int Count[4];	// [0]: top, [1]: bottom, [2]: trace3 and [3]: trace4
	int DropOutCount[4];
}TRACE_FLAW_COUNT;

typedef struct state{
	unsigned ViewerRef;
	TCHAR Client[16];
	int ScreenLength;
	int ScreenLengthOrig;
	int Units;				// 0=feet, 1=meters
	int DispMode;			// 0=Log (db),  1=linear
	int GraphCount;
	float BarDrawWidth;
	int GraphType[MAXGRAPH];
	NAME GraphName[MAXGRAPH];
	int Double[MAXGRAPH];
	int NumTraces[MAXGRAPH];
	float MaxHeight[MAXGRAPH];
	float MinHeight[MAXGRAPH];
	float MaxHeightDraw[MAXGRAPH];
	float MinHeightDraw[MAXGRAPH];
	THRESHOLD ThresholdRej[MAXGRAPH];
	THRESHOLD ThresholdQues[MAXGRAPH];
	THRESHOLD ThresholdRejOrig[MAXGRAPH];
	THRESHOLD ThresholdQuesOrig[MAXGRAPH];
	int BeginY[MAXGRAPH];
	int InspFlag;
	int SaveToDB;
	int SaveToDBTemp;
	TRACE_FLAW_COUNT TraceFlaw[MAXGRAPH];
	CString WorkOrderID;
	long EquipmentNumber;
	int	TotalTrace;
	int	TotalPlotTrace;
	int SignalType;
	float MotionPulseLength;
	CString PipeNumberID;
	long PipeAnalysisResult;	// 1 = good, 2 = questionable, 3 = reject
	long DetectedDefectCount;	// count of questionable and reject defects
	float NominalWall;
	unsigned short AutoColor;	// if AutoColor=1, the flaw bars use the color in command 4
	int	SignalType3[MAXGRAPH];
} STATE;

typedef struct data_point{
	float Xloc;
	float Yloc;
	float Xloc1;
	float Yloc1;
	int Color;
	int SignalClassification;
	// for sending to TCI
	int RadialLocation;	// in degress
	float XlocImp;		// w/o convert to metric
	float YlocImp;		// w/o convert to log and/or metric
} DATA_POINT;

#define DISPTIMER		300
#define PRERUNTIMER		301
#define SHOW_TCI_TIMER	302

#define RED			RGB(255,0,0)
//#define GREEN		RGB(20,255,20)


/* line positions in logical units

	Titlebar: height 50
	Double Bar/Curve, Up -- Down:
			0									begin
			2   ==============================  top thick line
				threshold line somewhere here
			102 ------------------------------  top baseline
			104 ------------------------------  bottom baseline
				threshold line somewhere here
			204 ==============================  bottom thick line
			214		|	|	|	|	|	|	    scale tick

  	Double Bar/Curve, Up -- Up:
			0									begin
			2   ==============================  top thick line
				threshold line somewhere here
			102 ------------------------------  top baseline and cutoff 
			                                    (max value) for bottom trace
				threshold line somewhere here
			202	------------------------------  bottom up-up baseline
			204 ==============================  bottom thick line
			214		|	|	|	|	|	|	    scale tick

	Single Bar/Curve, or map:
			0									Begin
			2   ==============================  top thick line
				threshold line somewhere here
			102 ------------------------------  top baseline
			104 ==============================  bottom thick line
			114		|	|	|	|	|	|	    scale tick
*/
#define LARGEFONTSIZEY  26 // 44
#define LARGEFONTSIZEX  12 // 44
#define SMALLFONTSIZEY	20 // 40
#define TINYFONTSIZEY	4  // 40
#define WINDOWWIDTH		1920

#define TITLEBARHEIGHT		26 // 50
#define TOPTHICKLINE		2
#define TOPBASELINE			102
#define BOTTOMBASELINE		105
#define BOTTOMUPUPBASELINE	202
#define BOTTOMTHICKLINE		205
#define SCALETICK			4
#define MIN_HEIGHT			20

#define ID_MY_STATUS_BAR 1010

#define ONE_SECOND_TIMER	1212
#define HALF_SECOND_TIMER	1213
#define ONE_MINUTE_TIMER	1214

typedef struct{
	CView * pView;
	int Height;
	int Width;
	int Order;
} WININFO;

// for simulator
#define SIMTIMER	300
#define PIPELENGTH	10	// IN FEET
#define PIPESPACING	2	// IN FEET

//#define CURRENT_VERSION		_T("Version 1.0")

#endif