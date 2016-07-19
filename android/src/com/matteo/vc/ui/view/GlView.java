package com.matteo.vc.ui.view;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import com.matteo.vc.jni.VcManager;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;


public class GlView  extends GLSurfaceView {
	private static String TAG = "GlView";
	private MyRender mRender;
	
	public GlView(Context context){
		super(context);
		this.initGv();
	}

	public GlView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.initGv();
	}

	private void initGv() {
		/*
		 * Setup the context factory for 2.0 rendering. See ContextFactory class
		 * definition below
		 */
		 setEGLContextFactory(new ContextFactory());

		/*
		 * We need to choose an EGLConfig that matches the format of our surface
		 * exactly. This is going to be done in our custom config chooser. See
		 * ConfigChooser class definition below.
		 */
		//setEGLConfigChooser(new ConfigChooser(5, 6, 5, 0, 0, 0));
		this.mRender = new MyRender();
		this.setRenderer(this.mRender);
	}

	class MyRender implements GLSurfaceView.Renderer {

		@Override
		public void onDrawFrame(GL10 gl) {
			VcManager.get().render();
		}

		@Override
		public void onSurfaceChanged(GL10 gl, int width, int height) {
			Log.i(TAG, String.format("Thread ID:%d\n",Thread.currentThread().getId()));
			VcManager.get().initGl(width, height);
		}

		@Override
		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		}

	}

	private static void checkEglError(String prompt, EGL10 egl) {
		int error;
		while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
			Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
		}
	}

	private static class ContextFactory implements
			GLSurfaceView.EGLContextFactory {
		private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

		public EGLContext createContext(EGL10 egl, EGLDisplay display,
				EGLConfig eglConfig) {
			Log.w(TAG, "creating OpenGL ES 2.0 context");
			checkEglError("Before eglCreateContext", egl);
			int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
			EGLContext context = egl.eglCreateContext(display, eglConfig,
					EGL10.EGL_NO_CONTEXT, attrib_list);
			checkEglError("After eglCreateContext", egl);
			return context;
		}

		public void destroyContext(EGL10 egl, EGLDisplay display,
				EGLContext context) {
			egl.eglDestroyContext(display, context);
		}
	}

}
