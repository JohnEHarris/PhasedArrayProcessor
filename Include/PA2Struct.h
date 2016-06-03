/*
Define structures and definitions for PhasedArray2 project
jeh
2016-05-17

*/

#ifndef PA2_STRUCTS_H

#ifndef BYTE
typedef unsigned char	BYTE;
#endif
#ifndef WORD
typedef unsigned short	WORD;
#endif

#ifndef UINT
typedef unsigned int	UINT;
#endif

enum IdataTypes {eRawInsp=10, eAscan=12, eKeepAlive=0xff};

/*****************	STRUCTURES	*********************/
typedef struct 
	{
    BYTE bMsgID;	// = eIdataTypes
    BYTE bSpare1; 
    BYTE bSeq; //sequence number (zero-based) of the first data point
    BYTE bDin; //digital input, Bit1=Direction, Bit2=Inspection Enable, Bit4=Away(1)/Toward(0)
    WORD wMsgSeqCnt;
    WORD wLocation; //x location in motion pulses
    WORD wClock; //unit in .2048ms
    WORD wPeriod; //unit in .2048ms
    BYTE bSpare2[4];
	} SDataHead; //16 bytes


typedef struct 
	{
    WORD wTof4;     //time of flight of Gate 4
    WORD wGateFlag;  //Gate flag bits, BIT0=Gate 1
    WORD wTof2;  //time of flight of Gate 2
    BYTE bAmp3;   //Gate 3 amplitude (0-255)
    BYTE bAmp2;   //Gate 2 amplitude (0-255)
	} SRawData; //8 bytes

typedef struct 
	{
    SDataHead DataHead; // 16 bytes
    SRawData RawData[128];  // raw data of 128 consecutive pulses 128*8=1024
	} SRawDataPacket; //1040 bytes

typedef struct 
	{
    SDataHead DataHead;
    WORD wData[512];
	} SCmdPacket; //1040 bytes

// If we want 2 out of 3 above threshold for Nc qualified, then bMod = 3. The Fifo is 3 elements deep.
// Each flaw reading goes into the fifo at location bInput and overwrites the oldest element in the fifo.
// That is why we call it a FIFO.
// bInput advances 1 fifo postion modulo bMod. After the input, the fifo is scanned from element 0 to bMod
// bAboveThld counts the number of readings above Thold. If it is >= bNc we have a bMax flaw.
// bMaxTemp is the max value in the FIFO w/o regard to Nc. If we have less than Nc AboveThld and bMaxTemp > thold
// then bMax is set to 80% of bThold. If we meet Nc count above thold then bMax = bMaxTemp and is returned by the 
// FifoInput() function
typedef struct
	{
	BYTE bCell[16];	// data cell containing input amplitudes 
	BYTE bInput;	// next location to fill in the FIFO
	BYTE bNc;		// how many flaw values above threshold
	BYTE bThold;	// Threshold value 0-127
	BYTE bMax;		// Max Nc qualified value in FIFO
	BYTE bMaxTemp;	// Max value in FIFO w/o regard to Nc
	BYTE bMod;		// active depth of FIFO.
	BYTE bAboveThld;	// how many FIFO element above thold
	} Nc_FIFO;

// Each wall reading goes into a wall averaging FIFO. The memory size of the FIFO is selected to be longer 
// than the expected averaging interval which should be 8 or less (typically 2 to 4 readings).
// The FIFO output is actually the sum of the values in the FIFO and the bad wall reading count.
// Values in the FIFO are not summed on each call, but the current location content in the FIFO is subtraced from the
// sum and then the new value is stored and added to the sum. Then the next location in the FIFO is selected for
// the next call (input) to the FIFO. This is exactly how a hardware implementation would work
// Wall reading are 16 bit values. Values less than 0.040 or greater than 2.00 inches are consider to be error.
// In case of a bad (erroneous) reading a counter is incremented and the FIFO does not advance
// Consecutive good wall readings above Nx length will reset the bad wall reading counter. 
// If the bad wall reading reaches a dropout value, the output reading from the FIFO 
// will be set to size of the averaging length (the average will be 1).
// The approximate conversion from machine reading to 0.001" increments is 1.452*hardware value
// Machine hardware readings for bad wall therefore are < 27 or > 1377
//

typedef struct
	{
	WORD wCell[16];	// data cell containing wall thickness readings
	BYTE bInput;	// next location to fill in the FIFO
	BYTE bNx;		// how many wall values to average - the divisor
	UINT uSum;		// sum of all wall values in FIFO
	WORD wBadWall;	// how many out of range range wall readings
	WORD wGoodWall;	// consecutive good wall readings
	WORD wWallMax;	// minimum allowed hardware wall reading
	WORD wWallMin;	// minimum allowed hardware wall reading
	WORD wDropOut;	// number of bad readings before drop out occurs
	} Nx_FIFO;

#if 0
// The value retured from the Nx function as a pointer to a Nx_FIFO_RETURN structure
typedef struct
	{
	UINT uSum;		// sum of all wall values in FIFO
	WORD wBadWall;	// how many out of range range wall readings
	} Nx_FIFO_RETURN;
#endif

/*****************	STRUCTURES	END *********************/

#define TEST_UT                          20
#define SET_INSPECT_MODE                 21
#define SET_INSPECT_ENABLE				 30

#define RAW_DATA_ID                      10
#define DATA_ID                          11
#define ASCAN_ID                         12

#define READ_TIMER_CMD_ID                201 //read command>200
#define READ_LM86_LT_CMD_ID              210
#define READ_LM86_RT_CMD_ID              211

#define SET_PULSE_RATE_CMD_ID           50
#define SET_SEQ_CMD_ID                  51
#define SET_PULSE_WIDTH_CMD_ID          52
#define SET_PULSE_CONTROL_CMD_ID        53
#define SET_PULSE_IN_OUT_CMD_ID         54
#define SET_PULSE_ENABLE_CMD_ID         55
#define SET_RECEIVE_ENABLE_CMD_ID       56

#define SET_PULSE_MEMORY_CMD_ID         57
#define SET_PULSE_ENABLE_ALL_CMD_ID     58
#define SET_PULSE_CPU_ENABLE_CMD_ID     59
#define SET_PULSE_AUTO_ENABLE_CMD_ID    40
#define SET_PULSE_ENABLE_MEM_CMD_ID     41


#define SET_GATE_DELAY_CMD_ID           60
#define SET_GATE_RANGE_CMD_ID           61
#define SET_GATE_BLANK_CMD_ID           62
#define SET_GATE_THRESHOLD_CMD_ID       63
#define SET_GATE_CONTROL_CMD_ID         64
#define SET_GATE_DATA_MODE_CMD_ID       65
#define SET_GATE_TOF_MODE_CMD_ID        66
#define SET_GATE_SEQ_REG_CMD_ID         67

#define SET_HV_SWITCH_MEM_CMD_ID        70
#define SET_HV_SEQ_GROUP_CMD_ID         71

#define SET_GAIN_DAC_CMD_ID             79
#define SET_GAIN_STEP_CMD_ID            76
#define SET_GAIN_TGC_DELAY_CMD_ID       77
#define SET_GAIN_SEQ_CMD_ID             78

#define SET_LM86_ACT_CMD_ID             81
#define SET_LM86_CMD_CMD_ID             82
#define SET_LM86_DATA_CMD_ID            83
#define SET_LM86_CONF_CMD_ID            84
#define SET_LM86_LH_CMD_ID              85
#define SET_LM86_LL_CMD_ID              86


#define RESET_SCAN_CMD_ID              110
#define SET_SCAN_1X1_CMD_ID            100
#define SET_SCAN_2X2_CMD_ID            101
#define SET_SCAN_16X16_CMD_ID          102
#define SET_SCAN_DELAY_CMD_ID          103
#define SET_SCAN_DELAY_16X16_CMD_ID    104
#define SET_SCAN16_16X16_CMD_ID        105

#define SET_ASCAN_DELAY_CMD_ID         120
#define SET_ASCAN_RANGE_CMD_ID         121
#define SET_ASCAN_SEQ_REG_CMD_ID       122
#define ENABLE_ASCAN_INT_CMD_ID        123
#define DISABLE_ASCAN_INT_CMD_ID       124
#define SET_ASCAN_PEAK_SEL_REG_CMD_ID  125
#define SET_ASCAN_ZERO_MEM_CONFIG_ID   126

//add for lvds setup
#define LVDS_DATA_ALIGN                  130
#define LVDS_ALIGN_RESET                 131
#define ADC_SERIAL_SET                   132
#define ADC_REG_UPDATE                   133

#define SET_RECEIVER_MEMORY_CMD_ID      135 
#define SET_SCAN_REC_DELAY_16X16_CMD_ID       136
#define SET_SCAN_PITCH_PULSER_DELAY_16X16_CMD_ID    137

#define SET_SCAN_PULSER_MODE                 138 //0: echo mode, 1: pitch mode


#define WRITE_SPI0_REG      140
#define RESET_ALT_LVDS      141

#define CHANGE_9272_REG     161
#define CHANGE_9272_GAIN     162

#define SET_ASCAN_SEQ_ONE_BEAM_CMD_ID  170

#define SET_TCG_STEP_CMD_ID			   197
#define SET_TCG_FN_CMD_ID              198
#define SET_SCAN_TYPE_CMD_ID           199

#define LINEAR_SCAN_0_DEGREE           0
#define LINEAR_SCAN_37_DEGREE          1
#define THREE_SCAN_LRW_8_BEAM          2
#define THREE_SCAN_LRW_16_BEAM         3
#define TWO_SCAN_LR_8_BEAM             4
#define TWO_SCAN_LR_16_BEAM            5
#define LONG_8_BEAM_12345678		   6
#define LONG_8_BEAM_56781234		   7
#define LONG_24_BEAM_800               8
#define LONG_24_BEAM_080               9
#define THREE_SCAN_LRW_8_BEAM_FOCUS    10
#define THREE_SCAN_LO1LO1R_8_BEAM_12345678    11
#define THREE_SCAN_LO1LO1R_8_BEAM_56781234    12
#define LONG_24_BEAM_12345678		   13
#define LONG_24_BEAM_56781234		   14
#define WALL_25_BEAM_90_DEGREE_PROBE   15

#define DataHeadLength 16
#define DataPacketLength 24
#define RAW_DATA_PACKET_LENGTH 1040
#define AscanPacketLength 1040
#define CmdPacketLength 1040



#endif	// PA2_STRUCTS_H
