@ECHO OFF
:START
ECHO Infinite Loop, kill window to exit
winsat formal -v -xml c:\winstatresults.xml
cls
ECHO ---------------*------------------
ECHO -------------*****----------------
ECHO -----------*********--------------
ECHO ---------*************------------
ECHO -----------*********--------------
ECHO -------------*****----------------
ECHO ---------------*------------------
GOTO :START
