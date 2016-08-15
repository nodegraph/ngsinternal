//Catch any uncaught exceptions at the top level window.
window.onerror = function (message, url, line_number, column_number, error) {
    console.log('Error: caught at Window: ' + message + 
            ' Script: ' + url + 
            ' Line: ' + line_number + 
            ' Column: ' + column_number + 
            ' Error: ' + error);
}

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
//Debugging dom.
//------------------------------------------------------------------------------------------------

//Dump the xpaths of an array of elements.
function dump_array_of_elements(elements) {
  for (var i=0; i<elements.length; i++) {
      console.log("found: " + get_xpath(elements[i]))
  }
}

//Dump the client rect to the console.
function dump_client_rect(rect) {
    console.log("client rect: " + rect.left + "," + rect.right + "," + rect.top + "," + rect.bottom)
}