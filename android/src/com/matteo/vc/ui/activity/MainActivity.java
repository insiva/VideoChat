package com.matteo.vc.ui.activity;

import com.matteo.vc.R;
import com.matteo.vc.service.VcService;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		VcService.startService(this, true);
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
		VcService.stopService(this);
	}
}
