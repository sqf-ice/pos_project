/*------------------------------------------------------------
* FileName: HttpPacket.h
* Author: liukai
* Date: 2016-02-22
------------------------------------------------------------*/
#ifndef HTTPPACKET_H
#define HTTPPACKET_H

#define HTTPPACKET_OK					0
#define	HTTPPACKET_ERR					-1
#define HTTPPACKET_INVALIDPARAM			-2

#define BUFFER_SIZE	1024
//
//typedef enum _appStatus {
//	MOBILEPAY_STATUS_NOT_CONFIGURED = '1',
//	MOBILEPAY_STATUS_NOT_REGISTERED = '2',
//	MOBILEPAY_STATUS_NOT_OPERATIVE = '3',
//	MOBILEPAY_STATUS_OPERATIVE = '4',
//	MOBILEPAY_STATUS_OPERATION_PENDING = '5'
//} APPSTATUS;

typedef enum _paymentCmd {
//Administration Commands
	CMD_ADMIN_REGISTER_POS,
	CMD_ADMIN_UPDATE_REGISTERED_POS_NAME,
	CMD_ADMIN_UNREGISTER_POS,
	CMD_ADMIN_ASSIGN_POS_UNITID_TO_POS,
	CMD_ADMIN_UNASSIGN_POS_UNITID_TO_POS,
	CMD_ADMIN_READ_POS_ASSIGNED_POS_UNITID,
	CMD_ADMIN_READ_POS_UNIT_ASSIGNED_POS_ID,
	CMD_ADMIN_GET_UNIQUE_POS_ID,
//Payment Commands
	CMD_PAYMENT_START,
	CMD_PAYMENT_GET_STATUS,
	CMD_PAYMENT_CANCEL,
	CMD_PAYMENT_REFUND
} REQUESTCMD;

//define payment status
#define PAYMENT_STATUS_IDLE					10
#define PAYMENT_STATUS_ISSUED				20
#define PAYMENT_STATUS_AWAIT_CHECKIN		30
#define PAYMENT_STATUS_CANCEL				40
#define PAYMENT_STATUS_ERROR				50
#define PAYMENT_STATUS_PAYMENT_ACCEPTED		80
#define PAYMENT_STATUS_DONE					100

//define http code
#define HTTP_OK			200
#define HTTP_ERR		400
#define HTTP_AUTH_ERR	401

typedef enum _dataType {
	DATATYPE_NONE,
	DATATYPE_STRING,
	DATATYPE_INTEGER,
	DATATYPE_NUMBER
} DATATYPE;

typedef struct _packElement {
	char element[24];
	DATATYPE dataType;
	void *data;
} PACKELEMENT;

//functions
int Get_Request_Type(void);
PACKELEMENT *Get_Pack_Element(void);
int Request_Process(REQUESTCMD ePackCmd);


#endif
