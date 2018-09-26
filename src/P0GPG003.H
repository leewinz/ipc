/* 
**  Define constant error variable.
** ================================================================
**  Copyright 1993,1996 Lee, Kwang Woo(leewin) All Rights Reserved.
**  E-mail : leewin@hitel.kol.nm.kr
** ================================================================
*/

#ifndef _P0GPG003_H
#define _P0GPG003_H

#pragma ident "%W% %E% leewin"

#define RETSUCC  (int) 0    /* return success */
#define RETFAIL  (int)-1    /* return failture */

#define ECWARN   (int) 0    /* warnning  code or general message */
#define ECPARAM  (int)-1    /* parameter error */
#define ECRETURN (int)-1    /* error status return code */
#define ECFOPEN  (int)-2    /* file open error */
#define ECALLOC  (int)-3    /* heap memory allocate error */
#define ECCPROC  (int)-4    /* create process error */ 
#define EMSGSND  (int)-5    /* message send to message queue error */ 
#define EMSGRCV  (int)-6    /* message receive from message queue error */
#define EMSGGET  (int)-7    /* message queue create error */
#define EMSGDEL  (int)-8    /* message queue delete error */
#define ECSTDNF  (int)-9    /* standards not found error */ 
#define ECEXECG  (int)-10   /* execute group error( execv,execl ) */ 

#define ECDBINI  (int)-20   /* RDBMS initialize error */
#define ECDBOPN  (int)-21   /* RDBMS data base open error */ 
#define ECDBQUY  (int)-22   /* RDBMS query error */
#define ECDBRET  (int)-23   /* RDBMS record retrieve error */
#define ECDBDUP  (int)-24   /* RDBMS record duplicate  error */
#define ECNETWK  (int)-25   /* network error or dbprocess dead */
#define ECDBEXE  (int)-26   /* RDBMS manipulate error ex) update */ 
#define ECDBLGN  (int)-27   /* RDBMS dblogin() error  */

#define EVERIFY  (int)-999  /* heap memory area verify test error */
 
#endif
