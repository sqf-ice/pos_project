#ifndef _FARSI_H_
#define _FARSI_H_

#define     SHIFT_RIGHT             0x00
#define     SHIFT_CENTER            0x01
#define     SHIFT_LEFT              0x02

#define     EFT_REVERSE             0x80

#define     AR_SUCCESS              0x00
#define     AR_OPENFILE_ERROR       0xFF
#define     AR_READFILE_ERROR       0xFE
#define     AR_PARA_ERROR           0xFD


int IfArabicString(const unsigned char *str);
unsigned char ArInit(unsigned char PrnFontID);
unsigned char ArStop();
unsigned char ArShowString(unsigned char lineno, unsigned char Mode, unsigned char *str);
unsigned char ArPrintString(unsigned char Mode, unsigned char *str);
unsigned char ArChgNumType(unsigned char type);

#endif
