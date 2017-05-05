#ifndef PROCCO_PAX_EMV_H
#define PROCCO_PAX_EMV_H

#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "emvlib.h"

int usEMVGetTLVFromColxn(WORD usTag, BYTE *psData, WORD *pLen);


#endif

