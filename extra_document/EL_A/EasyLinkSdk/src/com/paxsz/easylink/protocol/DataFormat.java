package com.paxsz.easylink.protocol;

import java.util.Arrays;

import com.pax.commonlib.convert.Convert;

public class DataFormat implements Cloneable {

	private byte type = 0;
	private byte code = 0;
	private byte[] reqdata = null;
	private byte[] rspData = null;
	private int status = -1;
	
	/**
	 * constructor
	 */
	public DataFormat() {
		super();
	}

	@Override
	protected Object clone() throws CloneNotSupportedException {
		return super.clone();
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + type;
		result = prime * result + code;
		result = prime * result + Arrays.hashCode(reqdata);
		result = prime * result + Arrays.hashCode(rspData);
		result = prime * result + status;
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		DataFormat other = (DataFormat) obj;
		if (type != other.type)
			return false;
		if (code != other.code)
			return false;
		if (!Arrays.equals(reqdata, other.reqdata))
			return false;
		if (!Arrays.equals(rspData, other.rspData))
			return false;
		if (status != other.status)
			return false;
		return true;
	}
	
	/**
	 * constructor
	 * @param type - types
	 * @param code - code
	 * @param data - data
	 */
	public DataFormat(byte type, byte code, byte[] data) {
		super();
		this.type = type;
		this.code = code;
		this.reqdata = data;
	} 

	/**
	 * constructor
	 * @param type - type
	 * @param code - code
	 */
	public DataFormat(byte type, byte code) {
		super();
		this.type = type;
		this.code = code;
		this.reqdata = null;
	}
	
	public byte gettype() {
		return type;
	}

	public byte getcode() {
		return code;
	}

	/**
	 * Set data to be sent
	 * @param data - data to be sent
	 */
	public void setReqdata(byte[] data) {
		this.reqdata = data;
	}

	/**
	 * Get response message
	 * @return - response message
	 */
	public String getRespMsg() {
		if (this.rspData == null || this.rspData.length < 6)
			return "";

		byte[] val;

		if (this.rspData.length > 6) {
			val = new byte[64];
			System.arraycopy(this.rspData, 6, val, 0, 64);
			return new String(val);
		}else {
			return "";
		}
	}
	
	/**
	 * Get response data
	 * @return response data
	 */
	public byte[] getRspData() {
		if (this.rspData == null || this.rspData.length < 70)
			return new byte[0];

		byte[] val = null;

		if (this.rspData.length > 70) {
			val = new byte[this.rspData.length - 70];
			System.arraycopy(this.rspData, 70, val, 0, this.rspData.length - 70);
		}

		return val;
	}

	/**
	 * Pack request data in DataFormat format
	 * @return	DataFormat data
	 */
	public byte[] packReqBytes() {
		if (this.reqdata != null) {
			int lc = reqdata.length;
			byte[] request = new byte[2 + lc];
			request[0] = this.type;
			request[1] = this.code;
			System.arraycopy(this.reqdata, 0, request, 2, lc);
			return request;
		} else {
			byte[] request = new byte[2];
			request[0] = this.type;
			request[1] = this.code;
			return request;
		}
	}

	/**
	 * Convert BCD data to String format
	 * @return	APDU hex string format
	 */
	public String packReqString() {
		byte[] data = this.packReqBytes();
		return Convert.bcd2Str(data);
	}
	
	/**
	 * Set response data
	 * @param dataAndStatus
	 */
	public void setRspData(byte[] dataAndStatus) {
		this.rspData = null;
		if (dataAndStatus == null || dataAndStatus.length < 6)
			return;

		this.rspData = new byte[dataAndStatus.length];
		System.arraycopy(dataAndStatus, 0, this.rspData, 0,
				dataAndStatus.length);

		this.status = Convert.byteArray2Int(dataAndStatus, 2);
	}

	/**
	 * Get response status
	 * @return status
	 */
	public int getStatus() {
		return status;
	}

	/**
	 * Get response status in String format
	 * @return statusString
	 */
	public String getStatusString() {
		byte[] statusCode = new byte[4];
		Convert.int2ByteArray(this.status, statusCode, 0);
		return Convert.bcd2Str(statusCode);
	}
}
