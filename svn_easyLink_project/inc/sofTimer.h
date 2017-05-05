/*
 * ============================================================================
 * COPYRIGHT
 *              Pax CORPORATION PROPRIETARY INFORMATION
 *   This software is supplied under the terms of a license agreement or
 *   nondisclosure agreement with Pax Corporation and may not be copied
 *   or disclosed except in accordance with the terms in that agreement.
 *      Copyright (C) 2016 Pax Corporation. All rights reserved.
 * Module Date:2016/08/09     
 * Module Auth:huangwp     
 * Description:soft timer module

 *      
 * ============================================================================
 */


#ifndef _SOFTIMER_H_
#define _SOFTIMER_H_
#define	SOF_TIMER_TOT		4

#define	SOF_TM00		0X00
#define	SOF_TM01		0X01
#define	SOF_TM02		0X02
#define	SOF_TM03		0X03
#define	SOF_TM04		0X04
#define	SOF_TM05		0X05
#define	SOF_TM06		0X06
#define	SOF_TM07		0X07
#define	SOF_TM08		0X08
#define	SOF_TM09		0X09
#define	SOF_TM0A		0X0A
#define	SOF_TM0B		0X0B
#define	SOF_TM0C		0X0C
#define	SOF_TM0D		0X0D
#define	SOF_TM0E		0X0E
#define	SOF_TM0F		0X0F
#define	SOF_TM10		0X10
#define	SOF_TM11		0X11
#define	SOF_TM12		0X12
#define	SOF_TM13		0X13
#define	SOF_TM14		0X14
#define	SOF_TM15		0X15
#define	SOF_TM16		0X16
#define	SOF_TM17		0X17
#define	SOF_TM18		0X18
#define	SOF_TM19		0X19
#define	SOF_TM1A		0X1A
#define	SOF_TM1B		0X1B
#define	SOF_TM1C		0X1C
#define	SOF_TM1D		0X1D
#define	SOF_TM1E		0X1E
#define	SOF_TM1F		0X1F

#define	TM_Msg_CMD	SOF_TM00

#define TM_MsgSrv_CMD SOF_TM02

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * ============================================================================
 * Function:    SofTimerSet
 * Description: set soft timer
 * Returns:     
 * Parameter: [in]uchar ucTMID   set timerid
 			  [in]uchar ucTMID   set time(ms)   correct to 10ms    
 * ============================================================================
 */

void  SofTimerSet(uchar ucTMID, ulong  ulMs);

/*
 * ============================================================================
 * Function:    SofTimerCheck
 * Description: check soft timer, get residue time
 * Returns:     return the count of time unit  ,the unit is 10ms
 * Parameter: [in]uchar ucTMID   timerid
 * ============================================================================
 */

ulong SofTimerCheck(uchar ucTMID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#pragma pack()


#endif
