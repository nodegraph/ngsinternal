var fs = require('fs');
var dump = function(text) {
    fs.appendFileSync("./dump.txt", text)
    console.log(text)
    } 
dump('starting')

process.on('uncaughtException', function(e) {
  console.log('caught exception at nodejs top: lineNumber: ' + e.lineNumber + ' message: ' + e.message + "\nstacktrace: " + e.stack);
});

// scripts
var get_script = function(filename) {
	var script = fs.readFileSync(filename, "utf8")
	return script
}
var event_recorder_script =  get_script('./eventrecorder.js')
var get_errors_script =  get_script('./geterrors.js')
var get_events_script =  get_script('./getevents.js')
var start_recording_script =  get_script('./startrecording.js')
var stop_recording_script =  get_script('./stoprecording.js')

// Recorded browser events.
var timer = null
var events = []

// -------------------------------------------------------
// WebSocket.
// -------------------------------------------------------

var WebSocketServer = require('ws').Server

var wss = new WebSocketServer({port: 8082})
var ws = null

wss.on('connection', function connection(ws_arg) {
  ws = ws_arg
  ws.on('message', function incoming(message) {
    try {
        dump('controller received: ' + message);
        if (message.startsWith('open_browser:')) {
        	open_browser()
        } else if (message.startsWith('close_browser:')) {
        	close_browser()
        } else if (message.startsWith('inject_event_recoder:')) {
        	inject_recorder()
        } else if (message.startsWith('get_events:')) {
        	get_events()
        } else if (message.startsWith('get_errors:')) {
        	get_errors()
        } else if (message.startsWith('start_recording:')) {
        	start_recording()
        } else if (message.startsWith('stop_recording:')) {
        	stop_recording()
        } else if (message.startsWith('get_e')) {
        	
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

        //var chromeOptions = new chrome.Options()
        //Win_x64-389148-chrome-win32
        //Win-338428-chrome-win32
        //chromeOptions.setChromeBinaryPath('/downloaded_software/chromium/Win_x64-389148-chrome-win32/chrome-win32/chrome.exe')

        driver = new webdriver.Builder()
        .forBrowser('firefox')//.forBrowser('chrome')//.forBrowser('firefox')
        //.setChromeOptions(chromeOptions)
        .build();

        flow = webdriver.promise.controlFlow();

        // Set default settings.
        driver.manage().timeouts().pageLoadTimeout(60000);
        
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
        if (!driver) {
            open_browser()
            run_driver_script(script)
            return
        }

        driver.getTitle().then(
        function (val) {
            run_driver_script(script)
        }
        , function (err) {
            dump("browser is not open: " + err)
            open_browser()
            run_driver_script(script)
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
        		function(error) {dump("Error: running browser script: "+ error.message + ":" + error.stack);d.reject(error)}
        )
        return d.promise
    })
}

var inject_event_recorder = function() {
    var script = 
    	`var result = true
	        if (typeof(octoplier) === "undefined") {
	            result = false
	        }
    	return result`
    return flow.execute(function() {
    	var d = webdriver.promise.defer();
    	console.log('111111111111111111')
	    run_browser_script(script).then(
	    		function(result) {
	    			if (!result) {
	    				console.log('222222222222222')
	    				run_browser_script(event_recorder_script).then(
	    						function(result){d.fulfill(result)},
	    						function(error){dump("Error: event recorder script: "+ error.message + ":" + error.stack);d.fulfill(null)})
	    			} else {
	    				d.fulfill(true)
		    		}
	    		},
	    		function(error) {dump("Error: check recorder script: "+ error.message + ":" + error.stack);d.fulfill(null)}
	    )
	    return d.promise
    })
}

var get_events = function () {
	console.log('333333333333333333')
	run_browser_script(get_events_script).then(
		function(result) { 
            if (result && result.length) {
                for (var i=0; i<result.length; i++) {
                    events.push(result[i])
                    dump("storing event: " + JSON.stringify(result[i]))
                }
            } else {
            	dump("get_events_weird_result: " + JSON.stringify(result))
            }
		},
		function (error) {
			dump("Error: get_events script: "+ error.message + ":" + error.stack)
		}
	)
}

var get_errors = function () {
	console.log('4444444444444444444')
	run_browser_script(get_errors_script).then(
		function(result) {
            if (result != "[]") {
                dump("browser script errors:" + result)
            }
		},
		function(error) {
			dump("Error: get_errors script: "+ error.message + ":" + error.stack)
		}
	)
}

var record_current_url = function () {
	return flow.execute(function() {
		var d = webdriver.promise.defer();
		driver.getCurrentUrl().then (
				function(result) {events.push({type: 'goto_url', url: result}); d.fulfill(result)},
				function(error) {"Error: record curren tufl: "+ error.message + ":" + error.stack; d.fulfill(null)})
		return d.promise
	})
}

var start_recording = function () {
	events = []
	open_browser()
	record_current_url()
	recording = true
	
	timer = setInterval(function() {
	        	inject_event_recorder()
	        	if (recording) {
	        		run_browser_script(start_recording_script)
	        	} else {
	        		run_browser_script(stop_recording_script)
	        	}
	        	get_events()
	        	get_errors()
	        }, 1000)
	console.log('starting timer: ' + timer)
	console.log('5555555555555555')
}

var stop_recording = function () {
	recording = false
//	console.log('stopping timer: ' + timer)
//	clearInterval(timer)
//	console.log('6666666666666666')
//	run_browser_script(stop_recording_script)
}



// -------------------------------------------------------
// Automation Atomics.
// -------------------------------------------------------

// Note the returned promise nevers errors.
// It will try to wait for the xpath element as long as it can.
var wait_time = 30000

var wait_for_element = function (xpath) {
    var element = driver.wait(until.elementLocated(By.xpath(xpath)), wait_time)  
}

var wait_for_element = function (xpath) {
    flow.execute(function() {
        var d = webdriver.promise.defer();
        driver.wait(function (xpath){return driver.findElement(By.xpath(xpath)).isDisplayed()}, wait_time)
            .then(function(element){d.fulfill(true)},
                  function(err){dump('Error: couldnt find element for ' + xpath); d.fulfill(false)})
        return d.promise
    })
}

var click_on_element = function (xpath) {
    flow.execute(function () {
        var d = webdriver.promise.defer();
        var element = driver.wait(until.elementLocated(By.xpath(xpath)), wait_time)  
        element.then(
            function(ele){ 
                driver.actions().click(element).perform()
                d.fulfill(null)
            },
            function(err){dump('Error: click on elements not found: ' +xpath); d.fulfill(null)})
        return d.promise
    })
}

//var click_on_element = function (xpath) {
//    flow.execute(function () {
//        var d = webdriver.promise.defer();
//        wait_for_element().then(
//            function(found){ 
//                if (found) { 
//                    driver.findElement(By.xpath(xpath)).then(
//                        function(ele){driver.actions().click(elements[0]).perform(); d.fulfill(null)},
//                        function(ele){d.fulfill(null)})
//                } else {
//                    dump('Error: click on elements not found1: ' +xpath)
//                    d.fulfill(null)
//                }
//            },
//            function(err){dump('Error: click on elements not found2: ' +xpath); d.fulfill(null)})
//        return d.promise
//    })
//}

//var click_on_element = function (xpath) {
//    flow.execute(function () {
//        var d = webdriver.promise.defer();
//        driver.findElements(By.xpath(xpath)).then(
//            function(elements){ 
//                if (elements.length) { 
//                    driver.actions().click(elements[0]).perform()
//                }
//                d.fulfill(null)
//            },
//            function(err){dump('Error: click on elements not found: ' +xpath); d.fulfill(null)})
//        return d.promise
//    })
//}

var mouse_over_element = function (xpath, relX, relY) {
    flow.execute(function () {
        var d = webdriver.promise.defer();
        var element = driver.wait(until.elementLocated(By.xpath(xpath)), 50)  
        element.then(
            function(element){ 
                driver.actions().mouseMove(element, {x: relX , y: relY}).perform().then(
                        function() {d.fulfill(null)}, function(err) {dump('Error: unable to mouse over element: ' +xpath + " : " + err.message + " : "  + err.stack); d.fulfill(null)})
            },
            function(err){dump('Error: mouse over elements not found: ' +xpath); d.fulfill(null)})
        return d.promise
    })
}

//var mouse_over_element = function (xpath, relX, relY) {
//    flow.execute(function () {
//        var d = webdriver.promise.defer();
//        driver.findElements(By.xpath(xpath)).then(
//            function(elements){ 
//                if (elements.length) { 
//                    // driver.findElement(By.xpath(xpath))
//                    driver.actions().mouseMove(elements[0], {x: relX , y: relY}).perform().then(
//                        function() {d.fulfill(null)}, function(err) {dump('Error: unable to mouse over element: ' +xpath + " : " + err.message + " : "  + err.stack); d.fulfill(null)})
//                } else {
//                    dump('Error: mouse over elements not found: ' +xpath)
//                    d.fulfill(null)
//                }
//            },
//            function(err){dump('Error: mouse over elements not found: ' +xpath); d.fulfill(null)})
//        return d.promise
//    })
//}

var send_result = function () {
    flow.execute( function() {
        var result = {test: 'replace this with real value'} // This should be changed to the real result when ready.
        var d = webdriver.promise.defer();
        ws.send('ok: ' + JSON.stringify(result))
        d.fulfill(result)
        return d.promise
    })
}

// -------------------------------------------------------
// Automation.
// -------------------------------------------------------

var run_driver_script = function(script) {
    dump("now running script")
    //try {
    //    eval(script)
    //} catch (e) {
    //    ws.send('fail: lineNumber: ' + e.lineNumber + ' message: '+ e.message + "\nstacktrace: " + e.stack)
    //} 

    try {
        var content = "exports.run = "+script+";";
        var options = { flag : 'w' };

        var env = {}
        env.ws = ws
        env.webdriver = webdriver 
        env.driver = driver 
        env.Key = Key 
        env.By = By 
        env.until = until 
        env.flow = flow 
        env.dump = dump 
        env.resize_browser = resize_browser
        env.wait_for_element = wait_for_element 
        env.click_on_element = click_on_element
        env.mouse_over_element = mouse_over_element
        env.send_result = send_result

        fs.writeFile('./evalscript.js', content, options, function (e) {
            try{
                if (e) {
                    ws.send('fail1: ' + e.message + '  stack: ' + e.stack)        
                }
                delete require.cache[require.resolve('./evalscript')]
                var debug = require('./evalscript');
                debug.run(env);
            }
            catch(e2){
                 ws.send('fail2: ' + e2.message + '  stack: ' + e2.stack);
            }
        });
        //process.binding('evals').Script.runInThisContext(script)
        //eval(script)
    } catch(e) {
        ws.send('fail3: ' + e.message + 'stack: ' + e.stack)
    }
}


var test = function () {
}

