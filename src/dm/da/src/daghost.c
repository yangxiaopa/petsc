#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: daghost.c,v 1.16 1999/01/31 16:11:27 bsmith Exp bsmith $";
#endif
 
/*
  Code for manipulating distributed regular arrays in parallel.
*/

#include "src/dm/da/daimpl.h"    /*I   "da.h"   I*/

#undef __FUNC__  
#define __FUNC__ "DAGetGhostCorners"
/*@
   DAGetGhostCorners - Returns the global (x,y,z) indices of the lower left
   corner of the local region, including ghost points.

   Not Collective

   Input Parameter:
.  da - the distributed array

   Output Parameters:
+  x,y,z - the corner indices (where y and z are optional; these are used
           for 2D and 3D problems)
-  m,n,p - widths in the corresponding directions (where n and p are optional;
           these are used for 2D and 3D problems)

   Level: beginner

   Note:
   The corner information is independent of the number of degrees of 
   freedom per node set with the DACreateXX() routine. Thus the x, y, z, and
   m, n, p can be thought of as coordinates on a logical grid, where each
   grid point has (potentially) several degrees of freedom.
   Any of y, z, n, and p can be passed in as PETSC_NULL if not needed.

.keywords: distributed array, get, ghost, corners, nodes, local indices

.seealso: DAGetCorners(), DACreate1d(), DACreate2d(), DACreate3d()

@*/
int DAGetGhostCorners(DA da,int *x,int *y,int *z,int *m, int *n, int *p)
{
  int w;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(da,DA_COOKIE);
  /* since the xs, xe ... have all been multiplied by the number of degrees 
     of freedom per cell, w = da->w, we divide that out before returning.*/
  w = da->w;  
  if (x) *x = da->Xs/w; if (m) *m = (da->Xe - da->Xs)/w;
  if (y) *y = da->Ys;   if (n) *n = (da->Ye - da->Ys);
  if (z) *z = da->Zs;   if (p) *p = (da->Ze - da->Zs); 
  PetscFunctionReturn(0);
}

