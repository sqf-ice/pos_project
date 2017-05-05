/*
*%-----------------------------------------------------------------------------
*% File........... : FILEHDR.H
*% Project........ : MAGIC AGP2
*% Module ........ : Data Storage Services
*% Sub-module .... : File Header
*% Description ... : File header structure
*% Copyright ..... : (c) Copyright SCHLUMBERGER Technology Corporation
*%                 : unpublished work.
*%                 : This computer program includes Confidential, Proprietary
*%                 : Information and is a Trade Secret of SCHLUMBERGER
*%                 : Technology Copr. All use, disclosure, and/or reproduction
*%                 : is prohibited unless authorised in writing.
*%                 : All rights reserved.
*%-----------------------------------------------------------------------------
*% $Revision: 1.2 $
*% $Archive:   Z:/Dev/Core/MCS/AGP2/AM/CORE/Include/FileHdr.h_v  $
*% $Log: FILEHDR.h,v $
*% Revision 1.2  2011/02/22 06:17:30  hejj
*% *** empty log message ***
*%
*% Revision 1.1  2010/12/07 07:33:31  hejj
*% 2010-12-7 15:32 by Gary
*%
* 
*    Rev 1.0   01 Mar 2001 14:57:50   kersaho
* First Release
*
*    Rev 1.0   04 Dec 2000 16:40:38   meyer
* First Release
*%-----------------------------------------------------------------------------
*/

#ifndef _FILEHEADER_H
#define _FILEHEADER_H

#include "typedef.h"
//#include "os_mbm.h"

/* Data Storage Area, Header */
#define FILE_NAME_SIZE BLOCK_NAME_SIZE

typedef struct                  /* File Header structure */
{
    BYTE ApplHandle;             /* Application handle */
    BYTE MercHandle;             /* Merchant handle */
    BYTE FileType;               /* Type of file (see OS_FILE.H) */
    BYTE OpenMode;               /* Mode that the file was opened in */
#define  APPEND     1           /* File pointer set to EOF before each writing */
#define  OVERWRITE  2           /* Overwrite mode. The initial pointer is 0 */
    /* Default is OVERWRITE */
    
    DWORD CurOffset;             /* Current file pointer: offset */
    DWORD FileSize;              /* Size of the file */
    BYTE  Status;
#define  NO_WRITE   0           /* File state was stable */
#define  WRITING    1           /* Write operation in process */
#define  DELETING   2           /* Deleting file */
#define  FAILED     3           /* Corrupted File */
    
} FILE_HEADER;

#define FILE_HEADER_SIZE sizeof(FILE_HEADER)

#define INIT_BLOCK_SIZE   512      /* initial size of a DSA file block */
#define EXTEND_BLOCK_SIZE 512      /* extended size of a DSA file block */

#define NO_HANDLE    0xff       /* free entry */
#define SHARE_HANDLE 0x00       /* handle of shared files */

#endif

