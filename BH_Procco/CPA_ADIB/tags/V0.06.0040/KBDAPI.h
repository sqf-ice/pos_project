/*
*%-----------------------------------------------------------------------------
*% File........... : KBDAPI.H
*% Project........ : SIGMA PORTABLE
*% Created ....... : 29.04.1996
*% Author ........ : Sami KODJA
*%                 : R&D T.T.S.
*%                 : SCHLUMBERGER Technologies
*%                 : 50, Avenue Jean Jaures, 92542 Montrouge Cedex, FRANCE
*% Version ....... : 1.00
*% Module ........ : Keyboard
*% Sub-module .... : Keyboard - High Level Services
*% Description ... : This module manages High Level Service fonctions :
*%                   Scroll Menu, Hot key Menu and Input data management
*% Copyright ..... : (c) Copyright SCHLUMBERGER Technology Corporation
*%                 : unpublished work.
*%                 : This computer program includes Confidential, Proprietary
*%                 : Information and is a Trade Secret of SCHLUMBERGER
*%                 : Technology Copr. All use, disclosure, and/or reproduction
*%                 : is prohibited unless authorised in writing.
*%                 : All rights reserved.
*%-----------------------------------------------------------------------------
*% $Revision: 1.3 $
*% $Archive:   Z:/Dev/Core/MCS/AGP2/AM/CORE/Include/KBDAPI.H_v  $
*% $Log: KBDAPI.h,v $
*% Revision 1.3  2011/04/28 01:54:48  hejj
*% *** empty log message ***
*%
*% Revision 1.2  2011/02/22 06:17:30  hejj
*% *** empty log message ***
*%
*% Revision 1.1  2010/12/07 07:33:31  hejj
*% 2010-12-7 15:32 by Gary
*%
* 
*    Rev 1.1   28 Nov 2001 18:05:12   FPicconi
* added IN_COR_COR
* 
*    Rev 1.0   01 Mar 2001 14:58:00   kersaho
* First Release
* 
*    Rev 1.0   Apr 06 1999 20:00:14   BRCAROTHERS
* Checkin of v2.16 files for new LAN configuration.
* 
*    Rev 1.5   23 Sep 1998 15:00:12   DORF
* Added AM_GetAmount
* 
*    Rev 1.4   06 Jul 1998 17:30:38   DORF
* menu can handle up to MAX_ITEMS per level
* 
*    Rev 1.3   23 Apr 1998 15:12:30   dave
* Updated header
*%-----------------------------------------------------------------------------
*/
#ifndef _KBDAPI_H
#define _KBDAPI_H

#include "typedef.h"

#include "common.h"

// A mettre en commentaire si ON NE FAIT PAS de tests sur le simulateur
// #define DEBUG_MODE

/* $HeaderFileStart --------------------------------------------------------*/

#define MAX_DISP_WIDTH (20+1)

/* ************************************************************************ */
/* Input field type and input data type */
/* ************************************************************************ */

/* Input field type  (Bit 7 to Bit 5) */
#define IN_RESET       (BYTE) 128
#define IN_NULL_TERM   (BYTE)  64
#define IN_READ_ONLY   (BYTE)  32
#define IN_COR_COR     (BYTE)  16

/* Input data type   (Bit 3 to Bit 0) */
#define IN_NUM         (BYTE)   1
#define IN_ALPHA       (BYTE)   2
#define IN_ALPHA_NUM   (BYTE)   3
#define IN_HIDDEN      (BYTE)   4
#define IN_PHONE       (BYTE)   5
#define IN_NUM_ALPHA   (BYTE)   6

/* Input return code */
#define INPUT_OK       (char)   0
#define INPUT_BAD      (char)  -1
#define INPUT_TIMEOUT  (char)  -2
#define INPUT_CANCEL   (char)  -3
#define INPUT_CLEAR    (char)  -4     //  RFU
#define INPUT_PROGRESS (char)  -5     //  RFU (today only for internal use)

#define MAX_ITEMS 20
typedef BYTEPTR MENU[MAX_ITEMS];
typedef BYTEPTR HOTKEY_MENU[5];

/* $HeaderFileEnd ----------------------------------------------------------*/

/* $GlobalDataStart ----------------------------------------------------------*/
/* $GlobalDataEnd ------------------------------------------------------------*/

/* $GlobalFnStart ------------------------------------------------------------*/
extern "C"
{
    char KeyBoardHotKeyMenu( HOTKEY_MENU fHkMenu, BYTE fTimeOut);
    
    char AM_GetAmount( BYTE * fAmount, BYTE * fMinAmount,
        BYTE fTimeOut, BYTE * fTransacType,
        BYTE fFirstChar, AMOUNT_CONFIG * fAmtCfg,
        OS_WORD* curr_code, char* title);
    
    BYTE KeyBoardCaptureAmount( BYTE * fAmount,
        BYTE fTimeOut, AMOUNT_CONFIG * fAmtCfg);
    void DisplayMenu( BYTE fNbItem, MENU fMenu, BYTEPTR fFirstItem,
        BYTE fNewItem);
    char ScrollMenu( BYTE fNbItem, MENU fMenu, BYTEPTR fFirstItem,
        BYTE fOldItem, BYTE fKey);
    
    void DisplayHotKeyMenu( HOTKEY_MENU fHkMenu);
    char HotKeyChoice( HOTKEY_MENU fHkMenu, BYTE fKey);
    BYTE KeyBoardValid( BYTE * fPrompt, BYTE * fMessage1,
        BYTE * fMessage2, BYTE * fMsgHotKey1, BYTE * fMsgHotKey2,
        BYTE * fMsgHotKey3, BYTE fKeyMask, OS_WORD fTimeOut);
}
/* $GlobalFnEnd --------------------------------------------------------------*/

/* $StaticDataStart ----------------------------------------------------------*/
/* $StaticDataEnd ------------------------------------------------------------*/


/* $StaticFnStart ------------------------------------------------------------*/
/* $StaticFnEnd --------------------------------------------------------------*/

#endif
/*$--- End Of File : kbdapi.h ---*/