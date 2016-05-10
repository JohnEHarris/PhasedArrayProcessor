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
/* Module:		 escapecodes - generic ASCII terminal escape codes          */
/* Purpose:     so I don't have to remember what they are                  */
/* Type:        header file                                                */
/* Create date: September 20, 2001                                         */
/* Author:      Randy A. Harris                                            */
/* Department:  Inspection / Field Engineering                             */
/* Derivations: None                                                       */
/***************************************************************************/                                          
/* Revisions:								   												*/
/*                                                                         */
/***************************************************************************/
#ifndef __INCEscapeCodesh
#define __INCEscapeCodesh

/* erase functions */
#define CLEAR 		 			"\x1B[2J"     /* clears the entire screen */
#define CLEARLINE				"\x1B[K"      /* clear to end of line  */

/* dynamic cursor controls */
#define CURSORUP         	"\x1B[%dA"    /* cursor UP x lines */
#define CURSORDOWN       	"\x1B[%dB"    /* cursor DOWN x lines */
#define CURSORRIGHT      	"\x1B[%dC"    /* cursor RIGHT x spaces */
#define CURSORLEFT       	"\x1B[%dD"    /* cursor LEFT x spaces */
#define SAVECURSOR       	"\x1B[s"      /* save current cursor position */
#define RESTORECURSOR    	"\x1B[u"      /* return to saved cursor position */

/* set graphic conditions */
#define NORMAL		 			"\x1B[0m"     /* normal display */
#define BOLD             	"\x1B[1m"     /* bold on */
#define UNDERLINE        	"\x1B[4m"     /* underline (mono mode only) */
#define BLINK            	"\x1B[5m"     /* blink on */
#define REVERSEVIDEO       	"\x1B[7m"     /* reverse video */
#define INVISIBLE        	"\x1B[8m"     /* non-displayed (invisible) */
#define BLACKFOREGROUND  	"\x1B[30m"    /* black foreground */
#define REDFOREGROUND    	"\x1B[31m"    /* red foreground */
#define GREENFOREGROUND  	"\x1B[32m"    /* green foreground */
#define YELLOWFOREGROUND 	"\x1B[33m"    /* yellow foreground */
#define BLUEFOREGROUND   	"\x1B[34m"    /* blue foreground */
#define MAGENTAFOREGROUND	"\x1B[35m"    /* magenta foreground */
#define CYANFOREGROUND     "\x1B[36m"    /* cyan foreground */
#define WHITEFOREGROUND    "\x1B[37m"    /* white foreground */
#define BLACKBACKGROUND  	"\x1B[40m"    /* black background */
#define REDBACKGROUND    	"\x1B[41m"    /* red background */
#define GREENBACKGROUND  	"\x1B[42m"    /* green background */
#define YELLOWBACKGROUND 	"\x1B[43m"    /* yellow background */
#define BLUEBACKGROUND   	"\x1B[44m"    /* blue background */
#define MAGENTABACKGROUND	"\x1B[45m"    /* magenta background */
#define CYANBACKGROUND     "\x1B[46m"    /* cyan background */
#define WHITEBACKGROUND    "\x1B[47m"    /* white background */

/* screen modes */
#define BW40X25			"\x1B[=0;7h"  /* 40 char x 25 line black/white */
#define CO40X25			"\x1B[=1;7h"  /* 40 char x 25 line color */
#define BW80X25			"\x1B[=2;7h"  /* 80 char x 25 line black/white */
#define CO80X25			"\x1B[=3;7h"  /* 80 char x 25 line color */
#define BWG320X200		"\x1B[=5;7h"  /* 320 x 200 black/white graphics */
#define BWG640X200		"\x1B[=6;7h"  /* 640 x 200 black/white graphics */
#define BWG640X350		"\x1B[=15;7h" /* 640 x 350 black/white graphics */
#define BWG640X480		"\x1B[=17;7h" /* 640 x 480 black/white graphics */
#define LINEWRAP			"\x1B[=7;7h"  /* wrap at end of line */
#define COG320X200X4		"\x1B[=4;7h"  /* 320 x 200   4 color graphics */
#define COG320X200X16	"\x1B[=13;7h" /* 320 x 200  16 color graphics */
#define COG320X200X256	"\x1B[=19;7h" /* 320 x 200 256 color graphics */
#define COG640X200X16	"\x1B[=14;7h" /* 640 x 200  16 color graphics */
#define COG640X350X16	"\x1B[=16;7h" /* 640 x 350  16 color graphics */
#define COG640X480X16	"\x1B[=18;7h" /* 640 x 480  16 color graphics */

#define MODERESET       "\x1B[=l"     /* reset mode # set with above commands */

/********************************************************************/
/* function key assignments                                         */
/*                                                                  */
/* note that all these key assignments default to "dir" 				  */
/* because I had to start somewhere alter the assigments as you see */
/* fit by changing the text between the "" characters ALSO, the     */
/* first two number after the [ character are the extended ASCII    */
/* code for a particular key                                        */
/********************************************************************/
#define FKEY01	"\x1b[0;59;"dir";13p"  /* F1 key assignment */
#define FKEY02	"\x1b[0;60;"dir";13p"  /* F2 key assignment */
#define FKEY03	"\x1b[0;61;"dir";13p"  /* F3 key assignment */
#define FKEY04	"\x1b[0;62;"dir";13p"  /* F4 key assignment */
#define FKEY05	"\x1b[0;63;"dir";13p"  /* F5 key assignment */
#define FKEY06	"\x1b[0;64;"dir";13p"  /* F6 key assignment */
#define FKEY07	"\x1b[0;65;"dir";13p"  /* F7 key assignment */
#define FKEY08	"\x1b[0;66;"dir";13p"  /* F8 key assignment */
#define FKEY09	"\x1b[0;67;"dir";13p"  /* F9 key assignment */
#define FKEY10	"\x1b[0;68;"dir";13p"  /* F10 key assignment */
#define FKEY11	"\x1b[0;133;"dir";13p" /* F11 key assignment */
#define FKEY12 "\x1b[0,134;"dir";13p" /* F12 key assignment */

/* hard cursor positioners */
#define HOME 				"\x1B[H"
#define GOTOROWCOL		"\x1B[%d;%df"				/* position cursor at row,col*/
#define TOPLINE         "\x1B[0;01f"      		/* first terminal line */
#define LINE01          "\x1B[0;01f"				/* first terminal line */                           
#define LINE02				"\x1B[1;01f"            /* second terminal line */
#define LINE03          "\x1B[2;01f"            /* third terminal line */
#define LINE04          "\x1B[3;01f"            /* fourth terminal line */
#define LINE05          "\x1B[4;01f"            /* fifth terminal line */
#define LINE06          "\x1B[5;01f"            /* sixth terminal line */
#define LINE07          "\x1B[6;01f"            /* seventh terminal line */
#define LINE08          "\x1B[7;01f"            /* eighth terminal line */
#define LINE09          "\x1B[8;01f"            /* nineth terminal line */
#define LINE10          "\x1B[9;01f"            /* tenth terminal line */ 
#define LINE11          "\x1B[10;01f"           /* eleventh terminal line */
#define LINE12          "\x1B[11;01f"           /* twelfth terminal line */
#define LINE13          "\x1B[12;01f"           /* thirteenth terminal line */
#define LINE14          "\x1B[13;01f"           /* fourteenth terminal line */ 
#define LINE15          "\x1B[14;01f"           /* fifteenth terminal line */
#define LINE16          "\x1B[15;01f"           /* sixteenth terminal line */
#define LINE17          "\x1B[16;01f"           /* seventeenth terminal line */
#define LINE18          "\x1B[17;01f"           /* eighteenth terminal line */
#define LINE19          "\x1B[18;01f"           /* nineteenth terminal line */
#define LINE20          "\x1B[19;01f"           /* twentyth terminal line */
#define LINE21          "\x1B[20;01f"           /* twentyfirst terminal line */
#define LINE22          "\x1B[21;01f"           /* twentysecond terminal line */
#define LINE23          "\x1B[22;01f"           /* twentythird terminal line */
#define LINE24          "\x1B[23;01f"           /* twentyfourth terminal line */
#define LINE25          "\x1B[24:01f"           /* twenthfifth terminal line */
#define BOTTOMLINE 		"\x1B[24;01f"				/* last terminal line */

/* normal key presses */
#define BELL            "\x1B\x07"					/* rings the terminal bell  */
#endif

