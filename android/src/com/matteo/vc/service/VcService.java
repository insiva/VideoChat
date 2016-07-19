package com.matteo.vc.service;

import com.matteo.vc.Constant;
import com.matteo.vc.jni.VcCall;
import com.matteo.vc.jni.VcCallAction;
import com.matteo.vc.jni.VcCallback;
import com.matteo.vc.jni.VcManager;
import com.matteo.vc.manager.FriendManager;
import com.matteo.vc.model.AccountInfo;
import com.matteo.vc.service.IVc.Stub;
import com.matteo.vc.ui.activity.CallActivity;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;

public class VcService extends Service implements FriendManager.FriendListener {
	private VcCall mCurrentCall;

	static final String START_COMMAND_KEY = "command";
	static final String START_COMMAND_INIT = "init";
	static final String START_COMMAND_DEINIT = "deinit";

	static {
		System.loadLibrary("VideoChat");
	}

	private VcManager mVcManager;

	@Override
	public void onCreate() {
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {

		String key = intent == null ? null : intent
				.getStringExtra(START_COMMAND_KEY);
		if (START_COMMAND_INIT.equals(key)) {
			this.initManager();
		} else if (START_COMMAND_DEINIT.equals(key)) {
			this.deinitManager();
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
		this.deinitManager();
	}

	private void initManager() {
		FriendManager.init(this);
		FriendManager.get().setFriendListener(this);
		if (FriendManager.get().me().available()) {
			this.mVcManager = VcManager.init(FriendManager.get().me().mAddr,
					FriendManager.get().me().mPort,
					FriendManager.get().me().mSsrc);
			this.mVcManager.setCallback(new MyCallback());
		}
	}

	private void deinitManager() {
		FriendManager.deinit();
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
			mCurrentCall = call;
			CallActivity.startActivity(VcService.this, call.getFriend().getSsrc(), false);
			Intent intent=new Intent(Constant.Action.CALL_STATE);
			intent.putExtra(Constant.CallState.KEY, Constant.CallState.INCOMING);
			VcService.this.sendBroadcast(intent);
		}

		@Override
		public void onEstablished(VcCall call) {
			mCurrentCall = call;
			Intent intent=new Intent(Constant.Action.CALL_STATE);
			intent.putExtra(Constant.CallState.KEY, Constant.CallState.INCOMING);
			VcService.this.sendBroadcast(intent);
		}

		@Override
		public void onConfirmed(VcCall call) {
			Intent intent=new Intent(Constant.Action.CALL_STATE);
			intent.putExtra(Constant.CallState.KEY, Constant.CallState.CONFIMRED);
			VcService.this.sendBroadcast(intent);
		}

		@Override
		public void onOutgoFail(VcCall call) {
			Intent intent=new Intent(Constant.Action.CALL_STATE);
			intent.putExtra(Constant.CallState.KEY, Constant.CallState.OUTGO_FAILED);
			VcService.this.sendBroadcast(intent);
		}

		@Override
		public void onDisconnect(VcCall call) {
			Intent intent=new Intent(Constant.Action.CALL_STATE);
			intent.putExtra(Constant.CallState.KEY, Constant.CallState.DISCONNECTED);
			VcService.this.sendBroadcast(intent);
		}
	}

	@Override
	public void onNewFriend(AccountInfo friend) {
		if (this.mVcManager != null) {
			this.mVcManager.addFriend(friend.mSsrc, friend.mAddr, friend.mPort);
		}
		Intent intent = new Intent(Constant.Action.NEW_FRIEND);
		this.sendBroadcast(intent);
	}

	public class VcBinder extends Stub {

		@Override
		public int makeCall(int ssrc) throws RemoteException {
			return mVcManager.makeCall(ssrc);
		}

		@Override
		public int handleCall(int action) throws RemoteException {
			int err = 0;
			switch (action) {
			case Constant.CallAction.ANSWER:
				mVcManager.handleCall(mCurrentCall,
						VcCallAction.CALL_ACTION_ANSWER);
				break;
			case Constant.CallAction.REJECT:
				mVcManager.handleCall(mCurrentCall,
						VcCallAction.CALL_ACTION_REJECT);
				break;
			case Constant.CallAction.HANGUP:
				mVcManager.handleCall(mCurrentCall,
						VcCallAction.CALL_ACTION_HANGUP);
				break;

			default:
				break;
			}
			return err;
		}

	}

	@Override
	public void onBroadcast() {
		Intent intent = new Intent(Constant.Action.NEW_FRIEND);
		this.sendBroadcast(intent);
	}
}
