package com.paxsz.easylink.device;

/**
 * Device information
 * 
 * @author PAX
 * 
 */
public class DeviceInfo {

	private CommType commtype;

	public enum CommType {
		BLUETOOTH, TCP, USB
	}

	private String blueName;
	private String identifier;
	private String ip;
	private int port;

	public DeviceInfo() {
	}

	/**
	 * Device information
	 * 
	 * @param name
	 *            name of device
	 * @param identifier
	 *            identifier of device
	 */
	public DeviceInfo(CommType commtype, String blueName, String identifier) {
		this.commtype = commtype;
		this.blueName = blueName;
		this.identifier = identifier;
	}

	/**
	 * @param commtype
	 * @param ip
	 * @param port
	 */
	public DeviceInfo(CommType commtype, String ip, int port) {
		this.commtype = commtype;
		this.ip = ip;
		this.port = port;
	}

	/**
	 * @param commtype
	 */
	public DeviceInfo(CommType commtype) {
		this.commtype = commtype;
	}

	public CommType getCommtype() {
		return commtype;
	}

	public void setCommtype(CommType commtype) {
		this.commtype = commtype;
	}

	public String getBlueName() {
		return blueName;
	}

	public void setBlueName(String blueName) {
		this.blueName = blueName;
	}

	public String getIdentifier() {
		return identifier;
	}

	public void setIdentifier(String identifier) {
		this.identifier = identifier;
	}

	public String getIp() {
		return ip;
	}

	public void setIp(String ip) {
		this.ip = ip;
	}

	public int getPort() {
		return port;
	}

	public void setPort(int port) {
		this.port = port;
	}
}