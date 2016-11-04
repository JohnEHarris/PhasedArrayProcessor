========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : Asi
========================================================================


AppWizard has created this Asi application for you.  This application
not only demonstrates the basics of using the Microsoft Foundation classes
but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your Asi application.

Asi.dsp
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

Asi.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CAsiApp application class.

Asi.cpp
    This is the main application source file that contains the application
    class CAsiApp.

Asi.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Visual C++.

Asi.clw
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

res\Asi.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file Asi.rc.

res\Asi.rc2
    This file contains resources that are not edited by Microsoft 
	Visual C++.  You should place all resources not editable by
	the resource editor in this file.




/////////////////////////////////////////////////////////////////////////////

AppWizard creates one dialog class:

AsiDlg.h, AsiDlg.cpp - the dialog
    These files contain your CAsiDlg class.  This class defines
    the behavior of your application's main dialog.  The dialog's
    template is in Asi.rc, which can be edited in Microsoft
	Visual C++.


/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named Asi.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, and your application is 
in a language other than the operating system's current language, you
will need to copy the corresponding localized resources MFC42XXX.DLL
from the Microsoft Visual C++ CD-ROM onto the system or system32 directory,
and rename it to be MFCLOC.DLL.  ("XXX" stands for the language abbreviation.
For example, MFC42DEU.DLL contains resources translated to German.)  If you
don't do this, some of the UI elements of your application will remain in the
language of the operating system.

/////////////////////////////////////////////////////////////////////////////

/*************!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#################*********************/

SPECIAL CONVENTIONS FOR BUILDING THIS PROJECT
1.  EACH DIALOG HAS AN ASSOCIATED .H FILE WHEN BUILT BY CLASSWIZARD.  PUT THAT HEADER
	INTO ASIDLG.H AND REMOVE FROM THE DIALOG'S  CPP FILE

2.  IN THE DIALOG'S CPP FILE, INCLUDE AsiDlg.h.

3  IN THE .H FILE WHICH IS NOW INCLUDED BY AsiDlg.h, place the public statement:
   static  CChildDialog* m_pDlg;
  THIS NAME, m_pDlg is used by convention.

4.  IN THE FILE, AsiDlg.cpp, place the static instantiation of this variable with a statement:
	CChild* CChild::m_pDlg;
PLACE THIS CODE AFTER THE COMMENT IN THE AsiDlg.cpp file:

/*********************************************************
Every child dialog must have a pointer to it.  Declare all
the child of AsiDlg dialog pointers here and in the child
dialog's header file with the statement:
static CChild *m_pDlg  where "child" is the dialog class name
**********************************************************/
CAsiDlg* pCAsiDlg;
CCatch* CCatch::m_pDlg;

5.  TO NULL THIS POINTER ON THE WINDOW'S CLOSE, SEE THE ONOK, ONCANCEL CODE IN MODULE
	CATCH.CPP.  IN ORDER TO GET THE WIZARD TO BUILD THE ONOK AND ONCANCEL FUNCTIONS,
	YOU MUST PUT IN 2 DUMMY BUTTONS WITH CONTROL ID'S OF  ONOK  and ONCANCEL respectively.
	AFTER YOU HAVE MADE THE FUNCTIONS CORRESPONDING TO THESE ID'S YOIU CAN DELETE
	THE BUTTONS FROM THE DIALOG.




/*************!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!#################*********************/

A040301
April 3, 2001

T:\Truscop2\MMI is home folder for New Truscope UDP/MMI project.

RENAME Asi project files from Asi.cpp to Truscan.cpp/h and
AsiDlg.cpp/h to TScanDlg.cpp/h.  Change class names from Asixxx to Tscanxxx.
Comment out code which was specific to Amalog/Sono/Iso by msg header file
or config.h and begin to replace with ut specific code.
Compiles ok but may not run at all.


B033001
Make destructor in ipxdlg.cpp to empty plistIpxIn and plistTemp.
Force catch.cpp to empty input list down to last element before
displaying.

C033001
Change tscandlg msg 4 & 19 to accomodate 2nd cat of oblique.

A040201
Make catch window larger at top, smaller at bottom in prep for
inst catch window.


A040301
Move Truscan/Truscope code and Include folder from I:\Truscan to T:\Truscop2.
Eliminate asi_ipx.h and replace with Instdata.h.  Uses existing Truscope
pc104 based instrument and packet data as input to UDP.  Uses Truscan 
cmnds to control udp.  (Commands numbers are biased up by 1000).  Only catch
window for raw packet data works initially.

B040301
Bias all cmnds to dos instrument by 1000.  Allows for Truscope instrument
commands to be included.  In instrument, if cmnd > 999, subtract
1000 from command.

C040301
Add channel button and raw message type to Catch dialog screen.
Not operational yet.  Remove broadcast and netset buttons.

D040301
Scroll bar works for ipx raw packet display in catch dialog.
No message yet to udp or catch screen display.

A040501
First coding of catch with msg03, truscope raw ipx packet.  Only 10
chnls with simulator.  No msg to set chnl number yet.

B040501
working version of Truscan mmi to display ipx data raw in catch.


A120701
Integrate New Udp header files into project.

A121001
Prior to removing win 3.11 constructs

B121001
Embed case selection for trace 2 into combo box ascii.  Add debug/normal
button on dialog.  Remove many/ most win 3.11 constructs.

A121201
Beginning to add context help to Oscope dialog.  Patter for other
dialogs.  Create same window for both normal and diagnostic scope
display but trace2 window has different contents depending on
whether diagnostic or normal.  Defaults to pc 104 debug off when
dialog is opened.

A121301
Trace1 now combo box again instead of radio buttons.  Same commands
as Existing win 3.11 operational scope dialogs.

B121301
Improve on un-do operation and choices.  Make undo/help/clos look like
3.11 layout.

C121301
Send 104 debug value at beginning of scope dialog.  Always turn off
104 output debugging when opening scope dialog.  Msg 48 & 49 scope
trace msgs work like 3.11 mmi.

A121401
Change oscpe and setup to use seq len of 2 or 1 for firing seq info.
change undo in setup to be like oscpe.

A032002
3 cat oblq plot and map.  Still not setting up inst correctly.  Probably not
setting seq len.  Testing scope dmx and motion bus features.

A032502
Set run mode to nop when downloading config file and the restore mode at end.

A032502
Some delays on mmi side in sending sysinit/cfg file. (intentional).  Maybe
not necessary.  Can inspect and can download with success 1 inst and
1 simulator.

A032602
Eliminate sysinit delays.  Machines still download ok but real inst
loses 1 packet. Simulator, none.
Move scroll bar table from 3.11 code into sbctbl.h used only by tcg.cpp.

B032602
Eliminate filter.cpp, isocal.cpp and gain.cpp.  Move receiver and gates in tscandlg
out of virtual implementation.  Add Showreceiver and show gate routines to acal.cpp
Put cfg file name on top title bar.  Implement joint cmd and allow to go to 4 billion
for joint number.

A032702
Change format of cal msg.  Add code to process cal msg and show echo bits.

B032702
Cal msg coming thru.  CAtch shows cal msg.  Udp seems ok.  Need to change acal.cpp
back to look like utron2 code.

C032702
Acal was so messed up being a merge of ASI/Utron2/ Win3.11 MMI that I went back to 
5/25/01 code.  Had to disable a lot to get the new (current) dialog to mesh with the
code.  Will start tomorrow to rebuild dialog using win32 techniques and forget
win3.11 code.

A032802
Fix pulser final position, cancel operation.

A040202
Tholds for all obq's diaplay on inspect screen.  Help contents correct from setup screen.

A040902
Show all 40 chnls in echo.  Not all boxes paint.  May be screen resolution problem.
font down from 8 to 6 and w/serif.  Fix mouse position over q2/q3 in inspect screen.

A041002
Remove ref's to Utron, replace with Truscope-2.  Change afx header for uniqueueness to
show truscope2 instead of asi.  Add obq 2&3 to cal window and make cal lvl slider work.
Change echo box and hard code some idc numbers to get to work.

A041102
Copy inspect code from Utron-2 to allow up to 6 pages of tabular inspection data
 on printout.

A041802
NcNx cmnds implemented.  system_init converted to class member.

A042402
Add nc display boxes to Inspect dialog.  Fix print/replay print to not print empty
pages on text only pages.  Stretch inspect screen to full screen width.  Eliminate
flaw count categories from status window.  Move pipe length to position formerly
used for "RANGE".

B042402.
Fill in cal procedures.

A042502
Fix open dialog size, align status window controls.  Add offset
code to setup.

A042602
Xdcr default offsets start with +5 instead of -5.

a042902
Changes to copy-to function, moved xdcr offsets in ConfigUdp window up under 
xdcr type selection list box.

A043002
Attempting to make continuous replay mode for otc.

A050702
Working on wall cal dialogs and udp wall cal operations.

A051002
Dialog and code to write site specific default values for x location, clock adjustment to
registry.  Wall coef msg's for both raw data (slope = 1, offset = 0) and EU data.
Detect wheter wall data is raw or eu from status word in inspect header.  Add code to
master and slave to pass up instrument download status but not displayed yet in mmi.

A051602
Sending job rec to master udp so he can send info to cdp.  Master udp records received
job rec.  Size of mmi buffer increased from 450 to 600 to accomodate job rec.

A052302
Add modeless dialog StrpChnl to allow operator to select chnl to display on a trace.
All chnls = 0xff as stored in UtRec.DisplayChannel[trace type]. 0=w,1=l,2=t,3=Q1 etc.
Detect msg to set display channels and if any trace fails to select ALL CHANNELS
for its option, set an alarm bit (14) in alarm screen.  Not yet implemented.
No implementation on vxworks side yet.  Changed UtRec in name but not is size.

A070203 - Yanming Guo
Add tooltip to the network status buttons.  Affected functions include CTscanDlg::OnInitDialog(),
CTscanDlg::ShowNetworkAlarm(), and CTscanDlg::ShowNetworkAlarm().

A070203 - Yanming Guo
Add CTscanDlg::SwitchInspectWindows() and CInspect::MoveReportWindow() to enable inspection
window switching whenever a new pipe comes in.  SwitchInspectWindows is called in CInspect::DrawMap().

C070203 - Yanming Guo
Change the structures RAW_WALL_HEAD and JOB_REC.

C073003 -YG, CL, KBM
Modify ipxDlg.cpp, inspect.h, tcpThreadRXList.cpp, TCGSetupDlg.cpp, inspect.cpp, tscandlg.cpp

C121003 - YG
Can run in time mode; fix TCG spike; add TCG curve 2 to get higher slope; fix gate delay, range, 
and blank conversion (from float to short) error.

