//------------------------------------------------------------------------------------------------
//Page readiness and mutations.
//------------------------------------------------------------------------------------------------

//If there are no dom mutations for 5 seconds, we consider the page to be loaded 
//and ready to be shown and interacted with for recording and playback.
//If there are subsequent dom changes after this, we expect the various browser controller
//actions to wait for the elements to appear.
var mutation_check_interval = 200 // time interval to check whether we've waited long enough
var mutation_done_interval = 5000 // min time since last mutation, to be considered fully completed and done
var last_mutation_time = null
var mutation_timer = null
var page_is_ready = false //Whether the page has seen no dom mutations for 5 seconds.

var start_mutation_timer = function() {
    if (mutation_timer == null) {
        // start checking to make things visible
        console.log("starting mutation observer")
        page_is_ready = false
        last_mutation_time = new Date();
        mutation_timer = setInterval(check_loading, mutation_check_interval)
    }
}

var stop_mutation_timer = function() {
    clearInterval(mutation_timer)
    mutation_timer = null
}

//Start listening for mutations on page load.
document.addEventListener ('DOMContentLoaded', on_loaded, false);
function on_loaded () {
    console.log("starting mutation observer!")
    start_mutation_timer()
}

function show_page() {
    // This is continuously called because some pages like google search will
    // change the dom inplace, and the body will pick up the hidepage.css settings.
//    console.log("making page visible!")
//    if (window.document.body) {
//        //window.document.body.style.setProperty("display", "inherit", "important");
//        window.document.body.style.setProperty("pointer-events", "inherit", "important");
//        window.document.body.style.setProperty("visibility", "inherit", "important");
//        //chrome.runtime.sendMessage({code: 'jitter_browser_size'})
//    }
}

function hide_page() {
    console.log("making page invisible!")
    if (window.document.body) {
        //window.document.body.style.setProperty("display", "none", "important");
        window.document.body.style.setProperty("pointer-events", "none", "important");
        window.document.body.style.setProperty("visibility", "hidden", "important");
        //chrome.runtime.sendMessage({code: 'jitter_browser_size'})
    }
}

function check_loading() {
    var current_time = new Date()
    var last_mutation_delta = current_time.getTime() - last_mutation_time.getTime()
    
    if (!recording) {
        show_page()
    } 
    
    if (last_mutation_delta > mutation_done_interval) {
        stop_mutation_timer()
        show_page()
        page_is_ready = true
        
        // Only send the page_is_ready from the top frame.
        if (this.window == this.window.top) {
            chrome.runtime.sendMessage({code: 'page_is_ready'})
        }
    }
}

//------------------------------------------------------------------------------------------------
//Our recording state.
//------------------------------------------------------------------------------------------------

//Whether we are recording.
var recording = false

//Ask the background script if we should be recording.
chrome.runtime.sendMessage({code: 'recorder_embedded'}, function(response) {
    if (response) {
        //start recording
        console.log("starting up recording")
        recording = true
        window.octoplier.event_proxy.connect()
        last_mutation_time = new Date();
    } else {
        console.log("not starting up recording")
        recording = false
    }
})

//------------------------------------------------------------------------------------------------
//Our dom events listener.
//------------------------------------------------------------------------------------------------

// Our namespace.
window.octoplier = {}

// The Logger.
window.octoplier.Logger = function() {
	var errors = []

	var log_info = function(msg) {
		chrome.runtime.sendMessage({code: "browser_info", message: msg})
	}
	
	var log_error = function(msg) {
		chrome.runtime.sendMessage({code: "browser_error", message: msg})
	}

	var log_exception = function(e) {
		chrome.runtime.sendMessage({code: "browser_exception", message: e.message + "\nstacktrace: " + e.stack})
	}

	return {
		log_info : log_info,
		log_error : log_error,
		log_exception : log_exception
	}
}
	
window.octoplier.EventProxy = function (window, document, logger) {
	// Copied from EventProxy.js in theintern/recorder github project,
	// and slightly modified. References to port or chrome objects have
	// been removed.
	var EVENT_TYPES = 'click dblclick mousedown mousemove mouseup mouseover keydown keypress keyup input'.split(' '); 
	// submit
	// mousemove
	// mouseover mouseout
	// beforeinput input change
	// selectstart selectionchange blur
	
	function EventProxy(window, document, logger) {
		this.window = window;
		this.document = document;
		this.logger = logger;
	}

	var passEventRelay = function () {
		return this.passEvent.apply(this, arguments);
	};
	
	var delayEventRelay = function () {
		return this.delayEvent.apply(this, arguments);
	};
	
	var sendEventRelay = function () {
		return this.sendEvent.apply(this, arguments);
	};
	
	var contextMenuEventRelay = function () {
	    return this.contextMenuEvent.apply(this, arguments);
	}
	
	var sendEventBound = null;
	var passEventBound = null;
	var delayEventBound = null;
	var contextMenuEventBound = null;
	
	EventProxy.prototype = {
		constructor: EventProxy,

		getTarget: null,
		
		is_connected: function () {
			if (sendEventBound === null) {
				return false
			}
			return true
		},
		
		connect: function () {
			if (this.is_connected()) {
				return
			}
			
			var self = this;
			sendEventBound = sendEventRelay.bind(self);
			passEventBound = passEventRelay.bind(self);
			delayEventBound = delayEventRelay.bind(self);
			contextMenuEventBound = contextMenuEventRelay.bind(self)
			
			this.window.addEventListener('message', passEventBound, true);
			this.window.addEventListener('beforeunload',delayEventBound, true);
			this.window.addEventListener('scroll',sendEventBound, true);
			this.window.addEventListener('submit',sendEventBound, true);
			this.document.addEventListener('contextmenu', contextMenuEventBound, true);
			EVENT_TYPES.forEach(function (eventType) {
				self.document.addEventListener(eventType, sendEventBound, true);
			});
		},
		
		disconnect: function () {
			if (!this.is_connected()) {
				return
			}
			
			EVENT_TYPES.forEach(function (eventType) {
				self.document.removeEventListener(eventType, sendEventBound, true);
			});
			this.document.removeEventListener('contextmenu', contextMenuEventBound, true);
			this.window.removeEventListener('submit',sendEventBound, true);
			this.window.removeEventListener('scroll',sendEventBound, true);
			this.window.removeEventListener('beforeunload', delayEventBound, true);
			this.window.removeEventListener('message', passEventBound, true);
			
			sendEventBound = null
			passEventBound = null
			delayEventBound = null
			contextMenuEventBound = null
		},
		
	    get_css_path: function(el) {
	        if (!(el instanceof Element)) 
	            return;
	        var path = [];
	        while (el.nodeType === Node.ELEMENT_NODE) {
	            var selector = el.nodeName.toLowerCase();
	            if (el.id) {
	                selector += '#' + el.id;
	                path.unshift(selector);
	                break;
	            } else {
	                var sib = el, nth = 1;
	                while (sib = sib.previousElementSibling) {
	                    if (sib.nodeName.toLowerCase() == selector)
	                       nth++;
	                }
	                if (nth != 1)
	                    selector += ":nth-of-type("+nth+")";
	            }
	            path.unshift(selector);
	            el = el.parentNode;
	        }
	        return path.join(" > ");
	     },

		getElementTextPath: function (element) {
			var tagPrefix = '//' + element.nodeName;

			var textValue = this.document.evaluate(
				'normalize-space(string())',
				element,
				null,
				this.window.XPathResult.STRING_TYPE,
				null
			).stringValue;

			var path = '[normalize-space(string())="' + textValue.replace(/"/g, '&quot;') + '"]';

			var matchingElements = this.document.evaluate(
				tagPrefix + path,
				this.document,
				null,
				this.window.XPathResult.UNORDERED_NODE_ITERATOR_TYPE,
				null
			);

			matchingElements.iterateNext();
			var matchesMultipleElements = Boolean(matchingElements.iterateNext());

			if (matchesMultipleElements) {
				// ignoring IDs because when the text strategy is being used it typically means that IDs are not
				// deterministic
				path = this.getElementXPath(element, true) + path;
			}
			else {
				path = tagPrefix + path;
			}

			return path;
		},

		getElementXPath: function (element, ignoreId) {
		    var path = [];

		    ignoreId = true;

		    do {
		        if (element.id && !ignoreId) {
		            path.unshift('id("' + element.id + '")');

		            // No need to continue to ascend since we found a unique root
		            break;
		        }
		        else if (element.parentNode) {
		            var nodeName = element.nodeName;
		            var hasNamedSiblings = Boolean(element.previousElementSibling || element.nextElementSibling);
		            // XPath is 1-indexed
		            var index = 1;
		            var sibling = element;

		            if (hasNamedSiblings) {
		                while ((sibling = sibling.previousElementSibling)) {
		                    if (sibling.nodeName == nodeName) {
		                        ++index;
		                    }
		                }

		                path.unshift(nodeName + '[' + index + ']');
		            }
		            else {
		                path.unshift(nodeName);
		            }
		        }
		        // The root node
		        else {
		            path.unshift('');
		        }
		    } while ((element = element.parentNode));

		    var full_path =  path.join('/');
		    if (full_path == "") {
		        return "/"
		    }
		    return full_path
		},
		
		contextMenuEvent: function (event) {
            if (recording && (!page_is_ready)) {
                event.stopPropagation();
                event.preventDefault();
                event.stopImmediatePropagation();
                return false;
            }
		},

		passEvent: function (event) {
		    
		    if (recording && (!page_is_ready)) {
    		    event.stopPropagation();
    		    event.preventDefault();
    		    event.stopImmediatePropagation();
    		    return false;
		    }
		    
		    if (!event.data || event.data.method !== 'recordEvent' || !event.data.detail) {
		        return;
		    }

		    var detail = event.data.detail;

		    for (var i = 0; i < this.window.frames.length; ++i) {
		        if (event.source === this.window.frames[i]) {
		            detail.targetFrame.unshift(i);
		            break;
		        }
		    }

		    this.send(detail);
		},
		
		delayEvent: function (event) {
		    //window.octoplier.event_proxy.disconnect()
		    page_is_ready = false
		    
//			if (event.type === 'beforeunload') {
//				this.logger.log_error("about to load a new page")
//				var confirmationMessage = "continue to next page";
//				event.returnValue = confirmationMessage; // Gecko, Trident, Chrome 34+
//				return confirmationMessage;              // Gecko, WebKit, Chrome <34
//			}
		},
		
		sendEvent: function (event) {
		    
            if (recording && (!page_is_ready)) {
                event.stopPropagation();
                event.preventDefault();
                event.stopImmediatePropagation();
                return false;
            }
		    
			//window.octoplier.logger.log_info("got send event: " + event.type)
			var target = event.target
			var rect = target.getBoundingClientRect ? target.getBoundingClientRect() : {};
			var data = {
					altKey: event.altKey,
					button: event.button,
					buttons: event.buttons,
					ctrlKey: event.ctrlKey,
					clientX: event.clientX,
					clientY: event.clientY,
					elementX: event.clientX - rect.left,
					elementY: event.clientY - rect.top,
					keyIdentifier: event.keyIdentifier,
					key: event.key,
					code: event.code,
					charCode: event.charCode,
					keyCode: event.keyCode,
					location: event.location,
					metaKey: event.metaKey,
					shiftKey: event.shiftKey,
					target: this.getTarget(target),
					targetFrame: [],
					type: event.type
				} 
			
			console.log("event target: " + data.target)
			
			if ((event.type == 'input') || (event.type == 'keydown')) {
				data.text = target.value
			}
			
			if (event.type === 'scroll') {
				data.scrollX = window.scrollX
				data.scrollY = window.scrollY
			}
						
			this.send(data);
		},
		
		send: function (detail) {
		    if (this.window !== this.window.top) {
		        this.window.parent.postMessage({
		            method: 'recordEvent',
		            detail: detail
		        }, '*');
		    } else {
		        var event = detail
		        var type = event.type
		        var cmd = ""
		            if (type == 'goto_url') {
		                cmd = "goto_url('" + event.url + "')\n"
		            } else if (type == 'click') {
		                cmd = "click_on_element('" + event.target +"')\n"
		            } else if ((type == 'keydown')) {
		                // Handling enter as a special case.
		                if (event.keyIdentifier == 'Enter') {
		                    cmd = "send_key('" + event.target + "', Key.ENTER)\n"
		                }
		            } else if (type == 'input') {
		                // On input we overwrite the entire field.
		                cmd = "set_text('" + event.target + "','" + event.text + "')\n"
		            } else if (type == 'mousemove') {
		                //cmd = "mouse_over_element('" + event.target + "', " + event.elementX + ", " + event.elementY + ")\n"
		            } else if (type == 'scroll') {
		                cmd = "driver.executeScript('window.scrollTo("+ event.scrollX +","+ event.scrollY +")')\n"
		            } else {
		            }
		        // If the dom event translates to a driver command
		        if (page_is_ready && cmd.length) {
		            console.log("sending command: " + cmd)
		            chrome.runtime.sendMessage({code: "command", command: cmd});
		        } else {
		            console.log("not sending event: " + JSON.stringify(event))
		        }
		    }
		},
		
		setStrategy: function (value) {
			switch (value) {
			    
			    case 'css':
			        this.getTarget = this.get_css_path;
			        break;
				case 'xpath':
					this.getTarget = this.getElementXPath;
					break;
				case 'text':
					this.getTarget = this.getElementTextPath;
					break;
				default:
					throw new Error('Invalid strategy "' + value + '"');
			}
		}
	};
	
	return new EventProxy(window, document, logger)
}
	

// Catch any uncaught exceptions at the top level window.
window.onerror = function (message, url, line_number, column_number, error) {
    window.octoplier.logger.log_error('Error: caught at Window: ' + message + 
    		' Script: ' + url + 
    		' Line: ' + line_number + 
    		' Column: ' + column_number + 
    		' Error: ' + error);
}



try {
	// Octoplier initialization.
	if (typeof window.octoplier.logger == 'undefined') {
		window.octoplier.logger = window.octoplier.Logger()
		window.octoplier.event_proxy = window.octoplier.EventProxy(window, window.document, window.octoplier.logger)
		window.octoplier.event_proxy.setStrategy('xpath');
				
		//setup our callback function to loop through each mutation and print the number of nodes
		//that have been added and removed
		var fnCallback = function (mutations) {
			mutations.forEach(function (mutation) {
				//chrome.runtime.sendMessage({code: "dom_changed"});
			    console.log("mutation observed!")
				last_mutation_time = new Date();
			    //start_mutation_timer()
			    //if (mutation_timer == null) {
			    //    if (!recording) {
			    //        show_page()
			    //    } 
			    //}
			    // start_mutation_timer()
		        // mutation.addedNodes.length
		        // mutation.removedNodes.length
		    });
		};
		
		//now create our observer and get our target element
		window.octoplier.observer = new MutationObserver(fnCallback),
		        objConfig = {
		            childList: true,
		            subtree : true,
		            attributes: false,
		            characterData : false
		        };
		
		//then actually do some observing
		window.octoplier.observer.observe(window.document, objConfig);
		
	}
} catch (e) {
	window.octoplier.logger.log_exception(e);
}

chrome.runtime.onMessage.addListener(
	function(request, sender, sendResponse) {
		console.log('got chrome message: ' + JSON.stringify(request))
		if (request.code == "start_recording") {
			window.octoplier.event_proxy.connect()
			last_mutation_time = new Date();
		} else if (request.code == "stop_recording") {
			window.octoplier.event_proxy.disconnect()
			page_is_ready = false
			if (window.document.body) {
			    window.document.body.style.setProperty("display", "inherit", "important");
			}
		} else if (request.code == "page_is_ready") {
		    sendResponse(page_is_ready)
		}
	}
);




