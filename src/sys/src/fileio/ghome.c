#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: ghome.c,v 1.24 1998/12/17 21:57:08 balay Exp bsmith $";
#endif
/*
      Code for manipulating files.
*/
#include "petsc.h"
#include "sys.h"
#if defined(HAVE_PWD_H)
#include <pwd.h>
#endif
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if !defined(PARCH_win32)
#include <sys/utsname.h>
#endif
#if defined(PARCH_win32)
#include <windows.h>
#include <io.h>
#include <direct.h>
#endif
#if defined (PARCH_win32_gnu)
#include <windows.h>
#endif
#if defined(HAVE_SYS_SYSTEMINFO_H)
#include <sys/systeminfo.h>
#endif
#include "pinclude/petscfix.h"

#undef __FUNC__  
#define __FUNC__ "PetscGetHomeDirectory"
/*@C
   PetscGetHomeDirectory - Returns user's home directory name.

   Not Collective

   Input Parameter:
.  maxlen - maximum lengh allowed

   Output Parameter:
.  dir - contains the home directory. Must be long enough to hold the name.

   Level: developer

   Note:
   On Windows NT machine the enviornmental variable HOME specifies the home directory.

.keywords: system, get, real, path

@*/
int PetscGetHomeDirectory(char dir[],int maxlen)
{
#if defined(PARCH_win32) || defined(PARCH_win32_gnu)
  char *d1 = getenv("HOME");

  PetscFunctionBegin;
  if (d1 == NULL) d1 ="c:";
  PetscStrncpy(dir,d1,maxlen);
#else
  struct passwd *pw = 0;

  PetscFunctionBegin;
  pw = getpwuid( getuid() );
  if (!pw)  PetscFunctionReturn(0);
  PetscStrncpy(dir, pw->pw_dir,maxlen);
#endif
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "PetscFixFilename"
/*@C
    PetscFixFilename - Fixes a file name so that it is correct for both Unix and 
    Windows by using the correct / or \ to seperate directories.

   Not Collective

   Input Parameter:
.  filein - name of file to be fixed

   Output Parameter:
.  fileout - the fixed name. Should long enough to hold the filename.

   Level: advanced

   Notes:
   Call PetscFixFilename() just before calling fopen().
@*/
int PetscFixFilename(const char filein[],char fileout[])
{
  int i,n;

  PetscFunctionBegin;
  if (!filein || !fileout) PetscFunctionReturn(0);

  n = PetscStrlen(filein);
  for (i=0; i<n; i++ ) {
#if defined(PARCH_win32)
    if (filein[i] == '/') fileout[i] = '\\';
#else
    if (filein[i] == '\\') fileout[i] = '/';
#endif
    else fileout[i] = filein[i];
  }
  fileout[n] = 0;

  PetscFunctionReturn(0);
}
