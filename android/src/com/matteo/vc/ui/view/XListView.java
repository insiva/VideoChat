package com.matteo.vc.ui.view;


import com.matteo.vc.R;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.drawable.ColorDrawable;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListAdapter;
import android.widget.ListView;

public class XListView extends ListView {

	static int COLOR_GRAY=-1;
	static int COLOR_TRANSPARENT=-1;
	
	private boolean mInScrollView;
	private boolean mHasDivider;
	private ListAdapter mMyAdapter;
	
	public XListView(Context context, AttributeSet attrs) {
		super(context, attrs);
		TypedArray a = context.obtainStyledAttributes(attrs,R.styleable.matteo); 
		this.mInScrollView=a.getBoolean(R.styleable.matteo_inScrollView, false);
		this.mHasDivider=a.getBoolean(R.styleable.matteo_hasDivider, true);
		a.recycle();
		if(COLOR_GRAY<0){
			COLOR_GRAY=context.getResources().getColor(R.color.gray);
			COLOR_TRANSPARENT=context.getResources().getColor(R.color.transparent);
		}
		if(this.mHasDivider){
			this.setDivider(new ColorDrawable(COLOR_GRAY));
			this.setDividerHeight(1);
		}else{
			this.setDividerHeight(0);
		}
		this.setSelector(new ColorDrawable(COLOR_TRANSPARENT));
		this.initHeader();
		this.initFooter();
	}
	
	private void initHeader(){
		//this.addHeaderView(v);
	}
	
	private void initFooter(){
		
	}

	@Override
	public void setAdapter(ListAdapter adapter) {
		super.setAdapter(adapter);
		this.mMyAdapter=adapter;
		if(this.mInScrollView){
			this.measureTotalHeight();
		}
	}
	
	public void measureTotalHeight(){
        if (this.mMyAdapter == null) {
            return;
        }

        int totalHeight = 0;
        int s=this.mMyAdapter.getCount();
        if(s<1){
        	return;
        }
        for (int i = 0; i < s; i++) {
            View listItem = this.mMyAdapter.getView(i, null, this);
            listItem.measure(0, 0);
            totalHeight += listItem.getMeasuredHeight();
        }

        ViewGroup.LayoutParams params = this.getLayoutParams();
        params.height = totalHeight + (this.getDividerHeight() * (s - 1));
        this.setLayoutParams(params);
	}
}
