#ifndef _COMMS_DEF_H_
#define _COMMS_DEF_H_

/**********************************************************************\
  COMMS Common Defines
\**********************************************************************/


/**********************************************************************\
Protocol Types
\**********************************************************************/


#define COMMS_NONE		0 /* None used */											
	
/**********************************************************************\
 Link Protocols																
\**********************************************************************/
#define COMMS_GPRS		10 /* GPRS */												
#define COMMS_ETHERNET	11 /* Ethernet */											
#define COMMS_WIFI		12 /* Wifi */												
#define COMMS_PPP 		13 /* ISP Connection */										
#define COMMS_GSMRADIO 	14 /* GSM Radio */											
#define COMMS_USBWIFI	15 /* WIFI */												


/**********************************************************************\
Messaging Protocols															
\**********************************************************************/
#define COMMS_TCPIP		20 /* TCP/IP */												
#define COMMS_GFTP		21 /* GFTP */												
	
#define COMMS_LINE		30 /* ASYNC PSTN DIALUP */									
#define COMMS_DIALUP	31 /* ASYNC PSTN DIALUP */									
#define COMMS_HDLC		32 /*  SYNC PSTN DIALUP */									
	
#define COMMS_SERIAL	40 /* ASYNC SERIAL */										
#define COMMS_RS485		41 /*  SYNC SERIAL */										

#define COMMS_GSM		50 /* GSM data call */										
#define COMMS_LAN_IP		60 /* LAN IP */										


/**********************************************************************\
Status / return codes														
\**********************************************************************/
#define COMMS_SUCCESS			  0 /*0 successful */								
#define COMMS_UNKNOWN_ERROR		-41 /* unknown error */								
#define COMMS_NOT_SUPPORTED		-42 /* a function is not supported */				
#define COMMS_PARAMETER_ERROR	-51 /* wrong input parameters */					
#define COMMS_USER_BREAK		-52 /* user pressed cancel */						
#define COMMS_TIMEOUT			-53 /* timeout */									
#define COMMS_DISCONNECTED		-54 /* disconnected */								
#define COMMS_NO_PHONE_LINE		-55 /* no phone line */								
#define COMMS_NO_GSM			-56 /* no gsm radio */								

#define COMMS_CONNECT_ERROR		-61 /* connect error */								
#define COMMS_SEND_ERROR		-62 /* send error */								
#define COMMS_RECV_ERROR		-63 /* recv error */								
#define COMMS_NETWORK_ERROR		-64 /* network error */								
#define COMMS_ACCEPT_ERROR		-65 /* connect error */								
			
/**********************************************************************\
Switch related return codes													
\**********************************************************************/
#define COMMS_SWITCH_ERROR		-71 /* switch dlm related error */					
#define COMMS_CHECKSUM_ERROR	-72 /* receive failed due to checksum error */		
#define COMMS_BUFFER_SIZE_ERROR	-73 /* insufficient buffer size */					
#define COMMS_SWITCH_KEY_ERROR	-74 /* secure kb key error */						


/**********************************************************************\
  Field Lengths				
\**********************************************************************/
#define L_COMMS_TEL_NUM			16 /* Length of Telephone Number */					
#define L_COMMS_IP_ADDRESS		16 /* Length of IP Address */						
#define L_COMMS_APN				19 /* Length of APN */								
#define L_COMMS_USERNAME		32 /* Length of username */							
#define L_COMMS_PASSWORD		32 /* Length of password */							
#define L_COMMS_PREFIX			5  /* Length of telephone prefix */					
#define L_COMMS_DESTHOSTNAME	32 /* Length of Dest Host Name */					

/**********************************************************************\
  CONFIG LEVELS															
\**********************************************************************/
#define COMMS_CONFIG_DEFAULTS	0													
#define COMMS_CONFIG_SIMPLE		1																											
#define COMMS_CONFIG_MODERATE	2													
#define COMMS_CONFIG_ADVANCED	3													

/**********************************************************************\
  Other defines																
\**********************************************************************/
#define COMMS_TRUE				1 /* used in boolean expressions */					
#define COMMS_FALSE				2 /* used in boolean expressions */					

#define COMMS_YES				COMMS_TRUE											
#define COMMS_NO				COMMS_FALSE											

#define UART_OK				0
#define UART_ER				-10
#define NO_UART				-1
#define UART_NO_CREATE		-2

#define SERIAL_EXPECT_MORE_BYTES_TIMEOUT	100

#endif	// _COMMS_DEF_H_
