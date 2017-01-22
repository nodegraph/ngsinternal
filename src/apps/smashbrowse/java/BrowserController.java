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

public class BrowserController {
	// Must be called with the following arguments in the specified order.
	// settings_dir
	// app_socket_port
	public static void main(String[] args) {
		// Process arguments.
		// The app's setting directory where we store the node graphs.
		String settings_dir = args[0]; 
		// The app's socket server port. We pass it on to the chrome extension in the browser.
		int app_socket_port = Integer.parseInt(args[1]); 
		
		// Get other directories.
		String working_dir = System.getProperty("user.dir");
		String chrome_ext_dir = working_dir + java.io.File.separator + "chromeextension";
		String chrome_user_data_dir = FSWrap.create_chrome_user_data_dir(settings_dir);
		String download_extension_dir = FSWrap.get_download_extension_dir(settings_dir);
		
		// Create our webdriver wrapper.
		WebDriverWrap web_driver = new WebDriverWrap(settings_dir, chrome_ext_dir, chrome_user_data_dir, download_extension_dir, app_socket_port);
		
		// We use gson to convert between json and java objects.
		Gson gson = new Gson();
		
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
			MessageEnums.WebDriverRequestType request_type = req.get_request_type();
			
			switch (request_type) {
			
			case kOpenBrowser: {
				web_driver.open_browser();
				ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
                System.out.println(resp.to_string());
				break;
			}
			
			case kShutdown: {
                web_driver.close_browser();
                System.exit(0);
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
            
            case kIsBrowserOpen: {
                boolean open = web_driver.browser_is_open();
                ResponseMessage resp = new ResponseMessage(req.get_id(), open, gson.toJsonTree(open));
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
            
            case kGetCurrentURL: {
                String url = web_driver.get_current_url();
                ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(url));
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
            
            case kUpdateCurrentTab: {
                web_driver.update_current_tab();
                // Todo: Note this message needs to go to the chrome extension as well.
                ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
                System.out.println(resp.to_string());
                break;
            }
            
            case kOpenTab: {
            	// Todo: Note this message needs to be handled by the chrome extension.
                break;
            }
            
            case kDestroyCurrentTab: {
                web_driver.destroy_current_tab();
                ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
                System.out.println(resp.to_string());
                break;
            }
            
            case kPerformMouseAction: {
            	MessageEnums.MouseActionType mouse_action_type = BaseMessage.get_enum(req.get_args().getAsJsonObject(), "mouse_action", MessageEnums.MouseActionType.class);
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
                    }
                }
            	break;
            }
            
            case kPerformTextAction: {
            	MessageEnums.TextActionType text_action_type = BaseMessage.get_enum(req.get_args().getAsJsonObject(), "text_action", MessageEnums.TextActionType.class);
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
                    }
                }
                break;
            } 
            
            case kPerformElementAction: {
            	MessageEnums.ElementActionType element_action_type = BaseMessage.get_enum(req.get_args().getAsJsonObject(), "element_action", MessageEnums.ElementActionType.class);
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
                    	MessageEnums.DirectionType dir = BaseMessage.get_enum(req.get_args().getAsJsonObject(), "scroll_direction", MessageEnums.DirectionType.class);
                    	web_driver.scroll(dir);
                    	break;
                    } 
                    default: {
                        ResponseMessage resp = new ResponseMessage(req.get_id(), false, gson.toJsonTree("unknown element action"));
                        System.out.println(resp.to_string());
                    }
                }
                break;
            } 
            
            case kDownloadFiles: {
                web_driver.download_files();
                ResponseMessage resp = new ResponseMessage(req.get_id(), true, gson.toJsonTree(true));
                System.out.println(resp.to_string());
                break;
            }
			}
		}

		// try {
		// } catch (IOException e) {
		// e.printStackTrace();
		// }

	}
}

//case RequestType.kOpenBrowser: {
//    let on_response = (open: boolean) => {
//        if (!open) {
//            if (!this.webdriverwrap.open_browser()) {
//                send_msg_to_app(new ResponseMessage(msg.id, false, false))
//                return
//            }
//            // Now convert this request to navigate to a default url with a port number.
//            let url = "https://www.google.com/?" + get_ext_server_port()
//            let relay = new RequestMessage(msg.id, RequestType.kNavigateTo, { url: url })
//            this.receive_json(relay.to_string())
//        } else {
//            send_msg_to_app(new ResponseMessage(msg.id, true, true))
//        }
//    }
//    this.webdriverwrap.browser_is_open(on_response)
//} break