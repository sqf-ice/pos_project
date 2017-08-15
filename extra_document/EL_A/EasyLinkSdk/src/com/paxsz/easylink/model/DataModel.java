package com.paxsz.easylink.model;

/**
 * Data model
 * @author PAX
 *
 */
public class DataModel {

	/**
	 * Data type
	 * TRANSACTION_DATA Transaction data
	 * CONFIGURATION_DATA Configuration data
	 */
	public enum DataType {
		TRANSACTION_DATA(1), CONFIGURATION_DATA(2);
		private int value;
		
		private DataType(int num){
			this.value = num;
		}
		public int toValue(){
			return value;
		}
	}
	
	
	/**
	 * 
	 * Protocol type
	 * MULTI_FRAMEP_ROTOCOL - Multi-frame protocol (STX + VERSION + PACKAGE NO. + FRAME NO. + DATA LEN + DATA + CRC)
	 * SIMPLE_WITH_MULTI_FRAME_PROTOCOL - Multi-frame protocol + Simple protocol (STX + CMD + DATA LEN + DATA + LRC)
	 */
	public enum ProtocolType {
		MULTI_FRAMEP_ROTOCOL(0), SIMPLE_WITH_MULTI_FRAME_PROTOCOL(1);
		private int value;
		
		private ProtocolType(int num){
			this.value = num;
		}
		public int toValue(){
			return value;
		}
	}

}
