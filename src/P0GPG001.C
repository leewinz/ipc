/* 
** unix system utility library
** ================================================================
**  Copyright 1993,1996 Lee, Kwang Woo(leewin) All Rights Reserved.
**  E-mail : leewin@hitel.kol.nm.kr
** ================================================================
*/

#ifndef _P0GPG001_C  
#define _P0GPG001_C

#pragma ident  "%W% %E% leewin"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <memory.h>
#include <malloc.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#ifdef	SIGTSTP		/* true if BSD system */
#include<sys/file.h>
#include<sys/ioctl.h>
#endif
#include <p0gpg001.h> 
#include <p0gpg003.h> 
#endif

/* Connect for this library 
** At least one call before using library function
*/
void ConnectLib( usr,pwd, app, svr )
char *usr,*pwd,*app,*svr;
{
    char *enviro;

    if( (enviro = getenv("LEEWIN")) == (char*)NULL )
        enviro = (char*)LEEWIN;
    if( (char*)usr != (char*)NULL )
        strcpy( UserId, usr );
    if( (char*)pwd != (char*)NULL )
        strcpy( Password, pwd);
    if( (char*)app != (char*)NULL )
        strcpy( AppName, app);
    if( (char*)svr != (char*)NULL )
        strcpy( ServerName, svr);
    strcpy( ErrorList, enviro );
    strcat( ErrorList, "/" );
    strcpy( PathList, ErrorList );
    if( (char*)app != (char*)NULL )
        sprintf( ErrorList+strlen(ErrorList),"%s.log", app);
    else 
        sprintf( ErrorList+strlen(ErrorList),"%d.log", getpid());

    return;
}

/* Old format for library connecting
*/
void LibraryInterface( usr,pwd, app, svr )
char *usr,*pwd,*app,*svr;
{
    ConnectLib( usr,pwd, app, svr );
    return;
}

/* remove program message log file 
*/
void RemoveList( str ) 
char *str;
{
    int pid = -1;
/*
#ifdef SIGCLD
    signal( SIGCLD, SIG_DFL );    
#endif
*/
    switch( (pid = fork()) ) {
        case  -1 : Fatal( "RemoveErrorList()", ECCPROC );
                   break;
        case   0 : unlink( str );
                   exit(0);
        default  : wait( (int*)0 );
                   break;
    }
/*
#ifdef SIGCLD
    signal( SIGCLD, SignalChild );    
#endif     
*/
    return;
}   

/* Write message to log file and exit if fatal error
*/
void Fatal( string ,flag )
char *string;
int flag;
{
    FILE *fp;
    char src[2][BUFLEN];


    memset(src, '\0', sizeof(src));
    if( flag != (int)RDBMSMESSAGE )  /* RDBMSMESSAGE : 7000 */
        sprintf( src[0],"Programmer Msg : %4d %s", flag,string );
    else strcpy( src[0], string );
    sprintf( src[1],"System     Msg : %4d %s", errno,sys_errlist[errno] );

    if( MsgSwitch ) { 
        fprintf( stderr,"%s\n", src[0] );
        fprintf( stderr,"%s\n", src[1] );
    }
    else {
        fp = fopen( ErrorList,"a+t");
        if( fp == (FILE*)NULL ) 
            fprintf( stderr, "Error list file open error\n");
        else {
            fprintf( fp, "%s\n",  src[0] );
            fprintf( fp, "%s\n\n",src[1] );
            fclose(fp);
        }
    }

    if( flag < 0 ) {
        signal( SIGINT, SIG_DFL );     /* signal interrupt default */ 
        signal( SIGQUIT, SIG_DFL );    /* signal quit default */ 
        signal( SIGUSR1, SIG_DFL );    /* restore SIGUSR1 */
        signal( SIGUSR2, SIG_DFL );    /* restore SIGUSR2 */
        if( JepumQueue > 0 ) InternalServerKill( JepumQueue );
        exit(1);
    }
    else errno = 0;

    return;
}

/* Convert low case string to upper case string
*/
void StrToUpr( str )
char *str;
{
    int length,k;
         
    length = strlen(str);
    for(k=0; k < length; k++ )
       str[k] = toupper( str[k] );

    return;
}

/* Return current date-time string by request length.
** request length is 2:  "yy"
**                   4:  "yymm"
**                   6:  "yymmdd"
**                   8:  "yymmddhh" 
**                  10:  "yymmddhhmm"
**                  12:  "yymmddhhmmss"
*/
void TimeStemp( tar,p )
char *tar;
unsigned p;
{
    register int i;
    time_t ti;
    struct tm *tt;
 
    ti = time(NULL);
    tt = localtime(&ti);

    memset( tar, 0x30, p );
    p = ( p % 2 ) ? p--: p;
    i = p; 
    --i;
    switch( p ) {
        case 12:
            tar[i--] = ( tt->tm_sec  %  10   ) + 0x30;
            tar[i--] = ( tt->tm_sec  /  10   ) + 0x30;
        case 10:
            tar[i--] = ( tt->tm_min  %  10   ) + 0x30;
            tar[i--] = ( tt->tm_min  /  10   ) + 0x30;
        case  8:
            tar[i--] = ( tt->tm_hour %  10   ) + 0x30;
            tar[i--] = ( tt->tm_hour /  10   ) + 0x30;
        case  6:
        default:
            tar[i--] = ( tt->tm_mday %  10   ) + 0x30;
            tar[i--] = ( tt->tm_mday /  10   ) + 0x30;
        case  4:
            tar[i--] = ( (tt->tm_mon+1) %  10   ) + 0x30;
            tar[i--] = ( (tt->tm_mon+1) /  10   ) + 0x30;
        case  2:
            tar[i--] = ( tt->tm_year %  10   ) + 0x30;
            tar[i]   = ( tt->tm_year /  10   ) + 0x30;
            break;
     }
}

/* Convert string-day to integer values 
** between string-day from start date(1994/1/1).
** Using from 1994 year to 2090 year.
*/
int StringToDay( str_day )
char *str_day;
{
    int nal,yy,mm,dd,j,i=STARTDATE;
    char temp[3];
      
    memset( temp,'\0',sizeof(temp) );
    memcpy( temp,str_day,2 );

    yy = atoi(temp);
    if( yy <= 90 )      
        yy += 100;

    yy += 1900;

    memset( temp,'\0',sizeof(temp) );
    memcpy( temp,str_day+2,2 );
    mm = atoi(temp);
  
    memset( temp,'\0',sizeof(temp) );
    memcpy( temp,str_day+4,2 );
    dd = atoi(temp);
    nal = 0;

    while ( i < yy )
    {    
        for( j=0; j < 12 ; j++ )
            nal += Month[j];

        i++;

        if( !(i%4) )    Month[1] = 29;
        if( !(i%100) )  Month[1] = 28;
        if( !(i%400) )  Month[1] = 29;
    }
      
    for( j=0; j < mm-1 ; j++ )
        nal += Month[j];
   
    nal += dd;

    /* recover 1994 year */
    Month[1] = 28;

    return(nal);
}

/* Compute Term from integer values.
** Using from 1994 year to 2090 year. 
*/
int DayToTerm( day )
int day;
{
    int nal,j,i,yy=STARTDATE;
    char temp[3];
    BOOLEAN flag = True;

    nal = i = 0;

    while (flag)
    {    
        for( j=0; j < 12 && nal < day; j++ )
        { 
            nal += Month[j];
            i += 3;
        }

        if( nal >= day ) {
            nal -= Month[--j];
            i -= 3;
            break;
        }

        yy++;

        if( !(yy%4) )    Month[1] = 29;
        if( !(yy%100) )  Month[1] = 28;
        if( !(yy%400) )  Month[1] = 29;
    }
      
    if( (j = ( day - nal ) / 10 ) < 3 )
        j++;

    i += j;
    
    /* recover 1994 year */
    Month[1] = 28;

    return(i);
}

/* Convert integer date values to date string format.
** Using from 1994 year to 2090 year 
*/
void DayToString(string, day)
char *string;
int day;
{
    int nal,j,i,yy=1994;
    BOOLEAN flag = True;

    nal = i = 0;

    memset(string, '\0', sizeof(string));
    while (flag)
    {    
        for( j=0; j < 12 && nal < day; j++ )
            nal += Month[j];

        if( nal >= day ) {
            nal -= Month[j-1];
            day -= nal;
            yy -= 1900;
            sprintf( string, "%02d%02d%02d", yy, j, day );     
            break;
        }

        yy++;

        if( !(yy%4) )    Month[1] = 29;
        if( !(yy%100) )  Month[1] = 28;
        if( !(yy%400) )  Month[1] = 29;
    }
    
    /* recover 1994 year */
    Month[1] = 28;

    return;
}

/* Initialize and create message queue for RPC.
*/ 
int InitQueue( Key )
key_t Key;
{
    int MsgId = -1;

    if( (MsgId = msgget( Key, QPERM | IPC_CREAT )) < 0 )
        Fatal("InitQueue()", EMSGGET );

    return( MsgId );
}

/* Remove messge queue
*/
void MsgRemove( MsgId )
int MsgId;
{
    struct msqid_ds MsgStat;
    int retvalue = -1;
    
    if( (retvalue = msgctl( MsgId, IPC_RMID, &MsgStat )) < 0 )
        Fatal("MsgRemove()", EMSGDEL );
        
    return;
}

/* Transfer message to message-queue.
** message type
**  10000         : ServerJepum process kill
**  10000 - 19999 : insert
**  20000 - 29999 : max nalpan lenth update      
**  30000 - 39999 : over roll update      
*/
void SendMessage( MsgId, src,mtype )
int MsgId;
char *src;
long mtype;
{
    MESSAGE message; 
    int retval = -1,len;
    char err[50];

    memset( &message, '\0', sizeof(message));  
    memset( err, '\0', sizeof(err));
    sprintf(err,"SendMessage(%4ld message)", mtype );            

    message.type = mtype;
    len = strlen(src);
    strcpy( message.data, src );

    /* prevent system interrupt, message queue full error */ 
    do{
        retval = msgsnd( MsgId, &message,len, 0);
    }while( retval == -1 && (errno == EINTR || errno == EAGAIN) );

    if( retval < 0 ) Fatal( err, EMSGSND );

    return;
}

/* Process management.
** jmp type  1: wait until all child processes exit.
**           2: wait until one process
**           3: prevent creating more than PROCLIMT
*/
void CheckProcess( jmp, pid )
int jmp, pid;
{
    int ret,es, Rest;
    char Mesg[30];

    switch( jmp ) { 
        case  0:
            Rest = pid;
            while( ProcCnt > Rest ) 
            {   
                memset( Mesg, '\0', sizeof(Mesg) );
                ret = wait(&es);
                es = (es >> 8) & 0xFF;
                if( es ) {
                    sprintf( Mesg, "proc id: %d exit status %d", ret, es );
                    Fatal( Mesg, ret );
                }
                ProcCnt--;
            }
            break;
        case  1:
            while( (ret = wait(&es) ) != pid ) ;
            es = (es >> 8) & 0xFF;
            if( es ) {
                sprintf( Mesg, "proc id: %d exit status %d", ret, es );
                Fatal( Mesg, ret );
            }
            break;
        case  2:
        default:
            while( ProcCnt > (int)PROCLIMIT )
            {
                if( (ret = wait(&es)) == -1 )
                    break;
                es = (es >> 8) & 0xFF;
                if( es ) {
                    sprintf( Mesg, "proc id: %d exit status %d", ret, es );
                    Fatal( Mesg, ret );
                }
                ProcCnt--;
            }
            break;
    } 

    ProcCnt = ( ProcCnt < 0 ) ? 0: ProcCnt;
    errno = 0; 

    return;
}

/* Fill spaces `size` byte into string.
*/
void FillSpace( str, size )
char *str;
int size;
{
    int i;

    size--;
    i = strlen(str);
    
    if( (size - i) > 0 ) 
        memset( str+i, 0x20, size - i );

    return;
}

/* Catch child processes exit point.
*/
int SignalChild()
{
    int pid;

    pid = wait( (int*)NULL );

    ProcCnt = (--ProcCnt < 0 ) ? 0: ProcCnt;

    signal( SIGCLD, SignalChild );

    return( pid );
}

/* Write current date-time to log file.
*/
void CheckRunTime( src )
char *src;
{
    char timestemp[13],string[BUFLEN];

    memset(string, '\0', sizeof(string));
    memset(timestemp, '\0', sizeof(timestemp));

    TimeStemp( timestemp, 12 );    

    sprintf( string, "%s %s ", src, timestemp );

    Fatal(string, ECWARN );

    return;
}

/* Initialize a daemon process.
** Detach a daemon process from login session context.
*/
void DaemonInit(ignsigcld)
int	ignsigcld;	/* nonzero -> handle SIGCLDs so zombies don't clog */
{
     register int childpid, fd;

    /*
    ** If we were started by init (process 1) from the /etc/inittab file
    ** there's no need to detach.
    ** This test is unreliable due to an unavoidable ambiguity
    ** if the process is started by some other process and orphaned
    ** (i.e., if the parent process terminates before we are started).
    */

    if (getppid() == 1) goto DaemonStart;

    /*
    ** Ignore the terminal stop signals (BSD).
    */

#ifdef SIGTTOU
    signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
    signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
    signal(SIGTSTP, SIG_IGN);
#endif
#ifdef SIGHUP
    signal(SIGHUP, SIG_IGN);    
#endif
    /*
    ** If we were not started in the background, fork and
    ** let the parent exit.  This also guarantees the first child
    ** is not a process group leader.
    */

    if ( (childpid = fork()) < 0)
        Fatal("DaemonInit()", ECCPROC );
    else if (childpid > 0)
             exit(0);    /* parent */

    /*
    ** First child process.
    **
    ** Disassociate from controlling terminal and process group.
    ** Ensure the process can't reacquire a new controlling terminal.
    */

#ifdef	SIGTSTP		/* BSD */
    if (setpgrp(0, getpid()) == -1)
        Fatal("DaemonInit(setpgrp)", ECRETURN );

     /* lose controlling tty */
    if ( (fd = open("/dev/tty", O_RDWR)) >= 0) {
        ioctl(fd, TIOCNOTTY, (char *)NULL); 
        close(fd);
    }

#else	/* System V */

    if (setpgrp() == -1)
        Fatal("DaemonInit(setpgrp)", ECRETURN );

    /* immune from pgrp leader death */
    signal(SIGHUP, SIG_IGN);    

    if ( (childpid = fork()) < 0)
        Fatal("DaemonInit(second)" ECCPROC );
    else if (childpid > 0)
             exit(0);    /* first child */

    /* second child */
#endif

DaemonStart:
    /*
    ** Close any open files descriptors.
    */

    for (fd = 0; fd < NOFILE; fd++)
        close(fd);

    errno = 0;    /* probably got set to EBADF from a close */

    /*
    ** Move the current directory to root, to make sure we
    ** aren't on a mounted filesystem.
    */

    /* chdir("/"); */

    /*
    ** Clear any inherited file mode creation mask.
    */

    umask(0);
    
    /*
    ** See if the caller isn't interested in the exit status of its
    ** children, and doesn't want to have them become zombies and
    ** clog up the system.
    ** With System V all we need do is ignore the signal.
    ** With BSD, however, we have to catch each signal
    ** and execute the wait3() system call.
    */
    return;
}

void InternalServerKill( QueueNum )
int QueueNum;
{
    MESSAGE message;

    memset( message.data, '\0', sizeof(message.data) );
    sprintf( message.data, "%05d", QueueNum );
    SendMessage( QueueNum, message.data, 1 ); 
    return;
}

static void WarningCopyright()
{
    fprintf( stderr, "\n** LEEWIN is not defined. **\n"); 
    fprintf( stderr, "** You must be define for using this library. **\n" );
    fprintf( stderr, "** (c) Coypright Lee,Kwang Woo. 1992, 1995 **\n" );
    fprintf( stderr, "** All rights reserved **\n" );
    exit(0);
}

struct stat *GetFileInfo( filename )
char *filename;
{

   static struct stat statbuf;
   FILE *stream;

   if ((stream = fopen( filename, "r")) == NULL) 
      Fatal("GetFileInfo()", ECFOPEN );

   memset( &statbuf, '\0', sizeof(statbuf) );
   if( stat(filename, &statbuf) < 0 )
       Fatal("GetFileInfo(file state query)", RETFAIL);

   fclose(stream);

   return( &statbuf );
}

sw_int IsMinMax( std, val, opt )
void *std;
void *val;
us_int opt;
{ 
    byte *scp, *vcp;
    ss_int *ssp, *vsp;
    sw_int *swp, *vwp;
    sl_int *slp, *vlp;
    float  *sfp, *vfp;
    double *sdp, *vdp;
    sw_int ret;

    if ( std == (void*)NULL || val == (void*)NULL ) 
        return(coERR);         
    ret = 0;
    switch( (opt & coTYPE) ) {
    case coCHAR:
        scp  = (byte*)std;
        vcp = (byte*)val;
        if( (opt & coMNGT) == coMNGT && *scp >= *vcp ) ret = coUNFW;
        else if( (opt & coMNGE) == coMNGE && *scp > *vcp ) ret = coUNFW;
        else if( *scp > *vcp ) ret = coUNFW;
        scp++;
        if( (opt & coMXLT) == coMXLT && *scp <= *vcp ) ret = coOVFW;
        else if( (opt & coMXLE) == coMXLE && *scp < *vcp ) ret = coOVFW;
        else if( *scp < *vcp ) ret = coOVFW;
        break;
    case coSINT:
        ssp  = (ss_int*)std;
        vsp = (ss_int*)val;
        if( (opt & coMNGT) == coMNGT && *ssp >= *vsp ) ret = coUNFW;
        else if( (opt & coMNGE) == coMNGE && *ssp > *vsp ) ret = coUNFW;
        else if( *ssp > *vsp ) ret = coUNFW;
        ssp++;
        if( (opt & coMXLT) == coMXLT && *ssp <= *vsp ) ret = coOVFW;
        else if( (opt & coMXLE) == coMXLE && *ssp < *vsp ) ret = coOVFW;
        else if( *ssp < *vsp ) ret = coOVFW;
        break;
    default:
    case coWINT:
        swp  = (sw_int*)std;
        vwp = (sw_int*)val;
        if( (opt & coMNGT) == coMNGT && *swp >= *vwp ) ret = coUNFW;
        else if( (opt & coMNGE) == coMNGE && *swp > *vwp ) ret = coUNFW;
        else if( *swp > *vwp ) ret = coUNFW;
        swp++;
        if( (opt & coMXLT) == coMXLT && *swp <= *vwp ) ret = coOVFW;
        else if( (opt & coMXLE) == coMXLE && *swp < *vwp ) ret = coOVFW;
        else if( *swp < *vwp ) ret = coOVFW;
        break;
    case coLINT:
        slp = (sl_int*)std;
        vlp = (sl_int*)val;
        if( (opt & coMNGT) == coMNGT && *slp >= *vlp ) ret = coUNFW;
        else if( (opt & coMNGE) == coMNGE && *slp > *vlp ) ret = coUNFW;
        else if( *slp > *vlp ) ret = coUNFW;
        slp++;
        if( (opt & coMXLT) == coMXLT && *slp <= *vlp ) ret = coOVFW;
        else if( (opt & coMXLE) == coMXLE && *slp < *vlp ) ret = coOVFW;
        else if( *slp < *vlp ) ret = coOVFW;
        break;
    case coWFLT:
        sfp  = (float*)std;
        vfp = (float*)val;
        if( (opt & coMNGT) == coMNGT && *sfp >= *vfp ) ret = coUNFW;
        else if( (opt & coMNGE) == coMNGE && *sfp > *vfp ) ret = coUNFW;
        else if( *sfp > *vfp ) ret = coUNFW;
        sfp++;
        if( (opt & coMXLT) == coMXLT && *sfp <= *vfp ) ret = coOVFW;
        else if( (opt & coMXLE) == coMXLE && *sfp < *vfp ) ret = coOVFW;
        else if( *sfp < *vfp ) ret = coOVFW;
        break;
    case coDFLT:
        sdp  = (double*)std;
        vdp = (double*)val;
        if( (opt & coMNGT) == coMNGT && *sdp >= *vdp ) ret = coUNFW;
        else if( (opt & coMNGE) == coMNGE && *sdp > *vdp ) ret = coUNFW;
        else if( *sdp > *vdp ) ret = coUNFW;
        sdp++;
        if( (opt & coMXLT) == coMXLT && *sdp <= *vdp ) ret = coOVFW;
        else if( (opt & coMXLE) == coMXLE && *sdp < *vdp ) ret = coOVFW;
        else if( *sdp < *vdp ) ret = coOVFW;
        break;
    }

    return(ret);
}

int WaitProcess( pid )
int pid;
{
    int ret, es;
    char Mesg[80];

    errno = 0;
    memset(Mesg, '\0', sizeof(Mesg));
    /* wait until process */ 
    while( (ret = wait(&es) ) != pid && errno != ECHILD ) ;
    es = (es >> 8) & 0xFF;
    if( es ) {
        sprintf( Mesg, "proc id: %d exit status %d", ret, es );
        Fatal( Mesg, ret );
        ret = 0;
    }

    return( ret );
}
