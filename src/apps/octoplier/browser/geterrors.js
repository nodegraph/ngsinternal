// This file runs in the browser.

var result = "";

if (typeof(octoplier) === "undefined") {
    result = "";
} else {
    try {
        result = octoplier.logger.get_errors();
        result = JSON.stringify(result);
    } catch(e) {
        octoplier.logger.log_exception(e);
    }
}

result;



