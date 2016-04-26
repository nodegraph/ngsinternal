// This file is run in the browser.

var result;

if (typeof(OCTOPLIER_IS_AWESOME) === "undefined") {
    result = "event recorder not initialized";
} else {
    try {
        // Make sure our recorder is properly connected to the window.
        OCTOPLIER_IS_AWESOME.g_event_recorder.connect_to_window();

        // Unblock one of our blocked events.
        result = OCTOPLIER_IS_AWESOME.g_event_recorder.unblock();
        result = JSON.stringify(result);

    } catch(e) {
        OCTOPLIER_IS_AWESOME.g_debugger.log_exception(e);
    }
}

result;

