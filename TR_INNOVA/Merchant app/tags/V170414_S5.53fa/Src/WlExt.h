#ifndef __WLEXT_H__
#define __WLEXT_H__

#define WNET_GPRS_Q24  				0x01
#define WNET_GPRS_G24				0x02
#define WNET_CDMA_MG815				0x03
#define WNET_GPRS_GTM900			0x04
#define WNET_CDMA_CM300				0x05
#define WNET_WCDMA_TELIT			0x06
#define WNET_GPRS_ENFORA			0x07                      
#define WNET_CDMA_EM200				0x08

#define WNET_GPRS_Q2687				0x0a
#define WNET_CDMA_Q26EL				0x0b
#define WNET_CDMA_MC323				0x0c
#define WNET_GPRS_MG323				0x0d
#define WNET_WCDMA_MU509			0x0e
#define WNET_CDMA_MC509				0x0f
#define WNET_GPRS_G620				0x10
#define WNET_GPRS_G610				0x11 

#define REG_LOCATION                                1 // locaion network
#define REG_ROAMING                                 5 //roaming network
#define IMSI_NUM_LEN                                15 //the max len of imsi
#define CCID_NUM_LEN                                20 //the max len of ccid
#define ISP_NAME_LEN                                31 //the max len of isp name
#define MAX_CELL_NUM	                                7 //the max cell number
#define CTR_R                                               '\r'
#define CTR_N                                               '\n'


#define IMSI_KEY	1 //IMSI value
#define CCID_KEY	2 //CCID value
#define MODULEINFO_KEY	3 //module information
#define RSSI_KEY	4 //RSSI value
#define ISP_KEY	5 //ISP name
#define CELLINFO_KEY	6 // main cell and neighbor cell information


typedef struct WlCellInfo
{
    char mcc[3+1];//mobile country code
    char mnc[3+1];//moblie network code
    char lac[5+1];//location code
    char cell[5+1];//cell code
    char bsic[5+1];//base station identifier code
    char chann[5+1];//absolute Frequency channel number
    char rxlev[5+1];//receive level
    char reserver[32]; //resever for future use.
}WlCellInfo_T;

typedef struct WlInfo
{
    char imsi[IMSI_NUM_LEN+1];//the imsi number of sim card
    char ccid[CCID_NUM_LEN+1];//the ccid of sim card		
    char isp_name[ISP_NAME_LEN + 1];//CHINA MOBLE, etc
    char module_name[64]; //MG323,etc
    char manufacture[64]; //HUAWEI,etc
    char revision[64]; //11.810.01.00.00 ,etc
    char rssi;//31, etc
    char reserve[256]; //reserve for future use.
    int net_status;// 1-5
    WlCellInfo_T cell_info[MAX_CELL_NUM]; //cell_info[0], main cell information. cell_info[1]-cell_info[6], neighbor cell information.
}WlInfo_T;

/*
Input: key, The value is one of the following values:
				IMSI_KEY,
				CCID_KEY,
				MODULEINFO_KEY,
				RSSI_KEY,
				ISP_KEY,
				CELLINFO_KEY,
				
Output: info 
Return: void

notes:
1.When ppp is online, don't call this function.
2.Please call WlInit() and WlOpenPort()function before calling this function.
3.when there is any error happend, the items of WlInfo_T are set to 0, NULL, or blank.
*/
void Dll_WlGetInfo(int key, WlInfo_T *info);

#endif

