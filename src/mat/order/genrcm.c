/* genrcm.f -- translated by f2c (version 19931217).*/

#include "petsc.h"

/*****************************************************************/
/*****************************************************************/
/*********   GENRCM ..... GENERAL REVERSE CUTHILL MCKEE   ********/
/*****************************************************************/

/*    PURPOSE - GENRCM FINDS THE REVERSE CUTHILL-MCKEE*/
/*       ORDERING FOR A GENERAL GRAPH. FOR EACH CONNECTED*/
/*       COMPONENT IN THE GRAPH, GENRCM OBTAINS THE ORDERING*/
/*       BY CALLING THE SUBROUTINE RCM.*/

/*    INPUT PARAMETERS -*/
/*       NEQNS - NUMBER OF EQUATIONS*/
/*       (XADJ, ADJNCY) - ARRAY PAIR CONTAINING THE ADJACENCY*/
/*              STRUCTURE OF THE GRAPH OF THE MATRIX.*/

/*    OUTPUT PARAMETER -*/
/*       PERM - VECTOR THAT CONTAINS THE RCM ORDERING.*/

/*    WORKING PARAMETERS -*/
/*       MASK - IS USED TO MARK VARIABLES THAT HAVE BEEN*/
/*              NUMBERED DURING THE ORDERING PROCESS. IT IS*/
/*              INITIALIZED TO 1, AND SET TO ZERO AS EACH NODE*/
/*              IS NUMBERED.*/
/*       XLS - THE INDEX VECTOR FOR A LEVEL STRUCTURE.  THE*/
/*              LEVEL STRUCTURE IS STORED IN THE CURRENTLY*/
/*              UNUSED SPACES IN THE PERMUTATION VECTOR PERM.*/

/*    PROGRAM SUBROUTINES -*/
/*       FNROOT, RCM.*/
/*****************************************************************/
#undef __FUNC__  
#define __FUNC__ "SPARSEPACKgenrcm" 
int SPARSEPACKgenrcm(int *neqns, int *xadj, int *adjncy, 
	int *perm, int *mask, int *xls)
{
    /* System generated locals */
    int i__1;

    /* Local variables */
    static int nlvl, root, i, ccsize;
    extern int SPARSEPACKfnroot(int *, int *, int *, 
	    int *, int *, int *, int *), SPARSEPACKrcm(int *, 
	    int *, int *, int *, int *, int *, int *);
    static int num;

    PetscFunctionBegin;
    /* Parameter adjustments */
    --xls;
    --mask;
    --perm;
    --adjncy;
    --xadj;

    i__1 = *neqns;
    for (i = 1; i <= i__1; ++i) {
	mask[i] = 1;
    }
    num = 1;
    i__1 = *neqns;
    for (i = 1; i <= i__1; ++i) {
/*          FOR EACH MASKED CONNECTED COMPONENT ...*/
	if (mask[i] == 0) {
	    goto L200;
	}
	root = i;
/*             FIRST FIND A PSEUDO-PERIPHERAL NODE ROOT.*/
/*             NOTE THAT THE LEVEL STRUCTURE FOUND BY*/
/*             FNROOT IS STORED STARTING AT PERM(NUM).*/
/*             THEN RCM IS CALLED TO ORDER THE COMPONENT*/
/*             USING ROOT AS THE STARTING NODE.*/
	SPARSEPACKfnroot(&root, &xadj[1], &adjncy[1], &mask[1], &nlvl, &xls[1], &perm[
		num]);
	SPARSEPACKrcm(&root, &xadj[1], &adjncy[1], &mask[1], &perm[num], &ccsize, &xls[
		1]);
	num += ccsize;
	if (num > *neqns) {
	    PetscFunctionReturn(0);
	}
L200:
	;
    }
    PetscFunctionReturn(0);
}

