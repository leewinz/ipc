/* 
**  Sybase DB interface and utility library in the unix system.
** ================================================================
**  Copyright 1993,1996 Lee, Kwang Woo(leewin) All Rights Reserved.
**  E-mail : leewin@hitel.kol.nm.kr
** ================================================================
*/

#ifndef _P0GPG002_H
#define _P0GPG002_H

#pragma ident  "%W% %E% leewin"

#include <p0gpg001.h>
#ifndef __sybfront__
#include <sybfront.h> 
#endif
#ifndef __sybdb__
#include <sybdb.h> 
#endif

#ifdef _cplusplus
extern "C" {
#endif

#define SetDBLoginInfo(a) ( DBSETLUSER(a,UserId),  \
                            DBSETLPWD(a,Password), \
                            DBSETLAPP(a,AppName) )
#ifdef _P0GPG002_C  

LOGINREC  *pHeadLogin;

/* parameter: 
** DBPROCESS*,int,int,int,char*,char*
** purpose:  management Sybase Error status 
*/
int  ErrHandler();  
/* parameter: 
** DBPROCESS*,DBINT,int,int,char*,char*,char*,DBUSMALLINT 
** purpose:  management  Sybase general message
*/
int  MsgHandler();  
/* signal SIGUSR1 procedure 
** purpose:  free from Sybase log file full
*/
int  FreeLog();     
/* signal SIGUSR2 procedure
** purpose: abnormal terminate process management 
*/
int GoodbyeSybase();  
DBPROCESS *ConnectDBLib();
DBPROCESS *ConnectBcp();
void DisConnectDBLib();
#else
extern LOGINREC  *pHeadLogin;
extern int  ErrHandler();
extern int  MsgHandler();
extern int  FreeLog();
extern int  GoodbyeSybase();
extern DBPROCESS *ConnectDBLib();
extern DBPROCESS *ConnectBcp();
extern void        DisConnectDBLib();
#endif

#ifdef _cplusplus
}
#endif

#endif 
