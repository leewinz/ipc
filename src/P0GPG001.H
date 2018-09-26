/* 
** unix system utility library
** ================================================================
**  Copyright 1993,1996 Lee, Kwang Woo(leewin) All Rights Reserved.
**  E-mail : leewin@hitel.kol.nm.kr
** ================================================================
*/

#ifndef _P0GPG001_H  
#define _P0GPG001_H

#pragma ident "%W% %E% leewin"

#ifdef _cplusplus
extern "C" {
#endif

#ifndef LEEWIN
#define LEEWIN "/USER02/P0SDEV02/bin"
#endif

#ifndef INTGERTYPE 
#define INTGERTYPE
typedef unsigned char  ubyte;
typedef          char  byte;
typedef unsigned short us_int;
typedef          short ss_int;
typedef unsigned int   uw_int;
typedef          int   sw_int;
typedef unsigned long  ul_int;
typedef          long  sl_int;
#endif

/* search option
*/
#ifndef SEARCHOPTION
#define soITM 0x1000   /* search object in a group */
#define soGRP 0x2000   /* search object group */
#define soCMI 0x0100   /* compare a object */
#define soCMG 0x0200   /* compare group */
#define soPRG 0x0010   /* search pre-order group */
#define soING 0x0020   /* search in-order group */
#define soPOG 0x0040   /* search post-order group */
#define soPRI 0x0001   /* search pre-order(forward) item */
#define soINI 0x0002   /* search in-order item */
#define soPOI 0x0004   /* search post-order(backward) item */
#endif

#ifndef COMPAREOPTION
#define coCHAR 0x0001  /* compare char values */
#define coSINT 0x0002  /* compare small int values */
#define coWINT 0x0004  /* compare int values(default) */
#define coLINT 0x0008  /* compare long int values */
#define coWFLT 0x0010  /* compare float values */
#define coDFLT 0x0020  /* compare double float values */
#define coLFLT 0x0020  /* compare long double values */
#define coMNGT 0x0100  /* greater than to min */
#define coMNGE 0x0200  /* greater than or equal to min(default) */
#define coMXLT 0x0400  /* less than to max */ 
#define coMXLE 0x0800  /* less than or equal to max(default) */
#define coTYPE 0x00FF
#define coERR  -0x0002 /* return value if error */
#define coUNFW -0x0001 /* return value if greater than to maximum */ 
#define coOVFW 0x0001  /* return value if greater than to maximum */ 
#endif

#define  RDBMSMESSAGE 7000
#define  QPERM        0600            /* permissions for queue */
#define  BUFLEN      (us_int)1024     /* buffer size for queue */
#define  PROCLIMIT    5               /* process limit value   */
#define  SYSERR      -1               /* system call error value   */
#define  CHILD       0                /* child process  */
#define  STARTDATE   1994

typedef enum {
        False,
        True
       } BOOLEAN;

/* struct of message in IPC  */ 
typedef struct{
        long type; 
        char data[BUFLEN];
      } MESSAGE;

/* macro define */
#define  SPR           sprintf
#define  MC(a,b,c)     memcpy(a,b, c)
#define  MS(a)         memset(a,'\0', sizeof(a))
#define  SC(a,b)       strcat(a,b)
#define  IA(a,b)       SPR( (MS(a,0),a),"%-d",b)
#define  AI(a,b)       a=atoi(b)

#define  GetFltn(a,b,c,d,e,k) ( MS(b),MC(b,d,e),gcvt( atof(b)/c, k, a ),\
                                a[sizeof(a)-1]='\0' ) 
#define  GetFlt(a,b,c,d,e) ( MS(b),MC(b,d,e),SPR(a,"%-g",atof(b)/c ),\
                                a[sizeof(a)-1]='\0' ) 
#define  GetInt(a,b,c,h)   ( MS(b),MC(b,c,h),SPR(a,"%-d",atoi(b)),\
                                a[sizeof(a)-1]='\0' ) 
#define  Str2Flt(a,b,c,d,e) ( MS(b),MC(b,d,e),a=atof(b)/c )
#define  Str2Int(a,b,c,h)   ( MS(b),MC(b,c,h),a=atoi(b) )
#define  RemoveErrorList(a) ( RemoveList(a) )

#define  min(a,b)   ( ( a > b ) ? b:a )
#define  max(a,b)   ( ( a > b ) ? a:b )

#ifdef _P0GPG001_C
short int Month[] = { 31, 28, 31, 30, 31, 30,
                      31, 31, 30, 31, 30, 31, };
int  ProcCnt;
int  JepumQueue = -1;                  /* message queue identifier */
BOOLEAN MsgSwitch = True;
char CurDataBase[16];
char UserId[16];
char Password[16];
char AppName[16];
char ServerName[16];
char ErrorList[256];
char PathList[256];

void Fatal(/* char*, int */);          /* fatal error jump routine */       
int  StringToDay(/* char* */);         /* convert string to integer day */
void DayToString(/* int, */);          /* convert integer day to string */
int  DayToTerm(/* uw_int */);          /* convert due-date to term */ 
void StrToUpr(/* char* */);            /* string to upper case  */
void TimeStemp(/* char*,int */);       /* current day string access  */
int  InitQueue(/* key_t */);           /* message queue initation */
void SendMessage(/* key_t, char*, long */); /* sending data to message queue*/
void MsgRemove(/* key_t */);           /* delete message queue */
void FillSpace(/* char*, int */);      /* fill space int length */
void CheckProcess(/* int, int */);     /* child process management */
void CheckRunTime(/* char* */);        /* current time write */
void DaemonInit(/* int */);            /* daemon type process initialize */ 
void RemoveList(/* char* */);          /* running information file delete */
int  SignalChild();                    /* catch SIGCLD signal */
void InternalServerKill(/* int */);    /* internal server process kill */
/* parameter: 
** char *user-id, char *password,
** char *application-name, char *server-name, 
*/
void ConnectLib();                     /* connect for this library */
void LibraryInterface();               /* for using this library */
struct stat *GetFileInfo(/* char* */); /* query file information */
static void WarningCopyright();
sw_int IsMinMax(/* void*, void*, us_int */);
int  WaitProcess(/* int */);

#else 

extern BOOLEAN MsgSwitch;
extern short int Month[12];
extern int   ProcCnt;
extern int   JepumQueue; 
extern char  CurDataBase[16];
extern char  UserId[16];
extern char  Password[16];
extern char  AppName[16];
extern char  ServerName[16];
extern char  ErrorList[256];
extern char  PathList[256];

extern void Fatal();  
extern int  StringToDay();
extern void DayToString();
extern int  DayToTerm();
extern void StrToUpr();
extern void TimeStemp();
extern int  InitQueue();
extern void SendMessage();
extern void MsgRemove();
extern void FillSpace();
extern void CheckProcess();
extern int  SignalChild();
extern void ConnectLib();
extern void LibraryInterface();
extern void CheckRunTime();
extern void DaemonInit();
extern void RemoveList();
extern void InternalServerKill();
extern struct stat *GetFileInfo();
extern sw_int IsMinMax();
extern int  WaitProcess();
#endif

extern void error();
extern int  errno;
extern char *sys_errlist[];
extern int  sys_nerr;

#ifdef _cplusplus
}
#endif

#endif           /* _P0GPG001_H */
