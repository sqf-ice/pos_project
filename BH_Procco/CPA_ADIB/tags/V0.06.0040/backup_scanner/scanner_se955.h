#ifndef _SE955_H_
#define _SE955_H_
/*
SE955 条码枪支持库
xielihong 20081227
*/

#include "posapi.h"

typedef struct 
{
	uchar data[256];
	uchar datalen;
	uchar opcode;
	uchar status;
	uchar no_use;
}SE955_TDATA;

/* 第三层 API */
int SE955_Open(void);
void SE955_Close(void);
int SE955_StartDecode(void);
int SE955_GetDecodeData(uchar *bartype,uchar *outstr,ushort timeoutms);
int SE955_SetParam1(uchar param,uchar value);
int SE955_SetParam2(uchar extend,uchar param,uchar value);
int SE955_SendCmd(uchar opcode,uchar *data,uchar datalen);

/* 第二层 API */
int SE955_SendRecv(SE955_TDATA * indata,SE955_TDATA * outdata,ushort timeoutms);

/* 第一层 API */
int SE955_RecvPacket(SE955_TDATA * outdata,ushort timeoutms);
int SE955_SendPacket(SE955_TDATA * indata);

/* 错误码 */
#define SE955_ERROR_OK        0
#define SE955_ERROR_UNKNOWN  -1 /* 未知错误 */
#define SE955_ERROR_OPEN     -2 /* 串口打开失败  */
#define SE955_ERROR_NOOPEN   -3 /* 没打开 */
#define SE955_ERROR_NODECODE -4 /* 无SE955 */
#define SE955_ERROR_CMDNAK   -5 
#define SE955_ERROR_CMDACK    0 
#define SE955_ERROR_PARAM    -6 /* 参数错误 */
#define SE955_ERROR_TIMEOUT  -7 /* 超时 */
#define SE955_ERROR_PACKET   -8 /* 接收的包非法，校验不过 */
#define SE955_ERROR_DENY     -9 /* 不允许的操作，可能是由于参数设置错误了 */
#define SE955_ERROR_PORT     -10 /* 串口读写出错 */

/* 命令 */
#define SE955_AIM_OFF           0xc4
#define SE955_AIM_ON            0xc5     /* 启动瞄准功能，类似激光笔，平常只有一个红点 */
#define SE955_BEEP              0xe6     /* 蜂鸣器 */
#define SE955_CMD_ACK           0xD0
#define SE955_CMD_NAK           0xD1
#define SE955_DECODE_DATA       0xF3     /* decoder only */
#define SE955_EVENT             0xf6
#define SE955_LED_OFF           0xe8
#define SE955_LED_ON            0xe7
#define SE955_PARAM_DEFAULTS    0xc8     /* 恢复参数为工厂模式 */
#define SE955_PARAM_REQUEST     0xc7     /* 读取参数 */
#define SE955_PARAM_SEND        0xc6     /* 设置参数 */
#define SE955_REPLY_REVISION    0xa4     /* decoder only */
#define SE955_REQUEST_REVISION  0xa3     /* 读取版本信息 */
#define SE955_SCAN_DISABLE      0xea     /* 禁用扫描，也就不可以去读条码了 */
#define SE955_SCAN_ENABLE       0xe9     /* 启用扫描功能 */
#define SE955_SLEEP             0xeb     /* 手动进入省电模式 */
#define SE955_START_DECODE      0xe4     /* 开始扫描 */
#define SE955_STOP_DECODE       0xe5
#define SE955_WAKEUP            0x00     /* not support 直接给串口发送0x00 就可以了 */
#define SE955_CUSTOM_DEFAULTS   0x12


#define SE955_NAK_RESEND 1
#define SE955_NAK_DENIED 6
#define SE955_NAK_BAD_CONTEXT  2


/*

使用方法:
  1. 打开设备，一般只需调用一次
  ret = SE955_Open();

  2. 设置参数，参数在关机后失效
  参数的param 和 value 值的意义见 datasheet
  ret = SE955_SetParam1(param,value);
  ret = SE955_SetParam2(extend,param,value);
  SE955_SetParam1(0x80,0x01); // low power,auto sleep

  3. 发送命令，只能用于返回 SE955_CMD_ACK 的命令
     支持的命令有 
	   SE955_AIM_OFF     SE955_AIM_ON          SE955_BEEP          SE955_LED_OFF 
	   SE955_LED_ON      SE955_PARAM_DEFAULTS  SE955_PARAM_SEND    SE955_SCAN_DISABLE 
	   SE955_SCAN_ENABLE SE955_SLEEP           SE955_START_DECODE  SE955_STOP_DECODE
   示例:
  //	ret = SE955_SendCmd(SE955_AIM_ON,0,0); // 启动的话，就像激光笔一样，只有一个点
	ret = SE955_SendCmd(SE955_AIM_OFF,0,0);
	ret = SE955_SendCmd(SE955_BEEP,"\x18",1);
	ret = SE955_SendCmd(SE955_LED_OFF,"\x01",1);
	DelayMs(1000);
	ret = SE955_SendCmd(SE955_LED_ON,"\x01",1);
	ret = SE955_SendCmd(SE955_SCAN_DISABLE,0,0);
	ret = SE955_SendCmd(SE955_SCAN_ENABLE,NULL,0);
	ret = SE955_SendCmd(SE955_SLEEP,0,0);
//	ret = SE955_SendCmd(SE955_PARAM_DEFAULTS,0,0); // reset to factory setting

  4. 读卡流程
  uchar data[256];
  uchar bartype;
  int ret;
  ret = SE955_StartDecode();
  if (ret == SE955_ERROR_OK)
  {
     ret = SE955_GetDecodeData(&bartype,data,3000); // 读卡器默认扫描3秒
	 if (ret == SE955_ERROR_OK)
	 {
	    // 读卡成功
	 }
	 else
	 {
		// 这里主要是超时了
	   }
	 
	}
	
  5. 关闭设备
   SE955_Close();

  6. 第二层发收函数，主要用于第三层无法完成的场合，主要是参数，状态等的获取
  SE955_SendRecv 主要用于 SE955_PARAM_REQUEST SE955_REQUEST_REVISION
   示例
  	{
		SE955_TDATA indata;
		SE955_TDATA outdata;
		indata.opcode = SE955_PARAM_REQUEST;
		indata.status = 0;
		indata.datalen = 1;
		memcpy(indata.data,"\xfe",1); // get all params 
		ret = SE955_SendRecv(&indata,&outdata,1000);
	}
	{
		SE955_TDATA indata;
		SE955_TDATA outdata;
		indata.opcode = SE955_REQUEST_REVISION;
		indata.status = 0;
		indata.datalen = 0;
		ret = SE955_SendRecv(&indata,&outdata,1000);
	}

  备注：
  1. 默认启用low power模式，自动低功耗
  2. SE955_SetParam 关机失效
  3. 如果手动使decoder工作在continuous模式(就是扫描枪常亮)，此时没必要调用 SE955_StartDecode
     但注意要经常调用SE955_GetDecodeData进行收包
     因为
	 1) decoder 如果收到数据，马上就会发过来
	 2) decoder 如果没有收到确认包(CMD_ACK)，会重发
  
*/


#endif
