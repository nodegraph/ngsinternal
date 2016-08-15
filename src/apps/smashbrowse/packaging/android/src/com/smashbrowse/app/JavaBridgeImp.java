// Our package.
package com.smashbrowse.app;

import android.util.Log;
import android.content.Context;
import android.os.Bundle;

// Bring in notification related.
import android.app.Notification;
import android.app.NotificationManager;

// Bring in vibrate related.
import android.os.Vibrator;

import android.view.KeyEvent;
import android.view.inputmethod.InputMethodManager;
import android.view.View;
import android.view.ViewGroup;
import java.util.ArrayList;

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
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		// Call our super's implementation.
	    super.onCreate(savedInstanceState);
	    
	    // Initialize our notification related members.
		_notification_manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
		_notification_builder = new Notification.Builder(this);
		//_notification_builder.setSmallIcon(R.drawable.icon);
		
		// Initialize our vibration related members.
		_vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
	}
	
	// Vibrate.
	public void vibrate(long milli_secs) {
		if (_vibrator == null) {
			_vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
		}
		Log.d("vibrate time ", " : " +milli_secs);
		_vibrator.vibrate(milli_secs);
	}
	
	// Notify.
	public void notify(String title, String content) {
		if (_notification_manager == null) {
			_notification_manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
			_notification_builder = new Notification.Builder(this);
			//_notification_builder.setSmallIcon(R.drawable.icon);
		}
		_notification_builder.setContentTitle(title);
		_notification_builder.setContentText(content);
		_notification_manager.notify(1, _notification_builder.build());
	}
	
	// Dismiss android keyboard.
	public void dismiss_keyboard_from_webview() {
		
		// Note the following keys may seem random but they seem to work to
		// cover most of the case of the Qt bug where the android keyboard won't 
		// show up again if opened in a WebView.
		
		// This simulates pressing the hardware back key on android.
		_self.dispatchKeyEvent(
			new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_BACK)); 
		_self.dispatchKeyEvent(
			new KeyEvent(KeyEvent.ACTION_UP, KeyEvent.KEYCODE_BACK));
			
		// Even when the keyboard is closed down, if there is a dropdown in the web page
		// that is left open then we hit the Qt bug where the keyboard will not show up
		// in other QML textfields. This is why we send the enter key event down to close it.
		_self.dispatchKeyEvent(
			new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_ENTER)); 
		_self.dispatchKeyEvent(
			new KeyEvent(KeyEvent.ACTION_UP, KeyEvent.KEYCODE_ENTER));
			
		// Just being paranoid. Sending down escape also to possibly get out of other
		// partially open gui states.
		_self.dispatchKeyEvent(
			new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_ESCAPE)); 
		_self.dispatchKeyEvent(
			new KeyEvent(KeyEvent.ACTION_UP, KeyEvent.KEYCODE_ESCAPE));
			
		// We use the input manager to close all the keyboards in all the views.
		InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
		
		// Get the root view.
		View view = this.findViewById(android.R.id.content);
		if (view != null) {  
			Log.d("VVVVVVVVVVVVVVVVVVVVVVVVVVVV", " view is not null");
		    imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
		} else {
			Log.d("VVVVVVVVVVVVVVVVVVVVVVVVVVVV", " view is null");
		}
		
		ArrayList<View> views = getAllViewsFromRoots(this.findViewById(android.R.id.content));
	    for (View v : views) {
	    	Log.d("VVVVVVVVVVVVVVVVVVVVVVVVVVVV", " hiding keyboard from view");
	    	imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
	    }
	}
	
	public static ArrayList<View> getAllViewsFromRoots(View...roots) {
	    ArrayList<View> result = new ArrayList<View>();
	    for (View root : roots)
	        getAllViews(result, root);
	    return result;
	}

	private static void getAllViews(ArrayList<View> allviews, View parent) {
	    allviews.add(parent);
	    if (parent instanceof ViewGroup) {
	        ViewGroup viewGroup = (ViewGroup)parent;
	        for (int i = 0; i < viewGroup.getChildCount(); i++)
	            getAllViews(allviews, viewGroup.getChildAt(i));
	    }
	}
	

}
