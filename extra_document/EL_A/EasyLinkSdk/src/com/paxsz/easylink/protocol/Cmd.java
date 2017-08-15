package com.paxsz.easylink.protocol;

import java.util.HashMap;
import java.util.Map;

public class Cmd {

	private byte TYPE;
	private byte CODE;
	
	private static Map<Command, Cmd> cmdMap;
	
	public Cmd(byte TYPE, byte CODE) {
		this.TYPE = TYPE;
		this.CODE = CODE;
	}
	
	public static void initCmdTable() {
		cmdMap = new HashMap<Command,Cmd>();
		cmdMap.put(Command.CMD_COMM_CONNECT, new Cmd((byte)0x80, (byte)0x00));
		cmdMap.put(Command.CMD_COMM_DISCONNECT, new Cmd((byte)0x80, (byte)0x01));
		cmdMap.put(Command.CMD_UI_SHOW_PAG, new Cmd((byte)0x80, (byte)0x10));
		cmdMap.put(Command.CMD_GET_PIN_BLOCK, new Cmd((byte)0x80, (byte)0x20));
		cmdMap.put(Command.CMD_ENCRYPT_DATA, new Cmd((byte)0x80, (byte)0x21));
		cmdMap.put(Command.CMD_TRANS_START, new Cmd((byte)0x80, (byte)0x30));
		cmdMap.put(Command.CMD_TRANS_COMPLETE, new Cmd((byte)0x80, (byte)0x31));
		cmdMap.put(Command.CMD_PARAM_GET_DATA, new Cmd((byte)0x80, (byte)0x41));
		cmdMap.put(Command.CMD_PARAM_SET_DATA, new Cmd((byte)0x80, (byte)0x40));
		cmdMap.put(Command.CMD_TMS_DOWNLOAD_FILE, new Cmd((byte)0x80, (byte)0x50));
		cmdMap.put(Command.CMD_SWITCH_COMM_MODE, new Cmd((byte)0x80, (byte)0x60));

	}
	
	public enum Command {
		CMD_COMM_CONNECT,
		CMD_COMM_DISCONNECT,
		CMD_UI_SHOW_PAG,
		CMD_GET_PIN_BLOCK,
		CMD_ENCRYPT_DATA,
		CMD_TRANS_START,
		CMD_TRANS_COMPLETE,
		CMD_PARAM_GET_DATA,
		CMD_PARAM_SET_DATA,
		CMD_TMS_DOWNLOAD_FILE,
		CMD_SWITCH_COMM_MODE
	}
	
	public byte getTYPE() {
		return TYPE;
	}

	public void setTYPE(byte tYPE) {
		TYPE = tYPE;
	}

	public byte getCODE() {
		return CODE;
	}

	public void setCODE(byte cODE) {
		CODE = cODE;
	}

	public static Map<Command, Cmd> getCmdTable() {
		return cmdMap;
	}

	public static void setCmdTable(Map<Command, Cmd> cmdMap) {
		Cmd.cmdMap = cmdMap;
	}
}
