package com.matteo.vc.ui.activity;

import com.matteo.vc.Constant;
import com.matteo.vc.R;
import com.matteo.vc.jni.VcCallAction;
import com.matteo.vc.jni.VcManager;
import com.matteo.vc.manager.FriendManager;
import com.matteo.vc.model.AccountInfo;
import com.matteo.vc.ui.view.VcVideoView;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

public class CallActivity extends Activity implements OnClickListener {

	static final String SSRC_KEY = "ssrc";
	static final String IS_OUTGO_CALL_KEY = "is_outgo_call_key";
	static final int WHAT_CLOSE_ACTIVITY = 0x001;

	private AccountInfo mFriend;

	private Handler mHandler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case WHAT_CLOSE_ACTIVITY:
				CallActivity.this.finish();
				break;

			default:
				break;
			}
		}
	};

	private RelativeLayout rlCallControl;
	private TextView tvFriend;
	private TextView tvState;
	private LinearLayout llIncoming, llOutgoing;
	private ImageView ivReject, ivAnswer, ivHangup;
	private CallStateReceiver mCallStateReceiver;
	private FrameLayout flCall;
	private VcVideoView mVideoView;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.activity_call);
		// this.mVcManager=VcManager.get();
		this.init();
		int ssrc = this.getIntent().getIntExtra(SSRC_KEY, 0);
		boolean isOutgoCall = this.getIntent().getBooleanExtra(
				IS_OUTGO_CALL_KEY, false);
		this.mFriend = FriendManager.get().findBySsrc(ssrc);
		if (this.mFriend == null) {
			Toast.makeText(this, R.string.outgo_failed, Toast.LENGTH_LONG)
					.show();
			this.closeActivity();
		} else {
			if (isOutgoCall) {
				this.onCallOutgoing();
			} else {
				this.onCallIncoming();
			}
		}
	}

	private void init() {
		//this.gvRemote = (GlView) this.findViewById(R.id.gvRemote);
		//this.svLocal = (SurfaceView) this.findViewById(R.id.svLocal);
		this.flCall=(FrameLayout)this.findViewById(R.id.flCall);
		this.rlCallControl = (RelativeLayout) this
				.findViewById(R.id.rlCallControl);
		this.tvFriend = (TextView) this.findViewById(R.id.tvFriend);
		this.tvState = (TextView) this.findViewById(R.id.tvState);
		this.llIncoming = (LinearLayout) this.findViewById(R.id.llIncoming);
		this.llOutgoing = (LinearLayout) this.findViewById(R.id.llOutgoing);
		this.ivAnswer = (ImageView) this.findViewById(R.id.ivAnswer);
		this.ivReject = (ImageView) this.findViewById(R.id.ivReject);
		this.ivHangup = (ImageView) this.findViewById(R.id.ivHangup);
		this.ivAnswer.setOnClickListener(this);
		this.ivReject.setOnClickListener(this);
		this.ivHangup.setOnClickListener(this);

		this.mCallStateReceiver = new CallStateReceiver();
		IntentFilter intentFilter = new IntentFilter(Constant.Action.CALL_STATE);
		this.registerReceiver(this.mCallStateReceiver, intentFilter);
	}

	private void onCallOutgoing() {
		this.tvFriend.setText(String.format("%s:%d", this.mFriend.mAddr,
				this.mFriend.mPort));
		this.tvState.setText(R.string.outgoing);
		int err = VcManager.get().makeCall(this.mFriend.mSsrc);
		if (err != 0) {
			Toast.makeText(this, R.string.outgo_failed, Toast.LENGTH_LONG)
					.show();
			this.closeActivity();
		}
		this.llOutgoing.setVisibility(View.VISIBLE);
		this.llIncoming.setVisibility(View.GONE);
	}

	private void onCallIncoming() {
		this.tvFriend.setText(String.format("%s:%d", this.mFriend.mAddr,
				this.mFriend.mPort));
		this.tvState.setText(R.string.incoming);
		this.llIncoming.setVisibility(View.VISIBLE);
		this.llOutgoing.setVisibility(View.GONE);
	}

	private void onCallDisconnect() {
		if(this.mVideoView!=null){
			//this.mVideoView.setVisibility(View.GONE);
			this.mVideoView.hide();
		}
		this.rlCallControl.setVisibility(View.VISIBLE);
		this.llIncoming.setVisibility(View.GONE);
		this.llOutgoing.setVisibility(View.GONE);
		this.tvState.setText(R.string.call_disconnect);
		this.closeActivity();
	}

	private void onCallConfirmed() {
		this.rlCallControl.setVisibility(View.GONE);
		this.mVideoView=new VcVideoView(this);
		this.flCall.addView(this.mVideoView, FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		this.unregisterReceiver(this.mCallStateReceiver);
	}

	private void closeActivity() {
		this.mHandler.sendEmptyMessageDelayed(WHAT_CLOSE_ACTIVITY, 3000);
	}

	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.ivAnswer:
			VcManager.get().handleCall(VcManager.get().getCurrentCall(),
					VcCallAction.CALL_ACTION_ANSWER);
			break;
		case R.id.ivReject:
			VcManager.get().handleCall(VcManager.get().getCurrentCall(),
					VcCallAction.CALL_ACTION_REJECT);
			break;
		case R.id.ivHangup:
			VcManager.get().handleCall(VcManager.get().getCurrentCall(),
					VcCallAction.CALL_ACTION_HANGUP);
			break;

		default:
			break;
		}
	}
	
	public static void startActivity(Context context, int ssrc,
			boolean isOutgoCall,boolean newTask) {
		Intent intent = new Intent(context, CallActivity.class);
		intent.putExtra(SSRC_KEY, ssrc);
		intent.putExtra(IS_OUTGO_CALL_KEY, isOutgoCall);
		if(newTask){
			intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		}
		context.startActivity(intent);
	}

	public static void startActivity(Context context, int ssrc,
			boolean isOutgoCall) {
		CallActivity.startActivity(context, ssrc, isOutgoCall,false);
	}

	class CallStateReceiver extends BroadcastReceiver {

		CallStateReceiver() {

		}

		@Override
		public void onReceive(Context context, Intent intent) {
			int action = intent.getIntExtra(Constant.CallState.KEY, 0);
			switch (action) {
			case Constant.CallState.INCOMING:

				break;
			case Constant.CallState.ESTABLISHED:

				break;
			case Constant.CallState.CONFIMRED:
				CallActivity.this.onCallConfirmed();
				break;
			case Constant.CallState.DISCONNECTED:
				CallActivity.this.onCallDisconnect();
				break;

			default:
				break;
			}
		}

	}
}
