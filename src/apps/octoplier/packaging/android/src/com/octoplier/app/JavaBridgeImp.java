// Our package.
package com.octoplier.app;

import android.util.Log;
import android.content.Context;
import android.os.Bundle;

// Bring in notification related.
import android.app.Notification;
import android.app.NotificationManager;

// Bring in vibrate related.
import android.os.Vibrator;

// The JavaBridgeImp acts as our bridge to call into java from c++.
// We're just using static methods for now as it makes it easier to call from c++.
public class JavaBridgeImp extends org.qtproject.qt5.android.bindings.QtActivity {
	
	// This static global is so that the c++ side can find us.
	private static JavaBridgeImp _self;

	// Private instance members.
	private static NotificationManager _notification_manager;
	private static Notification.Builder _notification_builder;
	private static Vibrator _vibrator;
	
	// Constructor.
	public JavaBridgeImp() {
		_self = this;
	}
	
	// Main entry point for c++ access.
	public static JavaBridgeImp get_java_bridge() {
		return _self;
	}
	
	// Override to perform extra initialization.
	//@Override
	//public void onCreate(Bundle savedInstanceState)
	//{
	//	// Call our super's implementation.
	//    super.onCreate(savedInstanceState);
	//    
	//    //// Initialize our notification related members.
	//	//_notification_manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
	//	//_notification_builder = new Notification.Builder(this);
	//	////_notification_builder.setSmallIcon(R.drawable.icon);
	//	
	//	//// Initialize our vibration related members.
	//	//_vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
	//}
	
	// Notify.
	public void notify(String title, String content) {
		//if (_notification_manager == null) {
		//	_notification_manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
		//	_notification_builder = new Notification.Builder(this);
		//	//_notification_builder.setSmallIcon(R.drawable.icon);
		//}
		//_notification_builder.setContentTitle(title);
		//_notification_builder.setContentText(content);
		//_notification_manager.notify(1, _notification_builder.build());
	}
	
	// Vibrate.
	public void vibrate(long milli_secs) {
		//if (_vibrator == null) {
		//	_vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
		//}
		//Log.d("vibrate time ", " : " +milli_secs);
		//_vibrator.vibrate(milli_secs);
	}
}
