/*
*%-----------------------------------------------------------------------------
*% File........... : SLALG.H
*% Project........ : SIGMA PORTABLE
*% Created ....... : 09.04.1996
*% Author ........ : Eric YANBEKIAN
*%                 : R&D T.T.S.
*%                 : SCHLUMBERGER Technologies
*%                 : 50, Avenue Jean Jaures, 92542 Montrouge Cedex, FRANCE
*% Version ....... : 1.00
*% Module ........ : Application Library
*% Sub-module .... : Algorithm utilities
*% Description ... : This module supplies different algorithm utilities
*%           such as luhn key or CRC 16
*% Copyright ..... : (c) Copyright SCHLUMBERGER Technology Corporation
*%                 : unpublished work.
*%                 : This computer program includes Confidential, Proprietary
*%                 : Information and is a Trade Secret of SCHLUMBERGER
*%                 : Technology Copr. All use, disclosure, and/or reproduction
*%                 : is prohibited unless authorised in writing.
*%                 : All rights reserved.
*%-----------------------------------------------------------------------------
*%
*% $Revision ------------------------------------------------------------------
*% REV   DATE      BY    DESCRIPTION
*% ----  --------  ----  ------------------------------------------------------
*% 1.00  19.04.96  EY    Initial Version
*% 1.00  29.07.96  SK    Modification for integration
*% 1.00  23.09.96  SK    Integration of AddBcdNum, SubBcdNum, CmpBcdNum,
*%                       MulBcdNum and DivBcdNum functions
*% $Log: SLALG.h,v $
*% Revision 1.1  2010/12/07 07:33:32  hejj
*% 2010-12-7 15:32 by Gary
*%
 * 
 *    Rev 1.1   Mar 27 2001 10:19:50   doguy
 *  
 * 
 *    Rev 1.0   01 Mar 2001 15:07:14   kersaho
 * To be updated 
 * 
 *    Rev 1.0   01 Mar 2001 12:13:28   kersaho
 * First release
 * 
 *    Rev 1.0   04 Sep 1997 16:57:50   dave
 *  
 *
 *    Rev 1.3   Nov 22 1996 12:06:38   KODJA
 * MulBcdNum, DivBcdNum modification of the API: fOper2 becomes a DWORD instead of a OS_WORD
*%
*%
*%-----------------------------------------------------------------------------
*/

/* Maximum size (nb BYTES) for Multiplication and division */
#define MAX_BCD2_SIZE 16   // ie 32 nibbles/digits



/* $GlobalFnStart ---- Function prototypes ---------------------------------*/
extern "C"
{
   BYTE AlgLuhnKey( BYTE fCardNumSize, BYTEPTR fCardNum);
   OS_WORD AlgCrc16( OS_WORD fInBufLen, BYTEPTR fInBuf);
   BYTE AlgLrc( OS_WORD fInBufSize, BYTEPTR fInBuf, BYTE fInitLrc);

   AL_STATUS AddBcdNum( BYTEPTR fOper1, BYTEPTR fOper2,
                         BYTEPTR fResult, int fBuffSize);
   AL_STATUS SubBcdNum( BYTEPTR fOper1, BYTEPTR fOper2,
                         BYTEPTR fResult, int fBuffSize);
   int CmpBcdNum( BYTEPTR fOper1, int fOper1Size,
                   BYTEPTR fOper2, int fOper2Size);
   AL_STATUS MulBcdNum( BYTEPTR fOper1, DWORD fOper2,
                         BYTEPTR fResult, int fBuffSize);
   AL_STATUS DivBcdNum( BYTEPTR fOper1, DWORD fOper2,
                         BYTEPTR fResult, int fBuffSize);

}
/* $GlobalFnEnd -----------------------------------------------------------*/


/*$--- End Of File : slalg.h ---*/
