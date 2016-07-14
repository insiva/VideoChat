package com.matteo.vc.service;

import com.matteo.vc.Config;
import com.matteo.vc.Constant;
import com.matteo.vc.jni.VcCall;
import com.matteo.vc.jni.VcCallback;
import com.matteo.vc.jni.VcManager;
import com.matteo.vc.model.AccountInfo;
import com.matteo.vc.util.XUtils;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.text.TextUtils;

public class VcService extends Service {
	static final String START_COMMAND_KEY = "command";
	static final String START_COMMAND_INIT = "init";
	static final String START_COMMAND_DEINIT = "deinit";

	static {
		System.loadLibrary("VideoChat");
	}

	private AccountInfo mMe;
	private VcManager mVcManager;

	@Override
	public void onCreate() {
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {

		String key = intent.getStringExtra(START_COMMAND_KEY);
		if (START_COMMAND_INIT.equals(key)) {
			this.initVcManager();
		} else if (START_COMMAND_DEINIT.equals(key)) {
			this.deinitVcManager();
		}
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		this.deinitVcManager();
	}

	private void initVcManager() {
		this.mMe = new AccountInfo(XUtils.getLocalAddr(this),
				Config.getLocalPort(), XUtils.random());
		if (!TextUtils.isEmpty(this.mMe.mAddr)) {
			this.mVcManager = VcManager.init(this.mMe.mAddr, this.mMe.mPort,
					this.mMe.mSsrc);
			this.mVcManager.setCallback(new MyCallback());
		}
	}

	private void deinitVcManager() {
		if (this.mVcManager != null) {
			this.mVcManager.setCallback(null);
			VcManager.deinit();
			this.mVcManager = null;
		}
	}

	public static void startService(Context context, boolean isInit) {
		Intent intent = new Intent(Constant.Action.VC_SERVICE);
		intent.setPackage(Constant.PACKAGE);
		intent.putExtra(START_COMMAND_KEY, isInit ? START_COMMAND_INIT
				: START_COMMAND_DEINIT);
		context.startService(intent);
	}

	public static void stopService(Context context) {
		Intent intent = new Intent(Constant.Action.VC_SERVICE);
		intent.setPackage(Constant.PACKAGE);
		context.stopService(intent);
	}

	class MyCallback extends VcCallback {
		@Override
		public void onIncoming(VcCall call) {
		}

		@Override
		public void onEstablished(VcCall call) {
		}

		@Override
		public void onConfirmed(VcCall call) {
		}

		@Override
		public void onOutgoFail(VcCall call) {
		}

		@Override
		public void onDisconnect(VcCall call) {
		}
	}
}
