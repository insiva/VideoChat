package com.matteo.vc;

public class Config {

	private static final int LOCAL_PORT = 6734;
	private static String mLocalAddr = null;
	
	public static int getLocalPort(){
		return Config.LOCAL_PORT;
	}
	
	public static String getLocalAddr(){
		return Config.mLocalAddr;
	}
	
	public static void setLocalAddr(String localAddr){
		Config.mLocalAddr=localAddr;
	}
}
