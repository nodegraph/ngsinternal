// This file runs in the browser.

var result = [];

if (typeof(window.octoplier) === "undefined") {
    result = [];
} else {
    try {
        result = window.octoplier.event_stack.get_events();
        //result = JSON.stringify(result);
    } catch(e) {
        window.octoplier.logger.log_exception(e);
    }
}

return result
