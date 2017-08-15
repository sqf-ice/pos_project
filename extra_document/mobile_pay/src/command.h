/*------------------------------------------------------------
* FileName: command.h
* Author: liukai
* Date: 2016-04-19
------------------------------------------------------------*/
#ifndef COMMAND_H
#define COMMAND_H

#include "mPayExchange.h"

#define QRCODE_FORMAT		"mobilepaypos://pos?id=%15s&source=qr"

typedef struct mobilePayDataConf MOBILEPAYDATACONF;
typedef struct mobilePayDataECR MOBILEPAYDATAECR;

int Command_Boot(MOBILEPAYDATAECR *ptMPdata);
int Command_Config(MOBILEPAYDATACONF *ptMPconfig);
int Command_Payment(MOBILEPAYDATACONF *ptMPconfig, MOBILEPAYDATAECR *ptMPdata);
int Command_Refund(MOBILEPAYDATACONF *ptMPconfig);
int Command_Show_Last_Txn(void);
int Command_Execute_Pending_Operation(MOBILEPAYDATACONF *ptMPconfig, MOBILEPAYDATAECR *ptMPdata);
int Command_Unregister(MOBILEPAYDATACONF *ptMPconfig);
int Command_End_Of_Day(void);

#endif
