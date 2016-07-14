package com.matteo.vc.model;

public class AccountInfo {

	public String mAddr;
	public int mPort;
	public int mSsrc;

	public AccountInfo(String addr, int port, int ssrc) {
		this.mSsrc = ssrc;
		this.mAddr = addr;
		this.mPort = port;
	}
}
