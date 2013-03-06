package com.example.rtprecv;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {
	Button btnStart = null;
	Button btnFinish = null;
	TextView textLog = null;
	
	static
	{
		System.loadLibrary("rtp-jni");		
	}
	
	private native int RtpTest();  
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		
		btnStart=(Button)findViewById(R.id.btnStart);
        btnFinish=(Button)findViewById(R.id.btnFinish);
        textLog = (TextView)findViewById(R.id.textLog);
        
        textLog.setText("Test start!");
        System.out.println("-start recv-");
        
        btnStart.setOnClickListener(new OnClickListener() {
			int recvCount = 0;
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				
				recvCount = RtpTest();
									
				textLog.setText("packet received:" + recvCount);
			}
		});
        
        btnFinish.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				finish();
			}
		});
		
        
    }     

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
