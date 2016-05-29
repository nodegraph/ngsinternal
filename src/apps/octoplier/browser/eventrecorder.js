// This file runs in the browser.

// Our namespace.
var octoplier = {}

// The Logger.
octoplier.Logger = function() {
	var errors = []

	var get_errors = function() {
		var temp = errors
		errors = []
		return temp
	}

	var log_error = function(message) {
		errors.push(message)
	}

	var log_exception = function(e) {
		log_error("exception logged: " + e.message + "\nstacktrace: " + e.stack)
	}

	return {
		get_errors : get_errors,
		log_error : log_error,
		log_exception : log_exception
	}
}
	
// Event Stack.
octoplier.EventStack = function() {
	var events = []
	
	var get_events = function() {
		var temp = events
		events = []
		return temp
	}
	
	var push_event = function(event) {
		events.push(event)
	}
	
	return {
		get_events: get_events,
		push_event: push_event
	}
}
	
octoplier.EventProxy = function (window, document, logger, event_stack) {
	// Copied from EventProxy.js in theintern/recorder github project,
	// and slightly modified. References to port or chrome objects have
	// been removed.
	var EVENT_TYPES = 'click dblclick mousedown mouseup keydown keypress keyup submit input'.split(' '); 
	
	// mousemove
	
	// mouseover mouseout
	// mousemove
	// beforeinput input change
	// selectstart selectionchange blur
	
	function EventProxy(window, document, logger, event_stack) {
		this.window = window;
		this.document = document;
		this.lastMouseDown = {};
		this.logger = logger;
		this.event_stack = event_stack;
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
	
	var sendEventBound = null;
	var passEventBound = null;
	var delayEventBound = null;
	
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
			
			this.window.addEventListener('message', passEventBound, false);
			this.window.addEventListener('beforeunload',delayEventBound, false);
			this.window.addEventListener('scroll',sendEventBound, false);
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
			this.window.removeEventListener('scroll',sendEventBound, false);
			self.window.removeEventListener('beforeunload', delayEventBound, false);
			self.window.removeEventListener('message', passEventBound, false);
			
			sendEventBound = null
			passEventBound = null
			delayEventBound = null
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
							if (sibling.nodeName === nodeName) {
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

		passEvent: function (event) {
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

		send: function (detail) {
			if (this.window !== this.window.top) {
				this.window.parent.postMessage({
					method: 'recordEvent',
					detail: detail
				}, '*');
			}
			else {
				this.event_stack.push_event(detail)
			}
		},
		
		delayEvent: function (event) {
			if (event.type === 'beforeunload') {
				console.log("about to load a new page")
				var confirmationMessage = "continue to next page";
				event.returnValue = confirmationMessage; // Gecko, Trident, Chrome 34+
				return confirmationMessage;              // Gecko, WebKit, Chrome <34
			}
		},
		
		sendEvent: function (event) {
			
			var lastMouseDown = this.lastMouseDown;
			var target;

			function isDragEvent() {
				return Math.abs(event.clientX - lastMouseDown[event.button].event.clientX) > 5 ||
					Math.abs(event.clientY - lastMouseDown[event.button].event.clientY > 5);
			}

			if (event.type === 'click' && isDragEvent()) {
				return;
			}

			if (event.type === 'mousedown') {
				lastMouseDown[event.button] = {
					event: event,
					elements: this.document.elementsFromPoint(event.clientX, event.clientY)
				};
			}

			// When a user drags an element that moves with the mouse, the element will not be dragged in the recorded
			// output unless the final position of the mouse is recorded relative to an element that did not move
			if (event.type === 'mouseup') {
				target = (function () {
					// The nearest element to the target that was not also the nearest element to the source is
					// very likely to be an element that did not move along with the drag
					var sourceElements = lastMouseDown[event.button].elements;
					var targetElements = this.document.elementsFromPoint(event.clientX, event.clientY);
					for (var i = 0; i < sourceElements.length; ++i) {
						if (sourceElements[i] !== targetElements[i]) {
							return targetElements[i];
						}
					}

					// TODO: Using document.body instead of document.documentElement because of
					// https://code.google.com/p/chromedriver/issues/detail?id=1049
					return this.document.body;
				}).call(this);
			}
			else {
				target = event.target;
			}

			
			var rect = target.getBoundingClientRect ? target.getBoundingClientRect() : {};

			this.send({
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
				type: event.type,
				text: target.value,      // input
				scrollX: window.scrollX, // scroll
				scrollY: window.scrollY  // scroll
			});
		},
		
		replay_event: function (event) {
			try {
				// Make sure document is fully loaded.
				if (document.readyState != 'complete') {
					this.logger.log_error("document not loaded yet");
					return false
				}
					
				// Find the target element.
				var element = document.evaluate(event.target, document, null, XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
				if (!element) {
					if ((event.type == "mousemove") ||
						(event.type == "mouseover") ||
						(event.type == "mouseout")) {
						this.logger.log_error("Info: mousemove/over/out could not find element to dispatch to: " + event.target);
						return true
					}
					this.logger.log_error("Error: mouse event could not find element to dispatch to: " + event.target);
					return false
				}
				
	            switch(event.type) {
		            // MouseEvent types: onclick, ondblclick, onmousedown, onmouseup, onmouseover, onmousemove, and onmouseout
			        case "click":
			        case "dblclick":
			        case "mousedown":
			        case "mouseup":
			        case "mousemove":
			        case "mouseover":
			        case "mouseout": {
			        	if (event.type == "click") {
			        		element.click()
			        	} else {
			        		var fake = new MouseEvent(event.type, event);
			        		element.dispatchEvent(fake);
			        	}
			        	break;
			        }
			        case "selectstart":
			        case "selectionchange":
			        case "keydown":
		            case "keyup":
		            case "keypress": {
		            	if (event.keyCode == 13) {
			            	var fake = new KeyboardEvent(event.type, event);
		            	
			                // Chromium Hack
			                Object.defineProperty(fake, 'keyCode', {
			                                          get : function() {
			                                              return this.keyCodeVal;
			                                          }
			                                      });
			                Object.defineProperty(fake, 'charCode', {
			                                          get : function() {
			                                              return this.charCodeVal;
			                                          }
			                                      });
			                Object.defineProperty(fake, 'which', {
			                                          get : function() {
			                                              return this.whichVal;
			                                          }
			                                      });
			                Object.defineProperty(fake, 'char', {
			                                          get : function() {
			                                              return this.charVal;
			                                          }
			                                      });
			                Object.defineProperty(fake, 'key', {
			                                          get : function() {
			                                              return this.keyVal;
			                                          }
			                                      });
			                fake.keyCodeVal = event.keyCode;
			                fake.charCodeVal = event.charCode;
			                fake.whichVal = event.which;
			                fake.charVal = event.char;
			                fake.keyVal = event.key;
			                element.dispatchEvent(fake);
		            	}
		                break;
		            }
		            case "input": {
					    element.value = event.text
		            	break
		            }
		            case "submit": {
		            	this.logger.log_error("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS")
		            	element.submit();
		            	break;
		            }
		            case "scroll": {
		            	this.logger.log_error("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL")
		            	window.scroll(event.scrollX,event.scrollY)
		            	break;
		            }
		            default: {
		            	this.logger.log_error("unknown event was attempted to be replayed")
		            	return false;
		            	break;
		            }
	            }
			} catch (e) {
				this.logger.log_exception(e)
				return false
			}
			
			return true
		},

		setStrategy: function (value) {
			switch (value) {
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
	
	return new EventProxy(window, document, logger, event_stack)
}
	

// Catch any uncaught exceptions at the top level window.
window.onerror = function (message, url, line_number, column_number, error) {
    octoplier.logger.log_error('Error: caught at Window: ' + message + 
    		' Script: ' + url + 
    		' Line: ' + line_number + 
    		' Column: ' + column_number + 
    		' Error: ' + error);
}

try {
	// Octoplier initialization.
	if (typeof octoplier.logger == 'undefined') {
		octoplier.logger = octoplier.Logger()
		octoplier.event_stack = octoplier.EventStack()
		octoplier.event_proxy = octoplier.EventProxy(window, document, octoplier.logger, octoplier.event_stack)
		octoplier.event_proxy.setStrategy('xpath');
		//octoplier.event_proxy.connect()
		//alert("created new octoplier instance")
	}
} catch (e) {
	octoplier.logger.log_exception(e);
}

