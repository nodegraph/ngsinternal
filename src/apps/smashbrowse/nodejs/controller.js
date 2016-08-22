//------------------------------------------------------------------------------------------------
//Globals.
//------------------------------------------------------------------------------------------------

// Note default arguments for function requires ES6.
// This version of nodejs doesn't seem to have ES6 yet.

// We use strict mode. In strict mode we can't use undeclared variables.
"use strict";

// Our imports.
var WebSocketServer = require('ws').Server
var fs = require('fs')
var HttpServer = require('http')
var HttpsServer = require('https')
var Path = require('path')
var request = require('request');

//------------------------------------------------------------------------------------------------
//Debugging Utils.
//------------------------------------------------------------------------------------------------
var debugging = true
function log_info(msg) {
    if (debugging) {
        console.log("Info: " + msg)
    }
}
function log_error(msg) {
    if (debugging) {
        console.log("Error: " + msg)
    }
}
function log_exception(e) {
    if (debugging) {
        console.log("Exception: " + e.message + " stack: " + e.stack)
    }
}

process.on('uncaughtException', function(e) {
    log_error("exception caught at top!")
    log_exception(e)
});


//------------------------------------------------------------------------------------------------
//File I/O.
//------------------------------------------------------------------------------------------------

// This is the bin directory of the app. Nodejs is also placed there.
var g_nodejs_dir = process.cwd()
var g_user_data_dir = process.argv[2]  // This will be fed on the command line as node.exe controller.js c:/.../some_dir

// Make sure the nodejs dir exists in the user data dir.
var dir = Path.join(g_user_data_dir, "nodejs")
if (!file_exists(dir)) {
	create_dir(dir)
}

function file_exists(path) {
	try {
		fs.accessSync(path, fs.F_OK);
		return true
	} catch(e) {
        return false
    }
	return false
}

function write_to_file(filename, text) {
    fs.writeFileSync(filename, text, 'utf8');
}


function append_to_file(filename, text) {
    fs.appendFileSync(filename, text)
} 

function write_to_dump(text) {
    append_to_file('./dump.txt', test)
} 

function read_file(filename) {
    var script = fs.readFileSync(filename, 'utf8')
    return script
}

function delete_file(filename) {
    fs.unlinkSync(filename);
}

function delete_dir(path) {
    if( fs.existsSync(path) ) {
        fs.readdirSync(path).forEach(function(file) {
            var full_path = Path.join(path,file)
            if(fs.statSync(full_path).isDirectory()) {
                delete_dir(full_path)
            } else {
                fs.unlinkSync(full_path);
            }
        });
        fs.rmdirSync(path);
    }
};

function create_dir(path) {
    if (!fs.existsSync(path)){
        fs.mkdirSync(path);
    }
}

  
//------------------------------------------------------------------------------------------------
//Socket Message.
//------------------------------------------------------------------------------------------------
var SocketMessage = function(arg) {
    if (typeof arg === 'string') {
        this.text = arg
        this.obj = JSON.parse(this.text)
    } else if (arg && typeof arg === 'object') {
        this.text = JSON.stringify(arg)
        this.obj = arg
    }
}

SocketMessage.prototype.get_obj = function() {
    return this.obj
}

SocketMessage.prototype.get_text = function() {
    return this.text
}

SocketMessage.prototype.is_request = function() {
    return this.obj.hasOwnProperty('request')
}

SocketMessage.prototype.is_response = function() {
    return !this.is_request()
}

//------------------------------------------------------------------------------------------------
//Socket server creation.
//------------------------------------------------------------------------------------------------

function create_socket_server(config, message_handler, done) {
    // Dummy handler for http/s requests.
    var dummy_handler = function(req, res) {
        res.writeHead(200);
        res.end("Http/s server with websockets is running!\n")
    };

    // Create the http/s server.
    var httx_server = null;
    if (config.ssl) {
        httx_server = HttpsServer.createServer({
            key: fs.readFileSync(config.ssl_key),
            cert: fs.readFileSync(config.ssl_cert)
        }, dummy_handler)
        
    } else {
        httx_server = HttpServer.createServer(dummy_handler)
    }
    
    // On error.
    httx_server.once('error', function(error) {
        if (error.code === 'EADDRINUSE') {
            // The current config port number is in use.
            // So we increment it and recurse.
            config.port += 1
            create_socket_server(config, message_handler, done)
        } else {
            // Call the callback with failed result.
            done({server: null, socket: null})
        }
    });
    
    // On success.
    httx_server.once('listening', function() {
        // Create the socket server.
        var server_info = {}
        server_info.server = new WebSocketServer({server: httx_server});
        server_info.socket = null
        server_info.port = config.port
        server_info.server.on('connection', function (wsConnect) {
            server_info.socket = wsConnect
            server_info.socket.on('message', message_handler)
        })
        // Now call the callback.
        done(server_info)
    });

    httx_server.listen(config.port)
}


//------------------------------------------------------------------------------------------------
// Secure web socket to communicate with chrome extension.
//------------------------------------------------------------------------------------------------
var extension_server_config = {
        ssl: true,
        port: 8093,
        ssl_key: './key.pem',
        ssl_cert: './cert.pem'
    };
var extension_server = null
var extension_server_file = Path.join(g_user_data_dir, 'chromeextension', 'extensionserverport.js')

// Called when the extension server is built.
function extension_server_built(server) {
    extension_server = server
    var text = 'var g_nodejs_port = ' + extension_server.port
    write_to_file(extension_server_file, text)
}

//Create the extension server.
if (file_exists(extension_server_file)) {
	delete_file(extension_server_file)
}
create_socket_server(extension_server_config, receive_from_extension, extension_server_built)

//Send an obj as a message to the extension.
function send_to_extension(obj) {
    var socket_message = new SocketMessage(obj)
    extension_server.socket.send(socket_message.get_text())
} 

//The extension can send us requests from the smash browse context menu,
//or it can send us reponses from request originating within the app,
//which in turn could have been triggered from the smash browse context menu.
function receive_from_extension(message) {
    console.log('nodejs got message from extension: ' + message)
    var msg = new SocketMessage(message)
    if (msg.is_request()) {
        // We have a request from the extension.
        var request = msg.get_obj()
         send_to_app(message);
    } else {
        // We have a response from the extension.
        // We don't handle any responses ourself. We just relay them to the app.
        var response = msg.get_obj()
        send_to_app(response)
    }
}

//------------------------------------------------------------------------------------------------
// Regular WebSocket to communicate with app.
//------------------------------------------------------------------------------------------------
var app_server_config = {
        ssl: false,
        port: 8092,
    };
var app_server = null
var app_server_file = Path.join(g_user_data_dir, 'nodejs','appserverport.txt')

//Called when the app server is built.
function app_server_built(server) {
    app_server = server
    write_to_file(app_server_file, app_server.port)
}

//Create the app server.
if (file_exists(app_server_file)) {
	delete_file(app_server_file)
}
create_socket_server(app_server_config, receive_from_app, app_server_built)


//Send an obj as a message to the app.
function send_to_app(obj) {
    var socket_message = new SocketMessage(obj)
    app_server.socket.send(socket_message.get_text())
}

//The can send us messages either from c++ or from qml.
//These requests will be handled by webdriverjs, or the extension scripts in the browser.
//Requests will always return with a response message containing return values.
function receive_from_app(message) {
    var msg = new SocketMessage(message)
    console.log('nodejs got message from app: ' + message)
    
    // Make sure the message is a request.
    if (!msg.is_request()) {
        console.log("Error receive_from_app received something other than a request.")
    }
    
    // Handle the request.
    var request = msg.get_obj()
    switch (request.request) {
    	case 'shutdown': {
    		close_browser().then(function(){process.exit(-1)})
    		break;
    	}
        case 'check_browser_is_open':
            function on_response(open) {
                if (!open) {
                    open_browser()
                }
            }
            browser_is_open(on_response)
            // No response is sent back to the app. Typically this is called in a polling manner.
            break
        case 'resize_browser':
            resize_browser(request.width, request.height)
            // No response is sent back to the app. Typically this is called in a polling manner.
            break
        case 'open_browser':
            open_browser(request.url)
            send_to_app({response: true})
            break
        case 'close_browser':
            close_browser()
            send_to_app({response: true})
            break
        case 'navigate_to':
            navigate_to(request.url).then(function() {
                send_to_app({response: true})
            }, function(error) {
                send_to_app({response: false})
            })
            break
        case 'navigate_back':
            navigate_back().then(function(){
                send_to_app({response: true})
            }, function(error) {
                send_to_app({response: false})
            })
            break
        case 'navigate_forward':
            navigate_forward().then(function(){
                send_to_app({response: true})
            }, function(error) {
                send_to_app({response: false})
            })
            break
        case 'navigate_refresh':
            navigate_refresh().then(function(){
                send_to_app({response: true})
            }, function(error) {
                send_to_app({response: false})
            })
            break
        case 'perform_action':
            if (request.xpath) {
                // If the xpath has been resolved by the content script,
                // then we let webdriver perform actions.
                switch(request.action) {
                    case 'send_click':
                        click_on_element(request.xpath)
                        break
                    case 'send_text':
                        send_text(request.xpath, request.text)
                        break
                    case 'send_enter':
                        send_key(request.xpath, Key.RETURN)
                        break
                    case 'get_text':
                        get_text(request.xpath)
                        break
                    case 'select_option':
                        select_option(request.xpath, request.option_text)
                        break
                    case 'scroll_down':
                    case 'scroll_up':
                    case 'scroll_right':
                    case 'scroll_left':
                        // Scroll actions need to be performed by the extension
                        // so we pass it through.
                        send_to_extension(request) 
                        break
                }
            } else {
                // Send request to extension to resolve the xpath,
                // and to unblock the events in the content script so that
                // the webdriver actions can take effect on the elements.
                send_to_extension(request) 
            }
            break
        default:
            // By default we send requests to the extension. 
            // (It goes through bg script, and then into content script.)
            console.log('zzzzzzzzzzzzzzzzzz')
            send_to_extension(request)
            break
            
    }
}

//------------------------------------------------------------------------------------------------
// Browser Control.
//------------------------------------------------------------------------------------------------

var webdriver = require('selenium-webdriver')
var chrome = require('selenium-webdriver/chrome');
var driver = null
var Key = webdriver.Key
var By = webdriver.By
var until = webdriver.until
var flow = null

function browser_is_open(callback) {
    if (driver) {
        driver.getTitle().then(
                function(){callback(true)},
                function(){callback(false)});
    } else {
        callback(false)
    }
}

function open_browser(url) {
    try {
        // Remove the files in the chrome user data dir.
        var dir = Path.join(g_user_data_dir, "chromeuserdata")
        if (file_exists(dir)) {
        	delete_dir(dir)
    	}
        create_dir(dir)

        var chromeOptions = new chrome.Options()
        //Win_x64-389148-chrome-win32
        //Win-338428-chrome-win32
        //chromeOptions.setChromeBinaryPath('/downloaded_software/chromium/Win_x64-389148-chrome-win32/chrome-win32/chrome.exe')
        chromeOptions.addArguments("--load-extension=" + g_user_data_dir + "/chromeextension")
        chromeOptions.addArguments("--ignore-certificate-errors")
        chromeOptions.addArguments("--disable-web-security")
        chromeOptions.addArguments("--user-data-dir=" + g_user_data_dir + "/chromeuserdata")
        chromeOptions.addArguments("--first-run")
        //chromeOptions.addArguments("--app=file:///"+url)
        
        // "C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" --disable-web-security --user-data-dir --app=https://www.google.com

        driver = new webdriver.Builder()
        .forBrowser('chrome')//.forBrowser('firefox')
        .setChromeOptions(chromeOptions)
        .build();

        flow = webdriver.promise.controlFlow()
        
        // Set default settings.
        driver.manage().timeouts().pageLoadTimeout(60000);
        
        webdriver.promise.controlFlow().on('uncaughtException', function(e) {
            console.error('Unhandled error: ' + e);
        });
    } catch (e) {
        log_exception(e)
    }
}

function close_browser() {
    return driver.quit()
    //driver = null
}

//Returns a promise which navigates the browser to another url.
function navigate_to(url) {
    return driver.navigate().to(url)
}

//Returns a promise which navigates the browser forward in the history.
function navigate_forward() {
    return driver.navigate().forward();
}

//Returns a promise which navigates the browser backwards in the history.
function navigate_back() {
    return driver.navigate().back();
}

//Returns a promise which refreshes the browser.
function navigate_refresh() {
    return driver.navigate().refresh();
}

//Returns a promise which resizes the browser.
function resize_browser(width, height) {
    return driver.manage().window().setSize(width, height);
}

//Returns a promise which jitters the browser size.
function jitter_browser_size() {
    return driver.manage().window().getSize().then( function(s) {
        driver.manage().window().setSize(s.width+1, s.height+1).then(function() {
            driver.manage().window().setSize(s.width, s.height);
        })
    })
}

//------------------------------------------------------------------------------------------------
// Dom Element Actions.
//------------------------------------------------------------------------------------------------

// Note the returned promise nevers errors.
// It will try to wait for the xpath
// element as long as it can.

var trivial_wait_time = 1000 //1000
var critical_wait_time = 1000 //30000

//Returns a promise which evaulates to an existing element or is rejected.
var get_element = function (xpath, wait_milli) {
    return flow.execute(function() {
        var d = webdriver.promise.defer();
        // Wait for element to be present.
        driver.wait(function () {return driver.isElementPresent(By.xpath(xpath));}, wait_milli)
        // Then retrieve the actual element.
        .then(function(found){
            if (found) {
                driver.findElement(By.xpath(xpath)).then(
                        function(element) {
                            d.fulfill(element)
                        })
            } else {
                d.reject(null)
            }
        }, function(error) {
            log_info('Warning: could not find: ' + xpath)
            throw error
        })
        return d.promise
    })
}

//Returns a promise which evaulates to a visible element.
var get_visible_element = function (xpath, wait_milli) {
    return get_element(xpath, wait_milli).then(
            function (element) {
                return driver.wait(until.elementIsVisible(element), wait_milli).then(
                        function(element){return element},
                        function(error){log_info('Warning: element was not visible: ' + xpath); throw error})
            }
    )
}

// Creates promise chain which will type one key into an element.
var send_key = function(xpath, key) {
    var p = get_visible_element(xpath, critical_wait_time).then(function(element) {
        return element.sendKeys(key)
    })
    terminate_chain(p)
}

var get_text = function(xpath) {
    var p = get_visible_element(xpath, critical_wait_time).then(function(element) {
        return element.getText()
    })
    terminate_chain(p)
}

// Creates promise chain which will set text on an element.
var send_text = function(xpath, text) {
    var p = get_visible_element(xpath, critical_wait_time).then(function(element) {
        return element.sendKeys(Key.HOME,Key.chord(Key.SHIFT,Key.END),text)
    })
    terminate_chain(p)
}

// Creates a promise chain which will click on an element.
var click_on_element = function (xpath) {
    console.log("clicking on element: " + xpath)
    var p = get_visible_element(xpath, critical_wait_time).then(function(element) {
        return driver.actions().click(element).perform()
    })
    terminate_chain(p)
}

// Creates a promise chain which will mouseover an element.
var mouse_over_element = function (xpath, relative_x, relative_y) {
    var p = get_visible_element(xpath, trivial_wait_time).then(function(element) {
        return driver.actions().mouseMove(element, {x: relative_x , y: relative_y}).perform().then(
                function() {},
                function(error){log_info('Warning: could not move_over_element (computedstyle): ' + xpath);throw(error)}
        )
    })
    terminate_chain(p)
}

// Creates a promise which will select an option in a select dropdown.
var select_option = function(xpath, option_text) {
    var p = get_visible_element(xpath, critical_wait_time).then(function(element) {
        return element.findElement(By.xpath('option[normalize-space(text())="' + option_text + '"]')).click()
    })
    terminate_chain(p)
}

//// Creates a promise which will send results back to the native app.
//var send_result = function () {
//    p = flow.execute( function() {
//        var result = {test: 'replace this with real value'} // This should be changed to the real result when ready.
//        var d = webdriver.promise.defer();
//        app_socket.send('ok: ' + JSON.stringify(result))
//        d.fulfill(result)
//        return d.promise
//    })
//    terminate_chain(p)
//}

// Helper to terminate promise chains.
var terminate_chain = function(p) {p.then(
        function() {
            // Make sure the events are blocked. They may be unblocked to allow webdriver actions to take effect.
            send_to_extension({request: 'block_events'})
            // Send success response to the app.
            console.log('terminating chain success with numargs: ' + arguments.length)
            if (arguments.length == 0) {
                send_to_app({response: true})
            } else {
                // Send the first argument in the response.
                send_to_app({response: true, value: arguments[0]})
            }
        },
        function(error) {
            // Make sure the events are blocked. They may be unblocked to allow webdriver actions to take effect.
            send_to_extension({request: 'block_events'})
            // Output error details.
            log_error("Error in chain!")
            if (error.stack.indexOf('mouse_over_element') >=0) {
                log_error('error from mouse_over_element')
            } else if (error.stack.indexOf('click_on_element') >=0) {
                log_error('error from click_on_element')
            } else if (error.stack.indexOf('send_key') >=0) {
                log_error('error from send_key')
            } else if (error.stack.indexOf('send_text') >=0) {
                log_error('error from send_text')
            } else {
                log_exception(error)
            }
            // Send failure reponse to the app.
            send_to_app({response: false})
        })
}
