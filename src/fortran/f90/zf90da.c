#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: zf90.c,v 1.9 1998/10/19 22:15:18 bsmith Exp $";
#endif

#include "src/fortran/f90/zf90.h"
#include "is.h"
#include "vec.h"
#include "mat.h"
#include "da.h"

#if !defined (HAVE_NOF90)

#ifdef HAVE_FORTRAN_CAPS
#define vecgetarrayf90_            VECGETARRAYF90
#define vecrestorearrayf90_        VECRESTOREARRAYF90
#define matgetarrayf90_            MATGETARRAYF90
#define matrestorearrayf90_        MATRESTOREARRAYF90
#define isgetindicesf90_           ISGETINDICESF90
#define isblockgetindicesf90_      ISBLOCKGETINDICESF90
#define isrestoreindicesf90_       ISRESTOREINDICESF90
#define isblockrestoreindicesf90_  ISBLOCKRESTOREINDICESF90
#define vecduplicatevecsf90_       VECDUPLICATEVECSF90
#define vecdestroyvecsf90_         VECDESTROYVECSF90
#define dagetglobalindicesf90_     DAGETGLOBALINDICESF90
#elif !defined(HAVE_FORTRAN_UNDERSCORE)
#define vecgetarrayf90_            vecgetarrayf90
#define vecrestorearrayf90_        vecrestorearrayf90
#define matgetarrayf90_            matgetarrayf90
#define matrestorearrayf90_        matrestorearrayf90
#define isgetindicesf90_           isgetindicesf90
#define isblockgetindicesf90_      isblockgetindicesf90
#define isrestoreindicesf90_       isrestoreindicesf90 
#define isblockrestoreindicesf90_  isblockrestoreindicesf90
#define vecduplicatevecsf90_       vecduplicatevecsf90
#define vecdestroyvecsf90_         vecdestroyvecsf90
#define dagetglobalindicesf90_     dagetglobalindicesf90
#endif

EXTERN_C_BEGIN

void vecgetarrayf90_(Vec x,array1d *ptr,int *__ierr)
{
  Scalar *fa;
  int    len;
  Vec    xin = (Vec)PetscToPointer(x);
  *__ierr = VecGetArray(xin,&fa);      if (*__ierr) return;
  *__ierr = VecGetLocalSize(xin,&len); if (*__ierr) return;
  *__ierr = PetscF90Create1dArrayScalar(fa,len,ptr);
}
void vecrestorearrayf90_(Vec x,array1d *ptr,int *__ierr)
{
  Scalar *fa;
  Vec    xin = (Vec)PetscToPointer(x);
  *__ierr = PetscF90Get1dArrayScalar(ptr,&fa);if (*__ierr) return;
  *__ierr = PetscF90Destroy1dArrayScalar(ptr);if (*__ierr) return;
  *__ierr = VecRestoreArray(xin,&fa);         if (*__ierr) return;
}

/* --------------------------------------------------------------- */

void matgetarrayf90_(Vec x,array2d *ptr,int *__ierr)
{
  Scalar *fa;
  int    m,n;
  Mat    xin = (Mat)PetscToPointer(x);
  *__ierr = MatGetArray(xin,&fa);      if (*__ierr) return;
  *__ierr = MatGetLocalSize(xin,&m,&n); if (*__ierr) return;
  *__ierr = PetscF90Create2dArrayScalar(fa,m,n,ptr);
}
void matrestorearrayf90_(Vec x,array2d *ptr,int *__ierr)
{
  Scalar *fa;
  Mat    xin = (Mat)PetscToPointer(x);
  *__ierr = PetscF90Get2dArrayScalar(ptr,&fa);if (*__ierr) return;
  *__ierr = PetscF90Destroy2dArrayScalar(ptr);if (*__ierr) return;
  *__ierr = MatRestoreArray(xin,&fa);         if (*__ierr) return;
}
/* --------------------------------------------------------------- */

void isgetindicesf90_(IS x,array1d *ptr,int *__ierr)
{
  int    *fa;
  int    len;
  IS    xin = (IS)PetscToPointer(x);
  *__ierr = ISGetIndices(xin,&fa);      if (*__ierr) return;
  *__ierr = ISGetSize(xin,&len); if (*__ierr) return;
  *__ierr = PetscF90Create1dArrayInt(fa,len,ptr);
}
void isrestoreindicesf90_(IS x,array1d *ptr,int *__ierr)
{
  int    *fa;
  IS     xin = (IS)PetscToPointer(x);
  *__ierr = PetscF90Get1dArrayInt(ptr,&fa);if (*__ierr) return;
  *__ierr = PetscF90Destroy1dArrayInt(ptr);if (*__ierr) return;
  *__ierr = ISRestoreIndices(xin,&fa);         if (*__ierr) return;
}


void isblockgetindicesf90_(IS x,array1d *ptr,int *__ierr)
{
  int    *fa;
  int    len;
  IS    xin = (IS)PetscToPointer(x);
  *__ierr = ISBlockGetIndices(xin,&fa);      if (*__ierr) return;
  *__ierr = ISBlockGetSize(xin,&len); if (*__ierr) return;
  *__ierr = PetscF90Create1dArrayInt(fa,len,ptr);
}
void isblockrestoreindicesf90_(IS x,array1d *ptr,int *__ierr)
{
  int    *fa;
  IS     xin = (IS)PetscToPointer(x);
  *__ierr = PetscF90Get1dArrayInt(ptr,&fa);if (*__ierr) return;
  *__ierr = PetscF90Destroy1dArrayInt(ptr);if (*__ierr) return;
  *__ierr = ISBlockRestoreIndices(xin,&fa);         if (*__ierr) return;
}

/* ---------------------------------------------------------------*/

void dagetglobalindicesf90_(DA da,int *n, array1d *indices, int *__ierr )
{
  int *idx;
  *__ierr = DAGetGlobalIndices((DA)PetscToPointer(da),n,&idx);
  if (*__ierr) return;
  *__ierr = PetscF90Create1dArrayInt(idx,*n,indices);
}

/* ---------------------------------------------------------------*/

void vecduplicatevecsf90_(Vec v,int *m,array1d *ptr, int *__ierr )
{
  Vec *lV;
  PetscFortranAddr *newvint;
  int i;
  *__ierr = VecDuplicateVecs((Vec)PetscToPointer(v),*m,&lV);
  if (*__ierr) return;
  newvint = (PetscFortranAddr *) PetscMalloc((*m)*sizeof(PetscFortranAddr)); 
  if (!newvint) {*__ierr = PETSC_ERR_MEM; return;}
  for (i=0; i<*m; i++) {
    newvint[i] = PetscFromPointer(lV[i]);
  }
  PetscFree(lV); 
  *__ierr = PetscF90Create1dArrayPetscFortranAddr(newvint,*m,ptr);
}

void vecdestroyvecsf90_(array1d *ptr,int *m,int *__ierr )
{
  PetscFortranAddr *vecs;
  int       i;

  *__ierr = PetscF90Get1dArrayPetscFortranAddr(ptr,&vecs);if (*__ierr) return;
  for (i=0; i<*m; i++) {
    *__ierr = VecDestroy((Vec)PetscToPointer(&vecs[i]));
    if (*__ierr) return;
    PetscRmPointer(vecs[i]); 
  }
  *__ierr = PetscF90Destroy1dArrayPetscFortranAddr(ptr);if (*__ierr) return;
  PetscFree(vecs);
}

EXTERN_C_END

#else  /* !defined (HAVE_NOF90) */


/*
     Dummy function so that compilers won't complain about 
  empty files.
*/
int F90_ZF90_Dummy(int dummy)
{
  return 0;
}
 

#endif



