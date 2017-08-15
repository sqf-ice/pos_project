package com.paxsz.easylink.api;

import java.util.HashMap;

import android.annotation.SuppressLint;
import com.paxsz.easylink.protocol.DataFormat;

public class ResponseCode {
	
	
	public static final int EL_RET_OK = 0;//sucess
   
	//communication moudle return code
    public static final int EL_COMM_RET_BASE = 1000;
    public static final int EL_COMM_RET_CONNECTED = 1001;
    public static final int EL_COMM_RET_DISCONNECT_FAIL = 1002;
    public static final int EL_COMM_RET_NOTCONNECTED = 1003;
    public static final int EL_COMM_RET_PARAM_FILE_NOT_EXIST = 1004;
    
	//UI Code
	public static final int EL_UI_RET_BASE = 2000;
	public static final int EL_UI_RET_INVALID_WIDGETNAME	= (EL_UI_RET_BASE + 1);	//invalid widget name
	public static final int EL_UI_RET_TIME_OUT = (EL_UI_RET_BASE + 2);	//time out
	public static final int EL_UI_RET_INVALID_PAGE = (EL_UI_RET_BASE +3);	//invalid page
	public static final int EL_UI_RET_PARSEUI_FAILED = (EL_UI_RET_BASE +4);	//parseui failed
	public static final int EL_UI_RET_VALUESIZEERROR = (EL_UI_RET_BASE +5);	//widget value size error
	public static final int EL_UI_RET_INPUT_TYPE_ERRO	= (EL_UI_RET_BASE + 6);	//input type not find
	public static final int EL_UI_RET_INVALID_WIDGETVALUE	= (EL_UI_RET_BASE + 7);	//invalid widget value
	public static final int EL_UI_RET_USER_CANCEL	= (EL_UI_RET_BASE + 8); 	//cancel exit
	public static final int EL_UI_RET_MENUITEMNUM_ERROR = (EL_UI_RET_BASE + 9);	//menuitem number = 0
	
	//Security Code
	public static final int EL_SECURITY_RET_BASE = 3000;
	public static final int EL_SECURITY_RET_NO_KEY = (EL_SECURITY_RET_BASE + 1);  /*key doesnot exist*/
	public static final int EL_SECURITY_RET_PARAM_INVALID = (EL_SECURITY_RET_BASE + 2);  /*Parameter error or invalid*/
	public static final int EL_SECURITY_RET_ENCRYPT_DATA_ERR = (EL_SECURITY_RET_BASE + 3);  /*Encrypt data error*/
	public static final int EL_SECURITY_RET_GET_PIN_BLOCK_ERR = (EL_SECURITY_RET_BASE + 4);  /*Get pin block error*/
	public static final int EL_SECURITY_RET_NO_PIN_INPUT = (EL_SECURITY_RET_BASE + 5);  /*Not input pin*/
	public static final int EL_SECURITY_RET_INPUT_CANCEL = (EL_SECURITY_RET_BASE + 6);  /*user cancel*/
	public static final int EL_SECURITY_RET_INPUT_TIMEOUT = (EL_SECURITY_RET_BASE + 7);  /*input timeout*/
	
	//Transaction return code
	public static final int EL_TRANS_RET_BASE = 4000;
	public static final int EL_TRANS_RET_ICC_RESET_ERR = (EL_TRANS_RET_BASE + 1);
	public static final int EL_TRANS_RET_ICC_CMD_ERR = (EL_TRANS_RET_BASE + 2);
	public static final int EL_TRANS_RET_ICC_BLOCK = (EL_TRANS_RET_BASE + 3);
	public static final int EL_TRANS_RET_EMV_RSP_ERR = (EL_TRANS_RET_BASE + 4);
	public static final int EL_TRANS_RET_EMV_APP_BLOCK = (EL_TRANS_RET_BASE + 5);
	public static final int EL_TRANS_RET_EMV_NO_APP = (EL_TRANS_RET_BASE + 6);
	public static final int EL_TRANS_RET_EMV_USER_CANCEL = (EL_TRANS_RET_BASE + 7);
	public static final int EL_TRANS_RET_EMV_TIME_OUT = (EL_TRANS_RET_BASE + 8);
	public static final int EL_TRANS_RET_EMV_DATA_ERR = (EL_TRANS_RET_BASE + 9);
	public static final int EL_TRANS_RET_EMV_NOT_ACCEPT = (EL_TRANS_RET_BASE + 10);
	public static final int EL_TRANS_RET_EMV_DENIAL = (EL_TRANS_RET_BASE + 11);
	public static final int EL_TRANS_RET_EMV_KEY_EXP = (EL_TRANS_RET_BASE + 12);
	public static final int EL_TRANS_RET_EMV_NO_PINPAD = (EL_TRANS_RET_BASE + 13);
	public static final int EL_TRANS_RET_EMV_NO_PASSWORD = (EL_TRANS_RET_BASE + 14);
	public static final int EL_TRANS_RET_EMV_SUM_ERR = (EL_TRANS_RET_BASE + 15);
	public static final int EL_TRANS_RET_EMV_NOT_FOUND = (EL_TRANS_RET_BASE + 16);
	public static final int EL_TRANS_RET_EMV_NO_DATA = (EL_TRANS_RET_BASE + 17);
	public static final int EL_TRANS_RET_EMV_OVERFLOW = (EL_TRANS_RET_BASE + 18);
	public static final int EL_TRANS_RET_NO_TRANS_LOG = (EL_TRANS_RET_BASE + 19);
	public static final int EL_TRANS_RET_RECORD_NOTEXIST = (EL_TRANS_RET_BASE + 20);
	public static final int EL_TRANS_RET_LOGITEM_NOTEXIST = (EL_TRANS_RET_BASE + 21);
	public static final int EL_TRANS_RET_ICC_RSP_6985 = (EL_TRANS_RET_BASE + 22);
	public static final int EL_TRANS_RET_CLSS_USE_CONTACT = (EL_TRANS_RET_BASE + 23);
	public static final int EL_TRANS_RET_EMV_FILE_ERR = (EL_TRANS_RET_BASE + 24);
	public static final int EL_TRANS_RET_CLSS_TERMINATE = (EL_TRANS_RET_BASE + 25);
	public static final int EL_TRANS_RET_CLSS_FAILED = (EL_TRANS_RET_BASE + 26);
	public static final int EL_TRANS_RET_CLSS_DECLINE = (EL_TRANS_RET_BASE + 27);
	public static final int EL_TRANS_RET_CLSS_TRY_ANOTHER_CARD = (EL_TRANS_RET_BASE + 28);
	public static final int EL_TRANS_RET_PARAM_ERR = (EL_TRANS_RET_BASE + 30);
	public static final int EL_TRANS_RET_CLSS_WAVE2_OVERSEA = (EL_TRANS_RET_BASE + 31);
	public static final int EL_TRANS_RET_CLSS_WAVE2_TERMINATED = (EL_TRANS_RET_BASE + 32);
	public static final int EL_TRANS_RET_CLSS_WAVE2_US_CARD = (EL_TRANS_RET_BASE + 33);
	public static final int EL_TRANS_RET_CLSS_WAVE3_INS_CARD = (EL_TRANS_RET_BASE + 34);
	public static final int EL_TRANS_RET_CLSS_RESELECT_APP = (EL_TRANS_RET_BASE + 35);
	public static final int EL_TRANS_RET_CLSS_CARD_EXPIRED = (EL_TRANS_RET_BASE + 36);
	public static final int EL_TRANS_RET_EMV_NO_APP_PPSE_ERR = (EL_TRANS_RET_BASE + 37);
	public static final int EL_TRANS_RET_CLSS_USE_VSDC = (EL_TRANS_RET_BASE + 38);
	public static final int EL_TRANS_RET_CLSS_CVMDECLINE = (EL_TRANS_RET_BASE + 39);
	public static final int EL_TRANS_RET_CLSS_REFER_CONSUMER_DEVICE = (EL_TRANS_RET_BASE + 40);
	public static final int EL_TRANS_RET_CLSS_LAST_CMD_ERR = (EL_TRANS_RET_BASE + 41);
	public static final int EL_TRANS_RET_CLSS_API_ORDER_ERR = (EL_TRANS_RET_BASE + 42);
	public static final int EL_TRANS_RET_EMV_FAIL = (EL_TRANS_RET_BASE + 43);	// by ZhangYuan
	public static final int EL_TRANS_RET_EMV_DECLINED = (EL_TRANS_RET_BASE + 44);

	//return code
	public static final int EL_PARAM_RET_BASE	= 5000;
	public static final int EL_PARAM_RET_ERR_DATA = (EL_PARAM_RET_BASE + 1);
	public static final int EL_PARAM_RET_INVALID_PARAM = (EL_PARAM_RET_BASE + 2);
	public static final int EL_PARAM_RET_PARTIAL_FAILED = (EL_PARAM_RET_BASE + 3);
	public static final int EL_PARAM_RET_ALL_FAILED = (EL_PARAM_RET_BASE + 4);
	public static final int EL_PARAM_RET_SIZE_SMALL = (EL_PARAM_RET_BASE + 5);
	public static final int EL_PARAM_RET_API_ORDER_ERR = (EL_PARAM_RET_BASE + 6);
	public static final int EL_PARAM_RET_ENCRYPT_SENSITIVE_DATA_FAILED = (EL_PARAM_RET_BASE + 7);
	
	//FILEDOWNLOAD_RET_CODE
	public static final int FILEDOWNLOAD_ERR_BASE	= 7000;
	public static final int FILEDOWNLOAD_ERR_INVALID_PARAM = (FILEDOWNLOAD_ERR_BASE+1); // invalid parameter 
	public static final int FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL = (FILEDOWNLOAD_ERR_BASE + 2); // fail to download file
	public static final int FILEDOWNLOAD_ERR_FILE_OVERSIZE = (FILEDOWNLOAD_ERR_BASE+3); // file over size
	public static final int FILEDOWNLOAD_ERR_NOT_ALLOWED = (FILEDOWNLOAD_ERR_BASE+4);	// file not allowed
	public static final int FILEDOWNLOAD_ERR_PARSE_ERR = (FILEDOWNLOAD_ERR_BASE+5);	// parse file failed
    
    //SDK status code
    public static final int EL_SDK_RET_BASE = 9000;//Base return code for SDK
    public static final int EL_SDK_RET_PARAM_INVALID = (EL_SDK_RET_BASE + 1);//invalid param
    public static final int EL_SDK_RET_COMM_CONNECT_ERR = (EL_SDK_RET_BASE + 2);//comm connect err
    public static final int EL_SDK_RET_COMM_DISCONNECT_ERR = (EL_SDK_RET_BASE + 3);//comm disconnect err
    public static final int EL_SDK_RET_COMM_DISCONNECTED = (EL_SDK_RET_BASE + 4);//comm disconnected
    public static final int EL_SDK_RET_COMM_SEND_ERR = (EL_SDK_RET_BASE + 5);//comm sent err
    public static final int EL_SDK_RET_COMM_RECV_ERR = (EL_SDK_RET_BASE + 6);//comm recv err
	public static final int EL_SDK_RET_FILE_NOT_EXIST = (EL_SDK_RET_BASE + 7);//file does not exist
    
    // Error code for procotol layer 
    public static final int EL_SDK_PROTO_RET_BASE = 9500;//Base return code for proto err
    public static final int EL_SDK_RET_PROTO_GENERAL_ERR = (EL_SDK_PROTO_RET_BASE + 1);//general error
    public static final int EL_SDK_RET_PROTO_ARG_ERR = (EL_SDK_PROTO_RET_BASE + 2);//argument error
    public static final int EL_SDK_RET_PROTO_PACKET_TOO_LONG = (EL_SDK_PROTO_RET_BASE + 3);//packet too long
    public static final int EL_SDK_RET_PROTO_NO_ENOUGH_DATA = (EL_SDK_PROTO_RET_BASE + 4);//receive data not enough
    public static final int EL_SDK_RET_PROTO_DATA_FORMAT = (EL_SDK_PROTO_RET_BASE + 5);//data format error
    public static final int EL_SDK_RET_PROTO_TIMEOUT = (EL_SDK_PROTO_RET_BASE + 6);//timeout
    
    
	private static HashMap<Integer, String> respcodeArry;

	public static HashMap<Integer, String> getRespcodeArry() {
		return respcodeArry;
	}

	public static void setRespcodeArry(HashMap<Integer, String> respcodeArry) {
		ResponseCode.respcodeArry = respcodeArry;
	}

	@SuppressLint("UseSparseArrays")
	public static void initRespCodeTable(){
		
		respcodeArry = new HashMap<Integer, String>();
		respcodeArry.put(0, "success");
		
		//communication moudle return code
		respcodeArry.put(EL_COMM_RET_BASE, "Comm err");
		respcodeArry.put(EL_COMM_RET_CONNECTED, "Already connected");
		respcodeArry.put(EL_COMM_RET_DISCONNECT_FAIL, "Disconnect fail");
		respcodeArry.put(EL_COMM_RET_NOTCONNECTED, "Not connected");
		respcodeArry.put(EL_COMM_RET_PARAM_FILE_NOT_EXIST, "Open compatible communication mode failed");
		
		//UI Code
		respcodeArry.put(EL_UI_RET_BASE, "UI err");
		respcodeArry.put(EL_UI_RET_INVALID_WIDGETNAME, "Invalid parameter");
		respcodeArry.put(EL_UI_RET_TIME_OUT, "Time out");
		respcodeArry.put(EL_UI_RET_INVALID_PAGE, "Invalid page");
		respcodeArry.put(EL_UI_RET_PARSEUI_FAILED, "Parse UI failed");
		respcodeArry.put(EL_UI_RET_VALUESIZEERROR, "Value size err");
		respcodeArry.put(EL_UI_RET_INPUT_TYPE_ERRO, "Input type err");
		respcodeArry.put(EL_UI_RET_INVALID_WIDGETVALUE, "Invalid widget value");
		respcodeArry.put(EL_UI_RET_USER_CANCEL, "User Cancel");
		respcodeArry.put(EL_UI_RET_MENUITEMNUM_ERROR, "Menuitemnun err");
		
		//Security Code
		respcodeArry.put(EL_SECURITY_RET_BASE, "Security err");
		respcodeArry.put(EL_SECURITY_RET_NO_KEY, "Key does not exist");
		respcodeArry.put(EL_SECURITY_RET_PARAM_INVALID, "Parameter err or invalid");
		respcodeArry.put(EL_SECURITY_RET_ENCRYPT_DATA_ERR, "Encrypt data err");
		respcodeArry.put(EL_SECURITY_RET_GET_PIN_BLOCK_ERR, "Get pin block err");
		respcodeArry.put(EL_SECURITY_RET_NO_PIN_INPUT, "No input pin ");
		respcodeArry.put(EL_SECURITY_RET_INPUT_CANCEL, "User cancel");
		respcodeArry.put(EL_SECURITY_RET_INPUT_TIMEOUT, "Input timeout");
		
		//Transaction return code
		respcodeArry.put(EL_TRANS_RET_BASE, "Transaction err");
		respcodeArry.put(EL_TRANS_RET_ICC_RESET_ERR, "IC card reset is failed");
		respcodeArry.put(EL_TRANS_RET_ICC_CMD_ERR, "IC card command is failed");
		respcodeArry.put(EL_TRANS_RET_ICC_BLOCK, "IC card is blocked");
		respcodeArry.put(EL_TRANS_RET_EMV_RSP_ERR, "Status Code returned by IC card is not 9000");
		respcodeArry.put(EL_TRANS_RET_EMV_APP_BLOCK, "The Application selected is blocked");
		respcodeArry.put(EL_TRANS_RET_EMV_NO_APP, "There is no AID matched between ICC and terminal");
		respcodeArry.put(EL_TRANS_RET_EMV_USER_CANCEL, "The Current operation or transaction was canceled by user");
		respcodeArry.put(EL_TRANS_RET_EMV_TIME_OUT, "User’s operation is timeout");
		respcodeArry.put(EL_TRANS_RET_EMV_DATA_ERR, "Data err is found");
		respcodeArry.put(EL_TRANS_RET_EMV_NOT_ACCEPT, "Transaction is not accepted");
		respcodeArry.put(EL_TRANS_RET_EMV_DENIAL, "Transaction is denied");
		respcodeArry.put(EL_TRANS_RET_EMV_KEY_EXP, "Certification Authority Public Key is Expired");
		respcodeArry.put(EL_TRANS_RET_EMV_NO_PINPAD, "PIN enter is required, but PIN pad is not present or not working");
		respcodeArry.put(EL_TRANS_RET_EMV_NO_PASSWORD, "PIN enter is required, PIN pad is present, but there is no PIN entered");
		respcodeArry.put(EL_TRANS_RET_EMV_SUM_ERR, "Checksum of CAPK is err");
		respcodeArry.put(EL_TRANS_RET_EMV_NOT_FOUND, "Appointed Data Element can’t be found");
		respcodeArry.put(EL_TRANS_RET_EMV_NO_DATA, "The length of the appointed Data Element is 0");
		respcodeArry.put(EL_TRANS_RET_EMV_OVERFLOW, "Memory is overflow");
		respcodeArry.put(EL_TRANS_RET_NO_TRANS_LOG, "There is no Transaction log");
		respcodeArry.put(EL_TRANS_RET_RECORD_NOTEXIST, "Appointed log is not existed");
		respcodeArry.put(EL_TRANS_RET_LOGITEM_NOTEXIST, "Appointed Label is not existed in current log record");
		respcodeArry.put(EL_TRANS_RET_ICC_RSP_6985, "Status Code returned by IC card for GPO is 6985");
		respcodeArry.put(EL_TRANS_RET_CLSS_USE_CONTACT, "Must use other interface for the transaction");
		respcodeArry.put(EL_TRANS_RET_EMV_FILE_ERR, "There is file err found");
		respcodeArry.put(EL_TRANS_RET_CLSS_TERMINATE, "Must terminate the transaction");
		respcodeArry.put(EL_TRANS_RET_CLSS_FAILED, "Contactless transaction is failed");
		respcodeArry.put(EL_TRANS_RET_CLSS_DECLINE, "Transaction should be declined");
		respcodeArry.put(EL_TRANS_RET_CLSS_TRY_ANOTHER_CARD, "Try another card (DPAS Only)");
		respcodeArry.put(EL_TRANS_RET_PARAM_ERR, "Parameter is err = EMV_PARAM_ERR");
		respcodeArry.put(EL_TRANS_RET_CLSS_WAVE2_OVERSEA, "International transaction(for VISA AP PayWave Level2 IC card use)");
		respcodeArry.put(EL_TRANS_RET_CLSS_WAVE2_TERMINATED, "Wave2 DDA response TLV format err");
		respcodeArry.put(EL_TRANS_RET_CLSS_WAVE2_US_CARD, "US card(for VISA AP PayWave L2 IC card use)");
		respcodeArry.put(EL_TRANS_RET_CLSS_WAVE3_INS_CARD, "Need to use IC card for the transaction(for VISA PayWave IC card use)");
		respcodeArry.put(EL_TRANS_RET_CLSS_RESELECT_APP, "Select the next AID in candidate list");
		respcodeArry.put(EL_TRANS_RET_CLSS_CARD_EXPIRED, "IC card is expired");
		respcodeArry.put(EL_TRANS_RET_EMV_NO_APP_PPSE_ERR, "No application is supported(Select PPSE is err)");
		respcodeArry.put(EL_TRANS_RET_CLSS_USE_VSDC, "Switch to contactless PBOC");
		respcodeArry.put(EL_TRANS_RET_CLSS_CVMDECLINE, "CVM result in decline for AE");
		respcodeArry.put(EL_TRANS_RET_CLSS_REFER_CONSUMER_DEVICE, "Status Code returned by IC card is 6986, please see phone");
		respcodeArry.put(EL_TRANS_RET_CLSS_LAST_CMD_ERR, "The last read record command is err(qPBOC Only)");
		respcodeArry.put(EL_TRANS_RET_CLSS_API_ORDER_ERR, "APIs are called in wrong order. Please call Clss_GetDebugInfo_xxx to get err codes.");
		respcodeArry.put(EL_TRANS_RET_EMV_FAIL, "emv fail");
		respcodeArry.put(EL_TRANS_RET_EMV_DECLINED, "emv declined");
		
		//return code
		respcodeArry.put(EL_PARAM_RET_BASE, "param err");
		respcodeArry.put(EL_PARAM_RET_ERR_DATA, "Input data err");
		respcodeArry.put(EL_PARAM_RET_INVALID_PARAM, "Invalid parameter");
		respcodeArry.put(EL_PARAM_RET_PARTIAL_FAILED, "Partial operation failed");
		respcodeArry.put(EL_PARAM_RET_ALL_FAILED, "All operation failed");
		respcodeArry.put(EL_PARAM_RET_SIZE_SMALL, "The output buffer size is not enough");
		respcodeArry.put(EL_PARAM_RET_API_ORDER_ERR, "Must call this function after StartTransaction step");
		respcodeArry.put(EL_PARAM_RET_ENCRYPT_SENSITIVE_DATA_FAILED, "Sensitive data failed");
		
		//FILEDOWNLOAD_RET_CODE
		respcodeArry.put(FILEDOWNLOAD_ERR_BASE, "File download err");
		respcodeArry.put(FILEDOWNLOAD_ERR_INVALID_PARAM, "Invalid parameter");
		respcodeArry.put(FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL, "Download parameter file failed");
		respcodeArry.put(FILEDOWNLOAD_ERR_FILE_OVERSIZE, "File Oversize");
		respcodeArry.put(FILEDOWNLOAD_ERR_NOT_ALLOWED, "not allowed");
		respcodeArry.put(FILEDOWNLOAD_ERR_PARSE_ERR, "parse err");
		
		//sdk
		respcodeArry.put(EL_SDK_RET_BASE, "Sdk err");
		respcodeArry.put(EL_SDK_RET_PARAM_INVALID, "Invalid param");
		respcodeArry.put(EL_SDK_RET_COMM_CONNECT_ERR, "Comm connect err");
		respcodeArry.put(EL_SDK_RET_COMM_DISCONNECT_ERR, "Comm disconnect err");
		respcodeArry.put(EL_SDK_RET_COMM_DISCONNECTED, "Comm disconnected");
		respcodeArry.put(EL_SDK_RET_COMM_SEND_ERR, "Comm sent err");
		respcodeArry.put(EL_SDK_RET_COMM_RECV_ERR, "Comm recv err");
		respcodeArry.put(EL_SDK_RET_FILE_NOT_EXIST, "File does not exist");
		
		respcodeArry.put(EL_SDK_PROTO_RET_BASE, "Proto err");
		respcodeArry.put(EL_SDK_RET_PROTO_GENERAL_ERR, "General err");
		respcodeArry.put(EL_SDK_RET_PROTO_ARG_ERR, "Argument err");
		respcodeArry.put(EL_SDK_RET_PROTO_PACKET_TOO_LONG, "Packet too long");
		respcodeArry.put(EL_SDK_RET_PROTO_NO_ENOUGH_DATA, "Receive data not enough");
		respcodeArry.put(EL_SDK_RET_PROTO_DATA_FORMAT, "Data format err");
		respcodeArry.put(EL_SDK_RET_PROTO_TIMEOUT, "Timeout");

	}
	
	public static String getRespCodeMsg(DataFormat dataRecv){
	
		return respcodeArry.get(dataRecv.getStatus()) != null ? respcodeArry.get(dataRecv.getStatus()) : "unknow err";
	
	}
	
	public static String getRespCodeMsg(int code){
		
		return respcodeArry.get(code) != null ? respcodeArry.get(code) : "unknow err";
		
	}
}
