package com.matteo.vc.ui.view;

import java.io.IOException;
import java.util.List;

import com.matteo.vc.Constant;
import com.matteo.vc.R;
import com.matteo.vc.jni.VC;
import com.matteo.vc.jni.VcCall;
import com.matteo.vc.jni.VcCallAction;
import com.matteo.vc.jni.VcManager;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;
import android.util.Log;
import android.view.Gravity;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.FrameLayout;
import android.widget.ImageView;

public class VcVideoView extends FrameLayout implements OnClickListener,
		SurfaceHolder.Callback, PreviewCallback {
	static final String TAG = "VcVideoView";
	static final int LOCAL_SV_WIDTH = 240;
	static final int LOCAL_SV_HEIGHT = 320;
	static final int PREVIEW_WIDTH = 320;
	static final int PREVIEW_HEIGHT = 240;
	static final int CAMERA_FPS = 10;
	// private VcCall mCall;

	private Context mContext;
	private Camera mCamera;
	private SurfaceHolder mSurfaceHolder;

	private SurfaceView svLocal;
	private GlView gvRemote;
	private ImageView ivHangup;
	private int mPreviewWidth, mPreviewHeight;
	private int mPreviewFps;
	private boolean mHasRecvRemoteCameraParameters;
	private RemoteCameraParametersReceiver mRemoteCameraParametersReceiver;

	private int mFrameCount = 0;
	private long mFirstFrameTime = 0;
	private int mTotalBytes = 0;
	private long mTotalCostTime = 0;

	public VcVideoView(Context context) {
		super(context);
		this.mContext = context;
		// this.mCall=VcManager.get().getCurrentCall();
		this.setBackgroundColor(Color.BLACK);
		this.mHasRecvRemoteCameraParameters = false;
		FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(
				LOCAL_SV_WIDTH, LOCAL_SV_HEIGHT, Gravity.TOP | Gravity.RIGHT);
		this.svLocal = new SurfaceView(context);
		this.svLocal.setZOrderOnTop(true);
		this.addView(this.svLocal, params);

		this.ivHangup = new ImageView(context);
		this.ivHangup.setImageResource(R.drawable.hangup_selector);
		this.ivHangup.setClickable(true);
		this.ivHangup.setOnClickListener(this);
		params = new FrameLayout.LayoutParams(
				FrameLayout.LayoutParams.WRAP_CONTENT,
				FrameLayout.LayoutParams.WRAP_CONTENT, Gravity.BOTTOM
						| Gravity.CENTER_HORIZONTAL);
		params.setMargins(0, 0, 0, 40);
		this.addView(this.ivHangup, params);

		this.mSurfaceHolder = this.svLocal.getHolder(); // 绑定SurfaceView，取得SurfaceHolder对象
		this.mSurfaceHolder.setFixedSize(LOCAL_SV_WIDTH, LOCAL_SV_HEIGHT); // 预览大小設置
		this.mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		this.mSurfaceHolder.addCallback(this);
		this.mCamera = Camera.open();
		this.post(new Runnable() {

			@Override
			public void run() {
				startCamera();
			}
		});

		this.mRemoteCameraParametersReceiver = new RemoteCameraParametersReceiver();
		IntentFilter intentFilter = new IntentFilter(
				Constant.Action.REMOTE_CAMERA_PARAMETERS);
		this.mContext.registerReceiver(this.mRemoteCameraParametersReceiver,
				intentFilter);
	}

	public void hide() {
		this.svLocal.setZOrderOnTop(false);
		this.svLocal.setVisibility(View.GONE);
		this.setVisibility(View.GONE);
	}

	@SuppressWarnings("deprecation")
	private void startCamera() {

		try {
			mCamera.setPreviewDisplay(this.mSurfaceHolder);
		} catch (IOException e) {
			e.printStackTrace();
		}

		Camera.Parameters parameters = mCamera.getParameters();
		// List<Size> supportedPreviewSizes = parameters
		// .getSupportedPreviewSizes();
		this.mPreviewWidth = PREVIEW_WIDTH;
		this.mPreviewHeight = PREVIEW_HEIGHT;
		parameters.setPreviewSize(this.mPreviewWidth, this.mPreviewHeight);
		parameters.setPictureSize(this.mPreviewWidth, this.mPreviewHeight);
		parameters.setPreviewFormat(ImageFormat.YV12);
		// parameters.setPreviewFrameRate(CAMERA_FPS);
		parameters.setPreviewFpsRange(CAMERA_FPS * 1000, CAMERA_FPS * 1000);
		this.mPreviewFps = CAMERA_FPS;

		mCamera.setDisplayOrientation(90);
		mCamera.setParameters(parameters);
		mCamera.setPreviewCallback((PreviewCallback) this);
		mCamera.startPreview();
		VcManager.get().setMyCameraParameters(this.mPreviewWidth,
				this.mPreviewHeight, this.mPreviewFps);
		Log.i(TAG, String.format("My:Width=%d, Height=%d,FPS=%d.",
				this.mPreviewWidth, this.mPreviewHeight, this.mPreviewFps));
		this.initGlView();
	}

	private void initGlView() {
		if (this.mHasRecvRemoteCameraParameters) {
			Log.i(TAG, "Init GlSurfaceView.");
			this.gvRemote = new GlView(this.mContext);
			FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(
					FrameLayout.LayoutParams.MATCH_PARENT,
					FrameLayout.LayoutParams.MATCH_PARENT);
			this.addView(this.gvRemote, 1, params);
		}
	}

	@Override
	protected void onDetachedFromWindow() {
		super.onDetachedFromWindow();
		this.mCamera.setPreviewCallback(null); // ！！这个必须在前，不然退出出错
		this.mCamera.stopPreview();
		this.mCamera.release();
		this.mCamera = null;
		this.mContext.unregisterReceiver(this.mRemoteCameraParametersReceiver);
	}

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		long t1 = System.currentTimeMillis();
		if (this.mFrameCount == 0) {
			this.mFirstFrameTime = System.currentTimeMillis();
		} else {
			if (this.mFrameCount % CAMERA_FPS == 0) {
				double ts = (System.currentTimeMillis() - this.mFirstFrameTime) / 1000d;
				double fps = (double) mFrameCount / ts;
				double speed = this.mTotalBytes / (ts * 1024d);
				Log.d(TAG,
						String.format(
								"Local Count:%d, AvgCost:%d, FPS:%f2, Total Bytes:%d, Speed:%f kBps",
								this.mFrameCount, this.mTotalCostTime
										/ this.mFrameCount, fps,
								this.mTotalBytes, speed));
			}
		}
		this.mFrameCount++;
		this.mTotalBytes += VC.pushYv12Frame(data);
		this.mTotalCostTime += (System.currentTimeMillis() - t1);
		// Log.d(TAG,String.format("Duration:%d", System.currentTimeMillis() -
		// t1));
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {

	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {

	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {

	}

	@Override
	public void onClick(View v) {
		VcManager.get().handleCall(VcManager.get().getCurrentCall(),
				VcCallAction.CALL_ACTION_HANGUP);
	}

	class RemoteCameraParametersReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			VcVideoView.this.mHasRecvRemoteCameraParameters = true;
			Log.i(TAG, "Received remote camera parameters.");
			VcVideoView.this.initGlView();
		}
	}

}
