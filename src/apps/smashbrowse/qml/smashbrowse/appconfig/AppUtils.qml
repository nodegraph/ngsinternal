import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0

import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4


QtObject {
    id: app_utils
    property var valid_license_callback
    property var invalid_license_callback
    property var nodejs_connected_callback

    // Public.
    function connect_to_nodejs(callback) {
        // Create connection.
        app_comm.nodejs_connected.connect(__connected_to_nodejs)
        // Cache callback.
        nodejs_connected_callback = callback
        // Start polling.
        app_comm.start_polling()
    }

    // Private.
    function __connected_to_nodejs() {
        // Destroy the connection.
        app_comm.nodejs_connected.disconnect(__connected_to_nodejs)
        // Call the cached callback.
        nodejs_connected_callback()
        // Clear callback cache.
        nodejs_connected_callback = null
    }

    // Public.
    function check_license(edition, license, success_callback, failure_callback) {
        // Connect to callback.
        app_comm.command_finished.connect(__on_license_validity)

        // Cache the callbacks.
        valid_license_callback = success_callback
        invalid_license_callback = failure_callback

        // Get nodejs to check the license for us.
        var request = {request: "check_license", edition: edition, license: license};
        var json_text = JSON.stringify(request);
        app_comm.handle_request_from_app(json_text)
    }

    // Private.
    function __on_license_validity(msg) {
        // Disconnect the callback.
        app_comm.command_finished.disconnect(__on_license_validity)

        // Examine the result message.
        var obj = JSON.parse(msg)
        if (obj.response == true && obj.value == "valid_license") {
            valid_license_callback()
        } else {
            invalid_license_callback()
        }

        // Clear callback cache.
        valid_license_callback = null
        invalid_license_callback = null
    }

    // Public.
    function load_last_graph() {
        // Load the last node graph.
        file_model.load_model()
        file_model.load_graph()
        node_graph_page.node_graph.update()
        // Frame everything.
        node_graph_page.node_graph.frame_all()
        node_graph_page.node_graph.update()
    }
}
