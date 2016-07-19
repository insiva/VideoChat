package com.matteo.vc.ui.activity;

import java.util.ArrayList;

import com.matteo.vc.Constant;
import com.matteo.vc.R;
import com.matteo.vc.manager.FriendManager;
import com.matteo.vc.model.AccountInfo;
import com.matteo.vc.service.VcService;
import com.matteo.vc.ui.view.XListView;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.TextView;

public class MainActivity extends Activity {

	private XListView lvFriends;
	private ArrayList<AccountInfo> mFriends;
	private FriendReceiver mFriendReceiver;
	private FriendAdapter mFriendAdapter;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		VcService.startService(this, true);
		this.mFriendReceiver = new FriendReceiver();
		this.registerReceiver(this.mFriendReceiver, new IntentFilter(
				Constant.Action.NEW_FRIEND));
		this.lvFriends = (XListView) this.findViewById(R.id.lvFriends);
		this.mFriends = new ArrayList<AccountInfo>();
		this.mFriendAdapter = new FriendAdapter();
		this.lvFriends.setAdapter(this.mFriendAdapter);
	}
	
	@Override
	protected void onResume() {
		super.onResume();
		this.refreshFriends();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		this.unregisterReceiver(mFriendReceiver);
		VcService.stopService(this);
	}

	private void refreshFriends() {
		this.mFriends.clear();
		if(FriendManager.get()!=null){
			this.mFriends.addAll(FriendManager.get().getFriends());
		}
		this.mFriendAdapter.notifyDataSetChanged();
	}

	class FriendAdapter extends BaseAdapter {

		@Override
		public int getCount() {
			return mFriends.size();
		}

		@Override
		public Object getItem(int position) {
			return mFriends.get(position);
		}

		@Override
		public long getItemId(int position) {
			return position;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			FriendItem item = null;
			if (convertView == null) {
				item = new FriendItem(MainActivity.this);
				convertView = item;
			} else {
				item = (FriendItem) convertView;
			}
			item.setFriend(mFriends.get(position));
			return convertView;
		}

	}

	class FriendItem extends LinearLayout implements OnClickListener{

		private AccountInfo mFriend;
		private TextView tvFriend;

		public FriendItem(Context context) {
			super(context);
			View.inflate(context, R.layout.layout_friend_item, this);
			tvFriend=(TextView)this.findViewById(R.id.tvFriend);
			this.setBackgroundResource(R.drawable.default_selector);
			this.setClickable(true);
			this.setOnClickListener(this);
		}

		public void setFriend(AccountInfo friend) {
			this.mFriend = friend;
			this.tvFriend.setText(String.format("%s:%d", this.mFriend.mAddr,this.mFriend.mPort));
		}

		@Override
		public void onClick(View v) {
			CallActivity.startActivity(MainActivity.this,this.mFriend.mSsrc, true);
		}

	}

	class FriendReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			refreshFriends();
		}
	}
}
