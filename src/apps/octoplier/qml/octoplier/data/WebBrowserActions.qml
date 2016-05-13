import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {}

    ListElement {
        navigator_title: "Browser Actions"
        image_url: "qrc:///icons/ic_public_white_48dp.png"
        title: "Google"
        description: "Perform a google search."
        script: "web_browser_page.web_view_alias.url = utils.url_from_input(\"www.google.com\");
                 menu_stack_page.visible = false"
    }

    ListElement {
        image_url: "qrc:///icons/ic_public_white_48dp.png"
        title: "Bing"
        description: "Perform a bing search."
        script: "web_browser_page.web_view_alias.url = utils.url_from_input(\"www.bing.com\");
                 menu_stack_page.visible = false"
    }

    ListElement {
        image_url: "qrc:///icons/ic_public_white_48dp.png"
        title: "Yahoo"
        description: "Perform a yahoo search."
        script: "web_browser_page.web_view_alias.url = utils.url_from_input(\"www.yahoo.com\");
                 menu_stack_page.visible = false"
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
        script: "web_browser_page.web_view_alias.goBack();
                 menu_stack_page.visible = false"
    }

    ListElement {
        image_url: "qrc:///icons/ic_arrow_forward_white_48dp.png"
        title: "Forward"
        description: "Undo the back operation."
        script: "web_browser_page.web_view_alias.goForward();
                 menu_stack_page.visible = false"
    }

    ListElement {
        image_url: "qrc:///icons/ic_refresh_white_48dp.png"
        title: "Refresh"
        description: "Refresh the browser."
        script: "if (web_browser_page.web_view_alias.loading) {
                    web_browser_page.web_view_alias.stop()
                }
                web_browser_page.web_view_alias.reload()
                menu_stack_page.visible = false"
    }

    ListElement {
        image_url: "qrc:///icons/ic_close_white_48dp.png"
        title: "Stop"
        description: "Stop the browser's current loading operation."
        script: "web_browser_page.web_view_alias.stop();
                 menu_stack_page.visible = false"
    }

}

