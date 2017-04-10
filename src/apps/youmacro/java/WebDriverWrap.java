import org.openqa.selenium.WebDriver;
import org.openqa.selenium.By;
import org.openqa.selenium.Keys;
import org.openqa.selenium.Point;
import org.openqa.selenium.Proxy;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.Dimension;
import org.openqa.selenium.interactions.Actions;
import org.openqa.selenium.remote.CapabilityType;
import org.openqa.selenium.remote.DesiredCapabilities;
import org.openqa.selenium.Capabilities;

import org.openqa.selenium.firefox.FirefoxDriver;
import org.openqa.selenium.chrome.ChromeDriver;
import org.openqa.selenium.chrome.ChromeOptions;
import org.openqa.selenium.JavascriptExecutor;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.Stack;
import java.util.Set;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.imageio.ImageIO;

import org.openqa.selenium.remote.RemoteWebDriver;
import org.openqa.selenium.support.ui.ExpectedConditions;
import org.openqa.selenium.support.ui.WebDriverWait;
import org.openqa.selenium.chrome.ChromeDriverService;


public class WebDriverWrap {
	private ChromeDriverService _service;
	private WebDriver _web_driver;
	private String _settings_dir; // youmacro settings dir
	private String _chrome_ext_dir; // chrome extension dir
	private int _app_socket_port = -1;
	private Stack<String> _window_handles;
	
	private String _app_page;
	private String _blank_page;
	private String _socket_connect_page;
	
	private String _driver_location;
	
	// Variables for hack to close the "disable developer mode extensions" popup.
	private boolean _open_post_window; // Indicates whether to open another window after the first browser window is opened.
	private boolean _destroy_first_window; // Indicates whether to destory the first window.
	
	private long _open_count;
	
	public class WebElementWithPos {
		WebElement element;
		int local_x;
		int local_y;
		
		public WebElementWithPos(WebElement element, int local_x, int local_y) {
			this.element = element;
			this.local_x = local_x;
			this.local_y = local_y;
		}
	}
	
	public WebDriverWrap(String settings_dir,
					int app_socket_port) {
		_web_driver = null;
		_settings_dir = settings_dir;
		_app_socket_port = app_socket_port;
		
		_window_handles = new Stack<String>();
		
		_open_count = 0;
		
		// Get working directory.
		String working_dir = System.getProperty("user.dir");
		working_dir = working_dir.replace('\\', '/');
        System.err.println("user dir is: " + working_dir);
        
        // Determine chome extension dir.
        if (FSWrap.platform_is_windows()) {
        	_chrome_ext_dir = working_dir + "/chromeextension";
        	_driver_location = working_dir + "/bin/chromedriver.exe";
        } else {
        	_chrome_ext_dir = working_dir + "/chromeextension";
        	_driver_location = working_dir + "/bin/chromedriver";
        }
        System.err.println("chrome ext dir is: " + _chrome_ext_dir);
        System.setProperty("webdriver.chrome.driver", _driver_location);
        
        // Determine our html page locations.
        String base_loc;
        if (FSWrap.platform_is_windows()) {
        	base_loc = "file:///" + working_dir; // Note 3 forward slashes as working_dir starts with a drive letter like D:.
        } else {
        	base_loc = "file://" + working_dir; // Note 2 forward slashes as working_dir already starts with a slash.
        }
        
        //base_loc = "https://www.youmacro.com/product";
        _app_page = base_loc + "/html/youmacro.html";
        _blank_page = base_loc + "/html/blank.html";
        
        // The hack to close the "Disable developer mode extensions" bubble only seems to work on windows.
        // we now only use this on windows, because on osx when we do this the CTRL-click won't open the link at all 
        // (it should open it in a new browser)
        // also on osx it won't close the first windows
        _open_post_window = false;
        _destroy_first_window = false;
        _socket_connect_page = base_loc + "/html/wait.html";
        //_socket_connect_page = base_loc + "/html/youmacro.html";
		
		// Build the webdriver service and start it.
		_service = new ChromeDriverService.Builder()
		        .usingAnyFreePort()
		        .build();
		start_service();
		
	}
	
    //------------------------------------------------------------------------------------------------
    // Web Driver Service Control.
    //------------------------------------------------------------------------------------------------
	public boolean start_service() {
		try {
			_service.start();
			return true;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			System.err.println("Unable to start chromedriver servier: ");
			e.printStackTrace();
			return false;
		}
	}
	
	public void stop_service() {
		// Note the main purpose of stopping the service is so that
		// chromedriver.exe process gets killed off.
		// This is called when quitting the application.
		_service.stop();
	}
	
	
    //------------------------------------------------------------------------------------------------
    // Browser App Control.
    //------------------------------------------------------------------------------------------------
	
	public boolean browser_is_open() {
	    try {
	        _web_driver.getTitle();
	        return true;
	    } catch (Exception ex) {
	        return false;
	    }
	}
	
	boolean open_browser() {
		if (browser_is_open()) {
			return true;
		}
		
		DesiredCapabilities capabilities = new DesiredCapabilities();
		{
		}
		
		ChromeOptions chrome_opts = new ChromeOptions();
		String app_port = Integer.toString(_app_socket_port);
		String url = _socket_connect_page + "?" + app_port;
		{
			// Create a new chrome user data dir for this browser instance.
			String chrome_user_data_dir = FSWrap.create_chrome_user_data_dir(_settings_dir);
			
			// Specify the start up page even though it doesn't navigate to it.
			// This doesn't work properly when there are spaces in the url.
			// Which we have on windows file based urls. Like our app and wait pages.
	        //chrome_opts.addArguments(url);
	        
	        // Load extensions. Separate multiple extension with a comma and not spaces between them.
			chrome_opts.addArguments("--load-extension=" + _chrome_ext_dir); 
	        
	        //chrome_opts.addArguments("--ignore-certificate-errors");
	        //chrome_opts.addArguments("--allow-insecure-localhost");
	        //chrome_opts.addArguments("--disable-web-security");
	        chrome_opts.addArguments("--user-data-dir=" + chrome_user_data_dir);
	        
	        //chrome_opts.addArguments("--first-run");
	        chrome_opts.addArguments("--no-first-run");
	        //chrome_opts.addArguments("--app=http://www.google.com");
	        
	        chrome_opts.addArguments("--incognito");
	        chrome_opts.addArguments("--enable-extensions");
	        
	        chrome_opts.addArguments("--na");
	        
	        //chrome_opts.addArguments("--allow-file-access-from-files");
	        //chrome_opts.addArguments("--enable-local-file-accesses");
	        
		}
		
		capabilities.setCapability(ChromeOptions.CAPABILITY, chrome_opts);
        
        //_web_driver = new ChromeDriver(capabilities);
        _web_driver = new RemoteWebDriver(_service.getUrl(), capabilities);
        //_web_driver.manage().window().setSize(new Dimension(1024,1150));
        
        
        //WebElement tempElement = _web_driver.findElement(By.cssSelector("body"));
		//tempElement.click();
		//tempElement.sendKeys(Keys.chord(Keys.CONTROL, "N"));
        
        
        // Enable incognito mode for our web extension.
        
        // Goto to the extensions page. There will only be one extension, ours.
        _web_driver.get("chrome://extensions-frame");
        
        // Wait for the checkbox to appears, as sometimes it won't be ready in time.
        WebDriverWait wait = new WebDriverWait(_web_driver, 10);
        
        
        // On macos there will be a chrome automation extension. 
        // On windows there will be no chrome automation extension.
        // So on macos find 2 incognito check boxes
        // On windows find 1 incognito check box.
        int num_check_boxes = 2;
        //if (FSWrap.platform_is_windows()) {
        //	num_check_boxes = 1;
        //}
        String any_check_box_xpath = "//label[@class='incognito-control']/input[@type='checkbox']";
        List<WebElement> elements = _web_driver.findElements(By.xpath(any_check_box_xpath));
        while (elements.size() < num_check_boxes) {
        	elements = _web_driver.findElements(By.xpath(any_check_box_xpath));
        }
        
        System.err.println("Found num elements: " + elements.size());
        
        // Make sure the first two incognito check boxes are checked.
		for (int i=0; i<num_check_boxes; i++) {
			WebElement e = elements.get(i);
			try {
				wait.until(ExpectedConditions.elementToBeClickable(e));
		        if (!e.isSelected()) {
		            e.click();
		        }
	        } catch (Throwable exception) {}
        }
        
        // The url on the command line doesn't seem to work, so we navigate to it.
        navigate_to(url);
        
        _open_post_window = true;
        
        return true;
	}
	
	boolean open_browser_post() {
		if (!_open_post_window) {
			return true;
		}
		
		_open_post_window = false;
    	_destroy_first_window = true;
    	
    	System.err.println("opening up new window for hack");
    	
        // Open up a new window. This will often overlap and hide our first window.
        // This is useful because we're waiting for a popup to popup in the first window before closing it.
        JavascriptExecutor js = (JavascriptExecutor) _web_driver;
        // We always open in a uniquely named window (smash_browse_#).
        String script = "window.open('"+ _app_page +"', '" + "smash_browse_" + (++_open_count) + "', 'noopener=yes,dependent=no,location=yes,toolbar=yes,scrollbars=yes,resizable=no,width=1024,height=1150');";
        js.executeScript(script);
        
        System.err.println("executed script: " + script);
        
        
        System.err.println("now waiting 4 seconds");
        
        // This wait allows the "Disable developer mode extensions" popup to popup in our first browser window.
        // Now when we close this window it will automatically act as if the cancel button
        // was pressed in this popup. It will now no longer popup again.
//        try {
//			Thread.sleep(15000);
//		} catch (InterruptedException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
        
        System.err.println("done waiting 4 seconds");
		return true;
	}
	
    void close_browser() {
    	if (browser_is_open()) {
    		_web_driver.quit();
    	}
    	_window_handles.clear();
    	
    	// Try to destory the chrome user data dirs.
    	FSWrap.destroy_chrome_user_data_dirs(_settings_dir);
    }
    
    void release_browser() {
    	if (browser_is_open()) {
    		
            // Goto to the extensions page. There will only be one extension, ours.
            _web_driver.get("chrome://extensions-frame");
            // Wait for the checkbox to appears, as sometimes it won't be ready in time.
            WebDriverWait wait = new WebDriverWait(_web_driver, 10);
            
            // Find 2 enable checkes boxes for youmacro and selenium.
            int num_check_boxes = 2;
            String xpath = "//*/div/div[2]/div/label/input";
            List<WebElement> elements = _web_driver.findElements(By.xpath(xpath));
            while (elements.size() < num_check_boxes) {
            	elements = _web_driver.findElements(By.xpath(xpath));
            }
            
            // Make sure both are disabled.
    		for (int i=0; i<num_check_boxes; i++) {
    			WebElement e = elements.get(i);
    			try {
    				wait.until(ExpectedConditions.elementToBeClickable(e));
    		        if (e.isSelected()) {
    		            e.click();
    		        }
    	        } catch (Throwable exception) {}
            }
            
            // Navigate back to previous page.
            navigate_back();
    		
    		_web_driver = null;
    	}
    	_window_handles.clear();
    }
    
    // Returns a promise which resizes the browser.
    void resize_browser(int width, int height) {
    	Dimension d = new Dimension(width, height);
        _web_driver.manage().window().setSize(d);
    }
    
    Dimension get_browser_size() {
    	return _web_driver.manage().window().getSize();
    }
    
    //------------------------------------------------------------------------------------------------
    // Browser Navigation.
    //------------------------------------------------------------------------------------------------
    
    String get_current_url() {
        return _web_driver.getCurrentUrl();
    }
    
    void navigate_to(String url){
        _web_driver.navigate().to(url);
    }
    
    void navigate_forward() {
        _web_driver.navigate().forward();
    }

    void navigate_back() {
        _web_driver.navigate().back();
    }

    void navigate_refresh() {
        _web_driver.navigate().refresh();
    }

    //------------------------------------------------------------------------------------------------
    // Extension Popups.
    //------------------------------------------------------------------------------------------------
    
	void switch_to_popup() {
		String current_handle = _web_driver.getWindowHandle();
		System.err.println("current handle: " + current_handle);
		
		System.err.println("num seen handles: " + _window_handles.size());
		Set<String> handles = _web_driver.getWindowHandles();
		System.err.println("current num handles: " +handles.size());
		
//		for (String handle : handles) {
//			if (!current_handle.equals(handle)) {
//				driver.switchTo().window(handle);
//			}
//		}
	}
	
    
    //------------------------------------------------------------------------------------------------
    // Browser Tabs.
    //------------------------------------------------------------------------------------------------

    void destroy_current_tab() {
        // Do nothing if have only one tab left.
        if (this._window_handles.size() == 1) {
            return;
        }  

        _web_driver.close();
        _window_handles.pop();
        _web_driver.switchTo().window(_window_handles.lastElement());
    }

	void update_current_tab() {
		System.err.println("updating current tab !!!!!!!!!");
        Set<String> handles = _web_driver.getWindowHandles();
        // Check for newly added tabs.
        int num_created = 0;
        for (String handle: handles) {
        	// If we already know about this handle then continue.
        	int index_from_top = _window_handles.search(handle);
        	if (index_from_top >= 1) {
        		continue;
        	}
        	
        	_web_driver.switchTo().window(handle);
        	String url = get_current_url();
        	// Skip tabs or windows which hold chrome debugging tools.
            // These usually have urls that start with: chrome:// or chrome-devtools://
        	if (!url.startsWith("chrome://") &&  !url.startsWith("chrome-devtools://")) {
            	// Otherwise we incorporate this handle as well.
                this._window_handles.push(handle);
                // Note there should ideally be one created handle per call to this method.
                num_created++;
        	}
        }
        if (num_created > 1) {
            System.err.println("WebDriverWrap.update_current_tab: found more than new tab was created.");
        }
        // Check for destroyed tabs.
        Stack<String> to_destroy = new Stack<String>();
        for (String handle: _window_handles) {
        	// If our cached handle is in the handles set, then it hasn't been removed.
        	if (handles.contains(handle)) {
        		continue;
        	}
            // Otherwise cache the handle for removal later.
        	to_destroy.push(handle);
            // Note there should ideally be one destroyed handle per call to this method.
            
        }
        int num_destroyed = 0;
        for (String handle: to_destroy) {
        	this._window_handles.remove(handle);
        	num_destroyed++;
        }
        if (num_destroyed > 1) {
            System.err.println("WebDriverWrap.update_current_tab: more than one tab has been destroyed.");
        }
        
	    // Switch to the latest tab.
	    _web_driver.switchTo().window(_window_handles.lastElement());
	    
	    if (_destroy_first_window) {
	    	destroy_first_window();
	    }
    }
	
	// Used to destroy the first window which will show the browser toolbar with extensions.
	void destroy_first_window() {
		_destroy_first_window = false;
		
		// Switch to the first window.
		_web_driver.switchTo().window(_window_handles.firstElement());
		System.err.println("closing first window!!!!!!");
		_web_driver.close();
		
		// Switch to the last window.
		_web_driver.switchTo().window(_window_handles.lastElement());
		_web_driver.manage().window().setSize(new Dimension(1024,1150));
	}
	
    //------------------------------------------------------------------------------------------------
    // Browser Frames.
    //------------------------------------------------------------------------------------------------
	
    // Negative numbers in the fe_index_path will make the path equivalent to an empty path.
    // Empty paths switch the frame to the top frame.
    void switch_to_frame(String fe_index_path) {
        System.err.println("switching to fe_index_path: " + fe_index_path);
        _web_driver.switchTo().defaultContent();

        // Loop through each sub frame.
        String[] splits = fe_index_path.split("/");
        for (String split: splits) {

            // Note when empty strings are split on '/', you get an array with one element which is an empty string.
            if (split == "") {
                continue;
            }

            // Get the frame index as a number.
            int frame_index = -1;
            try {
            	frame_index = Integer.parseInt(split);
            } catch (java.lang.NumberFormatException e) {
            	System.err.println("invalid fe_index_path");
            }

            // Stop going deeper if get a negative index.
            if (frame_index < 0) {
                System.err.println("switching found -1 so going to default content");
                return;
            }

            System.err.println("switching to frame: " + frame_index);
            
            // Debug dump of the current frame after switching.
            JavascriptExecutor je = (JavascriptExecutor)_web_driver;
            String name = (String) je.executeScript("return self.name");
            System.err.println("current frame name is: " + name);

            // Debug dump of the number of frames.
            List<WebElement> frames = _web_driver.findElements(By.tagName("iframe"));
            System.err.println("num frames: " + frames.size());

            // Debug dump of the number of frames.
            //List<WebElement> old_frames = _web_driver.findElements(By.tagName("frame"));
            //System.err.println("num old frames: " + old_frames.size());
            
            // We switch to the frame by using it's WebElement instead of by its index.
            // For example as follows: this.driver.switchTo().frame(frame_index)
            // This is because otherwise it doesn't match up with the fe_index_path produced in PageWrap.get_fe_index_path().
            System.err.println("switching to index: " + frame_index);
            _web_driver.switchTo().frame(frames.get(frame_index));
            
            // Debug dump of the name of the frame after switching.
            //String next_frame_info = (String) je.executeScript("return self.frameElement.className + \"--\" + self.name + \" w--\" + self.frameElement.clientWidth + \" h--\" + self.frameElement.clientHeight");
            //System.err.println("frame class--name is: " + next_frame_info);
        }
        
        //System.err.println("switch to frame with contents: " + _web_driver.getPageSource());
    }
    
    //------------------------------------------------------------------------------------------------
    // Dom Elements.
    //------------------------------------------------------------------------------------------------

    WebElement get_element(String fe_index_path, String xpath) {
        switch_to_frame(fe_index_path);
        return _web_driver.findElement(By.xpath(xpath));
    }

    List<WebElement> get_elements(String fe_index_path, String xpath) {
        switch_to_frame(fe_index_path);
        return _web_driver.findElements(By.xpath(xpath));
    }
    
    // Finds an element that is on top of given element and position.
    // For example sometimes there will be a transparent div overlayed on top of images and text for ads.
    void switch_to_top_most_element(WebElementWithPos we) {
        JavascriptExecutor js = (JavascriptExecutor) _web_driver;
        String script = "var rect = arguments[0].getBoundingClientRect(); " +
        				"var client_x = rect.left + arguments[1]; " +
        				"var client_y = rect.top + arguments[2]; " +
        				"var element = document.elementFromPoint(client_x, client_y);" +
        				//"console.log('rect: ' + rect.left + ',' + rect.right + ',' + rect.top + ',' + rect.bottom);" +
        				"return element;";
        Object obj = js.executeScript(script, we.element, we.local_x, we.local_y);
        //System.err.println("script returned: " + obj.toString());
        WebElement top_element = (WebElement)obj;
        if ((top_element != null) && !top_element.equals(we.element)) {
        	Point t = top_element.getLocation();
        	Point e = we.element.getLocation();
        	int diff_x = e.x - t.x;
        	int diff_y = e.y - t.y;
        	
        	we.element = top_element;
        	we.local_x += diff_x;
        	we.local_y += diff_y;
        }
    }

    //------------------------------------------------------------------------------------------------
    // Actions on Dom Elements.
    //------------------------------------------------------------------------------------------------
    
    void send_key(String fe_index_path, String xpath, String key) {
        get_element(fe_index_path, xpath).sendKeys(key);
    }
    
    void send_key_to_active(String key) {
    	WebElement active = _web_driver.switchTo().activeElement(); 
        active.sendKeys(key);
    }

    String get_text(String fe_index_path, String xpath) {
        return get_element(fe_index_path, xpath).getText();
    }

    void send_text(String fe_index_path, String xpath, String text) {
        get_element(fe_index_path, xpath).sendKeys(Keys.HOME, Keys.chord(Keys.SHIFT, Keys.END), text);
    }
    
    void send_text_to_active(String text) {
    	WebElement active = _web_driver.switchTo().activeElement(); 
    	active.sendKeys(text); 
    }
    
    void click_on_element(String fe_index_path, String xpath, int local_x, int local_y, boolean hold_ctrl) {
        WebElement e = get_element(fe_index_path, xpath);
        WebElementWithPos we = new WebElementWithPos(e, local_x, local_y);
        switch_to_top_most_element(we);
        
        Dimension dim = we.element.getSize();
        System.err.println("click elem dim is: " + dim.getWidth() + "," + dim.getHeight());
        Actions seq = new Actions(_web_driver);
        if ((we.local_x >= 0) && (we.local_y >= 0) && (we.local_x < dim.width) && (we.local_y < dim.height)) {
            seq = seq.moveToElement(we.element, we.local_x, we.local_y);
        } else {
            seq = seq.moveToElement(we.element, dim.width/2, dim.height/2);
        }
        if (hold_ctrl) {
            if (FSWrap.platform_is_windows()) {
                    seq = seq.keyDown(Keys.CONTROL);
            } else {
                    seq = seq.keyDown(Keys.COMMAND);
            }
        }
        seq = seq.click();
	    if (hold_ctrl) {
	    	if (FSWrap.platform_is_windows()) {
	                seq = seq.keyUp(Keys.CONTROL);
	    	} else {
	                seq = seq.keyUp(Keys.COMMAND);
	        }
	    }
        seq.perform();
    }

    void mouse_over_element(String fe_index_path, String xpath, int local_x, int local_y) {
        WebElement e = get_element(fe_index_path, xpath);
        WebElementWithPos we = new WebElementWithPos(e, local_x, local_y);
        switch_to_top_most_element(we);
        
        Dimension dim = we.element.getSize();
        Actions seq = new Actions(_web_driver);
        
        if ((we.local_x >= 0) && (we.local_y >= 0) && (we.local_x < dim.width) && (we.local_y < dim.height)) {
        	seq = seq.moveToElement(we.element, we.local_x, we.local_y);
        } else {
        	seq = seq.moveToElement(we.element, dim.width/2, dim.height/2);
        }
        seq.perform();
    }
    
    void choose_option(String fe_index_path, String xpath, String option_text) {
        WebElement element = get_element(fe_index_path, xpath);
        element.findElement(By.xpath("option[normalize-space(text())=\"" + option_text + "\"]")).click();
    }
    
//    void scroll(MessageEnums.DirectionType dir) {
//    	String script = "";
//    	//String prefix = "window.document.documentElement.style.overflow = 'auto';"; // show scrollbar
//    	//String suffix = "window.document.documentElement.style.overflow = 'hidden';"; // hide scrollbar
//    	
//    	switch (dir) {
//        	case down: {
//        		script = "window.scrollBy(0, window.document.documentElement.clientHeight * 0.80);";
//        		break;
//        	}
//        	case up: {
//        		script = "window.scrollBy(0, window.document.documentElement.clientHeight * -0.80);";
//        		break;
//        	}
//        	case right: {
//        		script = "window.scrollBy(window.document.documentElement.clientWidth * 0.80, 0);";
//        		break;
//        	}
//        	case left: {
//        		script = "window.scrollBy(window.document.documentElement.clientWidth * -0.80, 0);";
//        		break;
//        	}
//    	}
//    	JavascriptExecutor js = (JavascriptExecutor) _web_driver;
//    	js.executeScript(script);
//    }
    
    // image_url is the url of the image.
    // format is something like "png".
    // filename is the filename to save to on the local computer.
    boolean download_image(String image_url, String directory, String base_filename, String format_ext) {
    	try {
		    URL url = new URL(image_url);
		    BufferedImage image = ImageIO.read(url);
		    if (!format_ext.equals("gif") && !format_ext.equals("png") && !format_ext.equals("jpg")) {
		    	format_ext = "png";
		    }
		    File dir = new File(directory);
		    dir.mkdirs();
		    File file = File.createTempFile(base_filename, "."+format_ext, dir);
		    return ImageIO.write(image, format_ext, file);
    	} catch(Exception e) {
    		System.err.println("Error downloading image: " + e.getMessage());
    	}
    	return false;
    }
    
}
