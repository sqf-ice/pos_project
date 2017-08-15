package com.paxsz.easylink.listener;

import com.paxsz.easylink.device.DeviceInfo;

/**
 * Create listener for SearchDevice  
 * @author PAX
 *
 */
public interface SearchDeviceListener {

	/**
	 * Listener for searching devices
	 * @param paramDeviceInfo device information
	 */
	public abstract void discoverOneDevice(DeviceInfo paramDeviceInfo);

	/**
	 * Listener if complete searching devices 
	 */
	public abstract void discoverComplete();
	
}