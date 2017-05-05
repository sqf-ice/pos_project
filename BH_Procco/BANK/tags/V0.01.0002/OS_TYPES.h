/******************************* os_types.h **********************************
 *
 *
 *        PURPOSE:  Definitions for MagIC standard data types
 *
 ****************************************************************************/



#ifndef __OS_TYPES_H__
#define __OS_TYPES_H__

#include "posapi.h"

typedef unsigned char		UCHAR;
typedef unsigned char		byte;
typedef unsigned char		BYTE;

typedef char				CHAR;

typedef short				SHORT;
typedef short int			sint;

typedef unsigned short int  usint;
typedef unsigned int		UINT;
typedef unsigned int		WORD;
typedef unsigned int		OS_WORD;

typedef int					INT;

typedef unsigned long		ULONG;
typedef unsigned long		DWORD;

typedef long				LONG;

typedef void				VOID;

typedef float				FLOAT;

typedef unsigned long int   ulint;

typedef long int          	SDWORD;
typedef unsigned long int 	UDWORD;
typedef unsigned short int	UWORD;

typedef BYTE*				PBYTE;
typedef CHAR*				PCHAR;


#define near
#define far
#define  __based(void) near


typedef int BOOLEAN;
#define OS_FALSE  ((BOOLEAN)0)
#define OS_TRUE   ((BOOLEAN)1)
#define OS_NONE   ((BOOLEAN)2)

#define VERSION_STR_SIZE   17


typedef unsigned int    EVT_ID;       /* OS event defines */
typedef unsigned int    APP_EVT_MASK;
typedef unsigned int    MBX_ID;       /* OS mailbox */
typedef unsigned int    MSG_ID;       /* OS message */
typedef unsigned int    SEM_ID;       /* OS semaphore */
typedef unsigned int    TASK_ID;      /* OS task */

typedef void(*INTVECTOR)();

/* enum for the OS_STATUS values. Note that none of the values should
* be 0 for safety purposes.
*/
typedef enum
{
	OS_FUNCTION_NOT_FOUND      = 0,
	OS_OK                      = 1,
	OS_INIT_FAIL               = 2,

	/* OS display error codes */
	OS_DISP_ERROR              = 3,
	OS_DISP_LOC_ERROR          = 4,
	OS_DISP_POS_ERROR          = 5,
	OS_DISP_INIT_FAIL          = 6,

	/* OS miscellaneous codes */
	OS_CONFIG_LOST             = 7,

	/* OS keyboard error codes */
	OS_KEYBD_ERROR             = 8,
	OS_KEYBD_INIT_FAIL         = 9,
	OS_KEYBD_TIMEOUT           = 10,

	/* OS magnetic stripe reader error codes */
	OS_MAG_ERROR               = 11,
	OS_MAG_TIMEOUT             = 12,
	OS_MAG_NOT_PRESENT         = 13,
	OS_MAG_SETUP_INVALID       = 14,
	OS_MAG_TRACK_ERROR         = 15,

	/* OS serial port error codes */
	OS_PORT_ERROR              = 17,    /* an error occurred with the port hw      */
	OS_PORT_TX_FULL            = 18,    /* the requested packet size is too big   */
	/* for the TX buffer or would cause an    */
	/* overflow due to the current contents   */
	/* of the TX buffer                       */
	OS_PORT_RX_FULL            = 19,    /* The RX buffer (used for buffered I/O)  */
	/* has overflowed due to the continued    */
	/* reception of data without the data     */
	/* being transferred to the application   */
	/* program                                */
	OS_PORT_BAD_BAUD           = 20,    /* The specified baud_rate  is invalid for*/
	/* the selected port                        */
	OS_PORT_BAD_TYPE           = 21,    /* The selected port cannot be configured */
	/* to the specified Interface_Type;       */
	/* not supported                          */

	OS_PORT_BAD_STOP_LEN       = 22,    /* Selection of stop bits param incorrect   */
	OS_PORT_BAD_PORT           = 23,    /* selected port param is invalid; either */
	/* it is an optional port which is not in */
	/* the system or the param is out of range*/
	OS_PORT_TX_BUSY            = 24,    /* the TX byte buffer is not empty. Either*/
	/* delay or check TxRDY signal and retry  */
	OS_PORT_NO_DATA            = 25,    /* no data available during port read     */
	OS_PORT_TIMEOUT            = 26,    /* the specific function 'timed out' prior*/
	/* to a successful complete of it's action*/
	OS_PORT_READ_ERR           = 27,    /* an error occurred during input of a    */
	/* data char this may have been parity,    */
	/*   overrun or framing                     */
	OS_PORT_BUFFER_NOT_ENABLED = 28,    /* the tx_buffer_enable param of the      */
	/*      COMM_PARAM_STRUCT for this port      */
	/*      is not enabled                        */

	OS_PORT_BAD_BUFF_TYPE       = 29,   /* the BUFFER TYPE input parameter was    */
	/* incorrect for the specified function   */

	/* OS clock error codes */
	OS_CLK_ERROR               = 30,
	OS_CLK_HOUR_BAD            = 31,
	OS_CLK_MIN_BAD             = 32,
	OS_CLK_SCD_BAD             = 33,
	OS_CLK_DAY_WEEK_BAD        = 34,
	OS_CLK_DAY_MONTH_BAD       = 35,
	OS_CLK_MONTH_BAD           = 36,
	OS_CLK_YEAR_BAD            = 37,
	OS_CLK_TIMER_ERROR         = 38,

	/* OS task error codes */
	OS_TASK_ERROR              = 40,
	OS_TASK_PRIORITY_BAD       = 41,
	OS_TASK_STACK_ERROR        = 42,
	OS_TASK_CURRENT            = 43,

	/* OS semaphore error codes */
	OS_SEM_ERROR               = 44,
	OS_SEM_OVERFLOW            = 45,
	OS_SEM_TIMEOUT             = 46,

	/* OS event table error codes */
	OS_EVT_ERROR               = 47,
	OS_EVT_MASK_BAD            = 48,
	OS_EVT_TIMEOUT             = 49,

	/* OS message error codes */
	OS_MSG_ERROR               = 50,
	OS_MSG_NO_MEM              = 51,

	/* OS mailbox error codes */
	OS_MBX_ERROR               = 52,
	OS_MBX_TIMEOUT             = 53,

	/* OS memory management error codes */
	OS_MEM_ERROR               = 54,
	OS_MEM_HEAP_ERROR          = 55,

	/* OS security error codes */
	OS_SEC_ERROR               = 56,
	OS_SEC_DISP_ERROR          = 57,
	OS_SEC_KEYBD_ERROR         = 58,
	OS_SEC_PROMPT_ERROR        = 59,
	OS_SEC_CANCEL              = 60,
	OS_SEC_TIMEOUT             = 61,
	OS_SEC_INVALID             = 63,
	OS_SEC_KEYSLOT_ERROR       = 64,
	OS_SEC_DUKPT_INVALID       = 65,

	OS_LOAD_CONTINUE           = 71,
	OS_LOAD_END                = 72,
	OS_LOAD_BAD_HANDLE         = 73,
	OS_LOAD_DUP_NAME           = 74,
	OS_LOAD_MORE               = 75,
	OS_LOAD_MEM_ERROR          = 76,
	OS_LOAD_ERROR              = 77,
	OS_LOAD_SEC_ERROR          = 78,
	OS_LOAD_APP_LIST_FULL      = 79,
	OS_LOAD_ABORT              = 80,

	OS_APP_ERROR               = 81,
	OS_APP_CURRENT             = 82,
	OS_APP_NAME_ERROR          = 83,
	OS_APP_LIST_END            = 84,
	OS_APP_TASK_ERROR          = 85,

	OS_PORT_BAD_TAILGATE       = 86,
	OS_PORT_INIT_FAIL          = 87,

	OS_XMODEM_CANCEL           = 88,
	OS_XMODEM_BAD_PACKET_TYPE  = 89,

	OS_APP_BAD_FILE_TYPE       = 90,
	OS_APP_BAD_CHECKSUM        = 91,

	OS_MODEM_1200              = 176,      /* OBSOLETE */
	OS_MODEM_2400              = 177,      /* OBSOLETE */
	OS_MODEM_UNKNOWN           = 178,      /* OBSOLETE */
	OS_MODEM_ERROR             = 179,      /* OBSOLETE */

	OS_POWERMGMT_ERROR         = 180,
	OS_PRINTER_NO_PAPER        = 181,
	OS_PRINTER_ERROR           = 182,
	OS_PRINTER_INIT_FAIL       = 183,
	OS_PRINTER_BUSY            = 184,

	OS_USER_BREAK				=185,
	OS_MOD_LINE_DROPPED			=186,
	OS_MOD_TIME_OUT				=187,

	/* Modem return codes */

	OS_MOD_ABORT               = 190, /* Abort requested */  
	OS_MOD_NORETURN            = 191, /* No returns */
	OS_MOD_VER                 = 192, /* software version returns */
	OS_MOD_TEST                = 193, /* Test sequence requested */
	OS_MOD_SREG                = 194, /* S register contents */

	/* !!CAUTION!! : DO NOT CHANGE BELOW OS_MOD_ VALUES. THESE VALUES ARE
	*               DIRECTLY USED BY OS_MODEM.C WHICH TRANSLATES THEM IN
	*               HAYES RETURN CODES.
	*/


	//rockwell codes
	OS_MOD_OK                  = 200,
	OS_MOD_CONNECT_300         = 201,
	OS_MOD_RING                = 202, /* Ring detected */
	OS_MOD_NOCARRIER           = 203,
	OS_MOD_ERROR               = 204,
	OS_MOD_CONNECT_1200        = 205,
	OS_MOD_NODIALTONE          = 206,
	OS_MOD_BUSY                = 207,
	OS_MOD_NOANSWER            = 208,
	OS_MOD_NOLINE              = 209,
	OS_MOD_CONNECT_2400        = 210,
	/* 211 to 221 are reserved by modem */
	OS_MOD_CONNECT_4800        = 211,
	OS_MOD_CONNECT_7200        = 212,
	OS_MOD_CONNECT_9600        = 213,
	OS_MOD_CONNECT_12000       = 214,
	OS_MOD_CONNECT_14400       = 215,

	OS_MOD_CONNECT_SNAC        = 220, /* future usage */
	OS_MOD_CONNECT_SNAC_SECURED= 221, /* future usage */
	OS_MOD_CONNECT_HYPERCOM    = 222, /* future usage */
	OS_MOD_CONNECT_SERVEUR     = 223, /* future usage */
	OS_MOD_DELAYED             = 224, /* Numero temporarily forbidden (fu) */
	OS_MOD_UNKNOWN             = 225, /* Unknown modem circuit */
	OS_MOD_K224                = 226, /* 73K224 modem circuit */
	OS_MOD_K324                = 227, /* 73K324 modem circuit */
	OS_MOD_CHIP                = 228, /* Rockwell modem circuit */
	//rockwell codes END 		 
	/* 228 to 299 are reserved by modem */

	OS_PROGRAM_OK              = 301,
	OS_ERASE_OK                = 302,
	OS_BAD_DEVICE              = 303,
	OS_RECOGNIZED_DEVICE       = 304,
	OS_BAD_PARAM               = 305,
	OS_ERROR_PROGRAM           = 306,
	NOT_FOR_THIS_DEVICE        = 307,
	OS_ALL_SECTORS_PROTECTED   = 308,
	OS_SOME_SECTORS_PROTECTED  = 309,

	OS_EMS_DUPFILE             = 400,   /* reserve 400 - 499 to EMS */
	OS_EMS_NOT_AVAILABLE       = 401,
	OS_EMS_FULL                = 402,
	OS_EMS_NOTFOUND            = 403,
	OS_EMS_EOF                 = 404,
	OS_EMS_LOCKED_FILE         = 405,

	OS_SOFT_RELOC_OK           = 500,  /* Reserve 500 - 599 to soft management */

	/* MBM return codes */
	OS_MBM_ERROR               = 600,

	/* Serial communication port return codes */
	OS_COM_ERROR               = 5000,
	OS_COM_TXFULL              = 5001,
	OS_COM_RXFULL              = 5002,
	OS_COM_RX                  = 5003,
	OS_COM_TIMEOUT             = 5004,
	OS_COM_BAD_PARAM           = 5005,
	OS_COM_CURRENT             = 5006,
	OS_COM_PORT3BUSY           = 5007,
	OS_COM_ABORT               = 5008,
	OS_COM_R_ERR               = 5009,
	OS_COM_R_CMD               = 5010,
	OS_COM_R_DATA              = 5011,
	OS_COM_R_BUSY              = 5012,
	OS_COM_R_BASE              = 5013,
	OS_COM_R_SYNCHRO           = 5014,
	OS_COM_IR_NOT_ONBASE       = 5015,
	OS_NOT_ONBASE              = OS_COM_IR_NOT_ONBASE,

#ifdef BASE
	/* Miscellaneous */
	OS_RAM_FAIL                = 5020,
	OS_RAM_OK                  = 5021,
#endif

	OS_AUTHENT_FAIL            = 6012,
	OS_UNLOCK_FAIL             = 6013,

	OS_COM_IR_ERR              = 6014,
	OS_COM_CORD_ERR            = 6015,


#ifdef BASE
	// Bad input parameter
	OS_BAD_PARAM               = 0xFFFE,
#endif



	OS_NOK					  =-7,
   /* System and general error */
   OS_DEVICE_NOT_READY        = -6,
   OS_ABORT                   = -5,
   OS_INVALID_DEVICE          = -4,
   OS_INVALID_PARAM           = -3,
   OS_NOT_SUPPORTED           = -2,
   OS_ERROR                   = -1

} OS_STATUS;

#endif

