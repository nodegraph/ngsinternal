import java.net.Socket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.openqa.selenium.Keys;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonObject;
import com.google.gson.JsonElement;


public class JComm {
	// Must be called with the following arguments in the specified order.
	// settings_dir
	// app_socket_port
	
	static String settings_dir;
	static int app_socket_port = -1;
	
	static String working_dir;
	static String chrome_ext_dir;
	static String download_extension_dir;
	
	static WebDriverWrap web_driver;
	static Gson gson;
	
	public static void main(String[] args) {
				
		// Process arguments.
		// The app's setting directory where we store the node graphs.
		settings_dir = args[0]; 
		// The app's socket server port. We pass it on to the chrome extension in the browser.
		app_socket_port = Integer.parseInt(args[1]); 
		
		// Get other directories.
		working_dir = System.getProperty("user.dir");
		chrome_ext_dir = working_dir + java.io.File.separator + "chromeextension";
		download_extension_dir = FSWrap.get_download_extension_dir(settings_dir);
		
		// Create our webdriver wrapper.
	    web_driver = new WebDriverWrap(settings_dir, chrome_ext_dir, download_extension_dir, app_socket_port);
		
		// We use gson to convert between json and java objects.
		gson = new Gson();
		
		// Keep scanning the input stream till we die.
		Scanner scanner = new Scanner(System.in);
		while (true) {
			String data = scanner.nextLine();
			JsonObject obj = gson.fromJson(data, JsonObject.class);
			BaseMessage msg = BaseMessage.create_from_obj(obj);

			if (msg.get_msg_type() != MessageEnums.MessageType.kRequestMessage) {
				continue;
			}
			
			RequestMessage req = (RequestMessage) msg;
			try {
				handle_request(req);
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}



	}
	
	static void handle_request(RequestMessage req) {
		MessageEnums.WebDriverRequestType request_type = req.get_request_type();
		switch (request_type) {
		
		case kIsBrowserOpen: {
	        boolean open = web_driver.browser_is_open();
	        ResponseMessage resp = new ResponseMessage(req.get_id(), open, gson.toJsonTree(open));
	        System.out.println(resp.to_string());
	        break;
	    } 
		
		case kOpenBrowser: {
			web_driver.open_browser();
			ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	        System.out.println(resp.to_string());
			break;
		}
		
		case kCloseBrowser: {
	    	boolean open = web_driver.browser_is_open();
	    	if (open) {
	    		web_driver.close_browser();
	    	}
	    	ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	        System.out.println(resp.to_string());
	        break;
	    }
		
		case kReleaseBrowser: {
			web_driver.release_browser();
			ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	        System.out.println(resp.to_string());
			break;
		}
		
	    case kResizeBrowser: {
	    	int width = req.get_args().getAsJsonObject().get("width").getAsInt();
	    	int height = req.get_args().getAsJsonObject().get("height").getAsInt();
	        web_driver.resize_browser(width, height);
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kNavigateTo: {
	    	String url = req.get_args().getAsJsonObject().get("url").getAsString();
	    	web_driver.navigate_to(url);
	    	ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	    	System.out.println(resp.to_string());
	    	break;
	    }
	    
	    case kNavigateBack: {
	        web_driver.navigate_back();
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kNavigateForward: {
	        web_driver.navigate_forward();
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kNavigateRefresh: {
	        web_driver.navigate_refresh();
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kGetCurrentURL: {
	        String url = web_driver.get_current_url();
	        System.err.println("The extracted url: -->" + url + "<--");
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(url));
	        System.out.println(resp.to_string());
	        break;
	    } 
	    
	    case kUpdateCurrentTab: {
	        web_driver.update_current_tab();
	        // Todo: Note this message needs to go to the chrome extension as well.
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kDestroyCurrentTab: {
	        web_driver.destroy_current_tab();
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kPerformMouseAction: {
	    	MessageEnums.MouseActionType mouse_action_type = MessageEnums.MouseActionType.get_enum(req.get_args().getAsJsonObject().get("mouse_action").getAsInt());
	    	String frame_index_path = req.get_args().getAsJsonObject().get("frame_index_path").getAsString();
	    	String xpath = req.get_args().getAsJsonObject().get("xpath").getAsString();
	    	JsonObject pos = req.get_args().getAsJsonObject().get("local_mouse_position").getAsJsonObject();
	    	int local_mouse_position_x = pos.get("x").getAsInt();
	    	int local_mouse_position_y = pos.get("y").getAsInt();
	    	switch (mouse_action_type) {
	            case kSendClick: {
	                web_driver.click_on_element(frame_index_path, xpath, local_mouse_position_x, local_mouse_position_y, false);
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	                System.out.println(resp.to_string());
	                break;
	            } 
	            case kSendCtrlClick: {
	                web_driver.click_on_element(frame_index_path, xpath, local_mouse_position_x, local_mouse_position_y, true);
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	                System.out.println(resp.to_string());
	                break;
	            }
	            case kMouseOver: {
	                web_driver.mouse_over_element(frame_index_path, xpath, local_mouse_position_x, local_mouse_position_y);
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	                System.out.println(resp.to_string());
	                break;
	            }
	            default: {
	                ResponseMessage resp = new ResponseMessage(req.get_id(), false, gson.toJsonTree("unknown mouse action"));
	                System.out.println(resp.to_string());
	                break;
	            }
	        }
	    	break;
	    }
	    
	    case kPerformTextAction: {
	    	MessageEnums.TextActionType text_action_type = MessageEnums.TextActionType.get_enum(req.get_args().getAsJsonObject().get("text_action").getAsInt());
	    	String frame_index_path = req.get_args().getAsJsonObject().get("frame_index_path").getAsString();
	    	String xpath = req.get_args().getAsJsonObject().get("xpath").getAsString();
	        switch (text_action_type) {
	            case kSendText: {
	            	String text = req.get_args().getAsJsonObject().get("text").getAsString();
	            	web_driver.send_text(frame_index_path, xpath, text);
	            	ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	                System.out.println(resp.to_string());
	            	break;
	            }
	            case kSendEnter: {
	                web_driver.send_key(frame_index_path, xpath, Keys.RETURN.toString());
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	                System.out.println(resp.to_string());
	                break;
	            }
	            default: {
	                ResponseMessage resp = new ResponseMessage(req.get_id(), false, gson.toJsonTree("unknown text action"));
	                System.out.println(resp.to_string());
	                break;
	            }
	        }
	        break;
	    } 
	    
	    case kPerformElementAction: {
	    	MessageEnums.ElementActionType element_action_type = MessageEnums.ElementActionType.get_enum(req.get_args().getAsJsonObject().get("element_action").getAsInt());
	    	String frame_index_path = req.get_args().getAsJsonObject().get("frame_index_path").getAsString();
	    	String xpath = req.get_args().getAsJsonObject().get("xpath").getAsString();
	        switch (element_action_type) {
	            case kGetText: {
	                String text = web_driver.get_text(frame_index_path, xpath);
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(text));
	                System.out.println(resp.to_string());
	                break;
	            } 
	            case kChooseOption: {
	            	String option_text = req.get_args().getAsJsonObject().get("option_text").getAsString();
	                web_driver.choose_option(frame_index_path, xpath, option_text);
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
	                System.out.println(resp.to_string());
	                break;
	            }
	            case kScroll: {
	                // Scroll actions need to be performed by the extension
	                // so we pass it through.
	            	MessageEnums.DirectionType dir = MessageEnums.DirectionType.get_enum(req.get_args().getAsJsonObject().get("scroll_direction").getAsInt());
	            	web_driver.scroll(dir);
	            	break;
	            } 
	            default: {
	                ResponseMessage resp = new ResponseMessage(req.get_id(), false, gson.toJsonTree("unknown element action"));
	                System.out.println(resp.to_string());
	                break;
	            }
	        }
	        break;
	    } 
		}	
	}
	
}




