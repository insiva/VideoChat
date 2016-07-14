package com.matteo.vc.util;

import java.util.Random;

import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

public class XUtils {
	static final String Tag = XUtils.class.getName();

	private static Random mRandom = null;

	public static int random(int max) {
		if (mRandom == null) {
			mRandom = new Random(System.currentTimeMillis());
		}
		return mRandom.nextInt(max);
	}

	public static int random() {
		return XUtils.random(random(Integer.MAX_VALUE));
	}

	public static String getLocalAddr(Context context) {
		WifiManager wifiManager = (WifiManager) context
				.getSystemService(Context.WIFI_SERVICE);
		WifiInfo wifiInfo = wifiManager.getConnectionInfo();
		int ipAddress = wifiInfo.getIpAddress();
		Log.d(Tag, "int ip " + ipAddress);
		if (ipAddress == 0)
			return null;
		return ((ipAddress & 0xff) + "." + (ipAddress >> 8 & 0xff) + "."
				+ (ipAddress >> 16 & 0xff) + "." + (ipAddress >> 24 & 0xff));
	}
}
