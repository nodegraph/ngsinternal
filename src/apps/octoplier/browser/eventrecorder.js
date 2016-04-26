// ---------------------------------------------------------------------------------
// This is file is run in the browser.
// ---------------------------------------------------------------------------------

try {

    // Catch any uncaught exceptions at the top level window.
    window.onerror = function (errorMsg, url, lineNumber) {
        OCTOPLIER_IS_AWESOME.g_debugger.log_error('Error caught at Window: ' + errorMsg + ' Script: ' + url + ' Line: ' + lineNumber);
    }

    // The OCTOPLIER namespace.
    // OCTOPLIER_IS_AWESOME is assumed to be unique in the browser for all
    // possible web pages we may visit. Of course it's impossible to guarantee
    // this, but we can minimize the chances of conflict by using a long name.
    var OCTOPLIER_IS_AWESOME = {};

    // --------------------------------------------------------------------
    // Error logging.
    // --------------------------------------------------------------------

    OCTOPLIER_IS_AWESOME.Debugger = function() {
            this.errors = [];
    }

    OCTOPLIER_IS_AWESOME.Debugger.prototype = new Object();

    OCTOPLIER_IS_AWESOME.Debugger.prototype.get_errors = function() {
        var temp = this.errors;
        this.errors = [];
        return temp;
    }

    OCTOPLIER_IS_AWESOME.Debugger.prototype.log_error = function(message) {
        this.errors.push(message);
    }

    OCTOPLIER_IS_AWESOME.Debugger.prototype.log_exception = function(e) {
        this.log_error("exeption: " + e.message + "\nstacktrace: " + e.stack);
    }

    // --------------------------------------------------------------------
    // Http requests.
    // --------------------------------------------------------------------

    // Retrieves the contents of a url.
    // The url can be a qrc resource file.
    OCTOPLIER_IS_AWESOME.get_url_contents = function(url,urlContentsCallback) {
        var request = new XMLHttpRequest();
        request.open('GET', url);
        request.onreadystatechange = function(event) {
            if (request.readyState == XMLHttpRequest.DONE) {
                urlContentsCallback(request.responseText);
            }
        }
        request.send();
    },

    // Retrieves the contents of a url and runs it in a web view.
    OCTOPLIER_IS_AWESOME.eval_java_script = function(web_view,url,evalResultCallback) {
        this.get_url_contents(url, function(urlContents){web_view.runJavaScript(urlContents, evalResultCallback);});
    },

    // --------------------------------------------------------------------
    // Get favicons.
    // --------------------------------------------------------------------
//    OCTOPLIER_IS_AWESOME.get_favicon = function(contentsCallback){
//        try {
//            queryURL = "http://www.google.com/s2/favicons?domain="+window.location.href;
//            OCTOPLIER_IS_AWESOME.g_debugger.log_error("url: "+window.location.href);
//            OCTOPLIER_IS_AWESOME.get_url_contents(queryURL, contentsCallback);
//        } catch (e) {
//            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
//            return "booya";
//        }
//    },

    OCTOPLIER_IS_AWESOME.get_favicon = function(){
        var favicon = undefined;
        var nodeList = document.getElementsByTagName("link");
        for (var i = 0; i < nodeList.length; i++)
        {
            if((nodeList[i].getAttribute("rel") == "icon")||(nodeList[i].getAttribute("rel") == "shortcut icon"))
            {
                favicon = nodeList[i].getAttribute("href");
            }
        }
        if (favicon === undefined) {
            favicon = window.location.origin + '/favicon.ico';
        }
        return favicon;
    },

    // --------------------------------------------------------------------
    // Stringification.
    // --------------------------------------------------------------------

    OCTOPLIER_IS_AWESOME.help_dump_object = function(obj, visited, prefix, depth, maxDepth) {
        // Check for cycles.
        if (visited.indexOf(obj) != -1) {
            return "CYCLE_DETECTED;"
        }
        visited.push(obj);

        // Increase the indentation.
        prefix += "    ";

        // Initialize the dump;
        var dump = "\n" + prefix + "{\n";

        // Dump each property.
        for (var property in obj) {
            try {
                dump += prefix + property + ':';

                var propValue = obj[property];
                if (typeof propValue === 'object') {
                    if (propValue === null) {
                        dump += "null;\n";
                    } else {
                        if (depth < maxDepth) {
                            dump += this.help_dump_object(propValue, visited, prefix, depth+1, maxDepth) + ";\n"
                        } else {
                            dump += "MAX_RECURSION_HIT;\n"
                        }
                    }
                } else if (propValue === undefined) {
                    dump += "undefined;\n";
                } else {
                    dump += propValue + ";\n";
                }
            } catch(e) {
                OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
            }
        }

        // Finish off the dump.
        dump += prefix+ "}";
        return dump;
    },

    OCTOPLIER_IS_AWESOME.dump_object = function(obj,maxDepth) {
        var visited = [];
        var depth = 0;
        return this.help_dump_object(obj,visited, "", depth, maxDepth);
    },

    // --------------------------------------------------------------------
    // Object Comparisons.
    // --------------------------------------------------------------------
    OCTOPLIER_IS_AWESOME.has_same_contents = function(obj1, obj2) {
        try {
            if (obj1 === null){
                if (obj2 === null){
                    return true;
                } else {
                    return false;
                }
            }

            if (obj2 === null) {
                if (obj1 === null) {
                    return true;
                } else {
                    return false;
                }
            }

            var keys1 = Object.keys(obj1);
            var keys2 = Object.keys(obj2);
            if (keys1.length !== keys2.length) {
                return false;
            }
            for (var i = 0; i<keys1.length; i++) {
                var k = keys1[i];
                if (!(k in obj2)) {
                    return false;
                }
                if (typeof obj1[k] === 'object') {
                    if ((obj1[k] != null) && (obj2[i] != null)) {
                        if (!this.has_same_contents(obj1[i], obj2[i])) {
                            return false;
                        }
                    }
                } else {
                    if (obj1[k] !== obj2[k]) {
                        return false;
                    }
                }
            }
            return true;
        } catch (e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    },

    // --------------------------------------------------------------------
    // Dom Element Utilities.
    // --------------------------------------------------------------------

    OCTOPLIER_IS_AWESOME.get_css_path = function(el) {
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
     }

    // ---------------------------------------------------------------------------------
    // The Browser Action class. (Used for actions like forward, back and reload.)
    // ---------------------------------------------------------------------------------

    OCTOPLIER_IS_AWESOME.BrowserAction = function(type) {
        // Create a new instance of our base.
        // Note because we declared our prototype here, any methods needs to be created
        // in this function as well.
        this.__proto__ = new Object();
        this.type = type;
    }

    // ---------------------------------------------------------------------------------
    // The Event Info class.
    // ---------------------------------------------------------------------------------

    OCTOPLIER_IS_AWESOME.EventInfo = function(event)  {
        try {
            // Create a new instance of our base.
            // Note because we declared our prototype here, any methods needs to be created
            // in this function as well.
            this.__proto__ = new Object();

            this.type = event.type;
            this.target_selector = OCTOPLIER_IS_AWESOME.get_css_path(event.target);
            this.value = event.target.value; // Note not all elements will have a value.

            // Create a sub object to hold initialization info.
            this.event_init = {};

            // Event initialization info.
            this.event_init.bubbles = event.bubbles;
            this.event_init.cancelable = event.cancelable;

        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    // ---------------------------------------------------------------------------------
    // The UIEvent Info class.
    // ---------------------------------------------------------------------------------

    OCTOPLIER_IS_AWESOME.UIEventInfo = function(event) {
        try {
            // Create a new instance of our base.
            // Note because we declared our prototype here, any methods needs to be created
            // in this function as well.
            this.__proto__ = new OCTOPLIER_IS_AWESOME.EventInfo(event);

            // UIEvent initialization info.
            this.event_init.detail = event.detail;
            this.event_init.view = null;// event.view; //null;
        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    // ---------------------------------------------------------------------------------
    // The MouseEvent Info class.
    // ---------------------------------------------------------------------------------

    OCTOPLIER_IS_AWESOME.MouseEventInfo = function (event) {
        try {
            // Create a new instance of our base.
            // Note because we declared our prototype here, any methods needs to be created
            // in this function as well.
            this.__proto__ = new OCTOPLIER_IS_AWESOME.UIEventInfo(event);

            // MouseEvent initialization info.
            this.event_init.screenX = event.screenX;
            this.event_init.screenY = event.screenY;
            this.event_init.clientX = event.clientX;
            this.event_init.clientY = event.clientY;
            this.event_init.ctrlKey = event.ctrlKey;
            this.event_init.shiftKey = event.shiftKey;
            this.event_init.altKey = event.altKey;
            this.event_init.metaKey = event.metaKey;
            this.event_init.button = event.button;
            this.event_init.relatedTarget = event.relatedTarget;
            this.event_init.buttons = event.buttons;

        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    // ---------------------------------------------------------------------------------
    // The KeyboardEvent Info class.
    // ---------------------------------------------------------------------------------

    OCTOPLIER_IS_AWESOME.KeyboardEventInfo = function(event) {
        try {
            // Create a new instance of our base.
            // Note because we declared our prototype here, any methods needs to be created
            // in this function as well.
            this.__proto__ = new OCTOPLIER_IS_AWESOME.UIEventInfo(event);

            // KeyboardEvent initialization info.
            this.event_init.key = event.key; //String.fromCharCode(event.keyCode);
            this.event_init.code = event.code; //String.fromCharCode(event.charCode);
            this.event_init.location = event.location;
            this.event_init.ctrlKey = event.ctrlKey;
            this.event_init.shiftKey = event.shiftKey;
            this.event_init.altKey = event.altKey;
            this.event_init.metaKey = event.metaKey;
            this.event_init.repeat = event.repeat;
            this.event_init.isComposing = event.isComposing;
            this.event_init.charCode = event.charCode;
            this.event_init.keyCode = event.keyCode;
            this.event_init.which = event.which;

        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    // ---------------------------------------------------------------------------------
    // The InputEvent Info class.
    // ---------------------------------------------------------------------------------

    OCTOPLIER_IS_AWESOME.InputEventInfo = function(event) {
        try {
            // Create a new instance of our base.
            // Note because we declared our prototype here, any methods needs to be created
            // in this function as well.
            this.__proto__ = new OCTOPLIER_IS_AWESOME.UIEventInfo(event);

            // InputEvent initialization info.
            this.event_init.isComposing = event.isComposing;

        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }


    // ---------------------------------------------------------------------------------
    // The Event Recorder class.
    // ---------------------------------------------------------------------------------

    OCTOPLIER_IS_AWESOME.EventRecorder = function() {
        try {
            this.window = null;
            this.blocked_infos = [];
            this.unblocked_infos = [];
        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    OCTOPLIER_IS_AWESOME.EventRecorder.prototype = new Object();

    OCTOPLIER_IS_AWESOME.EventRecorder.prototype.block = function(info) {
        try {
            this.blocked_infos.push(info);
        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    OCTOPLIER_IS_AWESOME.EventRecorder.prototype.get_num_blocked = function() {
        try {
            return this.blocked_infos.length;
        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    OCTOPLIER_IS_AWESOME.EventRecorder.prototype.get_num_unblocked = function() {
        try {
            return this.unblocked_infos.length;
        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    OCTOPLIER_IS_AWESOME.EventRecorder.prototype.unblock = function() {
        try {
            if (this.blocked_infos.length == 0) {
                return null;
            }

//            var debugQueues = " num blocked: " + OCTOPLIER_IS_AWESOME.g_event_recorder.get_num_blocked()
//                            + " num unblocked: " + OCTOPLIER_IS_AWESOME.g_event_recorder.get_num_unblocked();
//            OCTOPLIER_IS_AWESOME.g_debugger.log_error(debugQueues);

            var info = this.blocked_infos[0];
            switch(info.type) {
                // MouseEvent types: onclick, ondblclick, onmousedown, onmouseup, onmouseover, onmousemove, and onmouseout
            case "click":
            case "dblclick":
            case "mousedown":
            case "mouseup":
            case "mouseover":
            case "mousemove":
            case "mouseout": {
                event = new MouseEvent(info.type, info.event_init);

                this.blocked_infos.shift();
                this.unblocked_infos.push(info);

                //OCTOPLIER_IS_AWESOME.g_debugger.log_exception("dispatching event to selector: " + info.target_selector + "with type: " + event.type);

                var element = document.querySelector(info.target_selector);
                if (element) {
                    element.dispatchEvent(event);
                } else {
                    OCTOPLIER_IS_AWESOME.g_debugger.log_exception("could not find element to dispatch to");
                }
                break;
            }
            case "keydown":
            case "keypress":
            case "keyup": {
                var event = new KeyboardEvent(info.type, info.event_init);

                // Chromium Hack
                Object.defineProperty(event, 'keyCode', {
                                          get : function() {
                                              return this.keyCodeVal;
                                          }
                                      });
                Object.defineProperty(event, 'charCode', {
                                          get : function() {
                                              return this.charCodeVal;
                                          }
                                      });
                Object.defineProperty(event, 'which', {
                                          get : function() {
                                              return this.whichVal;
                                          }
                                      });
                Object.defineProperty(event, 'char', {
                                          get : function() {
                                              return this.charVal;
                                          }
                                      });
                Object.defineProperty(event, 'key', {
                                          get : function() {
                                              return this.keyVal;
                                          }
                                      });

                event.keyCodeVal = info.event_init.keyCode;
                event.charCodeVal = info.event_init.charCode;
                event.whichVal = info.event_init.which;
                event.charVal = info.event_init.char;
                event.keyVal = info.event_init.key;

                this.blocked_infos.shift();
                this.unblocked_infos.push(info);

                var element = document.querySelector(info.target_selector);
                if (element) {
                    element.dispatchEvent(event);
                } else {
                    OCTOPLIER_IS_AWESOME.g_debugger.log_exception("could not find element to dispatch to");
                }
                break;
            }
            case "input": {
                this.blocked_infos.shift();
                // no need to pass the info to the unblocked queue as we're going to handle it here.

                OCTOPLIER_IS_AWESOME.g_debugger.log_error("setting tiny change text on: " + info.target_selector + " with value: " + info.value);

                var element = document.querySelector(info.target_selector);
                if (element) {
                    element.value = info.value;
                } else {
                    OCTOPLIER_IS_AWESOME.g_debugger.log_exception("could not find element to dispatch to");
                }
                break;
            }
            case "change": {
                this.blocked_infos.shift();
                // no need to pass the info to the unblocked queue as we're going to handle it here.

                OCTOPLIER_IS_AWESOME.g_debugger.log_error("setting change text on: " + info.target_selector + " with value: " + info.value);

                var element = document.querySelector(info.target_selector);
                if (element) {
                    element.value = info.value;
                } else {
                    OCTOPLIER_IS_AWESOME.g_debugger.log_exception("could not find element to dispatch to");
                }
                break;
            }
            case "submit": {
                this.blocked_infos.shift();
                // no need to pass the info to the unblocked queue as we're going to handle it here.

                OCTOPLIER_IS_AWESOME.g_debugger.log_error("setting change text on: " + info.target_selector + " with value: " + info.value);

                var element = document.querySelector(info.target_selector);
                if (element) {
                    element.submit();
                } else {
                    OCTOPLIER_IS_AWESOME.g_debugger.log_exception("could not find element to dispatch to");
                }
                break;
            }
            }

            return info;
        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    OCTOPLIER_IS_AWESOME.EventRecorder.prototype.extract_info = function(event) {
        try {
            // Event types that we're interested in.
            // The properties are taken from the DOM Level 3 Events Specification.
            switch(event.type) {
                // MouseEvent types: onclick, ondblclick, onmousedown, onmouseup, onmouseover, onmousemove, and onmouseout
            case "click":
            case "dblclick":
            case "mousedown":
            case "mouseup":
            case "mouseover":
            case "mousemove":
            case "mouseout":
            case "change": {
                return new OCTOPLIER_IS_AWESOME.MouseEventInfo(event);
            }
            // KeyboardEvent types: keydown, keypress, keyup
            case "keydown":
            case "keypress":
            case "keyup": {
                return new OCTOPLIER_IS_AWESOME.KeyboardEventInfo(event);
            }
            // "input" type
            case "input": {
                return new OCTOPLIER_IS_AWESOME.InputEventInfo(event);
            }
            // "change" type
            case "change": {
                return new OCTOPLIER_IS_AWESOME.EventInfo(event);
            }
            // "submmit" type
            case "submit": {
                return new OCTOPLIER_IS_AWESOME.EventInfo(event);
            }

            }
            return null;
        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    OCTOPLIER_IS_AWESOME.EventRecorder.prototype.handle_event = function(event) {
        try {

            // If the event is one of our unblocked events we let it
            // continue on with it's regular capture, target and bubble phases.
            // Otherwise we block it, for dispatch at a later time.

            if (event.type === "beforeunload") {
                // If we still have blocked elements, we show the confirmation dialog
                // to delay navigating away from the current page.
                // That should be plenty of time for us to grab the unblocked infos.
                if (this.blocked_infos.length > 0) {
                    return "Are you sure you want to navigate away?";
                }
                return;
            }

            // Extract the info from the event.
            var info = this.extract_info(event);
            //OCTOPLIER_IS_AWESOME.g_debugger.log_exception("handle_event: " + dump_object(info,2));

            // The info will be null if it's an event we're not interested in tracking.
            if (info === null) {
                // We block this event by stopping its propagation.
                event.preventDefault();
                event.stopImmediatePropagation();
                OCTOPLIER_IS_AWESOME.g_debugger.log_exception("letting event through because it's null");
                return true;
            }

            // Check if this info matches one of our unblocked ones.
            for (var i = 0; i < this.get_num_unblocked(); i++) {
                if (OCTOPLIER_IS_AWESOME.has_same_contents(this.unblocked_infos[i],info)) {
                    this.unblocked_infos.splice(0, i+1);
                    //OCTOPLIER_IS_AWESOME.g_debugger.log_exception("letting event through with this many blocked"+this.get_num_unblocked());
                    // Return if this event is permitted to flow.
                    return true;
                }
            }

            // Otherwise we queue up this info as blocked.
            this.block(info);

            // We block this event by stopping its propagation.
            event.preventDefault();
            event.stopImmediatePropagation();

            //OCTOPLIER_IS_AWESOME.g_debugger.log_exception("blocking this event:" + dump_object(info.event_init,1));

            return false;
        } catch(e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    OCTOPLIER_IS_AWESOME.EventRecorder.prototype.connect_to_window = function() {
        try {
            if (typeof(window) === "undefined") {
                return;
            }

            if (typeof(window.document) === "undefined") {
                return;
            }

            if (this.window != window) {
                this.window = window;
                var wnd = this.window;

                // Page reloads.
                wnd.addEventListener('beforeonload', this, false);

                // MouseEvent types.
                wnd.addEventListener('click', this, false);
    //                wnd.addEventListener('dblclick', this, false);
    //                wnd.addEventListener('mousedown', this, false);
    //                wnd.addEventListener('mouseup', this, false);
    //                wnd.addEventListener('mouseover', this, false);
    //                wnd.addEventListener('mousemove', this, false);
    //                wnd.addEventListener('mouseout', this, false);
                wnd.addEventListener('change', this, false);

                // KeyboardEvent types.
    //                wnd.addEventListener('keydown', this, false);
    //                wnd.addEventListener('keypress', this, false);
    //                wnd.addEventListener('keyup', this, false);

    //                // "input" types. (Note "input" implements the InputEvent and Event interfaces.
    //                wnd.addEventListener('input', this, false);

                // "change" types. (Note "change" implements the Event interface.)
                // The change event is fired for <input>, <select>, and <textarea> elements when a change to the element's value is committed
                // by the user. Unlike the input event, the change event is not necessarily fired for each change to an element's value.
                wnd.addEventListener('change', this, false);

    //                // "select" types. (Note "select" implements the UIEvent interface if generated from a user interface, Event otherwise.
    //                wnd.addEventListener('select', this, false);

                // "submit" types. (Note "submit" implements the Event interface.)
                // Note that submit is fired only on the form element, not the button or submit input.
                wnd.addEventListener('submit', this, false);

            }
        } catch (e) {
            OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
        }
    }

    // Create our singleton event recorder if it doesn't exist.
    if (typeof(OCTOPLIER_IS_AWESOME.g_event_recorder) === "undefined") {
        OCTOPLIER_IS_AWESOME.g_event_recorder = new OCTOPLIER_IS_AWESOME.EventRecorder();
    }

    // Create our singleton debugger if it doesn't exist.
    if (typeof(OCTOPLIER_IS_AWESOME.g_debugger) === "undefined") {
        OCTOPLIER_IS_AWESOME.g_debugger = new OCTOPLIER_IS_AWESOME.Debugger();
    }

} catch (e) {
    OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
}
