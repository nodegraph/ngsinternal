import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {}

    ListElement {
        navigator_title: "Browser Actions"

        image_url: "qrc:///icons/ic_public_white_48dp.png"
        title: "Open Indirect Browser"
        description: "The indirect browser eases node creation."
        script: 'var smash_browse_url = app_comm.get_smash_browse_url();
                 var request = {request: "open_browser", url: smash_browse_url};
                 var json_text = JSON.stringify(request);
                 app_comm.handle_request_from_app(json_text);
                 main_bar.on_switch_to_mode(app_settings.web_browser_mode)'
    }

    ListElement {
        image_url: "qrc:///icons/ic_public_white_48dp.png"
        title: "Close Indirect Browser"
        description: "Closes the indirect browser."
        script: 'var request = {request: "close_browser"};
                 var json_text = JSON.stringify(request);
                 app_comm.handle_request_from_app(json_text);
                 main_bar.on_switch_to_mode(app_settings.web_browser_mode)'
    }

    ListElement {
        image_url: "qrc:///icons/ic_public_white_48dp.png"
        title: "Go to URL"
        description: "Navigate to a specific URL."
        script: "menu_stack_page.on_url_entry_page()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_arrow_back_white_48dp.png"
        title: "Back"
        description: "Go back to previous page."
        script: 'var request = {request: "navigate_back"};
                 var json_text = JSON.stringify(request);
                 app_comm.handle_request_from_app(json_text);
                 main_bar.on_switch_to_mode(app_settings.web_browser_mode)'
    }

    ListElement {
        image_url: "qrc:///icons/ic_arrow_forward_white_48dp.png"
        title: "Forward"
        description: "Undo the back operation."
        script: 'var request = {request: "navigate_forward"};
                 var json_text = JSON.stringify(request);
                 app_comm.handle_request_from_app(json_text);
                 main_bar.on_switch_to_mode(app_settings.web_browser_mode)'
    }

    ListElement {
        image_url: "qrc:///icons/ic_refresh_white_48dp.png"
        title: "Refresh"
        description: "Refresh the browser."
        script: 'var request = {request: "navigate_refresh"};
                 var json_text = JSON.stringify(request);
                 app_comm.handle_request_from_app(json_text);
                 main_bar.on_switch_to_mode(app_settings.web_browser_mode)'
    }

}

