// Inspection Display Processor Constants File

#ifndef __MC_GDP_MESSAGES
#define __MC_GDP_MESSAGES

#define	TOP			0
#define BOTTOM		1
#define	TRACE_3		2
#define TRACE_4		3

#define	FEET		0
#define METERS		1

#define	LOG			0
#define LINEAR		1

#define SINGLE		0
#define UP_UP		1
#define UP_DOWN		2
#define MULTI		3

#define	ACTION_PLOT		0
#define	ACTION_SAVE		1
#define	ACTION_ERASE	2

// signal type
#define SIGNAL_MAP			0xa000

#define SIGNAL_THIN_WALL	0xa005
#define SIGNAL_THICK_WALL	0xa00e
#define SIGNAL_AVE_WALL		0xa017
#define LID					0xa001
#define LOD					0xa002
#define TID					0xa003
#define TOD					0xa004

#define SIGNAL_MIN_DIAMETER	0xa010
#define SIGNAL_AVE_DIAMETER	0xa018
#define SIGNAL_MAX_DIAMETER	0xa01f

// graph types
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
#define INVALID				99

// limits
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
#define MAX_TRACE_SIZE		3000

#define MIN_RPM				60
#define MAX_RPM				600

#define MIN_LINE_SPEED		1		//	ft/min
#define MAX_LINE_SPEED		400

// Table name
#define SIGNAL_DETAIL_TABLE		1
#define GRAPH_DETAIL_TABLE		2
#define WALLMATRIX_DETAIL_TABLE	3
#define TRACE_DETAIL_TABLE		4
#define JOINT_DETAIL_TABLE		5

// Structure definitions
#define MAXFIELD				5
#define MAX_CMD_SIZE			256
#define UNIQUE_ID_LENGTH		50

typedef struct{
	TCHAR Text[16];
	char Spec[8];
} FIELD;

typedef union{ 
	// Ping
	struct{
		int	Cmd;
		unsigned Sequence;
		int spare[10];
	} Cmd0;

	// New Graph
	struct{
		int	Cmd;
		unsigned Sequence;
		char Client[16];
		int	Length;
		int Units;
		int	GraphCount;
		float BarDrawWidth;
		long EquipmentNumber;
		//char WorkOrderID[16];
		int SaveToDB;
		int GdpMsgVer;		// 1
		int StopInspState;	// 0=stop, 1=run
		int SaveColor;		// RGB
		int NotSaveColor;	// RGB
		int StopColor;		// RGB
		int spare[10];
	} Cmd1;

	// Graph Specification
	struct{
		int	Cmd;
		unsigned Sequence;
		int ViewRef;
		int	GraphNum;
		int GraphType;
		TCHAR GraphName[16];
		int Double;
		int MaxHeight;
		int MinHeight;
		int DispMode;
		int	DispUnits;
		int	SignalTypeTop;
		int	SignalTypeBottom;
		int NumberTraces;
		int	SignalType3;
		int	SignalType4;
		int NominalWall;	// or NorminalOD when display diameter traces
		int spare[10];
	} Cmd2;

	// Graph Threshold
	struct{
		int	Cmd;
		unsigned Sequence;
		int ViewRef;
		int	GraphNum;
		int	Top;
		float Threshold;
		int ThresholdType;
		int ThresholdColor;
		TCHAR TraceName[16];
		int	AutoColor;
		TCHAR ThresholdLabel[16];
		int	SignalType;
		int spare[10];
	} Cmd3;

	// Graph Data
	struct{
		int	Cmd;
		unsigned Sequence;
		int ViewRef;
		int	GraphNum;
		int Top;
		int Action;
		float Xloc;
		float Yloc;
		float Xloc1;
		float Yloc1;
		int Color;
		int Sensor;
		int Angle;
		int SignalType;
		int SignalClassification;
		TCHAR PipeNumberID[UNIQUE_ID_LENGTH];
		float MPLength;
	} Cmd4;

	// Erase Graph
	struct{
		int	Cmd;
		unsigned Sequence;
		int ViewRef;
		int	GraphNum;
		int spare[10];
	} Cmd5;

	// Delete Graphs
	struct{
		int	Cmd;
		unsigned Sequence;
		int ViewRef;
		int spare[10];
	} Cmd6;

	// Status Field
	struct{
		int	Cmd;
		unsigned Sequence;
		int ViewRef;
		int	GraphNum;
		int NumOfFld;
		FIELD Field[MAXFIELD];
	} Cmd7;

	// Status Values
	struct{
		int	Cmd;
		unsigned Sequence;
		int ViewRef;
		int	GraphNum;
		int FieldNum;
		float Value;
		TCHAR ValueStr[50];
		int spare[10];
	} Cmd8;

	// Joint Start 
	struct{ 
		int	Cmd;
		unsigned Sequence;
		int ViewRef;
		TCHAR PipeNumberID[UNIQUE_ID_LENGTH];
		int LineSpeed;
		int HeadRpm;
		int SaveToDB;
		int DispUnits;
		int DispMode;
		int NominalWall;	// or NorminalOD when display diameter traces
		int spare[10]; 
	} Cmd9;

	// Joint End
	struct{ 
		int		Cmd;
		unsigned Sequence;
		int		ViewRef;
		TCHAR	PipeNumberID[UNIQUE_ID_LENGTH];
		float	PipeLength;
		int		WallDropoutCount;
		float	WallDropoutDuration;
		int		RunTime;
		int		RPM;
		int		Prf;
		int		AbsDropout;
		int		AbsShoeDropout_0;
		int		AbsShoeDropout_1;
		int		AbsShoeDropout_2;
		int		AbsShoeDropout_3;
		int		ShoeDropout_0;
		int		ShoeDropout_1;
		int		ShoeDropout_2;
		int		ShoeDropout_3;
		float	LineSpeed;		// feets per minute
		float	MagEntry1;		// Sonoscope entry coil 1 mag current or Amalog mag current
		float	MagEntry2;		// Sonoscope entry coil 2 mag current
		float	MagExit;		// Sonoscope exit coil mag current
		float	Demag;			// Demag current
		float	MinWall;
		float	MaxWall;
		float	MeanWall;
		float	Start;			// IE start location in feet
		float	Stop;
	} Cmd10;

	// Dummy
	struct{
		USHORT dummy[MAX_CMD_SIZE/2];
	} Cmd11;

} RECVBUFFER;


#endif
