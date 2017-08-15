/*------------------------------------------------------------
* FileName: global.h
* Author: liukai
* Date: 2016-02-29
------------------------------------------------------------*/
#ifndef GLOBAL_H
#define GLOBAL_H

/*Base*/
/*********************************************************************************
v1.00.00 initial version   by Kevin Liu	2016/05/26

*********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <osal.h>
#include "xui.h"
#include <signal.h>
#include <openssl/sha.h>
#include "curl.h"
#include "jansson.h"
#include <qrencode.h>
#include "base.h"
#include "mPayExchange.h"
#include "display.h"
#include "fileoper.h"
#include "httppacket.h"
#include "crc32.h"
#include "command.h"
#include "qrtopng.h"
#include "log.h"

#define MOBILEPAY_VERSION	"v1.00.00"

#define CURRENCY_NAME	"DKK"

//debug
/********************************************************/
//#define TEST_POSUNITID		"0089"
//#define DEBUG_LOG_OPEN
//#define DEBUG_LOG_WRITE_FILE
/********************************************************/

#define MP_OK						0
#define MP_ERR						-1
#define MP_ERR_INVALID_PARAM		-2
#define MP_ERR_TIMEOUT				-3
#define	MP_ERR_NETWORK				-4
#define MP_ERR_FILE_NOT_EXIST		-5
#define MP_ERR_FILE_INVLID			-6
#define MP_ERR_FILE_OPER			-7
#define MP_ERR_FILE_CHECKSUM		-8
#define MP_ERR_REGISTER				-9
#define MP_ERR_USERCANCEL			-10

unsigned char gl_ucConnDevice;


#endif
