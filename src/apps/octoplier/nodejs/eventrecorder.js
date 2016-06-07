// This file runs in the browser.

// Our namespace.
window.octoplier = {}

// The Logger.
window.octoplier.Logger = function() {
	var errors = []

	var get_errors = function() {
		var temp = errors.slice()
		errors.length = 0
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
window.octoplier.EventStack = function() {
	var events = []
	
	var get_events = function() {
		var temp = events.slice()
		events.length = 0
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
	
window.octoplier.EventProxy = function (window, document, logger, event_stack) {
	// Copied from EventProxy.js in theintern/recorder github project,
	// and slightly modified. References to port or chrome objects have
	// been removed.
	var EVENT_TYPES = 'click dblclick mousedown mouseup keydown keypress keyup submit input'.split(' '); 
	// mousemove
	// mouseover mouseout
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
			
			this.window.addEventListener('message', passEventBound, true, true);
			this.window.addEventListener('beforeunload',delayEventBound, true, true);
			this.window.addEventListener('scroll',sendEventBound, true, true);
			this.window.addEventListener('submit',sendEventBound, true, true);
			EVENT_TYPES.forEach(function (eventType) {
				self.document.addEventListener(eventType, sendEventBound, true, true);
			});
		},
		
		disconnect: function () {
			if (!this.is_connected()) {
				return
			}
			
			EVENT_TYPES.forEach(function (eventType) {
				self.document.removeEventListener(eventType, sendEventBound, true);
			});
			this.window.removeEventListener('submit',sendEventBound, true);
			this.window.removeEventListener('scroll',sendEventBound, true);
			this.window.removeEventListener('beforeunload', delayEventBound, true);
			this.window.removeEventListener('message', passEventBound, true);
			
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
//			if (event.type === 'beforeunload') {
//				this.logger.log_error("about to load a new page")
//				var confirmationMessage = "continue to next page";
//				event.returnValue = confirmationMessage; // Gecko, Trident, Chrome 34+
//				return confirmationMessage;              // Gecko, WebKit, Chrome <34
//			}
		},
		
		sendEvent: function (event) {
			
			window.octoplier.logger.log_error("got send event: " + event.type)
			
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
					elements: [this.document.elementFromPoint(event.clientX, event.clientY)]
				};
			}

			// When a user drags an element that moves with the mouse, the element will not be dragged in the recorded
			// output unless the final position of the mouse is recorded relative to an element that did not move
			if (event.type === 'mouseup') {
				target = (function () {
					// The nearest element to the target that was not also the nearest element to the source is
					// very likely to be an element that did not move along with the drag
					var sourceElements = lastMouseDown[event.button].elements;
					var targetElements = [this.document.elementFromPoint(event.clientX, event.clientY)];
					for (var i = 0; i < sourceElements.length; ++i) {
						if (sourceElements[i] !== targetElements[i]) {
							return targetElements[i];
						}
					}

					// TODO: Using document.body instead of document.documentElement because of
					// https://code.google.com/p/chromedriver/issues/detail?id=1049
					return this.document.body;
				}).call(this);
			} else {
				target = event.target;
			}

			
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
			
			if (event.type == 'input') {
				data.text = target.value
			}
			
			if (event.type === 'scroll') {
				data.scrollX = window.scrollX
				data.scrollY = window.scrollY
			}
						
			this.send(data);
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
		window.octoplier.event_stack = window.octoplier.EventStack()
		window.octoplier.event_proxy = window.octoplier.EventProxy(window, window.document, window.octoplier.logger, window.octoplier.event_stack)
		window.octoplier.event_proxy.setStrategy('xpath');
		//window.octoplier.event_proxy.connect()
		//alert("created new octoplier instance")
	}
} catch (e) {
	window.octoplier.logger.log_exception(e);
}

return true


