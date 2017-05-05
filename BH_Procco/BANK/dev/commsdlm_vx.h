
#ifndef _COMMSDLM_VX_H_
#define _COMMSDLM_VX_H_

#include "os_types.h"
#include "comms_def.h"
//#include VERITYPESDEF_H
#include "typedefs_vx.h"


#define         GPRS_ATTACH_SERVICE      128  /* Establish a connection by service code 99(default value) */
#define         GPRS_ATTACH_IP_SERVICE   64   /* Establish an IP connection by service code 98*/

#define MAX_COND  6
#define MAX_PH_NUM_PER_TARGET 3 /* number of phone numbers for each target */

#define RXTMO_DEF 100       	/* RxTimeout  [ms] for RX data */
#define SYNC_HDLC       1

#define DIAL_TARGETS   "DTRGTS" /* this is the block name                  */

#define MAX_PH_NUM_PER_TARGET 3 /* number of phone numbers for each target */
#define FORMAT_7E             0
#define FORMAT_7O             1
#define FORMAT_7N             2
#define FORMAT_8E             3
#define FORMAT_8O             4
#define FORMAT_8N             5

#define TYPE_8000_GSM    8013

#define TMO_DEFAULT				30 		/* Timeout [s] for logon to the network */
#define RXTMO_DEF				100     /* RxTimeout  [ms] for RX data */
#define NOA						5		/* Number of attemps */
#define LINE_ONLY				-1
#define GSM_CALL_DATA           0
#define UART_B                  1

#define  SUCCESS         0

#define L_COMMS_TEL_NUM			16 /* Length of Telephone Number */					
#define L_COMMS_IP_ADDRESS		16 /* Length of IP Address */						
#define L_COMMS_APN				32 /* Length of APN */						//modified by Kevin v0.05.0015 20150616		
#define L_COMMS_USERNAME		32 /* Length of username */							
#define L_COMMS_PASSWORD		32 /* Length of password */							
#define L_COMMS_PREFIX			5  /* Length of telephone prefix */					
#define L_COMMS_DESTHOSTNAME	32 /* Length of Dest Host Name */					

#define PROTMNGR_USERNAME_LEN       32
#define PROTMNGR_PASSWORD_LEN       32
#define PROTMNGR_IP_ADDRESS_STR_LEN 32
#define PROTMNGR_MAC_ADDRESS_LEN    6
#define LOGGING_MSG_LEN             17

#define TCPSERVER "COMMSVR"
#define WAIT_TIMEOUT 5 // In seconds, WAIT FOR EVENT TIMEOUT

typedef int	SOCKET;

#define EVENT_DATA_SIZE 500 
#define CONN_TIMEOUT 25 // In Seconds, CONNECTION TIMEOUT
#define RECV_TIMEOUT 7 // In Seconds, DATA RECV TIMEOUT

///////////////////////////// GPRS Defination /////////////////////////////////////////////////
extern unsigned char SendBuffer[750];
extern unsigned char ReceiveBuffer[750];
extern unsigned char SendRcv_Buffer[750];
//char socket_handle[4];
//unsigned char szRespEventData[500] ; //Response data from DEVMAN
extern long watchdog_timer;
//int Strong_Connection=0;
 

extern char envDefaultIP[];
extern char envGatewayIP[];
extern char envSubnetMASK[];
extern char default_ip[];
extern char gateway_ip[];
extern char subnet_mask[];

extern char envUseDHCP[];  /* IP NIN DHCP den alýnmas?*/
extern char envSetDHCP[];  /* DHCP den alýnan IP hazýr */
extern int  UseDHCP;                       /* IP nin DHCP den alýnmas?    */
extern int  SetDHCP;                       /* DHCP den alýnan IP Hazýr     */




typedef struct																			
{																						
	byte use_dhcp;								/* Set COMMS_TRUE for using DHCP */		
	char ip_address[L_COMMS_IP_ADDRESS];		/* IP Address, such as "192.168.1.104"*/
	char netmask[L_COMMS_IP_ADDRESS];			/* Netmask, such as "255.255.255.0"  */	
	char gateway_address[L_COMMS_IP_ADDRESS];	/* Gateway Address,such as 192.168.1.1*/
	char pri_dns[L_COMMS_IP_ADDRESS];			/* Primary DNS Address */				
	char sec_dns[L_COMMS_IP_ADDRESS];			/* Secondary DNS Address */				
	char host_ip[L_COMMS_IP_ADDRESS];			/* Host Address */						
	char DestHostName[L_COMMS_DESTHOSTNAME];	/* Length of Dest Host Name */			
	byte ConnectRetry;							/* Connection Retry */					
	usint LoginTimeout;        					/* in seconds from 0-32767(default 60)*/
} EthernetParamStruct;			


typedef struct																			
{																						
	char apn[L_COMMS_APN];																
	char username[L_COMMS_USERNAME];			/* username */							
	char password[L_COMMS_PASSWORD];			/* password */							
	byte use_dhcp;								/* Set COMMS_TRUE for using DHCP */		
	char ip_address[L_COMMS_IP_ADDRESS];		/* IP Address, such as "192.168.1.104" */	
	char netmask[L_COMMS_IP_ADDRESS];			/* Netmask, such as "255.255.255.0"  */		
	char gateway_address[L_COMMS_IP_ADDRESS];	/* Gateway Address, such as "192.168.1.1"*/	
	char pri_dns[L_COMMS_IP_ADDRESS];			/* Primary DNS Address */				
	char sec_dns[L_COMMS_IP_ADDRESS];			/* Secondary DNS Address */				
	char host_ip[L_COMMS_IP_ADDRESS];			/* Host Address */						
	char DestHostName[L_COMMS_DESTHOSTNAME];	/* DESTINATION HOST NAME */				
	byte LoginTimeout;							/* Login Timeout in sec. 0-32767 (60)*/	
	usint hport;
} GprsParamStruct;

typedef struct																			
{																						
	char host_ip[L_COMMS_IP_ADDRESS];	/* host ip number, such as "192.168.1.1" */		
	usint host_port;	/* host tcp port, such as 80 */									
	SOCKET sock;		/* internal, do not modify! */									
	ulint read_timeout;	/* read timeout in milliseconds*/								
	byte TcpIpParams;	/* Connection Parameters */										
	#define TCPIP_NOCONECTSOCKET	0x01	/* Only Connect to ISP, do not open Socket! */
} TcpipParamStruct;	


typedef enum
{
	GFTP_SUCCESS        =  0,
	GFTP_CONNECT_ERR    = -1, /*cnct_rl*/
	GFTP_USER_ERR       = -2, /*user_rl*/
	GFTP_PASS_ERR       = -3, /*pass_rl*/
	GFTP_ACCT_ERR       = -4, /*acct_rl*/
	GFTP_CD_ERR         = -5, /*cd_rl  */
	GFTP_DEL_ERR        = -6, /*del_rl */
	GFTP_PASV_ERR       = -7, /*port_rl*/
	GFTP_LIST_ERR       = -8, /*list_rl*/
	GFTP_UPLOAD_ERR     = -9, /*stor_rl*/
	GFTP_DOWNLOAD_ERR   = -10,/*retr_rl*/
	GFTP_RESUME_ERR     = -11,/*rest_rl*/
	GFTP_TYPE_ERR       = -12,/*type_rl*/
	GFTP_LOGOFF_ERR     = -13,/*quit_rl*/
	GFTP_GENERAL_ERR    = -14,
	GFTP_OPEN_ERR       = -15,
	GFTP_SEND_ERR       = -16,
	GFTP_RECV_ERR       = -17,
	GFTP_INVALID_PTR    = -18,
	GFTP_FULL_HANDLES   = -19,
	GFTP_ABORTED        = -20,
	GFTP_Max_Errors     = -21
}GFTP_err_t;


typedef enum
{
	GFTP_PARAMS,
	GFTP_RAM,
	GFTP_RAMDISK,
	GFTP_RAMDISKTLV,
	GFTP_NOSAVE
}GFTP_FileType_t;

typedef enum
{
	GFTP_APPEND        = 0x01,
	GFTP_OVERWRITE     = 0x02,
	GFTP_RESUME        = 0x04,
	GFTP_ASCII         = 0x08,
	GFTP_EBCDIC        = 0x10,
	GFTP_IMAGE         = 0x20,
}GFTP_TransferType_t;

typedef enum
{
	GFTP_BLOCKING,
	GFTP_NONBLOCKING
}GFTP_RunMode_t;
																	

///////////////////////////////////////////////////////////////////////////////////////////////

#endif	// _COMMSDLM_VX_H_

