#include <stdio.h>
#include <string.h>
#include "Utf8ToAr.h"

static int UTF8ToUTF16(unsigned char *outb, int *outlen, const unsigned char *in, int *inlen, char isLittleEndian);
static int UTF16ToAr(unsigned char *out, int *outlen, unsigned char *in, int inlen, char isLittleEndian);
static char IsLittleEndian(void);

void UTF8ToAr(const unsigned char *UTF8, unsigned int LenIn, unsigned char *Ar)
{
	unsigned char UTF16[1024];
	unsigned int UTF16_Len = 0, UTF8_Len = LenIn, Ar_len = 0;

	UTF8ToUTF16(UTF16, &UTF16_Len, UTF8, &UTF8_Len, IsLittleEndian());
	UTF16ToAr(Ar, &Ar_len, UTF16, UTF16_Len, IsLittleEndian());
}

static int UTF8ToUTF16(unsigned char *outb, int *outlen, const unsigned char *in, int *inlen, char isLittleEndian)
{
	unsigned short *out = (unsigned short*) outb;
	const unsigned char* processed = in;
	const unsigned char *const instart = in;
	unsigned short *outstart= out, *outend;
	const unsigned char* inend;
	unsigned int c, d;
	int trailing;
	unsigned char *tmp;
	unsigned short tmp1, tmp2;

	if((out == NULL) || (outlen == NULL) || (inlen == NULL)) return -1;
	if(in == NULL)
	{
		*outlen = 0;
		*inlen = 0;
		return 0;
	}
	inend= in + *inlen;
	outend = out + (*outlen / 2);
	while(in < inend)
	{
		d= *in++;
		if (d < 0x80) { c= d; trailing= 0; }
		else if(d < 0xC0)
		{
			*outlen = (out - outstart) * 2;
			*inlen = processed - instart;
			return -1;
		}
		else if(d < 0xE0) { c= d & 0x1F; trailing= 1; }
		else if(d < 0xF0) { c= d & 0x0F; trailing= 2; }
		else if(d < 0xF8) { c= d & 0x07; trailing= 3; }
		else 
		{
			*outlen = (out - outstart) * 2;
			*inlen = processed - instart;
			return -1;
		}

		if(inend - in < trailing) break;

		for(; trailing; trailing--)
		{
			if((in >= inend) || (((d= *in++) & 0xC0) != 0x80))
				break;
			c <<= 6;
			c |= d & 0x3F;
		}

		if(c < 0x10000)
		{
			//if(out >= outend) break;
			if(isLittleEndian) { *out++ = c; } 
			else
			{
				tmp = (unsigned char *) out;
				*tmp = c ;
				*(tmp + 1) = c >> 8 ;
				out++;
			}
		}
		else if(c < 0x110000)
		{
			//if(out+1 >= outend) break;
			c -= 0x10000;
			if(isLittleEndian)
			{
				*out++ = 0xD800 | (c >> 10);
				*out++ = 0xDC00 | (c & 0x03FF);
			} 
			else
			{
				tmp1 = 0xD800 | (c >> 10);
				tmp = (unsigned char *) out;
				*tmp = (unsigned char) tmp1;
				*(tmp + 1) = tmp1 >> 8;
				out++;

				tmp2 = 0xDC00 | (c & 0x03FF);
				tmp = (unsigned char *) out;
				*tmp = (unsigned char) tmp2;
				*(tmp + 1) = tmp2 >> 8;
				out++;
			}
		}
		else break;

		processed = in;
	}

	*outlen = (out - outstart) * 2;
	*inlen = processed - instart;

	return(*outlen);
}


static int UTF16ToAr(unsigned char *out, int *outlen, unsigned char *in, int inlen, char isLittleEndian)
{
	int i = 0, k = 0;
	unsigned char c1 = 0, c2 = 0, *p;
	int len = 0;
	unsigned short svalue;

	if(NULL == in) { out[0] = '\0'; return 0; }

	len = (inlen < 0 ? 0 : inlen);
	if(len % 2 == 1) len --;

	p = in;
	while(in<p+len)
	{
		if(isLittleEndian)
		{
			c1 = *(in+1);
			i = 0;
		}
		else
		{
			c1 = *in;
			i = 1;
		}

		if(NULL == out) return -1;
		if(c1 != 00)
		{
			//out[k++] = *(in+i) + 0xA0;
			memcpy((unsigned char *)&svalue, in, 2);
			if ((svalue >= 0x0620) && (svalue <= 0x0636))
			{
				out[k++] = svalue - 0x0560;
			}
			else if ((svalue >= 0x0637) && (svalue <= 0x063A))
			{
				out[k++] = svalue - 0x055F;
			}  
			else if ((svalue >= 0x0641) && (svalue <= 0x0643))
			{
				out[k++] = svalue - 0x0564;
			}  
			else if (svalue == 0x0644)
			{
				out[k++] = svalue - 0x0563;
			}  
			else if ((svalue >= 0x0645) && (svalue <= 0x0648))
			{
				out[k++] = svalue - 0x0562;
			}  
			else if ((svalue >= 0x0649) && (svalue <= 0x064A))
			{
				out[k++] = svalue - 0x055D;
			}
			else
			{
				out[k++] = *(in+i) + 0xA0;
			}
		}
		else
		{
			out[k++] = *(in+i);
		}
		in+=2;
	}
	out[k] = 0;

	*outlen = k;
	return 0;
}

static char IsLittleEndian(void)
{
	union w{
		int a;
		char b;
	} c;

	c.a = 1;
	return (c.b == 1);
}
