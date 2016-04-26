// This file is run in the browser.

var result="";

if (typeof(OCTOPLIER_IS_AWESOME) === "undefined") {
    result = "";
} else {
    try {
        // Grab any debug messages.
        result = OCTOPLIER_IS_AWESOME.g_debugger.get_errors();
        result = JSON.stringify(result);
        result += "favicon:" + OCTOPLIER_IS_AWESOME.get_favicon();
    } catch(e) {
        OCTOPLIER_IS_AWESOME.log_exception(e);
    }
}

result;



