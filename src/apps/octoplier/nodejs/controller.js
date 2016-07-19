//------------------------------------------------------------------------------------------------
//Globals.
//------------------------------------------------------------------------------------------------

// We use strict mode. In strict mode we can't use undeclared variables.
"use strict";

// Our imports.
var WebSocketServer = require('ws').Server
var fs = require('fs')
var HttpServer = require('http')
var HttpsServer = require('https')

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

function write_to_file(filename, text) {
    fs.appendFileSync(filename, text)
} 

function write_to_dump(text) {
    write_to_file("./dump.txt", test)
} 

function read_file(filename) {
    var script = fs.readFileSync(filename, "utf8")
    return script
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

function create_socket_server(config, message_handler) {
    // Dummy handler for http/s requests.
    var dummy_handler = function(req, res) {
        res.writeHead(200);
        res.end("Http/s server with websockets is running!\n")
    };

    // Create the http/s server.
    var httx_server = null;
    if ( config.ssl ) {
        httx_server = HttpsServer.createServer({
            key: fs.readFileSync( config.ssl_key ),
            cert: fs.readFileSync( config.ssl_cert )
        }, dummy_handler ).listen( config.port )
    } else {
        httx_server = HttpServer.createServer(dummy_handler).listen( config.port )
    }

    // Create the socket server.
    var socket_server = new WebSocketServer({server: httx_server});
    var socket = null
    socket_server.on('connection', function (wsConnect) {
        socket = wsConnect
        socket.on('message', message_handler)
    })
    return {socket_server: socket_server, socket: socket}
}


//------------------------------------------------------------------------------------------------
// Secure web socket to communicate with chrome extension.
//------------------------------------------------------------------------------------------------
var extension_socket_server_config = {
        ssl: true,
        port: 8083,
        ssl_key: './key.pem',
        ssl_cert: './cert.pem'
    };
var result = create_socket_server(extension_socket_server_config, on_message_from_extension)
var extension_socket_server = result.socket_server
var extension_socket = result.socket

function send_to_extension(obj) {
    var socket_message = new SocketMessage(obj)
    extension_socket.send(socket_message.get_text())
} 

//The extension can send us requests from the smash browse context menu,
//or it can send us reponses from request originating within the app,
//which in turn could have been triggered from the smash browse context menu.
function on_message_from_extension(message) {
    var msg = new SocketMessage(message)
    log_info('message from extension: ' + msg.get_text());
    if (msg.is_request()) {
        // We have a request from the extension.
        var request = msg.get_obj()
        switch(request.request) {
            case 'dummy':
                break
        }
    } else {
        // We have a response from the extension.
        // We don't handle any responses ourself. We just relay them to the app.
        var response = msg.get_obj()
        send_to_app(response)
    }
}

console.log('-----------------------------------')

//------------------------------------------------------------------------------------------------
// Regular WebSocket to communicate with app.
//------------------------------------------------------------------------------------------------
var app_socket_server_config = {
        ssl: false,
        port: 8082,
    };
var result = create_socket_server(app_socket_server_config, on_message_from_app)
var app_socket_server = result.socket_server
var app_socket = result.socket

function send_to_app(obj) {
    var socket_message = new SocketMessage(obj)
    app_socket.send(socket_message.get_text())
}

//app_socket_server.on('connection', function connection(ws_arg) {
//    console.log('bbbbbbbbbbbbbbbbbbbbbbbb')
//    app_socket = ws_arg
//    log_info("controller now connected to native app");
//    app_socket.on('message', on_message_from_app);
//}); 

function on_message_from_app(message) {
    console.log('cccccccccccccccccccccccccc')
    var msg = new SocketMessage(message)
    
    // Make sure the message is a request.
    if (!msg.is_request()) {
        console.log("Error on_message_from_app received something other than a request.")
    }
    
    // Handle the request.
    var request = msg.get_obj()
    log_info('nodejs received: ' + msg.get_text());
    switch (request.request) {
        case 'open_browser':
            open_browser()
            send_to_app({response: true})
            break
        case 'close_browser':
            close_browser()
            send_to_app({response: true})
            break
        case 'goto_url':
            goto_url(body).then(function() {
                send_to_app({response: true})
            }, function(error) {
                send_to_app({response: false})
            })
            break
        case 'back':
            navigate_back().then(function(){
                send_to_app({response: true})
            }, function(error) {
                send_to_app({response: false})
            })
            break
        case 'forward':
            navigate_forward().then(function(){
                send_to_app({response: true})
            }, function(error) {
                send_to_app({response: false})
            })
            break
        case 'get_xpath_from_elem_cache':
            send_to_extension(request)
            // The extension will return the response.
            break
        case 'extract_text':
            send_to_extension(request)
            // The extension will return the response.
            break
        case 'send_key':
            send_key(request.xpath, request.key)
            // A promise will return the response.
            break
        case 'send_text':
            send_text(request.xpath, request.text)
            // A promise will return the response.
            break
        case 'click_on_element':
            click_on_element(request.xpath)
            // A promise will return the response.
            break
        case 'mouse_over_element':
            mouse_over_element(request.xpath, request.relative_x, request.relative_y)
            // A promise will return the response.
            break;
            
    }
}

//------------------------------------------------------------------------------------------------
// Browser Control.
//------------------------------------------------------------------------------------------------

var webdriver = null
var chrome = null
var driver = null
var Key = null
var By = null
var until = null
var flow = null

var open_browser = function () {
    try {
        webdriver = require('selenium-webdriver')
        chrome = require('selenium-webdriver/chrome');
        By = webdriver.By
        Key = webdriver.Key
        until = webdriver.until

        var chromeOptions = new chrome.Options()
        //Win_x64-389148-chrome-win32
        //Win-338428-chrome-win32
        //chromeOptions.setChromeBinaryPath('/downloaded_software/chromium/Win_x64-389148-chrome-win32/chrome-win32/chrome.exe')
        chromeOptions.addArguments("--load-extension=D:/src/ngsinternal/src/apps/octoplier/chrome")
        chromeOptions.addArguments("--ignore-certificate-errors")
        chromeOptions.addArguments("--disable-web-security")
        chromeOptions.addArguments("--user-data-dir")
        
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

var close_browser = function () {
    driver.quit()
}

var resize_browser = function (width, height) {
    driver.manage().window().setSize(width, height);
}

var jitter_browser_size = function() {
    driver.manage().window().getSize().then( function(s) {
        driver.manage().window().setSize(s.width+1, s.height+1).then(function() {
            driver.manage().window().setSize(s.width, s.height);
        })
    })
}

//------------------------------------------------------------------------------------------------
// Browser Actions.
//------------------------------------------------------------------------------------------------

// Note the returned promise nevers errors.
// It will try to wait for the xpath
// element as long as it can.

var trivial_wait_time = 1000 //1000
var critical_wait_time = 1000 //30000

//Returns a promise which navigates the browser to another url.
var goto_url = function (url) {
    return driver.get(url)
}

//Returns a promise which navigates the browser forward in the history.
var navigate_forward = function () {
    return driver.navigate().forward();
}

//Returns a promise which navigates the browser backwards in the history.
var navigate_back = function () {
    return driver.navigate().back();
}

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
                log_info("type of element: " + typeof(element))
                return driver.wait(until.elementIsVisible(element), wait_milli).then(
                        function(element){return element},
                        function(error){log_info('Warning: element was not visible: ' + xpath); throw error})
            }
    )
}

// Creates promise chain which will type one key into an element.
var send_key = function(xpath, key) {
    p = get_visible_element(xpath, critical_wait_time).then(function(element) {
        return element.sendKeys(key)
    })
    terminate_chain(p)
}

// Creates promise chain which will set text on an element.
var send_text = function(xpath, text) {
    p = get_visible_element(xpath, critical_wait_time).then(function(element) {
        return element.sendKeys(Key.HOME,Key.chord(Key.SHIFT,Key.END),text)
    })
    terminate_chain(p)
}

// Creates a promise chain which will click on an element.
var click_on_element = function (xpath) {
    p = get_visible_element(xpath, critical_wait_time).then(function(element) {
        return driver.actions().click(element).perform()
    })
    terminate_chain(p)
}

// Creates a promise chain which will mouseover an element.
var mouse_over_element = function (xpath, relative_x, relative_y) {
    p = get_visible_element(xpath, trivial_wait_time).then(function(element) {
        return driver.actions().mouseMove(element, {x: relative_x , y: relative_y}).perform().then(
                function() {},
                function(error){log_info('Warning: could not move_over_element (computedstyle): ' + xpath);throw(error)}
        )
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
            // Send success response to the app.
            send_to_app({response: true})
        },
        function(error) {
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
