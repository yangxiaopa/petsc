#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: signal.c,v 1.56 1998/12/17 21:57:22 balay Exp bsmith $";
#endif
/*
      Routines to handle signals the program will receive. 
    Usually this will call the error handlers.
*/
#include <signal.h>
#include "petsc.h"             /*I   "petsc.h"   I*/
#include "sys.h"
#include "pinclude/petscfix.h"     

struct SH {
  int    cookie;
  int    (*handler)(int,void *);
  void   *ctx;
  struct SH* previous;
};
static struct SH* sh        = 0;
static int        SignalSet = 0;

static char *SIGNAME[] = { 
    "Unknown signal", 
    "HUP",
    "INT",
    "QUIT",
    "ILL",
    "TRAP",
    "ABRT",
    "EMT",  
    "FPE:\nPETSC ERROR: Floating Point Exception, probably divide by zero",
    "KILL", 
    "BUS: \nPETSC ERROR: Bus Error, possibly illegal memory access",  
    "SEGV:\nPETSC ERROR: Segmentation Violation, probably memory access out of range", 
    "SYS",
    "PIPE",
    "ALRM",
    "TERM", 
    "URG",
    "STOP",
    "TSTP",
    "CONT", 
    "CHLD" }; 

#undef __FUNC__  
#define __FUNC__ "PetscSignalHandler"
/*
    PetscSignalHandler - This is the signal handler called by the system. This calls 
             any signal handler set by PETSc or the application code.
 
   Input Parameters: (depends on system)
.    sig - integer code indicating the type of signal
.    code - ??
.    sigcontext - ??
.    addr - ??

*/
#if defined(PARCH_IRIX)  || defined(PARCH_IRIX64) || defined(PARCH_IRIX5)|| defined(PARCH_sun4)
static void PetscSignalHandler( int sig, int code,struct sigcontext * scp,char *addr)
#else
static void PetscSignalHandler( int sig )
#endif
{
  int ierr;

  PetscFunctionBegin;
  if (!sh || !sh->handler) {
    ierr = PetscDefaultSignalHandler(sig,(void*)0);
  } else{
    ierr = (*sh->handler)(sig,sh->ctx);
  }
  if (ierr) MPI_Abort(PETSC_COMM_WORLD,0);
}


#undef __FUNC__  
#define __FUNC__ "PetscDefaultSignalHandler"
/*@
   PetscDefaultSignalHandler - Default signal handler.

   Not Collective

   Level: advanced

   Input Parameters:
+  sig - signal value
-  ptr - unused pointer

.keywords: default, signal, handler
@*/
int PetscDefaultSignalHandler( int sig, void *ptr)
{
  int         ierr;
  static char buf[128];

  PetscFunctionBegin;
  signal( sig, SIG_DFL );
  if (sig >= 0 && sig <= 20) {
    sprintf( buf, "Caught signal %s\n", SIGNAME[sig] );
  } else {
    PetscStrcpy( buf, "Caught signal\n" );
  }
  PetscStrcat(buf,"PETSC ERROR: Try option -start_in_debugger or ");
  PetscStrcat(buf,"-on_error_attach_debugger ");
  PetscStrcat(buf,"to\nPETSC ERROR: determine where problem occurs\n");
#if defined(USE_PETSC_STACK)
  if (!PetscStackActive) {
    PetscStrcat(buf,"PETSC ERROR: or try option -log_stack\n");
  } else {
    PetscStackPop;  /* remove stack frames for error handlers */
    PetscStackPop;
    PetscStrcat(buf,"PETSC ERROR: likely location of problem given above in stack\n");
    (*PetscErrorPrintf)("--------------- Stack Frames ---------------\n");
    PetscStackView(VIEWER_STDERR_WORLD);
    (*PetscErrorPrintf)("--------------------------------------------\n");
  }
#endif
  ierr =  PetscError(0,"unknownfunction","unknown file"," ",PETSC_ERR_SIG,0,buf);
  MPI_Abort(PETSC_COMM_WORLD,ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "PetscPushSignalHandler"
/*@C
   PetscPushSignalHandler - Catches the usual fatal errors and 
   calls a user-provided routine.

   Not Collective

    Input Parameter:
+  routine - routine to call when a signal is received
-  ctx - optional context needed by the routine

  Level: developer

.keywords: push, signal, handler
@*/
int PetscPushSignalHandler(int (*routine)(int, void*),void* ctx )
{
  struct  SH *newsh;

  PetscFunctionBegin;
  if (!SignalSet && routine) {
#if defined(PARCH_IRIX5)  && defined(__cplusplus)
    signal( SIGQUIT, (void (*)(...)) PetscSignalHandler );
    signal( SIGILL,  (void (*)(...)) PetscSignalHandler );
    signal( SIGFPE,  (void (*)(...)) PetscSignalHandler );
    signal( SIGSEGV, (void (*)(...)) PetscSignalHandler );
    signal( SIGSYS,  (void (*)(...)) PetscSignalHandler );
#elif (defined(PARCH_IRIX64) || defined(PARCH_IRIX)) && defined(__cplusplus)
    signal( SIGQUIT, (void (*)(int)) PetscSignalHandler );
    signal( SIGILL,  (void (*)(int)) PetscSignalHandler );
    signal( SIGFPE,  (void (*)(int)) PetscSignalHandler );
    signal( SIGSEGV, (void (*)(int)) PetscSignalHandler );
    signal( SIGSYS,  (void (*)(int)) PetscSignalHandler );
#elif defined(PARCH_win32)
    /*
    signal( SIGILL,  PetscSignalHandler );
    signal( SIGFPE,  PetscSignalHandler );
    signal( SIGSEGV, PetscSignalHandler );
    */
#elif defined(PARCH_win32_gnu) || defined (PARCH_linux) 
    signal( SIGILL,  PetscSignalHandler );
    signal( SIGFPE,  PetscSignalHandler );
    signal( SIGSEGV, PetscSignalHandler );
    signal( SIGBUS,  PetscSignalHandler );
    signal( SIGQUIT, PetscSignalHandler );
#else
    signal( SIGILL,  PetscSignalHandler );
    signal( SIGFPE,  PetscSignalHandler );
    signal( SIGSEGV, PetscSignalHandler );
    signal( SIGBUS,  PetscSignalHandler );
    signal( SIGQUIT, PetscSignalHandler );
    signal( SIGSYS,  PetscSignalHandler );
#endif
    SignalSet = 1;
  }
  if (!routine) {
#if (defined(PARCH_IRIX)  || defined(PARCH_IRIX5) || defined(PARCH_IRIX64)) && defined(__cplusplus)
    signal( SIGILL,  0 );
    signal( SIGFPE,  0 );
    signal( SIGQUIT, 0 );
    signal( SIGSEGV, 0 );
    signal( SIGSYS,  0 );
#elif defined(PARCH_win32)
    signal( SIGILL,  0 );
    signal( SIGFPE,  0 );
    signal( SIGSEGV, 0 );
#elif defined(PARCH_win32_gnu) || defined (PARCH_linux) 
    signal( SIGILL,  0 );
    signal( SIGFPE,  0 );
    signal( SIGQUIT, 0 );
    signal( SIGSEGV, 0 );
    signal( SIGBUS,  0 );
#else
    signal( SIGILL,  0 );
    signal( SIGFPE,  0 );
    signal( SIGQUIT, 0 );
    signal( SIGSEGV, 0 );
    signal( SIGBUS,  0 );
    signal( SIGSYS,  0 );
#endif
    SignalSet = 0;
  }
  newsh = (struct SH*) PetscMalloc(sizeof(struct SH)); CHKPTRQ(newsh);
  if (sh) {newsh->previous = sh;} 
  else {newsh->previous = 0;}
  newsh->handler = routine;
  newsh->ctx     = ctx;
  sh             = newsh;
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "PetscPopSignalHandler"
int PetscPopSignalHandler(void)
{
  struct SH *tmp;

  PetscFunctionBegin;
  if (!sh) PetscFunctionReturn(0);
  tmp = sh;
  sh  = sh->previous;
  PetscFree(tmp);
  if (!sh || !sh->handler) {
#if (defined(PARCH_IRIX)  || defined(PARCH_IRIX5) || defined(PARCH_IRIX64)) && defined(__cplusplus)
    signal( SIGILL,  0 );
    signal( SIGFPE,  0 );
    signal( SIGQUIT, 0 );
    signal( SIGSEGV, 0 );
    signal( SIGSYS,  0 );
#elif defined(PARCH_win32)
    signal( SIGILL,  0 );
    signal( SIGFPE,  0 );
    signal( SIGSEGV, 0 );
#elif defined(PARCH_win32_gnu) || defined (PARCH_linux) 
    signal( SIGILL,  0 );
    signal( SIGFPE,  0 );
    signal( SIGQUIT, 0 );
    signal( SIGSEGV, 0 );
    signal( SIGBUS,  0 );
#else
    signal( SIGILL,  0 );
    signal( SIGFPE,  0 );
    signal( SIGQUIT, 0 );
    signal( SIGSEGV, 0 );
    signal( SIGBUS,  0 );
    signal( SIGSYS,  0 );
#endif
    SignalSet = 0;
  } else {
    SignalSet = 1;
  }
  PetscFunctionReturn(0);
}




