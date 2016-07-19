package com.matteo.vc.model;

import org.json.JSONException;
import org.json.JSONObject;

import android.os.Parcel;
import android.os.Parcelable;
import android.text.TextUtils;

public class AccountInfo implements Parcelable {

	static final String ADDR_KEY = "addr";
	static final String PORT_KEY = "port";
	static final String SSRC_KEY = "ssrc";

	public String mAddr;
	public int mPort;
	public int mSsrc;

	public AccountInfo(String addr, int port, int ssrc) {
		this.mSsrc = ssrc;
		this.mAddr = addr;
		this.mPort = port;
	}

	public boolean available() {
		return (!TextUtils.isEmpty(this.mAddr)) && this.mPort != 0;
	}

	
	public JSONObject toJsonObject() {
		JSONObject jsonObject = new JSONObject();
		try {
			jsonObject.put(ADDR_KEY, this.mAddr);
			jsonObject.put(PORT_KEY, this.mPort);
			jsonObject.put(SSRC_KEY, this.mSsrc);
		} catch (JSONException e) {
			e.printStackTrace();
		}
		return jsonObject;
	}
	
	@Override
	public String toString() {
		return this.toJsonObject().toString();
	}

	public static AccountInfo parseJson(String json) {
		JSONObject jsonObject = null;
		try {
			jsonObject = new JSONObject(json);
		} catch (JSONException e) {
			e.printStackTrace();
		}
		if (jsonObject == null) {
			return null;
		}
		AccountInfo accountInfo = new AccountInfo(
				jsonObject.optString(ADDR_KEY), jsonObject.optInt(PORT_KEY),
				jsonObject.optInt(SSRC_KEY));
		if (accountInfo.available()) {
			return accountInfo;
		}
		return null;
	}

	@Override
	public int describeContents() {
		return 0;
	}

	@Override
	public void writeToParcel(Parcel dest, int flags) {
		dest.writeInt(this.mSsrc);
		dest.writeString(this.mAddr);
		dest.writeInt(this.mPort);
	}
	
	public static final Parcelable.Creator<AccountInfo> CREATOR=new Creator<AccountInfo>() {

		@Override
		public AccountInfo createFromParcel(Parcel source) {
			int ssrc=source.readInt();
			String addr=source.readString();
			int port=source.readInt();
			AccountInfo accountInfo=new AccountInfo(addr, port, ssrc);
			return accountInfo;
		}

		@Override
		public AccountInfo[] newArray(int size) {
			return null;
		}
	};
}
