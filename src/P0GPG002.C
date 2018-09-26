/* 
**  Sybase DB interface and utility library in the unix system.
** ================================================================
**  Copyright 1993,1996 Lee, Kwang Woo(leewin) All Rights Reserved.
**  E-mail : leewin@hitel.kol.nm.kr
** ================================================================
*/

#ifndef _P0GPG002_C  
#define _P0GPG002_C

#pragma ident "%W% %E% leewin"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>
#include <signal.h>
#include <sybfront.h> 
#include <sybdb.h> 
#include <p0gpg001.h>
#include <p0gpg003.h>
#include <p0gpg002.h>
#endif

/* User redefined Sybase DB-library error handle.
** Forward declarations of the error handler and message handler.
*/
int ErrHandler(dbproc, severity, dberr, oserr, dberrstr, oserrstr)
DBPROCESS       *dbproc;
int             severity;
int             dberr;
int             oserr;
char            *dberrstr;
char            *oserrstr;
{
    char src[BUFLEN];

    memset(src, '\0', sizeof(src));
    sprintf( src,"DBMS E-Msg : %4d %s\n", dberr,dberrstr );

    if ((dbproc == NULL) || (DBDEAD(dbproc)))
    return(INT_EXIT);
    else {
        Fatal( src, (int)RDBMSMESSAGE ); 

        if (oserr != DBNOERR) {
            memset(src, '\0', sizeof(src));
            sprintf( src,"DBMS E-Sys : %4d %s\n", oserr,oserrstr );
            Fatal( src, (int)RDBMSMESSAGE ); 
        }
    }

    /* when DB Library error, all process killing */
    if( severity >= 16 ) 
        kill( -1, SIGUSR2 ); 

    return(INT_EXIT);
}


/* User redefined Sybase DB-library message handle.
** Forward declarations of the error handler and message handler.
*/
int MsgHandler(dbproc, msgno, msgstate, severity, msgtext, srvname, 
                                                  procname, line)
DBPROCESS       *dbproc;
DBINT           msgno;
int             msgstate;
int             severity;
char            *msgtext;
char            *srvname;
char            *procname;
DBUSMALLINT     line;
{
    char src[BUFLEN];

    /* general message remove */
    if( msgno > 5700 && msgno < 5705 )
        return(0);

    memset(src, '\0', sizeof(src));
    strcpy( src,"DBMS L-Msg : " );

	if (strlen(srvname) > 0) {
		strcat (src, "(Server) ");
		strcat (src, srvname);
    }
	if (strlen(procname) > 0) {
		strcat (src, "(Procedure) ");
		strcat (src, procname);
    }

	strcat (src, "(Msg)");
	strcat (src, msgtext );
             
    Fatal( src, (int)RDBMSMESSAGE );

    /* when DBMS log full */
    if( msgno == 7409 || msgno == 7412 || msgno == 7413 || msgno == 7415 )
        kill( getpid(), SIGUSR1);

    /* when over user, waiting  */
    if( msgno == 7401 ) sleep(10); 
    
	return(0);
}

/* Starting by signal handler.
** Clear Sybase DBMS transaction buffer when log buffer full. 
*/
int FreeLog()
{
    LOGINREC         *login;
    DBPROCESS        *Proc;

    if (dbinit() == FAIL)
        Fatal("FreeLog()", ECDBINI );
    
    dberrhandle(ErrHandler);
    dbmsghandle(MsgHandler);

    login = dblogin();
    DBSETLUSER(login, UserId);
    DBSETLPWD( login, Password);
    DBSETLAPP( login, AppName);

    if( strlen(ServerName) > 0 )
        Proc = dbopen(login, ServerName);
    else Proc = dbopen(login, NULL );

    if( Proc == (DBPROCESS *)NULL) {
        dbexit();
        Fatal( "FreeLog()", ECDBOPN );
    }

    Fatal(" Sybase Log file dump process start ", ECWARN );

    dbfcmd(Proc, "dump tran %s with no_log ", CurDataBase ); 
    dbsqlexec( Proc );
    while( dbresults(Proc) != NO_MORE_RESULTS )
        continue;
    dbclose(Proc);
    
    /* for next time ... */
    signal( SIGUSR1, FreeLog );

    return(0);
}

/* Make normal exit condition from fatal DB-library error.
*/
int GoodbyeSybase()
{
    LOGINREC         *login;
    DBPROCESS        *Proc;

    if (dbinit() == FAIL)
        Fatal("GoodbyeSybase()", ECDBINI );
    
    dberrhandle(ErrHandler);
    dbmsghandle(MsgHandler);

    login = dblogin();
    DBSETLUSER(login, UserId);
    DBSETLPWD( login, Password);
    DBSETLAPP( login, AppName);

    if( strlen(ServerName) > 0 )
        Proc = dbopen(login, ServerName);
    else Proc = dbopen(login, NULL );

    if( Proc == (DBPROCESS *)NULL) {
        dbexit();
        Fatal( "GoodbyeSybase()", ECDBOPN );
    }

    dbuse( CurDataBase );
/*
    dbcmd(Proc, "quit "); 
    dbsqlexec( Proc );
    while( dbresults(Proc) != NO_MORE_RESULTS )
        continue;
*/
    dbexit();

    /* for next time ... */
    signal( SIGUSR2, GoodbyeSybase );

    return(0);
}

/* Db-library initialize and making LOGINREC.
** Return new DBPROCESS pointer.
*/
DBPROCESS *ConnectDBLib(SvrName)
char *SvrName;
{
    DBPROCESS        *dbproc;

    if ( pHeadLogin == (LOGINREC*)NULL ) {
        if (dbinit() == FAIL)
            Fatal("ConnectDBLib()", ECDBINI );
    
        dberrhandle(ErrHandler);
        dbmsghandle(MsgHandler);

        pHeadLogin = dblogin();
        if ( pHeadLogin == (LOGINREC*)NULL )
            Fatal("ConnectDBLib()", ECDBLGN );
        SetDBLoginInfo(pHeadLogin);
    }

    if( strlen(SvrName) > 0 )
        dbproc = dbopen(pHeadLogin, SvrName);
    else dbproc = dbopen(pHeadLogin, ServerName );

    if( dbproc == (DBPROCESS *)NULL) {
        dbexit();
        Fatal( "DbConnect()", ECDBOPN );
    }

    return(dbproc);
}

/* Deallocate LOGINREC and disconnect all DBPROCESS.
*/
void DisConnectDBLib()
{
   if ( pHeadLogin != (LOGINREC*)NULL )
       dbloginfree(pHeadLogin); 
   dbexit();
   return;
}

/* Return new DBPROCESS pointer for bulk-copy process.
*/
DBPROCESS *ConnectBcp(SvrName)
char *SvrName;
{
    DBPROCESS *dbproc;
    LOGINREC *login;

    if ( pHeadLogin == (LOGINREC*)NULL ) {
        if (dbinit() == FAIL)
            Fatal("ConnectBcp()", ECDBINI );
    
        dberrhandle(ErrHandler);
        dbmsghandle(MsgHandler);
    }

    login = dblogin();
    if ( login == (LOGINREC*)NULL )
        Fatal("ConnectBcp()", ECDBLGN );

    SetDBLoginInfo(login);
    BCP_SETL( login, TRUE );	

    if( strlen(SvrName) > 0 )
        dbproc = dbopen(login, SvrName);
    else dbproc = dbopen(login, ServerName );

    if( dbproc == (DBPROCESS *)NULL) {
        dbexit();
        Fatal( "ConnectBcp()", ECDBOPN );
    }
    dbloginfree(login); 

    return(dbproc);
}
