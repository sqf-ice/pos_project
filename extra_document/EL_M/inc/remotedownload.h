/*------------------------------------------------------------
* FileName: remotedownload.h
* Author:	ZhangYuan
* Date:		2016-08-30
------------------------------------------------------------*/

#ifndef REMOTE_DOWNLOAD_H_
#define REMOTE_DOWNLOAD_H_

#define MPOS_RESPONSE_DATA_OFFSET		9

#define MPOS_STATUS_OK					0x00
#define MPOS_STATUS_CMD_NOTSPT			-0xffff
#define MPOS_STATUS_NOTASKLIST			1
#define MPOS_STATUS_FORGET_SET_TASK		2
#define MPOS_STATUS_FILENO_NOT_SAME		3
#define MPOS_STATUS_TASK_NUM_OVERFLOW	4
#define MPOS_STATUS_FIRMWARE_SAVE_ERR	5
#define MPOS_STATUS_PARA_SAVE_ERR		6
#define MPOS_STATUS_SYS_ERR				7		// system error, like file operation error

#define TMS_TASK_NUM_MAX				10	

#define MPOS_FILE_REMOTEDOWNLOAD_BLK_IDX	3100

// task
#define PROTIMS_DOWNLOAD_MONITOR         0x00    // 下载监控文件
#define PROTIMS_CREATE_FILE_SYSTEM       0x01    // 创建文件系统
#define PROTIMS_DOWNLOAD_FONT            0x02    // 下载字库
#define PROTIMS_DELETE_APPLICATION       0x03    // 删除应用程序
#define PROTIMS_DOWNLOAD_APPLICATION     0x04    // 下载应用程序
#define PROTIMS_DOWNLOAD_PARA_FILE       0x05    // 下载参数文件
#define PROTIMS_DELETE_PARA_FILE         0x06    // 删除参数文件
#define PROTIMS_DELETE_ALL_APPLICATION   0x07    // 删除所有应用程序及其附属文件（包括参数文件和日志）
#define PROTIMS_WNET_FIRMWARE_UPDATE     0x09    // 无线模块驱动程序更新												//20130416
#define PROTIMS_DOWNLOAD_DLL             0x0A	 // 应用动态库文件下载任务	
#define PROTIMS_DELLETE_DLL              0x0B    // 删除动态库文件
#define PROTIMS_DOWNLOAD_USPUK			 0x10
#define PROTIMS_DOWNLOAD_UAPUK			 0x11
#define PROTIMS_DOWNLOAD_PUBFILE		 0x12
#define PROTIMS_DELETE_PUBFILE			 0x13


//////////////////////////////////////////////////////////////////////////
// download status flag for ucDone in TASK_TABLE
#define PROTIMS_NONE_STATUS					0
#define PROTIMS_DOWNLOADING_STATUS			1
#define PROTIMS_DOWNLOADED_STATUS			2
#define PROTIMS_UPDATED_STATUS				3
// Task List
typedef struct
{
	unsigned int	iFileSize;			// 当前任务大小尺寸
	unsigned char	ucTaskNo;			// 任务号
	unsigned char	ucTaskType;			// 任务类型
	unsigned char	aucFileName[33];	// 文件名
	unsigned char	aucAppName[33];		// 应用程序名
	unsigned char	aucVersion[21];		// 版本号
	unsigned char	ucForceUpdate;		//强制更新标志
	unsigned char	Rev[5];				// 用作补齐
	unsigned char	ucDone;				// 0:未下载 1:下载中 2:下载完成 3:更新完成
}TASK_TABLE;	//total 42 + 4*4 + 22 = 80

int GetTermSN(unsigned char *pucRspDataOut, int *piLenOut);
int GetTermExtSN(unsigned char *pucRspDataOut, int *piLenOut);
int GetTermVerInfo(unsigned char *pucRspDataOut, int *piLenOut);
int GetTerminalInfo(unsigned char *pucRspDataOut, int *piLenOut);
int RemoteDownload_TaskInfo(const unsigned char *pucReqData, unsigned char *pucRspData, int *piRspLen);
int RemoteDownload_SaveData(const unsigned char *pucReqData, unsigned char *pucRspData, int *piRspLen);
int RemoteDownload_TaskAsk(const unsigned char *pucReqData, unsigned char *pucRspData, int *piRspLen, unsigned char *pucSendRspFlag);



#endif // REMOTE_DOWNLOAD_H_

