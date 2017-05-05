/********************************************************************
*                setitim.c
*********************************************************************
                                                      
       Purpose:      Set an interval timeout in 1/64 second ticks      
       Parameters Passed: 	number of time increments and the time 
		 							increment                   
       Parameter Returned: long value of tick for the expiration time 
		 							suitable for comparison to the system tick
									value
*******************************************************************/

//#include <io.h>
#include "debug.h"

unsigned long set_itimeout (int h_clock, unsigned int time, unsigned long gradient)
{
#ifndef DEVELOP_VERSION
    /* ensure the gradient is valid */
   
/*
	unsigned long cur_ticks;
	switch (gradient)
    {
        case TM_MINUTES:
        case TM_SECONDS:
		case TM_TICKS:        
            break;        
        default:        
            return INVALID_TIME;
    }
   
    // copy the current clock tick value to the variable 
    //ioctl (h_clock, GetCtrl | 0x0001, &cur_ticks);
    // Vipul
    cur_ticks = read_ticks();
    //cur_ticks = (1000 * read_ticks()) / 64;
    //
    
    // return the current tick value plus the delay 
   return (cur_ticks + (gradient * (unsigned long) time));
*/ 
#endif
	return 0;
}

