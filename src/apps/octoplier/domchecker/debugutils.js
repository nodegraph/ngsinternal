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

