#ifndef NIOS_MSG_H
#define NIOS_MSG_H


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
#define RawDataPacketLength 1040
#define AscanPacketLength 1040
#define CmdPacketLength 1040

#if 0
typedef struct 
	{
    unsigned char bMsgID;
    unsigned char bSpare1; 
    unsigned char bSeq; //sequence of the first data
    unsigned char bDin; //digital input

    unsigned short wMsgSeqCnt;
    unsigned short wLocation; //x location motion pulse
    unsigned short wClock; //unit in .2048ms
    unsigned short wPeriod; //.2048ms
    unsigned char bSpare2[4];
	} stDataHead; //16

#endif

typedef struct 
	{
	unsigned char bMsgID;
	unsigned char bSpare1;
	unsigned char bSeq;
	unsigned char bDin;

	unsigned short wMsgSeqCnt;
	unsigned short wLocation;
	unsigned short wClock; //unit in .2048ms
	unsigned short wPeriod; //.2048ms
	unsigned char bSpare2[4]; //more spare
	} SAscanHead; //16


#if 0
typedef struct {
    unsigned short wTof4Max;
    unsigned short wTof3;
    unsigned short wTof4MaxMin;
    unsigned char bAmp3;
    unsigned char bAmp2;
} stRawData; //8
#endif


typedef struct {
    unsigned char bAmp[4];
    unsigned short wTofPeak[2];
} SData; //8 bytes

#if 0
typedef struct 
	{
    stDataHead DataHead;
    stRawData RawData[128];
	} SRawDataPacket; //1040 bytes
#endif

typedef struct {
    stDataHead DataHead;
    SData Data;
} SDataPacket; //24 bytes

typedef struct 
	{
    SAscanHead AscanHead;
    char Ascan[1024];
	} SAscanPacket; //1040 bytes

#if 0
typedef struct {
    stDataHead DataHead;
    unsigned short wData[512];
} SCmdPacket; //1040 bytes
#endif


#endif  //one time through