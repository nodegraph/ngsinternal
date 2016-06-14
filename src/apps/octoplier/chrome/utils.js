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

