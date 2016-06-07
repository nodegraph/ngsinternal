// This file runs in the browser.

var result = [];

if (typeof(window.octoplier) === "undefined") {
    result = ['window.octoplier is undefined'];
} else {
    try {
        result = window.octoplier.logger.get_errors();
        //result = JSON.stringify(result);
    } catch(e) {
        window.octoplier.logger.log_exception(e);
    }
}

return result



