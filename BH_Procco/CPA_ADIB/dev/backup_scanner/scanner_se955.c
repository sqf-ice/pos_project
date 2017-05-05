

#include "scanner_se955.h"


/*
SE955 条码枪支持库
changelog:
20081227 xielihong
       第一个可用的版本

20090202 xielihong
      1. 对open 和close 进行优化
      2. 在P90上测试通过

*/

#define SE955_PORT 2

static uchar g_port=0xff;

int SE955_Open(void)
{
	uchar ucRet;
	int ret;
	ucRet = PortOpen(SE955_PORT,"9600,8,n,1");
	if (ucRet  != 0x00)
	{
		return SE955_ERROR_OPEN;
	}
	g_port = SE955_PORT; /* 要先赋值，不然后面的函数 没法运行 */
	ret = SE955_SetParam1(0x9f,0x01); // enable ack/nak
	if (ret != 0) 
	{
		g_port = 0xFF;
		PortClose(SE955_PORT);
		return SE955_ERROR_NODECODE;
	}
	DelayMs(100);
	SE955_SetParam1(0xee,0x01); // packeted decode data

	SE955_SetParam1(0x8a,0x08); // triggering: host mode

	SE955_SetParam1(0x80,0x01); // low power,auto sleep

	SE955_SetParam1(0x38,0x00); // do not beep after decode
	g_port = SE955_PORT;
	return SE955_ERROR_OK;
}

void SE955_Close(void)
{
	if (g_port != 0xFF) 
	{
		// 在这里要对一些影响用户观感的选项关闭呢
		// 还是直接回到工厂状态好一点?
		SE955_SendCmd(SE955_AIM_OFF,0,0); // 关闭瞄准功能
		SE955_SetParam1(0x8a,0x08);            // 关闭连续功能
		SE955_SendCmd(SE955_SLEEP,0,0);   // sleep，省电模式
		PortClose(g_port);
		g_port = 0xff;
	}
}

static ushort SE955_GetChecksum(uchar *buf,int size)
{
// SE955 的checksum 为数据的字节的和，再取补码
	int i;
	ushort checksum;
	checksum = 0;
	for (i=0;i<size;i++) 
	{
		checksum += buf[i];
	}
	checksum = (ushort)~(checksum) +1; // 取补码
	return checksum;
}

static void SE955_GetChecksumBuf(uchar *buf,int size,uchar *chksumbuf)
{
	ushort checksum;
	checksum = SE955_GetChecksum(buf,size);
	chksumbuf[0] = checksum >> 8;
	chksumbuf[1] = checksum & 0x00FF;
}

static int SE955_ValidPacket(uchar *buf,int size) // 这里的size是包含 checksum的
{
	ushort checksum1,checksum2;
	checksum1 = SE955_GetChecksum(buf,size-2);
	checksum2 = buf[size-2];
	checksum2 <<= 8;
	checksum2 |= buf[size-1];
	return checksum1 == checksum2;
}

int SE955_RecvPacket(SE955_TDATA * outdata,ushort timeoutms)
{
	uchar ch;
	uchar ucRet;
	uchar rxLength;
	
	uchar buf[256];
	int bufIdx;

	if (g_port == 0xFF) 
	{
		return SE955_ERROR_NOOPEN;
	}

	// 收第一个数据
	ucRet = PortRecv(g_port,&ch,timeoutms);


	if (ucRet != 0) 
	{
		// 超时了
		return SE955_ERROR_TIMEOUT;
	}

	// 收到第一个数据了
	buf[0] = ch;
	bufIdx = 1;
	rxLength = ch;

	while(bufIdx < rxLength+2)
	{
		ucRet = PortRecv(g_port,&ch,100);
		if (ucRet == 0)
		{
			buf[bufIdx] = ch;
			bufIdx++;
		}
		else
		{
			// 接收超时了
			PortReset(g_port);
			return SE955_ERROR_TIMEOUT;
		}
	}
	
	if (SE955_ValidPacket(buf,bufIdx)) 
	{
		// 包有效
		if (outdata != NULL) 
		{
			outdata->opcode = buf[1];
			outdata->status = buf[3];
			outdata->datalen = rxLength-4;
			memcpy(outdata->data,&buf[4],outdata->datalen);
		}
		return SE955_ERROR_OK;
	}
	return SE955_ERROR_PACKET;
}

int SE955_SendPacket(SE955_TDATA * indata)
{
	uchar txBuf[256];
	uchar txLength;
	uchar ucRet;

	if (g_port == 0xFF) 
	{
		return SE955_ERROR_NOOPEN;
	}

	if (indata == NULL) 
	{
		return SE955_ERROR_PARAM;
	}
	
	memset(txBuf,0,sizeof(txBuf));
	PortSends(g_port,txBuf,6); // 用于唤醒 decoder ，经测试，最少需要4个字节的0x00
	
	txLength = indata->datalen +4;
	txBuf[0] = txLength;
	txBuf[1] = indata->opcode;
	txBuf[2] = 0x04; // host
	txBuf[3] = indata->status;
	memcpy(&txBuf[4],indata->data,indata->datalen);

	SE955_GetChecksumBuf(txBuf,txLength,&txBuf[txLength]);
	ucRet = PortSends(g_port,txBuf,(ushort)(txLength+2));
	if (ucRet == 0x00) 
	{
		return SE955_ERROR_OK;
	}
	else
	{
		//return SE955_ERROR_PORT;
		return ucRet;
	}
}

int SE955_SendRecv(SE955_TDATA * indata,SE955_TDATA * outdata,ushort timeoutms)
{
	int ret;
	int retrys;
	SE955_TDATA myoutdata;

	PortReset(g_port);
	retrys = 2;
	while(retrys)
	{
		ret = SE955_SendPacket(indata);
		if (ret != 0) return ret;
		ret = SE955_RecvPacket(&myoutdata,timeoutms);
		if (ret == 0)
		{
			if (myoutdata.opcode == SE955_CMD_ACK) 
			{
				if (outdata != NULL) 
				{
					*outdata = myoutdata;
				}
				return SE955_ERROR_OK;
			}
			else if(myoutdata.opcode == SE955_CMD_NAK)
			{
				uchar cause;
				cause = myoutdata.data[0];
				if (cause == SE955_NAK_RESEND) 
				{
					// decoder 出错 ，重发
					retrys--;
				}
				else if (cause == SE955_NAK_DENIED) 
				{
					return SE955_ERROR_DENY;
				}
				else
				{
					// 未知错误
					return SE955_ERROR_UNKNOWN;
				}
			}
			else
			{
				if (outdata != NULL) 
				{
					*outdata = myoutdata;
				}
				return SE955_ERROR_OK; // 其他命令的直接返回
			}
		}
		else
		{
			retrys--;
		}
		DelayMs(500);
	}
	return SE955_ERROR_TIMEOUT;
}

/* 只限于只返回 SE955_CMD_ACK 和 SE955_CMD_NAK 的命令 */
int SE955_SendCmd(uchar opcode,uchar *data,uchar datalen)
{
	int ret;
	SE955_TDATA indata;
	SE955_TDATA outdata;
	indata.opcode = opcode;
	indata.status = 0;
	indata.datalen = datalen;
	if (data != NULL) 
	{
		memcpy(indata.data,data,indata.datalen);
	}

	ret = SE955_SendRecv(&indata,&outdata,1500);
	return ret;
}

int SE955_SetParam1(uchar param,uchar value)
{
	uchar indata[16];
	uchar indatalen;
	int ret;
	indata[0] = 0x00;
	indata[1] = param;
	indata[2] = value;
	indatalen = 3;

	ret = SE955_SendCmd(SE955_PARAM_SEND,indata,indatalen);
	return ret;
}

int SE955_SetParam2(uchar extend,uchar param,uchar value)
{
	uchar indata[16];
	uchar indatalen;
	int ret;
	indata[0] = 0x00;
	indata[1] = extend;
	indata[2] = param;
	indata[3] = value;
	indatalen = 4;

	ret = SE955_SendCmd(SE955_PARAM_SEND,indata,indatalen);
	return ret;
}

int SE955_StartDecode(void)
{
	int ret;
	ret = SE955_SendCmd(SE955_START_DECODE,0,0);
	return ret;
}

static int SE955_SendAck(void)
{
	SE955_TDATA indata;
	indata.opcode = SE955_CMD_ACK;
	indata.status = 0;
	indata.datalen = 0;
	return SE955_SendPacket(&indata);
}

int SE955_GetDecodeData(uchar *bartype,uchar *outstr,ushort timeoutms)
{
	int ret;
	SE955_TDATA outdata;

	// 返回的数据是打包的
	ret = SE955_RecvPacket(&outdata,timeoutms);
	if (ret == 0) 
	{
		SE955_SendAck(); // 告诉decode 我已经收到了
		outdata.data[outdata.datalen] = 0; // 添加个字符串的结束符
		*bartype = outdata.data[0];
		memcpy(outstr,&outdata.data[1],outdata.datalen+1);
	}
	return ret;
}

int SE955_StopDecode(void)
{
	int ret;
	ret = SE955_SendCmd(SE955_STOP_DECODE,0,0);
	return ret;
}

