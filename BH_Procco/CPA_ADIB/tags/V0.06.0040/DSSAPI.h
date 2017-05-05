/*
*%-----------------------------------------------------------------------------
*% File........... : DSSAPI.H
*% Project........ : SIGMA PORTABLE
*% Created ....... : 24.06.1996
*% Author ........ : Eric YANBEKIAN
*%                 : R&D T.T.S.
*%                 : SCHLUMBERGER Technologies
*%                 : 50, Avenue Jean Jaures, 92542 Montrouge Cedex, FRANCE
*% Version ....... : 1.00
*% Module ........ : Data Storage Services
*% Sub-module .... : Management Data
*% Description ... : DSS API functions
*% Copyright ..... : (c) Copyright SCHLUMBERGER Technology Corporation
*%                 : unpublished work.
*%                 : This computer program includes Confidential, Proprietary
*%                 : Information and is a Trade Secret of SCHLUMBERGER
*%                 : Technology Copr. All use, disclosure, and/or reproduction
*%                 : is prohibited unless authorised in writing.
*%                 : All rights reserved.
*%-----------------------------------------------------------------------------
*% $Revision: 1.2 $
*% $Archive:   Z:/Dev/Core/MCS/AGP2/AM/CORE/Include/Dssapi.h_v  $
*% $Log: DSSAPI.h,v $
*% Revision 1.2  2011/04/28 01:54:48  hejj
*% *** empty log message ***
*%
*% Revision 1.1  2010/12/07 07:33:31  hejj
*% 2010-12-7 15:32 by Gary
*%
* 
*    Rev 1.1   Mar 27 2001 09:46:48   doguy
*  
* 
*    Rev 1.0   01 Mar 2001 14:57:28   kersaho
* First Release
*
*    Rev 1.0   Apr 06 1999 20:00:04   BRCAROTHERS
* Checkin of v2.16 files for new LAN configuration.
*
*    Rev 1.2   12 May 1998 10:25:06   dave
* Added ability to open corrupt file
*
*    Rev 1.1   23 Apr 1998 15:12:24   dave
* Updated header
*%-----------------------------------------------------------------------------
*/
#ifndef _DSSAPI_H
#define _DSSAPI_H

#include "typedef.h"
#include "filehdr.h"

/* $HeaderFileStart --------------------------------------------------------*/

/* Record definition layout */
typedef struct
{
    OS_WORD KeyOffset;              /* key position within each record */
    OS_WORD KeyLen;                 /* key length */
    OS_WORD RecLen;                 /* record length */
} RECORD_DEF;
typedef RECORD_DEF far *RECORD_DEFP;  /* far pointer to record definiton */

/* file creating types */
#define VARIABLE_LEN 0x01       /* variable record length (fixed, by default) */
#define INI_FILE     0x02       /* initialization file */

/* open mode, file-sharing mode */
//#define OS_O_APPEND   0x01         /* set the file pointer to EOF prior to each
/* write */
// #define OS_O_CREATE   0x02         /* create the file if it does not exist */
// #define O_VARLEN   0x04         /* variable record length (only with O_CREAT) */
// #define O_INI      0x08         /* INI file (only with O_CREAT) * NOT USED * */
// #define O_SHARED_A 0x10         /* file shared by all the applications */
// #define O_SHARED_M 0x20         /* file shared by all the merchants */
// #define O_FORCE    0x80         /* force open of corrupt file */

/* seek starting point options */
// conflict with paypro

/* mnemonics for error codes */
#define NO_MEMORY   -1          /* not enough memory */
#define NO_ENTRY    -2          /* file not found (no entry in the DAT) */
#define BAD_HANDLE  -3          /* bad file handle */
#define BAD_PARAM   -4          /* invalid parameter */
#define DENIED      -5          /* access denied */
#define NO_KEY      -6          /* key not found */
#define UNMOVABLE   -7          /* defragmentation error: blocks are
* unmovable */
#define STACK_LIMIT -8          /* stacking limit encountered (recursive
* calls) */
#define CORRUPT     -9          /* corrupted file (returned by Open()) */

/* $HeaderFileEnd ----------------------------------------------------------*/

/* $GlobalDataStart ----------------------------------------------------------*/
/* $GlobalDataEnd ------------------------------------------------------------*/

/* $GlobalFnStart ---- Function prototypes ---------------------------------*/


#endif
/*$--- End Of File : dssapi.h ---*/
