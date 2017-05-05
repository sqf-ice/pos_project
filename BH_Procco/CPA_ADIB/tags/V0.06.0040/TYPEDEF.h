/*
*%-----------------------------------------------------------------------------
*% File........... : TYPEDEF.H
*% Project........ : SIGMA PORTABLE
*% Created ....... : 05.06.1996
*% Author ........ : EY
*%                 : R&D T.T.S.
*%                 : SCHLUMBERGER Technologies
*%                 : 50, Avenue Jean Jaures, 92542 Montrouge Cedex, FRANCE
*% Version ....... : 1.00
*% Module ........ : Any
*% Sub-module .... : type definitions
*% Description ... :
*% Copyright ..... : (c) Copyright SCHLUMBERGER Technology Corporation
*%                 : unpublished work.
*%                 : This computer program includes Confidential, Proprietary
*%                 : Information and is a Trade Secret of SCHLUMBERGER
*%                 : Technology Copr. All use, disclosure, and/or reproduction
*%                 : is prohibited unless authorised in writing.
*%                 : All rights reserved.
*%-----------------------------------------------------------------------------
*% $Revision: 1.3 $
*% $Archive:   Z:/Dev/Core/MCS/AGP2/AM/CORE/Include/Typedef.h_v  $
*% $Log: TYPEDEF.h,v $
*% Revision 1.3  2010/12/09 02:32:17  zhangh
*% *** empty log message ***
*%
*% Revision 1.2  2010/12/09 02:27:12  hejj
*% *** empty log message ***
*%
*% Revision 1.1  2010/12/07 07:33:32  hejj
*% 2010-12-7 15:32 by Gary
*%
 * 
 *    Rev 1.0   01 Mar 2001 14:58:54   kersaho
 * First Release
 * 
 *    Rev 1.0   Apr 06 1999 20:09:40   BRCAROTHERS
 * Checkin of v2.16 files for new LAN configuration.
 * 
 *    Rev 1.1   23 Apr 1998 15:10:50   dave
 * Updated header
*%-----------------------------------------------------------------------------
*/
#ifndef _TYPEDEF_H
#define _TYPEDEF_H

    #include "os_define.h"

    /* $GlobalDataStart --------------------------------------------------------*/
    typedef BYTE  *BYTEPTR;
    typedef OS_WORD  *WORDPTR;
    typedef DWORD *DWORDPTR;

    #define OS_NO  (BYTE)0
    #define OS_YES (BYTE)1


    /* $GlobalDataEnd ----------------------------------------------------------*/

#endif
/*$--- End Of File : typedef.h ---$*/


