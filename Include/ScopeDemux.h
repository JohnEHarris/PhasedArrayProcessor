/***************************************************************************/
/*    ScopeDemux                                                           */
/*                                                                         */
/*                                                                         */
/*      THIS MATERIAL IS PROPRIETARY TO  TUBOSCOPE AND IS                  */    
/*      NOT TO BE REPRODUCED, USED OR DISCLOSED EXCEPT IN ACCORDANCE       */    
/*      WITH PROGRAM LICENSE OR UPON WRITTEN AUTHORIZATION OF  TUBOSCOPE,  */ 
/*      HOUSTON, TX.                                                       */
/*                                                                         */
/*          COPYRIGHT (C)       2001 TUBOSCOPE                             */
/*                                                                         */
/***************************************************************************/
/* Module:		 scopedemux.h                                               */
/* Purpose:     contains the global vars and constants used by ScopeDemux  */
/* Type:        header file                                                */
/* Create date: September 20, 2001                                         */
/* Author:      Randy A. Harris                                            */
/* Department:  Inspection / Field Engineering                             */
/* Derivations: None                                                       */
/***************************************************************************/                                          
/* Revisions:								   												*/
/*                                                                         */
/***************************************************************************/
#ifndef __INCscopedemuxh
#define __INCscopedemuxh

/* function prototypes */
void		AnnounceOurselves(void);				/* display tubo copyright banner   */
BOOL 		PCI_Get_WD_handle(HANDLE *phWD);    /* get a handle to Jungo's windrvr */

/* functions that exercise the ScopeDemux board */
/* fn[x.x] numbers reflect the convention used by */
/* Mark Eakin in his hardware description white papers */
BOOL   SD_GetSTATUS(P9054_HANDLE bhPlx, UCHAR whichflag);										/* fn[0.0] - get status for 1 of 4 hardware subsections */
STATUS SD_SetOSCOPEDisplayMode(P9054_HANDLE bhPlx, UCHAR whichtrace, UCHAR whichmode);	/* fn[0.1] - set the input to display on scope traces 1 or 2 */
STATUS SD_SetGATEControl(P9054_HANDLE bhPlx, UCHAR gateControl);								/* fn[1.0] - set the gate control register */
STATUS SD_SetTRIGGERControl(P9054_HANDLE bhPlx, UCHAR trigger);								/* fn[2.0] - set the trigger control register */
STATUS SD_SetSEQUENCELength(P9054_HANDLE bhPlx, UCHAR sequencelength);						/* fn[3.0] - set the firing/receiving sequence length */
STATUS SD_SetMDACLevel(P9054_HANDLE bhPlx, WORD mdacLevel, UCHAR mdacChannel);			/* fn[6.0] - set the mdac channel level to a 12 bit value */
STATUS SD_StrobeMDAC(P9054_HANDLE bhPlx);																/* fn[6.1] - strobe the mdac input buffer to the output buffer */
STATUS SD_ClearMDAC(P9054_HANDLE bhPlx);																/* fn[6.2] - clear the mdac output buffers */
STATUS SD_GetADCValue(P9054_HANDLE bhPlx, short *ADCValue);										/* fn[7.0] - read the A/C converter */
STATUS SD_SetSTRIPCHARTLevel(P9054_HANDLE bhPlx, WORD chartLevel, UCHAR chartChannel);	/* fn[A.0] - set the strip chart channel level to a 12 bit value */
STATUS SD_StrobeSTRIPCHART(P9054_HANDLE bhPlx);														/* fn[A.1] - strobe the strip chart input buffer to the output buffer */
STATUS SD_ClearSTRIPCHART(P9054_HANDLE bhPlx);														/* fn[A.2] - clear the strip chart output buffers */
STATUS SD_SetDIOConfiguration(P9054_HANDLE bhPlx, WORD SourceModeDirection);				/* fn[B.0] - set source, mode, and direction for DIO groups */
STATUS SD_SetDIOGroupOutput(P9054_HANDLE bhPlx, DWORD DIOGroup, UCHAR DIOOutBits);		/* fn[B.1] - write a byte to a DIO output group */
STATUS SD_GetDIOGroupInput(P9054_HANDLE bhPlx, DWORD DIOGroup, UCHAR *DIOInBits);		/* fn[B.2] - read a byte from a DIO group input */

/* JUNGO DriverBuilder run-time registration */
void RegisterDriverBuilder(void);	   /* called to register the driver at run-time */

/* proof of concept functions */
/* these just exhibit our ability */
/* to control things on the board */
/* they can be removed in the final version */
void LEDBlinkAll(void);						/* display mode 0 */
void LEDBlinkAlternate(void);   			/* display mode 1 */
void LEDBarberPole(void);					/* display mode 2 */
void LEDCylon(void);							/* display mode 3 */
void ReadSwitchImage(void);      		/* read the switches */

/* constants */
#define  DASHES      	"------------------------------------------------------------"
#define	TUBOSCOPE 		"Tuboscope"
#define  COPYRIGHT 		"Copyright 2001 - 2002"
#define  VERSION   		"Version 1.0.0"
#define  MODULE    		"ScopeDemux User Mode Driver"
#define  ACTIVE    		"ACTIVE"
#define	CLOSED			"CLOSED"
#define  DISABLED  		"DISABLED"
#define  ENABLED   		"ENABLED"
#define  INACTIVE  		"INACTIVE"
#define  LOADED      	"LOADED"
#define  NOTLOADED   	"NOT LOADED"
#define  NOTPRESENT		"NOT PRESENT"
#define  OPEN				"OPEN"
#define  PRESENT   		"PRESENT"

/* this is the DriverBuilder license string  */
/* which must be used to register the driver */ 
#define  JUNGOLICENSE   "68c9563bbd7cc5d69e76d53bd2bde937.Varco"


#define SIXTYHERTZ			60						/* defines DEFAULT system clock rate */
#define FIVEHUNDREDHERTZ	500					/* defines OUR system clock rate */

/******************************/
/* PLX 9054 - pci bridge chip */
/******************************/
#define  PLXVENDORID 	0x000010b5				/* PLX vendor ID for PLX devices */
#define  PLXDEVICE9054	0x00005406				/* PLX 9054 device */
#define  PLXDEVICEID    PLXDEVICE9054        /* the specific PLX device we're interested in */

/*****************************************/
/* masks, bitstrippers, bit positioners  */
/*****************************************/
#define	ADCBUSYMASK				0x80				/* A/D convertor busy flag */
#define  CHARTBUSYMASK  		0x40     		/* Strip chart recorder busy flag */
#define  MDACBUSYMASK   		0x20				/* Multiplying DAC busy flag */
#define  DIOBUSYMASK    		0x10     		/* Digital I/O busy flag */

#define  GATECONTROLMASK      0x03           /* bit stripper mask for gate control param */
#define  HIGHNYBBLEMASK    	0xf0  			/* mask for upper half of a byte */
#define  LOWNYBBLEMASK     	0x0f  			/* mask for lower half of a byte */
#define  SWITCHIMAGEMASK      0x03				/* bit mask for input switches */

#define  TRIGGERBITS32MASK		0x0c           /* sanity test mask for bits 3 & 2 of trigger control */
#define  TRIGGERBITS10MASK    0x03           /* sanity test mask for bits 1 & 0 of trigger control */
#define  TRIGGERCONTROLMASK   0x1f           /* bit stripper for trigger control param */

#define  DIOCOMMANDREGMASK    0x0fff         /* bit stripper for DIO command register param */

#define  CHARTLEVELSHIFTCOUNT 4              /* # bits to left shift a chart level input value */
#define  DEFLEDIMAGE       	0  				/* assumes current sources, this keeps all leds off */
#define  DEFSWITCHIMAGE	  		0     			/* assumes positive logic, this implies no switches activated */

/* maxs & mins */
#define  DATADONTCARE         0              /* used with writes when data value doesn't matter */
#define  MAXGATECONTROL       2					/* max gate control states */
#define	MAXMDACCHANNEL			8					/* max mdac channels */
#define  MAXOSCOPETRACE       2              /* max oscilloscope traces */
#define  MAXSTRIPCHARTCHANNEL 8              /* max strip chart recorder channels */


/* offsets in Address Space 0 to LOCAL memory */
/* NOTE: THESE ARE OFFSETS, NOT ABSOLUTES */

/* fn[0.0] */
#define  CONTROLFUNCTION      0x00000000				/* offset of the CONTROL function register (writes only) */
#define  CONTROLREG_0         CONTROLFUNCTION		/* byte 0 offset of CONTROL register */
#define  CONTROLREG_1         CONTROLFUNCTI0N+1	   /* byte 1 offset of CONTROL register */
#define  CONTROLREG_2         CONTROLFUNCTION+2    /* byte 2 offset of CONTROL register */
#define  CONTROLREG_3         CONTROLFUNCTION+3    /* byte 3 offset of CONTROL register */

/* fn[0.1] */
#define  STATUSFUNCTION       0x00000000     		/* offset of the STATUS function register (reads only) */
#define  STATUSREG_0          STATUSFUNCTION     	/* byte 0 offset of status register */
#define  STATUSREG_1          STATUSFUNCTION+1     /* byte 1 offset of status register */
#define  STATUSREG_2          STATUSFUNCTION+2     /* byte 2 offset of status register */
#define  STATUSREG_3          STATUSFUNCTION+3     /* byte 3 offset of status register */

/* fn[1.0] */
#define  GATEFUNCTION         0x00000100     		/* offset of the GATE function register */
#define  GATEREG_0				GATEFUNCTION         /* byte 0 offset of the GATE function register */

/* fn[2.0] */
#define  TRIGGERFUNCTION      0x00000200     		/* offset of the TRIGGER function register */
#define  TRIGGERREG_0         TRIGGERFUNCTION      /* byte 0 offset of the TRIGGER function register */

/* fn[3.0] */
#define  SEQUENCEFUNCTION     0x00000300     		/* offset of the SEQUENCE function register */
#define  SEQUENCEREG_0			SEQUENCEFUNCTION     /* byte 0 offset of sequence register */			
#define  SEQUENCEREG_1			SEQUENCEFUNCTION+1   /* byte 1 offset of sequence register */
#define  SEQUENCEREG_2			SEQUENCEFUNCTION+2   /* byte 2 offset of sequence register */
#define  SEQUENCEREG_3			SEQUENCEFUNCTION+3   /* byte 3 offset of sequence register */

/* fn[6.0] */
#define  MDACFUNCTION			0x00000600				/* offset of the MDAC function register */
#define  MDACLOADREG          MDACFUNCTION     		/* offset of the MDAC load register */
#define  MDACSTROBEREG        MDACFUNCTION+0x10    /* offset of the MDAC strobe register */
#define  MDACCLEARREG         MDACFUNCTION+0x20		/* offset of the MDAC clear register (not normally used */

/* define some mdac channel identifiers and aliases */
#define	MDACCH_0					0							/* mdac channel 0 designator */
#define  MDACCH_1					1							/* mdac channel 1 designator */
#define  MDACCH_2					2							/* mdac channel 2 designator */
#define  MDACCH_3					3							/* mdac channel 3 designator */
#define  MDACCH_4					4							/* mdac channel 4 designator */
#define  MDACCH_5					5							/* mdac channel 5 designator */
#define  MDACCH_6					6							/* mdac channel 6 designator */
#define  MDACCH_7					7							/* mdac channel 7 designator */

/* the naming convention for the mdac channel */
/* aliases were determined by Mark Eakin      */
#define	V_CH_NULL				MDACCH_0					/* nulls out baseline dc level offset errors */
#define	V_NR					MDACCH_1					/* FW mode baseline noise reject set */
#define	V_GAIN					MDACCH_2					/* signal path gain set */
#define  V_SCOPE_NULL			MDACCH_3					/* scope / outbuffer dc offset error null */
#define  VT_I					MDACCH_4					/* Gate 1 threshold level voltage set */
#define  VT_II					MDACCH_5					/* Gate 2 threshold level voltage set */
#define  V_SHUNT				MDACCH_6					/* scope gate threshold zero level null */
#define  V_NOTDEFINED			MDACCH_7					/* mdac channel 8 function not defined */

/* fn[7.0] */
#define  ADCFUNCTION			0x00000700				/* offset of the A/D function register */
#define  ADCSTARTCONVERSION		ADCFUNCTION          /* ANY write causes A/D to start conversion */
#define  ADCVALUE_LOW			ADCFUNCTION          /* low byte of converted value */
#define  ADCVALUE_HIGH			ADCFUNCTION+1        /* high byte of converted value */

/* fn[A.0] */
#define  STRIPCHARTFUNCTION 	0x00000a00					/* offset of the STRIPCHART function register */
#define  STRIPCHARTLOADREG    STRIPCHARTFUNCTION     	/* offset of the STRIPCHART load register */
#define  STRIPCHARTSTROBEREG  STRIPCHARTFUNCTION+0x10 /* offset of the STRIPCHART strobe register */
#define  STRIPCHARTCLEARREG   STRIPCHARTFUNCTION+0x20	/* offset of the STRIPCHART clear register (not normally used */

/* fn[B.0] */
#define  DIOFUNCTION          0x00000b00     		/* offset of the Digitial I/O functon register */
#define  DIOGROUP_1           DIOFUNCTION				/* byte 0 offset of DIO group 1 */
#define  DIOGROUP_2           DIOFUNCTION+1			/* byte 1 offset of DIO group 2 */
#define  DIOGROUP_3           DIOFUNCTION+2			/* byte 2 offset of DIO group 3 */
#define  DIOGROUP_4           DIOFUNCTION+3			/* byte 3 offset of DIO group 4 (no actual hardware yet) */

#define  DIOCOMMANDREG_0    	DIOFUNCTION+0x10     /* byte 0 offset of the control register (WRITES ONLY) */
#define  DIOCOMMANDREG_1    	DIOFUNCTION+0x11		/* byte 1 offset of the control register (WRITES ONLY) */
#define  DIOCOMMANDREG_2    	DIOFUNCTION+0x12		/* byte 2 offset of the control register (WRITES ONLY) */
#define  DIOCOMMANDREG_3    	DIOFUNCTION+0x13		/* byte 3 offset of the control register (WRITES ONLY) */

/* proof of concept routines */
#define  LEDADDRESSOFFSET  	DIOGROUP_3     /* offset of the led driver */
#define  SWITCHADDRESSOFFSET  DIOGROUP_1     /* offset of the switch input buffer */


/************************/
/* oscilloscope display */
/************************/
/* trace 1 selector and modes */
#define	OSCOPETRACE1			0x00				/* selects oscilloscope trace 1 */
#define  OSCOPERFMODE			0x00           /* selects RF mode display (low gain is set) */
#define  OSCOPEFWMODE			0x01				/* selects FW mode display (high gain is set) */
#define  OSCOPET1MODEMASK     0xfe           /* masks trace 1 mode bit */

/* trace 2 selector and modes */
#define  OSCOPETRACE2			0x01				/* selects oscilloscope trace 2 */
#define  OSCOPEGATEMODE			0x00				/* selects GATE mode display */			
#define  OSCOPESYNCMODE			0x02 				/* selects SYNC mode display */
#define  OSCOPET2MODEMASK     0xfd           /* masks trace 2 mode bit */ 

#define  SEQUENCELENGTH_1     0x00           /* defines firing/receiving sequence length 1 */
#define  SEQUENCELENGTH_2     0x01           /* defines firing/receiving sequence length 2 */
#define  SEQUENCELENGTHMASK   0x01           /* sequence length mask bit */

/* vars */
P9054_HANDLE	hPlx;     							/* a handle to the PLX9054 bridge chip */
HANDLE 			hWD;									/* a handle to Jungo's windrvr */
WD_PCI_SLOT    pciSlot;             			/* bus, slot, function holder */
BOOL				fUseInt;             			/* flag to use interrupts - SHOULD ALWAYS BE FALSE */
BOOL           bfResult;            			/* return flag from 9054 routines */
BOOL				Stopped;             			/* flag - true causes program termination */
DWORD				cardcount;           			/* # cards in system with 9054 bridges */

/* debuggers */
UCHAR	LedImage;										/* the current bit image for the led outputs */
UCHAR SwitchImage;               				/* the current switch input image */
#endif
