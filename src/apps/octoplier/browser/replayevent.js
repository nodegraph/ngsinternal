// This file runs in the browser.

var result = false

try {
	// Grab the events.
    result = octoplier.event_stack.get_events();
    result = JSON.stringify(result);
    r
} catch(e) {
    octoplier.logger.log_exception(e);
}

result;
