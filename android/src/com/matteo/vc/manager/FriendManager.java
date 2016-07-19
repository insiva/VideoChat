package com.matteo.vc.manager;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.ArrayList;

import org.json.JSONException;
import org.json.JSONObject;

import com.matteo.vc.Constant;
import com.matteo.vc.model.AccountInfo;
import com.matteo.vc.util.XUtils;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

public class FriendManager {
	static final String TAG="FriendManager";
	static final String BROADCAST_KEY = "broadcast";

	private static FriendManager mInstance = null;

	private DatagramSocket mServer;
	
	private AccountInfo mMe;
	private UdpServer mUdpServer;
	private FriendListener mFriendListener;
	private ArrayList<AccountInfo> mFriends;

	private FriendManager(Context context) {
		String mac = XUtils.getLocalMacAddr(context);
		int ssrc = mac == null ? 0 : mac.hashCode();
		this.mMe = new AccountInfo(XUtils.getLocalAddr(context),
				Constant.RTP_PORT, ssrc);
		this.mFriends=new ArrayList<AccountInfo>();
		if (this.mMe.available()) {
			this.mUdpServer = new UdpServer();
			this.mUdpServer.start();
		}
	}

	public AccountInfo me() {
		return this.mMe;
	}
	
	public ArrayList<AccountInfo> getFriends(){
		return this.mFriends;
	}

	public void setFriendListener(FriendListener listener) {
		this.mFriendListener = listener;
	}
	
	public AccountInfo findBySsrc(int ssrc){
		for (AccountInfo ai : mFriends) {
			if(ai.mSsrc==ssrc){
				return ai;
			}
		}
		return null;
	}
	
	public void broadcastSsrc(){
		if(this.mUdpServer.mBroadcastThread.mBroadcastHandler!=null){
			this.mUdpServer.mBroadcastThread.mBroadcastHandler.sendEmptyMessage(0);
		}
	}

	public static FriendManager get() {
		return mInstance;
	}

	public static void init(Context context) {
		mInstance = new FriendManager(context);
	}

	public static void deinit() {
		mInstance.mUdpServer.mIsRun = false;
	}
	
	class BroadcastThread extends Thread{
		private boolean mIsRun;
		private Handler mBroadcastHandler;
		
		BroadcastThread(){
			mIsRun=true;
		}
		
		private void broadcast() {
			FriendManager.this.mFriends.clear();
			if(mFriendListener!=null){
				mFriendListener.onBroadcast();
			}
			if (!FriendManager.this.mMe.available()) {
				return;
			}
			InetAddress addr = null;
			try {
				addr = InetAddress.getByName(Constant.BROADCAST_ADDRESS);
			} catch (UnknownHostException e1) {
				e1.printStackTrace();
			}
			if (addr == null) {
				return;
			}
			JSONObject jsonObject = FriendManager.this.mMe.toJsonObject();
			try {
				jsonObject.put(BROADCAST_KEY, true);
			} catch (JSONException e1) {
				e1.printStackTrace();
			}
			byte[] sendBuf;
			sendBuf = jsonObject.toString().getBytes();
			DatagramPacket sendPacket = new DatagramPacket(sendBuf,
					sendBuf.length, addr, Constant.UDP_BROADCAST_PORT);
			try {
				mServer.send(sendPacket);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		@Override
		public void run() {
			Looper.prepare();
			this.broadcast();
			this.mBroadcastHandler=new Handler(){
				@Override
				public void handleMessage(Message msg) {
					super.handleMessage(msg);
					broadcast();
				}
			};
			Looper.loop();
		}
	}

	class UdpServer extends Thread {

		private boolean mIsRun;
		private BroadcastThread mBroadcastThread;

		UdpServer() {
			this.mIsRun = true;
			this.mBroadcastThread=new BroadcastThread();
		}

		@Override
		public void run() {
			try {
				mServer = new DatagramSocket(Constant.UDP_BROADCAST_PORT);
			} catch (SocketException e) {
				e.printStackTrace();
			}
			this.mBroadcastThread.start();
			byte[] recvBuf = new byte[256];
			DatagramPacket recvPacket = new DatagramPacket(recvBuf,
					recvBuf.length);
			while (this.mIsRun) {
				try {
					mServer.receive(recvPacket);
				} catch (IOException e) {
					e.printStackTrace();
				}
				this.onDataRecved(recvPacket);
			}
			mServer.close();
			this.mBroadcastThread.mIsRun=false;
		}

		private boolean onDataRecved(DatagramPacket recvPacket) {
			String recvStr = new String(recvPacket.getData(), 0,
					recvPacket.getLength());
			Log.i(TAG, recvStr);
			AccountInfo friend = AccountInfo.parseJson(recvStr);
			if (friend == null ||friend.mSsrc==FriendManager.this.mMe.mSsrc) {
				return false;
			}
			AccountInfo theFriend=null;
			for (AccountInfo f : FriendManager.this.mFriends) {
				if(f.mSsrc==friend.mSsrc){
					theFriend=f;
				}
			}
			if(theFriend==null){
				FriendManager.this.mFriends.add(friend);
			}
			if (FriendManager.this.mFriendListener != null) {
				FriendManager.this.mFriendListener.onNewFriend(friend);
			}
			try {
				JSONObject jsonObject = new JSONObject(recvStr);
				if (jsonObject.optBoolean(BROADCAST_KEY, false)) {
					this.answer(recvPacket.getAddress(), recvPacket.getPort());
				}
			} catch (JSONException e) {
				e.printStackTrace();
			}
			return true;
		}

		private void answer(InetAddress addr, int port) {
			if (!FriendManager.this.mMe.available()) {
				return;
			}
			JSONObject jsonObject = FriendManager.this.mMe.toJsonObject();
			try {
				jsonObject.put(BROADCAST_KEY, false);
			} catch (JSONException e1) {
				e1.printStackTrace();
			}
			byte[] sendBuf;
			sendBuf = jsonObject.toString().getBytes();
			DatagramPacket sendPacket = new DatagramPacket(sendBuf,
					sendBuf.length, addr, port);
			try {
				mServer.send(sendPacket);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	public static interface FriendListener {
		public void onNewFriend(AccountInfo friend);
		public void onBroadcast();
	}
}
