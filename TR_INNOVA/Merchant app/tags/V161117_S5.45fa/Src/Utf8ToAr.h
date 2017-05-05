/****************************************************************************
NAME
    Utf8ToAr.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2013.02.26      - created
****************************************************************************/
#ifndef _UTF8_TO_AR_H_
#define _UTF8_TO_AR_H_

/****************************************************************************
 Function:          convert utf8 to ar
 Param in:
   UTF8				data with utf8 encode
   LenIn			length of data
 Param out:
   Ar				data after converting
 Return Code:
****************************************************************************/
void UTF8ToAr(const unsigned char *UTF8, unsigned int LenIn, unsigned char *Ar);

#endif
