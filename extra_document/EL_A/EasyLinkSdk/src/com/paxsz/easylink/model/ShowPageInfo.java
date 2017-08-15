package com.paxsz.easylink.model;

/**
 * Message information, used in showMsgBox API
 * 
 * @author PAX
 * 
 */
public class ShowPageInfo {

	private String widgetName;

	private String text;

	/**
	 * Message information
	 * 
	 * @param widgetName
	 *            name of widget
	 * @param text
	 *            text of widget
	 */
	public ShowPageInfo(String widgetName, String text) {
		this.widgetName = widgetName;
		this.text = text;
	}

	public String getWidgetName() {
		return widgetName;
	}

	public void setWidgetName(String widgetName) {
		this.widgetName = widgetName;
	}

	public String getText() {
		return text;
	}

	public void setText(String text) {
		this.text = text;
	}
}
