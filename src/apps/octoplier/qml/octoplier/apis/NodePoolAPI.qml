import QtQuick 2.4

import "/nodes/nodes.js" as Nodes

Item {
    objectName: "node_pool_api_object"
        
    // Create a node.
    function create_js_node(nodePath, computeType) {
        return Nodes.g_node_pool.create_js_node(nodePath, computeType)
    }

    // Destroy a node.
    function destroy_js_node(nodePath) {
        return Nodes.g_node_pool.destroy_js_node(nodePath)
    }
    
    function destroy_all_js_nodes() {
        return Nodes.g_node_pool.destroy_all_js_nodes();
    }

    // Get a node.
    function get_js_node(nodePath) {
        return Nodes.g_node_pool.get_js_node(nodePath)
    }

    // Get input plug names.
    function get_js_input_plug_names(nodePath) {
        var node = get_js_node(nodePath)
        if (!node) {
            return false
        }
        return node.get_js_input_plug_names()
    }

    // Get output plug names.
    function get_js_output_plug_names(nodePath) {
        var node = get_js_node(nodePath)
        if (!node) {
            console.log("xxxxxxxxxxxxxxxxxxxxxxxx")
            return false
        }
        return node.get_js_output_plug_names()
    }

    // Get params.
    function get_js_params(nodePath) {
        var node = get_js_node(nodePath)
        if (!node) {
            console.log("yyyyyyyyyyyyyyyyyyyyyyyyyyyy")
            return false // Note this is returning false instead of an empty object.
        }
        return node.get_js_params()
    }

    // Set params.
    function set_js_params(nodePath, params) {
        var node = get_js_node(nodePath)
        if (!node) {
            return false
        }
        return node.set_js_params(params)
    }

    // Set input data.
    function set_js_input_data(nodePath, inputPlugName, data) {
        var node = get_js_node(nodePath)
        if (!node) {
            return false
        }
        return node.set_js_input_data(inputPlugName, data)
    }

    // Get output data.
    function get_js_output_data(nodePath, outputPlugName) {
        var node = get_js_node(nodePath)
        if (!node) {
            return false
        }
        return node.get_js_output_data(outputPlugName)
    }

    // Compute.
    function perform_js_compute(nodePath) {
        var node = get_js_node(nodePath)
        if (!node) {
            return false
        }
        return node.perform_js_compute()
    }
}
