/*
*%---------------------------------------------------------------------------
*% File........... : CONNECT.H
*% Project........ : SIGMA PORTABLE
*% Created ....... : 01.12.1996
*% Author ........ : HV
*%                 : T.T.S.
*%                 : SCHLUMBERGER Technologies
*%                 : 50, Avenue Jean Jaures, 92542 Montrouge Cedex, FRANCE
*% Version ....... : 1.00
*% Module ........ : French Library
*% Sub-module .... : 
*% Description ... : This module process string and memory (buffer)
*% Copyright ..... : (c) Copyright SCHLUMBERGER Technology Corporation
*%                 : unpublished work.
*%                 : This computer program includes Confidential, Proprietary
*%                 : Information and is a Trade Secret of SCHLUMBERGER
*%                 : Technology Copr. All use, disclosure, and/or reproduction
*%                 : is prohibited unless authorised in writing.
*%                 : All rights reserved.
*%---------------------------------------------------------------------------
*%
*% $Revision ----------------------------------------------------------------
*% REV   DATE      BY    DESCRIPTION
*% ----  --------  ----  ----------------------------------------------------
*% 1.00  01.12.96  HV    Initial Version
*%		 26.12.96  HV    Reorganisation, modify functions and files
*% 1.01  31.01.98  TE	 Add ModemSendCommand() prototype
*%						 (TML-DV-8008/001-00/001)
*%
*%---------------------------------------------------------------------------
*/

#ifndef _CONNECT
	#define _CONNECT

/* $HeaderFileStart --------------------------------------------------------*/
/* $HeaderFileEnd ----------------------------------------------------------*/

/* $GlobalDataStart --------------------------------------------------------*/
/* $GlobalDataEnd ----------------------------------------------------------*/

/* $GlobalFnStart ----------------------------------------------------------*/
/* $GlobalFnEnd ------------------------------------------------------------*/

/* $StaticDataStart --------------------------------------------------------*/
/* $StaticDataEnd ----------------------------------------------------------*/

/* $StaticFnStart ----------------------------------------------------------*/
/* $StaticFnEnd ------------------------------------------------------------*/


// Constants for status
#define OK						0

// Constants for connection time out
#define TIMER_CNX				20000L
#define TIMER_DIAL				160000L
// TBD HV : le time out doit etre a 10s pour CBSA
#define TIMER_CNX_CBSA			20000L

// Constants for inter-character time out
#define L_INTERCHAR_TO			3000	// Long inter character time out
#define	S_INTERCHAR_TO			750		// Short inter character time out 
#define M_INTERCHAR_TO			1000	// Long inter character time out

// Timer inter Char PAD EMA - !!!!! remark : don't touch !!!!!
//#define CBSA_INTERCHAR_TO		500 

// Essai HV160197 : pour tester la radio, a revoir et remettre a 500 
#define CBSA_INTERCHAR_TO		1000 

// Constants for connection type
#define CNX_CBSA				0
#define CNX_CBPR				1


#endif

/*$--- End of File : connect.h ---*/
