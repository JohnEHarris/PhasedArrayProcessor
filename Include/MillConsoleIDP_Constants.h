// MillConsoleIDP_Constants.h

#pragma once

// Inspection Display Processor Constants File

#ifndef __MCIDP_CONSTANTS
#define __MCIDP_CONSTANTS

// port definitions for all the servers
#define MC_SQL_LISTENPORT		1433		// database server listens on this port
#define	MC_SCP_LISTENPORT		0xc000		// system control processor listens on this port
#define MC_ICP_LISTENPORT		0xc001		// inspection command processors listen on this port
#define MC_IDP_LISTENPORT		0xc002		// inspection display processors listen on this port
#define MC_PRO_LISTENPORT		0xc003		// protocol translator listens on this port
#define MC_RPG_LISTENPORT		0xc004		// report generator listens on this port
#define MC_PRV_LISTENPORT		0xc005		// proveup stations listen on this port

// Table name
#define SIGNAL_DETAIL_TABLE		1
#define GRAPH_DETAIL_TABLE		2
#define WALLMATRIX_DETAIL_TABLE	3
#define TRACE_DETAIL_TABLE		4

#define SZATTRIBUTE_BUFSIZE		1024	// size of the temporary string buffer
#define MAX_DB_RECORD_SIZE		MAX_TRACE_SIZE+1000
#define MAX_CMD_SIZE			256
#define MAXGRAPH				20
#define MAXTRACE				40
#define MAXFIELD				8

#define MAX_TRACE_FLAW	50		// for each trace, if > MAX_TRACE_FLAW, not save to db and set value to 0	
#define MAX_EQUIP_FLAW	500		// // for each equipment, if > MAX_EQUIP_FLAW, not save to db and set value to 0	

#define	TOP_TRACE			0
#define BOTTOM_TRACE		1

#define	FEET		0
#define METERS		1

#define SINGLE		0
#define DOUBLE		1

// graph types
#undef MAP
#define MAP			1
#define BAR			2
#define CURVE		3
#define TITLEBAR	4

// commands
#define PING				0
#define NEW_GRAPH			1
#define GRAPH_SPEC			2
#define GRAPH_THRESHOLD		3
#define GRAPH_DATA			4
#define ERASE_GRAPH			5
#define DELETE_GRAPH		6
#define STATUS_FIELD		7
#define STATUS_VALUE		8
#define JOINT_START			9
#define JOINT_END			10
#define NEW_WORK_ORDER		11
#define JOINT_CSV			12

// signal classification
#define SIGNAL_UNKNOWN			0xb000		// questionable 1, background noise
#define SIGNAL_GOOD				0xb001		// ADP send once every second, amplitude undetermined
#define SIGNAL_QUESTIONABLE		0xb002		// questionable 2 and 3
#define SIGNAL_REJECT			0xb003		// reject

// mins and maxs
//#define MAX_LOGIN_TIMEOUT				 30	// database login window
#define MIN_SIGNAL_VALUE				  0
#define MAX_SIGNAL_VALUE				255
#define MIN_SIGNAL_ANGLE				  0
#define MAX_SIGNAL_ANGLE				359
#define MIN_SIGNAL_CLASSIFICATION		0xb000
#define MAX_SIGNAL_CLASSIFICATION		0xbfff
#define MIN_SEGMENTS_PER_MOTION_PULSE	  1
#define MAX_SEGMENTS_PER_MOTION_PULSE	 30
#define MIN_WALLMATRIX_VALUE			  0
#define MAX_WALLMATRIX_VALUE			255

#define MIN_EQUIPMENT_NUMBER			  0
#define MAX_EQUIPMENT_NUMBER			0XFFFF

#define MIN_SIGNAL_TYPE			  0
#define MAX_SIGNAL_TYPE			0XFFFF

#define MAX_DB_DATA						1024

#define MIN_GRAPH_NUM		1
#define MAX_GRAPH_NUM		8

#define MIN_GRAPH_TOP		0
#define MAX_GRAPH_TOP		1

#define MIN_THRESHOLD_VALUE	1
#define MAX_THRESHOLD_VALUE	99

#define MIN_THRESHOLD_TYPE	0
#define MAX_THRESHOLD_TYPE	3

#define MIN_AUTO_COLOR		0
#define MAX_AUTO_COLOR		1

#define MIN_TRACE_SIZE		1
#define MAX_TRACE_SIZE		8000

#ifdef  _MSC_VER        // defined for microsft compiler
// structure definitions
#include <afxsock.h>

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
	BYTE	SignalValue;			// amplitude 0 - 255
    long	SignalClassification;	// good, questionable, reject, etc.
    CString ProveUpValue;			// magnitude in inches
    long	ProveUpClassification;	// good, questionable, reject, etc.
} SIGNAL_DETAIL;

typedef struct {
	BYTE TableName;
	CString PipeNumberID;			// unique number assigned by database
	long EquipmentNumber;			// unique number assigned by database
	long SignalType;				// unique number assigned by database
    int	TraceLength;				// number of points in the trace
	CByteArray TraceArray;			// y values of a trace
} TRACE_DETAIL;

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

typedef struct{
	HWND hWnd;
	BOOL *lpKillThread;
	CSocket * pServiceSocket;
	CPtrList* pDataPacketList;
	CDocument * pDoc;
	int ViewerRef;
} THREAD_INFO;

typedef struct{
	BOOL *lpKillThread;
	CPtrList* pDataList;
	BOOL * bDBConnected;
	bool * bDBUpdateFailed;
	long * pTimeUsed;
} DB_THREAD_INFO;

typedef struct{
	CView * pView;
	int Height;
	int Width;
} WININFO;

typedef struct s_cal_link{
	CString JointDetailID;			// 
	CString CalibrationDetailID;	// 
	long EquipmentNumberInt;	    // 
} CAL_LINK;

/*typedef struct{
	GRAPHFIELD GraphField[MAXGRAPH];
} STATUS;

typedef union{
	int Cmd;
	unsigned Sequence;
	int Response;
	int ErrorCode;
} SENDBUFFER;
*/
#endif

#ifndef field
typedef struct{
	TCHAR Text[16];
	char Spec[8];
} FIELD;
#define field
#endif

typedef struct{
	int NumOfFld;
	FIELD Field[MAXFIELD];  // Fields for a graph
	float Value[MAXFIELD];
} GRAPHFIELD;

typedef struct{
	char str[16];
} NAME;

typedef struct{
	int Value[2];	// [0]: for Top trace, [1]: for Bottom trace
	int Type[2];
	int Color[2];
	NAME Name[2];
}THRESHOLD;

typedef struct{
	int Count[2];	// [0]: top, [1]: bottom
}TRACE_FLAW_COUNT;


typedef struct{
	unsigned ViewerRef;
	char Client[16];
	int ScreenLength;
	int Units;
	int GraphCount;
	float BarDrawWidth;
	int GraphType[MAXGRAPH];
	NAME GraphName[MAXGRAPH];
	int Double[MAXGRAPH];
	int MaxHeight[MAXGRAPH];
	int MinHeight[MAXGRAPH];
	THRESHOLD ThresholdRej[MAXGRAPH];
	THRESHOLD ThresholdQues[MAXGRAPH];
	int BeginY[MAXGRAPH];
	int SaveToDB;
	TRACE_FLAW_COUNT TraceFlaw[MAXGRAPH];
	//CString WorkOrderID;
	long EquipmentNumber;
	int	TotalTrace;
	int SignalType;
} STATE;

typedef struct{
	float Xloc;
	float Yloc;
	float Xloc1;
	float Yloc1;
	int Color;
} DATA_POINT;

#define DISPTIMER		300
#define PRERUNTIMER		301

/* line positions in logical units

	Titlebar: height 50
	Double Bar or Double Curve:
			0									begin
			2   ==============================  top thick line
				threshold line somewhere here
			102 ------------------------------  top baseline
			104 ------------------------------  bottom baseline
				threshold line somewhere here
			204 ==============================  bottom thick line
			214		|	|	|	|	|	|	    scale tick

	Single Bar, Single Curve, or map:
			0									Begin
			2   ==============================  top thick line
				threshold line somewhere here
			102 ------------------------------  top baseline
			104 ==============================  bottom thick line
			114		|	|	|	|	|	|	    scale tick
*/
#define LARGEFONTSIZEY  26// 44
#define LARGEFONTSIZEX  12 // 44
#define SMALLFONTSIZEY	20 //40
#define TINYFONTSIZEY	4 //40
#define WINDOWWIDTH		1920

#define TITLEBARHEIGHT	26 //50
#define TOPTHICKLINE	2
#define TOPBASELINE		102
#define BOTTOMBASELINE	104
#define BOTTOMTHICKLINE	204
#define SCALETICK		4
#define MIN_HEIGHT		20

#define ID_MY_STATUS_BAR 1010
#define CLOCKTIMER 1212


// for simulator
#define SIMTIMER	300
#define PIPELENGTH	20	// IN FEET
#define PIPESPACING	2	// IN FEET

#endif
