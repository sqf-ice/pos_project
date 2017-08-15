package com.paxsz.easylink.listener;

/**
 * Create listener for FileDownload API 
 * @author PAX
 *
 */
public interface FileDownloadListener {

	/**
	 * Listener for the progress of downloadFile
	 * @param current current file data offset
	 * @param total total length of file data
	 */
	public void onDownloadProgress(int current, int total);
	
	public boolean cancelDownload();
}
