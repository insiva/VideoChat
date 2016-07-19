package com.matteo.vc.ui.view;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;

import com.matteo.vc.R;
import com.matteo.vc.jni.VC;
import com.matteo.vc.jni.VcCallAction;
import com.matteo.vc.jni.VcManager;

import android.content.Context;
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
	static final String TAG="VcVideoView";
	static final int LOCAL_SV_WIDTH = 240;
	static final int LOCAL_SV_HEIGHT = 450;
	static final int CAMERA_FPS = 16;

	private Camera mCamera;
	private SurfaceHolder mSurfaceHolder;

	private SurfaceView svLocal;
	private GlView gvRemote;
	private ImageView ivHangup;
	private int mPreviewWidth, mPreviewHeight;
	private int mPreviewFps;

	public VcVideoView(Context context) {
		super(context);

		this.gvRemote = new GlView(context);
		this.addView(this.gvRemote, FrameLayout.LayoutParams.MATCH_PARENT,
				FrameLayout.LayoutParams.MATCH_PARENT);
		FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(
				LOCAL_SV_WIDTH, LOCAL_SV_HEIGHT, Gravity.TOP | Gravity.RIGHT);
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
		this.startCamera();
		VcManager.get().setMyCameraParameters(this.mPreviewWidth,
				this.mPreviewHeight, this.mPreviewFps);
		Log.i(TAG, String.format("Width=%d, Height=%d,FPS=%d.", this.mPreviewWidth,this.mPreviewHeight,this.mPreviewFps));
	}

	private void startCamera() {
		this.mCamera = Camera.open();
		try {
			mCamera.setPreviewDisplay(this.mSurfaceHolder);
		} catch (IOException e) {
			e.printStackTrace();
		}
		Camera.Parameters parameters = mCamera.getParameters();
		// this.mPreviewFps=parameters.setPreviewFpsRange(min, max);
		List<Size> supportedPreviewSizes = parameters
				.getSupportedPreviewSizes();
		int selectIndex = supportedPreviewSizes.size() / 2;
		this.mPreviewWidth = supportedPreviewSizes.get(selectIndex).width;
		this.mPreviewHeight = supportedPreviewSizes.get(selectIndex).height;
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
	}

	@Override
	protected void onDetachedFromWindow() {
		super.onDetachedFromWindow();
		this.mCamera.setPreviewCallback(null); // ！！这个必须在前，不然退出出错
		this.mCamera.stopPreview();
		this.mCamera.release();
		this.mCamera = null;
	}

	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		VC.pushYv12Frame(data);
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

}
