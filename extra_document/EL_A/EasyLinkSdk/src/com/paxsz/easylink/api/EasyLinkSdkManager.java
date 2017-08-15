package com.paxsz.easylink.api;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import java.util.ArrayList;

import com.pax.commonlib.comm.BluetoothScan;
import com.pax.commonlib.comm.CommBluetooth;
import com.pax.commonlib.comm.CommTcp;
import com.pax.commonlib.comm.CommUsbHost;
import com.pax.commonlib.comm.IComm;
import com.pax.commonlib.comm.BluetoothScan.BtDevice;
import com.pax.commonlib.comm.BluetoothScan.BtScanListener;
import com.pax.commonlib.comm.IComm.ConnectStatus;
import com.pax.commonlib.convert.Convert;
import com.pax.commonlib.init.CommonLibInit;
import com.pax.commonlib.log.AppDebug;
import com.pax.mpos.proto.baifutong.api.BaiFuTong;
import com.pax.mpos.proto.baifutong.api.BaiFuTongException;
import com.paxsz.easylink.listener.FileDownloadListener;
import com.paxsz.easylink.listener.SearchDeviceListener;
import com.paxsz.easylink.model.DataModel.DataType;
import com.paxsz.easylink.model.DataModel.ProtocolType;
import com.paxsz.easylink.model.ShowPageInfo;
import com.paxsz.easylink.model.UIRespInfo;
import com.paxsz.easylink.protocol.Cmd;
import com.paxsz.easylink.protocol.DataFormat;
import com.paxsz.easylink.device.DeviceInfo;
import com.paxsz.easylink.protocol.Cmd.Command;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.hardware.usb.UsbDevice;

import android.util.Log;

/**
 *
 * @author huangjs
 *
 */
public class EasyLinkSdkManager extends Activity {

	private static final String TAG = "EasyLinkSdkManager";

	private Context context;
	private static EasyLinkSdkManager instance;

	private String btLastConnectedAddress;
	private IComm commbt = null;
	private IComm commtcp = null;
	private IComm commusb = null;
	private BaiFuTong comm = null;


	public static final int CONN_TIMEOUT_DEFAULT = 30000; // ms

	// USB
	private DeviceInfo.CommType commtype = null;

	ArrayList<CommUsbHost.UsbDeviceInfo> usbDevList = null;

	/**
	 * EasyLink SDK Manager constructor
	 *
	 * @param context context
	 */
	private EasyLinkSdkManager(Context context) {
		this.context = context;
		Cmd.initCmdTable();
		CommonLibInit.init(context);
		ResponseCode.initRespCodeTable();
		AppDebug.debug(true);
	}

	/**
	 * Create a EasyLinkSdkManager instance with the given context
	 *
	 * @param context
	 *            context
	 * @return EasyLinkSdkManager instance
	 */
	public static EasyLinkSdkManager getInstance(Context context) {
		if (instance == null) {
			instance = new EasyLinkSdkManager(context);
		}
		return instance;
	}

	private DataFormat dataSendAndRecv(DataFormat dataSend, int timeoutMs) {

		byte[] sendValue = dataSend.packReqBytes();
		AppDebug.i(TAG, Convert.bcd2Str(sendValue));
		byte[] recvValue = null;
		try {
			comm.send(sendValue, timeoutMs);
			recvValue = comm.recv(timeoutMs);
		} catch (BaiFuTongException e) {
			Log.e(TAG, "",e);
		}
		if(recvValue != null){
			Log.i(TAG, Convert.bcd2Str(recvValue));
		}
		DataFormat dataRecv = new DataFormat();
		dataRecv.setRspData(recvValue);
		return dataRecv;
	}

	/**
	 * Search devices
	 *
	 * @param dsl
	 *            Implement listener SearchDeviceListener
	 * @param timeout
	 *            timeout for searching devices
	 */
	public void searchDevices(final SearchDeviceListener dsl, long timeout) {
		final BluetoothAdapter mBtAdapter = BluetoothAdapter
				.getDefaultAdapter();
		while (!mBtAdapter.isEnabled()) {
			mBtAdapter.enable();
		}
		BtScanListener btScanListener = new BtScanListener() {

			@Override
			public void onFinished() {
				dsl.discoverComplete();
			}

			@Override
			public void onDiscovered(BtDevice device) {
				DeviceInfo deviceInfo = new DeviceInfo(
						DeviceInfo.CommType.BLUETOOTH, device.getName(),
						device.getAddress());
				dsl.discoverOneDevice(deviceInfo);
			}
		};
		BluetoothScan.getInstance(context, btScanListener,
				(int) (timeout / 1000)).start();
	}

	/**
	 * check if connected
	 *
	 * @return true-connected, false-no connected
	 */
	private boolean isConnected() {
		if (commtype == DeviceInfo.CommType.BLUETOOTH) {
			return commbt == null ? false : commbt.getConnectStatus() == ConnectStatus.CONNECTED ? true : false;
		} else if (commtype == DeviceInfo.CommType.TCP) {
			return commtcp == null ? false : commtcp.getConnectStatus() == ConnectStatus.CONNECTED ? true : false;
		} else if (commtype == DeviceInfo.CommType.USB) {
			return commusb == null ? false : commusb.getConnectStatus() == ConnectStatus.CONNECTED ? true : false;
		}
		return false;
	}

	/**
	 * Build connection
	 *
	 * @param devInfo
	 *            Device information <div>
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_COMM_RET_CONNECTED}
	 *         - already connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_COMM_RET_DISCONNECT_FAIL}
	 *         - disconnect failed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_COMM_RET_NOTCONNECTED}
	 *         - not connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_COMM_RET_PARAM_FILE_NOT_EXIST}
	 *         - POS terminal isn't initialized, need to download parameter
	 *         files
	 *         <p>
	 *         </div>
	 */
	public synchronized int connect(final DeviceInfo devInfo) {
		commtype = devInfo.getCommtype();
		int ret;
		if (commtype == DeviceInfo.CommType.BLUETOOTH) {
			ret = connectBT(devInfo);
			return ret;
		} else if (commtype == DeviceInfo.CommType.TCP) {
			ret = connectTCP(devInfo);
			return ret;
		} else if (commtype == DeviceInfo.CommType.USB) {
			ret = connectUSB(devInfo);
			return ret;
		} else {
			return ResponseCode.EL_SDK_RET_PARAM_INVALID;
		}

	}
	private int connectBT(final DeviceInfo devInfo) {
		Boolean isNeedConnected = true;
		if(isConnected())
		{
			if (devInfo.getIdentifier().isEmpty()) {
				return ResponseCode.EL_SDK_RET_PARAM_INVALID;
			}
			if (!devInfo.getIdentifier().equals(btLastConnectedAddress)) {
				disconnect();
			} else {
				isNeedConnected = false;
			}
		}
		try {
			Boolean result = true;
			if (isNeedConnected == true) {
				commbt = new CommBluetooth(context, devInfo.getIdentifier());
				comm = BaiFuTong.getInstance(context);
				comm.setComm(commbt);
				result = comm.connect();
			}
			if (result) {
				Cmd cmd = Cmd.getCmdTable().get(Command.CMD_COMM_CONNECT);
				DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE());
				DataFormat dataRecv = dataSendAndRecv(dataSend,
						10000);
				if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
					return dataRecv.getStatus();
				}
				btLastConnectedAddress = devInfo.getIdentifier();
				return 0;
			} else {
				return ResponseCode.EL_SDK_RET_COMM_CONNECT_ERR;
			}
		} catch (BaiFuTongException e) {
			Log.e(TAG, "",e);
			return ResponseCode.EL_SDK_RET_COMM_CONNECT_ERR;
		}

	}
	private int connectTCP(final DeviceInfo devInfo) {

		Boolean result = true;
		if (isConnected()) {
			return 0;
		}
		String host = devInfo.getIp();
		int port = devInfo.getPort();

		commtcp = new CommTcp(host, port, 10000, 20000);
		comm = BaiFuTong.getInstance(context);
		comm.setComm(commtcp);
		try {
			result = comm.connect();
		} catch (BaiFuTongException e) {
			Log.e(TAG, "",e);
		}
		if (result) {
			Cmd cmd = Cmd.getCmdTable().get(Command.CMD_COMM_CONNECT);
			DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE());
			DataFormat dataRecv;
			dataRecv = dataSendAndRecv(dataSend, 10000);

			if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
				return dataRecv.getStatus();
			}
			return 0;
		} else {
			return ResponseCode.EL_SDK_RET_COMM_CONNECT_ERR;
		}
	}
	private int connectUSB(final DeviceInfo devInfo) {

		Boolean result = true;
		if (isConnected()) {
			return 0;
		}
		// IGetPeerDevice
		CommUsbHost manager = new CommUsbHost(context);
		UsbDevice paxDevice = null;
		usbDevList = manager.getPeerDevice();
		Log.d(TAG, "dev count = " + usbDevList.size());

		if (usbDevList.isEmpty()) {
			Log.i(TAG, "No device found");
		}

		for (int i = 0; i < usbDevList.size(); i++) {

			CommUsbHost.UsbDeviceInfo temp = usbDevList.get(i);
			AppDebug.d(TAG, "dName: " + temp.getDevice().getDeviceName());
			AppDebug.d(TAG, "vid: " + temp.getDevice().getVendorId());
			AppDebug.d(TAG, "pid: " + temp.getDevice().getProductId());

			if (temp.getDevice().getVendorId() == CommUsbHost.PAX_VID
					&& temp.getDevice().getProductId() == CommUsbHost.PAX_PID) {
				paxDevice = temp.getDevice();
			}

			for (int j = 0; j < temp.getDeviceInterfaces().size(); j++) {
				CommUsbHost.UsbDeviceInterface intf = temp
						.getDeviceInterfaces().get(j);
				AppDebug.d(TAG, "intf" + i
						+ "-usbInterface-getInterfaceClass : "
						+ intf.interfaceClass);
				AppDebug.d(TAG, "intf" + i
						+ "-usbInterface-getInterfaceSubclass : "
						+ intf.interfaceSubclass);
				AppDebug.d(TAG, "intf" + i
						+ "-usbInterface-getInterfaceProtocol : "
						+ intf.interfaceProtocol);
			}
		}

		// connect usb device
		if (paxDevice == null) {
			Log.i(TAG, "No pax device found, connect error");
			return ResponseCode.EL_SDK_RET_COMM_CONNECT_ERR;
		}
		manager.setPaxSpecialUsbDevice(paxDevice);
		commusb = manager;

		comm = BaiFuTong.getInstance(context);
		comm.setComm(commusb);
		try {
			result = comm.connect();
		} catch (BaiFuTongException e) {
			Log.e(TAG, "",e);
		}
		if (result) {
			Cmd cmd = Cmd.getCmdTable().get(Command.CMD_COMM_CONNECT);
			DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE());
			DataFormat dataRecv;
			dataRecv = dataSendAndRecv(dataSend, 10000);

			if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
				return dataRecv.getStatus();
			}
			return 0;
		} else {
			return ResponseCode.EL_SDK_RET_COMM_CONNECT_ERR;
		}
	}

	/**
	 * Disconnect
	 * <div>
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_COMM_RET_DISCONNECT_FAIL}
	 *         - disconnect failed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_COMM_RET_NOTCONNECTED}
	 *         - not connected
	 *         <p>
	 *         </div>
	 */
	public synchronized int disconnect() {

		if (!isConnected()) {
			return ResponseCode.EL_COMM_RET_NOTCONNECTED;
		}
		try {
			Cmd cmd = Cmd.getCmdTable().get(Command.CMD_COMM_DISCONNECT);
			DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE());
			DataFormat dataRecv = dataSendAndRecv(dataSend, 10000);

			if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
				return dataRecv.getStatus();
			}
			comm.close();
			return 0;
		} catch (BaiFuTongException e) {
			Log.e(TAG, "",e);
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECT_ERR;
		}
	}

	/**
	 * Start Transaction
	 * <p>
	 * Detect card(MSR, EMV CHIP, EMV Contactless), and do corresponding
	 * processing.
	 * <p>
	 * Read track1, track2, track3 data, if MSR is swiped
	 * <p>
	 * Application selection, initial processing, terminal risk management, ODA,
	 * CVM, 1st GAC, if EMV Chip card is inserted
	 * <p>
	 * Application selection, get final selected application data,
	 * Paywave/Mastercard contactless/... processing, restriction processing,
	 * ODA, CVM, ..., if EMV contactless card is tapped
	 * <div>
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_DISCONNECTED}
	 *         - Not connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_PARAM_INVALID}
	 *         - Invalid parameter
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_ICC_RESET_ERR}
	 *         - IC card reset is failed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_ICC_CMD_ERR}
	 *         - IC card command is failed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_ICC_BLOCK}
	 *         - IC card is blocked
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_RSP_ERR}
	 *         - Status Code returned by IC card is not 9000
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_APP_BLOCK}
	 *         - The Application selected is blocked
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NO_APP}
	 *         - There is no AID matched between ICC and terminal
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_USER_CANCEL}
	 *         - The Current operation or transaction was canceled by user
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_TIME_OUT}
	 *         - User鈥檚 operation is timeout
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_DATA_ERR}
	 *         - Data error is found
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NOT_ACCEPT}
	 *         - Transaction is not accepted
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_DENIAL}
	 *         - Transaction is denied
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_KEY_EXP}
	 *         - Certification Authority Public Key is Expired
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NO_PINPAD}
	 *         - PIN enter is required, but PIN pad is not present or not
	 *         working
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NO_PASSWORD}
	 *         - PIN enter is required, PIN pad is present, but there is no PIN
	 *         entered
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_SUM_ERR}
	 *         - Checksum of CAPK is error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NOT_FOUND}
	 *         - Appointed Data Element can鈥檛 be found
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NO_DATA}
	 *         - The length of the appointed Data Element is 0
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_OVERFLOW}
	 *         - Memory is overflow
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_NO_TRANS_LOG}
	 *         - There is no Transaction log
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_RECORD_NOTEXIST}
	 *         - Appointed log is not existed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_LOGITEM_NOTEXIST}
	 *         - Appointed Label is not existed in current log record
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_ICC_RSP_6985}
	 *         - Status Code returned by IC card for GPO is 6985
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_USE_CONTACT}
	 *         - Must use other interface for the transaction
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_FILE_ERR}
	 *         - There is file error found
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_TERMINATE}
	 *         - Must terminate the transaction
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_FAILED}
	 *         - Contactless transaction is failed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_DECLINE}
	 *         - Transaction should be declined.
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_TRY_ANOTHER_CARD}
	 *         - Try another card (DPAS Only)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_PARAM_ERR}
	 *         - Parameter is error = EMV_PARAM_ERR
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_WAVE2_OVERSEA}
	 *         - International transaction(for VISA AP PayWave Level2 IC card
	 *         use)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_WAVE2_TERMINATED}
	 *         - Wave2 DDA response TLV format error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_WAVE2_US_CARD}
	 *         - US card(for VISA AP PayWave L2 IC card use)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_WAVE3_INS_CARD}
	 *         - Need to use IC card for the transaction(for VISA PayWave IC
	 *         card use)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_RESELECT_APP}
	 *         - Select the next AID in candidate list
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_CARD_EXPIRED}
	 *         - IC card is expired
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NO_APP_PPSE_ERR}
	 *         - No application is supported(Select PPSE is error)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_USE_VSDC}
	 *         - Switch to contactless PBOC
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_CVMDECLINE}
	 *         - CVM result in decline for AE
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_REFER_CONSUMER_DEVICE}
	 *         - Status Code returned by IC card is 6986, please see phone
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_LAST_CMD_ERR}
	 *         - The last read record command is error(qPBOC Only)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_API_ORDER_ERR}
	 *         - APIs are called in wrong order. Please call
	 *         Clss_GetDebugInfo_xxx to get error codes.
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_FAIL}
	 *         - EMV transaction failed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_DECLINED}
	 *         - EMV transaction been declined
	 *         <p>
	 *         </div>
	 */
	public synchronized int startTransaction() {
		if (!isConnected()) {
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECTED;
		}
		Cmd cmd = Cmd.getCmdTable().get(Command.CMD_TRANS_START);
		DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE(), null);
		DataFormat dataRecv;
		dataRecv = dataSendAndRecv(dataSend, 130000);

		if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
			return dataRecv.getStatus();
		}
		return 0;

	}

	/**
	 * Complete Transation Issuer data verification, perform script for EMV, EMV
	 * contactless card <div>
	 *
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_DISCONNECTED}
	 *         - Not connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_PARAM_INVALID}
	 *         - Invalid parameter
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_ICC_RESET_ERR}
	 *         - IC card reset is failed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_ICC_CMD_ERR}
	 *         - IC card command is failed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_ICC_BLOCK}
	 *         - IC card is blocked
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_RSP_ERR}
	 *         - Status Code returned by IC card is not 9000
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_APP_BLOCK}
	 *         - The Application selected is blocked
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NO_APP}
	 *         - There is no AID matched between ICC and terminal
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_USER_CANCEL}
	 *         - The Current operation or transaction was canceled by user
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_TIME_OUT}
	 *         - User鈥檚 operation is timeout
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_DATA_ERR}
	 *         - Data error is found
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NOT_ACCEPT}
	 *         - Transaction is not accepted
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_DENIAL}
	 *         - Transaction is denied
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_KEY_EXP}
	 *         - Certification Authority Public Key is Expired
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NO_PINPAD}
	 *         - PIN enter is required, but PIN pad is not present or not
	 *         working
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NO_PASSWORD}
	 *         - PIN enter is required, PIN pad is present, but there is no PIN
	 *         entered
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_SUM_ERR}
	 *         - Checksum of CAPK is error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NOT_FOUND}
	 *         - Appointed Data Element can鈥檛 be found
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NO_DATA}
	 *         - The length of the appointed Data Element is 0
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_OVERFLOW}
	 *         - Memory is overflow
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_NO_TRANS_LOG}
	 *         - There is no Transaction log
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_RECORD_NOTEXIST}
	 *         - Appointed log is not existed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_LOGITEM_NOTEXIST}
	 *         - Appointed Label is not existed in current log record
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_ICC_RSP_6985}
	 *         - Status Code returned by IC card for GPO is 6985
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_USE_CONTACT}
	 *         - Must use other interface for the transaction
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_FILE_ERR}
	 *         - There is file error found
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_TERMINATE}
	 *         - Must terminate the transaction
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_FAILED}
	 *         - Contactless transaction is failed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_DECLINE}
	 *         - Transaction should be declined.
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_TRY_ANOTHER_CARD}
	 *         - Try another card (DPAS Only)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_PARAM_ERR}
	 *         - Parameter is error = EMV_PARAM_ERR
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_WAVE2_OVERSEA}
	 *         - International transaction(for VISA AP PayWave Level2 IC card
	 *         use)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_WAVE2_TERMINATED}
	 *         - Wave2 DDA response TLV format error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_WAVE2_US_CARD}
	 *         - US card(for VISA AP PayWave L2 IC card use)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_WAVE3_INS_CARD}
	 *         - Need to use IC card for the transaction(for VISA PayWave IC
	 *         card use)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_RESELECT_APP}
	 *         - Select the next AID in candidate list
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_CARD_EXPIRED}
	 *         - IC card is expired
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_NO_APP_PPSE_ERR}
	 *         - No application is supported(Select PPSE is error)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_USE_VSDC}
	 *         - Switch to contactless PBOC
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_CVMDECLINE}
	 *         - CVM result in decline for AE
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_REFER_CONSUMER_DEVICE}
	 *         - Status Code returned by IC card is 6986, please see phone
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_LAST_CMD_ERR}
	 *         - The last read record command is error(qPBOC Only)
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_CLSS_API_ORDER_ERR}
	 *         - APIs are called in wrong order. Please call
	 *         Clss_GetDebugInfo_xxx to get error codes.
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_FAIL}
	 *         - EMV transaction failed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_TRANS_RET_EMV_DECLINED}
	 *         - EMV transaction been declined
	 *         <p>
	 *         </div>
	 */
	public synchronized int completeTransation() {
		if (!isConnected()) {
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECTED;
		}
		Cmd cmd = Cmd.getCmdTable().get(Command.CMD_TRANS_COMPLETE);
		DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE(), null);
		DataFormat dataRecv;

		dataRecv = dataSendAndRecv(dataSend, 10000);

		if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
			return dataRecv.getStatus();
		}
		return 0;
	}

	/**
	 * Get PinBlock
	 *
	 * @param PAN
	 *            non-shifted PAN data, or "", if "", then POS terminal will use
	 *            the PAN data stored (which read in StartTransaction API) to
	 *            calculate the PINBLOCK
	 * @param pinBlock
	 *            out parameter,pin block
	 * @param ksn
	 *            out parameter <div>
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - Success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_DISCONNECTED}
	 *         - Not connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_PARAM_INVALID}
	 *         - Invalid parameter
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_NO_KEY}
	 *         - Key doesn't exist
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_PARAM_INVALID}
	 *         - Parameter error or invalid
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_GET_PIN_BLOCK_ERR}
	 *         - Get pin block error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_NO_PIN_INPUT}
	 *         - Not input pin
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_INPUT_CANCEL}
	 *         - user cancel
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_INPUT_TIMEOUT}
	 *         - input timeout
	 *         <p>
	 *         </div>
	 */
	public synchronized int getPinBlock(final String PAN, ByteArrayOutputStream pinBlock,
										ByteArrayOutputStream ksn) {
		if (!isConnected()) {
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECTED;
		}
		byte[] data = new byte[1 + PAN.length()];
		data[0] = (byte) PAN.length();
		System.arraycopy(PAN.getBytes(), 0, data, 1, PAN.length());
		Cmd cmd = Cmd.getCmdTable().get(Command.CMD_GET_PIN_BLOCK);
		DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE(), data);
		DataFormat dataRecv;

		dataRecv = dataSendAndRecv(dataSend, 70000);

		if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
			return dataRecv.getStatus();
		}
		byte[] respData = dataRecv.getRspData();
		if (respData == null || respData.length < 8) {
			return ResponseCode.EL_SDK_RET_COMM_RECV_ERR;
		}
		byte[] bPinBlock = new byte[8];
		byte[] bKsn = new byte[10];
		System.arraycopy(respData, 0, bPinBlock, 0, 8);
		if (respData.length == 18) {
			System.arraycopy(respData, 8, bKsn, 0, 10);
		}
		try {
			pinBlock.write(bPinBlock);
			ksn.write(bKsn);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			Log.e(TAG, "",e);
		}
		return 0;
	}

	/**
	 * Encrypt data
	 *
	 * @param data
	 *            data to be encrypted
	 * @param encryptDataValue
	 *            encrypted data,out parameter <div>
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - Success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_DISCONNECTED}
	 *         - Not connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_PARAM_INVALID}
	 *         - Invalid parameter
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_NO_KEY}
	 *         - Key doesn't exist
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_PARAM_INVALID}
	 *         - Parameter error or invalid
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_ENCRYPT_DATA_ERR}
	 *         - Encrypt data error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_INPUT_CANCEL}
	 *         - user cancel
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SECURITY_RET_INPUT_TIMEOUT}
	 *         - input timeout
	 *         <p>
	 *         </div>
	 */

	public synchronized int encryptData(final byte[] data,
										ByteArrayOutputStream encryptDataValue) {
		if (!isConnected()) {
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECTED;
		}
		if (data == null) {
			return ResponseCode.EL_SDK_RET_PARAM_INVALID;
		}
		byte[] dataBuf = new byte[2 + data.length];

		// dataLen
		byte[] buf = new byte[2];
		Convert.short2ByteArray((short) data.length, buf, 0);
		System.arraycopy(buf, 0, dataBuf, 0, 2);
		// data
		System.arraycopy(data, 0, dataBuf, 2, data.length);
		Cmd cmd = Cmd.getCmdTable().get(Command.CMD_ENCRYPT_DATA);
		DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE(), dataBuf);
		DataFormat dataRecv;

		dataRecv = dataSendAndRecv(dataSend, 10000);

		if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
			return dataRecv.getStatus();
		}
		byte[] respData = dataRecv.getRspData();
		if (respData == null) {
			return ResponseCode.EL_SDK_RET_COMM_RECV_ERR;
		}
		try {
			encryptDataValue.write(respData);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			Log.e(TAG, "",e);
		}
		return 0;
	}

	/**
	 * SetData - Set up the TAG data (Terminal application parameter,
	 * Transaction parameter, EMV, EMV contactless)
	 *
	 * @param datatype
	 *            TRANSACTIONDATA:1 CONFIGURATIONDATA:2
	 * @param datalist
	 *            TLV data list
	 * @param failedTags
	 *            out parameter,failed tags <div>
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - Success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_DISCONNECTED}
	 *         - Not connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_PARAM_INVALID}
	 *         - Invalid parameter
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_ERR_DATA}
	 *         - Data error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_INVALID_PARAM}
	 *         - Parameter error or invalid
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_PARTIAL_FAILED}
	 *         - Partial failed when multi-TLV list are been set
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_ALL_FAILED}
	 *         - All failed when multi-TLV list are been set
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_SIZE_SMALL}
	 *         - Size small
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_API_ORDER_ERR}
	 *         - APIs are called in wrong order
	 *         <p>
	 *         </div>
	 */
	public synchronized int setData(final DataType datatype, final byte[] datalist,
									ByteArrayOutputStream failedTags) {
		if (!isConnected()) {
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECTED;
		}
		if (datalist == null) {
			return ResponseCode.EL_SDK_RET_PARAM_INVALID;
		}
		int offset = 0;
		byte[] dataBuf = new byte[1 + 2 + datalist.length];

		dataBuf[offset] = (byte) datatype.toValue();// Ocean add 20170417

		offset += 1;

		// dataLen
		byte[] buf = new byte[2];
		Convert.short2ByteArray((short) datalist.length, buf, 0);
		System.arraycopy(buf, 0, dataBuf, offset, 2);
		offset += 2;

		// data
		System.arraycopy(datalist, 0, dataBuf, offset, datalist.length);

		Cmd cmd = Cmd.getCmdTable().get(Command.CMD_PARAM_SET_DATA);

		DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE(), dataBuf);
		DataFormat dataRecv;

		dataRecv = dataSendAndRecv(dataSend, 10000);

		if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
			byte[] respData = dataRecv.getRspData();
			if (respData != null && respData.length != 0) {
				try {
					failedTags.write(respData);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					Log.e(TAG, "", e);
				}
			}
			return dataRecv.getStatus();
		}
		return 0;
	}

	/**
	 * GetData - Get value the TAG list (Terminal application parameter,
	 * Transaction parameter, EMV, EMV contactless)
	 *
	 * @param datatype
	 *            TRANSACTIONDATA, CONFIGURATIONDATA
	 * @param tagList
	 *            TAG list to be gotten
	 * @param dataList
	 *            out parameter, tlv data list <div>
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - Success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_DISCONNECTED}
	 *         - Not connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_PARAM_INVALID}
	 *         - Invalid parameter
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_ERR_DATA}
	 *         - Data error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_INVALID_PARAM}
	 *         - Parameter error or invalid
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_PARTIAL_FAILED}
	 *         - Partial failed when multi-TLV list are been gotten
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_ALL_FAILED}
	 *         - All failed when multi-TLV list are been gotten
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_SIZE_SMALL}
	 *         - Size small
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_API_ORDER_ERR}
	 *         - APIs are called in wrong order
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_PARAM_RET_ENCRYPT_SENSITIVE_DATA_FAILED}
	 *         - Encrypt data failed when get sensitive data from terminal
	 *         <p>
	 *         </div>
	 */
	public synchronized int getData(final DataType datatype, final byte[] tagList,
									ByteArrayOutputStream dataList) {
		if (!isConnected()) {
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECTED;
		}
		if (tagList == null) {
			return ResponseCode.EL_SDK_RET_PARAM_INVALID;
		}
		int offset = 0;
		byte[] dataBuf = new byte[1 + 2 + tagList.length];

		dataBuf[offset] = (byte) datatype.toValue();// Ocean add 20170417

		offset += 1;

		// dataLen
		byte[] buf = new byte[2];
		Convert.short2ByteArray((short) tagList.length, buf, 0);
		System.arraycopy(buf, 0, dataBuf, offset, 2);
		offset += 2;

		// data
		System.arraycopy(tagList, 0, dataBuf, offset, tagList.length);

		Cmd cmd = Cmd.getCmdTable().get(Command.CMD_PARAM_GET_DATA);
		DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE(), dataBuf);
		DataFormat dataRecv;

		dataRecv = dataSendAndRecv(dataSend, 10000);

		if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
			return dataRecv.getStatus();
		}
		byte[] respData = dataRecv.getRspData();
		if (respData == null) {
			return ResponseCode.EL_SDK_RET_COMM_RECV_ERR;
		}
		try {
			dataList.write(respData);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			Log.e(TAG, "",e);
		}
		return 0;
	}

	/**
	 * Show message which configured in UI XML file in POS terminal
	 *
	 * @param pageName
	 *            The name of page which configured in UI XML file. To show the
	 *            page;
	 * @param timeOutMs
	 *            time out
	 * @param showPageInfo
	 *            Set <widget name - text> arrayList, if need to change the
	 *            displaying text of widget configured in UI XML
	 *            <p>
	 *            eg. below is the configuration of textbox in UI XML file:
	 *            <p>
	 *            &lt;TextBox name="prompt1" x="0" y="0" value="PAX"
	 *            fontSize="0" textAlign="1" mode="0"
	 *            keyAccept="1">&lt;/TextBox
	 *            <p>
	 *            If need to change the displaying text, set the
	 *            showMsgInfo.widgetName = "prompt1", showMsgInfo.text =
	 *            "PLS REMOVE CARD"; <div>
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - Success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_DISCONNECTED}
	 *         - Not connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_PARAM_INVALID}
	 *         - Invalid parameter
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_UI_RET_INVALID_WIDGETNAME}
	 *         - Invalid widget name
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_UI_RET_TIME_OUT} -
	 *         Timeout
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_UI_RET_INVALID_PAGE}
	 *         - Invalid page
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_UI_RET_PARSEUI_FAILED}
	 *         - Fail to parse UI parameter file
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_UI_RET_VALUESIZEERROR}
	 *         - Invalid value in UI parameter file
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_UI_RET_INPUT_TYPE_ERRO}
	 *         - Invalid value in UI parameter file for InputBox
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_UI_RET_INVALID_WIDGETVALUE}
	 *         - Invalid value in UI parameter file for widget
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_UI_RET_USER_CANCEL}
	 *         - User cancel
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_UI_RET_MENUITEMNUM_ERROR}
	 *         - Invalid value in UI parameter file for Menu
	 *         <p>
	 *         </div>
	 */
	public synchronized int showPage(final String pageName, final int timeOutMs,
									 ArrayList<ShowPageInfo> showPageInfo) {

		if (!isConnected()) {
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECTED;
		}
		if (pageName.isEmpty()) {
			return ResponseCode.EL_SDK_RET_PARAM_INVALID;
		}

		int dataLen = 4 + 1 + pageName.length() + 2;// dataLen +
		// pageNameLen+pageName+timeOut
		int widgetNum = showPageInfo.size();
		if (widgetNum == 0) {
			dataLen += 2;// widgetNameLen+textLen
		}
		// 获取data长度
		for (int i = 0; i < widgetNum; i++) {
			ShowPageInfo msgInfo = showPageInfo.get(i);
			int widgetNameLen = msgInfo.getWidgetName().isEmpty() ? 0
					: msgInfo.getWidgetName().length();
			int textLen = msgInfo.getText().isEmpty() ? 0 : msgInfo.getText().length();
			dataLen += (1 + widgetNameLen + 1 + textLen);
		}

		byte[] data = new byte[dataLen];
		int offset = 0;

		// dataLen
		byte[] dataBuf = new byte[4];
		Convert.int2ByteArray(dataLen - 4, dataBuf, 0);
		System.arraycopy(dataBuf, 0, data, offset, 4);
		offset += 4;
		// pageNameLen
		data[offset] = (byte) pageName.length();
		offset += 1;
		// pageName
		System.arraycopy(pageName.getBytes(), 0, data, offset,
				pageName.length());
		offset += pageName.length();
		// timeout
		byte[] buf = new byte[4];
		Convert.int2ByteArray(timeOutMs, buf, 0);
		System.arraycopy(buf, 2, data, offset, 2);
		offset += 2;
		if (widgetNum == 0) {
			data[offset] = (byte) 0;
			offset += 1;
			data[offset] = (byte) 0;// textLen
			offset += 1;
		}

		for (int i = 0; i < widgetNum; i++) {
			ShowPageInfo msgInfo = showPageInfo.get(i);
			int widgetNameLen = msgInfo.getWidgetName().isEmpty() ? 0
					: msgInfo.getWidgetName().length();
			int textLen = msgInfo.getText().isEmpty() ? 0 : msgInfo.getText().length();

			// widgetNameLen
			data[offset] = (byte) widgetNameLen;
			offset += 1;
			if (widgetNameLen > 0) {
				// widgetName
				System.arraycopy(msgInfo.getWidgetName().getBytes(), 0, data,
						offset, widgetNameLen);
				offset += widgetNameLen;
			}
			// textLen
			data[offset] = (byte) textLen;
			offset += 1;
			// text
			if (textLen > 0) {
				System.arraycopy(msgInfo.getText().getBytes(), 0, data, offset,
						textLen);
				offset += textLen;
			}
		}
		Cmd cmd = Cmd.getCmdTable().get(Command.CMD_UI_SHOW_PAG);
		DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE(), data);
		DataFormat dataRecv;

		dataRecv = dataSendAndRecv(dataSend, 60000);

		if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
			return dataRecv.getStatus();
		}
		return 0;
	}
	public synchronized int showPage(final String pageName, final int timeOutMs,
									 ArrayList<ShowPageInfo> showPageInfo, UIRespInfo uiRespInfo) {

		if (!isConnected()) {
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECTED;
		}
		if (pageName.isEmpty()) {
			return ResponseCode.EL_SDK_RET_PARAM_INVALID;
		}

		int dataLen = 4 + 1 + pageName.length() + 2;// dataLen +
		// pageNameLen+pageName+timeOut
		int widgetNum = showPageInfo.size();
		if (widgetNum == 0) {
			dataLen += 2;// widgetNameLen+textLen
		}
		// 获取data长度
		for (int i = 0; i < widgetNum; i++) {
			ShowPageInfo msgInfo = showPageInfo.get(i);
			int widgetNameLen = msgInfo.getWidgetName().isEmpty() ? 0
					: msgInfo.getWidgetName().length();
			int textLen = msgInfo.getText().isEmpty() ? 0 : msgInfo.getText().length();
			dataLen += (1 + widgetNameLen + 1 + textLen);
		}

		byte[] data = new byte[dataLen];
		int offset = 0;

		// dataLen
		byte[] dataBuf = new byte[4];
		Convert.int2ByteArray(dataLen - 4, dataBuf, 0);
		System.arraycopy(dataBuf, 0, data, offset, 4);
		offset += 4;
		// pageNameLen
		data[offset] = (byte) pageName.length();
		offset += 1;
		// pageName
		System.arraycopy(pageName.getBytes(), 0, data, offset,
				pageName.length());
		offset += pageName.length();
		// timeout
		byte[] buf = new byte[4];
		Convert.int2ByteArray(timeOutMs, buf, 0);
		System.arraycopy(buf, 2, data, offset, 2);
		offset += 2;
		if (widgetNum == 0) {
			data[offset] = (byte) 0;
			offset += 1;
			data[offset] = (byte) 0;// textLen
			offset += 1;
		}

		for (int i = 0; i < widgetNum; i++) {
			ShowPageInfo msgInfo = showPageInfo.get(i);
			int widgetNameLen = msgInfo.getWidgetName().isEmpty() ? 0
					: msgInfo.getWidgetName().length();
			int textLen = msgInfo.getText().isEmpty() ? 0 : msgInfo.getText().length();

			// widgetNameLen
			data[offset] = (byte) widgetNameLen;
			offset += 1;
			if (widgetNameLen > 0) {
				// widgetName
				System.arraycopy(msgInfo.getWidgetName().getBytes(), 0, data,
						offset, widgetNameLen);
				offset += widgetNameLen;
			}
			// textLen
			data[offset] = (byte) textLen;
			offset += 1;
			// text
			if (textLen > 0) {
				System.arraycopy(msgInfo.getText().getBytes(), 0, data, offset,
						textLen);
				offset += textLen;
			}
		}
		Cmd cmd = Cmd.getCmdTable().get(Command.CMD_UI_SHOW_PAG);
		DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE(), data);
		DataFormat dataRecv;

		dataRecv = dataSendAndRecv(dataSend, 60000);
		if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
			return dataRecv.getStatus();
		}
		byte[] respData = dataRecv.getRspData();
		if(respData != null && respData.length >= 3){
			uiRespInfo.setType(getEUIActionType(respData[0]));
			byte[] uidata = new byte[respData.length - 3];
			System.arraycopy(respData, 3, uidata, 0 ,respData.length - 3);
			uiRespInfo.setActionData(uidata);
		}
		return 0;
	}
	private UIRespInfo.EUIActionType getEUIActionType(byte eUIActionType){
		if(eUIActionType == 4){
			return UIRespInfo.EUIActionType.UI_ACTION_SIGNATURE;
		}else{
			return UIRespInfo.EUIActionType.UI_ACTION_COMMENT_STAR;
		}
	}
	private byte[] getFileContent(String filePath) {

		File file = new File(filePath);
		FileInputStream fis = null;

		try {
			fis = new FileInputStream(file);
			int length = fis.available();

			byte[] buffer = new byte[length];
			int recv = fis.read(buffer);
			Log.i(TAG, "fileInputStream recv code:" + recv);
			fis.close();
			return buffer;
		} catch (IOException e) {
			Log.e(TAG, "",e);
		}

		return new byte[0];
	}

	/**
	 * Send file to POS terminal
	 *
	 * @param fileName
	 *            Name of file to be sent
	 * @param filePath
	 *            Path of file
	 * @param listener
	 *            Implement listener FileDownloadListener <div>
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - Success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_DISCONNECTED}
	 *         - Not connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_PARAM_INVALID}
	 *         - Invalid parameter
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#FILEDOWNLOAD_ERR_INVALID_PARAM}
	 *         - Invalid parameter
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#FILEDOWNLOAD_ERR_DOWNLOAD_FILE_FAIL}
	 *         - Fail to download file
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#FILEDOWNLOAD_ERR_FILE_OVERSIZE}
	 *         - File over size
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#FILEDOWNLOAD_ERR_NOT_ALLOWED}
	 *         - File is not allowed
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#FILEDOWNLOAD_ERR_PARSE_ERR}
	 *         - Fail to parse the file downloaded
	 *         <p>
	 *         </div>
	 */
	public synchronized int fileDownLoad(final String fileName, final String filePath,
										 FileDownloadListener listener) {
		String fileNameTmp;

		if (!isConnected()) {
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECTED;
		}
		if (fileName == null || filePath == null) {
			return ResponseCode.EL_SDK_RET_PARAM_INVALID;
		}
		if (fileName.length() > 32) {
			fileNameTmp = fileName.substring(fileName.length() - 32);
		} else {
			fileNameTmp = fileName;
		}

		final byte[] fileData = getFileContent(filePath);

		if (fileData.length == 0) {
			return ResponseCode.EL_SDK_RET_FILE_NOT_EXIST;
		}

		int realSize = 0;
		int sendBytes = 0;
		int packSize = 1024;
		int fileSize = fileData.length;
		boolean isLast = false;

		while (true) {
			isLast = ((sendBytes + packSize) >= fileSize);
			if (packSize > (fileSize - sendBytes)) {
				realSize = fileSize - sendBytes;
			} else {
				realSize = packSize;
			}
			byte[] dataBuf = new byte[2 + fileNameTmp.length() + 4 + 4 + 4
					+ realSize];
			int offset = 0;
			// fileNameLen
			byte[] buf = new byte[2];
			Convert.short2ByteArray((short) fileNameTmp.length(), buf, 0);
			System.arraycopy(buf, 0, dataBuf, offset, 2);
			offset += 2;

			// fileName
			System.arraycopy(fileNameTmp.getBytes(), 0, dataBuf, offset,
					fileNameTmp.length());
			offset += fileNameTmp.length();

			// fileSize
			byte[] buf2 = new byte[4];
			Convert.int2ByteArray(fileSize, buf2, 0);
			System.arraycopy(buf2, 0, dataBuf, offset, 4);
			offset += 4;

			// offset
			byte[] buf3 = new byte[4];
			Convert.int2ByteArray(sendBytes, buf3, 0);
			System.arraycopy(buf3, 0, dataBuf, offset, 4);
			offset += 4;

			// curTransferLen
			byte[] buf4 = new byte[4];
			Convert.int2ByteArray(realSize, buf4, 0);
			System.arraycopy(buf4, 0, dataBuf, offset, 4);
			offset += 4;

			// fileData
			System.arraycopy(fileData, sendBytes, dataBuf, offset, realSize);
			listener.onDownloadProgress(sendBytes, fileSize);

			Cmd cmd = Cmd.getCmdTable().get(Command.CMD_TMS_DOWNLOAD_FILE);
			DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE(), dataBuf);
			DataFormat dataRecv;
			dataRecv = dataSendAndRecv(dataSend, 30000);

			if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
				return dataRecv.getStatus();
			}
			sendBytes += realSize;
			// succeeded
			if (isLast) {
				return ResponseCode.EL_RET_OK;
			}
			if(listener.cancelDownload()){
				return ResponseCode.EL_UI_RET_USER_CANCEL;
			}

		}
	}
	/**
	 * Switch communication mode
	 *
	 * @param protocolType
	 *            {@link com.paxsz.easylink.protocol.DataModel.ProtocolType}
	 * @return <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_RET_OK} - Success
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_DISCONNECTED}
	 *         - Not connected
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_PARAM_INVALID}
	 *         - Invalid parameter
	 *         <p>
	 *         {@link com.paxsz.easylink.api.ResponseCode#EL_SDK_RET_COMM_RECV_ERR}
	 *         - Receive response error
	 *         <p>
	 *         </div>
	 */
	public synchronized int switchCommMode(final ProtocolType protocolType) {
		if (!isConnected()) {
			return ResponseCode.EL_SDK_RET_COMM_DISCONNECTED;
		}
		byte[] dataBuf = new byte[1];
		dataBuf[0] = (byte) protocolType.toValue();
		Cmd cmd = Cmd.getCmdTable().get(Command.CMD_SWITCH_COMM_MODE);
		DataFormat dataSend = new DataFormat(cmd.getTYPE(), cmd.getCODE(), dataBuf);
		DataFormat dataRecv;

		dataRecv = dataSendAndRecv(dataSend, 10000);

		if (dataRecv.getStatus() != ResponseCode.EL_RET_OK) {
			return dataRecv.getStatus();
		}
		return 0;
	}
}