//------------------------------------------------------------------------------------------------
//Debugging State.
//------------------------------------------------------------------------------------------------
var debugging = true
var log_info = function(msg) {
    if (debugging) {
        console.log("Info: " + msg)
    }
}
var log_error = function(msg) {
    if (debugging) {
        console.log("Error: " + msg)
    }
}
var log_exception = function(e) {
    if (debugging) {
        console.log("Exception: " + e.message + " stack: " + e.stack)
    }
}

//------------------------------------------------------------------------------------------------
//File I/O.
//------------------------------------------------------------------------------------------------
var fs = require('fs');

var write_to_file = function(filename, text) {
    fs.appendFileSync(filename, text)
} 

var write_to_dump = function(text) {
    write_to_file("./dump.txt", test)
} 

var read_file = function(filename) {
    var script = fs.readFileSync(filename, "utf8")
    return script
}

//------------------------------------------------------------------------------------------------
//Errors and utils.
//------------------------------------------------------------------------------------------------

process.on('uncaughtException', function(e) {
    log_error("exception caught at top!")
    log_exception(e)
});

var convert_key_identifier = function(id) {
    if (!is_modifier(id)) {
        return "'" + id.replace('U+','\\u') + "'"
    }
    return "Key." + id.toUpperCase()
}

var is_modifier = function(id) {
    if ((id.indexOf('U+') != 0)) {
        return true
    }
    return false
}

//Returns true if it is a DOM node
function is_node(o){
  return (
    typeof Node === "object" ? o instanceof Node : 
    o && typeof o === "object" && typeof o.nodeType === "number" && typeof o.nodeName==="string"
  );
}

//Returns true if it is a DOM element    
function is_element(o){
  return (
    typeof HTMLElement === "object" ? o instanceof HTMLElement : //DOM2
    o && typeof o === "object" && o !== null && o.nodeType === 1 && typeof o.nodeName==="string"
);
}


//------------------------------------------------------------------------------------------------
//Our main internal state.
//------------------------------------------------------------------------------------------------
var ActionMode = {
        Idle: 0,
        Recording: 1,
        Replaying: 2
}
var mode = ActionMode.Idle

//------------------------------------------------------------------------------------------------
//The recording state.
//------------------------------------------------------------------------------------------------

var recorded_cmds = []

//------------------------------------------------------------------------------------------------
//The replaying state.
//------------------------------------------------------------------------------------------------

var replay_cmds = []

var process_replay = function() {
    while(replay_cmds.length) {
        var cmd = replay_cmds.shift()
        if (cmd == "wait_till_page_is_ready()\n") {
            return
        }
        console.log("processing cmd: " + cmd)
        eval(cmd)
    }
    if (replay_cmds.length == 0) {
        mode = ActionMode.Idle
        close_browser()
    }
}

//------------------------------------------------------------------------------------------------
// Secure web socket to communicate with chrome extension.
//------------------------------------------------------------------------------------------------

var WebSocketServer = require('ws').Server
var extension_socket_server = null
var extension_socket = null

var start_extension_socket = function(){

    "use strict";

    var fs = require('fs');

    // you'll probably load configuration from config
    var cfg = {
        ssl: true,
        port: 8083,
        ssl_key: './key.pem',
        ssl_cert: './cert.pem'
    };

    var httpServ = ( cfg.ssl ) ? require('https') : require('http');
    var app      = null;

    // dummy request processing
    var processRequest = function( req, res ) {
        res.writeHead(200);
        res.end("Https server with websockets is running!\n");
    };

    if ( cfg.ssl ) {
        app = httpServ.createServer({
            // providing server with  SSL key/cert
            key: fs.readFileSync( cfg.ssl_key ),
            cert: fs.readFileSync( cfg.ssl_cert )
        }, processRequest ).listen( cfg.port );
    } else {
        app = httpServ.createServer( processRequest ).listen( cfg.port );
    }

    // passing or reference to web server so WS would knew port and SSL capabilities
    extension_socket_server = new WebSocketServer( { server: app } );
    extension_socket_server.on( 'connection', function ( wsConnect ) {
        extension_socket = wsConnect
        log_info("browsercontroller now connected to extension");
        extension_socket.on('message', on_message_from_extension);
    });
}

var on_message_from_extension = function (data) {
    log_info('message from extension: ' + data );
    var msg = JSON.parse(data)
    if (msg.code == 'page_is_ready') {
        // Continue with the rest of the replay.
        if (mode == ActionMode.Replaying) {
            jitter_browser_size()
            process_replay()
        } else if (mode == ActionMode.Recording) {
            jitter_browser_size()
            //recorded_cmds.push('wait_till_page_is_ready()\n')
        }
    } else if (msg.code == 'command') {
        recorded_cmds.push(msg.command)
    } else if (msg.code == 'jitter_browser_size') {
        // Use the webdriver to jitter the browser size.
        jitter_browser_size()
    }
}

start_extension_socket()

//------------------------------------------------------------------------------------------------
// Regular WebSocket to communicate with app.
//------------------------------------------------------------------------------------------------

var app_socket_server = new WebSocketServer({port: 8082})
var app_socket = null

app_socket_server.on('connection', function connection(ws_arg) {
    app_socket = ws_arg
    log_info("browsercontroller now connected to native app");
    app_socket.on('message', on_message_from_app);
}); 

var on_message_from_app = function (message) {
    try {
        log_info('controller received: ' + message);
        
        // unpack the message.
        var pos = message.indexOf(':')
        var code = message.substr(0,pos)
        var body = message.substr(pos+1)
        
        if (code == 'open_browser') {
            open_browser()
            app_socket.send('ok:')
        } else if (code == 'close_browser') {
            close_browser()
            app_socket.send('ok:')
        } else if (code == 'start_recording') {
            if (!extension_socket) {
                app_socket.send('fail:')
                return
            }
            recorded_cmds = []
            extension_socket.send(JSON.stringify({code: 'start_recording'}))
            console.log("starting to record")
            mode = ActionMode.Recording
            app_socket.send('ok:')
        } else if (code == 'stop_recording') {
            extension_socket.send(JSON.stringify({code: 'stop_recording'}))
            mode = ActionMode.Idle
            recorded_cmds.push("send_result()\n")
            app_socket.send('finished_recording:'+JSON.stringify(recorded_cmds))
            close_browser()
        } else if (code == 'get_recording') {
            app_socket.send('recording:'+JSON.stringify(recorded_cmds))
        } else if (code == 'replay_recording') {
            try{
                console.log("replaying recording: " + body)
                mode = ActionMode.Replaying
                replay_cmds = JSON.parse(body)
                open_browser()
                process_replay()
                return
            } catch(e) {
                log_exception(e)
            }
        } else if (code == 'goto_url') {
            goto_url(body).then(function(){
                if (mode == ActionMode.Recording) {
                    recorded_cmds.push("goto_url('" + body + "')")
                }
                app_socket.send('ok:')
            }, function(error) {
                app_socket.send('fail:')
            })
        } else if (code == 'back') {
            navigate_back().then(function(){
                if (mode == ActionMode.Recording) {
                    recorded_cmds.push("navigate_back()")
                }
                app_socket.send('ok:')
            }, function(error) {
                app_socket.send('fail:')
            })
        } else if (code == 'forward') {
            navigate_forward().then(function(){
                if (mode == ActionMode.Recording) {
                    recorded_cmds.push("navigate_forward()")
                }
                app_socket.send('ok:')
            }, function(error) {
                app_socket.send('fail:')
            })
        }
    } catch (e) {
        log_exception(e)
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

var trivial_wait_time = 60000 //1000
var critical_wait_time = 60000 //30000

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

//Waits for a page to be ready.
var page_is_ready = false
var wait_for_page_ready = function() {
    page_ready = false
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
        element.sendKeys(key)
    })
    terminate_chain(p)
}

// Creates promise chain which will set text on an element.
var set_text = function(xpath, text) {
    p = get_visible_element(xpath, critical_wait_time).then(function(element) {
        element.sendKeys(Key.HOME,Key.chord(Key.SHIFT,Key.END),text)
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
var mouse_over_element = function (xpath, relX, relY) {
    p = get_visible_element(xpath, trivial_wait_time).then(function(element) {
        return driver.actions().mouseMove(element, {x: relX , y: relY}).perform().then(
                function() {},
                function(error){log_info('Warning: could not move_over_element (computedstyle): ' + xpath);throw(error)}
        )
    })
    terminate_chain(p)
}

// Creates a promise which will send results back to the native app.
var send_result = function () {
    p = flow.execute( function() {
        var result = {test: 'replace this with real value'} // This should be changed to the real result when ready.
        var d = webdriver.promise.defer();
        app_socket.send('ok: ' + JSON.stringify(result))
        d.fulfill(result)
        return d.promise
    })
    terminate_chain(p)
}

// Helper to terminate promise chains.
var terminate_chain = function(p) {
    p.catch(function(error){
        log_error("Error in chain!")
        if (error.stack.indexOf('mouse_over_element') >=0) {
            log_error('error from mouse_over_element')
        } else if (error.stack.indexOf('click_on_element') >=0) {
            log_error('error from click_on_element')
        } else if (error.stack.indexOf('send_key') >=0) {
            log_error('error from send_key')
        } else if (error.stack.indexOf('set_text') >=0) {
            log_error('error from set_text')
        } else {
            log_exception(error)
        }
    })
}
