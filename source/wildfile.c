/*

Name:
WILDFILE.C

Description:
Some routines to support wildcard filename handling.. Done by MikMak

    1-3-95  : Adapted so it compiles for both borland & watcom
    30-11-95: OS2 code by Tom Stokes

Portability:

MSDOS:  BC(y)   Watcom(y)   DJGPP(y)
Win95:  BC(y)
Os2:    y
Linux:  n

(y) - yes
(n) - no (not possible or not useful)
(?) - may be possible, but not tested

*/
#ifdef __OS2__
#define INCL_DOS
#include <os2.h>
#include <os2me.h>
#include <mmio.h>
#else
#include <dos.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <process.h>
#include <errno.h>
#include "wildfile.h"

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#endif

#ifndef _MAX_DIR
#define _MAX_DIR 256
#endif

#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif

#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif

static char path[_MAX_PATH];
static char drive[_MAX_DRIVE];
static char dir[_MAX_DIR];
static char fname[_MAX_FNAME];      /* <- prevent TASM clash */
static char ext[_MAX_EXT];

#ifdef __OS2__
static HDIR         hdir = HDIR_SYSTEM;    /* os2 directory handle */
static FILEFINDBUF  ffblk;
#else
static struct find_t ffblk;
#endif

static char **newargv;
static int count;


static char *GetFirstName(char *wildname,int attrib)
/*
    Finds the first file in a directory that corresponds to the wildcard
    name 'wildname'.

    returns:        ptr to full pathname

                or

                NULL if file couldn't be found
*/
{
#ifdef __OS2__
        count = 1;
        if (!DosFindFirst(wildname, &hdir, FILE_NORMAL, &ffblk,
                        sizeof(ffblk), (PULONG)(&count), FIL_STANDARD)) {
                _splitpath(ffblk.achName,NULL,NULL,fname,ext);
                _makepath(path,drive,dir,fname,ext);
                return path;
        }
        return NULL;
#else
    _splitpath(wildname,drive,dir,fname,ext);
    if(!_dos_findfirst(wildname,attrib,&ffblk)){
        _splitpath(ffblk.name,NULL,NULL,fname,ext);
        _makepath(path,drive,dir,fname,ext);
        return path;
    }
    return NULL;
#endif
}



static char *GetNextName(void)
/*
    Finds another file in a directory that corresponds to the wildcard
    name of the GetFirstName call.

    returns:        ptr to full pathname

                or

                NULL if file couldn't be found
*/
{
#ifdef __OS2__
        count = 1;
        if(!DosFindNext(hdir, &ffblk, sizeof(ffblk), (PULONG)(&count))) {
                _splitpath(ffblk.achName,NULL,NULL,fname,ext);
                _makepath(path,drive,dir,ffblk.achName,NULL);
                return path;
    }
    return NULL;
#else
    if(!_dos_findnext(&ffblk)){
        _splitpath(ffblk.name,NULL,NULL,fname,ext);
        _makepath(path,drive,dir,ffblk.name,NULL);
        return path;
    }
    return NULL;
#endif
}



static void TackOn(char *s)
{
    newargv=(char **)realloc(newargv,(count+2)*sizeof(char *));

    if(newargv==NULL){
        perror("Glob");
        exit(-1);
    }

    newargv[count++]=strdup(s);
    newargv[count]=NULL;
}



static void Expand(char *wildname,int attrib)
{
    char *s;

    s=(strpbrk(wildname,"*?")==NULL) ? NULL : GetFirstName(wildname,attrib);

    if(s==NULL){

        /* wildname is not a pattern, or there's no match for
           this pattern -> add wildname to the list */

        TackOn(wildname);
    }
    else do{

        /* add all matches to the list */
        TackOn(s);

    } while((s=GetNextName()) != NULL);
}



static int fcmp(const void *a,const void *b)
{
    return(strcmp(*(char **)a,*(char **)b));
}



void MyGlob(int *argc,char **argv[],int attrib)
{
    int i;
    int *idxarr;

    newargv=NULL;
    count=1;

    idxarr=(int *)calloc(*argc+1,sizeof(int));
        newargv=(char **)calloc(2,sizeof(char *));

    if(newargv==NULL || idxarr==NULL){
        errno=ENOMEM;
        perror("Glob");
        exit(-1);
    }

    /* init newargv[0] */

    newargv[0]=(*argv)[0];

    /* Try to expand all arguments except argv[0] */

    for(i=1;i<*argc;i++){

        /* remember position old arg -> new arg */

        idxarr[i]=count;

        /* expand the wildcard argument */

        Expand((*argv)[i],attrib);
    }

    idxarr[i]=count;

    for(i=1;i<*argc;i++){
        qsort(&newargv[idxarr[i]],
              idxarr[i+1]-idxarr[i],
              sizeof(char *),fcmp);
    }

    /* replace the old argc and argv values by the new ones */

    *argc=count;
    *argv=newargv;

    free(idxarr);
}
