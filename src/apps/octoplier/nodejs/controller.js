var fs = require('fs');

var dump = function(text) {
    fs.appendFileSync("./dump.txt", text)
    console.log(text)
} 
dump('starting')

process.on('uncaughtException', function(e) {
  dump('caught exception at nodejs top: lineNumber: ' + e.lineNumber + ' message: ' + e.message + "\nstacktrace: " + e.stack);
});



//scripts
var get_script = function(filename) {
    var script = fs.readFileSync(filename, "utf8")
    return script
}

// Recorded browser events.
var timer = null
var script = null

// Command stack
var commands = []

//-------------------------------------------------------
// Secure web socket to communicate with chrome extension.
//-------------------------------------------------------

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
        dump("browsercontroller now connected to extension");
        extension_socket.on('message', function (data) {
            dump('message from extension: ' + data );
            var msg = JSON.parse(data)
            if (msg.code == 'script') {
                app_socket.send("script:" + msg.commands)
            } else if (msg.code == 'page_is_ready') {
                process_commands()
            }
        });
    });
}

start_extension_socket()

var process_commands = function() {
    while(commands.length) {
        var cmd = commands.shift()
        if (cmd == "wait_till_page_is_ready()\n") {
            return
        }
        eval(cmd)
    }
}

var clear_browser_cookies = function(callback) {
    extension_socket.send(JSON.stringify({code: 'clear_browser_cookies'}))
}

//process.stdin.on('readable', function() {
//    var chunk = process.stdin.read();
//    if (chunk !== null) {
//        dump("got data: " + chunk)
//        //process.stdout.write('{"data": "' + chunk +'"}');
//    }
//});

// -------------------------------------------------------
// WebSocket to communicate with native app.
// -------------------------------------------------------

var app_socket_server = new WebSocketServer({port: 8082})
var app_socket = null

app_socket_server.on('connection', function connection(ws_arg) {
    app_socket = ws_arg
    app_socket.on('message', function incoming(message) {
        dump("browsercontroller now connected to native app");
        try {
            dump('controller received: ' + message);
            if (message.startsWith('open_browser:')) {
                open_browser()
            } else if (message.startsWith('close_browser:')) {
                close_browser()
            } else if (message.startsWith('inject_event_recoder:')) {
                inject_recorder()
            } else if (message.startsWith('start_recording:')) {
                start_recording()
            } else if (message.startsWith('stop_recording:')) {
                stop_recording()
            } else if (message.startsWith('run_driver_script:')) {
                var script = message.replace('run_driver_script:', '')
                run_in_browser(script) 
            }
        } catch (e) {
            dump('on_message->caught exception: ' + e.message + "\nstacktrace: " + e.stack)
        }
    });
}); 

// -------------------------------------------------------
// Webdriver.
// -------------------------------------------------------

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
        chromeOptions.addArguments("load-extension=D:/src/ngsinternal/src/apps/octoplier/chrome")

        driver = new webdriver.Builder()
        .forBrowser('chrome')//.forBrowser('firefox')
        .setChromeOptions(chromeOptions)
        .build();

        flow = webdriver.promise.controlFlow();

        // Set default settings.
        driver.manage().timeouts().pageLoadTimeout(60000);
        
        webdriver.promise.controlFlow().on('uncaughtException', function(e) {
            console.error('Unhandled error: ' + e);
        });
    } catch (e) {
        dump('open_browser->caught exception: ' + e.message + "\nstacktrace: " + e.stack)
    }
}

var close_browser = function () {
    driver.quit()
}

var resize_browser = function (width, height) {
    driver.manage().window().setSize(width, height);
}

var run_in_browser = function (script) {
    try{
        commands = JSON.parse(script) //script.split(/\r?\n/)
        
        if (!driver) {
            open_browser()
            clear_browser_cookies()
            process_commands()
            return
        }

        driver.getTitle().then(
        function (val) {
            clear_browser_cookies(process_commands)
        }
        , function (err) {
            dump("browser is not open: " + err)
            open_browser()
            clear_browser_cookies()
            process_commands()
        })
    } catch(e) {
        dump('run_in_browser->caught exception: ' + e.message + "\nstacktrace: " + e.stack)
    }
}


//-------------------------------------------------------
// Script Injection.
//-------------------------------------------------------


var run_browser_script = function(script) {
    return flow.execute(function() {
        var d = webdriver.promise.defer();
        driver.executeScript(script).then(
                function(result) {d.fulfill(result)},
                function(error) {dump("Error: running browser script: " + script + " error: " + error.message + ":" + error.stack);d.reject(error)}
        )
        return d.promise
    })
}

//var record_current_url = function () {
//	return flow.execute(function() {
//		var d = webdriver.promise.defer();
//		driver.getCurrentUrl().then (
//				function(result) {events.push({type: 'goto_url', url: result}); d.fulfill(result)},
//				function(error) {"Error: record curren tufl: "+ error.message + ":" + error.stack; d.fulfill(null)})
//		return d.promise
//	})
//}

var start_recording = function () {
    extension_socket.send(JSON.stringify({code: 'start_recording'}))
    app_socket.send('ok:')
}

var stop_recording = function () {
    extension_socket.send(JSON.stringify({code: 'stop_recording'}))
    app_socket.send('ok:')
}

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
	
//var wrap_script = function (script) {
//    var wrapped = "function (env) {\n"
//    wrapped += "app_socket = env.app_socket\n"
//    wrapped += "webdriver = env.webdriver\n"
//    wrapped += "driver = env.driver\n"
//    wrapped += "Key = env.Key\n"
//    wrapped += "By = env.By\n"
//    wrapped += "until = env.until\n"
//    wrapped += "flow = env.flow\n"
//    wrapped += "resize_browser = env.resize_browser\n"
//    wrapped += "dump = env.dump\n"
//    wrapped += "goto_url = env.goto_url\n"
//    wrapped += "send_key = env.send_key\n"
//    wrapped += "set_text = env.set_text\n"
//    wrapped += "click_on_element = env.click_on_element\n"
//    wrapped += "mouse_over_element = env.mouse_over_element\n"
//    wrapped += "send_result = env.send_result\n"
//    wrapped += "\n"
//    //wrapped += "resize_browser(" + width + ", " + height + ")\n"
//    wrapped += script
//    wrapped += "\n"
//    wrapped += "//return our results\n"
//    wrapped += "send_result()\n"
//    wrapped += "}\n"
//
//    return wrapped
//}


// -------------------------------------------------------
// Automation
// Atomics.
// -------------------------------------------------------

// Note the returned promise nevers errors.
// It will try to wait for the xpath
// element as long as it can.

var trivial_wait_time = 50 //1000
var critical_wait_time = 5000 //30000

//Returns a promise which navigates the browser to another url.
var goto_url = function (url) {
    return driver.get(url)
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
            dump('Warning: could not find: ' + xpath)
            throw error
        })
        return d.promise
    })
}

//Returns a promise which evaulates to a visible element.
var get_visible_element = function (xpath, wait_milli) {
    return get_element(xpath, wait_milli).then(
            function (element) {
                dump("type of element: " + typeof(element))
                return driver.wait(until.elementIsVisible(element), wait_milli)
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
        return driver.actions().mouseMove(element, {x: relX , y: relY}).perform()
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
        dump("Error in chain: " + error.message + " stack: " + error.stack)
    })
}

//-------------------------------------------------------
//Automation.
//-------------------------------------------------------

//var run_driver_script = function(script) {
//    dump("now running script")
//    try {
//        var content = "exports.run = " + wrap_script(script) + ";";
//        var options = { flag : 'w' };
//
//        var env = {}
//        env.app_socket = app_socket
//        env.webdriver = webdriver 
//        env.driver = driver 
//        env.Key = Key 
//        env.By = By 
//        env.until = until 
//        env.flow = flow 
//        env.dump = dump 
//        env.resize_browser = resize_browser
//        env.goto_url = goto_url
//        env.send_key = send_key
//        env.set_text = set_text
//        env.click_on_element = click_on_element
//        env.mouse_over_element = mouse_over_element
//        env.send_result = send_result
//
//        fs.writeFile('./evalscript.js', content, options, function (e) {
//            try{
//                if (e) {
//                    app_socket.send('fail1: ' + e.message + '  stack: ' + e.stack)        
//                }
//                delete require.cache[require.resolve('./evalscript')]
//                var debug = require('./evalscript');
//                debug.run(env);
//            }
//            catch(e2){
//                 app_socket.send('fail2: ' + e2.message + '  stack: ' + e2.stack);
//            }
//        });
//    } catch(e) {
//        app_socket.send('fail3: ' + e.message + 'stack: ' + e.stack)
//    }
//}

