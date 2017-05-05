#ifndef _PAX_MIGRATE_H_
#define _PAX_MIGRATE_H_

#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.
#include "os_define.h"
#include "fstruct.h"
#include "Farsi.h"
#include "lng.h"

// #define KEY_F1          0x7a 
// #define KEY_F2          0x7b 
// #define KEY_F3          0x7c 
// #define KEY_F4          0x7d
// #define KEY_F5          0x61
// #define KEY_F7          0x63 
// #define KEY_F8          0x64
// #define KEY_ENTER       0x0D
// #define OS_KEY_CANCEL   0x54
// #define K_BACK			68		
// #define K_00			    46					
// #define K_UP			    66					
// #define K_DOWN			88					
// #define K_VOID			70					
// #define K_PVOID			65	
// #define K_ENTER			13					
// #define K_ESC			84					
// #define K_MENU			67					
// #define K_CLEAR			65					
// #define K_FUNC			140					
// #define K_PMENU			42	
// #define K_TPAD			111	


enum{ EVT_TIMER, EVT_KBD, EVT_MAG, EVT_ICC1_INS };

#define NO_CLEAR    0x01
#define CLR_LINE    0x02

void display_at(BYTE a, BYTE b, char *pszString, BYTE mode);
void write_at(char *pszString, int iLen, char xx, char yy);
void display(char *pszString);
int pax_migrate_rename(char *pszOldname, char *pszNewName);

void displayBig_at(BYTE a, BYTE b, char *pszString, BYTE mode);
void writeBig_at(char *pszString, int iLen, char xx, char yy);

void error_tone(void);

extern INT Close(INT Handle);

#endif  // #ifdef _PAX_MIGRATE_H_

// end of file
