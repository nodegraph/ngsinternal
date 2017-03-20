import java.net.Socket;
import java.awt.Dimension;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.openqa.selenium.Keys;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
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
	
	static WebDriverWrap web_driver;
	static Gson gson;
	
	public static void main(String[] args) {
				
		// Process arguments.
		// The app's setting directory where we store the node graphs.
		settings_dir = args[0]; 
		// The app's socket server port. We pass it on to the chrome extension in the browser.
		app_socket_port = Integer.parseInt(args[1]); 
		
		// Create our webdriver wrapper.
	    web_driver = new WebDriverWrap(settings_dir, app_socket_port);
		
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
				// Print the stack trace to the standard error stream.
				e.printStackTrace();
				// Send a response message containing the error to the standard output stream.
				JsonObject results = new JsonObject();
				results.addProperty("value", e.getMessage());
				ResponseMessage resp = new ResponseMessage(req.get_id(), false, results);
		        System.out.println(resp.to_string());
			}
		}



	}
	
	static JsonObject get_first_element(JsonObject args) {
		JsonObject in = args.getAsJsonObject().get("in").getAsJsonObject();
		JsonArray elements = in.get("elements").getAsJsonArray();
		if (elements.size() == 0) {
			return null;
		}
		return elements.get(0).getAsJsonObject();
	}
	
	static void handle_request(RequestMessage req) {
		
		JsonObject results = new JsonObject();
		results.addProperty("value", true);
		
		MessageEnums.WebDriverRequestType request_type = req.get_request_type();
		switch (request_type) {
		
		case kStartService: {
			boolean started = web_driver.start_service();
			results.addProperty("value", started);
			ResponseMessage resp = new ResponseMessage(req.get_id(), started, results);
	        System.out.println(resp.to_string());
	        break;
		}
		
		case kStopService: {
			web_driver.stop_service();
			ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
	        break;
		}
		
		case kIsBrowserOpen: {
	        boolean open = web_driver.browser_is_open();
			results.addProperty("value", open);
	        ResponseMessage resp = new ResponseMessage(req.get_id(), open, results);
	        System.out.println(resp.to_string());
	        break;
	    } 
		
		case kOpenBrowser: {
			web_driver.open_browser();
			ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
			break;
		}
		
		case kOpenBrowserPost: {
			web_driver.open_browser_post();
			ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
			break;
		}
		
		case kCloseBrowser: {
	    	boolean open = web_driver.browser_is_open();
	    	if (open) {
	    		web_driver.close_browser();
	    	}
	    	ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
	        break;
	    }
		
		case kReleaseBrowser: {
			web_driver.release_browser();
			ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
			break;
		}
		
	    case kSetBrowserSize: {
	    	int width = req.get_args().getAsJsonObject().get("width").getAsInt();
	    	int height = req.get_args().getAsJsonObject().get("height").getAsInt();
	        web_driver.resize_browser(width, height);
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kGetBrowserSize: {
	    	org.openqa.selenium.Dimension d = web_driver.get_browser_size();
	    	JsonObject obj = new JsonObject();
	    	obj.addProperty("width", d.width);
	    	obj.addProperty("height", d.height);
			results.add("value", obj);
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
	    	break;
	    }
	    
	    case kNavigateTo: {
	    	String url = req.get_args().getAsJsonObject().get("url").getAsString();
	    	web_driver.navigate_to(url);
	    	ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	    	System.out.println(resp.to_string());
	    	break;
	    }
	    
	    case kNavigateBack: {
	        web_driver.navigate_back();
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kNavigateForward: {
	        web_driver.navigate_forward();
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kNavigateRefresh: {
	        web_driver.navigate_refresh();
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kGetCurrentURL: {
	        String url = web_driver.get_current_url();
	        System.err.println("The extracted url: -->" + url + "<--");
			results.addProperty("value", url);
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
	        break;
	    } 
	    
	    case kUpdateCurrentTab: {
	        web_driver.update_current_tab();
	        // Todo: Note this message needs to go to the chrome extension as well.
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kDestroyCurrentTab: {
	        web_driver.destroy_current_tab();
	        ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	        System.out.println(resp.to_string());
	        break;
	    }
	    
	    case kPerformMouseAction: {
	    	MessageEnums.MouseActionType mouse_action_type = MessageEnums.MouseActionType.get_enum(req.get_args().getAsJsonObject().get("mouse_action").getAsInt());
	    	JsonObject element = get_first_element(req.get_args());
	    	if (element == null) {
	    		results.addProperty("value", "There were no elements to perform the mouse action on.");
	    		ResponseMessage resp = new ResponseMessage(req.get_id(), false, results);
	    		System.out.println(resp.to_string());
	    		break;
	    	}
	    	
	    	String fe_index_path = element.get("fe_index_path").getAsString();
	    	String xpath = element.get("xpath").getAsString();
	    	JsonObject pos = req.get_args().getAsJsonObject().get("local_mouse_position").getAsJsonObject();
	    	int local_mouse_position_x = pos.get("x").getAsInt();
	    	int local_mouse_position_y = pos.get("y").getAsInt();
	    	switch (mouse_action_type) {
	            case kSendClick: {
	                web_driver.click_on_element(fe_index_path, xpath, local_mouse_position_x, local_mouse_position_y, false);
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	                System.out.println(resp.to_string());
	                break;
	            } 
	            case kSendCtrlClick: {
	                web_driver.click_on_element(fe_index_path, xpath, local_mouse_position_x, local_mouse_position_y, true);
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	                System.out.println(resp.to_string());
	                break;
	            }
	            case kMouseOver: {
	                web_driver.mouse_over_element(fe_index_path, xpath, local_mouse_position_x, local_mouse_position_y);
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	                System.out.println(resp.to_string());
	                break;
	            }
	            default: {
	            	results.addProperty("value", "unknown mouse action");
	                ResponseMessage resp = new ResponseMessage(req.get_id(), false, results);
	                System.out.println(resp.to_string());
	                break;
	            }
	        }
	    	break;
	    }
	    
	    case kPerformTextAction: {
	    	MessageEnums.TextActionType text_action_type = MessageEnums.TextActionType.get_enum(req.get_args().getAsJsonObject().get("text_action").getAsInt());
	    	JsonObject element = get_first_element(req.get_args());
	    	if (element == null) {
	    		results.addProperty("value", "There were no elements to perform the text action on.");
	    		ResponseMessage resp = new ResponseMessage(req.get_id(), false, results);
	    		System.out.println(resp.to_string());
	    		break;
	    	}
	    	
	    	String fe_index_path = element.get("fe_index_path").getAsString();
	    	String xpath = element.get("xpath").getAsString();
	        switch (text_action_type) {
	            case kSendText: {
	            	String text = req.get_args().getAsJsonObject().get("text").getAsString();
	            	web_driver.send_text(fe_index_path, xpath, text);
	            	ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	                System.out.println(resp.to_string());
	            	break;
	            }
	            case kSendEnter: {
	                web_driver.send_key(fe_index_path, xpath, Keys.RETURN.toString());
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	                System.out.println(resp.to_string());
	                break;
	            }
	            default: {
	            	results.addProperty("value", "unknown text action");
	                ResponseMessage resp = new ResponseMessage(req.get_id(), false, results);
	                System.out.println(resp.to_string());
	                break;
	            }
	        }
	        break;
	    } 
	    
	    case kPerformElementAction: {
	    	MessageEnums.ElementActionType element_action_type = MessageEnums.ElementActionType.get_enum(req.get_args().getAsJsonObject().get("element_action").getAsInt());
	    	JsonObject element = get_first_element(req.get_args());
	    	if (element == null) {
	    		results.addProperty("value", "There were no elements to perform the element action on.");
	    		ResponseMessage resp = new ResponseMessage(req.get_id(), false, results);
	    		System.out.println(resp.to_string());
	    		break;
	    	}
	    	
	    	String fe_index_path = element.get("fe_index_path").getAsString();
	    	String xpath = element.get("xpath").getAsString();
	        switch (element_action_type) {
	            case kChooseOption: {
	            	String option_text = req.get_args().getAsJsonObject().get("option_text").getAsString();
	                web_driver.choose_option(fe_index_path, xpath, option_text);
	                ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	                System.out.println(resp.to_string());
	                break;
	            }
//	            case kScroll: {
//	                // Scroll actions need to be performed by the extension
//	                // so we pass it through.
//	            	MessageEnums.DirectionType dir = MessageEnums.DirectionType.get_enum(req.get_args().getAsJsonObject().get("scroll_direction").getAsInt());
//	            	web_driver.switch_to_frame(fe_index_path);
//	            	web_driver.scroll(dir);
//	            	ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
//	                System.out.println(resp.to_string());
//	            	break;
//	            } 
	            default: {
	            	results.addProperty("value", "unknown element action");
	                ResponseMessage resp = new ResponseMessage(req.get_id(), false, results);
	                System.out.println(resp.to_string());
	                break;
	            }
	        }
	        break;
	    }
	    
	    case kPerformImageAction: {
	    	MessageEnums.ImageActionType image_action_type = MessageEnums.ImageActionType.get_enum(req.get_args().getAsJsonObject().get("image_action").getAsInt());
	    	JsonObject element = get_first_element(req.get_args());
	    	if (element == null) {
	    		results.addProperty("value", "There were no elements to perform the image action on.");
	    		ResponseMessage resp = new ResponseMessage(req.get_id(), false, results);
	    		System.out.println(resp.to_string());
	    		break;
	    	}
	    	
	    	String fe_index_path = element.get("fe_index_path").getAsString();
	    	String xpath = element.get("xpath").getAsString();
	    	switch (image_action_type) {
	            case kGetImageURL: {
	            	String url = web_driver.get_image_url(fe_index_path, xpath);
	            	results.addProperty("value", url);
	            	ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	                System.out.println(resp.to_string());
	            	break;
	            }
	            case kDownloadImage: {
	            	String dir = req.get_args().getAsJsonObject().get("download_directory").getAsString();
	            	String image_url = web_driver.get_image_url(fe_index_path, xpath);
	            	String filename = image_url.substring(image_url.lastIndexOf('/') + 1);
	            	String format = filename.substring(filename.lastIndexOf('.') + 1);
	            	boolean downloaded = web_driver.download_image(image_url, format, filename);
	            	results.addProperty("value", downloaded);
	            	ResponseMessage resp = new ResponseMessage(req.get_id(), true, results);
	                System.out.println(resp.to_string());
	            	break;
	            }
	    	}
	    }
	    
		}	
	}
	
}




