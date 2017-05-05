/***************************** tcpip.h *************************************
 *
 *       **************************************************************
 *       ** Copyright (c) 1993-2001  Schlumberger Technologies, Inc. **
 *       ** This computer program includes confidential, proprietary **
 *       ** information and is a trade secret of Schlumberger.  All  **
 *       ** use, disclosure, and/or reproduction is prohibited un-   **
 *       ** less expressly authorized in writing.                    **
 *       ** All rights reserved.                                     **
 *       **************************************************************
 *
 *        PURPOSE:  Definitions for MagIC TCP/IP Library
 *
 *
 * 
 *  
 *
 ****************************************************************************/

#ifndef __TCPIP_H__
#define __TCPIP_H__


#include "os_ser.h"


#ifndef AF_INET
#define	AF_INET			2			/* internetwork: UDP, TCP, etc. */
#endif

#ifndef INADDR_NONE
#define	INADDR_NONE		    -1		/* -1 return */
#endif


// DO NOT CHANGE THE VALUES BETWEEN -1 and -100
// They are reserved for the stack's error codes
#define		TCP_ERR_ALLOC		-2
#define		TCP_ERR_PARAM		-3
#define		TCP_ERR_INVADDR		-4
#define		TCP_ERR_CONFIG		-5
#define		TCP_ERR_CONNECT		-6
#define		TCP_ERR_RESET		-7
#define		TCP_ERR_TIMEOUT		-8

#define		TCP_ERR_NOCONN		-101
#define		TCP_ERR_NORECV		-102
#define		TCP_ERR_WRITE		-103

#define		TCP_PPP_DOWN		-104


typedef enum
{

   PPP_OK                      = 1,
   PPP_IFACE_BAD_TYPE,
   PPP_IFACE_ERROR,
   PPP_BAD_PORT,
   PPP_BAD_BAUD,
   PPP_BAD_ATTEMPTS,
   PPP_BAD_AUTH,
   PPP_INIT_FAIL,
   PPP_CONN_TIMEOUT,
   PPP_MAX_ATTEMPTS,
   PPP_AUTH_FAILED,
   PPP_UP,
   PPP_DOWN,
   PPP_CONN_ERROR

} PPP_STATUS;


typedef enum {
	PPP_INTERNAL_MODEM,
	PPP_EXTERNAL_MODEM,
	PPP_SERIAL_PORT
} PPP_IFACE_TYPE;


#define		PPP_AUTH_PAP		1
#define		PPP_AUTH_CHAP		2
#define		PPP_AUTH_SCRIPT		4


typedef struct {
	PPP_IFACE_TYPE iface;
	PORT_TYPE      serial_port;
	BAUD_TYPE      baud_rate;
	BYTE           init_string[64];
	BYTE           tel_num[32];
	BYTE           dial_type;
	BYTE           dial_attempts;
	BYTE           auth_login[32];
	BYTE           auth_passwd[32];
	BYTE           login_script[80];
	OS_WORD           script_timeout;
	OS_WORD           auth_type;
} PPP_PARAM_STRUCT;


typedef int SOCK_DES;


#ifdef __cplusplus
extern "C" {
#endif

PPP_STATUS  PPP_Init( const PPP_PARAM_STRUCT * ppp_params);
PPP_STATUS  PPP_Connect( DWORD timeout );
PPP_STATUS  PPP_Disconnect( void );
PPP_STATUS  PPP_Close( void );
PPP_STATUS  PPP_Status( BYTE * localIPaddr, BYTE * remoteIPaddr );

SOCK_DES    TCP_Socket( void );
SOCK_DES    TCP_Bind( SOCK_DES sd, struct sockaddr_in * localAddr );
int         TCP_Listen( SOCK_DES sd, OS_WORD backLog );
SOCK_DES    TCP_Accept( SOCK_DES sd, struct sockaddr_in *peerAddr, DWORD timeout );
int         TCP_Connect( SOCK_DES sd, struct sockaddr_in *remoteAddr, DWORD timeout );
int         TCP_Read( SOCK_DES sd, BYTE * buf, OS_WORD size, DWORD timeout );
int         TCP_Write( SOCK_DES sd, const BYTE * buf, OS_WORD size, DWORD timeout );
int         TCP_Close( SOCK_DES sd, DWORD timeout );

DWORD       IP_InetAddr( BYTE * addr );
OS_WORD        IP_HostToNetworkShort( OS_WORD s );
DWORD       IP_HostToNetworkLong( DWORD l );

OS_WORD        IP_GetLastError( void );

#ifdef __cplusplus
}
#endif


#ifndef NET_H
struct in_addr {
	DWORD s_addr;
};

struct sockaddr_in {
	BYTE	sin_len;
	BYTE	sin_family;
	OS_WORD	sin_port;
	struct	in_addr sin_addr;
	BYTE	sin_zero[8];
};
#endif // NET_H

#endif // __TCPIP_H__
