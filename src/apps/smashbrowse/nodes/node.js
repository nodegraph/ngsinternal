//This runs in on the qml/javascript side. (Not in the browser.)

var Node;

try {

	// Constructor for the Node class.
	Node = function() {
		// Any parameters for our compute operation should
		// be placed in our params map.
		this.params = {}
		// Our input and output data.
		this.inputs = {}
		this.outputs = {}
	}

	// Our input plug names.
	Node.prototype.get_js_input_plug_names = function() {
		return ["inputXXX"];
	}

	// Our output plug names.
	Node.prototype.get_js_output_plug_names = function() {
		return ["outputYYY"];
	}

	// Our parameters. (This is used for seialization from c++.)
	Node.prototype.get_js_params = function() {
		return this.params;
	}
	Node.prototype.set_js_params = function(params) {
		this.params = params;
		return true;
	}
	
	// This is called from the c++ side to set up the
	// input data before the compute method is called below.
	Node.prototype.set_js_input_data = function(inputPlugName, data) {
		if (inputPlugName in this.get_js_input_plug_names()) {
			this.inputs[inputPlugName] = data;
			return true;
		}
		return false;
	}

	// Get input data.
	Node.prototype.getInputData = function(inputPlugName) {
		if (inputPlugName in this.get_js_input_plug_names()) {
			return this.inputs[inputPlugName];
		}
		return {};
	}

	// Set output data.
	Node.prototype.setOutputData = function(outputPlugName, data) {
		if (outputPlugName in this.get_js_output_plug_names()) {
			this.outputs[outputPlugName] = data;
			return true;
		}
		return false;
	}

	// Perform compute.
	// The output data should be set with the setOutput method,
	// when the compute is finished at the end of this method.
	Node.prototype.perform_js_compute = function() {
		return true;
	}

} catch (e) {
	console.log("Error in Node definition: " + e);
}
