#include <posapi.h> // use PAXPayPro2010's path, but don't copy to project folder.

#include "os_define.h"
#include "os_interface.h"
//#include "veritypes_def.h"
#include "defines.h"
#include "utility.h"
#include "display.h"



ULONG Bcd2Bin(BYTE *pBcd, BYTE Len);
void Bin2Bcd_Dword(BYTE * pBcd, ULONG Bin);
void Bin2Bcd_Word(BYTE *pBcd, UINT Bin);
void Bin2Bcd_Word(BYTE *pBcd, UINT Bin);
void Bin2Bcd_Byte(BYTE *pBcd, BYTE Bin);

extern void PubTrimTailChars(uchar *pszString, uchar ucRemoveChar);
extern void PubTrimHeadChars(uchar *pszString, uchar ucRemoveChar);


INT StrIsDigit(BYTE *Input)
{    
    if (StrLen(Input) == 0)
        return 0;
    
    while(*Input)
    {
        if (*Input < '0' || *Input > '9')
        {
            return 0;
        }
        Input++;
    }
    
    return 1;    
}

void reverse(char *s)
{
    int t;
    char *a;
    a = s + (strlen(s)-1);
    printf("the address is :%x\n",a);
    for( ; s < a ; s++,a--)
    {
        t = *s;
        *s = *a;
        *a = t;
    }
}

void itoa(int n,char *a ,int mode)
{
    int sign;
    char *b = a; /* pointer to hold the initial address */
    
    
    if((sign = n) < 0)
        n = -n;
    
    printf("the address od the array before starting is: %x\n",a);
    do
    {
        *a = n % mode + '0' ;
        a++;
    }while((n /= mode ) > 0);
    if(sign < 0)
    {
        *a = '-';
        a++;
    }
    printf("the address after reverse is:%x\n",a);
    
    reverse(b);
}

VOID StrLeftPad(BYTE *Dest,INT LenToPad,PBYTE Source,CHAR PaddingChar)
{
    BYTE TempStr[50];
    INT Len, i;
    
    memset (TempStr,0,50);
    Len = StrLen(Source);
    if (Len < LenToPad)
    {
        for(i=0;i<LenToPad;i++)
        {
            if (i<(LenToPad-Len)) TempStr[i] = PaddingChar;
            else TempStr[i] = Source[i-(LenToPad-Len)];
        }
        StrCpy(Dest,TempStr);
    }
}

VOID StrRightPad(PBYTE Dest,INT LenToPad,PBYTE Source,CHAR PaddingChar)
{
    BYTE TempStr[200];
    int Len, i;
    
    memset (TempStr,0,200);
    Len = StrLen(Source);    
    
    memcpy(TempStr,Source,Len);
        
    for (i = Len; i < LenToPad; i++ )
    {
        TempStr[i] = PaddingChar;
    }
    
    StrCpy(Dest, TempStr);
}


int StrCmp(BYTE *X, int Lx, BYTE *Y, int Ly)
{
    int i;	// first buffer index
    int j;	// second buffer index
    
    // initialisation of the buffers indexes
    i = j = 0;
    
    // if first buffer is longer than second buffer then
    if (Lx > Ly)
    {
        // suppress the unsignificant zero in the first buffer
        for (i = 0 ; (i < (Lx-Ly)) && (X[i] == 0) ; i++);
        // if first buffer is still longer then
        if (i != Lx-Ly)
            // first > second
            return (1);
    }
    
    // if second buffer is longer than first buffer then
    if (Lx < Ly)
    {
        // suppress the unsignificant zero in the second buffer
        for (j = 0 ; (j < (Ly-Lx)) && (Y[j] == 0) ; j++);
        // if second buffer is still longer then
        if (j != Ly-Lx)
            // second > first
            return (-1);
    }
    
    // here the two buffers have the same significant size
    // for each character until the end of the buffers
    for (; i < Lx ; i++ , j++)
    {
        // if char of the first buffer > char of the second buffer then
        if (X[i] > Y[j])
            // first > second
            return (1);
        // else if char of the second buffer > char of the first buffer then
        else if (X[i] < Y[j])
            // second > first
            return (-1);
    }
    // first = second if we run until here
    return (0);    
}

void AddBcd1(BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R)
{
    int i;		// operand 1 index
    int j;		// operand 2 index
    BYTE Carry;	// carry
    BYTE *A;	// operand 1 pointer
    BYTE *B;	// operand 2 pointer
    
    // if length of operand 1 >= length of operand 2 then
    if (Lx >= Ly)
    {
        // the operation will be R = X + Y
        // initialise operand 1 pointer with operand 1 address
        A = X;
        // initialise operand 1 index with operand 1 length - 1
        i = Lx-1;
        // initialise operand 2 pointer with operand 2 address
        B = Y;
        // initialise operand 2 index with operand 2 length - 1
        j = Ly-1;
    }
    // if length of operand 2 > length of operand 1 then
    else
    {
        // the operation will be R = Y + X
        // initialise operand 1 pointer with operand 2 address
        A = Y;
        // initialise operand 1 index with operand 2 length - 1
        i = Ly-1;
        // initialise operand 2 pointer with operand 1 address
        B = X;
        // initialise operand 2 index with operand 1 length - 1
        j = Lx-1;
    }
    
    // for the j last digit of the two operands
    for (Carry = 0 ; j >= 0 ; i-- , j--)
    {
        // result digit = operand 1 digit + operand 2 digit + carry
        R[i+1] = A[i] + B[j] + Carry;
        // carry = 0
        Carry = 0;
        // if result digit >= 10 then
        if (R[i+1] >= 10)
        {
            // result digit = result digit - 10
            R[i+1] -= 10;
            // carry = 1
            Carry = 1;
        }
    }
    
    // for the i-j remaining digits of the longer operand
    for (; i >= 0 ; i--)
    {
        // result digit = longer operand digit + carry
        R[i+1] = A[i] + Carry;
        // carry = 0
        Carry = 0;
        // if result digit >= 10 then
        if (R[i+1] >= 10)
        {
            // result digit = result digit - 10
            R[i+1] -= 10;
            // carry = 1
            Carry = 1;
        }
    }
    // higher result digit = carry
    R[0] = Carry;    
}

void SubstBcd1(BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R)
{
    BYTE Carry;	// carry
    int i;		// index for X operand
    int j;		// index for Y operand
    
    // for the Ly last digit of each operand
    for (Carry = 0 , i = Lx-1 , j = Ly-1 ; j >= 0 ; i-- , j--)
    {
        // if operand 1 digit >= operand 2 digit + carry then
        if (X[i] >= (Y[j] + Carry))
        {
            // result digit = operand 1 digit - (operand 2 digit + carry)
            R[i] = X[i] - (Y[j] + Carry);
            // carry = 0
            Carry = 0;
        }
        // if operand 1 digit < operand 2 digit + carry then
        else
        {
            // result digit = (operand 1 digit+10) - (operand 2 digit+carry)
            R[i] = (X[i] + 10) - (Y[j] + Carry);
            // carry = 1
            Carry = 1;
        }
    }
    
    // for the Lx-Ly digit of the first operand
    for (; i >= 0 ; i--)
    {
        // if operand 1 digit >= carry then
        if (X[i] >= Carry)
        {
            // result digit = operand 1 digit - carry
            R[i] = X[i] - Carry;
            // carry = 0
            Carry = 0;
        }
        // if operand 1 digit < carry then
        else
        {
            // result digit = (operand 1 digit + 10) - carry
            R[i] = (X[i] + 10) - Carry;
            // carry = 1
            Carry = 1;
        }
    }    
}

void MulBcd1(BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R)
{
    int i;			// first operand index
    int j;			// second operand index
    BYTE Carry;		// carry
    
    // initialisation of the result with 0
    for (i = 0 ; i < (Lx+Ly) ; R[i] = 0 , i++);
    
    // for each operand 2 digit
    for (i = Ly-1 ; i >= 0 ; i--)
    {
        // for each operand 1 digit
        for (Carry = 0 , j = Lx-1 ; j >= 0 ; j--)
        {
            // result digit = (op 2 digit * op 1 digit) + carry + result digit
            R[i+j+1] += (Y[i] * X[j]) + Carry;
            // carry = 0
            Carry = 0;
            // while result digit >= 10
            while (R[i+j+1] >= 10)
            {
                // result digit = result digit - 10
                R[i+j+1] -= 10;
                // carry = carry + 1
                Carry++;
            }
        }
        R[i] = Carry;
    }
}

void ConvAscBcd1(BYTE *fpAsc, BYTE *fpBcd1, WORD fpLength)
{
    WORD lbCounter;
    
    for (lbCounter=0;lbCounter<fpLength;lbCounter++)
        fpBcd1[lbCounter]=fpAsc[lbCounter]-0x30;
    
    return;
}

void ConvBcd1Asc(BYTE *fpBcd1, BYTE *fpAsc, WORD fpLength)
{
    WORD lbCounter;
    
    for (lbCounter=0;lbCounter<fpLength;lbCounter++)
        fpAsc[lbCounter]=fpBcd1[lbCounter]+0x30;
    return;
}

WORD AscLen(BYTE *fpAsc, WORD fwLength)
{
    WORD lbCounter;
    for (lbCounter=0;lbCounter<fwLength;lbCounter++)
        if (fpAsc[lbCounter]<'0' || fpAsc[lbCounter]>'9')
            break;
        return(lbCounter);
}

void AddAscii(BYTE *fpAsc1,WORD fwLen1,BYTE *fpAsc2,WORD fwLen2,BYTE *fpResult, WORD fwLenRet)
{
    WORD lwLen1;         		// Length of first number
    WORD lwLen2;               // Length of second number
    BYTE lpResultBcd1[20];     // Result in BCD1
    BYTE lpResultAsc[20];		// Result in Ascii
    
    BYTE lpBcd11[20];				// First number in BCD1
    BYTE lpBcd12[20];          // Second number in BCD1
    WORD lwLenRet;					// Length of result
    WORD lwCounter;            // Used for loops
    
    // Convert numbers to BCD1
    lwLen1=AscLen(fpAsc1,fwLen1);
    lwLen2=AscLen(fpAsc2,fwLen2);
    ConvAscBcd1(fpAsc1,lpBcd11,lwLen1);
    ConvAscBcd1(fpAsc2,lpBcd12,lwLen2);
    // Add numbers
    AddBcd1(lpBcd11, lwLen1, lpBcd12, lwLen2, lpResultBcd1);
    // Calculate the length of the result
    if (lwLen1>lwLen2)
        lwLenRet=lwLen1+1;
    else
        lwLenRet=lwLen2+1;
    // Convert result to ascii
    ConvBcd1Asc(lpResultBcd1, lpResultAsc, lwLenRet);
    lpResultAsc[lwLenRet]=0;
    
    // Clear insignificant 0s and right pad result with blanks
    L1SupZeros(lpResultAsc);
    lwLenRet=StrLen(lpResultAsc);
    for (lwCounter=lwLenRet;lwCounter<fwLenRet;lwCounter++)
        lpResultAsc[lwCounter]=' ';
    memcpy(fpResult,lpResultAsc,fwLenRet);
    return;
}

void SubstAscii(BYTE *fpAsc1,WORD fwLen1,BYTE *fpAsc2,WORD fwLen2,BYTE *fpResult, WORD fwLenRet)
{
    WORD lwLen1;         		// Length of first number
    WORD lwLen2;               // Length of second number
    BYTE lpResultBcd1[20];     // Result in BCD1
    BYTE lpResultAsc[20];		// Result in Ascii
    
    BYTE lpBcd11[20];				// First number in BCD1
    BYTE lpBcd12[20];          // Second number in BCD1
    WORD lwLenRet;					// Length of result
    WORD lwCounter;            // Used for loops
    int  Status;
    
    // Convert numbers to BCD1
    lwLen1=AscLen(fpAsc1,fwLen1);
    lwLen2=AscLen(fpAsc2,fwLen2);
    ConvAscBcd1(fpAsc1,lpBcd11,lwLen1);
    ConvAscBcd1(fpAsc2,lpBcd12,lwLen2);
    // Compare numbers
    Status = StrCmp(lpBcd11,lwLen1,lpBcd12,lwLen2);
    // Get sign and substract numbers
    switch (Status)
    {
    case 0 :			// Acs1=Asc2
        memset(lpResultBcd1,0,20);
        lwLenRet=lwLen1;
        break;
        
    case 1 :			// Asc1>Asc2
        SubstBcd1(lpBcd11, lwLen1, lpBcd12, lwLen2, lpResultBcd1);
        lwLenRet=lwLen1;
        break;
        
    case -1:			// Asc1<Asc2
        SubstBcd1(lpBcd12, lwLen2, lpBcd11, lwLen1, lpResultBcd1);
        lwLenRet=lwLen2;
        break;
    }
    
    // Convert result to ascii
    ConvBcd1Asc(lpResultBcd1, lpResultAsc, lwLenRet);
    lpResultAsc[lwLenRet]=0;
    // Clear insignificant 0s and right pad result with blanks
    L1SupZeros(lpResultAsc);
    lwLenRet=StrLen(lpResultAsc);
    for (lwCounter=lwLenRet;lwCounter<fwLenRet;lwCounter++)
        lpResultAsc[lwCounter]=' ';
    memcpy(fpResult,lpResultAsc,fwLenRet);
    return;
}


int AsciiCmp(BYTE *fpAsc1, WORD fwLen1,BYTE *fpAsc2, WORD fwLen2)
{
    BYTE lpBcd11[20];		// First number in BCD1
    BYTE lpBcd12[20];    // Second number in BCD1
    WORD lwLen1;         // Length of first number
    WORD lwLen2;         // Length of second number
    int	 Status;			// Return status
    
    // Convert numbers to BCD1
    lwLen1=AscLen(fpAsc1,fwLen1);
    lwLen2=AscLen(fpAsc2,fwLen2);
    ConvAscBcd1(fpAsc1,lpBcd11,lwLen1);
    ConvAscBcd1(fpAsc2,lpBcd12,lwLen2);
    // Compare numbers
    Status = StrCmp(lpBcd11,lwLen1,lpBcd12,lwLen2);
    return(Status);
}

void ConvAscToBcd(BYTE *fpAsc, BYTE *fpBcd1, WORD fpLength)
{
    WORD lbCounter;
    BYTE fpAsc1[20];
    
    lbCounter=0;
    
    if ((fpLength/2)*2 != fpLength)
    {
        memset(fpAsc1,'0',20);
        memcpy(fpAsc1+1, fpAsc, fpLength);
        memcpy(fpAsc, fpAsc1, fpLength+1);
    }
    
    do
    {
        fpBcd1[lbCounter/2]=( (fpAsc[lbCounter]-'0')*10+(fpAsc[lbCounter+1]-'0') )+0x1C;
        lbCounter +=2;
    }
    while( lbCounter<fpLength );
}

void ConvDecToHexStr(unsigned long DecDebit,BYTE *HexDebit)
{
    DWORD ldwTemp;
#pragma warning(disable: 4244) // Added by Kim_LinHB 2013-3-11 build 0.05.0009
    ldwTemp = DecDebit>> 24;
    HexDebit[0] = ldwTemp;
    ldwTemp = DecDebit>> 16;
    HexDebit[1] = ldwTemp;
    ldwTemp = DecDebit>> 8;
    HexDebit[2] = ldwTemp;
    HexDebit[3] = DecDebit;
#pragma warning(default: 4244) // Added by Kim_LinHB 2013-3-11 build 0.05.0009
}

void ConvDate(BYTE *fpBuffer)
{
    WORD Week[1],Day[1],Month[1],Year[1];
    
    // Date
    OS_ClkReadDate(Week,Day,Month,Year);
    
    fpBuffer[0] = (Day[0]>>4) + '0';//(Day[0]/10)+'0';
    fpBuffer[1] = (Day[0] & 0x0f) + '0';//(Day[0]%10)+'0';
    fpBuffer[2] = '/';
	// Modified by Kim_LinHB 2013-10-8 v0.05.0012
	fpBuffer[3] = (Month[0]>>4)+'0';//(Month[0]/10)+'0';
	fpBuffer[4] = (Month[0]& 0x0f)+'0';//(Month[0]%10)+'0';
    fpBuffer[5] = '/';
    fpBuffer[6] = (Year[0]/1000)+'0';
    fpBuffer[7] = ((Year[0]%1000)/100) +'0';
    fpBuffer[8] = (((Year[0]%1000)%100)/10) +'0';
    fpBuffer[9] = (((Year[0]%1000)%100)%10) +'0';
    fpBuffer[10]= 0;
}

BYTE HexToAsc(BYTE *fpHex,BYTE *fpAsc,BYTE Length)
{
    int i=0;
    
    for(i=0;i<Length;i++)
    {
        if(fpHex[i]/0x10+'0'>57)
            fpAsc[i*2]=fpHex[i]/0x10+'0'+ 7;
        else
            fpAsc[i*2]=fpHex[i]/0x10+'0';
        
        if(fpHex[i]%0x10+'0'>57)
            fpAsc[(i*2)+1]=fpHex[i]%0x10+'0'+7;
        else
            fpAsc[(i*2)+1]=fpHex[i]%0x10+'0';
    }
    
    return(OS_TRUE);
}

void AscToHex(char *fpStrAscii, char *fpStrBin,int fbLen)
{
    int lbNumber;
    int liCounter;
    
    for (liCounter =0; liCounter<fbLen; liCounter++)
    {
        if(fpStrAscii[liCounter*2]>57)	//大于num9
        {
            if(fpStrAscii[liCounter*2+1]>57)	//大于num9
                lbNumber = ((fpStrAscii[liCounter*2]-'7')*16)+(fpStrAscii[liCounter*2+1]-'7');
            else
                lbNumber = ((fpStrAscii[liCounter*2]-'7')*16)+(fpStrAscii[liCounter*2+1]-'0');
        }
        else
        {
            if(fpStrAscii[liCounter*2+1]>57)
                lbNumber = ((fpStrAscii[liCounter*2]-'0')*16)+(fpStrAscii[liCounter*2+1]-'7');
            else
                lbNumber = ((fpStrAscii[liCounter*2]-'0')*16)+(fpStrAscii[liCounter*2+1]-'0');
        }
        fpStrBin[liCounter]=lbNumber;
    }
}

void  ConvAsciiToBcd(BYTE * source, BYTE * dest , BYTE len)
{
    int i,k;
    for(i=0;i<len;i++)
    {
        k=(source[i*2]-'0')*10+(source[i*2+1]-'0');
        dest[i]=k;
    }
}


void StrAscToBcd(BYTE *fpAsc, BYTE *fpBcd1)
{
    INT Len;
    
    Len = StrLen(fpAsc);
    ConvAscToBcd(fpAsc, fpBcd1, Len);   
}

//by jarod 20110805
void StrAscToHex(BYTE *fpAsc, BYTE *fpHex1)
{
    INT Len;
    
    Len = StrLen(fpAsc);
	AscToHex(fpAsc, fpHex1, Len);	
}

WORD L1ConvAscInt( BYTE *fsStr )
{
    BYTE i;
    WORD tens;
    WORD Buf=0;
    
    if(StrLen(fsStr)>0)
    {
        tens = 1;
        
        for (i=0; i<StrLen(fsStr)-1; i++)
            tens*=10;
        
        for (i=0; i<StrLen(fsStr); i++)
        {
            Buf+=(fsStr[i]-'0')*tens;
            tens/=10;
        }
    }
    return(Buf);
}

void L1ConvIntAsc( WORD fwVal, BYTE *fsDest )
{
    BYTE i;
    WORD result;
    
    result = fwVal;
    i = 0;
    do
    {
        fsDest[i++] = result%10 + '0';
        result /= 10;
        
    } while ( result != 0 );
    
    fsDest[i] = 0;
    strrev((char *)fsDest);
}


void L1CadreStr( BYTE *fsStr, WORD fwLgRes )
{
    if (fsStr[0] == 0)
    {
        memset(fsStr,'0',fwLgRes);
        memset(fsStr+fwLgRes,0,1);
    }
    StrLeftPad(fsStr, fwLgRes+1, fsStr, '0');
}
void L1SupZeros( BYTE *fsStr )
{
    int position;
    BYTE fsStr1[255];
    
    StrCpy(fsStr1,fsStr);
    memset(fsStr,0,StrLen(fsStr));
    position = strspn((const char *)fsStr1,"0");
    StrCpy(fsStr,fsStr1+position);
    
}

void DisplayString(BYTE *Str, WORD Timeout)
{
    BYTE fpMess1[30]={0}, fpMess2[30]={0};
    BYTE Len;    

    Len = StrLen(Str);    
    
    if (StrLen(Str) < 21)
    {
        memcpy(fpMess1, Str, Len);
    }
    else
    {
        memcpy(fpMess1, Str, 21);
        memcpy(fpMess2, &Str[21], Len-21);
    }
    
    OS_ClearScreen();    
    OS_DisplayStr(3, STR_CENTER, (BYTE *)fpMess1);
    OS_DisplayStr(4, STR_CENTER, (BYTE *)fpMess2);
    DelayMs(Timeout);
}
void DisplayLongString(BYTE *Str, WORD Timeout)
{
	BYTE fpMess1[30]={0}, fpMess2[30]={0}, fpMess3[30]={0};
	BYTE Len;    

	Len = StrLen(Str);    

	if (Len <= 21)
	{
		memcpy(fpMess1, Str, Len);
	}
	else if(Len<42)
	{
		memcpy(fpMess1, Str, 21);
		memcpy(fpMess2, &Str[21], Len-21);
	}
	else 
	{
		memcpy(fpMess1, Str, 21);
		memcpy(fpMess2, &Str[21], 21);
		memcpy(fpMess3, &Str[42], Len-42);
	}

	OS_ClearScreen();    
	OS_DisplayStr(3, STR_CENTER, (BYTE *)fpMess1);
	OS_DisplayStr(4, STR_CENTER, (BYTE *)fpMess2);
	OS_DisplayStr(5, STR_CENTER, (BYTE *)fpMess3);
	DelayMs(Timeout);
}
void DisplayString_S80(BYTE *Str, WORD Timeout)
{
    BYTE fpMess1[30]={0}, fpMess2[30]={0};
    BYTE Len;    
    
	OS_ClearScreen();

    Len = StrLen(Str);
    if (StrLen(Str) > 16)
    {
		ScrPrint(0, 3, CFONT, "%.16s", Str);
		ScrPrint((128-(Len-16)*8)/2, 5, CFONT, "%.16s", Str+16);
    }
    else
    {
		ScrPrint((128-Len*8)/2, 3, CFONT, "%.16s", Str);
    }

    DelayMs(Timeout);
}
void DisplayLongString_S80(BYTE *Str, WORD Timeout)
{
	BYTE fpMess1[30]={0}, fpMess2[30]={0}, fpMess3[30]={0};
	BYTE Len;    

	OS_ClearScreen();

	Len = StrLen(Str);

	if (Len > 32)
	{
		memcpy(fpMess1, Str, 16);
		memcpy(fpMess2, &Str[16], 16);
		memcpy(fpMess3, &Str[32], 16);

		ScrPrint(0, 1, CFONT, "%.16s", fpMess1);
		ScrPrint(0, 3, CFONT, "%.16s", fpMess2);
		ScrPrint(0, 5, CFONT, "%.16s", fpMess3);
		DelayMs(Timeout);
		return;
	}
	DelayMs(Timeout);
}
/*
// 改写之后再开放
void lPrtSpace(BYTE fbNb)
{
    while (fbNb-- > 0)
    {
        PrnStr(" ");
    }
}
*/

// 改写之后再开放
WORD lPrtFF(BYTE fbNb)
{
    // "\r" -->> "\n"    
    while (fbNb-- > 0)
    {
        PrnStr("\n");
    }
    return(STAT_OK);
}

void AmountSeparation(BYTE *fpAmount, BYTE *fpFJAmount)
{
    char lpAmount[LG_AMOUNT+3];  	// Amount
    char buff[32], *pIn, *pOut;
    int ii;

    
    memcpy(lpAmount,fpAmount, LG_AMOUNT);
    lpAmount[LG_AMOUNT]= 0;
    PubTrimTailChars(lpAmount, ' ');
    if (strlen(lpAmount)<LG_AMOUNT-4)
    {
        StrLeftPad(lpAmount, LG_AMOUNT, lpAmount, '0');
    }

    pIn = &lpAmount[0];
    for (ii=0; ii<LG_AMOUNT-4; ii++)    // at least leave 0.000
    {
        if (*pIn=='0')
        {
            pIn++;
        }
        else
        {
            break;
        }
    }
    
    memmove(lpAmount, pIn, LG_AMOUNT+1-(pIn-lpAmount));
    pIn = lpAmount+strlen(lpAmount)-1;

    memset(buff, ' ', sizeof(buff));
    pOut  = buff+sizeof(buff)-1;
    *pOut = 0;
    pOut--;

    for (ii=0; ii<(int)strlen(lpAmount); ii++)
    {
        *pOut = *pIn;
        if (ii==2)
        {
            pOut--;
            *pOut = '.';
        }
        if ((ii==5 || ii==8 || ii==11) && (ii!=(int)(strlen(lpAmount)-1)))
        {
            pOut--;
            *pOut = ',';
        }
        pOut--;
        pIn--;
    }

    PubTrimHeadChars(buff, ' ');
    StrCpy(fpFJAmount, buff);

    // BYTE BufTmp[MAX_DISP_WIDTH + 1];
    // BYTE BufZero[LG_AMOUNT + 2];
    // BYTE Param[17];
    // WORD NbChar;
    // WORD Len;
    // AMOUNT_CONFIG AmtCfg;
    // 
    // 	AmtCfg.Separator = ',';
    // 	if((gCurrencyExponent==2)||(gCurrencyExponent==3))
    // 		AmtCfg.NbDeci 	  = gCurrencyExponent;
    // 	else
    // 		AmtCfg.NbDeci 	  = 3;
    // 	AmtCfg.DeciPoint = '.';
    // 	AmtCfg.LgMax 	  = 10;
    // 	StrCpy(AmtCfg.CurrencySymbol,gCurrencySymbol);
    // 	AmtCfg.CurrencySymbol[3]=0;
    // 
    // 	fPtrChaine[12]=0;
    // 
    // StrTrim(fPtrChaine,fPtrChaine,' ');
    // StrLeftPad(fPtrChaine,LG_AMOUNT+1,fPtrChaine,'0');
    // 
    // 	StrCpy(BufTmp, fPtrChaine);
    // 	NbChar = StrLen(fPtrChaine);
    // 
    // 	Len = NbChar - (WORD) AmtCfg.NbDeci;
    // 	ManSprintfn(Param, sizeof(Param), "%%.%ds%c%%.%ds",
    // 				 Len, AmtCfg.DeciPoint, AmtCfg.NbDeci);
    // 	ManSprintf(fOutChaine, Param, BufTmp,        // Cannot use ManSprintfn
    // 				BufTmp + NbChar - AmtCfg.NbDeci);
    // 	// Suppress the zeros not significant
    // 	BufTmp[0] = 0;
    // 	ManSscanf(fOutChaine, "%[0]%s", BufZero, BufTmp);
    // 	StrCpy(fOutChaine, BufTmp);
    // 	if ((AmtCfg.NbDeci) && (StrLen(BufTmp) < (AmtCfg.NbDeci + 2)))
    // 	{
    // 		ManSprintf(fOutChaine, "0%s", BufTmp);       // Cannot use ManSprintfn
    // 	}
    // 	if (fOutChaine[0] == 0)
    // 	{
    // 		fOutChaine[0] = '0';
    // 		fOutChaine[1] = 0;
    // 	}
    // 	ManSprintfn(BufTmp, sizeof(BufTmp), "%s%s", fOutChaine,
    // 				 AmtCfg.CurrencySymbol);
    // 	StrCpy(fOutChaine, BufTmp);
    // 	StrLeftPad(fOutChaine, 17, fOutChaine, ' ');
    // 
    // 	fOutChaine[16]=0;
}

WORD PLenStr(BYTE *fpStr, BYTE fbLen)
{
    if (fbLen)
        fbLen--;
    
    while(fpStr[fbLen]==' '|| fpStr[fbLen]==0)
    {
        if ( (fbLen--) == 0)
            return(0);
    }
    return(fbLen+1);
}

void PIncreaseDate(BYTE *fpDate, WORD fwDays)
{
    BYTE labDaysOfMonths[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    WORD lwDay;
    WORD lwMonth;
    WORD lwYear;
    
    lwDay = (fpDate[4]-'0')*10 + fpDate[5]-'0';
    lwMonth = (fpDate[2]-'0')*10 + fpDate[3]-'0';
    lwYear = (fpDate[0]-'0')*10 + fpDate[1]-'0';
    
    if ( (lwDay>31) || (lwMonth > 12) || (lwYear > 99) )
        return;
    
    if (!(lwYear&3))
        labDaysOfMonths[1]=29;
    else
        labDaysOfMonths[1]=28;
    
    lwDay += fwDays;
    while (lwDay > labDaysOfMonths[lwMonth-1])
    {
        lwDay -= labDaysOfMonths[lwMonth-1];
        lwMonth++;
        if (lwMonth==13)
        {
            lwMonth = 1;
            lwYear ++;
            if (lwYear == 100)
                lwYear = 0;
        }
        if (!(lwYear%3))
            labDaysOfMonths[1]=29;
        else
            labDaysOfMonths[1]=28;
    }
    
    fpDate[4]=lwDay/10 + '0';
    fpDate[5]=lwDay%10 + '0';
    fpDate[2]=lwMonth/10 + '0';
    fpDate[3]=lwMonth%10 + '0';
    fpDate[0]=lwYear/10 + '0';
    fpDate[1]=lwYear%10 + '0';
}

void Utils_DebugPrint(BYTE *header, void *data, WORD len)
{    
    if (gDebugType == DEBUG_PRINTER || gDebugType == DEBUG_PORT_AND_PRINTER)
    {
        OS_LogToHex((char *)header,data,len, DEBUG_PRINTER);
    }
    
    if (gDebugType == DEBUG_PORT || gDebugType == DEBUG_PORT_AND_PRINTER)
    {        
        OS_LogToHex(( char *)header, data,len, DEBUG_PORT);
    }
}

void Utils_DebugPrintMsg(char *pszData)
{
	PrnInit();
    PrnStr(pszData);
    PrnStr("\n\n\n\n\n");
	StartPrinter();
}

BYTE *Utils_ConvertBcdToBinary(BYTE *in, WORD len, BYTE isWord)
{
    unsigned long a;
    WORD b;
    BYTE out[5];
    
    if (isWord)
    {
        Utils_ConvertBcdToAsc(in, out, len);
        out[2*len]=0;
        b = atoi(out);
        memcpy (out, (BYTE *)&b, 2);
        out[2]=out[0];
        out[0]=out[1];
        out[1]=out[2];
        out[2]=0;
    }
    else a = SWAPLONG(Utils_ConvertBcdToULong(in,len,OS_FALSE));
    {
        memcpy(out,(BYTE *)&a, 4);
    }
    return (&out[0]);
}

unsigned long Utils_ConvertBcdToULong(BYTE *BCD, BYTE len, BYTE isCompressed)
{
    BYTE buffer[50];
    
    if (isCompressed)
    {
    }
    Utils_ConvertBcdToAsc(BCD, buffer, len);
    buffer[2*len]=0;
    return (atoul(buffer));
}

void Utils_ConvertAscToBcd(BYTE *ASC, BYTE *BCD, unsigned int fpLength,unsigned int lenOut, BYTE padType, BYTE padChar)
{
    unsigned int lbCounter,Length;
    unsigned int t=0;
    unsigned char ASCIIBuffer[600];
    unsigned char sub1,sub2;
    
    memset(ASCIIBuffer, padChar , sizeof(ASCIIBuffer));
    
    if ( lenOut == 0 ) // Auto Mode! if input length is odd then pad input with padChar
    {
        if (fpLength % 2)
            Length = fpLength + 1;
        else
            Length = fpLength;
    }
    else
    {
        t = lenOut*2 - fpLength;
        Length = 2*lenOut;
    }
    
    if ( padType == PAD_LEFT )
    {
        memcpy( ASCIIBuffer+t, ASC, fpLength );
    }
    else if ( padType == PAD_RIGHT )
    {
        memcpy( ASCIIBuffer, ASC, fpLength );
    }
    else  // treat as PAD_NONE
    {
        t=fpLength % 2;
        if (t == 1) ASCIIBuffer[0]=0x30;
        memcpy(ASCIIBuffer+t,ASC,fpLength);
        Length= fpLength+t;
    }
    for (lbCounter=0;(2*lbCounter)<Length;lbCounter++)
    {
        if (ASCIIBuffer[2*lbCounter]>0x39) sub1=0x37; else sub1=0x30;
        if (ASCIIBuffer[2*lbCounter+1]>0x39) sub2=0x37; else sub2=0x30;
        BCD[lbCounter]=((ASCIIBuffer[2*lbCounter]-sub1)<<4)|(ASCIIBuffer[2*lbCounter+1]-sub2);
    }
}

unsigned long atoul(BYTE *buffer)
{
    unsigned long multiplier=1;
    unsigned long result=0;
    int u;
    
    if (StrLen(buffer)>20)
        return(0);
    u = StrLen(buffer)-1;
    for (u=(StrLen(buffer)-1);u>=0;u--)
    {
        result+= (buffer[u]-0x30)*multiplier;
        multiplier *= 10;
    }
    return (result);
}
void Utils_ConvertBcdToAsc(BYTE *BCD, BYTE *pASCII, unsigned int fpLength)
{
    unsigned int lbCounter;
    
    if (fpLength*2 > 512)
        return; /* Stack Protection */
    
    for (lbCounter=0;lbCounter<fpLength;lbCounter++)
    {
        if ((BCD[lbCounter]>>4)<0x0A) pASCII[2*lbCounter]=(BCD[lbCounter]>>4)+0x30;
        else pASCII[2*lbCounter]=(BCD[lbCounter]>>4)+0x37;
        
        if ((BCD[lbCounter]&0x0F)<0x0A) pASCII[2*lbCounter+1]=(BCD[lbCounter]&0x0F)+0x30;
        else pASCII[2*lbCounter+1]=(BCD[lbCounter]&0x0F)+0x37;
    }
}


char *strrev(char *str)
{
    char *p1, *p2;
    
    if (! str || ! *str)
        return str;
    for (p1 = str, p2 = str + StrLen((PBYTE)str) - 1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}

void StrBcdToAsc(INT SourceLen, BYTE *Source, BYTE *Dest)
{
    
    Utils_ConvertBcdToAsc(Source, Dest, SourceLen);
    
}
void RTrim(PCHAR str,CHAR ch)
{
    int  len;
    len=StrLen((PBYTE)str);
    while (*(str+len-1) == ch && len>=0) len--;
    *(str+len)=0;
}

void LTrim(PCHAR str, CHAR ch)
{
    int  i=0;
    while (*(str+i) == ch) i++;
    while (*(str+i)) {
        *str=*(str+i);
        str++;
    }
    *str=0;
}
void StrTrim(PBYTE Source,PBYTE Dest, CHAR ch)
{
    BYTE szTemp[100]={0};
    
    StrCpy(szTemp, Source);
    
    LTrim((PCHAR)szTemp,ch);
    RTrim((PCHAR)szTemp,ch);
    StrCpy(Dest, szTemp);
    
    
    
}
VOID AddBcdNum(PBYTE Input1,PBYTE Input2, PBYTE OutBuffer, INT Len)
{
    
    Util_AddBcd(Input1, Len, Input2, Len, OutBuffer);
    
}
VOID Util_AddBcd(PBYTE X, INT Lx, PBYTE Y, INT Ly, PBYTE R)
{
    INT i;		/* operand 1 index */
    INT j;		/* operand 2 index */
    BYTE Carry;	/* carry */
    PBYTE A;	/* operand 1 pointer */
    PBYTE B;	/* operand 2 pointer */
    
    /* if length of operand 1 >= length of operand 2 then */
    if (Lx >= Ly)
    {
        /* the operation will be R = X + Y */
        /* initialise operand 1 pointer with operand 1 address */
        A = X;
        /* initialise operand 1 index with operand 1 length - 1 */
        i = Lx-1;
        /* initialise operand 2 pointer with operand 2 address */
        B = Y;
        /* initialise operand 2 index with operand 2 length - 1 */
        j = Ly-1;
    }
    /* if length of operand 2 > length of operand 1 then */
    else
    {
        /* the operation will be R = Y + X */
        /* initialise operand 1 pointer with operand 2 address */
        A = Y;
        /* initialise operand 1 index with operand 2 length - 1 */
        i = Ly-1;
        /* initialise operand 2 pointer with operand 1 address */
        B = X;
        /* initialise operand 2 index with operand 1 length - 1 */
        j = Lx-1;
    }
    
    /* for the j last digit of the two operands */
    for (Carry = 0 ; j >= 0 ; i-- , j--)
    {
        /* result digit = operand 1 digit + operand 2 digit + carry */
        R[i+1] = A[i] + B[j] + Carry;
        /* carry = 0 */
        Carry = 0;
        /* if result digit >= 10 then */
        if (R[i+1] >= 10)
        {
            /* result digit = result digit - 10 */
            R[i+1] -= 10;
            /* carry = 1 */
            Carry = 1;
        }
    }
    
    /* for the i-j remaining digits of the longer operand */
    for (; i >= 0 ; i--)
    {
        /* result digit = longer operand digit + carry */
        R[i+1] = A[i] + Carry;
        /* carry = 0 */
        Carry = 0;
        /* if result digit >= 10 then */
        if (R[i+1] >= 10)
        {
            /* result digit = result digit - 10 */
            R[i+1] -= 10;
            /* carry = 1 */
            Carry = 1;
        }
    }
    /* higher result digit = carry */
    R[0] = Carry;
}
int Common_StrCmp(char *X, int Lx, char *Y, int Ly)
{
    int i;	/* first buffer index */
    int j;	/* second buffer index */
    
    i = j = 0;
    /* if first buffer is longer than second buffer then */
    if (Lx > Ly)
    {
        /* suppress the unsignificant zero in the first buffer */
        for (i = 0 ; (i < (Lx-Ly)) && (X[i] == 0) ; i++);
        /* if first buffer is still longer then */
        if (i != Lx-Ly)
            /* first > second */
            return (1);
    }
    
    /* if second buffer is longer than first buffer then */
    if (Lx < Ly)
    {
        /* suppress the unsignificant zero in the second buffer */
        for (j = 0 ; (j < (Ly-Lx)) && (Y[j] == 0) ; j++);
        /* if second buffer is still longer then */
        if (j != Ly-Lx)
            /* second > first */
            return (-1);
    }
    
    /* here the two buffers have the same significant size
    for each character until the end of the buffers */
    for (; i < Lx ; i++ , j++)
    {
        /* if char of the first buffer > char of the second buffer then */
        if (X[i] > Y[j])
            /* first > second */
            return (1);
        /* else if char of the second buffer > char of the first buffer then */
        else if (X[i] < Y[j])
            /* second > first */
            return (-1);
    }
    /* first = second if we run until here */
    return (0);
}
VOID Common_SubstBcd1(BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R)
{
    BYTE Carry;
    int i;		/* index for X operand */
    int j;		/* index for Y operand */
    
    /* for the Ly last digit of each operand */
    for (Carry = 0 , i = Lx-1 , j = Ly-1 ; j >= 0 ; i-- , j--)
    {
        /* if operand 1 digit >= operand 2 digit + carry then */
        if (X[i] >= (Y[j] + Carry))
        {
            /* result digit = operand 1 digit - (operand 2 digit + carry) */
            R[i] = X[i] - (Y[j] + Carry);
            /* carry = 0 */
            Carry = 0;
        }
        /* if operand 1 digit < operand 2 digit + carry then */
        else
        {
            /* result digit = (operand 1 digit+10) - (operand 2 digit+carry) */
            R[i] = (X[i] + 10) - (Y[j] + Carry);
            /* carry = 1 */
            Carry = 1;
        }
    }
    
    /* for the Lx-Ly digit of the first operand */
    for (; i >= 0 ; i--)
    {
        /* if operand 1 digit >= carry then */
        if (X[i] >= Carry)
        {
            /* result digit = operand 1 digit - carry */
            R[i] = X[i] - Carry;
            /* carry = 0 */
            Carry = 0;
        }
        /* if operand 1 digit < carry then */
        else
        {
            /* result digit = (operand 1 digit + 10) - carry */
            R[i] = (X[i] + 10) - Carry;
            /* carry = 1 */
            Carry = 1;
        }
    }
}
BYTE Common_SimpleDivBcd1(BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R)
{
    BYTE Res[50];	/* process buffer pointer */
    BYTE Q;		/* quotient */
    int i;		/* buffer index for transfers */
    
    Q = 0;
    
    /* copy the dividand in the remaining and in the process buffer */
    for (i = 0 ; i < Lx ; Res[i] = R[i] = X[i] , i++);
    
    /* while dividand >= divisor */
    while (Common_StrCmp((char *)Res, Lx, (char *)Y, Ly) >= 0)
    {
        /* remainig = dividand - divisor */
        Common_SubstBcd1(Res, Lx, Y, Ly, R);
        /* quotient = quotient + 1 */
        Q++;
        /* dividand = remaining */
        for (i = 0 ; i < Lx ; Res[i] = R[i] , i++);
    }
    return (Q);
}

INT Util_DivBcd(BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *Q)
{
    int i;		/* buffer index for transfer */
    int Ld;		/* dividand length */
    BYTE R[50];	/* remaining buffer address */
    
    /* initialise the quotient with 0 */
    for (i = 0 ; i < Lx ; Q[i] = 0 , i++);
    
    /* suppression of the unsignificant digit of the dividand */
    while (!(*X) && (Lx))
    {
        X++;
        Lx--;
        Q++;
    }
    
    /* suppression of the unsignificant digit of the divisor */
    while (!(*Y) && (Ly))
    {
        Y++;
        Ly--;
    }
    
    /* if the divisor = 0 then */
    if (!Ly)
        /* return error division by zero */
        return (STAT_NOK);
    
    /* the quotient has Ly-1 left zero */
    Q += Ly-1;
    
    /* process length of the dividand = length of the divisor */
    Ld = Ly;
    
    /* while dividand >= divisor */
    while (Common_StrCmp((char *)X, Lx, (char *)Y, Ly) >= 0)
    {
        /* quotient digit = simple division of X by Y */
        *Q = Common_SimpleDivBcd1(X, Ld, Y, Ly, R);
        /* transfer the remaining of the simple division in the dividand */
        for (i = 0 ; i < Ld ; X[i] = R[i] , i++);
        /* increase length of the divisor and quotient pointer */
        Ld++;
        Q++;
    }
    return (STAT_OK);
}
void Util_MulBcd(BYTE *X, int Lx, BYTE *Y, int Ly, BYTE *R)
{
    int i;			/* first operand index */
    int j;			/* second operand index */
    BYTE Carry;		/* carry */
    
    /* initialisation of the result with 0 */
    for (i = 0 ; i < (Lx+Ly) ; R[i] = 0 , i++);
    
    /* for each operand 2 digit */
    for (i = Ly-1 ; i >= 0 ; i--)
    {
        /* for each operand 1 digit */
        for (Carry = 0 , j = Lx-1 ; j >= 0 ; j--)
        {
            /* result digit = (op 2 digit * op 1 digit) + carry + result digit */
            R[i+j+1] += (Y[i] * X[j]) + Carry;
            /* carry = 0 */
            Carry = 0;
            /* while result digit >= 10 */
            while (R[i+j+1] >= 10)
            {
                /* result digit = result digit - 10 */
                R[i+j+1] -= 10;
                /* carry = carry + 1 */
                Carry++;
            }
        }
        R[i] = Carry;
    }
}

unsigned char Util_AsciiBcdBin(unsigned char b)
{
    if (b >= 0x30 && b <= 0x3f) return b-0x30;
    return 0x10;
}


void Util_ConvAscBcd(PCHAR Input, PBYTE Result, INT Length)
{
    int i;
    unsigned char x;
    
    for (i=0; i < Length; i++)
    {
        x=Util_AsciiBcdBin(Input[i]);
        if (x >= 0x0f) break;
        if (i&1)
        {
            Result[i/2] &= 0xf0; Result[i/2] |= x;
            if (i<Length-1) Result[i/2+1]=0xf0;
        }
        else { Result[i/2] = (x<<4)|0x0f; }
    }
    
}

VOID DivBcdNum(PBYTE Input1,DWORD Input2, PBYTE OutBuffer, INT Len)
{
    
    BYTE szTemp[15]={0};
    BYTE szInput2[15]={0};
    INT iLen2;
    
    sprintf((PCHAR)szTemp,"%lu",Input2);
    iLen2 = StrLen(szTemp);
    Util_ConvAscBcd((PCHAR)szTemp, szInput2, iLen2);
    iLen2 = StrLen(szInput2);
    
    Util_DivBcd(Input1, Len,szInput2, iLen2,OutBuffer);
    
}

VOID MulBcdNum(PBYTE Input1,DWORD Input2, PBYTE OutBuffer, INT Len)
{
    
    BYTE szTemp[15]={0};
    BYTE szInput2[15]={0};
    INT iLen2;
    
    sprintf((PCHAR)szTemp,"%lu",Input2);
    iLen2 = StrLen(szTemp);
    Util_ConvAscBcd((PCHAR)szTemp, szInput2, iLen2);
    iLen2 = StrLen(szInput2);
    Util_MulBcd(Input1, Len,szInput2, iLen2,OutBuffer);
    
}

int Util_BcdAscii(char *dst,  unsigned char *src, int l)
{
    int i,n;
    unsigned char b;
    for(i=0,n=0; i<l; i++)
    {
        if ((b=(*src>>4))==0xf) break;
        n++; *dst++='0'+b;
        if ((b=(*src++&0xf))==0xf) break;
        n++; *dst++='0'+b;
    }
    *dst=0;
    return n;
}

VOID MemBcdToAsc(INT Len, PBYTE Source, PBYTE Dest)
{
    Util_BcdAscii((PCHAR)Dest,Source,Len);
}

void Utils_StringCenter(char *dest,char *source,char pCh,int Len)
{
    int  i,slen,k;
    
    slen = StrLen((PBYTE)source);
    
    if (slen < Len) 
    {
        k = (Len - slen)/2;
        
        for (i=0; i<k;    i++) *dest++ = pCh;
        for (i=0; i<slen; i++) *dest++ = *source++;
        k = Len - slen - k ;
        
        for (i=0; i<k; i++) *dest++ = pCh;
        
        *dest = 0;
    }
    else
    {
        StrCpy((PBYTE)dest,(PBYTE)source);
    }
}

INT StrLen(PBYTE Src)
{
    return (strlen((const char *)Src));    
}

void AscHex(uchar * Out, char *In, uint len)
{
    uint i;
    char TempChar;
    
    for (i = 0; i < (len * 2); i++)
    {
        TempChar = In[i];
        if ((TempChar >= 0x61) && (TempChar <= 0x66))	// If small letters     a,b,c,d,e,f
            TempChar -= 0x20;	// Convert to capitals  A,B,C,D,E,F
        if (!(((TempChar >= 0x41) && (TempChar <= 0x46)) || ((TempChar >= 0x30) && (TempChar <= 0x39))))
            TempChar = 0x46;	// If not in A-F or 0-9 interval, convert to 'F'.

        if (i % 2)
        {
            if ((TempChar >= 0x41) && (TempChar <= 0x46))
                *Out |= ((TempChar - 0x37));
            else
                *Out |= ((TempChar - 0x30) & 0x0F);
            Out++;
        }
        else
        {
            if ((TempChar >= 0x41) && (TempChar <= 0x46))
                *Out = ((TempChar - 0x37) << 4);
            else
                *Out = (TempChar << 4);
        }
    }
    return;
}


BYTE IsNulls(void *Buf, ushort Len)
{
    BYTE *Bf = Buf;
    
    while (Len)
    {
        if (*Bf != 0x00)
            return 0;
        Len--;
        Bf++;
    }
    return 1;
}

void ClearCustomMessages(MSG_Struct *stMSG)
{
    memset(stMSG->CSTMSG1, 0, LG_CSTMSG);
    memset(stMSG->CSTMSG2, 0, LG_CSTMSG);
    memset(stMSG->CSTMSG3, 0, LG_CSTMSG);
    memset(stMSG->CSTMSG4, 0, LG_CSTMSG);
    memset(stMSG->CSTMSG5, 0, LG_CSTMSG);
    memset(stMSG->CSTMSG6, 0, LG_CSTMSG);
    memset(stMSG->CSTMSG7, 0, LG_CSTMSG);
    memset(stMSG->CSTMSG8, 0, LG_CSTMSG);
}

ushort strnlen(char *Buf, ushort MaxLen)
{
    /*usint*/UINT i;
    
    if (!Buf)
        return 0;
    for (i = 0; i < MaxLen; ++i)
    {
        if (Buf[i] == 0x00)
            break;
    }
    return (/*usint*/UINT)i;
}

void BinToHex(char *po_hex, BYTE *pi_bin, int pi_len)
{
    int i;
    
    const char hex_map[] = "0123456789ABCDEF";
    
    for(i = 0; i < pi_len; i++)
    {
        po_hex[i + i + 0] = hex_map[pi_bin[i] >> 4];
        po_hex[i + i + 1] = hex_map[pi_bin[i] & 15];
    }
    
    po_hex[pi_len + pi_len] = 0;
}

int mCmmnUtil_AsciiToBcd(void *voFromAsc, BYTE *byToAsc, int siFromLen)
{
    int  i, sz;
    BYTE *Ptr=byToAsc;
    char flag,ch;
    char str[100], mode[2];
    
    memcpy(mode,"af",2);
    memcpy(str,(char*)voFromAsc, siFromLen);
    sz=siFromLen;
    
    if (mode[1] == 'f' && sz%2)
    {
        flag=1;
        *Ptr=0x00;
    }
    else
    {
        flag=0;
    }
    
   	for (i=0; i<sz; i++, flag=!flag)
    {
        if (flag)
        {
            if(str[i]<=0x39)
            {
                ch=(str[i] & 0x0F);
                *Ptr |= ch;
            }
            else
            {
                ch=str[i];	/**/				
                ch-=0x40;	/**/				
                ch+=0x09 ;	/**/
                *Ptr |= ch;
            }
            Ptr++;
        }
        else if(str[i]<=0x39)
        {
            ch=(str[i] << 4);
            *Ptr  = ch;
        }
        else
        {
            ch=((str[i] + 0x09) << 4);/**/
            *Ptr  = ch;
        }
    }
    return (siFromLen/2+siFromLen%2);
}


void MulAsciiBCD(BYTE * pDest, BYTE * pNum1, BYTE * pNum2, BYTE Len, BYTE InputType)
{
    BYTE Ascii2BCD_In1[6];
    BYTE Ascii2BCD_In2[6];
    BYTE BCD2Ascii_Out[6];
    ULONG Val1, Val2 = 0, Result;
    
    if (((InputType == ASCII_IN) && (Len > 12)) || ((InputType == BCD_IN) && (Len > 6))) {
        return;
    }
    
    if (InputType == ASCII_IN)
    {
        memset(pDest, 0x30, Len);	// Initialize output buffer
        Len /= 2;
        AscHex(Ascii2BCD_In1, (char *)pNum1, Len);
        pNum1 = Ascii2BCD_In1;
        AscHex(Ascii2BCD_In2, (char *)pNum2, Len);
        pNum2 = Ascii2BCD_In2;
    }
    else						// if (InputType == BCD_IN)
    {
        memset(pDest, 0, Len);	// Initialize output buffer
    }
    
    if (IsNulls(pNum1, (/*usint*/UINT) Len) || IsNulls(pNum2, (/*usint*/UINT) Len))	// Either of the multipliands is zero.
        return;
    
    if (Len == 6)	// Extreme case, handled seperately
    {
        Val1 = Bcd2Bin(&pNum1[1],5);
        Val2 = Bcd2Bin(&pNum2[1],5);
    }
    else
    {
        Val1 = Bcd2Bin(pNum1, Len);
        Val2 = Bcd2Bin(pNum2, Len);
    }
    Result = Val1 * Val2;
    
    memset(BCD2Ascii_Out, 0, LG_TRAMT);
    if (Len == 6)
        Bin2Bcd_Dword(&BCD2Ascii_Out[1], Result);
    else if (Len == 5)
        Bin2Bcd_Dword(BCD2Ascii_Out, Result);
    else if (Len == 3)
        Bin2Bcd_Word(BCD2Ascii_Out, (/*usint*/UINT) Result);
    else if (Len == 2)
        Bin2Bcd_Byte(BCD2Ascii_Out, (BYTE) Result);
    else
        return;
    
    if (InputType == ASCII_IN)
        BfAscii((char *)pDest, BCD2Ascii_Out, Len);
    else						// if (InputType == BCD_IN)
        memcpy(pDest, BCD2Ascii_Out, Len);
}

void Bin2Bcd_Byte(BYTE *pBcd, BYTE Bin)
{
    if (!pBcd)
    {
        return;
    }

    if (!Bin)
    {
        memset(pBcd, 0, 2);
        return;
    }
    pBcd[1] = (Bin % 10);
    pBcd[1] |= (((Bin % 100) / 10) << 4);
    pBcd[0] = ((Bin % 1000) / 100);
    pBcd[0] |= ((Bin / 1000) << 4);
}

void Bin2Bcd_Word(BYTE *pBcd, /*usint*/UINT Bin)
{
    if (!pBcd)
    {
        return;
    }
    if (!Bin)
    {
        memset(pBcd, 0, 3);
        return;
    }
    pBcd[2] = (BYTE) (Bin % 10);
    pBcd[2] |= (BYTE) (((Bin % 100) / 10) << 4);
    pBcd[1] = (BYTE) ((Bin % 1000) / 100);
    pBcd[1] |= (BYTE) (((Bin % 10000) / 1000) << 4);
    pBcd[0] = (BYTE) (Bin / 10000);
}

void Bin2Bcd_Dword(BYTE * pBcd, /*ulint*/ULONG Bin)
{
    pBcd[4] = (BYTE) (Bin % 10);
    pBcd[4] |= (BYTE) (((Bin % 100) / 10) << 4);
    pBcd[3] = (BYTE) ((Bin % 1000) / 100);
    pBcd[3] |= (BYTE) (((Bin % 10000) / 1000) << 4);
    pBcd[2] = (BYTE) ((Bin % 100000) / 10000);
    pBcd[2] |= (BYTE) (((Bin % 1000000) / 100000) << 4);
    pBcd[1] = (BYTE) ((Bin % 10000000) / 1000000);
    pBcd[1] |= (BYTE) (((Bin % 100000000) / 10000000) << 4);
    pBcd[0] = (BYTE) ((Bin % 1000000000) / 100000000);
    pBcd[0] |= (BYTE) ((Bin / 1000000000) << 4);
}

ULONG Bcd2Bin(BYTE *pBcd, BYTE Len)
{
    ULONG Binary = 0;
    UINT i;
    
    if (!pBcd)
    {
        return 0;
    }
    if ((Len > 6) || ((Len == 6) && (pBcd[0] != 0x00)) || ((Len == 6) && (pBcd[1] > 0x42)) || ((Len == 5) && (pBcd[0] > 0x42)))
        return 0;
    
    for (i = 0; i < Len; i++)
        Binary = (Binary * 100) + ((pBcd[i] & 0xF0) >> 4) * 10 + (pBcd[i] & 0xF);
    
    return Binary;
}

void DivAsciiBCD(BYTE * pDest, BYTE * pNum1, BYTE * pNum2, BYTE Len, BYTE InputType, BYTE * pRemain)
{
    BYTE Ascii2BCD_In1[6];
    BYTE Ascii2BCD_In2[6];
    BYTE BCD2Ascii_Out[6];
    ULONG Val1, Val2, Result;
    double i;
    
    if (((InputType == ASCII_IN) && (Len > 12)) || ((InputType == BCD_IN) && (Len > 6)))
        return;
    
    if (InputType == ASCII_IN)
    {
        Len /= 2;
        AscHex(Ascii2BCD_In1, (char *)pNum1, Len);
        pNum1 = Ascii2BCD_In1;
        AscHex(Ascii2BCD_In2, (char *)pNum2, Len);
        pNum2 = Ascii2BCD_In2;
    }
    
    if (InputType == BCD_IN)	// Initialize Destination Buffer
    {
        memset(pDest, 0x00, Len);
        memset(pRemain, 0x00, Len);
    }
    else	// (InputType == ASCII_IN)
    {
        memset(pDest, 0x30, Len * 2);
        memset(pRemain, 0x30, Len * 2);
    }
    
    if (IsNulls(pNum2, (/*usint*/UINT) Len) || IsNulls(pNum1, (/*usint*/UINT) Len))	// Divisor or  Dividend all zeros, do not calculate.
        return;
    
    if (Len == 6)	// Extreme case, handled seperately
    {
        Val1 = Bcd2Bin(&pNum1[1], 5);
        Val2 = Bcd2Bin(&pNum2[1], 5);
    }
    else
    {
        Val1 = Bcd2Bin(pNum1, Len);
        Val2 = Bcd2Bin(pNum2, Len);
    }
    
    // Calculate Quotient ----------------------------------------------------
    Result = Val1 / Val2;
    memset(BCD2Ascii_Out, 0, LG_TRAMT);
    if (Len == 6)
        Bin2Bcd_Dword(&BCD2Ascii_Out[1], Result);
    else if (Len == 5)
        Bin2Bcd_Dword(BCD2Ascii_Out, Result);
    else if (Len == 3)
        Bin2Bcd_Word(BCD2Ascii_Out, (/*usint*/UINT) Result);
    else if (Len == 2)
        Bin2Bcd_Byte(BCD2Ascii_Out, (BYTE) Result);
    else
        return;
    
    if (InputType == ASCII_IN)
        BfAscii((char *)pDest, BCD2Ascii_Out, Len);
    else						// if (InputType == BCD_IN)
        memcpy(pDest, BCD2Ascii_Out, Len);
    
    // Calculate Remainder ---------------------------------------------------
    i = (double)Val1 / (double)Val2;
	Result= (ULONG)i;  // Modified by Kim_LinHB 2013-3-11 build 0.05.0009 add (ULONG)
    i -= (/*ulint*/ULONG) i;
// by jarod 
//////////////////////////Adjust Remainder////////////////////////////
	if(i>=0.5)
	{
		Result++;
		memset(BCD2Ascii_Out, 0, LG_TRAMT);
		if (Len == 6)
			Bin2Bcd_Dword(&BCD2Ascii_Out[1], Result);
		else if (Len == 5)
			Bin2Bcd_Dword(BCD2Ascii_Out, Result);
		else if (Len == 3)
			Bin2Bcd_Word(BCD2Ascii_Out, (/*usint*/UINT) Result);
		else if (Len == 2)
			Bin2Bcd_Byte(BCD2Ascii_Out, (BYTE) Result);
		else
			return;

		if (InputType == ASCII_IN)
			BfAscii((char *)pDest, BCD2Ascii_Out, Len);
		else						// if (InputType == BCD_IN)
			memcpy(pDest, BCD2Ascii_Out, Len);		
	}
////////////////////////////////////////////////////////////////////
    Result = (/*ulint*/ULONG) (i * (double)Val2);
    memset(BCD2Ascii_Out, 0, LG_TRAMT);
    if (Len == 6)
        Bin2Bcd_Dword(&BCD2Ascii_Out[1], Result);
    else if (Len == 5)
        Bin2Bcd_Dword(BCD2Ascii_Out, Result);
    else if (Len == 3)
        Bin2Bcd_Word(BCD2Ascii_Out, (/*usint*/UINT) Result);
    else if (Len == 2)
        Bin2Bcd_Byte(BCD2Ascii_Out, (BYTE) Result);
    else
        return;
    
    if (InputType == ASCII_IN)
        BfAscii((char *)pRemain, BCD2Ascii_Out, Len);
    else						// if (InputType == BCD_IN)
        memcpy(pRemain, BCD2Ascii_Out, Len);    
}

uchar bytebin2bcd(uchar x)
{
    return (x / 10)*0x10 + (x % 10);
}

uchar AddAsciiBCD(uchar * pDest, uchar * pNum1, uchar * pNum2, uchar Len, uchar InputType)
{
    uchar Carry = 0;
    uchar BinByteProc;
    uchar i;
    uchar Ascii2BCD_In1[10];
    uchar Ascii2BCD_In2[10];
    uchar BCD2Ascii_Out[10];
    uchar *pTempDest = NULL;
    
    if (((InputType == ASCII_IN) && (Len > 20)) || ((InputType == BCD_IN) && (Len > 10)))
    {
        memset(pDest, 0, Len);
        return 0xFF;
    }

    if (InputType == ASCII_IN)
    {
        Len /= 2;
        AscHex(Ascii2BCD_In1, (char *)pNum1, Len);
        pNum1 = Ascii2BCD_In1;
        AscHex(Ascii2BCD_In2, (char *)pNum2, Len);
        pNum2 = Ascii2BCD_In2;
        pTempDest = pDest;
        pDest = BCD2Ascii_Out;
    }
    else
    {				// if (InputType == BCD_IN) // To avoid side-effect for BCD
        memcpy(Ascii2BCD_In1, pNum1, Len);
        pNum1 = Ascii2BCD_In1;
        memcpy(Ascii2BCD_In2, pNum2, Len);
        pNum2 = Ascii2BCD_In2;
    }

    //---------------------------------------------------------------------------------------
    for (i = Len - 1; i >= 0, i != 0xFF; --i)
    {
        // Check if bytes are valid BCD values
        if (((pNum1[i] & 0x0F) > 9) || (((pNum1[i] & 0xF0) >> 4) > 9) || ((pNum2[i] & 0x0F) > 9)
            || (((pNum2[i] & 0xF0) >> 4) > 9))
            return 0xFF;
        
        BinByteProc = Carry;
        BinByteProc += (pNum1[i] & 0x0F);
        BinByteProc += (((pNum1[i] & 0xF0) >> 4) * 10);
        BinByteProc += (pNum2[i] & 0x0F);
        BinByteProc += (((pNum2[i] & 0xF0) >> 4) * 10);
        Carry = (BinByteProc / 100);
        BinByteProc %= 100;
        pDest[i] = (BinByteProc % 10);
        pDest[i] |= ((BinByteProc / 10) << 4);
    }

    //---------------------------------------------------------------------------------------
    if (InputType == ASCII_IN)
        BfAscii((char *)pTempDest, pDest, Len);
    
    return Carry;
}
