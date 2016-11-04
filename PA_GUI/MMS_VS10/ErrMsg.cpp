/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
*   ErrMsg.c
*
*   COPYRIGHT 1999 BY TUBOSCOPE INC.  ALL RIGHTS RESERVED.
*
*   06-04-99   MWTaylor

	01/05/99	rename to errmsg.cpp  jeh
*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "stdafx.h"
#include "ErrMsg.h"

#include <winsock2.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*                The following functions are in this module               */

const char* GetWSAError(void);
const char* GetWSAErrStr(int errval);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*                 Variables which are global to all modules               */

/*              Variables which are global to this module only             */

static char *psWSAerr[] =
{
//  Windows Sockets definitions of regular Microsoft C error constants

"Unknown WSAError",
NULL,NULL,NULL,
"WSAEINTR",              //  (WSABASEERR+4)
NULL,NULL,NULL,NULL,
"WSAEBADF",              //  (WSABASEERR+9)
NULL,NULL,NULL,
"WSAEACCES",             //  (WSABASEERR+13)
"WSAEFAULT",             //  (WSABASEERR+14)
NULL,NULL,NULL,NULL,
NULL,NULL,NULL,
"WSAEINVAL",             //  (WSABASEERR+22)
NULL,
"WSAEMFILE",             //  (WSABASEERR+24)

//  Windows"Sockets"definitions"of"regular"Berkeley"error"constants

NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,
NULL,NULL,
"WSAEWOULDBLOCK",        //  (WSABASEERR+35)
"WSAEINPROGRESS",        //  (WSABASEERR+36)
"WSAEALREADY",           //  (WSABASEERR+37)
"WSAENOTSOCK",           //  (WSABASEERR+38)
"WSAEDESTADDRREQ",       //  (WSABASEERR+39)
"WSAEMSGSIZE",           //  (WSABASEERR+40)
"WSAEPROTOTYPE",         //  (WSABASEERR+41)
"WSAENOPROTOOPT",        //  (WSABASEERR+42)
"WSAEPROTONOSUPPORT",    //  (WSABASEERR+43)
"WSAESOCKTNOSUPPORT",    //  (WSABASEERR+44)
"WSAEOPNOTSUPP",         //  (WSABASEERR+45)
"WSAEPFNOSUPPORT",       //  (WSABASEERR+46)
"WSAEAFNOSUPPORT",       //  (WSABASEERR+47)
"WSAEADDRINUSE",         //  (WSABASEERR+48)
"WSAEADDRNOTAVAIL",      //  (WSABASEERR+49)
"WSAENETDOWN",           //  (WSABASEERR+50)
"WSAENETUNREACH",        //  (WSABASEERR+51)
"WSAENETRESET",          //  (WSABASEERR+52)
"WSAECONNABORTED",       //  (WSABASEERR+53)
"WSAECONNRESET",         //  (WSABASEERR+54)
"WSAENOBUFS",            //  (WSABASEERR+55)
"WSAEISCONN",            //  (WSABASEERR+56)
"WSAENOTCONN",           //  (WSABASEERR+57)
"WSAESHUTDOWN",          //  (WSABASEERR+58)
"WSAETOOMANYREFS",       //  (WSABASEERR+59)
"WSAETIMEDOUT",          //  (WSABASEERR+60)
"WSAECONNREFUSED",       //  (WSABASEERR+61)
"WSAELOOP",              //  (WSABASEERR+62)
"WSAENAMETOOLONG",       //  (WSABASEERR+63)
"WSAEHOSTDOWN",          //  (WSABASEERR+64)
"WSAEHOSTUNREACH",       //  (WSABASEERR+65)
"WSAENOTEMPTY",          //  (WSABASEERR+66)
"WSAEPROCLIM",           //  (WSABASEERR+67)
"WSAEUSERS",             //  (WSABASEERR+68)
"WSAEDQUOT",             //  (WSABASEERR+69)
"WSAESTALE",             //  (WSABASEERR+70)
"WSAEREMOTE",            //  (WSABASEERR+71)
NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,
NULL,NULL,NULL,

//  Extended"Windows"Sockets"error"constant"definitions

"WSASYSNOTREADY",        //  (WSABASEERR+91)
"WSAVERNOTSUPPORTED",    //  (WSABASEERR+92)
"WSANOTINITIALISED",     //  (WSABASEERR+93)
NULL,NULL,NULL,NULL,
NULL,NULL,NULL,
"WSAEDISCON",            //  (WSABASEERR+101)
"WSAENOMORE",            //  (WSABASEERR+102)
"WSAECANCELLED",         //  (WSABASEERR+103)
"WSAEINVALIDPROCTABLE",  //  (WSABASEERR+104)
"WSAEINVALIDPROVIDER",   //  (WSABASEERR+105)
"WSAEPROVIDERFAILEDINIT",//  (WSABASEERR+106)
"WSASYSCALLFAILURE",     //  (WSABASEERR+107)
"WSASERVICE_NOT_FOUND",  //  (WSABASEERR+108)
"WSATYPE_NOT_FOUND",     //  (WSABASEERR+109)
"WSA_E_NO_MORE",         //  (WSABASEERR+110)
"WSA_E_CANCELLED",       //  (WSABASEERR+111)
"WSAEREFUSED"            //  (WSABASEERR+112)
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const char* GetWSAError()
{
  return GetWSAErrStr(WSAGetLastError());
}   //  End GetWSAError()  06-14-99

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

const char* GetWSAErrStr(int errval)
{
  errval -= WSABASEERR;
  if (errval < 0 || errval > 112) errval = 0;
  return psWSAerr[errval];
}   //  End GetWSAErrStr()  06-14-99

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

/*          COPYRIGHT 1999 BY TUBOSCOPE INC.  ALL RIGHTS RESERVED.         */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

