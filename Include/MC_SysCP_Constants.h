// Mill Console Constants File
/******************************************************************************
Version	Date		Author	Description
-------------------------------------------------------------------------------
8.2.0.0 06/14/2012	Y.Chen	Add Phase Array Wall Monitor.
8.1.0.0 06/01/2012	Y.Chen	Add capability to back up 4 Youngstown tables to AccessDB.
							Add registry for Location, Export DB directory, and 
							Station ID for Youngstown.
8.0.0.1 05/23/2012	Y.Chen	Fixed Shrink/Expand malfunction problem by set WinVer
							back to 0x0502
8.0.0.0 05/17/2012	Y.Chen	Port to VS2010. If using other languages (not English), 
							the Work Order field only exclude:  \ / : * ? " < > |
7.4.0.0 04/26/2012	Y.Chen	(English only) Check Work Order field: length <=49,
							trim space on both ends, only allows a-z, A-Z, 0-9, 
							space, -, _, and .
7.3.0.0 04/02/2012	Y.Chen	Increase the height of the Client Stataus Icon Bar 
							again (to avoid crush) to accomodate font size increase
							on Windows 7. 
7.2.0.0 03/26/2012	Y.Chen	Lot of times, the GraphRange parameter is not ready 
							to use when CConnectionThread::SendGDPParameterDetail() 
							is called. That results in wrong GraphRande sent when 
							changing Active Work Order. Add a 100ms delay solved it.
7.1.0.1 09/01/2011	Y.Chen	Fixed bugs at Startup Splash. Translator deleted some options and translated database connection string.
7.1.0.0 08/26/2011	Y.Chen	Add Italian UI.
7.0.0.0 07/29/2011	Y.Chen	Allow CIP to control the Start/Stop inspection.
6.1.0.0 04/04/2011	Y.Chen	Allow creation of equipment number bigger then 100001.
6.0.0.2 03/07/2011	Y.Chen	Randy Knopsnider requests OD size increase up to 30" for Lorain.
6.0.0.1 02/18/2011	Y.Chen	Use EquipmentNumber to decide station position
							instead of using FamilyName (only 1 name for all Sonoscope).
6.0.0.0 02/11/2011	Y.Chen	Port to Win 7: Automatically change listening port. 
5.5.0.0 10/18/2010	Y.Chen	Increase the height of the Client Stataus Icon Bar 
							to avoid crush. 
5.4.0.0 08/24/2010	Y.Chen	Add capability to back up ProveUP table to AccessDB.
5.3.0.0 06/07/2010	Y.Chen	Killed a bug of checking unique wo, lot, heat 
							combination when edit a Work Order.
5.2.0.0 10/09/2009	Y.Chen	Stop insp if time interval between two new pipe req
							is smaller than min. 
5.1.0.1 07/31/2009	Y.Chen	Ensure the WO, Heat, Lot combination is unique. 
							Make the new WO as the active WO.
5.1.0.0 07/10/2009	Y.Chen	Add Chinese user interface. 
5.0.0.1 12/01/2008	Y.Chen	Max joint number increases from 9999 to 99999. 
5.0.0.0 10/02/2008	Y.Chen	Read database name, server name and help file names 
							from MCConfig.ini file.
4.1.0.0 06/12/2008	Y.Chen	Add Russian string to Active Job. New language DLLs.
4.0.0.1	05/21/2008	Y.Chen	Lower the min MPLength limit from 0.45" to 0.3".
4.0.0.0	04/04/2008	Y.Chen	Access DB every minute to keep the communication
							alive. Change Start, Stop button pair to only one 
							button visible at a time.
3.0.0.4	11/05/2007	R.Harris	Original not-Profibus SysCP from Vault.
*******************************************************************************
ONLY PROFIBUS VERSION OF SYSCP HAS CAPABILITY OF LOG MESSAGES INTO A LOCAL LOG 
FILE AND SEND INSPECTION STATION'S STATUS (STOP, INSPECT AND CALIBRATE) TO CIP.
******************************************************************************/

#ifndef __MCSCP_CONSTANTS
#define __MCSCP_CONSTANTS

#define LOCAL_HOST			"localhost"
#define MC_DBS_HOSTNAME		"mc-dbs"		// database server
#define MC_SCP_HOSTNAME		"mc-scp"		// System Control processor

#define MC_ACP_HOSTNAME		"mc-acp"	// amalog command processor
#define MC_PRO_HOSTNAME		"mc-pt"		// protocol translator
#define MC_SYSCP_HOSTNAME	"mc-syscp" // "mc-syscp1", "mc-syscp2"	// system control processor


// path to online manual for SCP
// this is a pdf file
#define DEFMANUALDISKDRIVE	"\\\\mc-dbs/"
#define DEFMANUALDIRECTORY	"AppExesFolder/Tuboscope On-screen Manual-Dwgs/"
#define DEFMANUALNAME		"05 SCP System Control"
#define DEFMANUALEXT		".pdf"

#define GENERAL			1
#define VM_YOUNGSTOWN	2	// 4 extra tables, export to a special name and location

//#define LOCATION		GENERAL

// path to the MCConcig.ini file.
// it should be the parent directory of the program.
#define MC_CONFIG_DIR	_T("..\\")
// this is the starting place for app 
// message definition
#define WM_SCP_APP 0x8000			

// messages views are interested in
#define	WM_INSERT_LOGMESSAGE_ITEM					(WM_SCP_APP + 0)
#define	WM_BEGIN_STARTUP							(WM_SCP_APP + 1)
#define WM_GET_FIRST_STATION						(WM_SCP_APP + 2)
#define WM_GET_LAST_STATION							(WM_SCP_APP + 3)
#define WM_GRANT_DBADMIN_ACCESS						(WM_SCP_APP + 4)
#define WM_STATION_STATUS_CHANGED					(WM_SCP_APP + 5)
#define WM_UPDATE_ID_STRUCTURE						(WM_SCP_APP + 6)
#define WM_CHECK_FOR_PIPE_DESCRIPTION				(WM_SCP_APP + 7)
#define WM_UPDATE_ACK_NAK_STRUCTURE					(WM_SCP_APP + 8)
#define WM_SEND_ACK_NAK_MESSAGE						(WM_SCP_APP + 9)
#define WM_SET_MODE_MESSAGE							(WM_SCP_APP + 120)


#define WM_REQUEST_CLIENT_IDENTITY					(WM_SCP_APP + 10)
#define WM_ACCEPT_CLIENT_IDENTITY					(WM_SCP_APP + 11)

#define WM_INSERT_CLIENTIDENTITY_ITEM				(WM_SCP_APP + 20)
#define WM_REMOVE_CLIENTIDENTITY_ITEM				(WM_SCP_APP + 21)

#define WM_INSERT_CONNECTIONTREEVIEW_ITEM			(WM_SCP_APP + 30)
#define WM_REMOVE_CONNECTIONTREEVIEW_ITEM			(WM_SCP_APP + 31)
#define WM_REVISE_CONNECTIONTREEVIEW_ITEM			(WM_SCP_APP + 32)

#define WM_INSERT_PIPENUMBER_ITEM					(WM_SCP_APP + 33)
#define WM_REMOVE_PIPENUMBER_ITEM					(WM_SCP_APP + 34)
#define WM_GET_PIPENUMBER_ITEM						(WM_SCP_APP + 35)

#define WM_INSERT_ALARMDETAIL_ITEM					(WM_SCP_APP + 40)
#define WM_REMOVE_ALARMDETAIL_ITEM					(WM_SCP_APP + 41)
#define WM_REVISE_ALARMDETAIL_ITEM					(WM_SCP_APP + 42)
#define WM_UPDATE_ALARM_BUTTON						(WM_SCP_APP + 43)
#define WM_UPDATE_ALARM_MASK						(WM_SCP_APP + 44)
#define WM_UPDATE_ALARM_MUTE						(WM_SCP_APP + 45)
#define WM_UPDATE_ALARM_CAPTION						(WM_SCP_APP + 46)

// messages clients are interested in
#define WM_REQUEST_CLIENT_START						(WM_SCP_APP + 50)
#define WM_REQUEST_CLIENT_STOP						(WM_SCP_APP + 52)
#define WM_REQUEST_CLIENT_RECONFIGURE				(WM_SCP_APP + 53)
#define WM_REQUEST_CLIENT_PRINT_CONFIGURATION		(WM_SCP_APP + 54)
#define WM_REQUEST_CLIENT_EXPORT_CONFIGURATION		(WM_SCP_APP + 55)
#define WM_REQUEST_CLIENT_PRINT_LOGFILE				(WM_SCP_APP + 56)
#define WM_REQUEST_CLIENT_EXPORT_LOGFILE			(WM_SCP_APP + 57)

// used by toolbars and dialog bars
#define WM_INIT_ACTIVE_WORKORDER					(WM_SCP_APP + 58)
#define WM_INIT_ACTIVE_HEAT							(WM_SCP_APP + 59)
#define WM_INIT_ACTIVE_LOT							(WM_SCP_APP + 60)
#define WM_INIT_ACTIVE_DATE							(WM_SCP_APP + 61)
#define WM_INIT_JOB_SELECTOR						(WM_SCP_APP + 62)
#define WM_INIT_PIPESEQUENCE_DIALOGBAR				(WM_SCP_APP + 63)

#define WM_SEND_PIPENUMBER_DETAIL					(WM_SCP_APP + 64)
#define WM_SEND_GDP_PARAMETER_DETAIL				(WM_SCP_APP + 65)
#define WM_SEND_MODIFIED_ALARM_DETAIL				(WM_SCP_APP + 66)
#define WM_HIDE_DATABASE_TOOLBAR_BUTTON				(WM_SCP_APP + 67)
#define WM_HIDE_CLIENT_CONTROL_TOOLBAR_BUTTON       (WM_SCP_APP + 68)

// used by pipe number custodians
#define WM_STATION_HANDLED_PIPE						(WM_SCP_APP + 69)
#define WM_INCREMENT_PIPE_SEQUENCE_NUMBER			(WM_SCP_APP + 70)
#define WM_DECREMENT_PIPE_SEQUENCE_NUMBER			(WM_SCP_APP + 71)
#define WM_INCREMENT_PIPE_SUFFIX					(WM_SCP_APP + 72)
#define WM_DECREMENT_PIPE_SUFFIX					(WM_SCP_APP + 73)

// used by report generator
#define WM_SEND_REPORT_GENERATOR_DETAIL				(WM_SCP_APP + 74)
#define WM_CHECK_COMMAND_ROUTE_ENABLE				(WM_SCP_APP + 75)
#define WM_UPDATE_MONITOR_ICON						(WM_SCP_APP + 76)
#define WM_UPDATE_TREE_COMMAND_ROUTING_CHECKBOXES	(WM_SCP_APP + 77)

// used by client status splitter pane
#define WM_INSERT_CLIENT_STATUS_ITEM				(WM_SCP_APP + 78)
#define WM_REMOVE_CLIENT_STATUS_ITEM				(WM_SCP_APP + 79)
#define WM_SET_CLIENT_STATUS_ITEM_IMAGE				(WM_SCP_APP + 80)
#define WM_GET_CLIENT_OPERATING_STATUS				(WM_SCP_APP + 81)

// used by remote operator logic
#define	WM_INSERT_REMOTE_OPERATOR_ITEM		(WM_SCP_APP + 82)
#define WM_REVISE_REMOTE_OPERATOR_ITEM		(WM_SCP_APP + 83)
#define WM_PROCESS_REMOTE_OPERATOR_ITEM		(WM_SCP_APP + 84)
#define WM_REQUEST_JOB_CHANGE				(WM_SCP_APP + 85)
#define WM_SEND_REMOTE_OPERATOR_DETAIL      (WM_SCP_APP + 86)
#define WM_GET_RUN_STATUS				    (WM_SCP_APP + 87)
#define WM_INIT_REMOTE_OPERATOR			    (WM_SCP_APP + 88)

// operator list messages
#define WM_UPDATE_OPERATOR_LIST				(WM_SCP_APP + 89)

// notice the gap between numbers
#define WM_SELECT_LANGUAGE					(WM_SCP_APP + 98)
#define WM_LANGUAGE_CHANGED					(WM_SCP_APP + 99)

/*****************/
/*  !!!NOTICE!!! */
/*****************/
// these must be contiguous
#define WM_SWITCHTO_LOGFILE_VIEW				(WM_SCP_APP + 100)
#define WM_SWITCHTO_EQUIPMENTDESCRIPTION_VIEW	(WM_SCP_APP + 101)
#define	WM_SWITCHTO_ROTARYSENSOR_VIEW			(WM_SCP_APP + 102)
#define WM_SWITCHTO_STATIONARYSENSOR_VIEW		(WM_SCP_APP + 103)
#define WM_SWITCHTO_SIGNALTYPES_VIEW			(WM_SCP_APP + 104)
#define WM_SWITCHTO_SIGNALCLASSES_VIEW			(WM_SCP_APP + 105)
#define WM_SWITCHTO_WALLMATRIXTYPES_VIEW		(WM_SCP_APP + 106)
#define WM_SWITCHTO_JOBDESCRIPTION_VIEW			(WM_SCP_APP + 107)
#define WM_SWITCHTO_PIPEDESCRIPTION_VIEW		(WM_SCP_APP + 108)
#define WM_SWITCHTO_CONNECTION_VIEW				(WM_SCP_APP + 109)
#define WM_SWITCHTO_VERSION_VIEW				(WM_SCP_APP + 110)
#define WM_SWITCHTO_EQUIPMENT_VIEW				(WM_SCP_APP + 111)
#define WM_SWITCHTO_ALARM_VIEW					(WM_SCP_APP + 112)
#define WM_SWITCHTO_ALARM_PANEL1_VIEW			(WM_SCP_APP + 113)
#define WM_SWITCHTO_ALARM_PANEL2_VIEW			(WM_SCP_APP + 114)
#define WM_SWITCHTO_ALARM_PANEL3_VIEW			(WM_SCP_APP + 115)
#define WM_SWITCHTO_ALARM_PANEL4_VIEW			(WM_SCP_APP + 116)
#define WM_SWITCHTO_PIPEDISPOSITIONTYPES_VIEW	(WM_SCP_APP + 117)
#define WM_SWITCHTO_DATABASE_MAINTENANCE_VIEW   (WM_SCP_APP + 118)
#define WM_SWITCHTO_CALIBRATION_TEMPLATES_VIEW  (WM_SCP_APP + 119)

/**************/
/* END NOTICE */
/**************/
// splitter locators
#define TOP_VIEW_ROW			0
#define BOTTOM_VIEW_ROW			1

#define LEFT_VIEW_PANE			0
#define RIGHT_VIEW_PANE			1

// conversions
#define INCHES_TO_MILLIMETERS					25.4
#define POUND_PER_FOOT_TO_KILOGRAMS_PER_METER	1.4882
#define FEET_PER_MINUTE_TO_METERS_PER_SECOND    0.00508

// timer IDs
#define REMOTE_OPERATOR_DETAIL_LIST_DEQUEUE_TIMER	99
#define ONE_MINUTE_DB_ACCESS_TIMER					1341

// program generics
#define ACK								   0x06	// acknowledge
#define NAK								   0x15	// negative acknowledge
#define MAX_THREAD_NAP_TIME					 10	// max time to allow thread napping after first PostThreadMessage failure
#define MAX_FQDN_LENGTH						 64	// max length of a fully qualified domain name
#define CONSISTENT_DATE_FORMAT			_T("%x %X %p")
#define MAX_PASSWORD_LENGTH					 24	// max chars in a password
#define DEF_MOTION_PULSE_LENGTH			   0.50	// as its name implies
#define MIN_MOTION_PULSE_LENGTH			   0.30 //0.45	// smallest motion pulse length
#define MAX_MOTION_PULSE_LENGTH			   1.25	// largest motion pulse length
#define SZATTRIBUTE_BUFSIZE				   1024	// size of the temporary string buffer
												// used by CExportJobToAccess
#define MAX_SP_VARCHAR_LENGTH				100	// default character count in a typical varchar
#define MIN_STARTUP_PROGRESS_RANGE		  	  0	// min range of the startup progress bar
#define MAX_STARTUP_PROGRESS_RANGE			100	// max range of the startup progress bar
#define MAX_GENERIC_COMMENT_LENGTH			511	// max length of job description comment
//#define JOB_DLGBAR_COMBOBOX_DROPWIDTH		250   // width of all combobox drop list in the workorder dialog bar
#define OPERATOR_DLGBAR_COMBOBOX_DROPWIDTH	250 // width of operator selector drop list in the select operator dialog bar


// color definitions
#define TUBOGREEN		RGB(0,128,0)		// an approximation, of course
#define TUBOSAND		RGB(206,186,159)	// an approximation, of course
#define	BLACK			RGB(0,0,0)
#define WHITE			RGB(255,255,255)
#define RED				RGB(255,0,0)
#define YELLOW			RGB(255,255,0)
#define GREEN			RGB(0,255,0)
#define GREENBAR		RGB(200,255,200)
#define TRANSPARENCY	RGB(255,0,255)		// magenta is used to indicate transparency


// generic bitmap ordinal positions
#define BMP_ZERO			   0	// position  0	- numeral 0
#define BMP_ONE				   1	// position  1	- numeral 1
#define BMP_TWO				   2	// position  2  - numeral 2
#define BMP_THREE			   3	// position  3	- numeral 3
#define BMP_FOUR			   4	// position  4	- numeral 4
#define BMP_FIVE			   5	// position  5	- numeral 5
#define BMP_SIX				   6	// position  6	- numeral 6
#define BMP_SEVEN			   7	// position  7	- numeral 7
#define BMP_EIGHT			   8	// position  8	- numeral 8
#define BMP_NINE			   9	// position  9	- numeral 9
#define BMP_GOOD			  10	// position 10	- green check denotes success
#define BMP_BAD				  11	// position 11	- red X denotes failure

// pipe description requestors
#define MAX_PIPE_DESCRIPTION_ITEMS	   3	// 
#define PIPE_DESCRIPTION_OD			   0	// request pipe's outside diameter
#define PIPE_DESCRIPTION_NOMINAL_WALL  1	// request pipe's nominal wall
#define PIPE_DESCRIPTION_RANGE		   2	// request pipe's length range

// database generics
#define MAX_SCP_CONNECTION_COUNT	10		// simultaneous connection max counts
#ifdef OLD_STRUCT_DEFS
#define MAX_LOGIN_TIMEOUT			120		// login timeout in seconds
#define MAX_QUERY_TIMEOUT			120		// query timeout in seconds
#endif

#define MAX_FAMILY_NAMES			    32	// equipment family names
#define MIN_UNIT_ID					     0
#define MAX_UNIT_ID				 	   999	 
#define MIN_PIPESEQUENCE_NUMBER		     1  // cannot be zero
#define MAX_PIPESEQUENCE_NUMBER	    100000	// max 10000 pipes
#define MIN_PIPESEQUENCE_SUFFIX			 1	// represents letter A
#define MAX_PIPESEQUENCE_SUFFIX			26  // represents letter Z
#define MIN_EQUIPMENT_POSITION			 0	// this is effectively the station number
#define MAX_EQUIPMENT_POSITION			10	// this is effectively the station number

// Equipment types
#define ROTARY_EQUIPMENT_TYPE			 0	// equipment has rotating shoes
#define STATIONARY_EQUIPMENT_TYPE		 1	// equipment has stationary shoes


// view generics
#define LISTVIEW_COLUMN0_WIDTH				   50	// column 0 in any list view is the row number
#define VERSIONVIEW_COLUMN_COUNT			   12	// # columns in CVersionView list control
#define WALLMATRIXTYPEVIEW_COLUMN_COUNT			4	// # columns in CWallMatrixView list control
#define STATIONARYSENSORVIEW_COLUMN_COUNT	   11	// # columns in CStationarySensorView list control
#define SIGNALTYPEVIEW_COLUMN_COUNT				4	// # columns in CSignalTypeView list control
#define SIGNALCLASSVIEW_COLUMN_COUNT			4	// # columns in CSignalCallView list control
#define ROTARYSENSORVIEW_COLUMN_COUNT		   11	// # columns in CRotarySensorView list control
#define PIPEDESCRIPTIONVIEW_COLUMN_COUNT	   10   // # columns in CPipeDescriptionView list control
#define LOGFILEVIEW_COLUMN_COUNT				5	// # columns in CLogfileView list control
#define JOBDESCRIPTIONVIEW_COLUMN_COUNT		   13	// # columns in CJobDescriptionView list control
#define EQUIPMENTVIEW_COLUMN_COUNT				6	// # columns in CEquipmentView list control
#define EQUIPMENTDESCRIPTIONVIEW_COLUMN_COUNT	8	// # columns in CEquipmentDescriptionView list control
#define PIPEDISPOSITIONSVIEW_COLUMN_COUNT       4	// # columns in CPipeDispositionsTypeView list control
#define ALARMVIEW_COLUMN_COUNT					8	// # columns in CAlarmView list control
#define CONNECTIONVIEW_COLUMN_COUNT             8   // # columns in CConnectionView list control
#define DEVICECONNECTIONVIEW_COLUMN_COUNT		4	// # columns in CDeviceConnectionView list control
#define CALIBRATONTEMPLATEVIEW_COLUMN_COUNT	    7	// # columns in CCalibrationTemplateView list control
#define GENERATEPIPEDESCRIPTION_COLUMN_COUNT    7	// # columns in CGeneratePipeDescription list control
#define GENERATEJOBDESCRIPTION_COLUMN_COUNT    11   // # columns in CGenerateJobDescription list control
#define JOBSELECTOR_COLUMN_COUNT                7   // # columns in JobSelector list control
#define EXPORT_JOBSELECTOR_COLUMN_COUNT         7   // # columns in ExportToAccess JobSelector list control

// mins and maxs used mostly for data validation
#define MIN_PIPE_DIAMETER			1.5	
#if 0	
#define MAX_PIPE_DIAMETER			20.0			
#else
#define MAX_PIPE_DIAMETER			30.0			
#endif
#define MIN_PIPE_NOMINAL_WALL		0.1
#define MAX_PIPE_NOMINAL_WALL		2.00
#define MIN_PIPE_WEIGHT_PER_UNIT	1.0
#define MAX_PIPE_WEIGHT_PER_UNIT	200.0

#define MIN_SHOE_ANGLE			   0		// minimum shoe angle expressed in whole degrees
#define MAX_SHOE_ANGLE			 359		// maximum shoe angle expressed in whole degrees
#define MAX_ALARM_BUTTONS		  16        // # buttons on alarm panel at any time
#define	MAX_EQUIPMENT_ALARMS	  64		// each connected equipment can have this many alarms
#define MAX_CONNECTION_COUNT	  10		// sustain max of 10 simultaneous connections
#define MAX_PACKET_LENGTH		8000		// was 6144 max bytes in a packet
#define CENTURY_CORRECTION		1900		// used to correct year numbers in tm's

// used for validation prior to database record entry
#define MIN_SIGNALTYPES_TYPE		0xa000
#define MAX_SIGNALTYPES_TYPE		0xa7ff		// 2048 available types

#define MIN_SIGNALCLASS_TYPE		0xb000
#define MAX_SIGNALCLASS_TYPE		0xbfff		// 4096 available types

#define MIN_WALLMATRIX_TYPE			0xc000
#define MAX_WALLMATRIX_TYPE			0xcfff		// 4096 available types

#define MIN_PIPEDISPOSITION_TYPE	0xd000
#define MAX_PIPEDISPOSITION_TYPE	0xdfff		// 4096 available types

// port definitions for all the servers
#define MC_SQL_LISTENPORT		1433		// database server listens on this port

// for Windows XP
#define	MC_SYSCP_LISTENPORT 	0xc000		// 0xfff0(65520), 0xc000	// system control processor listens on this port
#define MC_ICP_LISTENPORT		0xc001		// inspection command processors listen on this port
#define MC_IDP_LISTENPORT		0xc002		// inspection display processors listen on this port
#define MC_PRO_LISTENPORT		0xc003		// protocol translator listens on this port
#define MC_RPG_LISTENPORT		0xc004		// report generator listens on this port
#define MC_PRV_LISTENPORT		0xc005		// proveup stations listen on this port

// for Windows 7
#define	MC_SYSCP_LISTENPORT_WIN7 	0xfff0		// 0xfff0(65520), 0xc000	// system control processor listens on this port
#define MC_ICP_LISTENPORT_WIN7		0xfff1		// inspection command processors listen on this port
#define MC_IDP_LISTENPORT_WIN7		0xfff2		// inspection display processors listen on this port
#define MC_PRO_LISTENPORT_WIN7		0xfff3		// protocol translator listens on this port
#define MC_RPG_LISTENPORT_WIN7		0xfff4		// report generator listens on this port
#define MC_PRV_LISTENPORT_WIN7		0xfff5		// proveup stations listen on this port

// message types, flags, and other message related thingies
// message type
#define COMMAND_MESSAGE					0x1000	// message contains command
#define DATA_MESSAGE					0X1001	// message contains data
#define STATUS_MESSAGE					0x1002  // message contains status
#define ALARM_MESSAGE					0x1003	// message contains alarm
#define LOG_MESSAGE						0x1004	// message contains log entry
#define PIPE_NUMBER_MESSAGE				0x1005	// message contains complete pipe number
#define GDP_PARAMETER_MESSAGE			0x1006	// message contains parameters needed by GDP
#define REPORT_GENERATOR_MESSAGE		0x1007	// message contains information needed by the RPG
#define REMOTE_OPERATOR_MESSAGE			0x1008	// message contains remote job/pipe descriptions
#define ACK_NAK_MESSAGE					0x1009	// message contains ack or nak of a received message
#define SET_MODE_MESSAGE				0x100a	// message contains START/STOP command from CIP
#define UNKNOWN_MESSAGE					0x1fff  // message type not defined

// message content
#define COMMAND_DETAIL_MESSAGE				0x2000	// content is a command w/ parameters
#define CLIENT_IDENTITY_DETAIL_MESSAGE		0x2001  // content is client identity detail
#define STATUS_DETAIL_MESSAGE				0x2002	// content is status detail
#define ALARM_DETAIL_MESSAGE				0x2003  // content is client alarm detail
#define LOG_DETAIL_MESSAGE					0x2004  // content is a log detail message
#define PIPE_NUMBER_DETAIL_MESSAGE			0x2005	// content is a pipe number detail message
#define GDP_PARAMETER_DETAIL_MESSAGE		0x2006	// content is a gdp parameter detail message
#define REPORT_GENERATOR_DETAIL_MESSAGE		0x2007	// content is a rpg detail message
#define REMOTE_OPERATOR_DETAIL_MESSAGE		0x2008	// content is a remote operator detail message
#define ACK_NAK_DETAIL_MESSAGE				0x2009	// content is a message ack / nak
#define SET_MODE_DETAIL_MESSAGE				0x200a	// content is a START/STOP command from CIP
#define UNKNOWN_DETAIL_MESSAGE				0x2fff	// content is undefined

// Used by MC_SCP
// originators and targets, (range 0xe000 - 0xefff)

#define	AMALOG_COMMAND_PROCESSOR				0xe000  // amalog family equipment 
#define AMALOG_DISPLAY_PROCESSOR				0xe001	// amalog family equipment

#define SONOSCOPE_COMMAND_PROCESSOR				0xe100  // sonoscope family equipment
#define SONOSCOPE_DISPLAY_PROCESSOR				0xe101	// sonoscope family equipment
 
#define TRUSCOPE_COMMAND_PROCESSOR				0xe200  // truscope family equipment
#define TRUSCOPE_DISPLAY_PROCESSOR				0xe201	// truscope family equipment

#define PHASE_ARRAY_WALL_COMMAND_PROCESSOR		0xe210  // phased wall like truscope family equipment
#define PHASE_ARRAY_WALL_DISPLAY_PROCESSOR		0xe211	// phased wall like truscope family equipment

#define ROTARY_WALL_MONITOR_COMMAND_PROCESSOR	0xe300  // rotary wall monitor family equipment 
#define ROTARY_WALL_MONITOR_DISPLAY_PROCESSOR   0xe301	// rotary wall monitor family equipment

#define	OVALITY_COMMAND_PROCESSOR				0xe400  // ovality family equipment
#define OVALITY_DISPLAY_PROCESSOR				0xe401	// ovality family equipment

#define END_AREA_COMMAND_PROCESSOR				0xe500  // end area family equipment
#define END_AREA_DISPLAY_PROCESSOR				0xe501	// end area family equipment

#define EDDY_CURRENT_COMMAND_PROCESSOR			0xe600  // eddy current family equipment
#define EDDY_CURRENT_DISPLAY_PROCESSOR			0xe601	// eddy current family equipment

#define ECCENTRICITY_COMMAND_PROCESSOR			0xe700	// eccentricity family equipment
#define ECCENTRICITY_DISPLAY_PROCESSOR			0xe701	// eccentricity family equipment

#define WELLCHEK_COMMAND_PROCESSOR				0xe800	// well inspection family equipment
#define WELLCHEK_DISPLAY_PROCESSOR				0xe801	// well inspection family equipment

#define REPORT_GENERATOR						0xe900	// report generator
#define PROVE_UP_STATION					    0xea00	// prove up station 
#define SYSTEM_CONTROL_PROCESSOR				0xeb00  // SCP system control processor
#define LINE_CONTROLLER							0xec00	// line controller
#define PROTOCOL_TRANSLATOR						0xed00	// protocol translator

#define UNUSED_COMMAND_PROCESSOR_1			    0xee00	// not assigned
#define UNUSED_DISPLAY_PROCESSOR_1				0xee01	// not assigned

#define UNUSED_COMMAND_PROCESSOR_2				0xef00	// not assigned
#define UNUSED_DISPLAY_PROCESSOR_2				0xef01	// not assigned

#define REMOTE_OPERATOR							0xefa0	// remote operator

#define UNKNOWN_COMMAND_PROCESSOR				0xeffe	// unknown equipment
#define UNKNOWN_DISPLAY_PROCESSOR				0xefff	// unknown equipment
					

// status (range - 0xe000 - 0xefff)
#define PIPE_ENTERED_UNIT					    0xe000	// pipe has entered an inspection station
#define PIPE_EXITED_UNIT						0xe001	// pipe has exited an inspection station
#define PIPE_IN_UNIT							0xe002	// pipe is in inspection unit
#define	STATION_IS_RUNNING						0xe003	// inspection station is in run mode
#define STATION_IS_CALIBRATING					0xe004	// inspection station is calibrating
#define STATION_IS_STOPPED						0xe005	// inspection station is stopped
#define STATION_IS_NOT_ROUTED					0xe006	// inspection station does not receive commands
#define UNKNOWN_STATUS							0xefff	// don't know what the status is

// commands to clients, etc (range - 0xf000 - 0xffff)
#define	RECONFIGURE								0xf000	// commands target to reconfigure itself
#define	PRINT_CONFIGURATION						0xf001	// commands target to print its current configuration
#define EXPORT_CONFIGURATION					0xf002	// commands target to export its current configuration
#define START_INSPECTION						0xf003	// commands target to start inspection
#define COMMAND_NOT_ASSIGNED					0xf004	// no command
#define STOP_INSPECTION							0xf005	// commands target to stop inspection
#define PRINT_LOGFILE							0xf006	// commands target to print its logfile
#define EXPORT_LOGFILE							0xf007	// commands target to export its logfile
#define WHO_GOES_THERE					        0xf008  // commands target to fully identify itself
#define ACCEPT_PIPE_NUMBER						0xf009	// commands target to accept pipe number 
#define ACCEPT_GDP_PARAMETERS					0xf00a	// commands target to accept parameters needed by GDP
#define ACCEPT_REMOTE_OPERATOR_PARAMETERS		0xf00b	// commands target to accept remote operator detail

// handy structures to have around
/**************************************************/
/* structure to hold the maximum number of bytes  */
/* retrieved from a socket.  used to fetch socket */
/* data without having to know what the data is   */
/*                                                */
/* Originator and MessageType are used to parse   */
/* the message so that it may be directed to an   */
/* appropriate message handler                    */
/**************************************************/
typedef struct {
   USHORT Originator;			// entity that originated message
   USHORT Target;				// entity that should respond to message
   USHORT MessageType;			// what does this message contain
   USHORT MessageContent;		// what structure is contained in message
   UCHAR Spare[MAX_PACKET_LENGTH - (4 * sizeof(USHORT))];
} TCPDUMMY;

/***************************************/
/* This structure is used to ACK/NAK   */
/* any message whose type is contained */
/* in MessageTypeToAcknowledge         */
/*************************************************/
/* Example:							             */
/* to acknowledge a pipe_entered_unit which was  */
/* sent to the SysCP by the ACP, fill the        */	 
/* structure as follows then send the ack/nak    */
/* message back to the ACP                       */
/*                                               */
/* Originator = SYSTEM_CONTROL_PROCESSOR;        */
/* Target = AMALOG_COMMAND_PROCESSOR;            */
/* MessageType = ACK_NAK_MESSAGE;                */
/* MessageContent = ACK_NAK_MESSAGE_DETAIL;      */
/* MessageTypeToAcknowledge = PIPE_ENTERED_UNIT; */
/* Response = ACK                                */
/*************************************************/
   
typedef struct {
   USHORT Originator;				// entity that originated message
   USHORT Target;					// entity that should respond to message
   USHORT MessageType;				// what does this message contain
   USHORT MessageContent;			// what structure is contained in message
   USHORT OriginalMessageType;	    // what message type is being acknowledged
   USHORT Response;				    // ACK = message acknowledged, NAK = message not acknowledged
} MESSAGE_ACK_NAK;

/****************************************************/
/* structure to hold a command instruction which    */
/* is usually transmitted from the Mill Console SCP */
/* to connected clients (inspection equipment)      */
/****************************************************/
#define MAX_COMMAND_ARRAY_LENGTH 64
typedef struct {
  USHORT	Originator;		// (2) identifies who sent message
  USHORT	Target;			// (2) identifies who should receive message
  USHORT	MessageType;	// (2) identifies what this message contains
  USHORT	MessageContent; // (2) what structure is contained in message
  USHORT	Command;		// (2) identifies what action client should take
  USHORT	Parameter1;		// (2) 1st optional parameter, must be 0 if not used
  USHORT	Parameter2;		// (2) 2nd optional parameter, must be 0 if not used
  USHORT	Parameter3;		// (2) 3rd optional parameter, must be 0 if not used
  USHORT	Parameter4;		// (2) 4th optional parameter, must be 0 if not used

  TCHAR		Parameter5[MAX_COMMAND_ARRAY_LENGTH];	// (64) 5th optional parameter, 0 fill if not used
  TCHAR		Parameter6[MAX_COMMAND_ARRAY_LENGTH];	// (64) 6th optional parameter, 0 fill if not used
  TCHAR		Parameter7[MAX_COMMAND_ARRAY_LENGTH];	// (64) 7th optional parameter, 0 fill if not used
  TCHAR		Parameter8[MAX_COMMAND_ARRAY_LENGTH];	// (64) 8th optional parameter, 0 fill if not used
} COMMAND_MSG;


/**********************************************/
/* structure to hold just about everything    */
/* you need to know about a client connection */
/**********************************************/
#define MAX_CID_CHAR_LENGTH		50				// max length of char array
#define MAX_CID_IP_CHAR_LENGTH  20				// max length of char array
typedef struct {

// house keeping
    USHORT	Originator;						// identifies who sent message
    USHORT	Target;							// identifies who should receive message
    USHORT	MessageType;					// identifies message type
	USHORT  MessageContent;					// what structure is contained in message
    DWORD   ThreadID;						// thread id server assigned to this client connection

// client connection information  ... move to CCM  
	TCHAR	ServerIP[MAX_CID_IP_CHAR_LENGTH];	// the ip address of the server
	UINT	ServerPort;							// the port the server listens on
	TCHAR	ClientIP[MAX_CID_IP_CHAR_LENGTH];	// the ip address of the connected client
	UINT	ClientPort;							// the port the client was assigned by the server

// client equipment description
	TCHAR	EquipmentNumber[MAX_CID_CHAR_LENGTH];			// the tuboscope assigned id number
	TCHAR	FamilyName[MAX_CID_CHAR_LENGTH];				// generic name for this equipment
	TCHAR	EquipmentDescription[MAX_CID_CHAR_LENGTH];		// what this equipment does
	TCHAR	EquipmentLocation[MAX_CID_CHAR_LENGTH];			// where equipment is installed
	TCHAR	EquipmentInspectionLine[MAX_CID_CHAR_LENGTH];	// actual line location in plant

// client version information
	TCHAR	Company[MAX_CID_CHAR_LENGTH];					// name of company that produced client
	TCHAR	Description[MAX_CID_CHAR_LENGTH];				// general description of client
	TCHAR	ProgramVersion[MAX_CID_CHAR_LENGTH];			// current version of client
	TCHAR	ProjectName[MAX_CID_CHAR_LENGTH];				// project client was created for
	TCHAR	CopyrightNotice[MAX_CID_CHAR_LENGTH];			// standard copyright notice
	TCHAR	Executable[MAX_CID_CHAR_LENGTH];				// name of the client binary
	TCHAR	ProductName[MAX_CID_CHAR_LENGTH];				// top level product name
	TCHAR	ProductVersion[MAX_CID_CHAR_LENGTH];			// top level product version
	TCHAR	DevelopmentTeam[MAX_CID_CHAR_LENGTH];			// development team blurb
} CLIENT_IDENTITY_DETAIL;

/*****************************************/
/* structure to hold a single alarm item */
/*****************************************/
#define MAX_ALARM_NAME_LENGTH 32
typedef struct {
	TCHAR	AlarmName[MAX_ALARM_NAME_LENGTH];		// plain text alarm name
    UCHAR	AlarmNumber;							// number of this alarm (1-64)
	time_t	AlarmTimeStamp;							// time alarm was triggered
	BOOL	AlarmTriggered;							// FALSE = no alarm condition, TRUE = alarm condition
	BOOL	AlarmMasked;							// FALSE = not masked, TRUE = masked
	BOOL	AlarmMuted;								// FALSE = silent, TRUE = audible
} SINGLE_ALARM_DETAIL;

/*******************************************/
/* structure to hold 64 single alarm items */
/*******************************************/
#define MAX_AD_CHAR_LENGTH	50		// max char array length
typedef struct {

// house keeping
    USHORT	Originator;				// identifies who sent message
    USHORT	Target;					// identifies who should receive message
    USHORT	MessageType;			// identifies message type
	USHORT  MessageContent;			// what structure is contained in message
    long    ThreadID;				// thread id server assigned to this client connection

	TCHAR	EquipmentNumber[MAX_AD_CHAR_LENGTH];	// the tuboscope assigned id number
	TCHAR	FamilyName[MAX_AD_CHAR_LENGTH];			// generic name for equipment originating alarm

	time_t	MessageTimeStamp;		// client sets as 0, server fills in when message received

	// up to max single alarms
	SINGLE_ALARM_DETAIL  SingleAlarmDetail[MAX_EQUIPMENT_ALARMS];	
} ALARM_DETAIL;

// Used by everybody
/*********************************/
/* structure to hold log message */
/*********************************/
#define MAX_LFD_CHAR_ARRAY	50	// max char array length
#define MAX_LFD_MSG_LENGTH  64	// max message text length
typedef struct {

// house keeping
    USHORT	Originator;			// identifies who sent message
    USHORT	Target;				// identifies who should receive message
    USHORT	MessageType;		// identifies message type
	USHORT  MessageContent;		// what structure is contained in message
    long    ThreadID;			// thread id server assigned to this client connection

	TCHAR   EquipmentNumber[MAX_LFD_CHAR_ARRAY];// machine ID
	TCHAR	FamilyName[MAX_LFD_CHAR_ARRAY];		// generic name for equipment generating log message

	time_t	MessageTimeStamp;	// client sets as 0 server fills in when message received

	TCHAR	LogMessageText[MAX_LFD_MSG_LENGTH]; // log message text
} LOGFILE_DETAIL;

// Used by everybody
/*****************************************/
/* structure to hold pipe number message */
/*****************************************/
#define MAX_PND_CHAR_ARRAY		50	// max char array length
#define MAX_PND_SUFFIX_LENGTH	5	// max pipe number suffix length
#define MAX_PND_TYPE_LENGTH		5	// max pipe number type length
typedef struct {
    USHORT	Originator;			// identifies who sent message
    USHORT	Target;				// identifies who should receive message
    USHORT	MessageType;		// identifies message type
	USHORT  MessageContent;		// what structure is contained in message
	USHORT	PipeLocationStatus; // pipe entered, in, exited unit
    long    ThreadID;			// thread id server assigned to this client connection

    TCHAR	PipeNumberID[MAX_PND_CHAR_ARRAY];				// unique identifier assigned by database
	TCHAR   PipeNumberWorkOrder[MAX_PND_CHAR_ARRAY];		// clear text work order designator
	TCHAR	PipeNumberHeat[MAX_PND_CHAR_ARRAY];				// clear text heat designator
	TCHAR   PipeNumberLot[MAX_PND_CHAR_ARRAY];				// clear text lot designator
    TCHAR   CustomerJointIdentifier [MAX_PND_CHAR_ARRAY];	// could be serial # etc.
    int		PipeNumberSequence;								// pipe sequence number
	TCHAR	PipeNumberSuffix[MAX_PND_TYPE_LENGTH];			// pipe number suffix i.e. A, B, C, ... Z
	TCHAR	PipeNumberType[MAX_PND_TYPE_LENGTH];			// I,i = Inspection, C,c = Calibration
} PIPE_NUMBER_DETAIL;

/*******************************************/
/* structure to hold GDP Parameter message */
/*******************************************/
#define IMPERIAL		0
#define METRIC			1
#define LOGARITHMIC		0
#define LINEAR			1

typedef struct {
    USHORT	Originator;			// identifies who sent message
    USHORT	Target;				// identifies who should receive message
    USHORT	MessageType;		// identifies message type
	USHORT  MessageContent;		// what structure is contained in message
    long    ThreadID;			// thread id server assigned to this client connection
    int 	EquipmentNumber;	// cast representation of the equipment number in tblEquipmentDescriptions

    UCHAR	MeasurementUnits;	// 0 = imperial, 1 = metric
	UCHAR	GraphRange;			// 0 = logarithmic, 1 = linear
} GDP_PARAMETER_DETAIL;

/************************************/
/* structure to hold status message */
/************************************/
typedef struct {
    USHORT	Originator;			// identifies who sent message
    USHORT	Target;				// identifies who should receive message
    USHORT	MessageType;		// identifies message type
	USHORT  MessageContent;		// what structure is contained in message
    long    ThreadID;			// thread id server assigned to this client connection
    USHORT	Status;				// 
} STATUS_DETAIL;
  
/**************************************/
/* structure to hold unique record ids*/
/* and other various items to reduce  */
/* the number of client database      */
/* accesses                           */ 
/**************************************/
/* note, items will be added when     */
/* deemed necessary                   */
/**************************************/
#define UNIQUE_ID_DETAIL_ARRAY_LENGTH 64
typedef struct {
	TCHAR szWorkOrderID[UNIQUE_ID_DETAIL_ARRAY_LENGTH];	    // 0 fill if not used
	TCHAR szPipeDiameter[UNIQUE_ID_DETAIL_ARRAY_LENGTH];	// 0 fill if not used
	TCHAR szNominalWall[UNIQUE_ID_DETAIL_ARRAY_LENGTH];	    // 0 fill if not used
	TCHAR szPipeRange[UNIQUE_ID_DETAIL_ARRAY_LENGTH];	    // 0 fill if not used
} UNIQUE_ID_DETAIL;


/**********************************************/
/* structure to hold report generator message */
/**********************************************/
#define MAX_RGD_CHAR_ARRAY		50	// max char array length
#define MAX_RGD_SUFFIX_LENGTH	5	// max pipe number suffix length
#define MAX_RGD_TYPE_LENGTH		5	// max pipe number type length
typedef struct {
    USHORT	Originator;			// identifies who sent message
    USHORT	Target;				// identifies who should receive message
    USHORT	MessageType;		// identifies message type
	USHORT  MessageContent;		// what structure is contained in message
    long    ThreadID;			// thread id server assigned to this client connection

    TCHAR	PipeNumberID[MAX_RGD_CHAR_ARRAY];				// unique identifier assigned by database
	TCHAR   PipeNumberWorkOrder[MAX_RGD_CHAR_ARRAY];		// clear text work order #
	TCHAR	PipeNumberHeat[MAX_RGD_CHAR_ARRAY];				// clear text heat
    TCHAR   CustomerJointIdentifier [MAX_RGD_CHAR_ARRAY];	// could be serial # etc.
    int		PipeNumberSequence;								// pipe sequence number
	TCHAR	PipeNumberSuffix[MAX_RGD_TYPE_LENGTH];			// pipe number suffix i.e. A, B, C, ... Z
	TCHAR	PipeNumberType[MAX_RGD_TYPE_LENGTH];			// I,i = Inspection, C,c = Calibration
} REPORT_GENERATOR_DETAIL;

/***************************************/
/* structure to hold remote job / pipe */
/* descriptions which come from        */
/* external apps that connect to       */
/* customer computers                  */
/***************************************/
#define MAX_ROD_TCHAR_ARRAY   100	// max char array length
#define MAX_ROD_COMMENT_ARRAY 512   // this must match field size in the database table 
typedef struct {

// house keeping section
    USHORT	Originator;			// identifies who sent message
    USHORT	Target;				// identifies who should receive message
    USHORT	MessageType;		// identifies message type
	USHORT  MessageContent;		// what structure is contained in message
    long    ThreadID;			// thread id server assigned to this client connection

// job description section
    TCHAR   WorkOrderID[MAX_ROD_TCHAR_ARRAY];	// work order id (this is NOT WorkOrder)
    TCHAR	Customer[MAX_ROD_TCHAR_ARRAY];		// customer name 
	TCHAR	Location[MAX_ROD_TCHAR_ARRAY];		// inspection location
	TCHAR	WorkOrder[MAX_ROD_TCHAR_ARRAY];		// workorder descriptor (this is NOT WorkOrderID)
	TCHAR	Heat[MAX_ROD_TCHAR_ARRAY];			// heat descriptor
	TCHAR   Lot[MAX_ROD_TCHAR_ARRAY];			// lot descriptor
	TCHAR	Operator[MAX_ROD_TCHAR_ARRAY];		// job operator
	long	AmalogRPM;							// target AmalogRPM
	long	TruscopeRPM;						// target TruscopeRPM
	long	LineSpeed;							// target LineSpeed
	
// DEVELOPER WARNING: The content of the comment fields is purely
// arbitrary and may contain meaningful customer supplied data as well
// as generic comments (reference HengYang project)
	TCHAR	Comment_1[MAX_ROD_COMMENT_ARRAY];	// generic optional comment
	TCHAR	Comment_2[MAX_ROD_COMMENT_ARRAY];	// generic optional comment
	TCHAR	Comment_3[MAX_ROD_COMMENT_ARRAY];	// generic optional comment
	TCHAR	Comment_4[MAX_ROD_COMMENT_ARRAY];	// generic optional comment
	TCHAR	Comment_5[MAX_ROD_COMMENT_ARRAY];	// generic optional comment
	TCHAR	Comment_6[MAX_ROD_COMMENT_ARRAY];	// generic optional comment


// pipe description section
	float	OutsideDiameter;					// pipe OD
	float	NominalWall;						// pipe nominal wall thickness
	float	WeightPerUnitLength;				// always expressed in lbs/ft NOT Kg/m
	long	LastJointInLot;						// 
	TCHAR	PipeGrade[MAX_ROD_TCHAR_ARRAY];		// pipe grade
    int     PipeRange;							// numeric pipe range, 1,2,3, etc
} REMOTE_OPERATOR_DETAIL;

/************************************/
/* structure to hold set mode message */
/************************************/
typedef struct {
    USHORT	Originator;			// identifies who sent message
    USHORT	Target;				// identifies who should receive message
    USHORT	MessageType;		// identifies message type
	USHORT  MessageContent;		// what structure is contained in message
    long    ThreadID;			// thread id server assigned to this client connection

	USHORT	mode;				// 1=start inspection, 2=stop inspection
} SET_MODE_DETAIL;


/***********/
/* WARNING */
/************************************/
/* this structure is used solely by */
/* list control sorting routines so */
/* don't use it for anything else   */
/************************************/
typedef struct {
	CListCtrl * pListCtrl;
	DWORD dwColumn;
	bool  bSortAscending;
} SORT_CONTEXT;


/************/
/* WARNING  */
/**************************************/
/* this is used to do the pipe suffix */
/* conversion operations so we can    */
/* display letters in the buddy box   */
/* instead of the usual numbers       */
/**************************************/
#define ASCII_SPACE 0x20;		//ordinal number of USASCII space character
#define ASCII_AT	0x40;		//ordinal number of USASCII @ character
typedef enum ALPHABET {A, B, C, D, E, 
					   F, G, H, I, J, 
					   K, L, M, N, O, 
					   P, Q, R, S, T, 
					   U, V, W, X, Y, 
					   Z} Alphabet;

#endif