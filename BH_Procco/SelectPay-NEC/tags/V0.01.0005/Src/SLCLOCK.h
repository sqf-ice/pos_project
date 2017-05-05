#ifndef _SLCLOCK_H
#define _SLCLOCK_H
/*
*%-----------------------------------------------------------------------------
*% File........... : SLCLOCK.H
*% Module ........ : Application Library
*% Sub-module .... : Date and time processing
*% Description ... : This module process date and time (check format, read
*%                   and set internal clock ...)
*%-----------------------------------------------------------------------------
*/


/* $GlobalDataStart ----------------------------------------------------------*/
//#include "slgeneri.h"
#include "TYPEDEF.h"

typedef enum
{
   F_YYMMDD = 1,
   F_DDMMYY,
   F_YYMM,
   F_MMYY,
   F_DDMM,
   F_HHMMSS,
   F_HHMM,
   F_MMDDYY                     // for US consumption--BMC 10APR97
} DATEFMT;


/*$--- End Of File : slclock.h ---*/
#endif
