package de.blinkt.openvpn;

import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.lamerman.FileDialog;
import com.lamerman.SelectionMode;

public class FileSelectLayout extends LinearLayout implements OnClickListener {

	private TextView mData;
	private Fragment mFragment;
	private int mTaskId;
	private Button mSelectButton;

	public FileSelectLayout( Context context,AttributeSet attrset) {
		super(context,attrset);
		inflate(getContext(), R.layout.file_select, this);
		
		TypedArray ta = context.obtainStyledAttributes(attrset,R.styleable.FileSelectLayout);
		
		String title = ta.getString(R.styleable.FileSelectLayout_title);
		
		TextView tview = (TextView) findViewById(R.id.file_title);
		tview.setText(title);
		
		mData = (TextView) findViewById(R.id.file_selected_item);
		mSelectButton = (Button) findViewById(R.id.file_select_button);
		mSelectButton.setOnClickListener(this);

	}

	public void setFragment(Fragment fragment, int i)
	{
		mTaskId = i;
		mFragment = fragment;
	}
	
	public void getCertificateFileDialog() {
		Intent startFC = new Intent(getContext(),FileDialog.class);
		startFC.putExtra(FileDialog.START_PATH, "/sdcard");
		startFC.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
		
		mFragment.startActivityForResult(startFC,mTaskId);
	}

	
	public String getData() {
		return mData.getText().toString();
	}

	public void setData(String data) {
		mData.setText(data);
		
	}

	@Override
	public void onClick(View v) {
		if(v == mSelectButton) {
			getCertificateFileDialog();
		}
	}

	
}