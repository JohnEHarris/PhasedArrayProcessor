// CommDemo Constants File
#ifndef __COMMDEMO_CONSTANTS
#define __COMMDEMO_CONSTANTS
// ==========================================================================

#define DEFAULT_IP_ADDR _T("192.168.10.10") //	_T("192.168.10.40") // _T("127.0.0.1")
#define SERVER_LISTEN_PORT	7502 // 0xfffa		// either a listening port or 
										// the port to connect to.

#define TIMER_10_SEC	8888	// send client watchdog every 10 sec
#define TIMER_11_SEC	6666	// check server watchdog every 11 sec 

#define MAX_MSG_SIZE	1040	// in bytes
typedef struct s_message{
	unsigned int MessageLength;	
	int	MessageID;
	unsigned int Sequence;
	TCHAR UnicodeStr[MAX_MSG_SIZE/2-6];
} DUMMY;

typedef struct s_message_pt{
	unsigned short spare[512];	
} TCPTOPT;


//-------------------------------------------------------------------------------
// Messages from Server (Xigang) to Client (Tuboscope)
typedef struct s_job_info{
	unsigned int MessageLength;	// =324
	int	MessageID;				// = 493101
	unsigned int Sequence;
	TCHAR	JobID[50];			// work order
	TCHAR	Lot[50];
	TCHAR	Heat[50];
	//TCHAR	Grade[50];
	//TCHAR	Customer[50];	// customer name
	//TCHAR	Standard[24];
	float	OD;				// outside diameter
	float	Wall;			// wall thickness
	float	Length;			// mm
	//float	TotalWeight;	// Total weight of all the pipes in the job
	//unsigned int TotalPipes;		// total number of pipes
} JOB_INFO;

typedef struct s_watchdog_server{
	unsigned int MessageLength;	// =16
	int	MessageID;				// = 493100
	unsigned int Sequence;
	unsigned int Counter;		// increment every 10 seconds
} SERVER_WATCHDOG;

//-------------------------------------------------------------------------------
// Messages from Client (Tuboscope) to Server (Xigang)
typedef struct s_req_job_info{
	unsigned short Sequence;
	unsigned short MessageLength;	// =16
	unsigned short MessageID;				// = 314901
	unsigned short UnitID;		// line 1 or 2
} REQ_JOB_INFO;

typedef struct s_pipe_result{
	unsigned int MessageLength;	// =228
	int	MessageID;				// = 314902
	unsigned int UnitID;		// line 1 or 2
	unsigned int Sequence;
	TCHAR	JobID[50];		// work order
	TCHAR	Lot[50];
	int		PipeNum;		// Tuboscope pipe number
	int		Classification;	// =0: Good; =1: Questionable; =2: Reject
	unsigned int InspectionSpeed;	// mm/s. line speed
} PIPE_RESULT;

typedef struct s_watchdog_client{
	unsigned int MessageLength;	// =20
	int	MessageID;				// = 314900
	unsigned int UnitID;		// line 1 or 2
	unsigned int Sequence;
	unsigned int Counter;		// increment every 10 seconds
} CLIENT_WATCHDOG;

typedef struct s_insp_parameter{
	unsigned int MessageLength;	// =160
	int	MessageID;				// = 314903
	unsigned int UnitID;		// line 1 or 2
	unsigned int Sequence;
	TCHAR	Lot[50];			// Lot is unique at Xigang
	float LineSpeed;			// mm/s
	float AmaMag;				// amps
	float SonoMag1;
	float SonoMag2;
	float SonoMag3;
	float SonoDeMag;
	int rpm;
	float QuesThold[2];			// [0]: ID, [1]: OD
	float RejThold[2];			// [0]: ID, [1]: OD
} INSP_PARAMETER;

#define NO_CONNECT	0
#define SERVER		1
#define CLIENT		2

#define DISABLE	0
#define ENABLE	1

#endif
// ==========================================================================