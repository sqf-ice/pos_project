#ifndef _OS_DEFINE_
#define _OS_DEFINE_

#include "OS_TYPES.h"


// Gary add
#define CUSTOMER_CARD       0



#define MAX_DISP_WIDTH		22      
#define	MAX_OPEN_FILE		50


#define	INPUT_OK			0
#define INPUT_NOK			1




#define OS_LG_TRANSDATE			    6 // DDMMYY
#define OS_LG_TRANSTIME			    4 // HHMM




#define MAX_ALPNUM_KEYS 12 
#define CHAR_PER_KEY 6 


#define  IN_NUM				0x00
#define  IN_RESET			0x01
#define  IN_PHONE			0x02


#define TIMER_EXPIRED		0	
#define SECOND				100	

#define DISP_WIDTH          21      // Gary : 22->21


#define	EX_CARD				0	



/* key masks */
#define KEYB_COR_MASK     (1 << 0)
#define KEYB_VAL_MASK     (1 << 1)
#define KEYB_CANCEL_MASK  (1 << 2)
#define KEYB_NUMERIC_MASK (1 << 3)
#define KEYB_MENU_MASK    (1 << 4)
#define KEYB_HOT1_MASK    (1 << 5)
#define KEYB_HOT2_MASK    (1 << 6)
#define KEYB_HOT3_MASK    (1 << 7)
#define KEYB_HOT4_MASK    (1 << 8)
#define KEYB_SHIFT_MASK   (1 << 9)
#define KEYB_ALL_MASK ( KEYB_COR_MASK     |   \
	                    KEYB_VAL_MASK     |   \
	                    KEYB_CANCEL_MASK  |   \
	                    KEYB_SHIFT_MASK   |   \
	                    KEYB_NUMERIC_MASK |   \
	                    KEYB_HOT1_MASK    |   \
	                    KEYB_HOT2_MASK    |   \
	                    KEYB_HOT3_MASK    |   \
	                    KEYB_HOT4_MASK    |   \
	                    KEYB_MENU_MASK )




/*
#define	TIME_MASK				0x0001
#define	TIME_DELETE_ALARM		0x0002
#define	ICC_MASK				0x0004
#define	COM_MASK				0x0008
#define	MAG_MASK				0x0010
#define	KEYB_MASK				0x0012
#define	KEYB_MERCHANT_CANCEL	0x0014
#define	WAIT_ABORT				0x0016
#define	USER_CONNECT			0x0018
#define	ICC_WAIT_IN				0x0020
#define TIME_SET_ALARM			0x0022
*/

#define	USER_CONNECT			0x0018

#define BPS_2400		2
#define NO_PARITY		1
#define EIGHT_BITS		1
#define ONE_STOP_BIT	1



#define READ_AND_WRITE_BUFFERS	0




#define		DEBUG_NONE				0
#define		DEBUG_PORT				1
#define		DEBUG_PRINTER			2
#define		DEBUG_PORT_AND_PRINTER	3


#define OS_MOD_NODIALTONE	-1
#define OS_MODEM_UNKNOWN	-2
#define OS_MOD_NOCARRIER	-3
#define OS_MOD_UNKNOWN		-4
#define OS_MODEM_ERROR		-5
#define OS_MOD_NOANSWER		-6
#define OS_MOD_BUSY			-7

#define OS_MODEM_OPEN_ERROR	-8


#define  DTR_ON          0x01
#define  DTR_OFF         0x00
#define  RTS_OFF         0x00
#define  RTS_ON          0x02
#define  DCD_ON          0x08
#define  DTR_RTS_ON      0x03



#define MODEM_HDLC		0
#define MODEM_ASYNC		1





#define LINE_CHARS	42
#define LINE_BYTES	10



#define KEY_A1 0x61
#define KEY_A2 0x62
#define KEY_A3 0x63
#define KEY_A4 0x64

//----------------------------------------------------------------

#define SECOND 100           //!< \brief second definition (100)
#define MINUTE (60L*SECOND)  //!< \brief minute definition
#define HOUR (60L*MINUTE)    //!< \brief hour definition

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define castptr(cast_type,cast_var) ((cast_type)(void*)(cast_var))
//----------------------------------------------------------------

typedef struct 
{
	BYTE gFiles[MAX_OPEN_FILE][4+1];
	INT  ghFiles[MAX_OPEN_FILE][1];
	INT  gTotalOpenedFiled;
}OPEN_FILED;




#define LG_APPLTITLE   16
#define LG_APPLVERSION   2
// #define LG_APPLCHK      4
typedef struct
{
	BYTE ApplTitle[LG_APPLTITLE + 1];
	BYTE ApplCompatibility[LG_APPLVERSION + 1];
	BYTE ApplVersion[LG_APPLVERSION + 1];
	WORD ApplCHK;
	BYTE ApplStateActiv;
} APPLCONFIGSTRUCT;



typedef struct
{

	CHAR tx_buffer_enable; // = OS_TRUE;
	CHAR rx_buffer_enable;//  = OS_TRUE;
	CHAR bConnType;//HDLC OR SEN. 

	BYTE rate;
	BYTE format;
	BYTE protocol;
	BYTE parameter;
	INT	 iHandle;

	CHAR comType;	
	
}COMM_PARAM_STRUCT;



#define NUM_STATE		0x00
#define ALPHA_STATE		0x01
#define NR_OF_SIGNS		26												// Number of special signs in alpha mode
#define DSPL			21												// Display Buffer Length
// FormatFlag values:
#define NOFORMAT		0x00											// 0x00: No special format.
#define DESTROYDEFAULT	0x01											// 0x01: Destroy if input.
#define PANENTRYFORMAT	0x02											// 0x02: Pan Entry Format.
#define DATE_MMYY		0x04											// 0x04: Date Entry, MM/YY Format.
#define DATE_DDMMYYYY	0x08											// 0x08: Date Entry, DD/MM/YYYY Format.
#define TIME_HHMM		0x10											// 0x10: Time Entry, HH:MM Format.
#define IPENTRYFORMAT	0x20											// 0x20: IP Entry, NNN.NNN.NNN.NNN
#define NOCLEARDISP		0x40											// 0x40: Dont clear display


typedef enum
{
	/* assign numbers for port's to be sequential   */
	/* so the define can be used in an array index   */
	/*  DO NOT CHANGE THESE VALUES                  */
	PORT_1            = 0,     /* COM1 FULL RS   */
	PORT_2            = 1,     /* COM4 SIMPLE RS */
	PORT_3            = 2,     /* COM3 MODEM     */
	PORT_4            = 3,     /* COM5 SIMPLE RS */
	PORT_5            = 4,     /* BIBERON RS     */
	PORT_6            = 5,     /* ICC + 11ETU    */
	PORT_7            = 6,     /* M8000 internal */
	PORT_8            = 7,     /* M8000 internal */
	PORT_9            = 8,     /* Fastnet        */
	PORT_COUNT,                /* do NOT assign a value.  It must follow
							   *  the last valid port number */
							   PORT_NOT_SET,
							   NO_PORT_TYPE      = 85,
} PORT_TYPE;

/*      parts of the code in the system assume that the above PORT_1 through
**      PORT_4 are in numberic order.  If changes to the above structure are
**      made, do not insert between these limits; and grep on use of boundaries
**      (e.g., look for validity checks on PORT_4 as the upper bound, and
**      change them to PORT_5 or whatever)
*/


#endif



